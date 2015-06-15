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

import shutil
import os
import re
import sys
import getopt
import filecmp
import difflib

#
# Global controls.
#
RTEMS_DIR = "."
FreeBSD_DIR = "freebsd-org"
isVerbose = False
isDryRun = False
isDiffMode = False
filesProcessed = 0

class error(Exception):
    """Base class for exceptions."""
    def __init(self, msg):
        self.msg = 'error: %s' % (msg)
    def set_output(self, msg):
        self.msg = msg
    def __str__(self):
        return self.msg

def common_flags():
    return ['-O',
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
            ('freebsd/sys/sys',               '*.h',               'sys'),
            ('freebsd/sys/vm',                '*.h',               'vm'),
            ('freebsd/sys/dev/mii',           '*.h',               'dev/mii'),
            ('mDNSResponder/mDNSCore',        'mDNSDebug.h',       ''),
            ('mDNSResponder/mDNSCore',        'mDNSEmbeddedAPI.h', ''),
            ('mDNSResponder/mDNSShared',      'dns_sd.h',          ''),
            ('mDNSResponder/mDNSPosix',       'mDNSPosix.h',       '')]

# compare and process file only if different
#  + copy or diff depending on execution mode
def processIfDifferent(new, old, src):

    global filesProcessed
    global isVerbose, isDryRun, isEarlyExit

    if not os.path.exists(old) or \
       filecmp.cmp(new, old, shallow = False) == False:
        filesProcessed += 1
        if isDiffMode == False:
            if isVerbose == True:
                print "Move " + src + " to " + old
            if isDryRun == False:
                shutil.move(new, old)
        else:
            if isVerbose == True:
                print "Diff %s => %s" % (src, new)
            old_contents = open(old).readlines()
            new_contents = open(new).readlines()
            for line in \
                difflib.unified_diff(old_contents, new_contents,
                                     fromfile = src, tofile = new, n = 5):
                sys.stdout.write(line)

# Move target dependent files under a machine directory
def mapCPUDependentPath(path):
  return path.replace("include/", "include/machine/")

def fixIncludes(data):
    data = re.sub('#include <sys/lock.h>', '#include <rtems/bsd/sys/lock.h>', data)
    data = re.sub('#include <sys/time.h>', '#include <rtems/bsd/sys/time.h>', data)
    data = re.sub('#include <sys/cpuset.h>', '#include <rtems/bsd/sys/cpuset.h>', data)
    data = re.sub('#include <sys/errno.h>', '#include <rtems/bsd/sys/errno.h>', data)
    data = re.sub('#include <sys/param.h>', '#include <rtems/bsd/sys/param.h>', data)
    data = re.sub('#include <sys/types.h>', '#include <rtems/bsd/sys/types.h>', data)
    data = re.sub('#include <sys/resource.h>', '#include <rtems/bsd/sys/resource.h>', data)
    data = re.sub('#include <sys/unistd.h>', '#include <rtems/bsd/sys/unistd.h>', data)
    data = re.sub('#include <sys/_types.h>', '#include <rtems/bsd/sys/_types.h>', data)
    return data

# revert fixing the include paths inside a C or .h file
def revertFixIncludes(data):
    data = re.sub('#include <rtems/bsd/', '#include <', data)
    data = re.sub('#include <util.h>', '#include <rtems/bsd/util.h>', data)
    data = re.sub('#include <bsd.h>', '#include <rtems/bsd/bsd.h>', data)
    data = re.sub('#include <zerocopy.h>', '#include <rtems/bsd/zerocopy.h>', data)
    return data

# fix include paths inside a C or .h file
def fixLocalIncludes(data):
    data = re.sub('#include "opt_([^"]*)"', '#include <rtems/bsd/local/opt_\\1>', data)
    data = re.sub('#include "([^"]*)_if.h"', '#include <rtems/bsd/local/\\1_if.h>', data)
    data = re.sub('#include "miidevs([^"]*)"', '#include <rtems/bsd/local/miidevs\\1>', data)
    data = re.sub('#include "usbdevs([^"]*)"', '#include <rtems/bsd/local/usbdevs\\1>', data)
    return data

# revert fixing the include paths inside a C or .h file
def revertFixLocalIncludes(data):
    data = re.sub('#include <rtems/bsd/local/([^>]*)>', '#include "\\1"', data)
    return data

def assertHeaderFile(path):
    if path[-2] != '.' or path[-1] != 'h':
        print "*** " + path + " does not end in .h"
        print "*** Move it to a C source file list"
        sys.exit(2)

def assertSourceFile(path):
    if path[-2] != '.' or (path[-1] != 'c' and path[-1] != 'S'):
        print "*** " + path + " does not end in .c"
        print "*** Move it to a header file list"
        sys.exit(2)

class Converter(object):
    def convert(self, src):
        return open(src).read()

    def isConvertible(self):
        return True

class NoConverter(Converter):
    def convert(self, src):
        raise

    def isConvertible(self):
        return False

class EmptyConverter(Converter):
    def convert(self, src):
        return '/* EMPTY */\n'

class FromFreeBSDToRTEMSHeaderConverter(Converter):
    def convert(self, src):
        data = super(FromFreeBSDToRTEMSHeaderConverter, self).convert(src)
        data = fixLocalIncludes(data)
        data = fixIncludes(data)
        return data

class FromFreeBSDToRTEMSUserSpaceHeaderConverter(Converter):
    def convert(self, src):
        data = super(FromFreeBSDToRTEMSUserSpaceHeaderConverter, self).convert(src)
        data = fixIncludes(data)
        return data

class FromFreeBSDToRTEMSSourceConverter(Converter):
    def convert(self, src):
        data = super(FromFreeBSDToRTEMSSourceConverter, self).convert(src)
        data = fixLocalIncludes(data)
        data = fixIncludes(data)
        data = '#include <machine/rtems-bsd-kernel-space.h>\n\n' + data
        return data

class FromFreeBSDToRTEMSUserSpaceSourceConverter(Converter):
    def convert(self, src):
        data = super(FromFreeBSDToRTEMSUserSpaceSourceConverter, self).convert(src)
        data = fixIncludes(data)
        data = '#include <machine/rtems-bsd-user-space.h>\n\n' + data
        return data

class FromRTEMSToFreeBSDHeaderConverter(Converter):
    def convert(self, src):
        data = super(FromRTEMSToFreeBSDHeaderConverter, self).convert(src)
        data = revertFixLocalIncludes(data)
        data = revertFixIncludes(data)
        return data

class FromRTEMSToFreeBSDSourceConverter(Converter):
    def convert(self, src):
        data = super(FromRTEMSToFreeBSDSourceConverter, self).convert(src)
        data = re.sub('#include <machine/rtems-bsd-kernel-space.h>\n\n', '', data)
        data = re.sub('#include <machine/rtems-bsd-user-space.h>\n\n', '', data)
        data = revertFixLocalIncludes(data)
        data = revertFixIncludes(data)
        return data

class PathComposer(object):
    def composeFreeBSDPath(self, path):
        return path

    def composeRTEMSPath(self, path, prefix):
        path = prefix + path
        return path

class FreeBSDPathComposer(PathComposer):
    def composeFreeBSDPath(self, path):
        return FreeBSD_DIR + '/' + path

    def composeRTEMSPath(self, path, prefix):
        return prefix + 'freebsd/' + path

class RTEMSPathComposer(PathComposer):
    def composeFreeBSDPath(self, path):
        return path

    def composeRTEMSPath(self, path, prefix):
        path = prefix + 'rtemsbsd/' + path
        return path

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

class File(object):
    def __init__(self, path, pathComposer,
                 fromFreeBSDToRTEMSConverter, fromRTEMSToFreeBSDConverter, buildSystemComposer):
        self.path = path
        self.pathComposer = pathComposer
        self.fromFreeBSDToRTEMSConverter = fromFreeBSDToRTEMSConverter
        self.fromRTEMSToFreeBSDConverter = fromRTEMSToFreeBSDConverter
        self.buildSystemComposer = buildSystemComposer

    def copy(self, dst, src, converter = None):
        import tempfile
        if converter is not None and converter.isConvertible():
            try:
                if isDryRun == False:
                    os.makedirs(os.path.dirname(dst))
            except OSError:
                pass
            data = converter.convert(src)
            try:
                out = tempfile.NamedTemporaryFile(delete = False)
                out.write(data)
                out.close()
                processIfDifferent(out.name, dst, src)
            finally:
                try:
                    os.remove(out.name)
                except:
                    pass

    def copyFromFreeBSDToRTEMS(self):
        src = self.pathComposer.composeFreeBSDPath(self.path)
        dst = self.pathComposer.composeRTEMSPath(self.path, RTEMS_DIR + '/')
        self.copy(dst, src, self.fromFreeBSDToRTEMSConverter)

    def copyFromRTEMSToFreeBSD(self):
        src = self.pathComposer.composeRTEMSPath(self.path, RTEMS_DIR + '/')
        dst = self.pathComposer.composeFreeBSDPath(self.path)
        self.copy(dst, src, self.fromRTEMSToFreeBSDConverter)

    def getFragment(self):
        return self.buildSystemComposer.compose(self.pathComposer.composeRTEMSPath(self.path, ''))

# Module - logical group of related files we can perform actions on
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

    def copyFromFreeBSDToRTEMS(self):
        for f in self.files:
            f.copyFromFreeBSDToRTEMS()
        for cpu, files in self.cpuDependentSourceFiles.items():
            for f in files:
                f.copyFromFreeBSDToRTEMS()

    def copyFromRTEMSToFreeBSD(self):
        for f in self.files:
            f.copyFromRTEMSToFreeBSD()
        for cpu, files in self.cpuDependentSourceFiles.items():
            for f in files:
                f.copyFromRTEMSToFreeBSD()

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

    def addCPUDependentSourceFiles(self, cpu, files, sourceFileFragmentComposer):
        self.initCPUDependencies(cpu)
        self.cpuDependentSourceFiles[cpu] += \
            self.addFiles(files,
                          CPUDependentPathComposer(), FromFreeBSDToRTEMSSourceConverter(),
                          FromRTEMSToFreeBSDSourceConverter(), assertSourceFile,
                          sourceFileFragmentComposer)

    def addTargetSourceCPUDependentSourceFiles(self, targetCPUs, sourceCPU, files,
                                               sourceFileFragmentComposer):
        for cpu in targetCPUs:
            self.initCPUDependencies(cpu)
            self.cpuDependentSourceFiles[cpu] += \
                self.addFiles(files,
                              TargetSourceCPUDependentPathComposer(cpu, sourceCPU),
                              FromFreeBSDToRTEMSSourceConverter(), NoConverter(),
                              assertSourceFile, sourceFileFragmentComposer)

    def addTest(self, testFragementComposer):
        self.files += [File(testFragementComposer.testName,
                            PathComposer(), NoConverter(), NoConverter(),
                            testFragementComposer)]

    def addDependency(self, dep):
        self.dependencies += [dep]

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

    def copyFromFreeBSDToRTEMS(self):
        for m in sorted(self.modules):
            self.modules[m].copyFromFreeBSDToRTEMS()

    def copyFromRTEMSToFreeBSD(self):
        for m in sorted(self.modules):
            self.modules[m].copyFromRTEMSToFreeBSD()
