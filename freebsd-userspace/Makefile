include ../config.inc

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

CFLAGS += -Iinclude 
CFLAGS += -Irtems/include 
CFLAGS += -Ilib/libc/include 
CFLAGS += -Ilib/libc/resolv 
CFLAGS += -Ilib/netgraph
CFLAGS += -Isys
CFLAGS += -Ilocal 
# XXX hack to find rpc
CFLAGS += -I../services/librpc/include 

CFLAGS += -I$(INSTALL_BASE)/include

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
C_FILES += lib/libc/gen/err.c
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
C_FILES += lib/libc/net/name6.c
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

C_FILES += lib/libc/isc/ev_streams.c
C_FILES += lib/libc/isc/ev_timers.c

# RTEMS Specific Files
# C_FILES += rtems/rtems-net-setup.c
C_FILES += rtems/syslog.c

# ping command sources
C_FILES += commands/sbin/ping/ping.c
C_FILES += commands/sbin/ping6/ping6.c

# route command sources
C_FILES += commands/sbin/route/route.c

# dhclient command sources
C_FILES += commands/sbin/dhclient/alloc.c
C_FILES += commands/sbin/dhclient/bpf.c
C_FILES += commands/sbin/dhclient/clparse.c
C_FILES += commands/sbin/dhclient/conflex.c
C_FILES += commands/sbin/dhclient/convert.c
C_FILES += commands/sbin/dhclient/dhclient.c
C_FILES += commands/sbin/dhclient/dispatch.c
C_FILES += commands/sbin/dhclient/errwarn.c
C_FILES += commands/sbin/dhclient/hash.c
C_FILES += commands/sbin/dhclient/inet.c
C_FILES += commands/sbin/dhclient/options.c
C_FILES += commands/sbin/dhclient/packet.c
C_FILES += commands/sbin/dhclient/parse.c
C_FILES += commands/sbin/dhclient/privsep.c
C_FILES += commands/sbin/dhclient/tables.c
C_FILES += commands/sbin/dhclient/tree.c

# ifconfig command sources
C_FILES += commands/sbin/ifconfig/af_atalk.c
C_FILES += commands/sbin/ifconfig/af_inet.c
C_FILES += commands/sbin/ifconfig/af_link.c
C_FILES += commands/sbin/ifconfig/ifbridge.c
C_FILES += commands/sbin/ifconfig/ifclone.c
C_FILES += commands/sbin/ifconfig/ifgif.c
C_FILES += commands/sbin/ifconfig/ifgroup.c
C_FILES += commands/sbin/ifconfig/iflagg.c
C_FILES += commands/sbin/ifconfig/ifmedia.c
C_FILES += commands/sbin/ifconfig/ifvlan.c
C_FILES += commands/sbin/ifconfig/af_inet6.c
C_FILES += commands/sbin/ifconfig/af_nd6.c
C_FILES += commands/sbin/ifconfig/ifcarp.c
C_FILES += commands/sbin/ifconfig/ifconfig.c
C_FILES += commands/sbin/ifconfig/ifgre.c
C_FILES += commands/sbin/ifconfig/ifieee80211.c
C_FILES += commands/sbin/ifconfig/ifmac.c
C_FILES += commands/sbin/ifconfig/ifpfsync.c

# The following two files were left out to avoid
# porting issues.  regdomain uses an xml parser
# that is not part of the standard release and
# af_ipx uses thread0 which we are trying to avoid
# pulling in.
#
# C_FILES += commands/sbin/ifconfig/regdomain.c
# C_FILES += commands/sbin/ifconfig/af_ipx.c

ifeq (1,0)
# netstat command sources
# no need to support AppleTalk yet
# C_FILES += commands/usr.bin/netstat/atalk.c
C_FILES += commands/usr.bin/netstat/bpf.c
C_FILES += commands/usr.bin/netstat/if.c
C_FILES += commands/usr.bin/netstat/inet6.c
C_FILES += commands/usr.bin/netstat/inet.c
C_FILES += commands/usr.bin/netstat/ipsec.c
# no need to support IPX yet
# C_FILES += commands/usr.bin/netstat/ipx.c
C_FILES += commands/usr.bin/netstat/main.c
C_FILES += commands/usr.bin/netstat/mbuf.c
C_FILES += commands/usr.bin/netstat/mroute6.c
C_FILES += commands/usr.bin/netstat/mroute.c
# Disable netgraph support - this is a long thread to pull
# C_FILES += commands/usr.bin/netstat/netgraph.c
C_FILES += commands/usr.bin/netstat/pfkey.c
C_FILES += commands/usr.bin/netstat/route.c
C_FILES += commands/usr.bin/netstat/sctp.c
C_FILES += commands/usr.bin/netstat/unix.c
endif

C_O_FILES = $(C_FILES:%.c=%.o)
C_D_FILES = $(C_FILES:%.c=%.d)

LIB = libbsdc.a
GEN_FILES = include/rpc/rpcb_prot.h
GEN_FILES += commands/sbin/route/keywords.h
all: $(LIB)

$(LIB): $(GEN_FILES) $(C_O_FILES)
	$(AR) rcu $@ $^

include/rpc/rpcb_prot.h: include/rpc/rpcb_prot.x
	rm -f include/rpc/rpcb_prot.h
	rpcgen -h -o include/rpc/rpcb_prot.h include/rpc/rpcb_prot.x

commands/sbin/route/keywords.h: commands/sbin/route/keywords
	sed -e '/^#/d' -e '/^$$/d' commands/sbin/route//keywords > _keywords.tmp
	LC_ALL=C tr 'a-z' 'A-Z' < _keywords.tmp | paste _keywords.tmp - | \
	    awk '{ \
		if (NF > 1) \
			printf "#define\tK_%s\t%d\n\t{\"%s\", K_%s},\n", \
			    $$2, NR, $$1, $$2 }' \
	    > commands/sbin/route/keywords.h
	rm -f _keywords.tmp

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

