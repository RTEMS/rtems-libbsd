# SPDX-License-Identifier: BSD-2-Clause
"""LibBSD build configuration to waf integration module.
"""

# Copyright (c) 2015, 2021 Chris Johns <chrisj@rtems.org>. All rights reserved.
#
# Copyright (c) 2009, 2015 embedded brains GmbH.  All rights reserved.
#
#   embedded brains GmbH
#   Dornierstr. 4
#   82178 Puchheim
#   Germany
#   <info@embedded-brains.de>
#
# Copyright (c) 2012 OAR Corporation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

from __future__ import print_function

import os
import sys
import tempfile
import re

import builder

import rtems_waf.rtems as rtems


BUILDSET_DIR = builder.BUILDSET_DIR
BUILDSET_DEFAULT = builder.BUILDSET_DEFAULT

windows = os.name == 'nt'

if windows:
    host_shell = 'sh -c '
else:
    host_shell = ''

def _add_flags_if_not_present(current_flags, addional_flags):
    for flag in addional_flags:
        if flag not in current_flags:
            current_flags.append(flag)

def _remove_bsp_include_path(bsp_include_path, current_flags):
    # this does not handle quted strings; maybe needed
    for bsp_path in bsp_include_path:
        if bsp_path in current_flags:
            current_flags = [flag for flag in current_flags if flag != bsp_path]
    return current_flags

#
# The waf builder for libbsd.
#
class Builder(builder.ModuleManager):
    def __init__(self, trace=False):
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
        def _dataInsert(data, cpu, space, frag):
            #
            # The default handler returns None. Skip it.
            #
            if frag is not None:
                # Start at the top of the tree
                d = data
                path = frag[0]
                if path[0] not in d:
                    d[path[0]] = {}
                # Select the sub-part of the tree as the compile options
                # specialise how files are built.
                d = d[path[0]]
                # Group based on the space, ie kernel or user
                if space not in d:
                    d[space] = {}
                d = d[space]
                if type(path[1]) is list:
                    p = ' '.join(path[1])
                else:
                    p = path[1]
                if p not in d:
                    d[p] = {}
                d = d[p]
                if cpu not in d:
                    d[cpu] = {}
                config = frag[0][2][0]
                if config != 'default':
                    if 'configure' not in data:
                        data['configure'] = {}
                    configTest = frag[1]['configTest']
                    if configTest not in data['configure']:
                        data['configure'][configTest] = {}
                    data['configure'][configTest][config] = frag[0][2][1]
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

        enabled_modules = self.getEnabledModules()
        for mn in enabled_modules:
            m = self[mn]
            enabled = True
            for dep in m.dependencies:
                if dep not in enabled_modules:
                    enabled = False
                    break
            if enabled:
                for f in m.files:
                    _dataInsert(self.data, 'all', f.getSpace(),
                                f.getFragment())
                for cpu, files in sorted(m.cpuDependentSourceFiles.items()):
                    for f in files:
                        _dataInsert(self.data, cpu, f.getSpace(),
                                    f.getFragment())

        # Start here if you need to understand self.data. Add 'True or'
        if self.trace:
            import pprint
            pprint.pprint(self.data)

    def bsp_configure(self, conf, arch_bsp):
        if 'configure' in self.data:
            for configTest in self.data['configure']:
                for cfg in self.data['configure'][configTest]:
                    if configTest == 'header':
                        for h in self.data['configure'][configTest][cfg]:
                            conf.check(header_name=h,
                                       features="c",
                                       includes=conf.env.IFLAGS,
                                       mandatory=False)
                    elif configTest == 'library':
                        for l in self.data['configure'][configTest][cfg]:
                            if conf.check_cc(lib=l,
                                             fragment=rtems.test_application(),
                                             execute=False,
                                             mandatory=False):
                                conf.env['HAVE_%s' % l.upper()] = True
                    else:
                        bld.fatal('invalid config test: %s' % (configTest))
            conf.env.CFLAGS = _remove_bsp_include_path(conf.env.IFLAGS,
                                                       conf.env.CFLAGS)
            conf.env.CXXFLAGS = _remove_bsp_include_path(conf.env.IFLAGS,
                                                                    conf.env.CXXFLAGS)
            conf.env.LINKFLAGS = _remove_bsp_include_path(conf.env.IFLAGS,
                                                          conf.env.LINKFLAGS)
        section_flags = ["-fdata-sections", "-ffunction-sections"]
        _add_flags_if_not_present(conf.env.CFLAGS, section_flags)
        _add_flags_if_not_present(conf.env.CXXFLAGS, section_flags)
        _add_flags_if_not_present(conf.env.LINKFLAGS, ["-Wl,--gc-sections"])

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
        # Include paths, maintain paths for each build space.
        #
        include_paths = config['include-paths']
        if 'build' not in include_paths:
            bld.fatal('no build include path found in include-path defaults')
        buildinclude = include_paths['build']
        if isinstance(buildinclude, list):
            buildinclude = buildinclude[0]
        inc_paths = sorted(include_paths)
        inc_paths.remove('build')
        inc_paths.remove('cpu')
        includes = { 'bsp': [p[2:] for p in bld.env.IFLAGS] }
        for inc in inc_paths:
            includes[inc] = include_paths[inc]
        # cpu include paths must be the first searched
        if 'cpu' in include_paths:
            cpu = bld.get_env()['RTEMS_ARCH']
            for i in include_paths['cpu']:
                includes['kernel'].insert(0, i.replace('@CPU@', cpu))
        includes['kernel'] += [buildinclude]

        #
        # Path mappings
        #
        if 'path-mappings' in config:
            for source, target in config['path-mappings']:
                for space in includes:
                    incs = includes[space]
                    if source in incs:
                        target = [target] if isinstance(target,
                                                        str) else target
                        i = incs.index(source)
                        incs.remove(source)
                        incs[i:i] = target

        #
        # Place the kernel include paths after the user paths
        #
        includes['user'] += includes['kernel']

        #
        # Path mappings
        #
        if 'path-mappings' in config:
            for source, target in config['path-mappings']:
                if source in includes:
                    target = [target] if isinstance(target, str) else target
                    i = includes.index(source)
                    includes.remove(source)
                    includes[i:i] = target

        #
        # Collect the libbsd uses
        #
        libbsd_use = []

        #
        # Network test configuration
        #
        if not os.path.exists(bld.env.NET_CONFIG):
            bld.fatal('network configuraiton \'%s\' not found' %
                      (bld.env.NET_CONFIG))
        net_cfg = {
            'NET_CFG_INTERFACE_0': { 'mandatory': True,  },
            'NET_CFG_SELF_IP': { 'mandatory': True },
            'NET_CFG_NETMASK': { 'mandatory': True },
            'NET_CFG_PEER_IP': { 'mandatory': True },
            'NET_CFG_GATEWAY_IP': { 'manditory': True },
            'NET_CFG_NFS_MOUNT_PATH': { 'mandatory': False,
                                        'default': '@NET_CFG_PEER_IP@/rtems' },
            'NET_CFG_NFS_MOUNT_OPTIONS': { 'mandatory': False,
                                           'default': 'nfsv4,minorversion=1' }
        }
        tags = list(net_cfg.keys())
        config_inc = bld.path.find_node('config.inc')
        try:
            config_inc_lines = open(config_inc.abspath()).readlines()
        except:
            bld.fatal('network configuraiton \'%s\' read failed' %
                      (config_inc.abspath()))
        for l in config_inc_lines:
            if l.strip().startswith('NET_CFG_'):
                ls = l.split('=', 1)
                if len(ls) == 2:
                    lhs = ls[0].strip()
                    rhs = ls[1].strip()
                    if lhs in tags:
                        net_cfg[lhs]['default'] = rhs
        try:
            net_cfg_lines = open(bld.env.NET_CONFIG).readlines()
        except:
            bld.fatal('network configuraiton \'%s\' read failed' %
                      (bld.env.NET_CONFIG))
        lc = 0
        for l in net_cfg_lines:
            lc += 1
            if l.strip().startswith('NET_CFG_'):
                ls = l.split('=', 1)
                if len(ls) != 2:
                    bld.fatal('network configuraiton \'%s\' ' + \
                              'parse error: %d: %s' % (bld.env.NET_CONFIG, lc, l))
                lhs = ls[0].strip()
                rhs = ls[1].strip()
                if lhs in tags:
                    net_cfg[lhs]['value'] = rhs
        for tag in net_cfg:
            if 'value' not in net_cfg[tag]:
                if net_cfg[tag]['mandatory']:
                    bld.fatal('network configuraiton \'%s\' ' + \
                              'entry not found: %s' % (bld.env.NET_CONFIG, tag))
                net_cfg[tag]['value'] = net_cfg[tag]['default']
        updated = True
        while updated:
            updated = False
            for tag in net_cfg:
                for rtag in net_cfg:
                    if tag != rtag and 'value' in net_cfg[rtag]:
                        pattern = re.escape('@' + tag + '@')
                        repl = net_cfg[tag]['value']
                        value = re.sub(pattern, repl, net_cfg[rtag]['value'])
                        if value != net_cfg[rtag]['value']:
                            updated = True
                            net_cfg[rtag]['value'] = value
        transpose = [(':', '\:'), ('/', '\/')]
        sed = 'sed '
        for tag in net_cfg:
            tv = ''
            for c in net_cfg[tag]['value']:
                for t in transpose:
                    if c == t[0]:
                        tv += t[1]
                        c = None
                        break
                if c is not None:
                    tv += c
            sed += "-e 's/@%s@/%s/' " % (tag, tv)
        bld(target="testsuite/include/rtems/bsd/test/network-config.h",
            source="testsuite/include/rtems/bsd/test/network-config.h.in",
            rule=sed + " < ${SRC} > ${TGT}",
            update_outputs=True)

        #
        # Add a copy rule for all headers where the install path and the source
        # path are not the same.
        #
        if 'header-paths' in config:
            header_build_copy_paths = [
                hp for hp in config['header-paths']
                if hp[2] != '' and not hp[0].endswith(hp[2])
            ]
            for headers in header_build_copy_paths:
                target = os.path.join(buildinclude, headers[2])
                start_dir = bld.path.find_dir(headers[0])
                for header in start_dir.ant_glob(headers[1]):
                    relsourcepath = header.path_from(start_dir)
                    targetheader = os.path.join(target, relsourcepath)
                    bld(features='subst',
                        target=targetheader,
                        source=header,
                        is_copy=True)

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
        bld(rule=rtems_libbsd_modules_h_gen,
            target=modules_h_file_with_path,
            before=['c', 'cxx'])

        #
        # Add the specific rule based builders
        #

        #
        # KVM Symbols
        #
        if 'KVMSymbols' in self.data:
            kvmsymbols = self.data['KVMSymbols']['kernel']
            if 'includes' in kvmsymbols['files']:
                kvmsymbols_includes = kvmsymbols['files']['includes']
            else:
                kvmsymbols_includes = []
            bld(target=kvmsymbols['files']['all']['default'][0],
                source='rtemsbsd/rtems/generate_kvm_symbols',
                rule=host_shell + './${SRC} > ${TGT}',
                update_outputs=True)
            bld.objects(target='kvmsymbols',
                        features='c',
                        cflags=cflags,
                        includes=kvmsymbols_includes + includes['kernel'] + includes['bsp'],
                        source=kvmsymbols['files']['all']['default'][0])
            libbsd_use += ["kvmsymbols"]

        bld.add_group()

        #
        # RPC Generation
        #
        if 'RPCGen' in self.data:
            if bld.env.AUTO_REGEN:
                rpcgen = self.data['RPCGen']['user']
                rpcname = rpcgen['files']['all']['default'][0][:-2]
                bld(target=rpcname + '.h',
                    source=rpcname + '.x',
                    rule=host_shell + '${RPCGEN} -h -o ${TGT} ${SRC}')

        #
        # Route keywords
        #
        if 'RouteKeywords' in self.data:
            if bld.env.AUTO_REGEN:
                routekw = self.data['RouteKeywords']['user']
                rkwname = routekw['files']['all']['default'][0]
                rkw_rule = host_shell + "cat ${SRC} | " + \
                           "awk 'BEGIN { r = 0 } { if (NF == 1) " + \
                           "printf \"#define\\tK_%%s\\t%%d\\n\\t{\\\"%%s\\\", K_%%s},\\n\", " + \
                           "toupper($1), ++r, $1, toupper($1)}' > ${TGT}"
                bld(target=rkwname + '.h', source=rkwname, rule=rkw_rule)

        #
        # Lex
        #
        if 'lex' in self.data:
            lexes = self.data['lex']['user']
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
                    bld(target=lex['file'][:-2] + '.c',
                        source=lex['file'],
                        rule=lex_rule)
                if lex['build']:
                    bld.objects(target='lex_%s' % (lex['sym']),
                                features='c',
                                cflags=cflags,
                                includes=lexIncludes + includes['user'] + includes['bsp'],
                                defines=defines + lexDefines,
                                source=lex['file'][:-2] + '.c')
                libbsd_use += ['lex_%s' % (lex['sym'])]

        #
        # Yacc
        #
        if 'yacc' in self.data:
            yaccs = self.data['yacc']['user']
            for y in sorted(yaccs.keys()):
                yacc = yaccs[y]['all']['default']
                yaccFile = yacc['file']
                if yacc['sym'] is not None:
                    yaccSym = yacc['sym']
                else:
                    yaccSym = os.path.basename(yaccFile)[:-2]
                yaccHeader = '%s/%s' % (os.path.dirname(yaccFile),
                                        yacc['header'])
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
                            'sed -e \'/YY_BUF_SIZE/s/16384/1024/\' < ' + \
                            yaccSym + '.tab.c > ${TGT} && ' + \
                            'rm -f ' + yaccSym + '.tab.c && mv ' + yaccSym + '.tab.h ' + yaccHeader
                if bld.env.AUTO_REGEN:
                    bld(target=yaccFile[:-2] + '.c',
                        source=yaccFile,
                        rule=yacc_rule)
                if yacc['build']:
                    bld.objects(target='yacc_%s' % (yaccSym),
                                features='c',
                                cflags=cflags,
                                includes=yaccIncludes + includes['user'] + includes['bsp'],
                                defines=defines + yaccDefines,
                                source=yaccFile[:-2] + '.c')
                libbsd_use += ['yacc_%s' % (yaccSym)]

        #
        # We have 'm' different sets of flags and there can be 'n' cpus
        # specific files for those flags.
        #
        objs = 0
        for space in sorted(self.data['sources']):
            sources = sorted(self.data['sources'][space])
            if space == 'kernel' and 'default' in sources:
                sources.remove('default')
            for flags in sources:
                objs += 1
                build = self.data['sources'][space][flags]
                target = 'objs%02d' % (objs)
                bld_sources = Builder._sourceList(bld, build['all'])
                archs = sorted(build)
                for i in ['all', 'cflags', 'includes']:
                    if i in archs:
                        archs.remove(i)
                for arch in archs:
                    if bld.get_env()['RTEMS_ARCH'] == arch:
                        bld_sources += Builder._sourceList(bld, build[arch])
                bld_cflags = sorted(build.get('cflags', []))
                if 'default' in bld_cflags:
                    bld_cflags.remove('default')
                bld.objects(target=target,
                            features='c cxx',
                            cflags=cflags + bld_cflags,
                            cxxflags=cxxflags,
                            includes=sorted(build.get('includes', [])) +
                            includes[space] + includes['bsp'],
                            defines=defines,
                            source=bld_sources)
                libbsd_use += [target]

        #
        # We hold the kernel 'default' cflags set of files to the end to
        # create the static library with.
        #
        build = self.data['sources']['kernel']['default']
        bld_sources = Builder._sourceList(bld, build['all'])
        archs = sorted(build)
        archs.remove('all')
        for arch in archs:
            if bld.get_env()['RTEMS_ARCH'] == arch:
                bld_sources += Builder._sourceList(bld, build[arch])
        bld.stlib(target='bsd',
                  features='c cxx',
                  cflags=cflags,
                  cxxflags=cxxflags,
                  includes=includes['kernel'] + includes['bsp'],
                  defines=defines,
                  source=bld_sources,
                  use=libbsd_use)

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
            for headers in headerPaths:
                paths = [headers[0].replace('@CPU@', cpu)]
                # Apply the path mappings
                for source, targets in config['path-mappings']:
                    if source in paths:
                        i = paths.index(source)
                        paths.remove(source)
                        paths[i:i] = targets

                for hp in paths:
                    # Get the dest path
                    ipath = os.path.join(arch_inc_path, headers[2])
                    start_dir = bld.path.find_dir(hp)
                    if start_dir != None:
                        bld.install_files("${PREFIX}/" + ipath,
                                        start_dir.ant_glob(headers[1]),
                                        cwd=start_dir,
                                        relative_trick=True)

        bld.install_files(os.path.join("${PREFIX}", arch_inc_path,
                                       module_header_path),
                          modules_h_file_with_path,
                          cwd=bld.path)

        #
        # Tests
        #
        tests = []
        if 'tests' in self.data:
            tests = self.data['tests']['user']
        enabled_modules = self.getEnabledModules()
        for testName in sorted(tests):
            test = tests[testName]['all']
            test_source = []
            libs = ['bsd', 'm', 'z', 'rtemstest']
            for cfg in test:
                if len(test[cfg]['modules']) == 0:
                    build_test = True
                else:
                    build_test = False
                    for mod in test[cfg]['modules']:
                        if mod in enabled_modules:
                            build_test = True
                            break
                if build_test and cfg != 'default':
                    for c in cfg.split(' '):
                        if not bld.env['HAVE_%s' % (c)]:
                            build_test = False
                            break
                if build_test:
                    test_sources = ['testsuite/%s/%s.c' % (testName, f) \
                                    for f in test[cfg]['files']]
                    libs = test[cfg]['libs'] + libs
            if build_test:
                bld.program(target='%s.exe' % (testName),
                            features='cprogram',
                            cflags=cflags,
                            includes=includes['user'] + includes['bsp'],
                            source=test_sources,
                            use=['bsd'],
                            lib=libs,
                            install_path=None)
