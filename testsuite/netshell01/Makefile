include ../../config.inc

PGM=${ARCH}/netshell01.exe

# optional managers required
MANAGERS=all

# C source names
C_FILES = init.c test_main.c shellconfig.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

AM_CPPFLAGS += -I $(INSTALL_BASE)/include
AM_CPPFLAGS += -I ../init01
LINK_LIBS += $(INSTALL_BASE)/libbsdc.a
LINK_LIBS += $(INSTALL_BASE)/libbsd.a -lm ${REL_ARGS} 

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
