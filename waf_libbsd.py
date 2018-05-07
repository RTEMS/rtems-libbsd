#
#  Copyright (c) 2015-2018 Chris Johns <chrisj@rtems.org>. All rights reserved.
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
# Python 3 does no longer know the basestring class. Catch that.
try:
    basestring
except NameError:
    basestring = (str, bytes)

import os
import sys
import tempfile
import re

import builder

import rtems_waf.rtems as rtems

windows = os.name == 'nt'

if windows:
    host_shell = 'sh -c '
else:
    host_shell = ''

#
# The waf builder for libbsd.
#
class Builder(builder.ModuleManager):

    def __init__(self, trace = False):
        super(Builder, self).__init__()
        self.trace = trace
        self.data = {}

    @staticmethod
    def _sourceList(bld, files):
        sources = []
        if type(files) is dict:
            for cfg in files:
                if cfg in ['cflags', 'includes']:
                    continue
                if cfg != 'default':
                    for c in cfg.split(' '):
                        if not bld.env['HAVE_%s' % (c)]:
                            continue
                sources += sorted(files[cfg])
        else:
            sources = sorted(files)
        return sources

    def generate(self, rtems_version):

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

        self.generateBuild()

        self.data = {}

        for mn in self.getEnabledModules():
            m = self[mn]
            if m.conditionalOn == "none":
                for f in m.files:
                    _dataInsert(self.data, 'all', f.getFragment())
            for cpu, files in sorted(m.cpuDependentSourceFiles.items()):
                for f in files:
                    _dataInsert(self.data, cpu, f.getFragment())

        if self.trace:
            import pprint
            pprint.pprint(self.data)

    def bsp_configure(self, conf, arch_bsp):
        if 'configure' in self.data:
            for cfg in self.data['configure']:
                for h in self.data['configure'][cfg]:
                    conf.check(header_name = h,
                               features = "c",
                               includes = conf.env.IFLAGS,
                               mandatory = False)

    def build(self, bld):
        #
        # Localize the config.
        #
        config = self.getConfiguration()
        module_header_path = "rtems/bsd"
        module_header_name = "modules.h"

        #
        #
        # C/C++ flags
        #
        common_flags = []
        common_flags += ['-O%s' % (bld.env.OPTIMIZATION)]
        if 'common-flags' in config:
            common_flags += [f for f in config['common-flags']]
        if bld.env.WARNINGS and 'common-warnings' in config:
            common_flags += [f for f in config['common-warnings']]
        elif 'common-no-warnings' in config:
            common_flags += [f for f in config['common-no-warnings']]
        if 'cflags' in config:
            cflags = config['cflags'] + common_flags
        if 'cxxflags' in config:
            cxxflags = config['cxxflags'] + common_flags

        #
        # Defines
        #
        defines = []
        if len(bld.env.FREEBSD_OPTIONS) > 0:
            for o in bld.env.FREEBSD_OPTIONS.split(','):
                defines += ['%s=1' % (o.strip().upper())]

        #
        # Include paths
        #
        includes = []
        buildinclude = 'build-include'
        if 'cpu-include-paths' in config:
            cpu = bld.get_env()['RTEMS_ARCH']
            if cpu == "i386":
                cpu = 'x86'
            for i in config['cpu-include-paths']:
                includes += [i.replace('@CPU@', cpu)]
        if 'include-paths' in config:
            includes += config['include-paths']
        if 'build-include-path' in config:
            buildinclude = config['build-include-path']
            if not isinstance(buildinclude, basestring):
                buildinclude = buildinclude[0]
        includes += [buildinclude]

        #
        # Collect the libbsd uses
        #
        libbsd_use = []

        #
        # Network test configuration
        #
        if not os.path.exists(bld.env.NET_CONFIG):
            bld.fatal('network configuraiton \'%s\' not found' % (bld.env.NET_CONFIG))
        tags = [ 'NET_CFG_SELF_IP',
                 'NET_CFG_NETMASK',
                 'NET_CFG_PEER_IP',
                 'NET_CFG_GATEWAY_IP' ]
        try:
            net_cfg_lines = open(bld.env.NET_CONFIG).readlines()
        except:
            bld.fatal('network configuraiton \'%s\' read failed' % (bld.env.NET_CONFIG))
        lc = 0
        for l in net_cfg_lines:
            lc += 1
            if l.strip().startswith('NET_CFG_'):
                ls = l.split('=')
                if len(ls) != 2:
                    bld.fatal('network configuraiton \'%s\' ' + \
                              'parse error: %d: %s' % (bld.env.NET_CONFIG, lc, l))
                lhs = ls[0].strip()
                rhs = ls[1].strip()
                sed = 'sed '
                for t in tags:
                    if lhs == t:
                        sed += "-e 's/@%s@/%s/'" % (t, rhs)
        bld(target = "testsuite/include/rtems/bsd/test/network-config.h",
            source = "testsuite/include/rtems/bsd/test/network-config.h.in",
            rule = sed + " < ${SRC} > ${TGT}",
            update_outputs = True)

        #
        # Add a copy rule for all headers where the install path and the source
        # path are not the same.
        #
        if 'header-paths' in config:
            header_build_copy_paths = [
                hp for hp in config['header-paths'] if hp[2] != '' and not hp[0].endswith(hp[2])
            ]
            for headers in header_build_copy_paths:
                target = os.path.join(buildinclude, headers[2])
                start_dir = bld.path.find_dir(headers[0])
                for header in start_dir.ant_glob(headers[1]):
                    relsourcepath = header.path_from(start_dir)
                    targetheader = os.path.join(target, relsourcepath)
                    bld(features = 'subst',
                        target = targetheader,
                        source = header,
                        is_copy = True)

        #
        # Generate a header that contains information about enabled modules
        #
        def rtems_libbsd_modules_h_gen(self):
            output = ""
            output += '/*\n'
            output += ' * This file contains a list of modules that have been\n'
            output += ' * enabled during libbsd build. It is a generated file\n'
            output += ' * DO NOT EDIT MANUALLY.\n'
            output += ' */'
            output += '\n'
            output += '#ifndef RTEMS_BSD_MODULES_H\n'
            for mod in config['modules-enabled']:
                modname = re.sub("[^A-Za-z0-9]", "_", mod.upper())
                output += '#define RTEMS_BSD_MODULE_{} 1\n'.format(modname)
            output += '#endif /* RTEMS_BSD_MODULES_H */\n'
            self.outputs[0].write(output)
        modules_h_file_with_path = os.path.join(buildinclude,
                                                module_header_path,
                                                module_header_name)
        bld(rule = rtems_libbsd_modules_h_gen,
            target = modules_h_file_with_path,
            before = ['c', 'cxx'])

        #
        # Add the specific rule based builders
        #

        #
        # KVM Symbols
        #
        if 'KVMSymbols' in self.data:
            kvmsymbols = self.data['KVMSymbols']
            if 'includes' in kvmsymbols['files']:
                kvmsymbols_includes = kvmsymbols['files']['includes']
            else:
                kvmsymbols_includes = []
            bld(target = kvmsymbols['files']['all']['default'][0],
                source = 'rtemsbsd/rtems/generate_kvm_symbols',
                rule = host_shell + './${SRC} > ${TGT}',
                update_outputs = True)
            bld.objects(target = 'kvmsymbols',
                        features = 'c',
                        cflags = cflags,
                        includes = kvmsymbols_includes + includes,
                        source = kvmsymbols['files']['all']['default'][0])
            libbsd_use += ["kvmsymbols"]

        bld.add_group()

        #
        # RPC Generation
        #
        if 'RPCGen' in self.data:
            if bld.env.AUTO_REGEN:
                rpcgen = self.data['RPCGen']
                rpcname = rpcgen['files']['all']['default'][0][:-2]
                bld(target = rpcname + '.h',
                    source = rpcname + '.x',
                    rule = host_shell + '${RPCGEN} -h -o ${TGT} ${SRC}')

        #
        # Route keywords
        #
        if 'RouteKeywords' in self.data:
            if bld.env.AUTO_REGEN:
                routekw = self.data['RouteKeywords']
                rkwname = routekw['files']['all']['default'][0]
                rkw_rule = host_shell + "cat ${SRC} | " + \
                           "awk 'BEGIN { r = 0 } { if (NF == 1) " + \
                           "printf \"#define\\tK_%%s\\t%%d\\n\\t{\\\"%%s\\\", K_%%s},\\n\", " + \
                           "toupper($1), ++r, $1, toupper($1)}' > ${TGT}"
                bld(target = rkwname + '.h',
                    source = rkwname,
                    rule = rkw_rule)

        #
        # Lex
        #
        if 'lex' in self.data:
            lexes = self.data['lex']
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
                lex_rule = host_shell + '${LEX} -P ' + lex['sym'] + ' -t ${SRC} | ' + \
                           'sed -e \'/YY_BUF_SIZE/s/16384/1024/\' > ${TGT}'
                if bld.env.AUTO_REGEN:
                    bld(target = lex['file'][:-2]+ '.c',
                        source = lex['file'],
                        rule = lex_rule)
                if lex['build']:
                    bld.objects(target = 'lex_%s' % (lex['sym']),
                                features = 'c',
                                cflags = cflags,
                                includes = lexIncludes + includes,
                                defines = defines + lexDefines,
                                source = lex['file'][:-2] + '.c')
                libbsd_use += ['lex_%s' % (lex['sym'])]

        #
        # Yacc
        #
        if 'yacc' in self.data:
            yaccs = self.data['yacc']
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
                yacc_rule = host_shell + '${YACC} -b ' + yaccSym + \
                            ' -d -p ' + yaccSym + ' ${SRC} && ' + \
                            'sed -e \'/YY_BUF_SIZE/s/16384/1024/\' < ' + yaccSym + '.tab.c > ${TGT} && ' + \
                            'rm -f ' + yaccSym + '.tab.c && mv ' + yaccSym + '.tab.h ' + yaccHeader
                if bld.env.AUTO_REGEN:
                    bld(target = yaccFile[:-2] + '.c',
                        source = yaccFile,
                        rule = yacc_rule)
                if yacc['build']:
                    bld.objects(target = 'yacc_%s' % (yaccSym),
                                features = 'c',
                                cflags = cflags,
                                includes = yaccIncludes + includes,
                                defines = defines + yaccDefines,
                                source = yaccFile[:-2] + '.c')
                libbsd_use += ['yacc_%s' % (yaccSym)]

        #
        # We have 'm' different sets of flags and there can be 'n' cpus
        # specific files for those flags.
        #
        objs = 0
        sources = sorted(self.data['sources'])
        if 'default' in sources:
            sources.remove('default')
        for flags in sources:
            objs += 1
            build = self.data['sources'][flags]
            target = 'objs%02d' % (objs)
            bld_sources = Builder._sourceList(bld, build['all'])
            archs = sorted(build)
            for i in ['all', 'cflags', 'includes']:
                if i in archs:
                    archs.remove(i)
            for arch in archs:
                if bld.get_env()['RTEMS_ARCH'] == arch:
                    bld_sources += Builder._sourceList(bld, build[arch])
            if 'cflags' in build:
                bld_defines = [d[2:] for d in build['cflags']]
            else:
                bld_defines = []
            if 'includes' in build:
                bld_includes = build['includes']
            else:
                bld_includes = []
            bld.objects(target = target,
                        features = 'c',
                        cflags = cflags,
                        includes = sorted(bld_includes) + includes,
                        defines = defines + sorted(bld_defines),
                        source = bld_sources)
            libbsd_use += [target]

        #
        # We hold the 'default' cflags set of files to the end to create the
        # static library with.
        #
        build = self.data['sources']['default']
        bld_sources = Builder._sourceList(bld, build['all'])
        archs = sorted(build)
        archs.remove('all')
        for arch in archs:
            if bld.get_env()['RTEMS_ARCH'] == arch:
                bld_sources += Builder._sourceList(bld, build[arch])
        bld.stlib(target = 'bsd',
                  features = 'c cxx',
                  cflags = cflags,
                  cxxflags = cxxflags,
                  includes = includes,
                  defines = defines,
                  source = bld_sources,
                  use = libbsd_use)

        #
        # Installs.
        #
        # Header file collector.
        #
        arch_lib_path = rtems.arch_bsp_lib_path(bld.env.RTEMS_VERSION,
                                                bld.env.RTEMS_ARCH_BSP)
        arch_inc_path = rtems.arch_bsp_include_path(bld.env.RTEMS_VERSION,
                                                    bld.env.RTEMS_ARCH_BSP)

        bld.install_files("${PREFIX}/" + arch_lib_path, ["libbsd.a"])

        if 'header-paths' in config:
            headerPaths = config['header-paths']
            cpu = bld.get_env()['RTEMS_ARCH']
            if cpu == "i386":
                cpu = 'x86'
            for headers in headerPaths:
                # Get the dest path
                ipath = os.path.join(arch_inc_path, headers[2])
                start_dir = bld.path.find_dir(headers[0].replace('@CPU@', cpu))
                if start_dir != None:
                    bld.install_files("${PREFIX}/" + ipath,
                                      start_dir.ant_glob(headers[1]),
                                      cwd = start_dir,
                                      relative_trick = True)

        bld.install_files(os.path.join("${PREFIX}", arch_inc_path,
                                       module_header_path),
                          modules_h_file_with_path,
                          cwd = bld.path)

        #
        # Tests
        #
        tests = self.data['tests']
        for testName in sorted(tests):
            test = self.data['tests'][testName]['all']
            test_source = []
            libs = ['bsd', 'm', 'z']
            for cfg in test:
                build_test = True
                if cfg != 'default':
                    for c in cfg.split(' '):
                        if not bld.env['HAVE_%s' % (c)]:
                            build_test = False
                            break
                if build_test:
                    test_sources = ['testsuite/%s/%s.c' % (testName, f) \
                                    for f in test[cfg]['files']]
                    libs = test[cfg]['libs'] + libs
            if build_test:
                bld.program(target = '%s.exe' % (testName),
                            features = 'cprogram',
                            cflags = cflags,
                            includes = includes,
                            source = test_sources,
                            use = ['bsd'],
                            lib = libs,
                            install_path = None)
