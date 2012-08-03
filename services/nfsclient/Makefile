include ../../config.inc

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

CFLAGS += -I $(INSTALL_BASE)/include

CFLAGS += -w 
CFLAGS += -I include 
CFLAGS += -I proto 
CFLAGS += -std=gnu99
CFLAGS += -MT $@ -MD -MP -MF $(basename $@).d

C_FILES =
C_FILES += nfs.c
C_FILES += rpcio.c
C_FILES += sock_mbuf.c
C_FILES += xdr_mbuf.c

# CEXP additions
# C_FILES += cexphelp.c dirutils.c nfs.modini.c rpcio.modini.c

# This appears to be a test program which no longer compiles and was in
# the wrong directory
C_FILES += nfsTest.c

C_O_FILES = $(C_FILES:%.c=%.o)
C_D_FILES = $(C_FILES:%.c=%.d)

LIB = libnfs.a

all: $(LIB)

$(LIB): $(C_O_FILES)
	$(AR) rcu $@ $^

install: $(LIB)
	install -d $(INSTALL_BASE)/include/rtems
	install -c -m 644 include/rtems/ftpfs.h $(INSTALL_BASE)/
	install -c -m 644 $(LIB) $(INSTALL_BASE)

clean:
	rm -f $(LIB) $(C_O_FILES) $(C_D_FILES) $(GEN_FILES)

-include $(C_D_FILES)

doc: 

