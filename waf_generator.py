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

import os
import tempfile

import builder

trace = False

data = { }

def _add_files(name, files):
    if type(files) is not list:
        files = [files]
    if name not in data:
        data[name] = []
    data[name] += files

class SourceFileFragmentComposer(builder.BuildSystemFragmentComposer):

    def __init__(self, cflags = "default"):
        self.cflags = cflags

    def compose(self, path):
        return ['sources', self.cflags], [path]

class TestFragementComposer(builder.BuildSystemFragmentComposer):

    def __init__(self, testName, fileFragments, runTest = True, netTest = False):
        self.testName = testName
        self.fileFragments = fileFragments
        self.runTest = runTest
        self.netTest = netTest

    def compose(self, path):
        return ['tests', self.testName], { 'files': self.fileFragments,
                                           'run': self.runTest,
                                           'net': self.netTest }

class KVMSymbolsFragmentComposer(builder.BuildSystemFragmentComposer):

    def compose(self, path):
        return ['KVMSymbols', 'files'], [path]

class RPCGENFragmentComposer(builder.BuildSystemFragmentComposer):

    def compose(self, path):
        return ['RPCGen', 'files'], [path]

class RouteKeywordsFragmentComposer(builder.BuildSystemFragmentComposer):

    def compose(self, path):
        return ['RouteKeywords', 'files'], [path]

class LexFragmentComposer(builder.BuildSystemFragmentComposer):

    def __init__(self, sym, dep):
        self.sym = sym
        self.dep = dep

    def compose(self, path):
        return ['lex', path], { 'file': path,
                                'sym': self.sym,
                                'dep': self.dep }

class YaccFragmentComposer(builder.BuildSystemFragmentComposer):

    def __init__(self, sym, header):
        self.sym = sym
        self.header = header

    def compose(self, path):
        return ['yacc', path], { 'file': path,
                                 'sym': self.sym,
                                 'header': self.header }

# Module Manager - Collection of Modules
class ModuleManager(builder.ModuleManager):

    def restart(self):
        self.script = ''

    def add(self, line = ''):
        self.script += line + os.linesep

    def write(self):
        try:
            out = tempfile.NamedTemporaryFile(delete = False)
            out.write(self.script)
            out.close()
            wscript = builder.RTEMS_DIR + '/wscript'
            builder.processIfDifferent(out.name, wscript, "wscript")
        finally:
            try:
                os.remove(out.name)
            except:
                pass

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

        def _source_list(lhs, files, append = False):
            if append:
                adder = '+'
                adder_space = ' '
            else:
                adder = ''
                adder_space = ''
            ll = len(lhs)
            if len(files) == 1:
                self.add('%s %s= [%r]' % (lhs, adder, files[0]))
            elif len(files) == 2:
                self.add('%s %s= [%r,' % (lhs, adder, files[0]))
                self.add('%s %s   %r]' % (' ' * ll, adder_space, files[-1]))
            elif len(files) > 0:
                self.add('%s %s= [%r,' % (lhs, adder, files[0]))
                for f in files[1:-1]:
                    self.add('%s %s   %r,' % (' ' * ll, adder_space, f))
                self.add('%s %s   %r]' % (' ' * ll, adder_space, files[-1]))

        def _data_insert(data, cpu, frag):
            #
            # The default handler returns an empty string. Skip it.
            #
            if type(frag) is not str:
                d = data
                for p in frag[0]:
                    if p not in d:
                        d[p] = {}
                    d = d[p]
                if type(frag[1]) is list:
                    if cpu not in d:
                        d[cpu] = []
                    d[cpu] += frag[1]
                else:
                    d[cpu] = frag[1]

        data = { }

        for mn in self.getModules():
            m = self[mn]
            if m.conditionalOn == "none":
                for f in m.files:
                    _data_insert(data, 'all', f.getFragment())
            for cpu, files in sorted(m.cpuDependentSourceFiles.items()):
                for f in files:
                    _data_insert(data, cpu, f.getFragment())

        if trace:
            import pprint
            pprint.pprint(data)

        self.restart()

        self.add('#')
        self.add('# Generated waf script.')
        self.add('#')
        self.add('')
        self.add('try:')
        self.add('    import rtems_waf.rtems as rtems')
        self.add('except:')
        self.add('    print "error: no rtems_waf git submodule; see README.waf"')
        self.add('    import sys')
        self.add('    sys.exit(1)')
        self.add('')
        self.add('def init(ctx):')
        self.add('    rtems.init(ctx)')
        self.add('')
        self.add('def options(opt):')
        self.add('    rtems.options(opt)')
        self.add('')
        self.add('def configure(conf):')
        self.add('    conf.find_program("lex", mandatory = True)')
        self.add('    conf.find_program("rpcgen", mandatory = True)')
        self.add('    conf.find_program("yacc", mandatory = True)')
        self.add('    rtems.configure(conf)')
        self.add('    if rtems.check_networking(conf):')
        self.add('        conf.fatal("RTEMS kernel contains the old network support; configure RTEMS with --disable-networking")')
        self.add('')
        self.add('def build(bld):')
        self.add('    rtems.build(bld)')
        self.add('')
        self.add('    # C/C++ flags')
        self.add('    common_flags = []')
        for f in builder.common_flags():
            self.add('    common_flags += ["%s"]' % (f))
        for f in builder.common_no_warnings():
            self.add('    common_flags += ["%s"]' % (f))
        self.add('    cflags = %r + common_flags' % (builder.cflags()))
        self.add('    cxxflags = %r + common_flags' % (builder.cxxflags()))
        self.add('')
        self.add('    # Include paths')
        self.add('    includes = []')
        for i in builder.includes():
            self.add('    includes += ["%s"]' % (i[2:]))
        self.add('    for i in %r:' % (builder.cpu_includes()))
        self.add('        includes += ["%s" % (i[2:].replace("@CPU@", bld.get_env()["RTEMS_ARCH"]))]')
        self.add('')
        self.add('    # Support dummy PIC IRQ includes')
        self.add('    if bld.get_env()["RTEMS_ARCH"] not in ("arm", "i386", "lm32", "mips", "powerpc", "sparc", "m68k"):')
        self.add('        includes += ["rtems-dummy-pic-irq/include"]')
        self.add('')

        self.add('    # Collect the libbsd uses')
        self.add('    libbsd_use = []')
        self.add('')

        #
        # Add the specific rule based builders for generating files.
        #
        if 'KVMSymbols' in data:
            kvmsymbols = data['KVMSymbols']
            self.add('    # KVM Symbols')
            self.add('    bld(target = "%s",' % (kvmsymbols['files']['all'][0]))
            self.add('        source = "rtemsbsd/rtems/generate_kvm_symbols",')
            self.add('        rule = "./${SRC} > ${TGT}")')
            self.add('    bld.objects(target = "kvmsymbols",')
            self.add('                features = "c",')
            self.add('                cflags = cflags,')
            self.add('                includes = includes,')
            self.add('                source = "%s")' % (kvmsymbols['files']['all'][0]))
            self.add('    libbsd_use += ["kvmsymbols"]')
            self.add('')

        if 'RPCGen' in data:
            rpcgen = data['RPCGen']
            rpcname = rpcgen['files']['all'][0][:-2]
            self.add('    # RPC Generation')
            self.add('    bld(target = "%s.h",' % (rpcname))
            self.add('        source = "%s.x",' % (rpcname))
            self.add('        rule = "${RPCGEN} -h -o ${TGT} ${SRC}")')
            self.add('')

        if 'RouteKeywords' in data:
            routekw = data['RouteKeywords']
            rkwname = routekw['files']['all'][0]
            self.add('    # Route keywords')
            self.add('    rkw_rule = "cat ${SRC} | ' + \
                     'awk \'BEGIN { r = 0 } { if (NF == 1) ' + \
                     'printf \\"#define\\\\tK_%%s\\\\t%%d\\\\n\\\\t{\\\\\\"%%s\\\\\\", K_%%s},\\\\n\\", ' + \
                     'toupper($1), ++r, $1, toupper($1)}\' > ${TGT}"')
            self.add('    bld(target = "%s.h",' % (rkwname))
            self.add('        source = "%s",' % (rkwname))
            self.add('        rule = rkw_rule)')
            self.add('')

        if 'lex' in data:
            lexes = data['lex']
            self.add('    # Lex')
            for l in lexes:
                lex = lexes[l]['all']
                self.add('    bld(target = "%s.c",' % (lex['file'][:-2]))
                self.add('        source = "%s",' % (lex['file']))
                self.add('        rule = "${LEX} -P %s -t ${SRC} | ' % (lex['sym']) + \
                         'sed -e \'/YY_BUF_SIZE/s/16384/1024/\' > ${TGT}")')
                self.add('    bld.objects(target = "lex_%s",' % (lex['sym']))
                self.add('                features = "c",')
                self.add('                cflags = cflags,')
                self.add('                includes = includes,')
                self.add('                source = "%s.c")' % (lex['file'][:-2]))
                self.add('    libbsd_use += ["lex_%s"]' % (lex['sym']))
                self.add('')

        if 'yacc' in data:
            yaccs = data['yacc']
            self.add('    # Yacc')
            for y in yaccs:
                yacc = yaccs[y]['all']
                yacc_file = yacc['file']
                yacc_sym = yacc['sym']
                yacc_header = '%s/%s' % (os.path.dirname(yacc_file), yacc['header'])
                self.add('    bld(target = "%s.c",' % (yacc_file[:-2]))
                self.add('        source = "%s",' % (yacc_file))
                self.add('        rule = "${YACC} -b %s -d -p %s ${SRC} && ' % (yacc_sym, yacc_sym) + \
                         'sed -e \'/YY_BUF_SIZE/s/16384/1024/\' < %s.tab.c > ${TGT} && ' % (yacc_sym) + \
                         'rm -f %s.tab.c && mv %s.tab.h %s")' % (yacc_sym, yacc_sym, yacc_header))
                self.add('    bld.objects(target = "yacc_%s",' % (yacc_sym))
                self.add('                features = "c",')
                self.add('                cflags = cflags,')
                self.add('                includes = includes,')
                self.add('                source = "%s.c")' % (yacc_file[:-2]))
                self.add('    libbsd_use += ["yacc_%s"]' % (yacc_sym))
            self.add('')

        #
        # We have 'm' different sets of flags and there can be 'n' cpus
        # specific files for those flags.
        #
        objs = 0
        self.add('    # Objects built with different CFLAGS')
        for cflags in sorted(data['sources']):
            if cflags is not 'default':
                objs += 1
                _source_list('    objs%02d_source' % objs, sorted(data['sources'][cflags]['all']))
                archs = sorted(data['sources'][cflags])
                for arch in archs:
                    if arch is not 'all':
                        self.add('    if bld.get_env()["RTEMS_ARCH"] == "%s":' % arch)
                        _source_list('        objs%02d_source' % objs,
                                     sorted(data['sources'][cflags][arch]),
                                     append = True)
                defines = [d[2:] for d in cflags.split(' ')]
                self.add('    bld.objects(target = "objs%02d",' % (objs))
                self.add('                features = "c",')
                self.add('                cflags = cflags,')
                self.add('                includes = includes,')
                self.add('                defines = %r,' % (defines))
                self.add('                source = objs%02d_source)' % objs)
                self.add('    libbsd_use += ["objs%02d"]' % (objs))
                self.add('')

        #
        # We hold the 'default' cflags set of files to the end to create the
        # static library with.
        #
        _source_list('    source', sorted(data['sources']['default']['all']))
        archs = sorted(data['sources']['default'])
        for arch in archs:
            if arch is not 'all':
                self.add('    if bld.get_env()["RTEMS_ARCH"] == "%s":' % arch)
                _source_list('        source',
                             sorted(data['sources']['default'][arch]),
                             append = True)
        self.add('    bld.stlib(target = "bsd",')
        self.add('              features = "c cxx",')
        self.add('              cflags = cflags,')
        self.add('              cxxflags = cxxflags,')
        self.add('              includes = includes,')
        self.add('              source = source,')
        self.add('              use = libbsd_use)')
        self.add('')

        self.add('    # Tests')
        tests = data['tests']
        for test_name in tests:
            files = ['testsuite/%s/%s.c' % (test_name, f) for f in  data['tests'][test_name]['all']['files']]
            _source_list('    test_%s' % (test_name), sorted(files))
            self.add('    bld.program(target = "%s",' % (test_name))
            self.add('                features = "cprogram",')
            self.add('                cflags = cflags,')
            self.add('                includes = includes,')
            self.add('                source = test_%s,' % (test_name))
            self.add('                use = ["bsd"],')
            self.add('                lib = ["m", "z"])')
            self.add('')

        self.write()
