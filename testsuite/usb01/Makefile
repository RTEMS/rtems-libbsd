
include ../../config.inc

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

APP_PIECES = init \
	test-file-system

APP_O_FILES = $(APP_PIECES:%=%.o)
APP_DEP_FILES = $(APP_PIECES:%=%.dep)

APP = app.exe

DEPFLAGS = -MT $@ -MD -MP -MF $*.dep
AM_CPPFLAGS += -I $(INSTALL_BASE)/include -I.

CFLAGS += $(DEPFLAGS) $(GCCFLAGS) $(AM_CPPFLAGS) -Wno-unused

LINK_LIBS += $(INSTALL_BASE)/libbsd.a

CXXFLAGS += $(CFLAGS)

all: $(APP)

$(APP): $(APP_O_FILES)
	$(CC) $(CXXFLAGS) $^ $(LINK_LIBS) -o $(APP)

clean:
	rm -f $(APP_O_FILES) $(APP_DEP_FILES)

dist:
	cd .. && tar cjf usb-demo/usb-demo.tar.bz2 \
	  `find usb-demo -name '*.h' -o -name '*.c'` usb-demo/Makefile

%.dep:
	touch $@

include $(APP_DEP_FILES)
