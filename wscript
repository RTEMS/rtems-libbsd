# SPDX-License-Identifier: BSD-2-Clause
"""RTEMS LibBSD is a transparent source build of the FreeBSD kernel
source for RTEMS.

To use see README.waf shipped with this file.
"""

# Copyright (c) 2015-2016 Chris Johns <chrisj@rtems.org>. All rights reserved.
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

#
# RTEMS LibBSD is a transparent source build of the FreeBSD kernel source for RTEMS.
#
# To use see README.waf shipped with this file.
#

from __future__ import print_function

rtems_version = "6"

try:
    import rtems_waf.rtems as rtems
except:
    print("error: no rtems_waf git submodule; see README.waf")
    import sys
    sys.exit(1)

import os.path
import runpy
import sys

import waflib.Options

import libbsd
import waf_libbsd

builders = {}


def update_builders(ctx, buildset_opt):
    global builders
    builders = {}
    buildsets = []
    if buildset_opt == []:
        buildset_opt.append(waf_libbsd.BUILDSET_DEFAULT)
    for bs in buildset_opt:
        if os.path.isdir(bs):
            for f in os.listdir(bs):
                if f[-4:] == ".ini":
                    buildsets += [os.path.join(bs, f)]
        else:
            for f in bs.split(','):
                buildsets += [f]
    for bs in buildsets:
        try:
            builder = waf_libbsd.Builder()
            libbsd.load(builder)
            builder.loadConfig(bs)
            builder.generate(rtems_version)
        except Exception as exc:
            raise
            ctx.fatal(str(exc))
        builders[builder.getName()] = builder


def bsp_init(ctx, env, contexts):
    # This function generates the builders and adds build-xxx, clean-xxx and
    # install-xxx targets for them.

    if not 'buildset' in env.options:
        # This happens if 'waf configure' hasn't been executed. In that case we
        # create the builders during the configure phase. After the first time
        # 'waf configure' is executed 'buildset' is read from the .lock_xxx
        # file. In that case the builders are overwritten during configure
        # phase. This is not really the cleanest solution but it works.
        return

    update_builders(ctx, env.options['buildset'])
    for builder in builders:
        # Update the contextes for build variants
        for y in contexts:
            newcmd = y.cmd + '-' + builder
            newvariant = y.variant + '-' + builder

            class context(y):
                cmd = str(newcmd)
                variant = str(newvariant)
                libbsd_buildset_name = builder

    # Transform the commands to per build variant commands
    commands = []
    for cmd in waflib.Options.commands:
        if cmd.startswith(('build', 'clean', 'install')):
            for builder in builders:
                commands += [str(cmd + '-' + builder)]
        else:
            commands += [str(cmd)]
    waflib.Options.commands = commands


def init(ctx):
    rtems.init(ctx,
               version=rtems_version,
               long_commands=True,
               bsp_init=bsp_init)


def options(opt):
    rtems.options(opt)
    opt.add_option("--enable-auto-regen",
                   action="store_true",
                   default=False,
                   dest="auto_regen",
                   help="Enable auto-regeneration of LEX, RPC and YACC files.")
    opt.add_option("--enable-warnings",
                   action="store_true",
                   default=False,
                   dest="warnings",
                   help="Enable all warnings. The default is quiet builds.")
    opt.add_option("--net-test-config",
                   default="config.inc",
                   dest="net_config",
                   help="Network test configuration.")
    opt.add_option("--freebsd-options",
                   action="store",
                   default="",
                   dest="freebsd_options",
                   help="Set FreeBSD options (developer option). Supported: " + \
                   "bootverbose,verbose_sysinit,debug_locks,ktr,ktr_verbose," + \
                   "rtems_bsd_descrip_trace,rtems_bsd_syscall_trace," +\
                   "rtems_bsd_vfs_trace,netlink_verbose,invariants," +\
                   "invariant_support")
    opt.add_option(
        "--optimization",
        action="store",
        default="2",
        dest="optimization",
        help=
        "Set optimization level to OPTIMIZATION (-On compiler flag). Default is 2 (-O2)."
    )
    opt.add_option(
        "--buildset",
        action="append",
        default=[],
        dest="buildset",
        help=
        "Select build sets to build. If set to a directory," \
        " all .ini file in this directory will be used."
    )


def bsp_configure(conf, arch_bsp):
    conf.check(header_name="dlfcn.h", features="c")
    conf.check(header_name="rtems/pci.h", features="c", mandatory=False)
    if rtems.check_networking(conf):
        conf.fatal(
            "RTEMS kernel contains the old network support;" \
            " configure RTEMS with --disable-networking"
        )
    env = conf.env.derive()
    for builder in builders:
        ab = conf.env.RTEMS_ARCH_BSP
        variant = ab + "-" + builder
        conf.msg('Configure variant: ', variant)
        conf.setenv(variant, env)
        builders[builder].bsp_configure(conf, arch_bsp)
        conf.setenv(ab)


def configure(conf):
    if conf.options.auto_regen:
        conf.find_program("lex", mandatory=True)
        conf.find_program("rpcgen", mandatory=True)
        conf.find_program("yacc", mandatory=True)
    conf.env.AUTO_REGEN = conf.options.auto_regen
    conf.env.WARNINGS = conf.options.warnings
    conf.env.NET_CONFIG = conf.options.net_config
    conf.env.FREEBSD_OPTIONS = conf.options.freebsd_options
    conf.env.OPTIMIZATION = conf.options.optimization
    conf.env.BUILDSET = conf.options.buildset
    if len(conf.env.BUILDSET) == 0:
        conf.env.BUILDSET += [waf_libbsd.BUILDSET_DEFAULT]
    update_builders(conf, conf.env.BUILDSET)
    rtems.configure(conf, bsp_configure)


def test(bld):
    rtems.test_uninstall(bld)


def build(bld):
    rtems.build(bld)
    builders[bld.libbsd_buildset_name].build(bld)
