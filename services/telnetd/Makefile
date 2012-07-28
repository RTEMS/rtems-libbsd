include ../../config.inc

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

CFLAGS += -I $(INSTALL_BASE)/include

CFLAGS += -w 
CFLAGS += -I include 
CFLAGS += -std=gnu99
CFLAGS += -MT $@ -MD -MP -MF $(basename $@).d

C_FILES =
C_FILES += check_passwd.c
C_FILES += des.c
C_FILES += pty.c
C_FILES += telnetd.c
# genpw.c is native

C_O_FILES = $(C_FILES:%.c=%.o)
C_D_FILES = $(C_FILES:%.c=%.d)

LIB = libtelnetd.a

all: $(LIB)

$(LIB): $(C_O_FILES)
	$(AR) rcu $@ $^

install: $(LIB)
	install -d $(INSTALL_BASE)/include/rtems
	install -c -m 644 include/rtems/pty.h $(INSTALL_BASE)/rtems
	install -c -m 644 include/rtems/telnetd.h $(INSTALL_BASE)/rtems
	install -c -m 644 include/passwd.h $(INSTALL_BASE)/rtems
	install -c -m 644 $(LIB) $(INSTALL_BASE)

clean:
	rm -f $(LIB) $(C_O_FILES) $(C_D_FILES) $(GEN_FILES)

-include $(C_D_FILES)

doc: 

