include ../config.inc

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

CFLAGS += -I $(INSTALL_BASE)/include
#CFLAGS += -ffreestanding 
CFLAGS += -I include 
CFLAGS += -I lib/libc/include 
CFLAGS += -I sys
CFLAGS += -I local 
#CFLAGS += -I . 
#CFLAGS += -I rtemsbsd 
##CFLAGS += -I rtemsbsd/$(RTEMS_CPU)/include 
#CFLAGS += -I freebsd/$(RTEMS_CPU)/include 
#CFLAGS += -I contrib/altq 
#CFLAGS += -I contrib/pf 
#CFLAGS += -B $(INSTALL_BASE) 
CFLAGS += -w 
CFLAGS += -std=gnu99
CFLAGS += -MT $@ -MD -MP -MF $(basename $@).d

C_FILES =
C_FILES += lib/libc/inet/inet_addr.c
C_FILES += lib/libc/inet/inet_cidr_ntop.c
C_FILES += lib/libc/inet/inet_cidr_pton.c
C_FILES += lib/libc/inet/inet_lnaof.c
C_FILES += lib/libc/inet/inet_makeaddr.c
C_FILES += lib/libc/inet/inet_net_pton.c
C_FILES += lib/libc/inet/inet_ntop.c
C_FILES += lib/libc/inet/inet_neta.c
C_FILES += lib/libc/inet/inet_net_ntop.c
C_FILES += lib/libc/inet/inet_netof.c
C_FILES += lib/libc/inet/inet_network.c
C_FILES += lib/libc/inet/inet_ntoa.c
C_FILES += lib/libc/inet/inet_pton.c
C_FILES += lib/libc/inet/nsap_addr.c

C_FILES += lib/libc/net/base64.c
C_FILES += lib/libc/net/ether_addr.c
# C_FILES += lib/libc/net/gethostbydns.c
# C_FILES += lib/libc/net/gethostbyht.c
C_FILES += lib/libc/net/gethostbynis.c
# C_FILES += lib/libc/net/gethostnamadr.c
C_FILES += lib/libc/net/getifaddrs.c
C_FILES += lib/libc/net/getifmaddrs.c
C_FILES += lib/libc/net/getnameinfo.c
# C_FILES += lib/libc/net/getnetbydns.c
# C_FILES += lib/libc/net/getnetbyht.c
C_FILES += lib/libc/net/getnetbynis.c
# C_FILES += lib/libc/net/getnetnamadr.c
# C_FILES += lib/libc/net/getproto.c
# C_FILES += lib/libc/net/getprotoent.c
# C_FILES += lib/libc/net/getprotoname.c
# C_FILES += lib/libc/net/getservent.c

C_O_FILES = $(C_FILES:%.c=%.o)
C_D_FILES = $(C_FILES:%.c=%.d)

LIB = libbsdc.a

all: lib_bsdc

$(LIB): $(C_O_FILES)
	$(AR) rcu $@ $^

lib_bsdc:
	$(MAKE) $(LIB)

install: $(LIB)
	#install -d $(INSTALL_BASE)/include
	#install -c -m 644 $(LIB) $(INSTALL_BASE)

clean:
	rm -f $(LIB) $(C_O_FILES) $(C_D_FILES)

-include $(C_D_FILES)

doc: 

