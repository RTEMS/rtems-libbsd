include ../../config.inc

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

CFLAGS += -I $(INSTALL_BASE)/include

CFLAGS += -w 
CFLAGS += -std=gnu99
CFLAGS += -MT $@ -MD -MP -MF $(basename $@).d
# CFLAGS += -DHAVE_MD5
CFLAGS += -DNO_SSL -DNO_POPEN -DNO_CGI

C_FILES =
C_FILES += mongoose.c

C_O_FILES = $(C_FILES:%.c=%.o)
C_D_FILES = $(C_FILES:%.c=%.d)

LIB = libmghttpd.a

all: $(LIB)

$(LIB): $(C_O_FILES)
	$(AR) rcu $@ $^

install: $(LIB)
	install -d $(INSTALL_BASE)/include/rtems
	install -c -m 644 mongoose.h $(INSTALL_BASE)/rtems
	install -c -m 644 $(LIB) $(INSTALL_BASE)

clean:
	rm -f $(LIB) $(C_O_FILES) $(C_D_FILES) $(GEN_FILES)

-include $(C_D_FILES)

doc: 

