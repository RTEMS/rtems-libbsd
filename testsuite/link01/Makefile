#
#  $Id$
#

include ../../config.inc

PGM=${ARCH}/link01.exe
PGM_R=${ARCH}/link01_r.exe

# optional managers required
MANAGERS=all

# C source names
C_FILES = test.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

AM_CPPFLAGS += -I $(INSTALL_BASE)/include
LINK_LIBS += $(INSTALL_BASE)/libbsdc.a
LINK_LIBS += $(INSTALL_BASE)/libbsd.a ${REL_ARGS}

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

OBJS= $(C_O_FILES)
CLEAN_ADDITIONS += undefined.txt

all:    ${ARCH} $(PGM_R) $(PGM)
	$(NM) $(PGM_R) | grep "U " >undefined.txt
	@echo `cat undefined.txt | wc -l` symbols to resolve

$(PGM): $(OBJS)
	-$(make-exe)

$(PGM_R): $(OBJS)
	$(MAKE) -f Makefile_ldr
