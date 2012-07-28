
include ../../config.inc

APP=usb01
PGM=${ARCH}/$(APP).exe

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

C_PIECES = init test-file-system
C_O_FILES = $(C_PIECES:%=%.o)
C_DEP_FILES = $(C_PIECES:%=%.dep)

OBJS= $(C_O_FILES)

DEPFLAGS = -MT $@ -MD -MP -MF $*.dep
AM_CPPFLAGS += -I $(INSTALL_BASE)/include -I.
CLEAN_ADDITIONS += $(APP).exe $(ARCH)/$(APP).map $(C_DEP_FILES)
CLEAN_ADDITIONS += $(C_DEP_FILES)

CFLAGS += $(DEPFLAGS) $(GCCFLAGS) $(AM_CPPFLAGS)
CFLAGS += -Wno-unused
CFLAGS += -Wl,-Map,$(ARCH)/$(APP).map

LINK_LIBS += $(INSTALL_BASE)/libbsd.a

all: $(ARCH) $(PGM)

$(PGM): $(C_O_FILES)
	$(make-exe)

-include $(C_DEP_FILES)
