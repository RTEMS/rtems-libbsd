# SPDX-License-Identifier: BSD-2-Clause
"""Manage the libbsd build configuration data.
"""

# Copyright (c) 2015, 2020 Chris Johns <chrisj@rtems.org>. All rights reserved.
#
# Copyright (c) 2009, 2017 embedded brains GmbH.  All rights reserved.
#
#   embedded brains GmbH
#   Dornierstr. 4
#   82178 Puchheim
#   Germany
#   <info@embedded-brains.de>
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

import codecs
import copy
import difflib
import os
import re
import sys

try:
    import configparser
except ImportError:
    import ConfigParser as configparser

#
# Global controls.
#
LIBBSD_DIR = "."
FreeBSD_DIR = "freebsd-org"
verboseLevel = 0
isDryRun = False
isDiffMode = False
filesProcessedCount = 0
filesProcessed = []
filesTotal = 0
filesTotalLines = 0
filesTotalInserts = 0
filesTotalDeletes = 0
diffDetails = {}

verboseInfo = 1
verboseDetail = 2
verboseMoreDetail = 3
verboseDebug = 4

BUILDSET_DIR = "buildset"
BUILDSET_DEFAULT = "buildset/default.ini"


def verbose(level=verboseInfo):
    return verboseLevel >= level


def changedFileSummary(statsReport=False):

    global filesTotal, filesTotalLines, filesTotalInserts, filesTotalDeletes

    if isDiffMode == False:
        if verbose():
            print('%d file(s) were changed:' % (filesProcessedCount))
            for f in sorted(filesProcessed):
                print(' %s' % (f))
        else:
            print('%d file(s) were changed.' % (filesProcessedCount))
    if statsReport:
        print('Stats Report:')
        transparent = filesTotal - len(diffDetails)
        changes = filesTotalInserts + filesTotalDeletes
        opacity = (float(changes) / (filesTotalLines + changes)) * 100.0
        print(' Total File(s):%d  Unchanged:%d (%.1f%%)  Changed:%d' \
              '   Opacity:%5.1f%% Lines:%d Edits:%d (+):%d (-):%d'  % \
              (filesTotal, transparent, (float(transparent) / filesTotal) * 100.0, len(diffDetails), \
               opacity, filesTotalLines, changes, filesTotalInserts, filesTotalDeletes))
        #
        # Sort by opacity.
        #
        ordered_diffs = sorted(diffDetails.items(),
                               key=lambda diff: diff[1].opacity,
                               reverse=True)
        for f in ordered_diffs:
            print('  %s' % (diffDetails[f[0]].status()))


def readFile(name):
    try:
        contents = codecs.open(name,
                               mode='r',
                               encoding='utf-8',
                               errors='ignore').read()
    except UnicodeDecodeError as ude:
        print('error: reading: %s: %s' % (name, ude))
        sys.exit(1)
    return contents


def writeFile(name, contents):
    path = os.path.dirname(name)
    if not os.path.exists(path):
        try:
            os.makedirs(path)
        except OSError as oe:
            print('error: cannot create directory: %s: %s' % (path, oe))
            sys.exit(1)
    try:
        codecs.open(name, mode='w', encoding='utf-8',
                    errors='ignore').write(contents)
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
# Diff Record
#
class diffRecord:
    def __init__(self, src, dst, orig, diff, inserts, deletes):
        self.src = src
        self.dst = dst
        self.orig = orig
        self.diff = diff
        self.lines = len(orig)
        self.inserts = inserts
        self.deletes = deletes
        self.changes = inserts + deletes
        self.opacity = (float(self.changes) /
                        (self.lines + self.changes)) * 100.0

    def __repr__(self):
        return self.src

    def status(self):
        return 'opacity:%5.1f%% edits:%4d (+):%-4d (-):%-4d %s' % \
            (self.opacity, self.changes, self.inserts, self.deletes, self.src)


#
# This stuff needs to move to libbsd.py.
#


# Move target dependent files under a machine directory
def mapCPUDependentPath(path):
    return path.replace("include/", "include/machine/")


def fixIncludes(data):
    data = re.sub('#include <sys/resource.h>',
                  '#include <rtems/bsd/sys/resource.h>', data)
    data = re.sub('#include <sys/unistd.h>',
                  '#include <rtems/bsd/sys/unistd.h>', data)
    return data


# revert fixing the include paths inside a C or .h file
def revertFixIncludes(data):
    data = re.sub('#include <rtems/bsd/', '#include <', data)
    data = re.sub('#include <util.h>', '#include <rtems/bsd/util.h>', data)
    data = re.sub('#include <bsd.h>', '#include <rtems/bsd/bsd.h>', data)
    data = re.sub('#include <zerocopy.h>', '#include <rtems/bsd/zerocopy.h>',
                  data)
    data = re.sub('#include <modules.h>', '#include <rtems/bsd/modules.h>',
                  data)
    return data


# fix include paths inside a C or .h file
def fixLocalIncludes(data):
    data = re.sub('#include "opt_([^"]*)"',
                  '#include <rtems/bsd/local/opt_\\1>', data)
    data = re.sub('#include "([^"]*)_if.h"',
                  '#include <rtems/bsd/local/\\1_if.h>', data)
    data = re.sub('#include "miidevs([^"]*)"',
                  '#include <rtems/bsd/local/miidevs\\1>', data)
    data = re.sub('#include "usbdevs([^"]*)"',
                  '#include <rtems/bsd/local/usbdevs\\1>', data)
    return data


# revert fixing the include paths inside a C or .h file
def revertFixLocalIncludes(data):
    data = re.sub('#include <rtems/bsd/local/([^>]*)>', '#include "\\1"', data)
    return data

def assertNothing(path):
    pass

def assertHeaderFile(path):
    if path[-2] != '.' or path[-1] != 'h':
        print("*** " + path + " does not end in .h")
        print("*** Move it to a C source file list")
        sys.exit(2)


def assertSourceFile(path):
    if path[-2:] != '.c' and path[-2:] != '.S' and path[-3:] != '.cc':
        print("*** " + path + " does not end in .c, .cc or .S")
        print("*** Move it to a header file list")
        sys.exit(2)


def assertHeaderOrSourceFile(path):
    if path[-2] != '.' or (path[-1] != 'h' and path[-1] != 'c'):
        print("*** " + path + " does not end in .h or .c")
        print("*** Move it to another list")
        sys.exit(2)


def diffSource(dstLines, srcLines, src, dst):
    global filesTotal, filesTotalLines, filesTotalInserts, filesTotalDeletes
    #
    # Diff, note there is no line termination on each string.  Expand the
    # generator to list because the generator is not reusable.
    #
    diff = list(
        difflib.unified_diff(dstLines,
                             srcLines,
                             fromfile=src,
                             tofile=dst,
                             n=5,
                             lineterm=''))
    inserts = 0
    deletes = 0
    if len(diff) > 0:
        if src in diffDetails and \
           diffDetails[src].dst != dst and diffDetails[src].diff != diff:
            raise error('repeated diff of file different: src:%s dst:%s' %
                        (src, dst))
        for l in diff:
            if l[0] == '-':
                deletes += 1
            elif l[0] == '+':
                inserts += 1
        diffDetails[src] = diffRecord(src, dst, srcLines, diff, inserts,
                                      deletes)

    #
    # Count the total files, lines and the level of changes.
    #
    filesTotal += 1
    filesTotalLines += len(srcLines)
    filesTotalInserts += inserts
    filesTotalDeletes += deletes

    return diff


#
# Converters provide a way to alter the various types of code. The conversion
# process filters a file as it is copies from the source path to the
# destination path. Specialised versions are provided for different types of
# source.
#
class Converter(object):
    def convert(self,
                src,
                dst,
                hasSource=True,
                sourceFilter=None,
                srcContents=None):

        global filesProcessed, filesProcessedCount

        if verbose(verboseDebug):
            print("convert: filter:%s: %s -> %s" % \
                  (['yes', 'no'][sourceFilter is None], src, dst))

        #
        # If there is no source raise an error if we expect source else print a
        # warning and do not try and convert.
        #
        if srcContents is None:
            if not os.path.exists(src):
                if hasSource:
                    raise error('source not found: %s' % (src))
                else:
                    print('warning: no source: %s' % (src))
                    return

            #
            # Files read as a single string if not passed in.
            #
            srcContents = readFile(src)

        if os.path.exists(dst):
            dstContents = readFile(dst)
        else:
            print('warning: no destination: %s' % (dst))
            dstContents = ''

        #
        # Filter the source.
        #
        if sourceFilter is not None:
            srcContents = sourceFilter(srcContents)

        #
        # Split into a list of lines.
        #
        srcLines = srcContents.split(os.linesep)
        dstLines = dstContents.split(os.linesep)

        if verbose(verboseDebug):
            print('Unified diff: %s (lines:%d)' % (src, len(srcLines)))

        #
        # Diff, note there is no line termination on each string.
        #
        diff = diffSource(dstLines, srcLines, src, dst)

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
                    writeFile(dst, srcContents)
            else:
                print("diff -u %s %s" % (src, dst))
                for l in diff:
                    print(l)


class NoConverter(Converter):
    def convert(self, src, dst, hasSource=True, sourceFilter=None):
        return '/* EMPTY */\n'


class FromFreeBSDToRTEMSHeaderConverter(Converter):
    def sourceFilter(self, data):
        data = fixLocalIncludes(data)
        data = fixIncludes(data)
        return data

    def convert(self, src, dst):
        sconverter = super(FromFreeBSDToRTEMSHeaderConverter, self)
        sconverter.convert(src, dst, sourceFilter=self.sourceFilter)


class FromFreeBSDToRTEMSUserSpaceHeaderConverter(Converter):
    def sourceFilter(self, data):
        data = fixIncludes(data)
        return data

    def convert(self, src, dst):
        sconverter = super(FromFreeBSDToRTEMSUserSpaceHeaderConverter, self)
        sconverter.convert(src, dst, sourceFilter=self.sourceFilter)


class FromFreeBSDToRTEMSSourceConverter(Converter):
    def sourceFilter(self, data):
        data = fixLocalIncludes(data)
        data = fixIncludes(data)
        data = '#include <machine/rtems-bsd-kernel-space.h>\n\n' + data
        return data

    def convert(self, src, dst):
        sconverter = super(FromFreeBSDToRTEMSSourceConverter, self)
        sconverter.convert(src, dst, sourceFilter=self.sourceFilter)


class FromFreeBSDToRTEMSUserSpaceSourceConverter(Converter):
    def sourceFilter(self, data):
        data = fixIncludes(data)
        data = '#include <machine/rtems-bsd-user-space.h>\n\n' + data
        return data

    def convert(self, src, dst):
        sconverter = super(FromFreeBSDToRTEMSUserSpaceSourceConverter, self)
        sconverter.convert(src, dst, sourceFilter=self.sourceFilter)


class FromRTEMSToFreeBSDHeaderConverter(Converter):
    def sourceFilter(self, data):
        data = revertFixLocalIncludes(data)
        data = revertFixIncludes(data)
        return data

    def convert(self, src, dst):
        sconverter = super(FromRTEMSToFreeBSDHeaderConverter, self)
        sconverter.convert(src,
                           dst,
                           hasSource=False,
                           sourceFilter=self.sourceFilter)


class FromRTEMSToFreeBSDSourceConverter(Converter):
    def sourceFilter(self, data):
        data = re.sub('#include <machine/rtems-bsd-kernel-space.h>\n\n', '',
                      data)
        data = re.sub('#include <machine/rtems-bsd-user-space.h>\n\n', '',
                      data)
        data = revertFixLocalIncludes(data)
        data = revertFixIncludes(data)
        return data

    def convert(self, src, dst):
        sconverter = super(FromRTEMSToFreeBSDSourceConverter, self)
        sconverter.convert(src,
                           dst,
                           hasSource=False,
                           sourceFilter=self.sourceFilter)


#
# Compose a path based for the various parts of the source tree.
#
class PathComposer(object):
    def composeOriginPath(self, path):
        return path

    def composeLibBSDPath(self, path, prefix):
        return os.path.join(prefix, path)


class FreeBSDPathComposer(PathComposer):
    def composeOriginPath(self, path):
        return os.path.join(FreeBSD_DIR, path)

    def composeLibBSDPath(self, path, prefix):
        return os.path.join(prefix, 'freebsd', path)


class RTEMSPathComposer(PathComposer):
    def composeOriginPath(self, path):
        return path

    def composeLibBSDPath(self, path, prefix):
        return os.path.join(prefix, 'rtemsbsd', path)


class LinuxPathComposer(PathComposer):
    def composeOriginPath(self, path):
        return path

    def composeLibBSDPath(self, path, prefix):
        return os.path.join(prefix, 'linux', path)


class CPUDependentFreeBSDPathComposer(FreeBSDPathComposer):
    def composeLibBSDPath(self, path, prefix):
        path = super(CPUDependentFreeBSDPathComposer,
                     self).composeLibBSDPath(path, prefix)
        path = mapCPUDependentPath(path)
        return path


class CPUDependentRTEMSPathComposer(RTEMSPathComposer):
    def composeLibBSDPath(self, path, prefix):
        path = super(CPUDependentRTEMSPathComposer,
                     self).composeLibBSDPath(path, prefix)
        path = mapCPUDependentPath(path)
        return path


class CPUDependentLinuxPathComposer(LinuxPathComposer):
    def composeLibBSDPath(self, path, prefix):
        path = super(CPUDependentLinuxPathComposer,
                     self).composeLibBSDPath(path, prefix)
        path = mapCPUDependentPath(path)
        return path


class TargetSourceCPUDependentPathComposer(CPUDependentFreeBSDPathComposer):
    def __init__(self, targetCPU, sourceCPU):
        self.targetCPU = targetCPU
        self.sourceCPU = sourceCPU

    def composeLibBSDPath(self, path, prefix):
        path = super(TargetSourceCPUDependentPathComposer,
                     self).composeLibBSDPath(path, prefix)
        path = path.replace(self.sourceCPU, self.targetCPU)
        return path


class BuildSystemComposer(object):
    def __init__(self, includes=None):
        if type(includes) is not list:
            self.includes = [includes]
        else:
            self.includes = includes

    def __str__(self):
        return ''

    def getIncludes(self):
        if None in self.includes:
            incs = []
        else:
            incs = self.includes
        return incs

    def compose(self, path):
        """A None result means there is nothing to build."""
        return None

    @staticmethod
    def filesAsDefines(files):
        define_keys = ''
        for f in files:
            f = f.upper()
            for c in '\/-.':
                f = f.replace(c, '_')
            define_keys += ' ' + f
        return define_keys.strip()

    @staticmethod
    def cflagsIncludes(cflags, includes):
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


class SourceFileBuildComposer(BuildSystemComposer):
    def __init__(self, cflags="default", includes=None):
        self.cflags, self.includes = self.cflagsIncludes(cflags, includes)

    def __str__(self):
        return 'SF: ' + ' '.join(self.getFlags())

    def compose(self, path):
        flags = self.getFlags()
        return ['sources', flags, ('default', None)], \
            [path], self.cflags, self.includes

    def getFlags(self):
        return self.cflags + self.getIncludes()


class SourceFileIfHeaderComposer(SourceFileBuildComposer):
    def __init__(self, headers, cflags="default", includes=None):
        if headers is not list:
            headers = [headers]
        self.headers = headers
        super(SourceFileIfHeaderComposer, self).__init__(cflags=cflags,
                                                         includes=includes)

    def __str__(self):
        return 'SFIH:' + ' '.join(self.getFlags()) + \
            ' ' + self.filesAsDefines(self.headers)

    def compose(self, path):
        r = SourceFileBuildComposer.compose(self, path)
        r[0][2] = (self.filesAsDefines(self.headers), self.headers)
        return r


class TestFragementComposer(BuildSystemComposer):
    def __init__(self,
                 testName,
                 fileFragments,
                 configTest=None,
                 runTest=True,
                 netTest=False,
                 extraLibs=[],
                 modules=[]):
        self.testName = testName
        self.fileFragments = fileFragments
        self.configTest = configTest
        self.runTest = runTest
        self.netTest = netTest
        self.extraLibs = extraLibs
        self.modules = modules

    def __str__(self):
        return 'TEST: ' + self.testName

    def compose(self, path):
        return ['tests', self.testName, ('default', None)], {
            'configTest': self.configTest,
            'files': self.fileFragments,
            'run': self.runTest,
            'net': self.netTest,
            'libs': self.extraLibs,
            'modules': self.modules,
        }


class TestIfHeaderComposer(TestFragementComposer):
    def __init__(self,
                 testName,
                 headers,
                 fileFragments,
                 runTest=True,
                 netTest=False,
                 extraLibs=[],
                 modules=[]):
        if headers is not list:
            headers = [headers]
        self.headers = headers
        super(TestIfHeaderComposer, self).__init__(testName,
                                                   fileFragments,
                                                   'header',
                                                   runTest=runTest,
                                                   netTest=netTest,
                                                   extraLibs=extraLibs,
                                                   modules=modules)

    def compose(self, path):
        r = TestFragementComposer.compose(self, path)
        r[0][2] = (self.filesAsDefines(self.headers), self.headers)
        return r


class TestIfLibraryComposer(TestFragementComposer):
    def __init__(self,
                 testName,
                 libraries,
                 fileFragments,
                 runTest=True,
                 netTest=False,
                 extraLibs=[],
                 modules=[]):
        if libraries is not list:
            libraries = [libraries]
        self.libraries = libraries
        super(TestIfLibraryComposer, self).__init__(testName,
                                                    fileFragments,
                                                    'library',
                                                    runTest=runTest,
                                                    netTest=netTest,
                                                    extraLibs=extraLibs,
                                                    modules=modules)

    def compose(self, path):
        r = TestFragementComposer.compose(self, path)
        r[0][2] = (self.filesAsDefines(self.libraries), self.libraries)
        return r


class KVMSymbolsBuildComposer(BuildSystemComposer):
    def compose(self, path):
        return ['KVMSymbols', 'files', ('default', None)], \
            [path], self.includes


class RPCGENBuildComposer(BuildSystemComposer):
    def compose(self, path):
        return ['RPCGen', 'files', ('default', None)], \
            [path]


class RouteKeywordsBuildComposer(BuildSystemComposer):
    def compose(self, path):
        return ['RouteKeywords', 'files', ('default', None)], \
            [path]


class LexBuildComposer(BuildSystemComposer):
    def __init__(self, sym, dep, cflags=None, includes=None, build=True):
        self.sym = sym
        self.dep = dep
        self.cflags, self.includes = self.cflagsIncludes(cflags, includes)
        self.build = build

    def compose(self, path):
        d = {
            'file': path,
            'sym': self.sym,
            'dep': self.dep,
            'build': self.build
        }
        if None not in self.cflags:
            d['cflags'] = self.cflags
        if None not in self.includes:
            d['includes'] = self.includes
        return ['lex', path, ('default', None)], d


class YaccBuildComposer(BuildSystemComposer):
    def __init__(self, sym, header, cflags=None, includes=None, build=True):
        self.sym = sym
        self.header = header
        self.cflags, self.includes = self.cflagsIncludes(cflags, includes)
        self.build = build

    def compose(self, path):
        d = {
            'file': path,
            'sym': self.sym,
            'header': self.header,
            'build': self.build
        }
        if None not in self.cflags:
            d['cflags'] = self.cflags
        if None not in self.includes:
            d['includes'] = self.includes
        return ['yacc', path, ('default', None)], d


class File(object):
    '''A file of source we move backwards and forwards and build.'''
    def __init__(self, space, path, pathComposer, forwardConverter,
                 reverseConverter, buildSystemComposer):
        if verbose(verboseMoreDetail):
            print("FILE: %-6s %-50s F:%-45s R:%-45s" % \
                  (space, path,
                   forwardConverter.__class__.__name__,
                   reverseConverter.__class__.__name__))
        self.space = space
        self.path = path
        self.pathComposer = pathComposer
        self.originPath = self.pathComposer.composeOriginPath(self.path)
        self.libbsdPath = self.pathComposer.composeLibBSDPath(
            self.path, LIBBSD_DIR)
        self.forwardConverter = forwardConverter
        self.reverseConverter = reverseConverter
        self.buildSystemComposer = buildSystemComposer

    def __str__(self):
        out = self.space[0].upper() + ' ' + self.path
        bsc = str(self.buildSystemComposer)
        if len(bsc) > 0:
            out += ' (' + bsc + ')'
        return out

    def __eq__(self, other):
        state = self.space == other.space
        state = state and (self.path == self.path)
        state = state and (self.pathComposer == self.pathComposer)
        state = state and (self.originPath == self.originPath)
        state = state and (self.forwardConverter == self.forwardConverter)
        state = state and (self.self.reverseConverter
                           == self.self.reverseConverter)
        state = state and (self.buildSystemComposer
                           == self.buildSystemComposer)
        return state

    def processSource(self, forward):
        if forward:
            if verbose(verboseDetail):
                print("process source: %s => %s" %
                      (self.originPath, self.libbsdPath))
            self.forwardConverter.convert(self.originPath, self.libbsdPath)
        else:
            if verbose(verboseDetail):
                print("process source: %s => %s converter:%s" % \
                      (self.libbsdPath, self.originPath,
                       self.reverseConverter.__class__.__name__))
            self.reverseConverter.convert(self.libbsdPath, self.originPath)

    def getFragment(self):
        return self.buildSystemComposer.compose(
            self.pathComposer.composeLibBSDPath(self.path, ''))

    def getPath(self):
        return self.path

    def getSpace(self):
        return self.space


class Module(object):
    '''Logical group of related files we can perform actions on'''
    def __init__(self, manager, name, enabled=True):
        self.manager = manager
        self.name = name
        self.files = []
        self.cpuDependentSourceFiles = {}
        self.dependencies = []

    def __str__(self):
        out = [self.name + ':']
        if len(self.dependencies) > 0:
            out += [' Deps: ' + str(len(self.dependencies))]
            out += ['  ' + type(d).__name__ for d in self.dependencies]
        if len(self.files) > 0:
            counts = {}
            for f in self.files:
                space = f.getSpace()
                if space not in counts:
                    counts[space] = 0
                counts[space] += 1
            count_str = ''
            for space in sorted(counts.keys()):
                count_str += '%s:%d ' % (space[0].upper(), counts[space])
            count_str = count_str[:-1]
            out += [' Files: %d (%s)' % (len(self.files), count_str)]
            out += ['  ' + str(f) for f in self.files]
        if len(self.cpuDependentSourceFiles) > 0:
            out += [' CPU Dep: ' + str(len(self.cpuDependentSourceFiles))]
            for cpu in self.cpuDependentSourceFiles:
                out += ['  ' + cpu + ':']
                out += [
                    '   ' + str(f) for f in self.cpuDependentSourceFiles[cpu]
                ]
        return os.linesep.join(out)

    def initCPUDependencies(self, cpu):
        if cpu not in self.cpuDependentSourceFiles:
            self.cpuDependentSourceFiles[cpu] = []

    def getName(self):
        return self.name

    def getFiles(self):
        return (f for f in self.files)

    def processSource(self, direction):
        if verbose(verboseDetail):
            print("process module: %s" % (self.name))
        for f in self.files:
            f.processSource(direction)
        for cpu, files in self.cpuDependentSourceFiles.items():
            for f in files:
                f.processSource(direction)

    def addFile(self, f):
        if not isinstance(f, File):
            raise TypeError('invalid type for addFiles: %s' % (type(f)))
        self.files += [f]

    def addFiles(self,
                 space,
                 newFiles,
                 pathComposer,
                 forwardConverter,
                 reverseConverter,
                 assertFile,
                 buildSystemComposer=BuildSystemComposer()):
        files = []
        for newFile in newFiles:
            assertFile(newFile)
            files += [
                File(space, newFile, pathComposer, forwardConverter,
                     reverseConverter, buildSystemComposer)
            ]
        return files

    def addPlainTextFile(self, files):
        self.files += self.addFiles('user', files,
                                    FreeBSDPathComposer(), Converter(),
                                    Converter(), assertNothing)

    def addKernelSpaceHeaderFiles(self, files):
        self.files += self.addFiles('kernel', files, FreeBSDPathComposer(),
                                    FromFreeBSDToRTEMSHeaderConverter(),
                                    FromRTEMSToFreeBSDHeaderConverter(),
                                    assertHeaderOrSourceFile)

    def addUserSpaceHeaderFiles(self, files):
        self.files += self.addFiles(
            'user', files, FreeBSDPathComposer(),
            FromFreeBSDToRTEMSUserSpaceHeaderConverter(),
            FromRTEMSToFreeBSDHeaderConverter(), assertHeaderFile)

    def addRTEMSHeaderFiles(self, files):
        self.files += self.addFiles('user', files, RTEMSPathComposer(),
                                    NoConverter(), NoConverter(),
                                    assertHeaderFile)

    def addLinuxHeaderFiles(self, files):
        self.files += self.addFiles('kernel', files, PathComposer(),
                                    NoConverter(), NoConverter(),
                                    assertHeaderFile)

    def addCPUDependentFreeBSDHeaderFiles(self, files):
        self.files += self.addFiles('kernel', files,
                                    CPUDependentFreeBSDPathComposer(),
                                    FromFreeBSDToRTEMSHeaderConverter(),
                                    FromRTEMSToFreeBSDHeaderConverter(),
                                    assertHeaderFile)

    def addCPUDependentLinuxHeaderFiles(self, files):
        self.files += self.addFiles('kernel', files,
                                    CPUDependentLinuxPathComposer(),
                                    NoConverter(), NoConverter(),
                                    assertHeaderFile)

    def addTargetSourceCPUDependentHeaderFiles(self, targetCPUs, sourceCPU,
                                               files):
        for cpu in targetCPUs:
            self.files += self.addFiles(
                'kernel', files,
                TargetSourceCPUDependentPathComposer(cpu, sourceCPU),
                FromFreeBSDToRTEMSHeaderConverter(), NoConverter(),
                assertHeaderFile)

    def addSourceFiles(self, files, sourceFileBuildComposer):
        self.files += self.addFiles('user',
                                    files, PathComposer(), NoConverter(),
                                    NoConverter(), assertSourceFile,
                                    sourceFileBuildComposer)

    def addKernelSpaceSourceFiles(self, files, sourceFileBuildComposer):
        self.files += self.addFiles('kernel', files, FreeBSDPathComposer(),
                                    FromFreeBSDToRTEMSSourceConverter(),
                                    FromRTEMSToFreeBSDSourceConverter(),
                                    assertSourceFile, sourceFileBuildComposer)

    def addUserSpaceSourceFiles(self, files, sourceFileBuildComposer):
        self.files += self.addFiles(
            'user', files, FreeBSDPathComposer(),
            FromFreeBSDToRTEMSUserSpaceSourceConverter(),
            FromRTEMSToFreeBSDSourceConverter(), assertSourceFile,
            sourceFileBuildComposer)

    def addRTEMSKernelSourceFiles(self, files, sourceFileBuildComposer):
        self.files += self.addFiles('kernel', files, RTEMSPathComposer(),
                                    NoConverter(), NoConverter(),
                                    assertSourceFile, sourceFileBuildComposer)

    def addRTEMSUserSourceFiles(self, files, sourceFileBuildComposer):
        self.files += self.addFiles('user', files, RTEMSPathComposer(),
                                    NoConverter(), NoConverter(),
                                    assertSourceFile, sourceFileBuildComposer)

    def addLinuxSourceFiles(self, files, sourceFileBuildComposer):
        self.files += self.addFiles('kernel', files, PathComposer(),
                                    NoConverter(), NoConverter(),
                                    assertSourceFile, sourceFileBuildComposer)

    def addCPUDependentFreeBSDSourceFiles(self, cpus, files,
                                          sourceFileBuildComposer):
        for cpu in cpus:
            self.initCPUDependencies(cpu)
            self.cpuDependentSourceFiles[cpu] += \
                self.addFiles(
                    'kernel', files,
                    CPUDependentFreeBSDPathComposer(), FromFreeBSDToRTEMSSourceConverter(),
                    FromRTEMSToFreeBSDSourceConverter(), assertSourceFile,
                    sourceFileBuildComposer)

    def addCPUDependentRTEMSSourceFiles(self, cpus, files,
                                        sourceFileBuildComposer):
        for cpu in cpus:
            self.initCPUDependencies(cpu)
            self.cpuDependentSourceFiles[cpu] += \
                self.addFiles('kernel', files,
                              CPUDependentRTEMSPathComposer(), NoConverter(),
                              NoConverter(), assertSourceFile,
                              sourceFileBuildComposer)

    def addCPUDependentLinuxSourceFiles(self, cpus, files,
                                        sourceFileBuildComposer):
        for cpu in cpus:
            self.initCPUDependencies(cpu)
            self.cpuDependentSourceFiles[cpu] += \
                self.addFiles('kernel', files,
                              CPUDependentLinuxPathComposer(), NoConverter(),
                              NoConverter(), assertSourceFile,
                              sourceFileBuildComposer)

    def addTest(self, testFragementComposer):
        self.files += [
            File('user', testFragementComposer.testName, PathComposer(),
                 NoConverter(), NoConverter(), testFragementComposer)
        ]

    def addDependency(self, dep):
        if not isinstance(dep, str):
            raise TypeError('dependencies are a string: %s' % (self.name))
        self.dependencies += [dep]


class ModuleManager(object):
    '''A manager for a collection of modules.'''
    def __init__(self):
        self.modules = {}
        self.generator = {}
        self.configuration = {}
        self.setGenerators()

    def __getitem__(self, key):
        if key not in self.modules:
            raise KeyError('module %s not found' % (key))
        return self.modules[key]

    def __str__(self):
        out = ['Modules: ' + str(len(self.modules)), '']
        for m in sorted(self.modules):
            out += [str(self.modules[m]), '']
        return os.linesep.join(out)

    def _loadIni(self, ini_file):
        if not os.path.exists(ini_file):
            raise FileNotFoundError('file not found: %s' % (ini_file))
        ini = configparser.ConfigParser()
        ini.read(ini_file)
        if not ini.has_section('general'):
            raise Exception(
                "'{}' is missing a general section.".format(ini_file))
        if not ini.has_option('general', 'name'):
            raise Exception("'{}' is missing a general/name.".format(ini_file))
        if ini.has_option('general', 'extends'):
            extends = ini.get('general', 'extends')
            extendfile = None
            basepath = os.path.dirname(ini_file)
            if os.path.isfile(os.path.join(basepath, extends)):
                extendfile = os.path.join(basepath, extends)
            elif os.path.isfile(os.path.join(BUILDSET_DIR, extends)):
                extendfile = os.path.join(BUILDSET_DIR, extends)
            else:
                raise Exception(
                    "'{}': Invalid file given for general/extends:'{}'".format(
                        ini_file, extends))
            base = self._loadIni(extendfile)
            for s in ini.sections():
                if not base.has_section(s):
                    base.add_section(s)
                for o in ini.options(s):
                    val = ini.get(s, o)
                    base.set(s, o, val)
            ini = base
        return ini

    def _checkDependencies(self):
        enabled_modules = self.getEnabledModules()
        enabled_modules.remove('tests')
        for mod in enabled_modules:
            if mod not in self.modules:
                raise KeyError('enabled module not found: %s' % (mod))
            for dep in self.modules[mod].dependencies:
                if dep not in self.modules:
                    print(type(dep))
                    raise KeyError('dependent module not found: %s' % (dep))
                if dep not in enabled_modules:
                    raise Exception('module "%s" dependency "%s" not enabled' %
                                    (mod, dep))

    def getAllModules(self):
        if 'modules' in self.configuration:
            return sorted(self.configuration['modules'])
        return []

    def getEnabledModules(self):
        if 'modules-enabled' in self.configuration:
            return sorted(self.configuration['modules-enabled'])
        return []

    def addModule(self, module):
        name = module.name
        if name in self.modules:
            raise KeyError('module already added: %' % (name))
        self.modules[name] = module
        if 'modules' not in self.configuration:
            self.configuration['modules'] = []
        if 'modules-enabled' not in self.configuration:
            self.configuration['modules-enabled'] = []
        self.configuration['modules'] += [name]
        self.configuration['modules-enabled'] += [name]
        self.configuration['modules'].sort()
        self.configuration['modules-enabled'].sort

    def processSource(self, direction):
        if verbose(verboseDetail):
            print("process modules:")
        for m in sorted(self.modules):
            self.modules[m].processSource(direction)

    def setConfiguration(self, config):
        self.configuration = copy.deepcopy(config)

    def getConfiguration(self):
        return copy.deepcopy(self.configuration)

    def generateBuild(self, only_enabled=True):
        modules_to_process = self.getEnabledModules()
        # Used for copy between FreeBSD and RTEMS
        if only_enabled == False:
            modules_to_process = self.getAllModules()
        for m in modules_to_process:
            if m not in self.modules:
                raise KeyError('enabled module not registered: %s' % (m))
            self.modules[m].generate()
        self._checkDependencies()

    def duplicateCheck(self):
        dups = []
        modules_to_check = sorted(self.getAllModules(), reverse=True)
        while len(modules_to_check) > 1:
            mod = modules_to_check.pop()
            for m in modules_to_check:
                if m not in self.modules:
                    raise KeyError('enabled module not registered: %s' % (m))
                for fmod in self.modules[mod].getFiles():
                    for fm in self.modules[m].getFiles():
                        if fmod.getPath() == fm.getPath():
                            dups += [(m, mod, fm.getPath(), fm.getSpace())]
        return dups

    def loadConfig(self, config=BUILDSET_DEFAULT):
        if 'name' in self.configuration:
            raise KeyError('configuration already loaded: %s (%s)' % \
                           (self.configuration['name'], config))
        ini = self._loadIni(config)
        self.configuration['name'] = ini.get('general', 'name')
        self.configuration['modules-enabled'] = []
        mods = []
        if ini.has_section('modules'):
            mods = ini.options('modules')
        for mod in mods:
            if ini.getboolean('modules', mod):
                self.configuration['modules-enabled'].append(mod)

    def getName(self):
        if 'name' not in self.configuration:
            raise KeyError('configuration not loaded')
        return self.configuration['name']

    def setGenerators(self):
        self.generator['convert'] = Converter
        self.generator['no-convert'] = NoConverter
        self.generator[
            'from-FreeBSD-to-RTEMS-UserSpaceSourceConverter'] = \
                FromFreeBSDToRTEMSUserSpaceSourceConverter
        self.generator[
            'from-RTEMS-To-FreeBSD-SourceConverter'] = FromRTEMSToFreeBSDSourceConverter
        self.generator['buildSystemComposer'] = BuildSystemComposer

        self.generator['file'] = File

        self.generator['path'] = PathComposer
        self.generator['freebsd-path'] = FreeBSDPathComposer
        self.generator['rtems-path'] = RTEMSPathComposer
        self.generator['cpu-path'] = CPUDependentFreeBSDPathComposer
        self.generator[
            'target-src-cpu--path'] = TargetSourceCPUDependentPathComposer

        self.generator['source'] = SourceFileBuildComposer
        self.generator['test'] = TestFragementComposer
        self.generator['kvm-symbols'] = KVMSymbolsBuildComposer
        self.generator['rpc-gen'] = RPCGENBuildComposer
        self.generator['route-keywords'] = RouteKeywordsBuildComposer
        self.generator['lex'] = LexBuildComposer
        self.generator['yacc'] = YaccBuildComposer

        self.generator['source-if-header'] = SourceFileIfHeaderComposer
        self.generator['test-if-header'] = TestIfHeaderComposer
        self.generator['test-if-library'] = TestIfLibraryComposer
