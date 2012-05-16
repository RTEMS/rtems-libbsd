include ../../config.inc

APP = timeout01

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

C_PIECES = init timeout_test timeout_helper
C_O_FILES = $(C_PIECES:%=%.o)
C_DEP_FILES = $(C_PIECES:%=%.dep)

OBJS= $(C_O_FILES)

DEPFLAGS = -MT $@ -MD -MP -MF $*.dep
AM_CPPFLAGS += -I $(INSTALL_BASE)/include -I.
CLEAN_ADDITIONS += $(APP).exe $(APP).map $(C_DEP_FILES)

CFLAGS += $(DEPFLAGS) $(GCCFLAGS) $(AM_CPPFLAGS) -Wno-unused -Wl,-Map,$(APP).map

LINK_LIBS += $(INSTALL_BASE)/libbsd.a

all: $(APP).exe

$(APP).exe: $(C_O_FILES)
	$(make-exe)

-include $(C_DEP_FILES)
