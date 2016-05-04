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

import shutil
import os
import re
import sys
import getopt
import filecmp
import difflib
import codecs

#
# Global controls.
#
RTEMS_DIR = "."
FreeBSD_DIR = "freebsd-org"
verboseLevel = 0
isDryRun = False
isDiffMode = False
filesProcessedCount = 0
filesProcessed = []

verboseInfo = 1
verboseDetail = 2
verboseMoreDetail = 3
verboseDebug = 4

def verbose(level = verboseInfo):
    return verboseLevel >= level

def changedFileSummary():
    if isDiffMode == False:
        print('%d file(s) were changed:' % (filesProcessedCount))
        if verbose():
            for f in sorted(filesProcessed):
                print(' %s' % (f))

def read_file(name):
    try:
        contents = codecs.open(name, mode = 'r', encoding = 'utf-8', errors = 'ignore').read()
    except UnicodeDecodeError as ude:
        print('error: reading: %s: %s' % (name, ude))
        sys.exit(1)
    return contents

def write_file(name, contents):
    path = os.path.dirname(name)
    if not os.path.exists(path):
        try:
            os.makedirs(path)
        except OSError as oe:
            print('error: cannot create directory: %s: %s' % (dst_path, oe))
            sys.exit(1)
    try:
        codecs.open(name, mode = 'w',  encoding = 'utf-8', errors = 'ignore').write(contents)
    except UnicodeDecodeError as ude:
        print('error: write: %s: %s' % (name, ude))
        sys.exit(1)

#
# A builder error.
#
class error(Exception):
    """Base class for exceptions."""
    def __init__(self, msg):
        self.msg = 'error: %s' % (msg)
    def set_output(self, msg):
        self.msg = msg
    def __str__(self):
        return self.msg

#
# This stuff needs to move to libbsd.py.
#
def common_flags():
    return ['-O2',
            '-g',
            '-fno-strict-aliasing',
            '-ffreestanding',
            '-fno-common']

def common_warnings():
    return ['-Wall',
            '-Wno-format']

def common_no_warnings():
    return ['-w']

def includes():
    return ['-Irtemsbsd/include',
            '-Ifreebsd/sys',
            '-Ifreebsd/sys/contrib/altq',
            '-Ifreebsd/sys/contrib/pf',
            '-Ifreebsd/include',
            '-Ifreebsd/lib/libc/include',
            '-Ifreebsd/lib/libc/isc/include',
            '-Ifreebsd/lib/libc/resolv',
            '-Ifreebsd/lib/libutil',
            '-Ifreebsd/lib/libkvm',
            '-Ifreebsd/lib/libmemstat',
            '-Ifreebsd/lib/libipsec',
            '-Ifreebsd/contrib/libpcap',
            '-Irtemsbsd/sys',
            '-ImDNSResponder/mDNSCore',
            '-ImDNSResponder/mDNSShared',
            '-ImDNSResponder/mDNSPosix',
            '-Itestsuite/include']

def cpu_includes():
    return ['-Irtemsbsd/@CPU@/include',
            '-Ifreebsd/sys/@CPU@/include']

def cflags():
    return ['-std=gnu11']

def cxxflags():
    return ['-std=gnu++11']

def header_paths():
    #         local path                      wildcard             dest path
    return [('rtemsbsd/include',              '*.h',               ''),
            ('rtemsbsd/mghttpd',              'mongoose.h',        'mghttpd'),
            ('freebsd/include',               '*.h',               ''),
            ('freebsd/sys/contrib/altq/altq', '*.h',               'altq'),
            ('freebsd/sys/bsm',               '*.h',               'bsm'),
            ('freebsd/sys/cam',               '*.h',               'cam'),
            ('freebsd/sys/net',               '*.h',               'net'),
            ('freebsd/sys/net80211',          '*.h',               'net80211'),
            ('freebsd/sys/netatalk',          '*.h',               'netatalk'),
            ('freebsd/sys/netinet',           '*.h',               'netinet'),
            ('freebsd/sys/netinet6',          '*.h',               'netinet6'),
            ('freebsd/sys/netipsec',          '*.h',               'netipsec'),
            ('freebsd/sys/rpc',               '*.h',               'rpc'),
            ('freebsd/sys/sys',               '*.h',               'sys'),
            ('freebsd/sys/vm',                '*.h',               'vm'),
            ('freebsd/sys/dev/mii',           '*.h',               'dev/mii'),
            ('mDNSResponder/mDNSCore',        'mDNSDebug.h',       ''),
            ('mDNSResponder/mDNSCore',        'mDNSEmbeddedAPI.h', ''),
            ('mDNSResponder/mDNSShared',      'dns_sd.h',          ''),
            ('mDNSResponder/mDNSPosix',       'mDNSPosix.h',       '')]

# Move target dependent files under a machine directory
def mapCPUDependentPath(path):
  return path.replace("include/", "include/machine/")

def fixIncludes(data):
    data = re.sub('#include <sys/lock.h>',     '#include <rtems/bsd/sys/lock.h>', data)
    data = re.sub('#include <sys/time.h>',     '#include <rtems/bsd/sys/time.h>', data)
    data = re.sub('#include <sys/cpuset.h>',   '#include <rtems/bsd/sys/cpuset.h>', data)
    data = re.sub('#include <sys/errno.h>',    '#include <rtems/bsd/sys/errno.h>', data)
    data = re.sub('#include <sys/param.h>',    '#include <rtems/bsd/sys/param.h>', data)
    data = re.sub('#include <sys/types.h>',    '#include <rtems/bsd/sys/types.h>', data)
    data = re.sub('#include <sys/resource.h>', '#include <rtems/bsd/sys/resource.h>', data)
    data = re.sub('#include <sys/unistd.h>',   '#include <rtems/bsd/sys/unistd.h>', data)
    data = re.sub('#include <sys/_types.h>',   '#include <rtems/bsd/sys/_types.h>', data)
    return data

# revert fixing the include paths inside a C or .h file
def revertFixIncludes(data):
    data = re.sub('#include <rtems/bsd/',  '#include <', data)
    data = re.sub('#include <util.h>',     '#include <rtems/bsd/util.h>', data)
    data = re.sub('#include <bsd.h>',      '#include <rtems/bsd/bsd.h>', data)
    data = re.sub('#include <zerocopy.h>', '#include <rtems/bsd/zerocopy.h>', data)
    return data

# fix include paths inside a C or .h file
def fixLocalIncludes(data):
    data = re.sub('#include "opt_([^"]*)"',    '#include <rtems/bsd/local/opt_\\1>', data)
    data = re.sub('#include "([^"]*)_if.h"',   '#include <rtems/bsd/local/\\1_if.h>', data)
    data = re.sub('#include "miidevs([^"]*)"', '#include <rtems/bsd/local/miidevs\\1>', data)
    data = re.sub('#include "usbdevs([^"]*)"', '#include <rtems/bsd/local/usbdevs\\1>', data)
    return data

# revert fixing the include paths inside a C or .h file
def revertFixLocalIncludes(data):
    data = re.sub('#include <rtems/bsd/local/([^>]*)>', '#include "\\1"', data)
    return data

def assertHeaderFile(path):
    if path[-2] != '.' or path[-1] != 'h':
        print("*** " + path + " does not end in .h")
        print("*** Move it to a C source file list")
        sys.exit(2)

def assertSourceFile(path):
    if path[-2] != '.' or (path[-1] != 'c' and path[-1] != 'S'):
        print("*** " + path + " does not end in .c")
        print("*** Move it to a header file list")
        sys.exit(2)

#
# Converters provide a way to alter the various types of code. The conversion
# process filters a file as it is copies from the source path to the
# destination path. Specialised versions are provided for different types of
# source.
#
class Converter(object):

    def convert(self, src, dst, has_source = True, source_filter = None, src_contents = None):

        global filesProcessed, filesProcessedCount

        if verbose(verboseDebug):
            print("convert: filter:%s: %s -> %s" % \
                  (['yes', 'no'][source_filter is None], src, dst))

        #
        # If there is no source raise an error if we expect source else print a
        # warning and do not try and convert.
        #
        if src_contents is None:
            if not os.path.exists(src):
                if has_source:
                    raise error('source not found: %s' % (src))
                else:
                    print('warning: no source: %s' % (src))
                    return

            #
            # Files read as a single string if not passed in.
            #
            src_contents = read_file(src)

        if os.path.exists(dst):
            dst_contents = read_file(dst)
        else:
            print('warning: no destination: %s' % (dst))
            dst_contents = ''

        #
        # Filter the source.
        #
        if source_filter is not None:
            src_contents = source_filter(src_contents)

        #
        # Split into a list of lines.
        #
        src_lines = src_contents.split(os.linesep)
        dst_lines = dst_contents.split(os.linesep)

        if verbose(verboseDebug):
            print('Unified diff: %s (lines:%d)' % (src, len(src_lines)))

        #
        # Diff, note there is no line termination on each string.  Expand the
        # generator to list because the generator is not reusable.
        #
        diff = list(difflib.unified_diff(dst_lines,
                                         src_lines,
                                         fromfile = src,
                                         tofile = dst,
                                         n = 5,
                                         lineterm = ''))

        #
        # The diff list is empty if the files are the same.
        #
        if len(diff) > 0:

            if verbose(verboseDebug):
                print('Unified diff length: %d' % len(diff))

            filesProcessed += [dst]
            filesProcessedCount += 1
            if isDiffMode == False:
                if verbose(verboseDetail):
                    print("UPDATE: %s -> %s" % (src, dst))
                if isDryRun == False:
                    write_file(dst, src_contents)
            else:
                print("diff -u %s %s" % (src, dst))
                for l in diff:
                    print(l)

class NoConverter(Converter):
    def convert(self, src, dst, has_source = True, source_filter = None):
        return '/* EMPTY */\n'

class FromFreeBSDToRTEMSHeaderConverter(Converter):
    def source_filter(self, data):
        data = fixLocalIncludes(data)
        data = fixIncludes(data)
        return data

    def convert(self, src, dst):
        sconverter = super(FromFreeBSDToRTEMSHeaderConverter, self)
        sconverter.convert(src, dst, source_filter = self.source_filter)

class FromFreeBSDToRTEMSUserSpaceHeaderConverter(Converter):
    def source_filter(self, data):
        data = fixIncludes(data)
        return data

    def convert(self, src, dst):
        sconverter = super(FromFreeBSDToRTEMSUserSpaceHeaderConverter, self)
        sconverter.convert(src, dst, source_filter = self.source_filter)

class FromFreeBSDToRTEMSSourceConverter(Converter):
    def source_filter(self, data):
        data = fixLocalIncludes(data)
        data = fixIncludes(data)
        data = '#include <machine/rtems-bsd-kernel-space.h>\n\n' + data
        return data

    def convert(self, src, dst):
        sconverter = super(FromFreeBSDToRTEMSSourceConverter, self)
        sconverter.convert(src, dst, source_filter = self.source_filter)

class FromFreeBSDToRTEMSUserSpaceSourceConverter(Converter):
    def source_filter(self, data):
        data = fixIncludes(data)
        data = '#include <machine/rtems-bsd-user-space.h>\n\n' + data
        return data

    def convert(self, src, dst):
        sconverter = super(FromFreeBSDToRTEMSUserSpaceSourceConverter, self)
        sconverter.convert(src, dst, source_filter = self.source_filter)

class FromRTEMSToFreeBSDHeaderConverter(Converter):
    def source_filter(self, data):
        data = revertFixLocalIncludes(data)
        data = revertFixIncludes(data)
        return data

    def convert(self, src, dst):
        sconverter = super(FromRTEMSToFreeBSDHeaderConverter, self)
        sconverter.convert(src, dst, has_source = False,  source_filter = self.source_filter)

class FromRTEMSToFreeBSDSourceConverter(Converter):
    def source_filter(self, data):
        data = re.sub('#include <machine/rtems-bsd-kernel-space.h>\n\n', '', data)
        data = re.sub('#include <machine/rtems-bsd-user-space.h>\n\n', '', data)
        return data

    def convert(self, src, dst):
        sconverter = super(FromRTEMSToFreeBSDSourceConverter, self)
        sconverter.convert(src, dst, has_source = False, source_filter = self.source_filter)

#
# Compose a path based for the various parts of the source tree.
#
class PathComposer(object):
    def composeFreeBSDPath(self, path):
        return path

    def composeRTEMSPath(self, path, prefix):
        return os.path.join(prefix, path)

class FreeBSDPathComposer(PathComposer):
    def composeFreeBSDPath(self, path):
        return os.path.join(FreeBSD_DIR, path)

    def composeRTEMSPath(self, path, prefix):
        return os.path.join(prefix, 'freebsd', path)

class RTEMSPathComposer(PathComposer):
    def composeFreeBSDPath(self, path):
        return path

    def composeRTEMSPath(self, path, prefix):
        return os.path.join(prefix, 'rtemsbsd', path)

class CPUDependentPathComposer(FreeBSDPathComposer):
    def composeRTEMSPath(self, path, prefix):
        path = super(CPUDependentPathComposer, self).composeRTEMSPath(path, prefix)
        path = mapCPUDependentPath(path)
        return path

class TargetSourceCPUDependentPathComposer(CPUDependentPathComposer):
    def __init__(self, targetCPU, sourceCPU):
        self.targetCPU = targetCPU
        self.sourceCPU = sourceCPU

    def composeRTEMSPath(self, path, prefix):
        path = super(TargetSourceCPUDependentPathComposer, self).composeRTEMSPath(path, prefix)
        path = path.replace(self.sourceCPU, self.targetCPU)
        return path

class BuildSystemFragmentComposer(object):
    def __init__(self, includes = None):
        if type(includes) is not list:
            self.includes = [includes]
        else:
            self.includes = includes

    def compose(self, path):
        return ''

#
# File - a file in the source we move backwards and forwards.
#
class File(object):
    def __init__(self, path, pathComposer,
                 forwardConverter, reverseConverter, buildSystemComposer):
        if verbose(verboseMoreDetail):
            print("FILE: %-50s F:%-45s R:%-45s" % \
                  (path,
                   forwardConverter.__class__.__name__,
                   reverseConverter.__class__.__name__))
        self.path = path
        self.pathComposer = pathComposer
        self.freebsdPath = self.pathComposer.composeFreeBSDPath(self.path)
        self.rtemsPath = self.pathComposer.composeRTEMSPath(self.path, RTEMS_DIR)
        self.forwardConverter = forwardConverter
        self.reverseConverter = reverseConverter
        self.buildSystemComposer = buildSystemComposer

    def processSource(self, forward):
        if forward:
            if verbose(verboseDetail):
                print("process source: %s => %s" % (self.freebsdPath, self.rtemsPath))
            self.forwardConverter.convert(self.freebsdPath, self.rtemsPath)
        else:
            if verbose(verboseDetail):
                print("process source: %s => %s converter:%s" % \
                      (self.rtemsPath, self.freebsdPath, self.reverseConverter.__class__.__name__))
            self.reverseConverter.convert(self.rtemsPath, self.freebsdPath)

    def getFragment(self):
        return self.buildSystemComposer.compose(self.pathComposer.composeRTEMSPath(self.path, ''))

#
# Module - logical group of related files we can perform actions on
#
class Module:
    def __init__(self, name):
        self.name = name
        self.conditionalOn = "none"
        self.files = []
        self.cpuDependentSourceFiles = {}
        self.dependencies = []

    def initCPUDependencies(self, cpu):
        if cpu not in self.cpuDependentSourceFiles:
            self.cpuDependentSourceFiles[cpu] = []

    def processSource(self, direction):
        if verbose(verboseDetail):
            print("process module: %s" % (self.name))
        for f in self.files:
            f.processSource(direction)
        for cpu, files in self.cpuDependentSourceFiles.items():
            for f in files:
                f.processSource(direction)

    def addFiles(self, newFiles, buildSystemComposer = BuildSystemFragmentComposer()):
        files = []
        for newFile in newFiles:
            assertFile(newFile)
            files += [File(newFile, composers, buildSystemComposer)]
        return files

    def addFile(self, f):
        self.files += [f]

    def addFiles(self, newFiles,
                 pathComposer, fromFreeBSDToRTEMSConverter, fromRTEMSToFreeBSDConverter,
                 assertFile, buildSystemComposer = BuildSystemFragmentComposer()):
        files = []
        for newFile in newFiles:
            assertFile(newFile)
            files += [File(newFile, pathComposer, fromFreeBSDToRTEMSConverter,
                           fromRTEMSToFreeBSDConverter, buildSystemComposer)]
        return files

    def addKernelSpaceHeaderFiles(self, files):
        self.files += self.addFiles(files,
                                    FreeBSDPathComposer(), FromFreeBSDToRTEMSHeaderConverter(),
                                    FromRTEMSToFreeBSDHeaderConverter(), assertHeaderFile)

    def addUserSpaceHeaderFiles(self, files):
        self.files += self.addFiles(files,
                                    FreeBSDPathComposer(), FromFreeBSDToRTEMSUserSpaceHeaderConverter(),
                                    FromRTEMSToFreeBSDHeaderConverter(), assertHeaderFile)

    def addRTEMSHeaderFiles(self, files):
        self.files += self.addFiles(files, RTEMSPathComposer(),
                                    NoConverter(), NoConverter(), assertHeaderFile)

    def addCPUDependentHeaderFiles(self, files):
        self.files += self.addFiles(files,
                                    CPUDependentPathComposer(), FromFreeBSDToRTEMSHeaderConverter(),
                                    FromRTEMSToFreeBSDHeaderConverter(), assertHeaderFile)

    def addTargetSourceCPUDependentHeaderFiles(self, targetCPUs, sourceCPU, files):
        for cpu in targetCPUs:
            self.files += self.addFiles(files,
                                        TargetSourceCPUDependentPathComposer(cpu, sourceCPU),
                                        FromFreeBSDToRTEMSHeaderConverter(),
                                        NoConverter(), assertHeaderFile)

    def addSourceFiles(self, files, sourceFileFragmentComposer):
        self.files += self.addFiles(files,
                                    PathComposer(), NoConverter(), NoConverter(), assertSourceFile,
                                    sourceFileFragmentComposer)

    def addKernelSpaceSourceFiles(self, files, sourceFileFragmentComposer):
        self.files += self.addFiles(files,
                                    FreeBSDPathComposer(), FromFreeBSDToRTEMSSourceConverter(),
                                    FromRTEMSToFreeBSDSourceConverter(), assertSourceFile,
                                    sourceFileFragmentComposer)

    def addUserSpaceSourceFiles(self, files, sourceFileFragmentComposer):
        self.files += self.addFiles(files,
                                    FreeBSDPathComposer(),
                                    FromFreeBSDToRTEMSUserSpaceSourceConverter(),
                                    FromRTEMSToFreeBSDSourceConverter(), assertSourceFile,
                                    sourceFileFragmentComposer)

    def addRTEMSSourceFiles(self, files, sourceFileFragmentComposer):
        self.files += self.addFiles(files,
                                    RTEMSPathComposer(), NoConverter(), NoConverter(),
                                    assertSourceFile, sourceFileFragmentComposer)

    def addCPUDependentSourceFiles(self, cpus, files, sourceFileFragmentComposer):
        for cpu in cpus:
            self.initCPUDependencies(cpu)
            self.cpuDependentSourceFiles[cpu] += \
                self.addFiles(files,
                              CPUDependentPathComposer(), FromFreeBSDToRTEMSSourceConverter(),
                              FromRTEMSToFreeBSDSourceConverter(), assertSourceFile,
                              sourceFileFragmentComposer)

    def addTest(self, testFragementComposer):
        self.files += [File(testFragementComposer.testName,
                            PathComposer(), NoConverter(), NoConverter(),
                            testFragementComposer)]

    def addDependency(self, dep):
        self.dependencies += [dep]

#
# Manager - a collection of modules.
#
class ModuleManager:
    def __init__(self):
        self.modules = {}
        self.generator = {}
        self.setGenerators()

    def __getitem__(self, key):
        if key not in self.modules:
            raise KeyError('module %s not found' % (key))
        return self.modules[key]

    def getModules(self):
        return sorted(self.modules.keys())

    def addModule(self, module):
        self.modules[module.name] = module

    def processSource(self, direction):
        if verbose(verboseDetail):
            print("process modules:")
        for m in sorted(self.modules):
            self.modules[m].processSource(direction)
