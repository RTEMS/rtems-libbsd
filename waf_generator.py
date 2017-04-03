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

#
# Dump the data created from the fragments returned from the builder composers.
#
trace = False

data = { }

def _addFiles(name, files):
    if type(files) is not list:
        files = [files]
    if name not in data:
        data[name] = []
    data[name] += files

def _cflagsIncludes(cflags, includes):
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
        self.cflags, self.includes = _cflagsIncludes(cflags, includes)

    def compose(self, path):
        if None in self.includes:
            flags = self.cflags
        else:
            flags = self.cflags + self.includes
        return ['sources', flags, ('default', None)], [path], self.cflags, self.includes

class SourceFileIfHeaderComposer(SourceFileFragmentComposer):

    def __init__(self, headers, cflags = "default", includes = None):
        if headers is not list:
            headers = [headers]
        self.headers = headers
        super(SourceFileIfHeaderComposer, self).__init__(cflags = cflags, includes = includes)

    def compose(self, path):
        r = SourceFileFragmentComposer.compose(self, path)
        define_keys = ''
        for h in self.headers:
            h = h.upper()
            for c in '\/-.':
                h = h.replace(c, '_')
            define_keys += ' ' + h
        r[0][2] = (define_keys.strip(), self.headers)
        return r

class TestFragementComposer(builder.BuildSystemFragmentComposer):

    def __init__(self, testName, fileFragments, runTest = True, netTest = False):
        self.testName = testName
        self.fileFragments = fileFragments
        self.runTest = runTest
        self.netTest = netTest

    def compose(self, path):
        return ['tests', self.testName, ('default', None)], { 'files': self.fileFragments,
                                                              'run': self.runTest,
                                                              'net': self.netTest }

class TestIfHeaderComposer(TestFragementComposer):

    def __init__(self, testName, headers, fileFragments, runTest = True, netTest = False):
        if headers is not list:
            headers = [headers]
        self.headers = headers
        super(TestIfHeaderComposer, self).__init__(testName, fileFragments,
                                                   runTest = runTest, netTest = netTest)

    def compose(self, path):
        r = TestFragementComposer.compose(self, path)
        define_keys = ''
        for h in self.headers:
            h = h.upper()
            for c in '\/-.':
                h = h.replace(c, '_')
            define_keys += ' ' + h
        r[0][2] = (define_keys.strip(), self.headers)
        return r

class KVMSymbolsFragmentComposer(builder.BuildSystemFragmentComposer):

    def compose(self, path):
        return ['KVMSymbols', 'files', ('default', None)], [path], self.includes

class RPCGENFragmentComposer(builder.BuildSystemFragmentComposer):

    def compose(self, path):
        return ['RPCGen', 'files', ('default', None)], [path]

class RouteKeywordsFragmentComposer(builder.BuildSystemFragmentComposer):

    def compose(self, path):
        return ['RouteKeywords', 'files', ('default', None)], [path]

class LexFragmentComposer(builder.BuildSystemFragmentComposer):

    def __init__(self, sym, dep, cflags = None, includes = None):
        self.sym = sym
        self.dep = dep
        self.cflags, self.includes = _cflagsIncludes(cflags, includes)

    def compose(self, path):
        d = { 'file': path,
              'sym': self.sym,
              'dep': self.dep }
        if None not in self.cflags:
            d['cflags'] = self.cflags
        if None not in self.includes:
            d['includes'] = self.includes
        return ['lex', path, ('default', None)], d

class YaccFragmentComposer(builder.BuildSystemFragmentComposer):

    def __init__(self, sym, header, cflags = None, includes = None):
        self.sym = sym
        self.header = header
        self.cflags, self.includes = _cflagsIncludes(cflags, includes)

    def compose(self, path):
        d = { 'file': path,
              'sym': self.sym,
              'header': self.header }
        if None not in self.cflags:
            d['cflags'] = self.cflags
        if None not in self.includes:
            d['includes'] = self.includes
        return ['yacc', path, ('default', None)], d

# Module Manager - Collection of Modules
class ModuleManager(builder.ModuleManager):

    def restart(self):
        self.script = ''

    def add(self, line = ''):
        self.script += line + os.linesep

    def write(self):
        name = os.path.join(builder.LIBBSD_DIR, 'libbsd_waf.py')
        converter = builder.Converter()
        converter.convert(name, name, srcContents = self.script)

    def setGenerators(self):
        self.generator['convert'] = builder.Converter
        self.generator['no-convert'] = builder.NoConverter
        self.generator['from-FreeBSD-to-RTEMS-UserSpaceSourceConverter'] = builder.FromFreeBSDToRTEMSUserSpaceSourceConverter
        self.generator['from-RTEMS-To-FreeBSD-SourceConverter'] = builder.FromRTEMSToFreeBSDSourceConverter
        self.generator['buildSystemFragmentComposer'] = builder.BuildSystemFragmentComposer

        self.generator['file'] = builder.File

        self.generator['path'] = builder.PathComposer
        self.generator['freebsd-path'] = builder.FreeBSDPathComposer
        self.generator['rtems-path'] = builder.RTEMSPathComposer
        self.generator['cpu-path'] = builder.CPUDependentFreeBSDPathComposer
        self.generator['target-src-cpu--path'] = builder.TargetSourceCPUDependentPathComposer

        self.generator['source'] = SourceFileFragmentComposer
        self.generator['test'] = TestFragementComposer
        self.generator['kvm-symbols'] = KVMSymbolsFragmentComposer
        self.generator['rpc-gen'] = RPCGENFragmentComposer
        self.generator['route-keywords'] = RouteKeywordsFragmentComposer
        self.generator['lex'] = LexFragmentComposer
        self.generator['yacc'] = YaccFragmentComposer

        self.generator['source-if-header'] = SourceFileIfHeaderComposer
        self.generator['test-if-header'] = TestIfHeaderComposer

    def generate(self, rtems_version):

        def _sourceListSources(lhs, sources, append = False, block = 0):
            indent = block * 4
            if append:
                adder = '+'
                adderSpace = ' '
            else:
                adder = ''
                adderSpace = ''
            ll = len(lhs)
            if len(sources) == 1:
                self.add('%s%s %s= [%r]' % (' ' * indent, lhs, adder, sources[0]))
            elif len(sources) == 2:
                self.add('%s%s %s= [%r,' % (' ' * indent, lhs, adder, sources[0]))
                self.add('%s%s %s   %r]' % (' ' * indent, ' ' * ll, adderSpace, sources[-1]))
            elif len(sources) > 0:
                self.add('%s%s %s= [%r,' % (' ' * indent, lhs, adder, sources[0]))
                for f in sources[1:-1]:
                    self.add('%s%s %s   %r,' % (' ' * indent, ' ' * ll, adderSpace, f))
                self.add('%s%s %s   %r]' % (' ' * indent, ' ' * ll, adderSpace, sources[-1]))

        def _sourceList(lhs, files, append = False):
            if type(files) is dict:
                appending = False
                for cfg in files:
                    if cfg in ['cflags', 'includes']:
                        continue
                    if cfg != 'default':
                        cs = ''
                        ors = ''
                        for c in cfg.split(' '):
                            cs += '%s bld.env["HAVE_%s"]' % (ors, c)
                            ors = ' and'
                        self.add('    if%s:' % (cs))
                        _sourceListSources(lhs, sorted(files[cfg]), append = appending, block = 1)
                    else:
                        _sourceListSources(lhs, sorted(files[cfg]), append)
                    appending = True
            else:
                _sourceListSources(lhs, sorted(files), append)

        def _dataInsert(data, cpu, frag):
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
                    d[cpu] = { }
                config = frag[0][2][0]
                if config != 'default':
                    if 'configure' not in data:
                        data['configure'] = { }
                    data['configure'][config] = frag[0][2][1]
                if type(frag[1]) is list:
                    if config not in d[cpu]:
                        d[cpu][config] = []
                    d[cpu][config] += frag[1]
                else:
                    d[cpu][config] = frag[1]
                #
                # The CPU is for files and the flags and includes are common.
                #
                if len(frag) > 3:
                    if 'cflags' not in d:
                        d['cflags'] = []
                    d['cflags'] += frag[2]
                    d['cflags'] = list(set(d['cflags']))
                if len(frag) >= 3 and None not in frag[-1]:
                    if 'includes' not in d:
                        d['includes'] = []
                    d['includes'] += frag[-1]
                    d['includes'] = list(set(d['includes']))

        data = { }

        for mn in self.getModules():
            m = self[mn]
            if m.conditionalOn == "none":
                for f in m.files:
                    _dataInsert(data, 'all', f.getFragment())
            for cpu, files in sorted(m.cpuDependentSourceFiles.items()):
                for f in files:
                    _dataInsert(data, cpu, f.getFragment())

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
        self.add('import os')
        self.add('import os.path')
        # Import check done in the top level wscript file.
        self.add('import rtems_waf.rtems as rtems')
        self.add('')
        self.add('windows = os.name == "nt"')
        self.add('')
        self.add('if windows:')
        self.add('    host_shell = "sh -c "')
        self.add('else:')
        self.add('    host_shell = ""')
        self.add('')
        self.add('def init(ctx):')
        self.add('    pass')
        self.add('')
        self.add('def options(opt):')
        self.add('    pass')
        self.add('')
        self.add('def bsp_configure(conf, arch_bsp):')

        if 'configure' in data:
            for cfg in data['configure']:
                for h in data['configure'][cfg]:
                    self.add('    conf.check(header_name = "%s", features = "c", includes = conf.env.IFLAGS, mandatory = False)' % h)
        else:
            self.add('    pass')

        self.add('')
        self.add('def configure(conf):')
        self.add('    rtems.configure(conf, bsp_configure)')
        self.add('')
        self.add('def build(bld):')
        self.add('    # C/C++ flags')
        self.add('    common_flags = []')
        self.add('    common_flags += ["-O%s" % (bld.env.OPTIMIZATION)]')
        for f in builder.commonFlags():
            self.add('    common_flags += ["%s"]' % (f))
        self.add('    if bld.env.WARNINGS:')
        for f in builder.commonWarnings():
            self.add('        common_flags += ["%s"]' % (f))
        self.add('    else:')
        for f in builder.commonNoWarnings():
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
        self.add('    for i in %r:' % (builder.cpuIncludes()))
        self.add('        includes += ["%s" % (i[2:].replace("@CPU@", bld.get_env()["RTEMS_ARCH"]))]')
        self.add('    if bld.get_env()["RTEMS_ARCH"] == "i386":')
        self.add('        for i in %r:' % (builder.cpuIncludes()))
        self.add('            includes += ["%s" % (i[2:].replace("@CPU@", "x86"))]')
        for i in builder.includes() + ['-I' + builder.buildInclude()]:
            self.add('    includes += ["%s"]' % (i[2:]))
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
        # Add a copy rule for all headers where the install path and the source
        # path are not the same.
        #
        self.add('    # copy headers if necessary')
        self.add('    header_build_copy_paths = [')
        for hp in builder.headerPaths():
            if hp[2] != '' and not hp[0].endswith(hp[2]):
                self.add('                               %s,' % (str(hp)))
        self.add('                              ]')
        self.add('    for headers in header_build_copy_paths:')
        self.add('        target = os.path.join("%s", headers[2])' % (builder.buildInclude()))
        self.add('        start_dir = bld.path.find_dir(headers[0])')
        self.add('        for header in start_dir.ant_glob(headers[1]):')
        self.add('            relsourcepath = header.path_from(start_dir)')
        self.add('            targetheader = os.path.join(target, relsourcepath)')
        self.add('            bld(features = \'subst\',')
        self.add('                target = targetheader,')
        self.add('                source = header,')
        self.add('                is_copy = True)')
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
            self.add('    bld(target = "%s",' % (kvmsymbols['files']['all']['default'][0]))
            self.add('        source = "rtemsbsd/rtems/generate_kvm_symbols",')
            self.add('        rule = host_shell + "./${SRC} > ${TGT}",')
            self.add('        update_outputs = True)')
            self.add('    bld.objects(target = "kvmsymbols",')
            self.add('                features = "c",')
            self.add('                cflags = cflags,')
            self.add('                includes = %r + includes,' % (includes))
            self.add('                source = "%s")' % (kvmsymbols['files']['all']['default'][0]))
            self.add('    libbsd_use += ["kvmsymbols"]')
            self.add('')

        self.add('    bld.add_group()')

        if 'RPCGen' in data:
            rpcgen = data['RPCGen']
            rpcname = rpcgen['files']['all']['default'][0][:-2]
            self.add('    # RPC Generation')
            self.add('    if bld.env.AUTO_REGEN:')
            self.add('        bld(target = "%s.h",' % (rpcname))
            self.add('            source = "%s.x",' % (rpcname))
            self.add('            rule = host_shell + "${RPCGEN} -h -o ${TGT} ${SRC}")')
            self.add('')

        if 'RouteKeywords' in data:
            routekw = data['RouteKeywords']
            rkwname = routekw['files']['all']['default'][0]
            self.add('    # Route keywords')
            self.add('    if bld.env.AUTO_REGEN:')
            self.add('        rkw_rule = host_shell + "cat ${SRC} | ' + \
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
                lex = lexes[l]['all']['default']
                if 'cflags' in lex:
                    lexDefines = [d[2:] for d in lex['cflags']]
                else:
                    lexDefines = []
                if 'includes' in lex:
                    lexIncludes = lex['includes']
                else:
                    lexIncludes = []
                self.add('    if bld.env.AUTO_REGEN:')
                self.add('        bld(target = "%s.c",' % (lex['file'][:-2]))
                self.add('            source = "%s",' % (lex['file']))
                self.add('            rule = host_shell + "${LEX} -P %s -t ${SRC} | ' % (lex['sym']) + \
                         'sed -e \'/YY_BUF_SIZE/s/16384/1024/\' > ${TGT}")')
                self.add('    bld.objects(target = "lex_%s",' % (lex['sym']))
                self.add('                features = "c",')
                self.add('                cflags = cflags,')
                self.add('                includes = %r + includes,' % (lexIncludes))
                self.add('                defines = defines + %r,' % (lexDefines))
                self.add('                source = "%s.c")' % (lex['file'][:-2]))
                self.add('    libbsd_use += ["lex_%s"]' % (lex['sym']))
                self.add('')

        if 'yacc' in data:
            yaccs = data['yacc']
            self.add('    # Yacc')
            for y in sorted(yaccs.keys()):
                yacc = yaccs[y]['all']['default']
                yaccFile = yacc['file']
                if yacc['sym'] is not None:
                    yaccSym = yacc['sym']
                else:
                    yaccSym = os.path.basename(yaccFile)[:-2]
                yaccHeader = '%s/%s' % (os.path.dirname(yaccFile), yacc['header'])
                if 'cflags' in yacc:
                    yaccDefines = [d[2:] for d in yacc['cflags']]
                else:
                    yaccDefines = []
                if 'includes' in yacc:
                    yaccIncludes = yacc['includes']
                else:
                    yaccIncludes = []
                self.add('    if bld.env.AUTO_REGEN:')
                self.add('        bld(target = "%s.c",' % (yaccFile[:-2]))
                self.add('            source = "%s",' % (yaccFile))
                self.add('            rule = host_shell + "${YACC} -b %s -d -p %s ${SRC} && ' % \
                         (yaccSym, yaccSym) + \
                         'sed -e \'/YY_BUF_SIZE/s/16384/1024/\' < %s.tab.c > ${TGT} && ' % (yaccSym) + \
                         'rm -f %s.tab.c && mv %s.tab.h %s")' % (yaccSym, yaccSym, yaccHeader))
                self.add('    bld.objects(target = "yacc_%s",' % (yaccSym))
                self.add('                features = "c",')
                self.add('                cflags = cflags,')
                self.add('                includes = %r + includes,' % (yaccIncludes))
                self.add('                defines = defines + %r,' % (yaccDefines))
                self.add('                source = "%s.c")' % (yaccFile[:-2]))
                self.add('    libbsd_use += ["yacc_%s"]' % (yaccSym))
            self.add('')

        #
        # We have 'm' different sets of flags and there can be 'n' cpus
        # specific files for those flags.
        #
        objs = 0
        self.add('    # Objects built with different CFLAGS')
        sources = sorted(data['sources'])
        if 'default' in sources:
            sources.remove('default')
        for flags in sources:
            objs += 1
            build = data['sources'][flags]
            _sourceList('    objs%02d_source' % objs, build['all'])
            archs = sorted(build)
            for i in ['all', 'cflags', 'includes']:
                if i in archs:
                    archs.remove(i)
            for arch in archs:
                self.add('    if bld.get_env()["RTEMS_ARCH"] == "%s":' % arch)
                _sourceList('        objs%02d_source' % objs, build[arch], append = True)
            if 'cflags' in build:
                defines = [d[2:] for d in build['cflags']]
            else:
                defines = []
            if 'includes' in build:
                includes = build['includes']
            else:
                includes = []
            self.add('    bld.objects(target = "objs%02d",' % (objs))
            self.add('                features = "c",')
            self.add('                cflags = cflags,')
            self.add('                includes = %r + includes,' % (sorted(includes)))
            self.add('                defines = defines + %r,' % (sorted(defines)))
            self.add('                source = objs%02d_source)' % objs)
            self.add('    libbsd_use += ["objs%02d"]' % (objs))
            self.add('')

        #
        # We hold the 'default' cflags set of files to the end to create the
        # static library with.
        #
        build = data['sources']['default']
        _sourceList('    source', build['all'])
        archs = sorted(build)
        archs.remove('all')
        for arch in archs:
            self.add('    if bld.get_env()["RTEMS_ARCH"] == "%s":' % arch)
            _sourceList('        source', build[arch], append = True)
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
        # Header file collector.
        #
        self.add('    # Installs.    ')
        self.add('    bld.install_files("${PREFIX}/" + rtems.arch_bsp_lib_path(bld.env.RTEMS_VERSION, bld.env.RTEMS_ARCH_BSP), ["libbsd.a"])')
        headerPaths = builder.headerPaths()
        self.add('    header_paths = [%s,' % (str(headerPaths[0])))
        for hp in headerPaths[1:-1]:
            self.add('                     %s,' % (str(hp)))
        self.add('                     %s]' % (str(headerPaths[-1])))
        self.add('    for headers in header_paths:')
        self.add('        ipath = os.path.join(rtems.arch_bsp_include_path(bld.env.RTEMS_VERSION, bld.env.RTEMS_ARCH_BSP), headers[2])')
        self.add('        start_dir = bld.path.find_dir(headers[0])')
        self.add('        bld.install_files("${PREFIX}/" + ipath,')
        self.add('                          start_dir.ant_glob(headers[1]),')
        self.add('                          cwd = start_dir,')
        self.add('                          relative_trick = True)')
        self.add('')

        self.add('    # Tests')
        tests = data['tests']
        for testName in sorted(tests):
            test = data['tests'][testName]['all']
            block = 0
            files = []
            for cfg in test:
                if cfg != 'default':
                    cs = ''
                    ors = ''
                    for c in cfg.split(' '):
                        cs += '%s bld.env["HAVE_%s"]' % (ors, c)
                        ors = ' and'
                    self.add('    if%s:' % (cs))
                    block = 1
                files = ['testsuite/%s/%s.c' % (testName, f) \
                         for f in test[cfg]['files']]
            indent = ' ' * block * 4
            _sourceList('%s    test_%s' % (indent, testName), files)
            self.add('%s    bld.program(target = "%s.exe",' % (indent, testName))
            self.add('%s                features = "cprogram",' % (indent))
            self.add('%s                cflags = cflags,' % (indent))
            self.add('%s                includes = includes,' % (indent))
            self.add('%s                source = test_%s,' % (indent, testName))
            self.add('%s                use = ["bsd"],' % (indent))
            self.add('%s                lib = ["m", "z"],' % (indent))
            self.add('%s                install_path = None)' % (indent))
            self.add('')

        self.write()
