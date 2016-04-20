#
# RTEMS Project (https://www.rtems.org/)
#
# Copyright (c) 2015-2016 Chris Johns <chrisj@rtems.org>. All rights reserved.
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

#
# RTEMS LibBSD is a transparent source build of the FreeBSD kernel source for RTEMS.
#
# To use see README.waf shipped with this file.
#

from __future__ import print_function

rtems_version = "4.12"

try:
    import rtems_waf.rtems as rtems
except:
    print("error: no rtems_waf git submodule; see README.waf")
    import sys
    sys.exit(1)

import libbsd_waf

def init(ctx):
    rtems.init(ctx, version = rtems_version)
    libbsd_waf.init(ctx)

def options(opt):
    rtems.options(opt)
    opt.add_option("--enable-auto-regen",
                   action = "store_true",
                   default = False,
                   dest = "auto_regen",
                   help = "Enable auto-regeneration of LEX, RPC and YACC files.")
    opt.add_option("--enable-warnings",
                   action = "store_true",
                   default = False,
                   dest = "warnings",
                   help = "Enable all warnings. The default is quiet builds.")
    opt.add_option("--net-test-config",
                   default = "config.inc",
                   dest = "net_config",
                   help = "Network test configuration.")
    libbsd_waf.options(opt)

def bsp_configure(conf, arch_bsp):
    conf.check(header_name = "dlfcn.h", features = "c")
    conf.check(header_name = "rtems/pci.h", features = "c", mandatory = False)
    if not rtems.check_posix(conf):
        conf.fatal("RTEMS kernel POSIX support is disabled; configure RTEMS with --enable-posix")
    if rtems.check_networking(conf):
        conf.fatal("RTEMS kernel contains the old network support; configure RTEMS with --disable-networking")
    libbsd_waf.bsp_configure(conf, arch_bsp)

def configure(conf):
    if conf.options.auto_regen:
        conf.find_program("lex", mandatory = True)
        conf.find_program("rpcgen", mandatory = True)
        conf.find_program("yacc", mandatory = True)
    conf.env.AUTO_REGEN = conf.options.auto_regen
    conf.env.WARNINGS = conf.options.warnings
    conf.env.NET_CONFIG = conf.options.net_config
    rtems.configure(conf, bsp_configure)
    libbsd_waf.configure(conf, arch_bsp)

def build(bld):
    rtems.build(bld)
    libbsd_waf.build(bld)
