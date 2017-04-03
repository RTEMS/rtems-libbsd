#! /usr/bin/env python
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

# FreeBSD: http://svn.freebsd.org/base/releng/8.2/sys (revision 222485)

from __future__ import print_function

import os
import sys
import getopt

import builder
import waf_generator
import libbsd

isForward = True
isEarlyExit = False
isOnlyBuildScripts = False
statsReport = False

def usage():
    print("freebsd-to-rtems.py [args]")
    print("  -?|-h|--help      print this and exit")
    print("  -d|--dry-run      run program but no modifications")
    print("  -D|--diff         provide diff of files between trees")
    print("  -e|--early-exit   evaluate arguments, print results, and exit")
    print("  -m|--makefile     Warning: depreciated and will be removed ")
    print("  -b|--buildscripts just generate the build scripts")
    print("  -S|--stats        Print a statistics report")
    print("  -R|--reverse      default origin -> LibBSD, reverse that")
    print("  -r|--rtems        LibBSD directory (default: '.')")
    print("  -f|--freebsd      FreeBSD origin directory (default: 'freebsd-org')")
    print("  -v|--verbose      enable verbose output mode")

# Parse the arguments
def parseArguments():
    global isForward, isEarlyExit, statsReport
    global isOnlyBuildScripts
    try:
        opts, args = getopt.getopt(sys.argv[1:],
                                   "?hdDembSRr:f:v",
                                   [ "help",
                                     "help",
                                     "dry-run"
                                     "diff"
                                     "early-exit"
                                     "makefile"
                                     "buildscripts"
                                     "reverse"
                                     "stats"
                                     "rtems="
                                     "freebsd="
                                     "verbose" ])
    except getopt.GetoptError as err:
        # print help information and exit:
        print(str(err)) # will print something like "option -a not recognized"
        usage()
        sys.exit(2)
    for o, a in opts:
        if o in ("-v", "--verbose"):
            builder.verboseLevel += 1
        elif o in ("-h", "--help", "-?"):
            usage()
            sys.exit()
        elif o in ("-d", "--dry-run"):
            builder.isDryRun = True
        elif o in ("-D", "--diff"):
            builder.isDiffMode = True
        elif o in ("-e", "--early-exit"):
            isEarlyExit = True
        elif o in ("-b", "--buildscripts") or o in ("-m", "--makefile"):
            isOnlyBuildScripts = True
        elif o in ("-S", "--stats"):
            statsReport = True
        elif o in ("-R", "--reverse"):
            isForward = False
        elif o in ("-r", "--rtems"):
            builder.LIBBSD_DIR = a
        elif o in ("-f", "--freebsd"):
            builder.FreeBSD_DIR = a
        else:
            assert False, "unhandled option"

parseArguments()

print("Verbose:                     %s (%d)" % (("no", "yes")[builder.verbose()],
                                                builder.verboseLevel))
print("Dry Run:                     %s" % (("no", "yes")[builder.isDryRun]))
print("Diff Mode Enabled:           %s" % (("no", "yes")[builder.isDiffMode]))
print("Only Generate Build Scripts: %s" % (("no", "yes")[isOnlyBuildScripts]))
print("LibBSD Directory:            %s" % (builder.LIBBSD_DIR))
print("FreeBSD Directory:           %s" % (builder.FreeBSD_DIR))
print("Linux Directory:             %s" % (builder.Linux_DIR))
print("Direction:                   %s" % (("reverse", "forward")[isForward]))

# Check directory argument was set and exist
def wasDirectorySet(desc, path):
    if path == "not_set":
        print("error:" + desc + " Directory was not specified on command line")
        sys.exit(2)

    if os.path.isdir( path ) != True:
        print("error:" + desc + " Directory (" + path + ") does not exist")
        sys.exit(2)

# Were directories specified?
wasDirectorySet( "LibBSD", builder.LIBBSD_DIR )
wasDirectorySet( "FreeBSD", builder.FreeBSD_DIR )
wasDirectorySet( "Linux", builder.Linux_DIR )

# Are we generating or reverting?
if isForward == True:
    print("Forward from", builder.FreeBSD_DIR, "and", builder.Linux_DIR, "into", builder.LIBBSD_DIR)
else:
    print("Reverting from", builder.LIBBSD_DIR)
    if isOnlyBuildScripts == True:
        print("error: Build Script generation and Reverse are contradictory")
        sys.exit(2)

if isEarlyExit == True:
    print("Early exit at user request")
    sys.exit(0)

try:
    wafGen = waf_generator.ModuleManager()
    libbsd.sources(wafGen)
    if not isOnlyBuildScripts:
        wafGen.processSource(isForward)
    wafGen.generate(libbsd.rtems_version())
    builder.changedFileSummary(statsReport)
except IOError as ioe:
    print('error: %s' % (str(ioe)))
except builder.error as be:
    print('error: %s' % (be))
except KeyboardInterrupt:
    print('user abort')
