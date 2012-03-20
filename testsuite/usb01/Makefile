#
#  $Id$
#

include ../../config.inc

PGM=${ARCH}/usb01.exe

# optional managers required
MANAGERS=all

# C source names
C_FILES = init.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

AM_CPPFLAGS += -I $(INSTALL_BASE)/include
LINK_LIBS += $(INSTALL_BASE)/libbsd.a -Wl,-Map=jennifer.txt

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

OBJS= $(C_O_FILES)

all:    ${ARCH} $(PGM)

$(PGM): $(OBJS)
	$(make-exe)
