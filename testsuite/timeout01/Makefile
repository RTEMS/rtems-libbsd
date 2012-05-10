include ../../config.inc

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

APP_PIECES = init timeout_test timeout_helper

APP_O_FILES = $(APP_PIECES:%=%.o)
APP_DEP_FILES = $(APP_PIECES:%=%.dep)

APP = app.exe

DEPFLAGS = -MT $@ -MD -MP -MF $*.dep
AM_CPPFLAGS += -I $(INSTALL_BASE)/include -I.

CFLAGS += $(DEPFLAGS) $(GCCFLAGS) $(AM_CPPFLAGS) -Wno-unused -Wl,-Map,app.map

LINK_LIBS += $(INSTALL_BASE)/libbsd.a

all: $(APP)

$(APP): $(APP_O_FILES)
	$(CC) $(CFLAGS) $^ $(LINK_LIBS) -o $(APP)

clean:
	rm -f app.map $(APP) $(APP_O_FILES) $(APP_DEP_FILES)

-include $(APP_DEP_FILES)
