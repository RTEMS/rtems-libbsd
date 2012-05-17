include ../../config.inc

APP = timeout01
PGM=${ARCH}/$(APP).exe

# optional managers required
MANAGERS=all

# C source names
C_FILES = init.c timeout_helper.c timeout_test.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

AM_CPPFLAGS += -I $(INSTALL_BASE)/include
LINK_LIBS += $(INSTALL_BASE)/libbsd.a ${REL_ARGS}
CFLAGS += $(DEPFLAGS) $(GCCFLAGS) $(AM_CPPFLAGS) -Wno-unused -Wl,-Map,$(APP).map

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

OBJS= $(C_O_FILES)
CLEAN_ADDITIONS += $(APP).map *.dep

all:    ${ARCH} $(PGM)

$(PGM): $(OBJS)
	-$(make-exe)
