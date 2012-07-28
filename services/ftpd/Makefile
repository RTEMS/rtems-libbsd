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
C_FILES += ftpd.c

C_O_FILES = $(C_FILES:%.c=%.o)
C_D_FILES = $(C_FILES:%.c=%.d)

LIB = libftpd.a

all: $(LIB)

$(LIB): $(C_O_FILES)
	$(AR) rcu $@ $^

install: $(LIB)
	install -d $(INSTALL_BASE)/include/rtems
	install -c -m 644 ftpd.h $(INSTALL_BASE)/
	install -c -m 644 $(LIB) $(INSTALL_BASE)

clean:
	rm -f $(LIB) $(C_O_FILES) $(C_D_FILES) $(GEN_FILES)

-include $(C_D_FILES)

doc: 

