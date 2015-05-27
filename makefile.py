#
#  Copyright (c) 2015 Chris Johns <chrisj@rtems.org>. All rights reserved.
#
#  Copyright (c) 2009-2015 embedded brains GmbH.  All rights reserved.
#
#   embedded brains GmbH
#   Dornierstr. 4
#   82178 Puchheim
#   Germany
#   <info@embedded-brains.de>
#
#  Copyright (c) 2012 OAR Corporation. All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import tempfile

import builder

class SourceFileFragmentComposer(builder.BuildSystemFragmentComposer):

    def __init__(self, cflags = None):
        self.cflags = cflags

    def compose(self, path):
        fragment = 'LIB_C_FILES += ' + path + '\n'
        if self.cflags != None:
            fragment = fragment + path[:-1] + 'o: ' + path + '\n' \
                       + '\t$(CC) $(CPPFLAGS) $(CFLAGS) ' + self.cflags + ' -c $< -o $@\n'
        return fragment

class TestFragementComposer(builder.BuildSystemFragmentComposer):

    def __init__(self, testName, fileFragments, runTest = True, netTest = False):
        self.testName = testName
        self.fileFragments = fileFragments
        self.runTest = runTest
        self.netTest = netTest

    def compose(self, path):
        makefileFragment = ''
        testPrefix = 'TEST_' + self.testName.upper()
        testOFiles = testPrefix + '_O_FILES'
        testDFiles = testPrefix + '_D_FILES'
        testDir = 'testsuite/' + self.testName
        testExe = testDir + '/' + self.testName + '.exe'
        testMap = testDir + '/' + self.testName + '.map'
        testCollection = 'TESTS'
        if self.netTest:
            testCollection = 'NET_' + testCollection
        makefileFragment += '\n' + testPrefix + ' = ' + testExe + '\n' \
                           + testOFiles + ' =\n' \
                           + testDFiles + ' =\n'
        for fileFragment in self.fileFragments:
            makefileFragment += testOFiles + ' += ' + testDir + '/' + fileFragment + '.o\n' \
                               + testDFiles + ' += ' + testDir + '/' + fileFragment + '.d\n'
        makefileFragment += '$(' + testPrefix + '): $(' + testOFiles + ') $(LIB)\n' \
                            '\t$(LINK.c) -Wl,-Map,' + testMap + ' $^ -lm -lz -o $@\n' \
                            + testCollection + ' += $(' + testPrefix + ')\n' \
                            'O_FILES += $(' + testOFiles + ')\n' \
                            'D_FILES += $(' + testDFiles + ')\n'
        if self.runTest:
            makefileFragment += 'RUN_' + testCollection + ' += $(' + testPrefix + ')\n'
        return makefileFragment

class KVMSymbolsFragmentComposer(builder.BuildSystemFragmentComposer):

    def compose(self, path):
        return 'LIB_GEN_FILES += ' + path + '\n' \
            'LIB_C_FILES += ' + path + '\n' \
            + path + ': rtemsbsd/rtems/generate_kvm_symbols\n' \
            '\t./$< > $@\n'

class RPCGENFragmentComposer(builder.BuildSystemFragmentComposer):

    def compose(self, path):
        headerPath = path[:-2] + '.h'
        return headerPath + ': ' + path + '\n' \
            '\t	rm -f $@\n' \
            '\t	rpcgen -h -o $@ $<\n'

class RouteKeywordsFragmentComposer(builder.BuildSystemFragmentComposer):

    def compose(self, path):
        headerPath = path + '.h'
        tmpPath = path + '.tmp'
        return headerPath + ': ' + path + '\n' \
            '\tsed -e \'/^#/d\' -e \'/^$$/d\' $< > ' + tmpPath + '\n' \
            '\tLC_ALL=C tr \'a-z\' \'A-Z\' < ' + tmpPath + ' | paste ' + tmpPath + ' - | \\\n' \
            '\tawk \'{ if (NF > 1) printf "#define\\tK_%s\\t%d\\n\\t{\\"%s\\", K_%s},\\n", $$2, NR, $$1, $$2 }\' > $@\n' \
            '\trm -f ' + tmpPath + '\n'

class LexFragmentComposer(builder.BuildSystemFragmentComposer):

    def __init__(self, sym, dep):
        self.sym = sym
        self.dep = dep

    def compose(self, path):
        src = path[:-2] + '.c'
        dep = path[:path.rfind('/')] + '/' + self.dep
        return 'LIB_C_FILES += ' + src + '\n' \
            + src + ': ' + path + ' ' + dep + '\n' \
            '\t${LEX} -P ' + self.sym + ' -t $< | sed -e \'/YY_BUF_SIZE/s/16384/1024/\' > $@\n'

class YaccFragmentComposer(builder.BuildSystemFragmentComposer):

    def __init__(self, sym, header):
        self.sym = sym
        self.header = header

    def compose(self, path):
        src = path[:-2] + '.c'
        hdr = path[:path.rfind('/')] + '/' + self.header
        return 'LIB_C_FILES += ' + src + '\n' \
            + src + ': ' + path + '\n' \
            '\tyacc -b ' + self.sym + ' -d -p ' + self.sym + ' $<\n' \
            '\tsed -e ''/YY_BUF_SIZE/s/16384/1024/'' < ' + self.sym + '.tab.c > $@\n' \
            '\trm -f ' + self.sym + '.tab.c\n' \
            '\tmv ' + self.sym + '.tab.h ' + hdr + '\n'

# Module Manager - Collection of Modules
class ModuleManager(builder.ModuleManager):

    def setGenerators(self):
        self.generator['convert'] = builder.Converter
        self.generator['no-convert'] = builder.NoConverter

        self.generator['file'] = builder.File

        self.generator['path'] = builder.PathComposer
        self.generator['freebsd-path'] = builder.FreeBSDPathComposer
        self.generator['rtems-path'] = builder.RTEMSPathComposer
        self.generator['cpu-path'] = builder.CPUDependentPathComposer
        self.generator['target-src-cpu--path'] = builder.TargetSourceCPUDependentPathComposer

        self.generator['source'] = SourceFileFragmentComposer
        self.generator['test'] = TestFragementComposer
        self.generator['kvm-symbols'] = KVMSymbolsFragmentComposer
        self.generator['rpc-gen'] = RPCGENFragmentComposer
        self.generator['route-keywords'] = RouteKeywordsFragmentComposer
        self.generator['lex'] = LexFragmentComposer
        self.generator['yacc'] = YaccFragmentComposer

    def generate(self):
        data = 'include config.inc\n' \
               '\n' \
               'RTEMS_MAKEFILE_PATH = $(PREFIX)/$(TARGET)/$(BSP)\n' \
               'include $(RTEMS_MAKEFILE_PATH)/Makefile.inc\n' \
               'include $(RTEMS_CUSTOM)\n' \
               'include $(PROJECT_ROOT)/make/leaf.cfg\n' \
               '\n' \
               'COMMON_FLAGS += -fno-strict-aliasing\n' \
               'COMMON_FLAGS += -ffreestanding\n' \
               'COMMON_FLAGS += -fno-common\n' \
               'COMMON_FLAGS += -Irtemsbsd/include\n' \
               'COMMON_FLAGS += -Irtemsbsd/$(RTEMS_CPU)/include\n' \
               'COMMON_FLAGS += -Ifreebsd/sys\n' \
               'COMMON_FLAGS += -Ifreebsd/sys/$(RTEMS_CPU)/include\n' \
               'COMMON_FLAGS += -Ifreebsd/sys/contrib/altq\n' \
               'COMMON_FLAGS += -Ifreebsd/sys/contrib/pf\n' \
               'COMMON_FLAGS += -Ifreebsd/include\n' \
               'COMMON_FLAGS += -Ifreebsd/lib/libc/include\n' \
               'COMMON_FLAGS += -Ifreebsd/lib/libc/isc/include\n' \
               'COMMON_FLAGS += -Ifreebsd/lib/libc/resolv\n' \
               'COMMON_FLAGS += -Ifreebsd/lib/libutil\n' \
               'COMMON_FLAGS += -Ifreebsd/lib/libkvm\n' \
               'COMMON_FLAGS += -Ifreebsd/lib/libmemstat\n' \
               'COMMON_FLAGS += -Ifreebsd/lib/libipsec\n' \
               'COMMON_FLAGS += -Irtemsbsd/sys\n' \
               'COMMON_FLAGS += -ImDNSResponder/mDNSCore\n' \
               'COMMON_FLAGS += -ImDNSResponder/mDNSShared\n' \
               'COMMON_FLAGS += -ImDNSResponder/mDNSPosix\n' \
               'COMMON_FLAGS += -Itestsuite/include\n' \
               'COMMON_FLAGS += -DHAVE_RTEMS_PCI_H=1\n' \
               'COMMON_FLAGS += -Wall\n' \
               'COMMON_FLAGS += -Wno-format\n' \
               'COMMON_FLAGS += -MT $@ -MD -MP -MF $(basename $@).d\n' \
               'CFLAGS += $(COMMON_FLAGS)\n' \
               'CFLAGS += -std=gnu11\n' \
               'CXXFLAGS += $(COMMON_FLAGS)\n' \
               'CXXFLAGS += -std=gnu++11\n' \
               'NEED_DUMMY_PIC_IRQ=yes\n' \
               '\n' \
               'TEST_NETWORK_CONFIG = testsuite/include/rtems/bsd/test/network-config.h\n' \
               '\n' \
               'TESTS =\n' \
               'RUN_TESTS =\n' \
               '\n' \
               'NET_TESTS =\n' \
               'RUN_NET_TESTS =\n' \
               '\n' \
               'O_FILES =\n' \
               'D_FILES =\n' \
               '\n' \
               'LIB = libbsd.a\n' \
               'LIB_GEN_FILES =\n' \
               'LIB_C_FILES =\n' \
               'LIB_CXX_FILES =\n' \
               'LIB_CXX_FILES += rtemsbsd/rtems/rtems-bsd-cxx.cc\n'
        for mn in self.getModules():
            m = self[mn]
            if m.conditionalOn != "none":
                data += 'ifneq ($(' + m.conditionalOn + '),yes)\n'
            for f in m.files:
                data += f.getFragment()
            for cpu, files in sorted(m.cpuDependentSourceFiles.items()):
                data += 'ifeq ($(RTEMS_CPU), ' + cpu + ')\n'
                for f in files:
                    data += f.getFragment()
                if cpu in ("arm", "i386", "lm32", "mips", "powerpc", "sparc", "m68k"):
                    data += 'NEED_DUMMY_PIC_IRQ=no\n'
                data += 'endif\n'
                if m.conditionalOn != "none":
                    data += 'endif # ' + m.conditionalOn +'\n'
        data += '\n' \
                'ifeq ($(NEED_DUMMY_PIC_IRQ),yes)\n' \
                'CFLAGS += -I rtems-dummy-pic-irq/include\n' \
                'endif\n' \
                'LIB_O_FILES = $(LIB_C_FILES:%.c=%.o) $(LIB_CXX_FILES:%.cc=%.o)\n' \
                'O_FILES += $(LIB_O_FILES)\n' \
                'D_FILES += $(LIB_C_FILES:%.c=%.d) $(LIB_CXX_FILES:%.cc=%.d)\n' \
                '\n' \
                'all: $(LIB) $(TESTS) $(TEST_NETWORK_CONFIG) $(NET_TESTS)\n' \
                '\n' \
                '$(LIB): $(LIB_GEN_FILES) $(LIB_O_FILES)\n' \
                '\trm -f $@\n' \
                '\t$(AR) rcu $@ $^\n' \
                '\n' \
                'run_tests: $(RUN_TESTS)\n' \
                '\t$(TEST_RUNNER) $^\n' \
                '\tcheck_endof\n' \
                '\n' \
                'run_net_tests: $(RUN_NET_TESTS)\n' \
                '\t$(TEST_RUNNER) -N -T $(NET_TAP_INTERFACE) $^\n' \
                '\tcheck_endof\n' \
                '\n' \
                '$(TEST_NETWORK_CONFIG): $(TEST_NETWORK_CONFIG).in config.inc\n' \
                '\tsed -e \'s/@NET_CFG_SELF_IP@/$(NET_CFG_SELF_IP)/\' \\\n' \
                '\t-e \'s/@NET_CFG_NETMASK@/$(NET_CFG_NETMASK)/\' \\\n' \
                '\t-e \'s/@NET_CFG_PEER_IP@/$(NET_CFG_PEER_IP)/\' \\\n' \
                '\t-e \'s/@NET_CFG_GATEWAY_IP@/$(NET_CFG_GATEWAY_IP)/\' \\\n' \
                '\t< $< > $@\n' \
                '\n' \
                'CPU_SED  = sed\n' \
                'CPU_SED += -e \'/arm/d\'\n' \
                'CPU_SED += -e \'/i386/d\'\n' \
                'CPU_SED += -e \'/powerpc/d\'\n' \
                'CPU_SED += -e \'/mips/d\'\n' \
                'CPU_SED += -e \'/sparc64/d\'\n' \
                '\n' \
                'LIB_DIR = $(INSTALL_BASE)/lib\n' \
                'INCLUDE_DIR = $(INSTALL_BASE)/lib/include\n' \
                '\n' \
                'install: $(LIB)\n' \
                '\tinstall -d $(LIB_DIR)\n' \
                '\tinstall -m 644 $(LIB) $(LIB_DIR)\n' \
                '\tcd rtemsbsd/include ; for i in `find . -type d` ; do \\\n' \
                '\t  install -d $(INCLUDE_DIR)/$$i ; \\\n' \
                '\t  install -m 644 $$i/*.h $(INCLUDE_DIR)/$$i ; done\n' \
                '\tinstall -d $(INCLUDE_DIR)/mghttpd\n' \
                '\tinstall -m 644 rtemsbsd/mghttpd/mongoose.h $(INCLUDE_DIR)/mghttpd\n' \
                '\tcd freebsd/include ; for i in `find . -type d` ; do \\\n' \
                '\t  install -d $(INCLUDE_DIR)/$$i ; \\\n' \
                '\t  install -m 644 $$i/*.h $(INCLUDE_DIR)/$$i ; done\n' \
                '\tcd freebsd/sys/contrib/altq ; \\\n' \
                '\t  install -d $(INCLUDE_DIR)/altq ; \\\n' \
                '\t  install -m 644 altq/*.h $(INCLUDE_DIR)/altq\n' \
                '\tfor i in bsm cam net net80211 netatalk netinet netinet6 netipsec sys vm ; do \\\n' \
                '\t  install -d $(INCLUDE_DIR)/$$i ; \\\n' \
                '\t  install -m 644 freebsd/sys/$$i/*.h $(INCLUDE_DIR)/$$i ; done\n' \
                '\tcd freebsd/sys/dev/mii ; \\\n' \
                '\t  install -d $(INCLUDE_DIR)/dev/mii ; \\\n' \
                '\t  install -m 644 *.h $(INCLUDE_DIR)/dev/mii\n' \
                '\tinstall -m 644 mDNSResponder/mDNSCore/mDNSDebug.h $(INCLUDE_DIR)\n' \
                '\tinstall -m 644 mDNSResponder/mDNSCore/mDNSEmbeddedAPI.h $(INCLUDE_DIR)\n' \
                '\tinstall -m 644 mDNSResponder/mDNSShared/dns_sd.h $(INCLUDE_DIR)\n' \
                '\tinstall -m 644 mDNSResponder/mDNSPosix/mDNSPosix.h $(INCLUDE_DIR)\n' \
                '\n' \
                'clean:\n' \
                '\trm -f $(LIB_GEN_FILES) $(LIB) $(TESTS) $(O_FILES) $(D_FILES)\n' \
                '\trm -f libbsd.html\n' \
                '\n' \
                '-include $(D_FILES)\n' \
                '\n' \
                'doc: libbsd.html\n' \
                '\n' \
                'libbsd.html: libbsd.txt\n' \
                '\tasciidoc -o libbsd.html libbsd.txt\n'
        try:
            out = tempfile.NamedTemporaryFile(delete = False)
            out.write(data)
            out.close()
            makefile = builder.RTEMS_DIR + '/Makefile'
            builder.processIfDifferent(out.name, makefile, "Makefile")
        finally:
            try:
                os.remove(out.name)
            except:
                pass
