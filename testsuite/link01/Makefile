#
#  $Id$
#

include ../../config.inc

PGM=${ARCH}/link01.exe

# optional managers required
MANAGERS=all

# C source names
C_FILES = test.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

AM_CPPFLAGS += -I $(INSTALL_BASE)/include
LINK_LIBS += $(INSTALL_BASE)/libbsd.a -Wl,-r

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

OBJS= $(C_O_FILES)
CLEAN_ADDITIONS += undefined.txt

all:    ${ARCH} $(PGM)

$(PGM): $(OBJS)
	$(make-exe)
	$(NM) $(PGM) | grep "U " >undefined.txt
	@echo `cat undefined.txt | wc -l` symbols to resolve
	@test `cat undefined.txt | wc -l` -ne 0 && exit 1
