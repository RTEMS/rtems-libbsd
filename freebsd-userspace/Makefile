include ../config.inc

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

CFLAGS += -I include 
CFLAGS += -I rtems/include 
CFLAGS += -I lib/libc/include 
CFLAGS += -I sys
CFLAGS += -I local 
# XXX hack to find rpc
CFLAGS += -I ../services/librpc/include 

CFLAGS += -I $(INSTALL_BASE)/include

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
C_FILES += lib/libc/net/getaddrinfo.c
C_FILES += lib/libc/net/gethostbydns.c
C_FILES += lib/libc/net/gethostbyht.c
C_FILES += lib/libc/net/gethostbynis.c
C_FILES += lib/libc/net/gethostnamadr.c
C_FILES += lib/libc/net/getifaddrs.c
C_FILES += lib/libc/net/getifmaddrs.c
C_FILES += lib/libc/net/getnameinfo.c
C_FILES += lib/libc/net/getnetbydns.c
C_FILES += lib/libc/net/getnetbyht.c
C_FILES += lib/libc/net/getnetbynis.c
C_FILES += lib/libc/net/getnetnamadr.c
C_FILES += lib/libc/net/getproto.c
C_FILES += lib/libc/net/getprotoent.c
C_FILES += lib/libc/net/getprotoname.c
C_FILES += lib/libc/net/getservent.c
C_FILES += lib/libc/gen/gethostname.c
C_FILES += lib/libc/nameser/ns_name.c
C_FILES += lib/libc/nameser/ns_netint.c
C_FILES += lib/libc/nameser/ns_parse.c
C_FILES += lib/libc/nameser/ns_print.c
C_FILES += lib/libc/nameser/ns_ttl.c
C_FILES += lib/libc/net/if_indextoname.c
C_FILES += lib/libc/net/if_nameindex.c
C_FILES += lib/libc/net/linkaddr.c
C_FILES += lib/libc/net/map_v4v6.c
C_FILES += lib/libc/net/rcmd.c
C_FILES += lib/libc/net/recv.c
C_FILES += lib/libc/net/send.c
C_FILES += lib/libc/resolv/herror.c
C_FILES += lib/libc/resolv/res_comp.c
C_FILES += lib/libc/resolv/res_data.c
C_FILES += lib/libc/resolv/res_debug.c
C_FILES += lib/libc/resolv/res_init.c
C_FILES += lib/libc/resolv/res_mkquery.c
C_FILES += lib/libc/resolv/res_mkupdate.c
C_FILES += lib/libc/resolv/res_query.c
C_FILES += lib/libc/resolv/res_send.c
C_FILES += lib/libc/resolv/res_update.c
C_FILES += lib/libc/string/strsep.c

# RTEMS Specific Files
# C_FILES += rtems/rtems-net-setup.c
C_FILES += rtems/syslog.c

# ping command sources
C_FILES += commands/sbin/ping/ping.c
C_FILES += commands/sbin/ping6/ping6.c

C_O_FILES = $(C_FILES:%.c=%.o)
C_D_FILES = $(C_FILES:%.c=%.d)

LIB = libbsdc.a
GEN_FILES = include/rpc/rpcb_prot.h

all: $(LIB)

$(LIB): $(GEN_FILES) $(C_O_FILES)
	$(AR) rcu $@ $^

include/rpc/rpcb_prot.h: include/rpc/rpcb_prot.x
	rm -f include/rpc/rpcb_prot.h
	rpcgen -h -o include/rpc/rpcb_prot.h include/rpc/rpcb_prot.x

install: $(LIB)
	install -d $(INSTALL_BASE)/include
	cd include; for i in `find . -name '*.h'` ; do \
	  install -c -m 644 -D "$$i" "$(INSTALL_BASE)/include/$$i" ; done
	cd rtems/include; for i in `find . -name '*.h'` ; do \
	  install -c -m 644 -D "$$i" "$(INSTALL_BASE)/include/$$i" ; done
	cd local; for i in `find . -name '*.h'` ; do \
	  install -c -m 644 -D "$$i" "$(INSTALL_BASE)/include/$$i" ; done
	cd sys; for i in `find . -name '*.h'` ; do \
	  install -c -m 644 -D "$$i" "$(INSTALL_BASE)/include/$$i" ; done
	install -c -m 644 $(LIB) $(INSTALL_BASE)

clean:
	rm -f $(LIB) $(C_O_FILES) $(C_D_FILES) $(GEN_FILES)

-include $(C_D_FILES)

doc: 

