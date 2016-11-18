#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
# 
#  embedded brains GmbH
#  Dornierstr. 4
#  82178 Puchheim
#  Germany
#  <rtems@embedded-brains.de>
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
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

import argparse
import re

parser = argparse.ArgumentParser(
    description=(
        "Convert a binary firmware file to a rtems-libbsd firmware c file. "
        "Note that you have to convert the FreeBSD uu-encoded files first."
    ))

parser.add_argument(
    "name",
    help="Name of the firmware",
)
parser.add_argument(
    "fw_bin",
    help="Binary firmware file.",
    type=argparse.FileType("rb"),
)
parser.add_argument(
    "out",
    help="Output file.",
    type=argparse.FileType("w"),
)
parser.add_argument(
    "-l", "--license",
    help="License file. Will be formatted as a comment and put on top.",
    type=argparse.FileType("r"),
)

args = parser.parse_args()

if args.license is not None:
    args.out.write("/*\n")
    for line in args.license:
        args.out.write(" * " + line)
    args.out.write(" */\n")

name = args.name
cname = re.sub(r'[^0-9a-zA-Z]', "_", name)

args.out.write("#include <machine/rtems-bsd-kernel-space.h>\n")
args.out.write("#include <sys/types.h>\n")
args.out.write("#include <sys/kernel.h>\n")
args.out.write("#include <sys/firmware.h>\n")
args.out.write("\n")
args.out.write("static const unsigned char %s[] = {" % (cname))
count = 0
while True:
    c = args.fw_bin.read(1)
    if not c:
        break
    if count % 12 == 0:
        args.out.write("\n\t")
    count = count + 1
    args.out.write("0x%02x, " % ord(c))

args.out.write("\n};\n")
args.out.write("static const size_t %s_size = sizeof(%s);\n" % (cname, cname))
args.out.write("\n")
args.out.write("static void\n")
args.out.write("%s_sysinit(void)\n" % (cname))
args.out.write("{\n")
args.out.write("\tconst struct firmware *fp;\n")
args.out.write("\tfp = firmware_register(\"%s\",\n" % (name))
args.out.write("\t    %s,\n" % (cname))
args.out.write("\t    %s_size,\n" % (cname))
args.out.write("\t    1, NULL);\n")
args.out.write("\tBSD_ASSERT(fp != NULL);\n")
args.out.write("}\n")
args.out.write("SYSINIT(%s, SI_SUB_DRIVERS, SI_ORDER_ANY,\n" % (cname))
args.out.write("    %s_sysinit, NULL);\n" % (cname))

# vim: set ts=4 sw=4 et:
