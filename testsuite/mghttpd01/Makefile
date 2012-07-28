#
#  $Id$
#

include ../../config.inc

PGM=${ARCH}/mghttpd01.exe

# optional managers required
MANAGERS=all

# XXX = mghttpd01.doc mghttpd01.scn
# C source names
C_FILES = init.c test-http-client.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

AM_CPPFLAGS += -I $(INSTALL_BASE)/include
AM_CPPFLAGS += -I ../init01
AM_CPPFLAGS += -I ${r}/testsuites/support/include
LINK_LIBS += $(INSTALL_BASE)/libmghttpd.a
LINK_LIBS += $(INSTALL_BASE)/libbsdc.a
LINK_LIBS += $(INSTALL_BASE)/libbsd.a

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

OBJS= $(C_O_FILES)
CLEAN_ADDITIONS += init.c

all:    init.c ${ARCH} $(PGM)

init.c: ../init01/init.c
	cp ../init01/init.c .

$(PGM): $(OBJS)
	-$(make-exe)
