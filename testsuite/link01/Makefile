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
LINK_LIBS += $(INSTALL_BASE)/libbsd.a

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

OBJS= $(C_O_FILES)

all:    ${ARCH} $(PGM)

$(PGM): $(OBJS)
	$(make-exe)
