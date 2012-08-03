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
# RPC Files
C_FILES += src/rpc/auth_none.c
C_FILES += src/rpc/auth_unix.c
C_FILES += src/rpc/authunix_prot.c
C_FILES += src/rpc/bindresvport.c
C_FILES += src/rpc/clnt_generic.c
C_FILES += src/rpc/clnt_perror.c
C_FILES += src/rpc/clnt_raw.c
C_FILES += src/rpc/clnt_simple.c
C_FILES += src/rpc/clnt_tcp.c
C_FILES += src/rpc/clnt_udp.c
C_FILES += src/rpc/get_myaddress.c
C_FILES += src/rpc/getrpcent.c
C_FILES += src/rpc/getrpcport.c
C_FILES += src/rpc/netname.c
C_FILES += src/rpc/netnamer.c
C_FILES += src/rpc/pmap_clnt.c
C_FILES += src/rpc/pmap_getmaps.c
C_FILES += src/rpc/pmap_getport.c
C_FILES += src/rpc/pmap_prot2.c
C_FILES += src/rpc/pmap_prot.c
C_FILES += src/rpc/pmap_rmt.c
C_FILES += src/rpc/rpc_callmsg.c
C_FILES += src/rpc/rpc_commondata.c
C_FILES += src/rpc/rpcdname.c
C_FILES += src/rpc/rpc_dtablesize.c
C_FILES += src/rpc/rpc_prot.c
C_FILES += src/rpc/rtems_portmapper.c
C_FILES += src/rpc/rtems_rpc.c
C_FILES += src/rpc/rtime.c
C_FILES += src/rpc/svc_auth.c
C_FILES += src/rpc/svc_auth_unix.c
C_FILES += src/rpc/svc.c
C_FILES += src/rpc/svc_raw.c
C_FILES += src/rpc/svc_run.c
C_FILES += src/rpc/svc_simple.c
C_FILES += src/rpc/svc_tcp.c
C_FILES += src/rpc/svc_udp.c
# XDR Files
C_FILES += src/xdr/xdr_array.c
C_FILES += src/xdr/xdr.c
C_FILES += src/xdr/xdr_float.c
C_FILES += src/xdr/xdr_mem.c
C_FILES += src/xdr/xdr_rec.c
C_FILES += src/xdr/xdr_reference.c
C_FILES += src/xdr/xdr_sizeof.c
C_FILES += src/xdr/xdr_stdio.c

C_O_FILES = $(C_FILES:%.c=%.o)
C_D_FILES = $(C_FILES:%.c=%.d)

LIB = libxdr.a

all: $(LIB)

$(LIB): $(C_O_FILES)
	$(AR) rcu $@ $^

install: $(LIB)
	install -d $(INSTALL_BASE)/include/rpc
	install -d $(INSTALL_BASE)/include/xdr
	cd include; for i in `find . -name '*.h'` ; do \
	  install -c -m 644 -D "$$i" "$(INSTALL_BASE)/include/$$i" ; done
	install -c -m 644 $(LIB) $(INSTALL_BASE)

clean:
	rm -f $(LIB) $(C_O_FILES) $(C_D_FILES) $(GEN_FILES)

-include $(C_D_FILES)

doc: 

