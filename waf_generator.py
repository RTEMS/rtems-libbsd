#
#  Copyright (c) 2015-2016 Chris Johns <chrisj@rtems.org>. All rights reserved.
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

from __future__ import print_function

import os
import sys
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

def _clfags_includes(cflags, includes):
    if type(cflags) is not list:
        if cflags is not None:
            _cflags = cflags.split(' ')
        else:
            _cflags = [None]
    else:
        _cflags = cflags
    if type(includes) is not list:
        _includes = [includes]
    else:
        _includes = includes
    return _cflags, _includes

class SourceFileFragmentComposer(builder.BuildSystemFragmentComposer):

    def __init__(self, cflags = "default", includes = None):
        self.cflags, self.includes = _clfags_includes(cflags, includes)

    def compose(self, path):
        if None in self.includes:
            return ['sources', self.cflags], [path], self.cflags, self.includes
        return ['sources', self.cflags + self.includes], [path], self.cflags, self.includes

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
        return ['KVMSymbols', 'files'], [path], self.includes

class RPCGENFragmentComposer(builder.BuildSystemFragmentComposer):

    def compose(self, path):
        return ['RPCGen', 'files'], [path]

class RouteKeywordsFragmentComposer(builder.BuildSystemFragmentComposer):

    def compose(self, path):
        return ['RouteKeywords', 'files'], [path]

class LexFragmentComposer(builder.BuildSystemFragmentComposer):

    def __init__(self, sym, dep, cflags = None, includes = None):
        self.sym = sym
        self.dep = dep
        self.cflags, self.includes = _clfags_includes(cflags, includes)

    def compose(self, path):
        d = { 'file': path,
              'sym': self.sym,
              'dep': self.dep }
        if None not in self.cflags:
            d['cflags'] = self.cflags
        if None not in self.includes:
            d['includes'] = self.includes
        return ['lex', path], d

class YaccFragmentComposer(builder.BuildSystemFragmentComposer):

    def __init__(self, sym, header, cflags = None, includes = None):
        self.sym = sym
        self.header = header
        self.cflags, self.includes = _clfags_includes(cflags, includes)

    def compose(self, path):
        d = { 'file': path,
              'sym': self.sym,
              'header': self.header }
        if None not in self.cflags:
            d['cflags'] = self.cflags
        if None not in self.includes:
            d['includes'] = self.includes
        return ['yacc', path], d

# Module Manager - Collection of Modules
class ModuleManager(builder.ModuleManager):

    def restart(self):
        self.script = ''

    def add(self, line = ''):
        self.script += line + os.linesep

    def write(self):
        try:
            out = tempfile.NamedTemporaryFile(delete = False)
            try:
                out.write(bytes(self.script, sys.stdin.encoding))
            except:
                out.write(self.script)
            out.close()
            wscript = builder.RTEMS_DIR + '/libbsd_waf.py'
            builder.processIfDifferent(out.name, wscript, "libbsd_waf.py")
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

    def generate(self, rtems_version):

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
                # Start at the top of the tree
                d = data
                path = frag[0]
                if path[0] not in d:
                    d[path[0]] = {}
                # Select the sub-part of the tree as the compile options
                # specialise how files are built.
                d = d[path[0]]
                if type(path[1]) is list:
                    p = ' '.join(path[1])
                else:
                    p = path[1]
                if p not in d:
                    d[p] = {}
                d = d[p]
                if cpu not in d:
                    d[cpu] = []
                if type(frag[1]) is list:
                    d[cpu] += frag[1]
                else:
                    d[cpu] = frag[1]
                if len(frag) > 3:
                    if 'cflags' not in d[cpu]:
                        d['cflags'] = []
                    d['cflags'] += frag[2]
                if len(frag) >= 3 and None not in frag[-1]:
                    if 'includes' not in d[cpu]:
                        d['includes'] = []
                    d['includes'] += frag[-1]

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
        self.add('# RTEMS Project (https://www.rtems.org)')
        self.add('#')
        self.add('# Generated waf script. Do not edit, run ./freebsd-to-rtems.py -m')
        self.add('#')
        self.add('# To use see README.waf shipped with this file.')
        self.add('#')
        self.add('')
        self.add('from __future__ import print_function')
        self.add('')
        self.add('import os.path')
        self.add('import rtems_waf.rtems as rtems')
        self.add('')
        self.add('def init(ctx):')
        self.add('    pass')
        self.add('')
        self.add('def options(opt):')
        self.add('    pass')
        self.add('')
        self.add('def bsp_configure(conf, arch_bsp):')
        self.add('    pass')
        self.add('')
        self.add('def configure(conf):')
        self.add('    pass')
        self.add('')
        self.add('def build(bld):')
        self.add('    # C/C++ flags')
        self.add('    common_flags = []')
        for f in builder.common_flags():
            self.add('    common_flags += ["%s"]' % (f))
        self.add('    if bld.env.WARNINGS:')
        for f in builder.common_warnings():
            self.add('        common_flags += ["%s"]' % (f))
        self.add('    else:')
        for f in builder.common_no_warnings():
            self.add('        common_flags += ["%s"]' % (f))
        self.add('    cflags = %r + common_flags' % (builder.cflags()))
        self.add('    cxxflags = %r + common_flags' % (builder.cxxflags()))
        self.add('')
        self.add('    # Defines')
        self.add('    defines = []')
        self.add('    if len(bld.env.FREEBSD_OPTIONS) > 0:')
        self.add('        for o in bld.env.FREEBSD_OPTIONS.split(","):')
        self.add('            defines += ["%s=1" % (o.strip().upper())]')
        self.add('')
        self.add('    # Include paths')
        self.add('    includes = []')
        self.add('    for i in %r:' % (builder.cpu_includes()))
        self.add('        includes += ["%s" % (i[2:].replace("@CPU@", bld.get_env()["RTEMS_ARCH"]))]')
        self.add('    if bld.get_env()["RTEMS_ARCH"] == "i386":')
        self.add('        for i in %r:' % (builder.cpu_includes()))
        self.add('            includes += ["%s" % (i[2:].replace("@CPU@", "x86"))]')
        for i in builder.includes():
            self.add('    includes += ["%s"]' % (i[2:]))
        self.add('')
        self.add('    # Support dummy PIC IRQ includes')
        self.add('    if bld.get_env()["RTEMS_ARCH"] not in ("arm", "i386", "lm32", "mips", "powerpc", "sparc", "m68k"):')
        self.add('        includes += ["rtems-dummy-pic-irq/include"]')
        self.add('')

        self.add('    # Collect the libbsd uses')
        self.add('    libbsd_use = []')
        self.add('')

        #
        # Support the existing Makefile based network configuration file.
        #
        self.add('    # Network test configuration')
        self.add('    if not os.path.exists(bld.env.NET_CONFIG):')
        self.add('        bld.fatal("network configuraiton \'%s\' not found" % (bld.env.NET_CONFIG))')
        self.add('    net_cfg_self_ip = None')
        self.add('    net_cfg_netmask = None')
        self.add('    net_cfg_peer_ip = None')
        self.add('    net_cfg_gateway_ip = None')
        self.add('    net_tap_interface = None')
        self.add('    try:')
        self.add('        net_cfg_lines = open(bld.env.NET_CONFIG).readlines()')
        self.add('    except:')
        self.add('        bld.fatal("network configuraiton \'%s\' read failed" % (bld.env.NET_CONFIG))')
        self.add('    lc = 0')
        self.add('    for l in net_cfg_lines:')
        self.add('        lc += 1')
        self.add('        if l.strip().startswith("NET_CFG_"):')
        self.add('            ls = l.split("=")')
        self.add('            if len(ls) != 2:')
        self.add('                bld.fatal("network configuraiton \'%s\' parse error: %d: %s" % ' + \
                 '(bld.env.NET_CONFIG, lc, l))')
        self.add('            lhs = ls[0].strip()')
        self.add('            rhs = ls[1].strip()')
        self.add('            if lhs == "NET_CFG_SELF_IP":')
        self.add('                net_cfg_self_ip = rhs')
        self.add('            if lhs == "NET_CFG_NETMASK":')
        self.add('                net_cfg_netmask = rhs')
        self.add('            if lhs == "NET_CFG_PEER_IP":')
        self.add('                net_cfg_peer_ip = rhs')
        self.add('            if lhs == "NET_CFG_GATEWAY_IP":')
        self.add('                net_cfg_gateway_ip = rhs')
        self.add('            if lhs == "NET_TAP_INTERFACE":')
        self.add('                net_tap_interface = rhs')
        self.add('    bld(target = "testsuite/include/rtems/bsd/test/network-config.h",')
        self.add('        source = "testsuite/include/rtems/bsd/test/network-config.h.in",')
        self.add('        rule = "sed -e \'s/@NET_CFG_SELF_IP@/%s/\' ' + \
                 '-e \'s/@NET_CFG_NETMASK@/%s/\' ' + \
                 '-e \'s/@NET_CFG_PEER_IP@/%s/\' ' + \
                 '-e \'s/@NET_CFG_GATEWAY_IP@/%s/\' < ${SRC} > ${TGT}" % ' + \
                 '(net_cfg_self_ip, net_cfg_netmask, net_cfg_peer_ip, net_cfg_gateway_ip),')
        self.add('        update_outputs = True)')
        self.add('')

        #
        # Add the specific rule based builders for generating files.
        #
        if 'KVMSymbols' in data:
            kvmsymbols = data['KVMSymbols']
            if 'includes' in kvmsymbols['files']:
                includes = kvmsymbols['files']['includes']
            else:
                includes = []
            self.add('    # KVM Symbols')
            self.add('    bld(target = "%s",' % (kvmsymbols['files']['all'][0]))
            self.add('        source = "rtemsbsd/rtems/generate_kvm_symbols",')
            self.add('        rule = "./${SRC} > ${TGT}",')
            self.add('        update_outputs = True)')
            self.add('    bld.objects(target = "kvmsymbols",')
            self.add('                features = "c",')
            self.add('                cflags = cflags,')
            self.add('                includes = %r + includes,' % (includes))
            self.add('                source = "%s")' % (kvmsymbols['files']['all'][0]))
            self.add('    libbsd_use += ["kvmsymbols"]')
            self.add('')

        self.add('    bld.add_group()')

        if 'RPCGen' in data:
            rpcgen = data['RPCGen']
            rpcname = rpcgen['files']['all'][0][:-2]
            self.add('    # RPC Generation')
            self.add('    if bld.env.AUTO_REGEN:')
            self.add('        bld(target = "%s.h",' % (rpcname))
            self.add('            source = "%s.x",' % (rpcname))
            self.add('            rule = "${RPCGEN} -h -o ${TGT} ${SRC}")')
            self.add('')

        if 'RouteKeywords' in data:
            routekw = data['RouteKeywords']
            rkwname = routekw['files']['all'][0]
            self.add('    # Route keywords')
            self.add('    if bld.env.AUTO_REGEN:')
            self.add('        rkw_rule = "cat ${SRC} | ' + \
                     'awk \'BEGIN { r = 0 } { if (NF == 1) ' + \
                     'printf \\"#define\\\\tK_%%s\\\\t%%d\\\\n\\\\t{\\\\\\"%%s\\\\\\", K_%%s},\\\\n\\", ' + \
                     'toupper($1), ++r, $1, toupper($1)}\' > ${TGT}"')
            self.add('        bld(target = "%s.h",' % (rkwname))
            self.add('            source = "%s",' % (rkwname))
            self.add('            rule = rkw_rule)')
            self.add('')

        if 'lex' in data:
            lexes = data['lex']
            self.add('    # Lex')
            for l in sorted(lexes.keys()):
                lex = lexes[l]['all']
                if 'cflags' in lex:
                    lex_defines = [d[2:] for d in lex['cflags']]
                else:
                    lex_defines = []
                if 'includes' in lex:
                    lex_includes = lex['includes']
                else:
                    lex_includes = []
                self.add('    if bld.env.AUTO_REGEN:')
                self.add('        bld(target = "%s.c",' % (lex['file'][:-2]))
                self.add('            source = "%s",' % (lex['file']))
                self.add('            rule = "${LEX} -P %s -t ${SRC} | ' % (lex['sym']) + \
                         'sed -e \'/YY_BUF_SIZE/s/16384/1024/\' > ${TGT}")')
                self.add('    bld.objects(target = "lex_%s",' % (lex['sym']))
                self.add('                features = "c",')
                self.add('                cflags = cflags,')
                self.add('                includes = %r + includes,' % (lex_includes))
                self.add('                defines = defines + %r,' % (lex_defines))
                self.add('                source = "%s.c")' % (lex['file'][:-2]))
                self.add('    libbsd_use += ["lex_%s"]' % (lex['sym']))
                self.add('')

        if 'yacc' in data:
            yaccs = data['yacc']
            self.add('    # Yacc')
            for y in sorted(yaccs.keys()):
                yacc = yaccs[y]['all']
                yacc_file = yacc['file']
                if yacc['sym'] is not None:
                    yacc_sym = yacc['sym']
                else:
                    yacc_sym = os.path.basename(yacc_file)[:-2]
                yacc_header = '%s/%s' % (os.path.dirname(yacc_file), yacc['header'])
                if 'cflags' in yacc:
                    yacc_defines = [d[2:] for d in yacc['cflags']]
                else:
                    yacc_defines = []
                if 'includes' in yacc:
                    yacc_includes = yacc['includes']
                else:
                    yacc_includes = []
                self.add('    if bld.env.AUTO_REGEN:')
                self.add('        bld(target = "%s.c",' % (yacc_file[:-2]))
                self.add('            source = "%s",' % (yacc_file))
                self.add('            rule = "${YACC} -b %s -d -p %s ${SRC} && ' % (yacc_sym, yacc_sym) + \
                         'sed -e \'/YY_BUF_SIZE/s/16384/1024/\' < %s.tab.c > ${TGT} && ' % (yacc_sym) + \
                         'rm -f %s.tab.c && mv %s.tab.h %s")' % (yacc_sym, yacc_sym, yacc_header))
                self.add('    bld.objects(target = "yacc_%s",' % (yacc_sym))
                self.add('                features = "c",')
                self.add('                cflags = cflags,')
                self.add('                includes = %r + includes,' % (yacc_includes))
                self.add('                defines = defines + %r,' % (yacc_defines))
                self.add('                source = "%s.c")' % (yacc_file[:-2]))
                self.add('    libbsd_use += ["yacc_%s"]' % (yacc_sym))
            self.add('')

        #
        # We have 'm' different sets of flags and there can be 'n' cpus
        # specific files for those flags.
        #
        objs = 0
        self.add('    # Objects built with different CFLAGS')
        for flags in sorted(data['sources']):
            if flags is not 'default':
                objs += 1
                _source_list('    objs%02d_source' % objs, sorted(data['sources'][flags]['all']))
                archs = sorted(data['sources'][flags])
                for arch in archs:
                    if arch not in ['all', 'cflags', 'includes']:
                        self.add('    if bld.get_env()["RTEMS_ARCH"] == "%s":' % arch)
                        _source_list('        objs%02d_source' % objs,
                                     sorted(data['sources'][flags][arch]),
                                     append = True)
                if 'cflags' in data['sources'][flags]:
                    defines = [d[2:] for d in data['sources'][flags]['cflags']]
                else:
                    defines = []
                if 'includes' in data['sources'][flags]:
                    includes = data['sources'][flags]['includes']
                else:
                    includes = []
                self.add('    bld.objects(target = "objs%02d",' % (objs))
                self.add('                features = "c",')
                self.add('                cflags = cflags,')
                self.add('                includes = %r + includes,' % (includes))
                self.add('                defines = defines + %r,' % (defines))
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
        self.add('              defines = defines,')
        self.add('              source = source,')
        self.add('              use = libbsd_use)')
        self.add('')

        #
        # Head file collector.
        #
        self.add('    # Installs.    ')
        self.add('    bld.install_files("${PREFIX}/" + rtems.arch_bsp_lib_path(bld.env.RTEMS_VERSION, bld.env.RTEMS_ARCH_BSP), ["libbsd.a"])')
        header_paths = builder.header_paths()
        self.add('    header_paths = [%s,' % (str(header_paths[0])))
        for hp in header_paths[1:-1]:
            self.add('                     %s,' % (str(hp)))
        self.add('                     %s]' % (str(header_paths[-1])))
        self.add('    for headers in header_paths:')
        self.add('        ipath = os.path.join(rtems.arch_bsp_include_path(bld.env.RTEMS_VERSION, bld.env.RTEMS_ARCH_BSP), headers[2])')
        self.add('        start_dir = bld.path.find_dir(headers[0])')
        self.add('        bld.install_files("${PREFIX}/" + ipath,')
        self.add('                          start_dir.ant_glob("**/" + headers[1]),')
        self.add('                          cwd = start_dir,')
        self.add('                          relative_trick = True)')
        self.add('')

        self.add('    # Tests')
        tests = data['tests']
        for test_name in sorted(tests):
            files = ['testsuite/%s/%s.c' % (test_name, f) for f in  data['tests'][test_name]['all']['files']]
            _source_list('    test_%s' % (test_name), sorted(files))
            self.add('    bld.program(target = "%s.exe",' % (test_name))
            self.add('                features = "cprogram",')
            self.add('                cflags = cflags,')
            self.add('                includes = includes,')
            self.add('                source = test_%s,' % (test_name))
            self.add('                use = ["bsd"],')
            self.add('                lib = ["m", "z"],')
            self.add('                install_path = None)')
            self.add('')

        self.write()
