#!/usr/bin/python
#
#  Copyright (c) 2009-2013 embedded brains GmbH.  All rights reserved.
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

RTEMS_DIR = "not_set"
FreeBSD_DIR = "not_set"
isVerbose = False
isForward = True
isDryRun = False
isDiffMode = False
isEarlyExit = False
isOnlyMakefile = False
tempFile = "/tmp/tmp_FBRT"
filesProcessed = 0

def usage():
  print "freebsd-to-rtems.py [args]"
  print "  -?|-h|--help     print this and exit"
  print "  -d|--dry-run     run program but no modifications"
  print "  -D|--diff        provide diff of files between trees"
  print "  -e|--early-exit  evaluate arguments, print results, and exit"
  print "  -m|--makefile    just generate Makefile"
  print "  -R|--reverse     default FreeBSD -> RTEMS, reverse that"
  print "  -r|--rtems       RTEMS Libbsd directory"
  print "  -f|--freebsd     FreeBSD SVN directory"
  print "  -v|--verbose     enable verbose output mode"

# Parse the arguments
def parseArguments():
  global RTEMS_DIR, FreeBSD_DIR
  global isVerbose, isForward, isDryRun, isEarlyExit
  global isOnlyMakefile, isDiffMode
  try:
    opts, args = getopt.getopt(sys.argv[1:], "?hdDemRr:f:v",
                 ["help",
                  "help",
                  "dry-run"
                  "diff"
                  "early-exit"
                  "makefile"
                  "reverse"
                  "rtems="
                  "freebsd="
                  "verbose"
                 ]
                )
  except getopt.GetoptError, err:
    # print help information and exit:
    print str(err) # will print something like "option -a not recognized"
    usage()
    sys.exit(2)
  for o, a in opts:
    if o in ("-v", "--verbose"):
      isVerbose = True
    elif o in ("-h", "--help", "-?"):
      usage()
      sys.exit()
    elif o in ("-d", "--dry-run"):
      isDryRun = True
    elif o in ("-D", "--diff"):
      isDiffMode = True
    elif o in ("-e", "--early-exit"):
      isEarlyExit = True
    elif o in ("-m", "--makefile"):
      isOnlyMakefile = True
    elif o in ("-R", "--reverse"):
      isForward = False
    elif o in ("-r", "--rtems"):
      RTEMS_DIR = a
    elif o in ("-f", "--freebsd"):
      FreeBSD_DIR = a
    else:
       assert False, "unhandled option"

parseArguments()
print "Verbose:                " + ("no", "yes")[isVerbose]
print "Dry Run:                " + ("no", "yes")[isDryRun]
print "Diff Mode Enabled:      " + ("no", "yes")[isDiffMode]
print "Only Generate Makefile: " + ("no", "yes")[isOnlyMakefile]
print "RTEMS Libbsd Directory: " + RTEMS_DIR
print "FreeBSD SVN Directory:  " + FreeBSD_DIR
print "Direction:              " + ("reverse", "forward")[isForward]

# Check directory argument was set and exist
def wasDirectorySet(desc, path):
    if path == "not_set":
        print desc + " Directory was not specified on command line"
        sys.exit(2)

    if os.path.isdir( path ) != True:
        print desc + " Directory (" + path + ") does not exist"
        sys.exit(2)

# Were RTEMS and FreeBSD directories specified
wasDirectorySet( "RTEMS", RTEMS_DIR )
wasDirectorySet( "FreeBSD", FreeBSD_DIR )
 
# Are we generating or reverting?
if isForward == True:
    print "Forward from FreeBSD SVN into ", RTEMS_DIR
else:
    print "Reverting from ", RTEMS_DIR
    if isOnlyMakefile == True:
        print "Only Makefile Mode and Reverse are contradictory"
        sys.exit(2)

if isEarlyExit == True:
    print "Early exit at user request"
    sys.exit(0)

# Move target dependent files under a machine directory
def mapCPUDependentPath(path):
	return path.replace("include/", "include/machine/")

# compare and process file only if different
#  + copy or diff depending on execution mode
def processIfDifferent(new, old, src):
  global filesProcessed
  global isVerbose, isDryRun, isEarlyExit
  if not os.path.exists(old) or \
     filecmp.cmp(new, old, shallow=False) == False:
    filesProcessed += 1
    if isDiffMode == False:
      if isVerbose == True:
        print "Move " + src + " to " + old
      if isDryRun == False:
        shutil.move(new, old)
    else:
      #print "Diff " + src
      old_contents = open(old).readlines()
      new_contents = open(new).readlines()
      for line in difflib.unified_diff( \
          old_contents, new_contents, fromfile=src, tofile=new, n=5):
        sys.stdout.write(line)

def fixIncludes(data):
	data = re.sub('#include <sys/lock.h>', '#include <rtems/bsd/sys/lock.h>', data)
	data = re.sub('#include <sys/time.h>', '#include <rtems/bsd/sys/time.h>', data)
	data = re.sub('#include <sys/errno.h>', '#include <rtems/bsd/sys/errno.h>', data)
	data = re.sub('#include <sys/param.h>', '#include <rtems/bsd/sys/param.h>', data)
	data = re.sub('#include <sys/types.h>', '#include <rtems/bsd/sys/types.h>', data)
	data = re.sub('#include <sys/resource.h>', '#include <rtems/bsd/sys/resource.h>', data)
	data = re.sub('#include <sys/unistd.h>', '#include <rtems/bsd/sys/unistd.h>', data)
	data = re.sub('#include <sys/timespec.h>', '#include <rtems/bsd/sys/timespec.h>', data)
	data = re.sub('#include <sys/_types.h>', '#include <rtems/bsd/sys/_types.h>', data)
	return data

# revert fixing the include paths inside a C or .h file
def revertFixIncludes(data):
	data = re.sub('#include <rtems/bsd/', '#include <', data)
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
		return FreeBSD_DIR + '/' + path

	def composeRTEMSPath(self, path, prefix):
		return prefix + 'freebsd/' + path

class RTEMSPathComposer(PathComposer):
	def composeFreeBSDPath(self, path):
		return path

	def composeRTEMSPath(self, path, prefix):
		path = prefix + 'rtemsbsd/' + path
		return path

class CPUDependentPathComposer(PathComposer):
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

class MakefileFragmentComposer(object):
	def compose(self, path):
		return ''

class SourceFileMakefileFragmentComposer(MakefileFragmentComposer):
	def compose(self, path):
		return 'LIB_C_FILES += ' + path + '\n'

class TestMakefileFragementComposer(MakefileFragmentComposer):
	def __init__(self, testName, fileFragments, runTest):
		self.testName = testName
		self.fileFragments = fileFragments
		self.runTest = runTest

	def compose(self, path):
		testPrefix = 'TEST_' + self.testName.upper()
		testOFiles = testPrefix + '_O_FILES'
		testDFiles = testPrefix + '_D_FILES'
		testDir = 'testsuite/' + self.testName
		testExe = testDir + '/' + self.testName + '.exe'
		testMap = testDir + '/' + self.testName + '.map'
		makefileFragment = '\n' + testPrefix + ' = ' + testExe + '\n' \
			+ testOFiles + ' =\n' \
			+ testDFiles + ' =\n'
		for fileFragment in self.fileFragments:
			makefileFragment = makefileFragment + testOFiles + ' += ' + testDir + '/' + fileFragment + '.o\n' \
				+ testDFiles + ' += ' + testDir + '/' + fileFragment + '.d\n'
		makefileFragment = makefileFragment + '$(' + testPrefix + '): $(' + testOFiles + ') $(LIB)\n' \
			'\t$(LINK.c) -Wl,-Map,' + testMap + ' $^ -lm -lz -o $@\n' \
			'TESTS += $(' + testPrefix + ')\n' \
			'O_FILES += $(' + testOFiles + ')\n' \
			'D_FILES += $(' + testDFiles + ')\n'
		if self.runTest:
			makefileFragment = makefileFragment + 'RUN_TESTS += $(' + testPrefix + ')\n'
		return makefileFragment

class File(object):
	def __init__(self, path, pathComposer, fromFreeBSDToRTEMSConverter, fromRTEMSToFreeBSDConverter, makefileFragmentComposer):
		self.path = path
		self.pathComposer = pathComposer
		self.fromFreeBSDToRTEMSConverter = fromFreeBSDToRTEMSConverter
		self.fromRTEMSToFreeBSDConverter = fromRTEMSToFreeBSDConverter
		self.makefileFragmentComposer = makefileFragmentComposer

	def copy(self, dst, src, converter):
		if converter.isConvertible():
			global tempFile
			try:
				if isDryRun == False:
					os.makedirs(os.path.dirname(dst))
			except OSError:
				pass
			data = converter.convert(src)
			out = open(tempFile, 'w')
			out.write(data)
			out.close()
			processIfDifferent(tempFile, dst, src)

	def copyFromFreeBSDToRTEMS(self):
		src = self.pathComposer.composeFreeBSDPath(self.path)
		dst = self.pathComposer.composeRTEMSPath(self.path, RTEMS_DIR + '/')
		self.copy(dst, src, self.fromFreeBSDToRTEMSConverter)

	def copyFromRTEMSToFreeBSD(self):
		src = self.pathComposer.composeRTEMSPath(self.path, RTEMS_DIR + '/')
		dst = self.pathComposer.composeFreeBSDPath(self.path)
		self.copy(dst, src, self.fromRTEMSToFreeBSDConverter)

	def getMakefileFragment(self):
		return self.makefileFragmentComposer.compose(self.pathComposer.composeRTEMSPath(self.path, ''))

# Module Manager - Collection of Modules
class ModuleManager:
	def __init__(self):
		self.modules = []

	def addModule(self, module):
		self.modules.append(module)

	def copyFromFreeBSDToRTEMS(self):
		for m in self.modules:
			m.copyFromFreeBSDToRTEMS()

	def copyFromRTEMSToFreeBSD(self):
		for m in self.modules:
			m.copyFromRTEMSToFreeBSD()

	def createMakefile(self):
		global tempFile
		data = 'include config.inc\n' \
			'\n' \
			'include $(RTEMS_MAKEFILE_PATH)/Makefile.inc\n' \
			'include $(RTEMS_CUSTOM)\n' \
			'include $(PROJECT_ROOT)/make/leaf.cfg\n' \
			'\n' \
			'CFLAGS += -ffreestanding\n' \
			'CFLAGS += -fno-common\n' \
			'CFLAGS += -Irtemsbsd/include\n' \
			'CFLAGS += -Irtemsbsd/$(RTEMS_CPU)/include\n' \
			'CFLAGS += -Ifreebsd/sys\n' \
			'CFLAGS += -Ifreebsd/sys/$(RTEMS_CPU)/include\n' \
			'CFLAGS += -Ifreebsd/sys/contrib/altq\n' \
			'CFLAGS += -Ifreebsd/sys/contrib/pf\n' \
			'CFLAGS += -Icopied/rtemsbsd/$(RTEMS_CPU)/include\n' \
			'CFLAGS += -Ifreebsd/include\n' \
			'CFLAGS += -Ifreebsd/lib/libc/include\n' \
			'CFLAGS += -Ifreebsd/lib/libc/isc/include\n' \
			'CFLAGS += -Ifreebsd/lib/libc/resolv\n' \
			'CFLAGS += -Ifreebsd/lib/libutil\n' \
			'CFLAGS += -Ifreebsd/lib/libkvm\n' \
			'CFLAGS += -Ifreebsd/lib/libmemstat\n' \
			'CFLAGS += -Ifreebsd/lib/libipsec\n' \
			'CFLAGS += -Itestsuite/include\n' \
			'CFLAGS += -D__FreeBSD__\n' \
			'CFLAGS += -D__DBINTERFACE_PRIVATE\n' \
			'CFLAGS += -DINET6\n' \
			'CFLAGS += -w\n' \
			'CFLAGS += -std=gnu99\n' \
			'CFLAGS += -MT $@ -MD -MP -MF $(basename $@).d\n' \
			'NEED_DUMMY_PIC_IRQ=yes\n' \
			'\n' \
                        '# do nothing default so sed on rtems-bsd-kernel-space.h always works.\n' \
                        'SED_PATTERN += -e \'s/^//\'\n' \
			'\n' \
			'TESTS =\n' \
			'RUN_TESTS =\n' \
			'O_FILES =\n' \
			'D_FILES =\n' \
			'\n' \
			'LIB = libbsd.a\n' \
			'LIB_GEN_FILES = rtemsbsd/include/machine/rtems-bsd-kernel-space.h\n' \
			'LIB_C_FILES =\n'
		for m in self.modules:
			if m.conditionalOn != "none":
				data += 'ifneq ($(' + m.conditionalOn + '),yes)\n'

			for f in m.files:
				data += f.getMakefileFragment()
			for cpu, files in sorted(m.cpuDependentSourceFiles.items()):
				data += 'ifeq ($(RTEMS_CPU), ' + cpu + ')\n'
				for f in files:
					data += f.getMakefileFragment()
				if cpu in ("arm", "i386", "lm32", "mips", "powerpc", "sparc"):
					data += 'NEED_DUMMY_PIC_IRQ=no\n'
				data += 'endif\n'
			if m.conditionalOn != "none":
				data += 'else\n'
				data += 'SED_PATTERN += -e \'' + m.cppPattern +'\'\n'
				data += 'endif # ' + m.conditionalOn +'\n'
		data += '\n' \
			'ifeq ($(NEED_DUMMY_PIC_IRQ),yes)\n' \
			'CFLAGS += -I rtems-dummy-pic-irq/include\n' \
			'endif\n' \
			'LIB_O_FILES = $(LIB_C_FILES:%.c=%.o)\n' \
			'O_FILES += $(LIB_O_FILES)\n' \
			'D_FILES += $(LIB_C_FILES:%.c=%.d)\n' \
			'\n' \
			'all: $(LIB) $(TESTS)\n' \
			'\n' \
			'$(LIB): $(LIB_GEN_FILES) $(LIB_O_FILES)\n' \
			'\t$(AR) rcu $@ $^\n' \
			'run_tests: $(RUN_TESTS)\n' \
			'\t$(TEST_RUNNER) $^\n' \
			'\tcheck_endof\n' \
			'\n' \
			'# The following targets use the MIPS Generic in_cksum routine\n' \
			'rtemsbsd/include/machine/rtems-bsd-kernel-space.h: rtemsbsd/include/machine/rtems-bsd-kernel-space.h.in\n' \
			'\tsed $(SED_PATTERN) <$< >$@\n' \
			'\n' \
			'CPU_SED  = sed\n' \
			'CPU_SED += -e \'/arm/d\'\n' \
			'CPU_SED += -e \'/i386/d\'\n' \
			'CPU_SED += -e \'/powerpc/d\'\n' \
			'CPU_SED += -e \'/mips/d\'\n' \
			'CPU_SED += -e \'/sparc64/d\'\n' \
			'\n' \
			'install: $(LIB)\n' \
			'\tinstall -d $(INSTALL_BASE)/include\n' \
			'\tinstall -c -m 644 $(LIB) $(INSTALL_BASE)\n' \
			'\tcd rtemsbsd; for i in `find freebsd -name \'*.h\'` ; do \\\n' \
			'\t  install -c -m 644 -D "$$i" "$(INSTALL_BASE)/include/$$i" ; done\n' \
			'\tcd contrib/altq; for i in `find freebsd -name \'*.h\'` ; do \\\n' \
			'\t  install -c -m 644 -D "$$i" "$(INSTALL_BASE)/include/$$i" ; done\n' \
			'\tcd contrib/pf; for i in `find freebsd -name \'*.h\'` ; do \\\n' \
                        '\t  install -c -m 644 -D "$$i" "$(INSTALL_BASE)/include/$$i" ; done\n' \
			'\tfor i in `find freebsd -name \'*.h\' | $(CPU_SED)` ; do \\\n' \
			'\t  install -c -m 644 -D "$$i" "$(INSTALL_BASE)/include/$$i" ; done\n' \
			'\t-cd freebsd/$(RTEMS_CPU)/include && for i in `find . -name \'*.h\'` ; do \\\n' \
			'\t  install -c -m 644 -D "$$i" "$(INSTALL_BASE)/include/$$i" ; done\n' \
			'\t-cd rtemsbsd/$(RTEMS_CPU)/include && \\\n' \
			'\t  for i in `find . -name \'*.h\' | $(CPU_SED)` ; do \\\n' \
			'\t    install -c -m 644 -D "$$i" "$(INSTALL_BASE)/include/$$i" ; done\n' \
			'\t-cd copied/rtemsbsd/$(RTEMS_CPU)/include && for i in `find . -name \'*.h\'` ; do \\\n' \
			'\t  install -c -m 644 -D "$$i" "$(INSTALL_BASE)/include/$$i" ; done\n' \
			'\n' \
			'clean:\n' \
			'\trm -f $(LIB_GEN_FILES) $(LIB) $(TESTS) $(O_FILES) $(D_FILES)\n' \
			'\trm -f libbsd.html\n' \
			'\n' \
			'-include $(D_FILES)\n' \
			'\n' \
			'doc: libbsd.html\n' \
			'\n' \
			'libbsd.html: libbsd.txt\n' \
			'\tasciidoc -o libbsd.html libbsd.txt\n'
 
		out = open(tempFile, 'w')
		out.write(data)
		out.close()
		makefile = RTEMS_DIR + '/Makefile'
		processIfDifferent(tempFile, makefile, "Makefile")

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

# Module - logical group of related files we can perform actions on
class Module:
	def __init__(self, name):
		self.name = name
		self.conditionalOn = "none"
		self.cppPattern = "s///"
		self.files = []
		self.cpuDependentSourceFiles = {}
		self.dependencies = []

	def initCPUDependencies(self, cpu):
		if not self.cpuDependentSourceFiles.has_key(cpu):
			self.cpuDependentSourceFiles [cpu] = []

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

	def addFiles(self, currentFiles, newFiles, pathComposer, fromFreeBSDToRTEMSConverter, fromRTEMSToFreeBSDConverter, assertFile, makefileFragmentComposer = MakefileFragmentComposer()):
		for newFile in newFiles:
			assertFile(newFile)
			currentFiles.append(File(newFile, pathComposer, fromFreeBSDToRTEMSConverter, fromRTEMSToFreeBSDConverter, makefileFragmentComposer))
		return currentFiles

	def addFile(self, f):
		self.files.append(f)

	def addHeaderFiles(self, files):
		self.files = self.addFiles(self.files, files, PathComposer(), FromFreeBSDToRTEMSHeaderConverter(), FromRTEMSToFreeBSDHeaderConverter(), assertHeaderFile)

	def addUserSpaceHeaderFiles(self, files):
		self.files = self.addFiles(self.files, files, PathComposer(), FromFreeBSDToRTEMSUserSpaceHeaderConverter(), FromRTEMSToFreeBSDHeaderConverter(), assertHeaderFile)

	def addRTEMSHeaderFiles(self, files):
		self.files = self.addFiles(self.files, files, RTEMSPathComposer(), NoConverter(), NoConverter(), assertHeaderFile)

	def addCPUDependentHeaderFiles(self, files):
		self.files = self.addFiles(self.files, files, CPUDependentPathComposer(), FromFreeBSDToRTEMSHeaderConverter(), FromRTEMSToFreeBSDHeaderConverter(), assertHeaderFile)

	def addTargetSourceCPUDependentHeaderFiles(self, targetCPUs, sourceCPU, files):
		for cpu in targetCPUs:
			self.files = self.addFiles(self.files, files, TargetSourceCPUDependentPathComposer(cpu, sourceCPU), FromFreeBSDToRTEMSHeaderConverter(), NoConverter(), assertHeaderFile)

	def addSourceFiles(self, files):
		self.files = self.addFiles(self.files, files, PathComposer(), FromFreeBSDToRTEMSSourceConverter(), FromRTEMSToFreeBSDSourceConverter(), assertSourceFile, SourceFileMakefileFragmentComposer())

	def addUserSpaceSourceFiles(self, files):
		self.files = self.addFiles(self.files, files, PathComposer(), FromFreeBSDToRTEMSUserSpaceSourceConverter(), FromRTEMSToFreeBSDSourceConverter(), assertSourceFile, SourceFileMakefileFragmentComposer())

	def addRTEMSSourceFiles(self, files):
		self.files = self.addFiles(self.files, files, RTEMSPathComposer(), NoConverter(), NoConverter(), assertSourceFile, SourceFileMakefileFragmentComposer())

	def addCPUDependentSourceFiles(self, cpu, files):
		self.initCPUDependencies(cpu)
		self.cpuDependentSourceFiles [cpu] = self.addFiles(self.cpuDependentSourceFiles [cpu], files, CPUDependentPathComposer(), FromFreeBSDToRTEMSSourceConverter(), FromRTEMSToFreeBSDSourceConverter(), assertSourceFile, SourceFileMakefileFragmentComposer())

	def addTargetSourceCPUDependentSourceFiles(self, targetCPUs, sourceCPU, files):
		for cpu in targetCPUs:
			self.initCPUDependencies(cpu)
			self.cpuDependentSourceFiles [cpu] = self.addFiles(self.cpuDependentSourceFiles [cpu], files, TargetSourceCPUDependentPathComposer(cpu, sourceCPU), FromFreeBSDToRTEMSSourceConverter(), NoConverter(), assertSourceFile, SourceFileMakefileFragmentComposer())

	def addTest(self, testName, fileFragments, runTest = True):
		self.files.append(File(testName, PathComposer(), NoConverter(), NoConverter(), TestMakefileFragementComposer(testName, fileFragments, runTest)))

	def addDependency(self, dep):
		self.dependencies.append(dep)

# Create Module Manager and supporting Modules
#  - initialize each module with set of files associated
mm = ModuleManager()

rtems = Module('rtems')
rtems.addRTEMSSourceFiles(
	[
		'local/bus_if.c',
		'local/cryptodev_if.c',
		'local/device_if.c',
		'local/miibus_if.c',
		'local/pcib_if.c',
		'local/pci_if.c',
		'local/usb_if.c',
		'rtems/ipsec_get_policylen.c',
		'rtems/rtems-bsd-assert.c',
		'rtems/rtems-bsd-autoconf.c',
		'rtems/rtems-bsd-bus-dma.c',
		'rtems/rtems-bsd-bus-dma-mbuf.c',
		'rtems/rtems-bsd-cam.c',
		'rtems/rtems-bsd-chunk.c',
		'rtems/rtems-bsd-condvar.c',
		'rtems/rtems-bsd-conf.c',
		'rtems/rtems-bsd-delay.c',
		'rtems/rtems-bsd-get-file.c',
		'rtems/rtems-bsd-init.c',
		'rtems/rtems-bsd-init-with-irq.c',
		'rtems/rtems-bsd-jail.c',
		'rtems/rtems-bsd-kern_synch.c',
		'rtems/rtems-bsd-lock.c',
		'rtems/rtems-bsd-log.c',
		'rtems/rtems-bsd-malloc.c',
		'rtems/rtems-bsd-mutex.c',
		'rtems/rtems-bsd-newproc.c',
		'rtems/rtems-bsd-nexus.c',
		'rtems/rtems-bsd-panic.c',
		'rtems/rtems-bsd-pci_bus.c',
		'rtems/rtems-bsd-pci_cfgreg.c',
		'rtems/rtems-bsd-program.c',
		'rtems/rtems-bsd-rwlock.c',
		'rtems/rtems-bsd-shell.c',
		'rtems/rtems-bsd-shell-netcmds.c',
		'rtems/rtems-bsd-signal.c',
		'rtems/rtems-bsd-support.c',
		'rtems/rtems-bsd-sx.c',
		'rtems/rtems-bsd-syscall-api.c',
		'rtems/rtems-bsd-sysctlbyname.c',
		'rtems/rtems-bsd-sysctl.c',
		'rtems/rtems-bsd-sysctlnametomib.c',
		'rtems/rtems-bsd-thread.c',
		'rtems/rtems-bsd-timesupport.c',
		'rtems/rtems-kvm.c',
		'rtems/rtems-net-setup.c',
		'rtems/rtems-syslog-initialize.c',
		'rtems/syslog.c',
		'sys/dev/usb/controller/ehci_mpc83xx.c',
		'sys/dev/usb/controller/ohci_lpc24xx.c',
		'sys/dev/usb/controller/ohci_lpc32xx.c',
	]
)

class KVMSymbolsMakefileFragmentComposer(MakefileFragmentComposer):
	def compose(self, path):
		return 'LIB_GEN_FILES += ' + path + '\n' \
			'LIB_C_FILES += ' + path + '\n' \
			+ path + ': rtemsbsd/rtems/generate_kvm_symbols\n' \
			'\t./$< > $@\n'

rtems.addFile(File('rtems/rtems-kvm-symbols.c', RTEMSPathComposer(), NoConverter(), NoConverter(), KVMSymbolsMakefileFragmentComposer()))

base = Module('base')
base.addHeaderFiles(
	[
		'sys/bsm/audit.h',
		'sys/bsm/audit_kevents.h',
		'sys/fs/devfs/devfs_int.h',
		'sys/security/audit/audit.h',
		'sys/security/mac/mac_framework.h',
		'sys/sys/acl.h',
		'sys/sys/aio.h',
		'sys/sys/bitstring.h',
		'sys/sys/bufobj.h',
		'sys/sys/buf_ring.h',
		'sys/sys/_bus_dma.h',
		'sys/sys/bus_dma.h',
		'sys/sys/bus.h',
		'sys/sys/_callout.h',
		'sys/sys/callout.h',
		'sys/sys/capability.h',
		'sys/sys/condvar.h',
		'sys/sys/conf.h',
		'sys/sys/_cpuset.h',
		'sys/sys/cpuset.h',
		'sys/sys/cpu.h',
		'sys/sys/ctype.h',
		'sys/sys/domain.h',
		'sys/sys/endian.h',
		'sys/sys/eventhandler.h',
		'sys/sys/filedesc.h',
		'sys/sys/file.h',
		'sys/sys/filio.h',
		'sys/sys/fnv_hash.h',
		'sys/sys/hash.h',
		'sys/sys/hhook.h',
		'sys/sys/interrupt.h',
		'sys/sys/jail.h',
		'sys/sys/kernel.h',
		'sys/sys/khelp.h',
		'sys/sys/kobj.h',
		'sys/sys/kthread.h',
		'sys/sys/ktr.h',
		'sys/sys/libkern.h',
		'sys/sys/limits.h',
		'sys/sys/linker.h',
		'sys/sys/linker_set.h',
		'sys/sys/_lock.h',
		'sys/sys/_lockmgr.h',
		'sys/sys/lockmgr.h',
		'sys/sys/lock_profile.h',
		'sys/sys/lockstat.h',
		'sys/sys/loginclass.h',
		'sys/sys/mac.h',
		'sys/sys/malloc.h',
		'sys/sys/mbuf.h',
		'sys/sys/module.h',
		'sys/sys/module_khelp.h',
		'sys/sys/mount.h',
		'sys/sys/_mutex.h',
		'sys/sys/mutex.h',
		'sys/sys/_null.h',
		'sys/sys/osd.h',
		'sys/sys/pcpu.h',
		'sys/sys/priority.h',
		'sys/sys/priv.h',
		'sys/sys/proc.h',
		'sys/sys/protosw.h',
		'sys/sys/racct.h',
		'sys/sys/random.h',
		'sys/sys/reboot.h',
		'sys/sys/refcount.h',
		'sys/sys/resourcevar.h',
		'sys/sys/rman.h',
		'sys/sys/_rmlock.h',
		'sys/sys/rmlock.h',
		'sys/sys/rtprio.h',
		'sys/sys/runq.h',
		'sys/sys/_rwlock.h',
		'sys/sys/rwlock.h',
		'sys/sys/sbuf.h',
		'sys/sys/sdt.h',
		'sys/sys/select.h',
		'sys/sys/selinfo.h',
		'sys/sys/_semaphore.h',
		'sys/sys/sf_buf.h',
		'sys/sys/sigio.h',
		'sys/sys/_sigset.h',
		'sys/sys/smp.h',
		'sys/sys/_sockaddr_storage.h',
		'sys/sys/sockbuf.h',
		'sys/sys/socket.h',
		'sys/sys/socketvar.h',
		'sys/sys/sockio.h',
		'sys/sys/sockopt.h',
		'sys/sys/sockstate.h',
		'sys/sys/stddef.h',
		'sys/sys/stdint.h',
		'sys/sys/_sx.h',
		'sys/sys/sx.h',
		'sys/sys/sysctl.h',
		'sys/sys/syslog.h',
		'sys/sys/sysproto.h',
		'sys/sys/systm.h',
		'sys/sys/_task.h',
		'sys/sys/taskqueue.h',
		'sys/sys/nlist_aout.h',
		'sys/rpc/types.h',
		'sys/sys/timepps.h',
		'sys/sys/timetc.h',
		'sys/sys/_timeval.h',
		'sys/sys/timex.h',
		'sys/sys/tree.h',
		'sys/sys/ttycom.h',
		'sys/sys/ucred.h',
		'sys/sys/un.h',
		'sys/sys/unpcb.h',
		'sys/sys/vmmeter.h',
		'sys/sys/vnode.h',
		'sys/vm/uma_dbg.h',
		'sys/vm/uma.h',
		'sys/vm/uma_int.h',
		'sys/vm/vm_extern.h',
		'sys/vm/vm.h',
	]
)
base.addSourceFiles(
	[
		'sys/kern/init_main.c',
		'sys/kern/kern_event.c',
		'sys/kern/kern_hhook.c',
		'sys/kern/kern_intr.c',
		'sys/kern/kern_khelp.c',
		'sys/kern/kern_linker.c',
		'sys/kern/kern_mbuf.c',
		'sys/kern/kern_mib.c',
		'sys/kern/kern_module.c',
		'sys/kern/kern_mtxpool.c',
		'sys/kern/kern_osd.c',
		'sys/kern/kern_sysctl.c',
		'sys/kern/kern_time.c',
		'sys/kern/kern_timeout.c',
		'sys/kern/subr_bufring.c',
		'sys/kern/subr_bus.c',
		'sys/kern/subr_eventhandler.c',
		'sys/kern/subr_hash.c',
		'sys/kern/subr_hints.c',
		'sys/kern/subr_kobj.c',
		'sys/kern/subr_module.c',
		'sys/kern/subr_rman.c',
		'sys/kern/subr_sbuf.c',
		'sys/kern/subr_taskqueue.c',
		'sys/kern/subr_uio.c',
		'sys/kern/subr_unit.c',
		'sys/kern/sys_generic.c',
		'sys/kern/uipc_accf.c',
		'sys/kern/uipc_domain.c',
		'sys/kern/uipc_mbuf2.c',
		'sys/kern/uipc_mbuf.c',
		'sys/kern/uipc_sockbuf.c',
		'sys/kern/uipc_socket.c',
		'sys/libkern/arc4random.c',
		'sys/libkern/fls.c',
		'sys/libkern/inet_ntoa.c',
		'sys/libkern/random.c',
		'sys/vm/uma_core.c',
	]
)

devUsb = Module('dev_usb')
devUsb.addHeaderFiles(
	[
		'sys/dev/usb/ufm_ioctl.h',
		'sys/dev/usb/usb_busdma.h',
		'sys/dev/usb/usb_bus.h',
		'sys/dev/usb/usb_cdc.h',
		'sys/dev/usb/usb_controller.h',
		'sys/dev/usb/usb_core.h',
		'sys/dev/usb/usb_debug.h',
		'sys/dev/usb/usb_dev.h',
		'sys/dev/usb/usb_device.h',
		'sys/dev/usb/usbdi.h',
		'sys/dev/usb/usbdi_util.h',
		'sys/dev/usb/usb_dynamic.h',
		'sys/dev/usb/usb_endian.h',
		'sys/dev/usb/usb_freebsd.h',
		'sys/dev/usb/usb_generic.h',
		'sys/dev/usb/usb.h',
		'sys/dev/usb/usbhid.h',
		'sys/dev/usb/usb_hub.h',
		'sys/dev/usb/usb_ioctl.h',
		'sys/dev/usb/usb_mbuf.h',
		'sys/dev/usb/usb_msctest.h',
		'sys/dev/usb/usb_pf.h',
		'sys/dev/usb/usb_process.h',
		'sys/dev/usb/usb_request.h',
		'sys/dev/usb/usb_transfer.h',
		'sys/dev/usb/usb_util.h',
	]
)
devUsb.addSourceFiles(
	[
		'sys/dev/usb/usb_busdma.c',
		'sys/dev/usb/usb_core.c',
		'sys/dev/usb/usb_debug.c',
		'sys/dev/usb/usb_dev.c',
		'sys/dev/usb/usb_device.c',
		'sys/dev/usb/usb_dynamic.c',
		'sys/dev/usb/usb_error.c',
		'sys/dev/usb/usb_generic.c',
		'sys/dev/usb/usb_handle_request.c',
		'sys/dev/usb/usb_hid.c',
		'sys/dev/usb/usb_hub.c',
		'sys/dev/usb/usb_lookup.c',
		'sys/dev/usb/usb_mbuf.c',
		'sys/dev/usb/usb_msctest.c',
		'sys/dev/usb/usb_parse.c',
		'sys/dev/usb/usb_process.c',
		'sys/dev/usb/usb_request.c',
		'sys/dev/usb/usb_transfer.c',
		'sys/dev/usb/usb_util.c',
	]
)

devUsbAddOn = Module('dev_usb_add_on')
devUsbAddOn.addHeaderFiles(
	[
		'sys/dev/usb/usb_pci.h',
		'sys/dev/usb/usb_compat_linux.h',
	]
)
devUsbAddOn.addSourceFiles(
	[
		'sys/dev/usb/usb_compat_linux.c',
	]
)

devUsbBluetooth = Module('dev_usb_bluetooth')
devUsbBluetooth.addDependency(devUsb)
devUsbBluetooth.addHeaderFiles(
	[
		'sys/dev/usb/bluetooth/ng_ubt_var.h',
	]
)
devUsbBluetooth.addSourceFiles(
	[
		'sys/dev/usb/bluetooth/ng_ubt.c',
		'sys/dev/usb/bluetooth/ubtbcmfw.c',
	]
)

devUsbController = Module('dev_usb_controller')
devUsbController.addDependency(devUsb)
devUsbController.addHeaderFiles(
	[
		'sys/dev/usb/controller/ohci.h',
		'sys/dev/usb/controller/ohcireg.h',
		'sys/dev/usb/controller/ehci.h',
		'sys/dev/usb/controller/ehcireg.h',
		'sys/dev/usb/controller/uhcireg.h',
		'sys/dev/usb/controller/xhcireg.h',
	]
)
devUsbController.addSourceFiles(
	[
		'sys/dev/usb/controller/ohci.c',
		'sys/dev/usb/controller/ehci.c',
		'sys/dev/usb/controller/usb_controller.c',
	]
)

devUsbControllerAddOn = Module('dev_usb_controller_add_on')
devUsbControllerAddOn.addDependency(devUsb)
devUsbControllerAddOn.addHeaderFiles(
	[
		'sys/dev/usb/controller/at91dci.h',
		'sys/dev/usb/controller/atmegadci.h',
		'sys/dev/usb/controller/musb_otg.h',
		'sys/dev/usb/controller/uss820dci.h',
	]
)
devUsbControllerAddOn.addSourceFiles(
	[
		'sys/dev/usb/controller/at91dci_atmelarm.c',
		'sys/dev/usb/controller/at91dci.c',
		'sys/dev/usb/controller/atmegadci_atmelarm.c',
		'sys/dev/usb/controller/atmegadci.c',
		'sys/dev/usb/controller/ehci_ixp4xx.c',
		'sys/dev/usb/controller/ehci_pci.c',
		'sys/dev/usb/controller/musb_otg.c',
		'sys/dev/usb/controller/ehci_mbus.c',
		'sys/dev/usb/controller/musb_otg_atmelarm.c',
		'sys/dev/usb/controller/ohci_atmelarm.c',
		'sys/dev/usb/controller/ohci_pci.c',
		'sys/dev/usb/controller/uhci_pci.c',
		'sys/dev/usb/controller/uss820dci_atmelarm.c',
		'sys/dev/usb/controller/uss820dci.c',
	]
)

devUsbInput = Module('dev_usb_input')
devUsbInput.addDependency(devUsb)
devUsbInput.addHeaderFiles(
	[
		'sys/dev/usb/input/usb_rdesc.h',
	]
)
devUsbInput.addSourceFiles(
	[
		'sys/dev/usb/input/uhid.c',
		'sys/dev/usb/input/ukbd.c',
	]
)

devUsbInputMouse = Module('dev_usb_mouse')
devUsbInputMouse.addDependency(devUsb)
devUsbInputMouse.addHeaderFiles(
	[
		'sys/sys/tty.h',
		'sys/sys/mouse.h',
		'sys/sys/ttyqueue.h',
		'sys/sys/ttydefaults.h',
		'sys/sys/ttydisc.h',
		'sys/sys/ttydevsw.h',
		'sys/sys/ttyhook.h',
	]
)
devUsbInputMouse.addSourceFiles(
	[
		'sys/dev/usb/input/ums.c',
	]
)

devUsbMisc = Module('dev_usb_misc')
devUsbMisc.addDependency(devUsb)
devUsbMisc.addHeaderFiles(
	[
		'sys/dev/usb/misc/udbp.h',
	]
)
devUsbMisc.addSourceFiles(
	[
		'sys/dev/usb/misc/udbp.c',
		'sys/dev/usb/misc/ufm.c',
	]
)

devUsbNet = Module('dev_usb_net')
devUsbNet.addDependency(devUsb)
devUsbNet.addHeaderFiles(
	[
		'sys/dev/mii/mii.h',
		'sys/dev/mii/miivar.h',
		'sys/dev/usb/net/if_cdcereg.h',
		'sys/dev/usb/net/usb_ethernet.h',
	]
)
devUsbNet.addSourceFiles(
	[
		'sys/dev/usb/net/if_cdce.c',
		'sys/dev/usb/net/usb_ethernet.c',
	]
)

devUsbQuirk = Module('dev_usb_quirk')
devUsbQuirk.addDependency(devUsb)
devUsbQuirk.addHeaderFiles(
	[
		'sys/dev/usb/quirk/usb_quirk.h',
	]
)
devUsbQuirk.addSourceFiles(
	[
		'sys/dev/usb/quirk/usb_quirk.c',
	]
)

devUsbSerial = Module('dev_usb_serial')
devUsbSerial.addDependency(devUsb)
devUsbSerial.addHeaderFiles(
	[
		'sys/dev/usb/serial/uftdi_reg.h',
		'sys/dev/usb/serial/usb_serial.h',
	]
)
devUsbSerial.addSourceFiles(
	[
		'sys/dev/usb/serial/u3g.c',
		'sys/dev/usb/serial/uark.c',
		'sys/dev/usb/serial/ubsa.c',
		'sys/dev/usb/serial/ubser.c',
		'sys/dev/usb/serial/uchcom.c',
		'sys/dev/usb/serial/ucycom.c',
		'sys/dev/usb/serial/ufoma.c',
		'sys/dev/usb/serial/uftdi.c',
		'sys/dev/usb/serial/ugensa.c',
		'sys/dev/usb/serial/uipaq.c',
		'sys/dev/usb/serial/ulpt.c',
		'sys/dev/usb/serial/umct.c',
		'sys/dev/usb/serial/umodem.c',
		'sys/dev/usb/serial/umoscom.c',
		'sys/dev/usb/serial/uplcom.c',
		'sys/dev/usb/serial/usb_serial.c',
		'sys/dev/usb/serial/uslcom.c',
		'sys/dev/usb/serial/uvisor.c',
		'sys/dev/usb/serial/uvscom.c',
	]
)

devUsbStorage = Module('dev_usb_storage')
devUsbStorage.addDependency(devUsb)
devUsbStorage.addSourceFiles(
	[
		'sys/dev/usb/storage/umass.c',
	]
)

devUsbStorageAddOn = Module('dev_usb_storage_add_on')
devUsbStorageAddOn.addDependency(devUsb)
devUsbStorageAddOn.addHeaderFiles(
	[
		'sys/dev/usb/storage/rio500_usb.h',
	]
)
devUsbStorageAddOn.addSourceFiles(
	[
		'sys/dev/usb/storage/urio.c',
		'sys/dev/usb/storage/ustorage_fs.c',
	]
)

devUsbTemplate = Module('dev_usb_template')
devUsbTemplate.addDependency(devUsb)
devUsbTemplate.addHeaderFiles(
	[
		'sys/dev/usb/template/usb_template.h',
	]
)
devUsbTemplate.addSourceFiles(
	[
		'sys/dev/usb/template/usb_template.c',
		'sys/dev/usb/template/usb_template_cdce.c',
		'sys/dev/usb/template/usb_template_msc.c',
		'sys/dev/usb/template/usb_template_mtp.c',
	]
)

devUsbWlan = Module('dev_usb_wlan')
devUsbWlan.addDependency(devUsb)
devUsbWlan.addHeaderFiles(
	[
		'sys/dev/usb/wlan/if_rumfw.h',
		'sys/dev/usb/wlan/if_rumreg.h',
		'sys/dev/usb/wlan/if_rumvar.h',
		'sys/dev/usb/wlan/if_uathreg.h',
		'sys/dev/usb/wlan/if_uathvar.h',
		'sys/dev/usb/wlan/if_upgtvar.h',
		'sys/dev/usb/wlan/if_uralreg.h',
		'sys/dev/usb/wlan/if_uralvar.h',
		'sys/dev/usb/wlan/if_zydfw.h',
		'sys/dev/usb/wlan/if_zydreg.h',
	]
)
devUsbWlan.addSourceFiles(
	[
		'sys/dev/usb/wlan/if_rum.c',
		'sys/dev/usb/wlan/if_uath.c',
		'sys/dev/usb/wlan/if_upgt.c',
		'sys/dev/usb/wlan/if_ural.c',
		'sys/dev/usb/wlan/if_zyd.c',
	]
)

cam = Module('cam')
cam.addHeaderFiles(
	[
		'sys/sys/ata.h',
		'sys/cam/cam.h',
		'sys/cam/cam_ccb.h',
		'sys/cam/cam_sim.h',
		'sys/cam/cam_xpt_sim.h',
		'sys/cam/scsi/scsi_all.h',
		'sys/cam/scsi/scsi_da.h',
		'sys/cam/ata/ata_all.h',
		'sys/cam/cam_periph.h',
		'sys/cam/cam_debug.h',
		'sys/cam/cam_xpt.h',
	]
)
cam.addSourceFiles(
	[
		'sys/cam/cam.c',
		'sys/cam/scsi/scsi_all.c',
	]
)

devNet = Module('dev_net')
devNet.addHeaderFiles(
	[
		'sys/dev/mii/mii.h',
		'sys/dev/mii/mii_bitbang.h',
		'sys/dev/mii/miivar.h',
		'sys/dev/mii/brgphyreg.h',
		'sys/dev/mii/icsphyreg.h',
		'sys/dev/led/led.h',
		'sys/net/bpf.h',
		'sys/net/ethernet.h',
		'sys/net/if_arp.h',
		'sys/net/if_dl.h',
		'sys/net/if.h',
		'sys/net/if_media.h',
		'sys/net/if_types.h',
		'sys/net/if_var.h',
		'sys/net/vnet.h',
	]
)
devNet.addSourceFiles(
	[
		'sys/dev/mii/mii.c',
		'sys/dev/mii/mii_bitbang.c',
		'sys/dev/mii/mii_physubr.c',
		'sys/dev/mii/icsphy.c',
		'sys/dev/mii/brgphy.c',
	]
)

devNic = Module('dev_nic')
devNic.addHeaderFiles(
	[
		'sys/sys/pciio.h',
		'sys/dev/random/randomdev_soft.h',
		'sys/sys/eventvar.h',
		'sys/sys/kenv.h',
		'sys/isa/isavar.h',
		'sys/isa/pnpvar.h',
		'sys/netatalk/at.h',
		'sys/netatalk/endian.h',
		'sys/netatalk/aarp.h',
		'sys/netatalk/at_extern.h',
		'sys/netatalk/at_var.h',
		'sys/netatalk/ddp.h',
		'sys/netatalk/ddp_pcb.h',
		'sys/netatalk/ddp_var.h',
		'sys/netatalk/phase2.h',
		'sys/sys/mman.h',
		'sys/sys/buf.h',
		'sys/sys/mqueue.h',
		'sys/sys/tty.h',
		'sys/sys/ttyqueue.h',
		'sys/sys/ttydisc.h',
		'sys/sys/ttydevsw.h',
		'sys/sys/ttyhook.h',
		'sys/sys/user.h',
	]
)

devNic.addCPUDependentHeaderFiles(
	[
		'sys/arm/include/cpufunc.h',
		'sys/i386/include/specialreg.h',
		'sys/i386/include/md_var.h',
		'sys/i386/include/intr_machdep.h',
		'sys/i386/include/cpufunc.h',
		'sys/mips/include/cpufunc.h',
		'sys/mips/include/cpuregs.h',
		'sys/powerpc/include/cpufunc.h',
		'sys/powerpc/include/psl.h',
		'sys/powerpc/include/spr.h',
		'sys/sparc64/include/cpufunc.h',
		'sys/sparc64/include/asi.h',
		'sys/sparc64/include/pstate.h',
	]
)
devNic.addSourceFiles(
	[
		'sys/dev/random/harvest.c',
		'sys/netinet/tcp_hostcache.c',
		'sys/dev/led/led.c',
		'sys/netatalk/aarp.c',
		'sys/netatalk/at_control.c',
		'sys/netatalk/at_rmx.c',
		'sys/netatalk/ddp_input.c',
		'sys/netatalk/ddp_pcb.c',
		'sys/netatalk/ddp_usrreq.c',
		'sys/netatalk/at_proto.c',
		'sys/netatalk/ddp_output.c',
	]
)

devNic_re = Module('dev_nic_re')
devNic_re.addHeaderFiles(
	[
		'sys/pci/if_rlreg.h',
	]
)
devNic_re.addSourceFiles(
	[
		'sys/dev/re/if_re.c',
	]
)

devNic_fxp = Module('dev_nic_fxp')
devNic_fxp.addHeaderFiles(
	[
		'sys/dev/fxp/if_fxpreg.h',
		'sys/dev/fxp/if_fxpvar.h',
		'sys/dev/fxp/rcvbundl.h',
	]
)
devNic_fxp.addSourceFiles(
	[
		'sys/dev/fxp/if_fxp.c',
	]
)

devNic_e1000 = Module('dev_nic_e1000')
devNic_e1000.addHeaderFiles(
	[
		'sys/dev/e1000/e1000_80003es2lan.h',
		'sys/dev/e1000/e1000_82541.h',
		'sys/dev/e1000/e1000_82543.h',
		'sys/dev/e1000/e1000_82571.h',
		'sys/dev/e1000/e1000_82575.h',
		'sys/dev/e1000/e1000_api.h',
		'sys/dev/e1000/e1000_defines.h',
		'sys/dev/e1000/e1000_hw.h',
		'sys/dev/e1000/e1000_i210.h',
		'sys/dev/e1000/e1000_ich8lan.h',
		'sys/dev/e1000/e1000_mac.h',
		'sys/dev/e1000/e1000_manage.h',
		'sys/dev/e1000/e1000_mbx.h',
		'sys/dev/e1000/e1000_nvm.h',
		'sys/dev/e1000/e1000_osdep.h',
		'sys/dev/e1000/e1000_phy.h',
		'sys/dev/e1000/e1000_regs.h',
		'sys/dev/e1000/e1000_vf.h',
		'sys/dev/e1000/if_em.h',
		'sys/dev/e1000/if_igb.h',
		'sys/dev/e1000/if_lem.h',
	]
)
devNic_e1000.addSourceFiles(
	[
		'sys/dev/e1000/e1000_80003es2lan.c',
		'sys/dev/e1000/e1000_82542.c',
		'sys/dev/e1000/e1000_82575.c',
		'sys/dev/e1000/e1000_mac.c',
		'sys/dev/e1000/e1000_nvm.c',
		'sys/dev/e1000/e1000_vf.c',
		'sys/dev/e1000/if_lem.c',
		'sys/dev/e1000/e1000_82540.c',
		'sys/dev/e1000/e1000_82543.c',
		'sys/dev/e1000/e1000_api.c',
		'sys/dev/e1000/e1000_manage.c',
		'sys/dev/e1000/e1000_osdep.c',
		'sys/dev/e1000/if_em.c',
		'sys/dev/e1000/e1000_82541.c',
		'sys/dev/e1000/e1000_82571.c',
		'sys/dev/e1000/e1000_ich8lan.c',
		'sys/dev/e1000/e1000_mbx.c',
		'sys/dev/e1000/e1000_phy.c',
		'sys/dev/e1000/if_igb.c',
	]
)

# DEC Tulip aka Intel 21143
devNic_dc = Module('dev_nic_dc')
devNic_dc.addHeaderFiles(
	[
		'sys/dev/dc/if_dcreg.h',
	]
)
devNic_dc.addSourceFiles(
	[
		'sys/dev/dc/dcphy.c',
		'sys/dev/dc/if_dc.c',
		'sys/dev/dc/pnphy.c',
	]
)

# SMC9111x
devNic_smc = Module('dev_nic_smc')
devNic_smc.addHeaderFiles(
	[
		'sys/dev/smc/if_smcreg.h',
		'sys/dev/smc/if_smcvar.h',
	]
)
devNic_smc.addSourceFiles(
	[
		'sys/dev/smc/if_smc.c',
	]
)

# Crystal Semiconductor CS8900
devNic_cs = Module('dev_nic_cs')
devNic_cs.addHeaderFiles(
	[
		'sys/dev/cs/if_csreg.h',
		'sys/dev/cs/if_csvar.h',
	]
)
devNic_cs.addSourceFiles(
	[
		'sys/dev/cs/if_cs.c',
		'sys/dev/cs/if_cs_isa.c',
		'sys/dev/cs/if_cs_pccard.c',
	]
)

# Broadcomm BCE, BFE, BGE - MII is intertwined
devNic_broadcomm = Module('dev_nic_broadcomm')
devNic_broadcomm.addHeaderFiles(
	[
		'sys/dev/bce/if_bcefw.h',
		'sys/dev/bce/if_bcereg.h',
		'sys/dev/bfe/if_bfereg.h',
		'sys/dev/bge/if_bgereg.h',
	]
)
devNic_broadcomm.addSourceFiles(
	[
		'sys/dev/bce/if_bce.c',
		'sys/dev/bfe/if_bfe.c',
		'sys/dev/bge/if_bge.c',
	]
)

net = Module('net')
net.addHeaderFiles(
	[
		'sys/net/bpf_buffer.h',
		'sys/net/bpfdesc.h',
		'sys/net/bpf.h',
		'sys/net/bpf_jitter.h',
		'sys/net/bpf_zerocopy.h',
		'sys/net/bridgestp.h',
		'sys/net/ethernet.h',
		'sys/net/fddi.h',
		'sys/net/firewire.h',
		'sys/net/flowtable.h',
		'sys/net/ieee8023ad_lacp.h',
		'sys/net/if_arc.h',
		'sys/net/if_arp.h',
		'sys/net/if_atm.h',
		'sys/net/if_bridgevar.h',
		'sys/net/if_clone.h',
		'sys/net/if_dl.h',
		'sys/net/if_enc.h',
		'sys/net/if_gif.h',
		'sys/net/if_gre.h',
		'sys/net/if.h',
		'sys/net/if_lagg.h',
		'sys/net/if_llatbl.h',
		'sys/net/if_llc.h',
		'sys/net/if_media.h',
		'sys/net/if_mib.h',
		'sys/net/if_sppp.h',
		'sys/net/if_stf.h',
		'sys/net/if_tap.h',
		'sys/net/if_tapvar.h',
		'sys/net/if_tun.h',
		'sys/net/if_types.h',
		'sys/net/if_var.h',
		'sys/net/if_vlan_var.h',
		'sys/net/iso88025.h',
		'sys/net/netisr.h',
		'sys/net/netisr_internal.h',
		'sys/net/pfil.h',
		'sys/net/pfkeyv2.h',
		'sys/net/ppp_defs.h',
		'sys/net/radix.h',
		'sys/net/radix_mpath.h',
		'sys/net/raw_cb.h',
		'sys/net/route.h',
		'sys/net/slcompress.h',
		'sys/net/vnet.h',
	]
)
net.addSourceFiles(
	[
		'sys/kern/sys_socket.c',
		'sys/kern/uipc_syscalls.c',
		'sys/net/bridgestp.c',
		'sys/net/ieee8023ad_lacp.c',
		'sys/net/if_atmsubr.c',
		'sys/net/if.c',
		'sys/net/if_clone.c',
		'sys/net/if_dead.c',
		'sys/net/if_disc.c',
		'sys/net/if_edsc.c',
		'sys/net/if_ef.c',
		'sys/net/if_enc.c',
		'sys/net/if_epair.c',
		'sys/net/if_faith.c',
		'sys/net/if_fddisubr.c',
		'sys/net/if_fwsubr.c',
		'sys/net/if_gif.c',
		'sys/net/if_gre.c',
		'sys/net/if_iso88025subr.c',
		'sys/net/if_lagg.c',
		'sys/net/if_llatbl.c',
		'sys/net/if_loop.c',
		'sys/net/if_media.c',
		'sys/net/if_mib.c',
		'sys/net/if_spppfr.c',
		'sys/net/if_spppsubr.c',
		'sys/net/if_tap.c',
		'sys/net/if_tun.c',
		'sys/net/if_vlan.c',
		'sys/net/pfil.c',
		'sys/net/radix.c',
		'sys/net/radix_mpath.c',
		'sys/net/raw_cb.c',
		'sys/net/raw_usrreq.c',
		'sys/net/route.c',
		'sys/net/rtsock.c',
		'sys/net/slcompress.c',
		'sys/net/bpf_buffer.c',
		'sys/net/bpf.c',
		'sys/net/bpf_filter.c',
		'sys/net/bpf_jitter.c',
		'sys/net/if_arcsubr.c',
		'sys/net/if_bridge.c',
		'sys/net/if_ethersubr.c',
		'sys/net/netisr.c',
	]
)

netinet = Module('netinet')
netinet.addHeaderFiles(
	[
		'sys/netinet/cc.h',
		'sys/netinet/cc/cc_module.h',
		'sys/netinet/icmp6.h',
		'sys/netinet/icmp_var.h',
		'sys/netinet/if_atm.h',
		'sys/netinet/if_ether.h',
		'sys/netinet/igmp.h',
		'sys/netinet/igmp_var.h',
		'sys/netinet/in_gif.h',
		'sys/netinet/in.h',
		'sys/netinet/in_pcb.h',
		'sys/netinet/in_systm.h',
		'sys/netinet/in_var.h',
		'sys/netinet/ip6.h',
		'sys/netinet/ip_carp.h',
		'sys/netinet/ip_divert.h',
		'sys/netinet/ip_dummynet.h',
		'sys/netinet/ip_ecn.h',
		'sys/netinet/ip_encap.h',
		'sys/netinet/ip_fw.h',
		'sys/netinet/ip_gre.h',
		'sys/netinet/ip.h',
		'sys/netinet/ip_icmp.h',
		'sys/netinet/ip_ipsec.h',
		'sys/netinet/ip_mroute.h',
		'sys/netinet/ip_options.h',
		'sys/netinet/ip_var.h',
		'sys/netpfil/ipfw/dn_heap.h',
		'sys/netpfil/ipfw/dn_sched.h',
		'sys/netpfil/ipfw/ip_dn_private.h',
		'sys/netpfil/ipfw/ip_fw_private.h',
		'sys/netinet/pim.h',
		'sys/netinet/pim_var.h',
		'sys/netinet/sctp_asconf.h',
		'sys/netinet/sctp_auth.h',
		'sys/netinet/sctp_bsd_addr.h',
		'sys/netinet/sctp_constants.h',
		'sys/netinet/sctp_crc32.h',
		'sys/netinet/sctp_dtrace_declare.h',
		'sys/netinet/sctp_dtrace_define.h',
		'sys/netinet/sctp.h',
		'sys/netinet/sctp_header.h',
		'sys/netinet/sctp_indata.h',
		'sys/netinet/sctp_input.h',
		'sys/netinet/sctp_lock_bsd.h',
		'sys/netinet/sctp_os_bsd.h',
		'sys/netinet/sctp_os.h',
		'sys/netinet/sctp_output.h',
		'sys/netinet/sctp_pcb.h',
		'sys/netinet/sctp_peeloff.h',
		'sys/netinet/sctp_structs.h',
		'sys/netinet/sctp_sysctl.h',
		'sys/netinet/sctp_timer.h',
		'sys/netinet/sctp_uio.h',
		'sys/netinet/sctputil.h',
		'sys/netinet/sctp_var.h',
		'sys/netinet/tcp_debug.h',
		'sys/netinet/tcp_fsm.h',
		'sys/netinet/tcp.h',
		'sys/netinet/tcp_hostcache.h',
		'sys/netinet/tcpip.h',
		'sys/netinet/tcp_lro.h',
		'sys/netinet/tcp_offload.h',
		'sys/netinet/tcp_seq.h',
		'sys/netinet/tcp_syncache.h',
		'sys/netinet/tcp_timer.h',
		'sys/netinet/tcp_var.h',
		'sys/netinet/toecore.h',
		'sys/netinet/udp.h',
		'sys/netinet/udp_var.h',
		'sys/netinet/libalias/alias_local.h',
		'sys/netinet/libalias/alias.h',
		'sys/netinet/libalias/alias_mod.h',
		'sys/netinet/libalias/alias_sctp.h',
	]
)
# in_cksum.c is architecture dependent
netinet.addSourceFiles(
	[
		'sys/netinet/accf_data.c',
		'sys/netinet/accf_dns.c',
		'sys/netinet/accf_http.c',
		'sys/netinet/cc/cc.c',
		'sys/netinet/cc/cc_newreno.c',
		'sys/netinet/if_atm.c',
		'sys/netinet/if_ether.c',
		'sys/netinet/igmp.c',
		'sys/netinet/in.c',
		'sys/netinet/in_gif.c',
		'sys/netinet/in_mcast.c',
		'sys/netinet/in_pcb.c',
		'sys/netinet/in_proto.c',
		'sys/netinet/in_rmx.c',
		'sys/netinet/ip_carp.c',
		'sys/netinet/ip_divert.c',
		'sys/netinet/ip_ecn.c',
		'sys/netinet/ip_encap.c',
		'sys/netinet/ip_fastfwd.c',
		'sys/netinet/ip_gre.c',
		'sys/netinet/ip_icmp.c',
		'sys/netinet/ip_id.c',
		'sys/netinet/ip_input.c',
		'sys/netinet/ip_ipsec.c',
		'sys/netinet/ip_mroute.c',
		'sys/netinet/ip_options.c',
		'sys/netinet/ip_output.c',
		'sys/netinet/raw_ip.c',
		'sys/netinet/sctp_asconf.c',
		'sys/netinet/sctp_auth.c',
		'sys/netinet/sctp_bsd_addr.c',
		'sys/netinet/sctp_cc_functions.c',
		'sys/netinet/sctp_crc32.c',
		'sys/netinet/sctp_indata.c',
		'sys/netinet/sctp_input.c',
		'sys/netinet/sctp_output.c',
		'sys/netinet/sctp_pcb.c',
		'sys/netinet/sctp_peeloff.c',
		'sys/netinet/sctp_sysctl.c',
		'sys/netinet/sctp_timer.c',
		'sys/netinet/sctp_usrreq.c',
		'sys/netinet/sctputil.c',
		'sys/netinet/tcp_debug.c',
		#'netinet/tcp_hostcache.c',
		'sys/netinet/tcp_input.c',
		'sys/netinet/tcp_lro.c',
		'sys/netinet/tcp_offload.c',
		'sys/netinet/tcp_output.c',
		'sys/netinet/tcp_reass.c',
		'sys/netinet/tcp_sack.c',
		'sys/netinet/tcp_subr.c',
		'sys/netinet/tcp_syncache.c',
		'sys/netinet/tcp_timer.c',
		'sys/netinet/tcp_timewait.c',
		'sys/netinet/tcp_usrreq.c',
		'sys/netpfil/ipfw/dn_heap.c',
		'sys/netpfil/ipfw/dn_sched_fifo.c',
		'sys/netpfil/ipfw/dn_sched_prio.c',
		'sys/netpfil/ipfw/dn_sched_qfq.c',
		'sys/netpfil/ipfw/dn_sched_rr.c',
		'sys/netpfil/ipfw/dn_sched_wf2q.c',
		'sys/netpfil/ipfw/ip_dn_glue.c',
		'sys/netpfil/ipfw/ip_dn_io.c',
		'sys/netpfil/ipfw/ip_dummynet.c',
		'sys/netpfil/ipfw/ip_fw2.c',
		#'sys/netpfil/ipfw/ip_fw_dynamic.c',
		'sys/netpfil/ipfw/ip_fw_log.c',
		'sys/netpfil/ipfw/ip_fw_nat.c',
		'sys/netpfil/ipfw/ip_fw_pfil.c',
		'sys/netpfil/ipfw/ip_fw_sockopt.c',
		'sys/netpfil/ipfw/ip_fw_table.c',
		'sys/netinet/udp_usrreq.c',
		'sys/netinet/libalias/alias_dummy.c',
		'sys/netinet/libalias/alias_pptp.c',
		'sys/netinet/libalias/alias_smedia.c',
		'sys/netinet/libalias/alias_mod.c',
		'sys/netinet/libalias/alias_cuseeme.c',
		'sys/netinet/libalias/alias_nbt.c',
		'sys/netinet/libalias/alias_irc.c',
		'sys/netinet/libalias/alias_util.c',
		'sys/netinet/libalias/alias_db.c',
		'sys/netinet/libalias/alias_ftp.c',
		'sys/netinet/libalias/alias_proxy.c',
		'sys/netinet/libalias/alias.c',
		'sys/netinet/libalias/alias_skinny.c',
		'sys/netinet/libalias/alias_sctp.c',
	]
)

netinet6 = Module('netinet6')
netinet6.conditionalOn = "DISABLE_IPV6"
netinet6.cppPattern = 's/^\#define INET6 1/\/\/ \#define INET6 1/'
netinet6.addHeaderFiles(
	[
		'sys/netinet6/icmp6.h',
		'sys/netinet6/in6_gif.h',
		'sys/netinet6/in6.h',
		'sys/netinet6/in6_ifattach.h',
		'sys/netinet6/in6_pcb.h',
		'sys/netinet6/in6_var.h',
		'sys/netinet6/ip6_ecn.h',
		'sys/netinet6/ip6.h',
		'sys/netinet6/ip6_ipsec.h',
		'sys/netinet6/ip6_mroute.h',
		'sys/netinet6/ip6protosw.h',
		'sys/netinet6/ip6_var.h',
		'sys/netinet6/mld6.h',
		'sys/netinet6/mld6_var.h',
		'sys/netinet6/nd6.h',
		'sys/netinet6/pim6.h',
		'sys/netinet6/pim6_var.h',
		'sys/netinet6/raw_ip6.h',
		'sys/netinet6/scope6_var.h',
		'sys/netinet6/sctp6_var.h',
		'sys/netinet6/send.h',
		'sys/netinet6/tcp6_var.h',
		'sys/netinet6/udp6_var.h',
	]
)
netinet6.addSourceFiles(
	[
		'sys/net/if_stf.c',
		'sys/netinet6/dest6.c',
		'sys/netinet6/frag6.c',
		'sys/netinet6/icmp6.c',
		'sys/netinet6/in6.c',
		'sys/netinet6/in6_cksum.c',
		'sys/netinet6/in6_gif.c',
		'sys/netinet6/in6_ifattach.c',
		'sys/netinet6/in6_mcast.c',
		'sys/netinet6/in6_pcb.c',
		'sys/netinet6/in6_proto.c',
		'sys/netinet6/in6_rmx.c',
		'sys/netinet6/in6_src.c',
		'sys/netinet6/ip6_forward.c',
		'sys/netinet6/ip6_id.c',
		'sys/netinet6/ip6_input.c',
		'sys/netinet6/ip6_ipsec.c',
		'sys/netinet6/ip6_mroute.c',
		'sys/netinet6/ip6_output.c',
		'sys/netinet6/mld6.c',
		'sys/netinet6/nd6.c',
		'sys/netinet6/nd6_nbr.c',
		'sys/netinet6/nd6_rtr.c',
		'sys/netinet6/raw_ip6.c',
		'sys/netinet6/route6.c',
		'sys/netinet6/scope6.c',
		'sys/netinet6/sctp6_usrreq.c',
		'sys/netinet6/udp6_usrreq.c',
	]
)

netipsec = Module('netipsec')
netipsec.addHeaderFiles(
	[
		'sys/netipsec/ah.h',
		'sys/netipsec/ah_var.h',
		'sys/netipsec/esp.h',
		'sys/netipsec/esp_var.h',
		'sys/netipsec/ipcomp.h',
		'sys/netipsec/ipcomp_var.h',
		'sys/netipsec/ipip_var.h',
		'sys/netipsec/ipsec6.h',
		'sys/netipsec/ipsec.h',
		'sys/netipsec/keydb.h',
		'sys/netipsec/key_debug.h',
		'sys/netipsec/key.h',
		'sys/netipsec/keysock.h',
		'sys/netipsec/key_var.h',
		'sys/netipsec/xform.h',
	]
)
netipsec.addSourceFiles(
	[
		'sys/netipsec/ipsec.c',
		'sys/netipsec/ipsec_input.c',
		'sys/netipsec/ipsec_mbuf.c',
		'sys/netipsec/ipsec_output.c',
		'sys/netipsec/key.c',
		'sys/netipsec/key_debug.c',
		'sys/netipsec/keysock.c',
		'sys/netipsec/xform_ah.c',
		'sys/netipsec/xform_esp.c',
		'sys/netipsec/xform_ipcomp.c',
		'sys/netipsec/xform_ipip.c',
		'sys/netipsec/xform_tcp.c',
	]
)

net80211 = Module('net80211')
net80211.addHeaderFiles(
	[
		'sys/net80211/ieee80211_action.h',
		'sys/net80211/ieee80211_adhoc.h',
		'sys/net80211/ieee80211_ageq.h',
		'sys/net80211/ieee80211_amrr.h',
		'sys/net80211/ieee80211_crypto.h',
		'sys/net80211/ieee80211_dfs.h',
		'sys/net80211/ieee80211_freebsd.h',
		'sys/net80211/_ieee80211.h',
		'sys/net80211/ieee80211.h',
		'sys/net80211/ieee80211_hostap.h',
		'sys/net80211/ieee80211_ht.h',
		'sys/net80211/ieee80211_input.h',
		'sys/net80211/ieee80211_ioctl.h',
		'sys/net80211/ieee80211_mesh.h',
		'sys/net80211/ieee80211_monitor.h',
		'sys/net80211/ieee80211_node.h',
		'sys/net80211/ieee80211_phy.h',
		'sys/net80211/ieee80211_power.h',
		'sys/net80211/ieee80211_proto.h',
		'sys/net80211/ieee80211_radiotap.h',
		'sys/net80211/ieee80211_ratectl.h',
		'sys/net80211/ieee80211_regdomain.h',
		'sys/net80211/ieee80211_rssadapt.h',
		'sys/net80211/ieee80211_scan.h',
		'sys/net80211/ieee80211_sta.h',
		'sys/net80211/ieee80211_superg.h',
		'sys/net80211/ieee80211_tdma.h',
		'sys/net80211/ieee80211_var.h',
		'sys/net80211/ieee80211_wds.h',
	]
)
netipsec.addSourceFiles(
	[
		'sys/net80211/ieee80211_acl.c',
		'sys/net80211/ieee80211_action.c',
		'sys/net80211/ieee80211_adhoc.c',
		'sys/net80211/ieee80211_ageq.c',
		'sys/net80211/ieee80211_amrr.c',
		'sys/net80211/ieee80211.c',
		'sys/net80211/ieee80211_crypto.c',
		'sys/net80211/ieee80211_crypto_ccmp.c',
		'sys/net80211/ieee80211_crypto_none.c',
		'sys/net80211/ieee80211_crypto_tkip.c',
		'sys/net80211/ieee80211_crypto_wep.c',
		'sys/net80211/ieee80211_ddb.c',
		'sys/net80211/ieee80211_dfs.c',
		'sys/net80211/ieee80211_freebsd.c',
		'sys/net80211/ieee80211_hostap.c',
		'sys/net80211/ieee80211_ht.c',
		'sys/net80211/ieee80211_hwmp.c',
		'sys/net80211/ieee80211_input.c',
		'sys/net80211/ieee80211_ioctl.c',
		'sys/net80211/ieee80211_mesh.c',
		'sys/net80211/ieee80211_monitor.c',
		'sys/net80211/ieee80211_node.c',
		'sys/net80211/ieee80211_output.c',
		'sys/net80211/ieee80211_phy.c',
		'sys/net80211/ieee80211_power.c',
		'sys/net80211/ieee80211_proto.c',
		'sys/net80211/ieee80211_radiotap.c',
		'sys/net80211/ieee80211_ratectl.c',
		'sys/net80211/ieee80211_ratectl_none.c',
		'sys/net80211/ieee80211_regdomain.c',
		'sys/net80211/ieee80211_rssadapt.c',
		'sys/net80211/ieee80211_scan.c',
		'sys/net80211/ieee80211_scan_sta.c',
		'sys/net80211/ieee80211_sta.c',
		'sys/net80211/ieee80211_superg.c',
		'sys/net80211/ieee80211_tdma.c',
		'sys/net80211/ieee80211_wds.c',
		'sys/net80211/ieee80211_xauth.c',
	]
)

opencrypto = Module('opencrypto')
opencrypto.addHeaderFiles(
	[
		'sys/opencrypto/deflate.h',
		'sys/opencrypto/xform.h',
		'sys/opencrypto/cryptosoft.h',
		'sys/opencrypto/rmd160.h',
		'sys/opencrypto/cryptodev.h',
		'sys/opencrypto/castsb.h',
		'sys/opencrypto/skipjack.h',
		'sys/opencrypto/cast.h',
	]
)
opencrypto.addSourceFiles(
	[
		'sys/opencrypto/crypto.c',
		'sys/opencrypto/deflate.c',
		'sys/opencrypto/cryptosoft.c',
		'sys/opencrypto/criov.c',
		'sys/opencrypto/rmd160.c',
		'sys/opencrypto/xform.c',
		'sys/opencrypto/skipjack.c',
		'sys/opencrypto/cast.c',
	]
)

crypto = Module('crypto')
crypto.addHeaderFiles(
	[
		#'crypto/aesni/aesni.h',
		'sys/crypto/sha1.h',
		'sys/crypto/sha2/sha2.h',
		'sys/crypto/rijndael/rijndael.h',
		'sys/crypto/rijndael/rijndael_local.h',
		'sys/crypto/rijndael/rijndael-api-fst.h',
		'sys/crypto/des/des.h',
		'sys/crypto/des/spr.h',
		'sys/crypto/des/podd.h',
		'sys/crypto/des/sk.h',
		'sys/crypto/des/des_locl.h',
		'sys/crypto/blowfish/bf_pi.h',
		'sys/crypto/blowfish/bf_locl.h',
		'sys/crypto/blowfish/blowfish.h',
		'sys/crypto/rc4/rc4.h',
		#'crypto/via/padlock.h',
		'sys/crypto/camellia/camellia.h',
	]
)
crypto.addSourceFiles(
	[
		#'crypto/aesni/aesni.c',
		#'crypto/aesni/aesni_wrap.c',
		'sys/crypto/sha1.c',
		'sys/crypto/sha2/sha2.c',
		'sys/crypto/rijndael/rijndael-alg-fst.c',
		'sys/crypto/rijndael/rijndael-api.c',
		'sys/crypto/rijndael/rijndael-api-fst.c',
		'sys/crypto/des/des_setkey.c',
		'sys/crypto/des/des_enc.c',
		'sys/crypto/des/des_ecb.c',
		'sys/crypto/blowfish/bf_enc.c',
		'sys/crypto/blowfish/bf_skey.c',
		'sys/crypto/blowfish/bf_ecb.c',
		'sys/crypto/rc4/rc4.c',
		#'crypto/via/padlock.c',
		#'crypto/via/padlock_cipher.c',
		#'crypto/via/padlock_hash.c',
		'sys/crypto/camellia/camellia-api.c',
		'sys/crypto/camellia/camellia.c',
	]
)

altq = Module('altq')
altq.addHeaderFiles(
	[
		'sys/contrib/altq/altq/altq_rmclass.h',
		'sys/contrib/altq/altq/altq_cbq.h',
		'sys/contrib/altq/altq/altq_var.h',
		'sys/contrib/altq/altq/altqconf.h',
		'sys/contrib/altq/altq/altq.h',
		'sys/contrib/altq/altq/altq_hfsc.h',
		'sys/contrib/altq/altq/altq_red.h',
		'sys/contrib/altq/altq/altq_classq.h',
		'sys/contrib/altq/altq/altq_priq.h',
		'sys/contrib/altq/altq/altq_rmclass_debug.h',
		'sys/contrib/altq/altq/altq_cdnr.h',
		'sys/contrib/altq/altq/altq_rio.h',
		'sys/contrib/altq/altq/if_altq.h',
	]
)
altq.addSourceFiles(
	[
		'sys/contrib/altq/altq/altq_rmclass.c',
		'sys/contrib/altq/altq/altq_rio.c',
		'sys/contrib/altq/altq/altq_subr.c',
		'sys/contrib/altq/altq/altq_cdnr.c',
		'sys/contrib/altq/altq/altq_priq.c',
		'sys/contrib/altq/altq/altq_cbq.c',
		'sys/contrib/altq/altq/altq_hfsc.c',
		'sys/contrib/altq/altq/altq_red.c',
	]
)

# contrib/pf Module
pf = Module('pf')
pf.addHeaderFiles(
	[
		'sys/contrib/pf/net/if_pflog.h',
		'sys/contrib/pf/net/if_pflow.h',
		'sys/contrib/pf/net/if_pfsync.h',
		'sys/contrib/pf/net/pfvar.h',
		'sys/contrib/pf/net/pf_mtag.h',
	]
)
pf.addSourceFiles(
	[
		'sys/contrib/pf/net/if_pflog.c',
		'sys/contrib/pf/net/if_pfsync.c',
		'sys/contrib/pf/net/pf.c',
		'sys/contrib/pf/net/pf_if.c',
		'sys/contrib/pf/net/pf_ioctl.c',
		'sys/contrib/pf/net/pf_lb.c',
		'sys/contrib/pf/net/pf_norm.c',
		'sys/contrib/pf/net/pf_osfp.c',
		'sys/contrib/pf/net/pf_ruleset.c',
		'sys/contrib/pf/net/pf_table.c',
		'sys/contrib/pf/netinet/in4_cksum.c',
	]
)

pci = Module('pci')
pci.addSourceFiles(
	[
		'sys/dev/pci/pci.c',
		'sys/dev/pci/pci_user.c',
		'sys/dev/pci/pci_pci.c',
	]
)
pci.addHeaderFiles(
	[
		'sys/dev/pci/pcib_private.h',
		'sys/dev/pci/pci_private.h',
		'sys/dev/pci/pcireg.h',
		'sys/dev/pci/pcivar.h',
		'sys/dev/pci/pcivar.h',
	]
)
pci.addCPUDependentHeaderFiles(
	[
		'sys/i386/include/legacyvar.h',
	]
)
pci.addTargetSourceCPUDependentHeaderFiles(
	[ 'arm', 'avr', 'bfin', 'h8300', 'lm32', 'm32c', 'm32r', 'm68k', 'mips', 'nios2', 'powerpc', 'sh', 'sparc', 'sparc64', 'v850' ],
	'i386',
	[
		'sys/i386/include/legacyvar.h',
	]
)
pci.addTargetSourceCPUDependentHeaderFiles(
	[ 'arm', 'avr', 'bfin', 'h8300', 'i386', 'lm32', 'm32c', 'm32r', 'm68k', 'mips', 'nios2', 'powerpc', 'sh', 'sparc', 'sparc64', 'v850' ],
	'x86',
	[
		'sys/x86/include/pci_cfgreg.h',
	]
)
pci.addCPUDependentSourceFiles(
	'i386',
	[
		'sys/i386/i386/legacy.c',
	]
)
pci.addTargetSourceCPUDependentSourceFiles(
	[ 'arm', 'avr', 'bfin', 'h8300', 'lm32', 'm32c', 'm32r', 'm68k', 'mips', 'nios2', 'powerpc', 'sh', 'sparc', 'sparc64', 'v850' ],
	'i386',
	[
		'sys/i386/i386/legacy.c',
	]
)
pci.addTargetSourceCPUDependentSourceFiles(
	[ 'arm', 'avr', 'bfin', 'h8300', 'i386', 'lm32', 'm32c', 'm32r', 'm68k', 'mips', 'nios2', 'powerpc', 'sh', 'sparc', 'sparc64', 'v850' ],
	'x86',
	[
		'sys/x86/pci/pci_bus.c',
	]
)

userSpace = Module('userSpace')
userSpace.addUserSpaceHeaderFiles(
	[
		'include/arpa/ftp.h',
		'include/arpa/inet.h',
		'include/arpa/nameser_compat.h',
		'include/arpa/nameser.h',
		'include/db.h',
		'include/err.h',
		'include/ifaddrs.h',
		'include/mpool.h',
		'include/netconfig.h',
		'include/netdb.h',
		'include/nlist.h',
		'include/nsswitch.h',
		'include/resolv.h',
		'include/res_update.h',
		'include/rpc/auth_des.h',
		'include/rpc/auth.h',
		'include/rpc/auth_unix.h',
		'include/rpc/clnt.h',
		'include/rpc/clnt_soc.h',
		'include/rpc/clnt_stat.h',
		'include/rpc/pmap_clnt.h',
		'include/rpc/pmap_prot.h',
		'include/rpc/rpcb_clnt.h',
		'include/rpc/rpcent.h',
		'include/rpc/rpc.h',
		'include/rpc/rpc_msg.h',
		'include/rpc/svc_auth.h',
		'include/rpc/svc.h',
		'include/rpcsvc/nis_db.h',
		'include/rpcsvc/nislib.h',
		'include/rpcsvc/nis_tags.h',
		'include/rpc/svc_soc.h',
		'include/rpcsvc/ypclnt.h',
		'include/rpcsvc/yp_prot.h',
		'include/rpc/xdr.h',
		'include/sysexits.h',
		'lib/libc/db/btree/btree.h',
		'lib/libc/db/btree/extern.h',
		'lib/libc/db/recno/extern.h',
		'lib/libc/db/recno/recno.h',
		'lib/libc/include/isc/eventlib.h',
		'lib/libc/include/isc/list.h',
		'lib/libc/include/isc/platform.h',
		'lib/libc/include/libc_private.h',
		'lib/libc/include/namespace.h',
		'lib/libc/include/nss_tls.h',
		'lib/libc/include/port_after.h',
		'lib/libc/include/port_before.h',
		'lib/libc/include/reentrant.h',
		'lib/libc/include/resolv_mt.h',
		'lib/libc/include/spinlock.h',
		'lib/libc/include/un-namespace.h',
		'lib/libc/isc/eventlib_p.h',
		'lib/libc/net/netdb_private.h',
		'lib/libc/net/nss_backends.h',
		'lib/libc/net/res_config.h',
		'lib/libc/resolv/res_debug.h',
		'lib/libc/resolv/res_private.h',
		'lib/libc/stdio/local.h',
		'lib/libipsec/ipsec_strerror.h',
		'lib/libipsec/libpfkey.h',
		'lib/libkvm/kvm.h',
		'lib/libmemstat/memstat.h',
		'lib/libmemstat/memstat_internal.h',
		'lib/libutil/libutil.h',
		'sbin/dhclient/dhcpd.h',
		'sbin/dhclient/dhcp.h',
		'sbin/dhclient/dhctoken.h',
		'sbin/dhclient/privsep.h',
		'sbin/dhclient/tree.h',
		'sbin/ifconfig/ifconfig.h',
		'sbin/ifconfig/regdomain.h',
		'usr.bin/netstat/netstat.h',
	]
)

class RPCGENMakefileFragmentComposer(MakefileFragmentComposer):
	def compose(self, path):
		headerPath = path[:-2] + '.h'
		return 'LIB_GEN_FILES += ' + headerPath + '\n' \
			+ headerPath + ': ' + path + '\n' \
			'\t	rm -f $@\n' \
			'\t	rpcgen -h -o $@ $<\n'

userSpace.addFile(File('include/rpc/rpcb_prot.x', PathComposer(), Converter(), Converter(), RPCGENMakefileFragmentComposer()))

class RouteKeywordsMakefileFragmentComposer(MakefileFragmentComposer):
	def compose(self, path):
		headerPath = path + '.h'
		tmpPath = path + '.tmp'
		return 'LIB_GEN_FILES += ' + headerPath + '\n' \
			+ headerPath + ': ' + path + '\n' \
			'\tsed -e \'/^#/d\' -e \'/^$$/d\' $< > ' + tmpPath + '\n' \
			'\tLC_ALL=C tr \'a-z\' \'A-Z\' < ' + tmpPath + ' | paste ' + tmpPath + ' - | \\\n' \
			'\tawk \'{ if (NF > 1) printf "#define\\tK_%s\\t%d\\n\\t{\\"%s\\", K_%s},\\n", $$2, NR, $$1, $$2 }\' > $@\n' \
			'\trm -f ' + tmpPath + '\n'

userSpace.addFile(File('sbin/route/keywords', PathComposer(), Converter(), Converter(), RouteKeywordsMakefileFragmentComposer()))

class LexMakefileFragmentComposer(MakefileFragmentComposer):
	def __init__(self, sym, dep):
		self.sym = sym
		self.dep = dep

	def compose(self, path):
		src = path[:-2] + '.c'
		dep = path[:path.rfind('/')] + '/' + self.dep
		return 'LIB_GEN_FILES += ' + src + '\n' \
			'LIB_C_FILES += ' + src + '\n' \
			+ src + ': ' + path + ' ' + dep + '\n' \
			'\t${LEX} -P ' + self.sym + ' -t $< | sed -e \'/YY_BUF_SIZE/s/16384/1024/\' > $@\n'

class YaccMakefileFragmentComposer(MakefileFragmentComposer):
	def __init__(self, sym, header):
		self.sym = sym
		self.header = header

	def compose(self, path):
		src = path[:-2] + '.c'
		hdr = path[:path.rfind('/')] + '/' + self.header
		return 'LIB_GEN_FILES += ' + src + '\n' \
			'LIB_C_FILES += ' + src + '\n' \
			+ src + ': ' + path + '\n' \
			'\tyacc -b ' + self.sym + ' -d -p ' + self.sym + ' $<\n' \
			'\tsed -e ''/YY_BUF_SIZE/s/16384/1024/'' < ' + self.sym + '.tab.c > $@\n' \
			'\trm -f ' + self.sym + '.tab.c\n' \
			'\tmv ' + self.sym + '.tab.h ' + hdr + '\n'

rtems.addFile(File('lib/libc/net/nslexer.l', PathComposer(), Converter(), Converter(), LexMakefileFragmentComposer('_nsyy', 'nsparser.c')))
rtems.addFile(File('lib/libc/net/nsparser.y', PathComposer(), Converter(), Converter(), YaccMakefileFragmentComposer('_nsyy', 'nsparser.h')))
rtems.addFile(File('lib/libipsec/policy_token.l', PathComposer(), Converter(), Converter(), LexMakefileFragmentComposer('__libipsecyy', 'policy_parse.c')))
rtems.addFile(File('lib/libipsec/policy_parse.y', PathComposer(), Converter(), Converter(), YaccMakefileFragmentComposer('__libipsecyy', 'y.tab.h')))

userSpace.addUserSpaceSourceFiles(
	[
		'lib/libc/db/btree/bt_close.c',
		'lib/libc/db/btree/bt_conv.c',
		'lib/libc/db/btree/bt_debug.c',
		'lib/libc/db/btree/bt_delete.c',
		'lib/libc/db/btree/bt_get.c',
		'lib/libc/db/btree/bt_open.c',
		'lib/libc/db/btree/bt_overflow.c',
		'lib/libc/db/btree/bt_page.c',
		'lib/libc/db/btree/bt_put.c',
		'lib/libc/db/btree/bt_search.c',
		'lib/libc/db/btree/bt_seq.c',
		'lib/libc/db/btree/bt_split.c',
		'lib/libc/db/btree/bt_utils.c',
		'lib/libc/db/db/db.c',
		'lib/libc/db/mpool/mpool.c',
		'lib/libc/db/mpool/mpool-compat.c',
		'lib/libc/db/recno/rec_close.c',
		'lib/libc/db/recno/rec_delete.c',
		'lib/libc/db/recno/rec_get.c',
		'lib/libc/db/recno/rec_open.c',
		'lib/libc/db/recno/rec_put.c',
		'lib/libc/db/recno/rec_search.c',
		'lib/libc/db/recno/rec_seq.c',
		'lib/libc/db/recno/rec_utils.c',
		'lib/libc/gen/err.c',
		'lib/libc/gen/feature_present.c',
		'lib/libc/gen/gethostname.c',
		'lib/libc/inet/inet_addr.c',
		'lib/libc/inet/inet_cidr_ntop.c',
		'lib/libc/inet/inet_cidr_pton.c',
		'lib/libc/inet/inet_lnaof.c',
		'lib/libc/inet/inet_makeaddr.c',
		'lib/libc/inet/inet_neta.c',
		'lib/libc/inet/inet_net_ntop.c',
		'lib/libc/inet/inet_netof.c',
		'lib/libc/inet/inet_net_pton.c',
		'lib/libc/inet/inet_network.c',
		'lib/libc/inet/inet_ntoa.c',
		'lib/libc/inet/inet_ntop.c',
		'lib/libc/inet/inet_pton.c',
		'lib/libc/inet/nsap_addr.c',
		'lib/libc/isc/ev_streams.c',
		'lib/libc/isc/ev_timers.c',
		'lib/libc/nameser/ns_name.c',
		'lib/libc/nameser/ns_netint.c',
		'lib/libc/nameser/ns_parse.c',
		'lib/libc/nameser/ns_print.c',
		'lib/libc/nameser/ns_samedomain.c',
		'lib/libc/nameser/ns_ttl.c',
		'lib/libc/net/base64.c',
		'lib/libc/net/ether_addr.c',
		'lib/libc/net/gai_strerror.c',
		'lib/libc/net/getaddrinfo.c',
		'lib/libc/net/gethostbydns.c',
		'lib/libc/net/gethostbyht.c',
		'lib/libc/net/gethostbynis.c',
		'lib/libc/net/gethostnamadr.c',
		'lib/libc/net/getifaddrs.c',
		'lib/libc/net/getifmaddrs.c',
		'lib/libc/net/getnameinfo.c',
		'lib/libc/net/getnetbydns.c',
		'lib/libc/net/getnetbyht.c',
		'lib/libc/net/getnetbynis.c',
		'lib/libc/net/getnetnamadr.c',
		'lib/libc/net/getproto.c',
		'lib/libc/net/getprotoent.c',
		'lib/libc/net/getprotoname.c',
		'lib/libc/net/getservent.c',
		'lib/libc/net/if_indextoname.c',
		'lib/libc/net/if_nameindex.c',
		'lib/libc/net/if_nametoindex.c',
		'lib/libc/net/ip6opt.c',
		'lib/libc/net/linkaddr.c',
		'lib/libc/net/map_v4v6.c',
		'lib/libc/net/name6.c',
		'lib/libc/net/nsdispatch.c',
		'lib/libc/net/rcmd.c',
		'lib/libc/net/recv.c',
		'lib/libc/net/rthdr.c',
		'lib/libc/net/send.c',
		'lib/libc/posix1e/mac.c',
		'lib/libc/resolv/h_errno.c',
		'lib/libc/resolv/herror.c',
		'lib/libc/resolv/mtctxres.c',
		'lib/libc/resolv/res_comp.c',
		'lib/libc/resolv/res_data.c',
		'lib/libc/resolv/res_debug.c',
		'lib/libc/resolv/res_findzonecut.c',
		'lib/libc/resolv/res_init.c',
		'lib/libc/resolv/res_mkquery.c',
		'lib/libc/resolv/res_mkupdate.c',
		'lib/libc/resolv/res_query.c',
		'lib/libc/resolv/res_send.c',
		'lib/libc/resolv/res_state.c',
		'lib/libc/resolv/res_update.c',
		'lib/libc/stdio/fgetln.c',
		'lib/libc/stdlib/strtonum.c',
		'lib/libc/string/strsep.c',
		'lib/libipsec/ipsec_dump_policy.c',
		'lib/libipsec/ipsec_get_policylen.c',
		'lib/libipsec/ipsec_strerror.c',
		'lib/libipsec/pfkey.c',
		'lib/libipsec/pfkey_dump.c',
		'lib/libmemstat/memstat_all.c',
		'lib/libmemstat/memstat.c',
		'lib/libmemstat/memstat_malloc.c',
		'lib/libmemstat/memstat_uma.c',
		'lib/libutil/expand_number.c',
		'lib/libutil/humanize_number.c',
		'lib/libutil/trimdomain.c',
		'sbin/dhclient/alloc.c',
		'sbin/dhclient/bpf.c',
		'sbin/dhclient/clparse.c',
		'sbin/dhclient/conflex.c',
		'sbin/dhclient/convert.c',
		'sbin/dhclient/dhclient.c',
		'sbin/dhclient/dispatch.c',
		'sbin/dhclient/errwarn.c',
		'sbin/dhclient/hash.c',
		'sbin/dhclient/inet.c',
		'sbin/dhclient/options.c',
		'sbin/dhclient/packet.c',
		'sbin/dhclient/parse.c',
		'sbin/dhclient/privsep.c',
		'sbin/dhclient/tables.c',
		'sbin/dhclient/tree.c',
		'sbin/ifconfig/af_atalk.c',
		'sbin/ifconfig/af_inet6.c',
		'sbin/ifconfig/af_inet.c',
		'sbin/ifconfig/af_link.c',
		'sbin/ifconfig/af_nd6.c',
		'sbin/ifconfig/ifbridge.c',
		'sbin/ifconfig/ifcarp.c',
		'sbin/ifconfig/ifclone.c',
		'sbin/ifconfig/ifconfig.c',
		'sbin/ifconfig/ifgif.c',
		'sbin/ifconfig/ifgre.c',
		'sbin/ifconfig/ifgroup.c',
		'sbin/ifconfig/iflagg.c',
		'sbin/ifconfig/ifmac.c',
		'sbin/ifconfig/ifmedia.c',
		'sbin/ifconfig/ifpfsync.c',
		'sbin/ifconfig/ifvlan.c',
		'sbin/ping6/ping6.c',
		'sbin/ping/ping.c',
		'sbin/route/route.c',
		'usr.bin/netstat/atalk.c',
		'usr.bin/netstat/bpf.c',
		'usr.bin/netstat/if.c',
		'usr.bin/netstat/inet6.c',
		'usr.bin/netstat/inet.c',
		'usr.bin/netstat/ipsec.c',
		'usr.bin/netstat/main.c',
		'usr.bin/netstat/mbuf.c',
		'usr.bin/netstat/mroute6.c',
		'usr.bin/netstat/mroute.c',
		'usr.bin/netstat/route.c',
		'usr.bin/netstat/pfkey.c',
		'usr.bin/netstat/sctp.c',
		'usr.bin/netstat/unix.c',
	]
)

# in_chksum Module
in_cksum = Module('in_cksum')
in_cksum.addRTEMSHeaderFiles(
	[
        ]
)
in_cksum.addCPUDependentHeaderFiles(
	[
		'sys/i386/include/in_cksum.h',
		'sys/mips/include/in_cksum.h',
		'sys/powerpc/include/in_cksum.h',
		'sys/sparc64/include/in_cksum.h',
	]
)
in_cksum.addTargetSourceCPUDependentHeaderFiles(
	[ 'arm', 'avr', 'bfin', 'h8300', 'lm32', 'm32c', 'm32r', 'm68k', 'nios2', 'sh', 'sparc', 'v850' ],
	'mips',
	[
		'sys/mips/include/in_cksum.h',
	]
)
in_cksum.addTargetSourceCPUDependentSourceFiles(
	[ 'arm', 'avr', 'bfin', 'h8300', 'lm32', 'm32c', 'm32r', 'm68k', 'nios2', 'sh', 'sparc', 'v850' ],
	'mips',
	[
		'sys/mips/mips/in_cksum.c',
	]
)
in_cksum.addCPUDependentSourceFiles(
	'i386',
	[
		'sys/i386/i386/in_cksum.c',
	]
)
in_cksum.addCPUDependentSourceFiles(
	'mips',
	[
		'sys/mips/mips/in_cksum.c',
	]
)
in_cksum.addCPUDependentSourceFiles(
	'powerpc',
	[
		'sys/powerpc/powerpc/in_cksum.c',
	]
)
in_cksum.addCPUDependentSourceFiles(
	'sparc',
	[
		'sys/mips/mips/in_cksum.c',
	]
)
in_cksum.addCPUDependentSourceFiles(
	'sparc64',
	[
		'sys/sparc64/sparc64/in_cksum.c',
	]
)

tests = Module('tests')
tests.addTest('selectpollkqueue01', ['test_main'])
tests.addTest('rwlock01', ['test_main'])
tests.addTest('sleep01', ['test_main'])
tests.addTest('syscalls01', ['test_main'])
tests.addTest('commands01', ['test_main'])
tests.addTest('usb01', ['init', 'test-file-system'], False)
tests.addTest('loopback01', ['test_main'])
tests.addTest('netshell01', ['test_main', 'shellconfig'], False)
tests.addTest('swi01', ['init', 'swi_test'])
tests.addTest('timeout01', ['init', 'timeout_test'])
tests.addTest('init01', ['test_main'])
tests.addTest('thread01', ['test_main'])

# Register all the Module instances with the Module Manager
mm.addModule(rtems)
mm.addModule(base)
mm.addModule(net)
mm.addModule(netinet)
mm.addModule(netinet6)
mm.addModule(netipsec)
mm.addModule(net80211)
mm.addModule(opencrypto)
mm.addModule(crypto)
mm.addModule(altq)
mm.addModule(pf)
mm.addModule(devNet)

mm.addModule(devUsb)
mm.addModule(devUsbQuirk)
mm.addModule(devUsbController)

mm.addModule(cam)
mm.addModule(devUsbStorage)
#mm.addModule(devUsbNet)

# Add PCI
mm.addModule(pci)

# Add NIC devices
mm.addModule(devNic)
mm.addModule(devNic_re)
mm.addModule(devNic_fxp)
mm.addModule(devNic_e1000)
mm.addModule(devNic_dc)
mm.addModule(devNic_smc)
mm.addModule(devNic_broadcomm)
# TBD Requires ISA and PCCard Support to be pulled in.
# mm.addModule(devNic_cs)

# Add in_chksum
mm.addModule(in_cksum)

mm.addModule(userSpace)

mm.addModule(tests)

# XXX TODO Check that no file is also listed in empty
# XXX TODO Check that no file in in two modules

# Perform the actual file manipulation
if isForward:
	if not isOnlyMakefile:
		mm.copyFromFreeBSDToRTEMS()
	mm.createMakefile()
else:
	mm.copyFromRTEMSToFreeBSD()

# Print a summary if changing files
if isDiffMode == False:
  if filesProcessed == 1:
    print str(filesProcessed) + " file was changed."
  else:
    print str(filesProcessed) + " files were changed."
