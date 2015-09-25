#! /usr/bin/env python
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

# FreeBSD: http://svn.freebsd.org/base/releng/8.2/sys (revision 222485)

import os
import sys
import getopt

import builder
import makefile
import waf_generator
import libbsd

isForward = True
isEarlyExit = False
isOnlyMakefile = False

def usage():
    print "freebsd-to-rtems.py [args]"
    print "  -?|-h|--help     print this and exit"
    print "  -d|--dry-run     run program but no modifications"
    print "  -D|--diff        provide diff of files between trees"
    print "  -e|--early-exit  evaluate arguments, print results, and exit"
    print "  -m|--makefile    just generate Makefile"
    print "  -R|--reverse     default FreeBSD -> RTEMS, reverse that"
    print "  -r|--rtems       RTEMS Libbsd directory (default: '.')"
    print "  -f|--freebsd     FreeBSD SVN directory (default: 'freebsd-org')"
    print "  -v|--verbose     enable verbose output mode"

# Parse the arguments
def parseArguments():
    global isForward, isEarlyExit
    global isOnlyMakefile
    try:
        opts, args = getopt.getopt(sys.argv[1:],
                                   "?hdDemRr:f:v",
                                   [ "help",
                                     "help",
                                     "dry-run"
                                     "diff"
                                     "early-exit"
                                     "makefile"
                                     "reverse"
                                     "rtems="
                                     "freebsd="
                                     "verbose" ])
    except getopt.GetoptError, err:
        # print help information and exit:
        print str(err) # will print something like "option -a not recognized"
        usage()
        sys.exit(2)
    for o, a in opts:
        if o in ("-v", "--verbose"):
            builder.isVerbose = True
        elif o in ("-h", "--help", "-?"):
            usage()
            sys.exit()
        elif o in ("-d", "--dry-run"):
            builder.isDryRun = True
        elif o in ("-D", "--diff"):
            builder.isDiffMode = True
        elif o in ("-e", "--early-exit"):
            isEarlyExit = True
        elif o in ("-m", "--makefile"):
            isOnlyMakefile = True
        elif o in ("-R", "--reverse"):
            isForward = False
        elif o in ("-r", "--rtems"):
            builder.RTEMS_DIR = a
        elif o in ("-f", "--freebsd"):
            builder.FreeBSD_DIR = a
        else:
            assert False, "unhandled option"

parseArguments()

print "Verbose:                " + ("no", "yes")[builder.isVerbose]
print "Dry Run:                " + ("no", "yes")[builder.isDryRun]
print "Diff Mode Enabled:      " + ("no", "yes")[builder.isDiffMode]
print "Only Generate Makefile: " + ("no", "yes")[isOnlyMakefile]
print "RTEMS Libbsd Directory: " + builder.RTEMS_DIR
print "FreeBSD SVN Directory:  " + builder.FreeBSD_DIR
print "Direction:              " + ("reverse", "forward")[isForward]

# Check directory argument was set and exist
def wasDirectorySet(desc, path):
    if path == "not_set":
        print "error:" + desc + " Directory was not specified on command line"
        sys.exit(2)

    if os.path.isdir( path ) != True:
        print "error:" + desc + " Directory (" + path + ") does not exist"
        sys.exit(2)

# Were RTEMS and FreeBSD directories specified
wasDirectorySet( "RTEMS", builder.RTEMS_DIR )
wasDirectorySet( "FreeBSD", builder.FreeBSD_DIR )

# Are we generating or reverting?
if isForward == True:
    print "Forward from FreeBSD GIT into ", builder.RTEMS_DIR
else:
    print "Reverting from ", builder.RTEMS_DIR
    if isOnlyMakefile == True:
        print "error: Makefile Mode and Reverse are contradictory"
        sys.exit(2)

if isEarlyExit == True:
    print "Early exit at user request"
    sys.exit(0)

try:
    makefile_gen = makefile.ModuleManager()
    waf_gen = waf_generator.ModuleManager()

    libbsd.sources(makefile_gen)
    libbsd.sources(waf_gen)

    # Perform the actual file manipulation
    if isForward:
        if not isOnlyMakefile:
            makefile_gen.copyFromFreeBSDToRTEMS()
        waf_gen.generate()
    else:
        makefile_gen.copyFromRTEMSToFreeBSD()
    # Print a summary if changing files
    if builder.isDiffMode == False:
        print '%d file(s) were changed.' % (builder.filesProcessed)
except IOError, ioe:
    print 'error: %s' % (ioe)
