include config.inc

RTEMS_MAKEFILE_PATH = $(PREFIX)/$(TARGET)/$(BSP)
include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

COMMON_FLAGS += -fno-strict-aliasing
COMMON_FLAGS += -ffreestanding
COMMON_FLAGS += -fno-common
COMMON_FLAGS += -Irtemsbsd/include
COMMON_FLAGS += -Irtemsbsd/$(RTEMS_CPU)/include
COMMON_FLAGS += -Ifreebsd/sys
COMMON_FLAGS += -Ifreebsd/sys/$(RTEMS_CPU)/include
COMMON_FLAGS += -Ifreebsd/sys/contrib/altq
COMMON_FLAGS += -Ifreebsd/sys/contrib/pf
COMMON_FLAGS += -Ifreebsd/include
COMMON_FLAGS += -Ifreebsd/lib/libc/include
COMMON_FLAGS += -Ifreebsd/lib/libc/isc/include
COMMON_FLAGS += -Ifreebsd/lib/libc/resolv
COMMON_FLAGS += -Ifreebsd/lib/libutil
COMMON_FLAGS += -Ifreebsd/lib/libkvm
COMMON_FLAGS += -Ifreebsd/lib/libmemstat
COMMON_FLAGS += -Ifreebsd/lib/libipsec
COMMON_FLAGS += -Irtemsbsd/sys
COMMON_FLAGS += -ImDNSResponder/mDNSCore
COMMON_FLAGS += -ImDNSResponder/mDNSShared
COMMON_FLAGS += -ImDNSResponder/mDNSPosix
COMMON_FLAGS += -Itestsuite/include
COMMON_FLAGS += -DHAVE_RTEMS_PCI_H=1
COMMON_FLAGS += -Wall
COMMON_FLAGS += -Wno-format
COMMON_FLAGS += -MT $@ -MD -MP -MF $(basename $@).d
CFLAGS += $(COMMON_FLAGS)
CFLAGS += -std=gnu11
CXXFLAGS += $(COMMON_FLAGS)
CXXFLAGS += -std=gnu++11
NEED_DUMMY_PIC_IRQ=yes

TEST_NETWORK_CONFIG = testsuite/include/rtems/bsd/test/network-config.h

TESTS =
RUN_TESTS =

NET_TESTS =
RUN_NET_TESTS =

O_FILES =
D_FILES =

LIB = libbsd.a
LIB_GEN_FILES =
LIB_C_FILES =
LIB_CXX_FILES =
LIB_CXX_FILES += rtemsbsd/rtems/rtems-bsd-cxx.cc
LIB_C_FILES += freebsd/sys/contrib/altq/altq/altq_rmclass.c
LIB_C_FILES += freebsd/sys/contrib/altq/altq/altq_rio.c
LIB_C_FILES += freebsd/sys/contrib/altq/altq/altq_subr.c
LIB_C_FILES += freebsd/sys/contrib/altq/altq/altq_cdnr.c
LIB_C_FILES += freebsd/sys/contrib/altq/altq/altq_priq.c
LIB_C_FILES += freebsd/sys/contrib/altq/altq/altq_cbq.c
LIB_C_FILES += freebsd/sys/contrib/altq/altq/altq_hfsc.c
LIB_C_FILES += freebsd/sys/contrib/altq/altq/altq_red.c
LIB_C_FILES += freebsd/sys/kern/init_main.c
LIB_C_FILES += freebsd/sys/kern/kern_condvar.c
LIB_C_FILES += freebsd/sys/kern/kern_event.c
LIB_C_FILES += freebsd/sys/kern/kern_hhook.c
LIB_C_FILES += freebsd/sys/kern/kern_intr.c
LIB_C_FILES += freebsd/sys/kern/kern_khelp.c
LIB_C_FILES += freebsd/sys/kern/kern_linker.c
LIB_C_FILES += freebsd/sys/kern/kern_mbuf.c
LIB_C_FILES += freebsd/sys/kern/kern_mib.c
LIB_C_FILES += freebsd/sys/kern/kern_module.c
LIB_C_FILES += freebsd/sys/kern/kern_mtxpool.c
LIB_C_FILES += freebsd/sys/kern/kern_osd.c
LIB_C_FILES += freebsd/sys/kern/kern_synch.c
LIB_C_FILES += freebsd/sys/kern/kern_sysctl.c
LIB_C_FILES += freebsd/sys/kern/kern_time.c
LIB_C_FILES += freebsd/sys/kern/kern_timeout.c
LIB_C_FILES += freebsd/sys/kern/subr_bufring.c
LIB_C_FILES += freebsd/sys/kern/subr_bus.c
LIB_C_FILES += freebsd/sys/kern/subr_eventhandler.c
LIB_C_FILES += freebsd/sys/kern/subr_hash.c
LIB_C_FILES += freebsd/sys/kern/subr_hints.c
LIB_C_FILES += freebsd/sys/kern/subr_kobj.c
LIB_C_FILES += freebsd/sys/kern/subr_lock.c
LIB_C_FILES += freebsd/sys/kern/subr_module.c
LIB_C_FILES += freebsd/sys/kern/subr_rman.c
LIB_C_FILES += freebsd/sys/kern/subr_sbuf.c
LIB_C_FILES += freebsd/sys/kern/subr_sleepqueue.c
LIB_C_FILES += freebsd/sys/kern/subr_taskqueue.c
LIB_C_FILES += freebsd/sys/kern/subr_uio.c
LIB_C_FILES += freebsd/sys/kern/subr_unit.c
LIB_C_FILES += freebsd/sys/kern/sys_generic.c
LIB_C_FILES += freebsd/sys/kern/uipc_accf.c
LIB_C_FILES += freebsd/sys/kern/uipc_domain.c
LIB_C_FILES += freebsd/sys/kern/uipc_mbuf2.c
LIB_C_FILES += freebsd/sys/kern/uipc_mbuf.c
LIB_C_FILES += freebsd/sys/kern/uipc_sockbuf.c
LIB_C_FILES += freebsd/sys/kern/uipc_socket.c
LIB_C_FILES += freebsd/sys/kern/uipc_usrreq.c
LIB_C_FILES += freebsd/sys/libkern/arc4random.c
LIB_C_FILES += freebsd/sys/libkern/fls.c
LIB_C_FILES += freebsd/sys/libkern/inet_ntoa.c
LIB_C_FILES += freebsd/sys/libkern/random.c
LIB_C_FILES += freebsd/sys/vm/uma_core.c
LIB_C_FILES += freebsd/sys/vm/uma_dbg.c
LIB_C_FILES += freebsd/sys/cam/cam.c
LIB_C_FILES += freebsd/sys/cam/scsi/scsi_all.c
LIB_C_FILES += freebsd/contrib/libpcap/scanner.c
freebsd/contrib/libpcap/scanner.c: freebsd/contrib/libpcap/scanner.l freebsd/contrib/libpcap/scanner.c
	${LEX} -P pcap -t $< | sed -e '/YY_BUF_SIZE/s/16384/1024/' > $@
freebsd/contrib/libpcap/scanner.o: freebsd/contrib/libpcap/scanner.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -D_U_=__attribute__((unused)) -DHAVE_INTTYPES=1 -DHAVE_STDINT=1 -DHAVE_STRERROR=1 -DHAVE_STRLCPY=1 -DHAVE_SNPRINTF=1 -DHAVE_VSNPRINTF=1 -DNEED_YYPARSE_WRAPPER=1 -Dyylval=pcap_lval -c $< -o $@
LIB_C_FILES += freebsd/contrib/libpcap/grammar.c
freebsd/contrib/libpcap/grammar.c: freebsd/contrib/libpcap/grammar.y
	yacc -b -b pcap -d -p -b pcap $<
	sed -e /YY_BUF_SIZE/s/16384/1024/ < -b pcap.tab.c > $@
	rm -f -b pcap.tab.c
	mv -b pcap.tab.h freebsd/contrib/libpcap/tokdefs.h
freebsd/contrib/libpcap/grammar.o: freebsd/contrib/libpcap/grammar.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -D_U_=__attribute__((unused)) -DHAVE_INTTYPES=1 -DHAVE_STDINT=1 -DHAVE_STRERROR=1 -DHAVE_STRLCPY=1 -DHAVE_SNPRINTF=1 -DHAVE_VSNPRINTF=1 -DNEED_YYPARSE_WRAPPER=1 -Dyylval=pcap_lval -c $< -o $@
LIB_C_FILES += freebsd/contrib/libpcap/bpf_image.c
freebsd/contrib/libpcap/bpf_image.o: freebsd/contrib/libpcap/bpf_image.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -D_U_=__attribute__((unused)) -DHAVE_INTTYPES=1 -DHAVE_STDINT=1 -DHAVE_STRERROR=1 -DHAVE_STRLCPY=1 -DHAVE_SNPRINTF=1 -DHAVE_VSNPRINTF=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/libpcap/etherent.c
freebsd/contrib/libpcap/etherent.o: freebsd/contrib/libpcap/etherent.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -D_U_=__attribute__((unused)) -DHAVE_INTTYPES=1 -DHAVE_STDINT=1 -DHAVE_STRERROR=1 -DHAVE_STRLCPY=1 -DHAVE_SNPRINTF=1 -DHAVE_VSNPRINTF=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/libpcap/fad-getad.c
freebsd/contrib/libpcap/fad-getad.o: freebsd/contrib/libpcap/fad-getad.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -D_U_=__attribute__((unused)) -DHAVE_INTTYPES=1 -DHAVE_STDINT=1 -DHAVE_STRERROR=1 -DHAVE_STRLCPY=1 -DHAVE_SNPRINTF=1 -DHAVE_VSNPRINTF=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/libpcap/gencode.c
freebsd/contrib/libpcap/gencode.o: freebsd/contrib/libpcap/gencode.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -D_U_=__attribute__((unused)) -DHAVE_INTTYPES=1 -DHAVE_STDINT=1 -DHAVE_STRERROR=1 -DHAVE_STRLCPY=1 -DHAVE_SNPRINTF=1 -DHAVE_VSNPRINTF=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/libpcap/inet.c
freebsd/contrib/libpcap/inet.o: freebsd/contrib/libpcap/inet.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -D_U_=__attribute__((unused)) -DHAVE_INTTYPES=1 -DHAVE_STDINT=1 -DHAVE_STRERROR=1 -DHAVE_STRLCPY=1 -DHAVE_SNPRINTF=1 -DHAVE_VSNPRINTF=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/libpcap/pcap.c
freebsd/contrib/libpcap/pcap.o: freebsd/contrib/libpcap/pcap.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -D_U_=__attribute__((unused)) -DHAVE_INTTYPES=1 -DHAVE_STDINT=1 -DHAVE_STRERROR=1 -DHAVE_STRLCPY=1 -DHAVE_SNPRINTF=1 -DHAVE_VSNPRINTF=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/libpcap/pcap-bpf.c
freebsd/contrib/libpcap/pcap-bpf.o: freebsd/contrib/libpcap/pcap-bpf.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -D_U_=__attribute__((unused)) -DHAVE_INTTYPES=1 -DHAVE_STDINT=1 -DHAVE_STRERROR=1 -DHAVE_STRLCPY=1 -DHAVE_SNPRINTF=1 -DHAVE_VSNPRINTF=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/libpcap/pcap-common.c
freebsd/contrib/libpcap/pcap-common.o: freebsd/contrib/libpcap/pcap-common.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -D_U_=__attribute__((unused)) -DHAVE_INTTYPES=1 -DHAVE_STDINT=1 -DHAVE_STRERROR=1 -DHAVE_STRLCPY=1 -DHAVE_SNPRINTF=1 -DHAVE_VSNPRINTF=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/libpcap/optimize.c
freebsd/contrib/libpcap/optimize.o: freebsd/contrib/libpcap/optimize.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -D_U_=__attribute__((unused)) -DHAVE_INTTYPES=1 -DHAVE_STDINT=1 -DHAVE_STRERROR=1 -DHAVE_STRLCPY=1 -DHAVE_SNPRINTF=1 -DHAVE_VSNPRINTF=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/libpcap/nametoaddr.c
freebsd/contrib/libpcap/nametoaddr.o: freebsd/contrib/libpcap/nametoaddr.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -D_U_=__attribute__((unused)) -DHAVE_INTTYPES=1 -DHAVE_STDINT=1 -DHAVE_STRERROR=1 -DHAVE_STRLCPY=1 -DHAVE_SNPRINTF=1 -DHAVE_VSNPRINTF=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/libpcap/savefile.c
freebsd/contrib/libpcap/savefile.o: freebsd/contrib/libpcap/savefile.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -D_U_=__attribute__((unused)) -DHAVE_INTTYPES=1 -DHAVE_STDINT=1 -DHAVE_STRERROR=1 -DHAVE_STRLCPY=1 -DHAVE_SNPRINTF=1 -DHAVE_VSNPRINTF=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/libpcap/sf-pcap.c
freebsd/contrib/libpcap/sf-pcap.o: freebsd/contrib/libpcap/sf-pcap.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -D_U_=__attribute__((unused)) -DHAVE_INTTYPES=1 -DHAVE_STDINT=1 -DHAVE_STRERROR=1 -DHAVE_STRLCPY=1 -DHAVE_SNPRINTF=1 -DHAVE_VSNPRINTF=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/libpcap/sf-pcap-ng.c
freebsd/contrib/libpcap/sf-pcap-ng.o: freebsd/contrib/libpcap/sf-pcap-ng.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -D_U_=__attribute__((unused)) -DHAVE_INTTYPES=1 -DHAVE_STDINT=1 -DHAVE_STRERROR=1 -DHAVE_STRLCPY=1 -DHAVE_SNPRINTF=1 -DHAVE_VSNPRINTF=1 -c $< -o $@
LIB_C_FILES += freebsd/sys/crypto/sha1.c
LIB_C_FILES += freebsd/sys/crypto/sha2/sha2.c
LIB_C_FILES += freebsd/sys/crypto/rijndael/rijndael-alg-fst.c
LIB_C_FILES += freebsd/sys/crypto/rijndael/rijndael-api.c
LIB_C_FILES += freebsd/sys/crypto/rijndael/rijndael-api-fst.c
LIB_C_FILES += freebsd/sys/crypto/des/des_setkey.c
LIB_C_FILES += freebsd/sys/crypto/des/des_enc.c
LIB_C_FILES += freebsd/sys/crypto/des/des_ecb.c
LIB_C_FILES += freebsd/sys/crypto/blowfish/bf_enc.c
LIB_C_FILES += freebsd/sys/crypto/blowfish/bf_skey.c
LIB_C_FILES += freebsd/sys/crypto/blowfish/bf_ecb.c
LIB_C_FILES += freebsd/sys/crypto/rc4/rc4.c
LIB_C_FILES += freebsd/sys/crypto/camellia/camellia-api.c
LIB_C_FILES += freebsd/sys/crypto/camellia/camellia.c
LIB_C_FILES += freebsd/sys/dev/mii/mii.c
LIB_C_FILES += freebsd/sys/dev/mii/mii_bitbang.c
LIB_C_FILES += freebsd/sys/dev/mii/mii_physubr.c
LIB_C_FILES += freebsd/sys/dev/mii/icsphy.c
LIB_C_FILES += freebsd/sys/dev/mii/e1000phy.c
LIB_C_FILES += freebsd/sys/dev/mii/brgphy.c
LIB_C_FILES += freebsd/sys/dev/mii/micphy.c
LIB_C_FILES += freebsd/sys/dev/mii/ukphy.c
LIB_C_FILES += freebsd/sys/dev/mii/ukphy_subr.c
LIB_C_FILES += freebsd/sys/dev/tsec/if_tsec.c
LIB_C_FILES += freebsd/sys/dev/cadence/if_cgem.c
LIB_C_FILES += freebsd/sys/dev/dwc/if_dwc.c
LIB_C_FILES += freebsd/sys/arm/xilinx/zy7_slcr.c
LIB_C_FILES += freebsd/sys/dev/random/harvest.c
LIB_C_FILES += freebsd/sys/netinet/tcp_hostcache.c
LIB_C_FILES += freebsd/sys/dev/led/led.c
LIB_C_FILES += freebsd/sys/netatalk/aarp.c
LIB_C_FILES += freebsd/sys/netatalk/at_control.c
LIB_C_FILES += freebsd/sys/netatalk/at_rmx.c
LIB_C_FILES += freebsd/sys/netatalk/ddp_input.c
LIB_C_FILES += freebsd/sys/netatalk/ddp_pcb.c
LIB_C_FILES += freebsd/sys/netatalk/ddp_usrreq.c
LIB_C_FILES += freebsd/sys/netatalk/at_proto.c
LIB_C_FILES += freebsd/sys/netatalk/ddp_output.c
LIB_C_FILES += freebsd/sys/dev/bce/if_bce.c
LIB_C_FILES += freebsd/sys/dev/bfe/if_bfe.c
LIB_C_FILES += freebsd/sys/dev/bge/if_bge.c
LIB_C_FILES += freebsd/sys/dev/dc/dcphy.c
LIB_C_FILES += freebsd/sys/dev/dc/if_dc.c
LIB_C_FILES += freebsd/sys/dev/dc/pnphy.c
LIB_C_FILES += freebsd/sys/dev/e1000/e1000_80003es2lan.c
LIB_C_FILES += freebsd/sys/dev/e1000/e1000_82542.c
LIB_C_FILES += freebsd/sys/dev/e1000/e1000_82575.c
LIB_C_FILES += freebsd/sys/dev/e1000/e1000_mac.c
LIB_C_FILES += freebsd/sys/dev/e1000/e1000_nvm.c
LIB_C_FILES += freebsd/sys/dev/e1000/e1000_vf.c
LIB_C_FILES += freebsd/sys/dev/e1000/if_lem.c
LIB_C_FILES += freebsd/sys/dev/e1000/e1000_82540.c
LIB_C_FILES += freebsd/sys/dev/e1000/e1000_82543.c
LIB_C_FILES += freebsd/sys/dev/e1000/e1000_api.c
LIB_C_FILES += freebsd/sys/dev/e1000/e1000_manage.c
LIB_C_FILES += freebsd/sys/dev/e1000/e1000_osdep.c
LIB_C_FILES += freebsd/sys/dev/e1000/if_em.c
LIB_C_FILES += freebsd/sys/dev/e1000/e1000_82541.c
LIB_C_FILES += freebsd/sys/dev/e1000/e1000_82571.c
LIB_C_FILES += freebsd/sys/dev/e1000/e1000_ich8lan.c
LIB_C_FILES += freebsd/sys/dev/e1000/e1000_mbx.c
LIB_C_FILES += freebsd/sys/dev/e1000/e1000_phy.c
LIB_C_FILES += freebsd/sys/dev/e1000/if_igb.c
LIB_C_FILES += freebsd/sys/dev/fxp/if_fxp.c
LIB_C_FILES += freebsd/sys/dev/re/if_re.c
LIB_C_FILES += freebsd/sys/dev/smc/if_smc.c
LIB_C_FILES += freebsd/sys/dev/usb/usb_busdma.c
LIB_C_FILES += freebsd/sys/dev/usb/usb_core.c
LIB_C_FILES += freebsd/sys/dev/usb/usb_debug.c
LIB_C_FILES += freebsd/sys/dev/usb/usb_dev.c
LIB_C_FILES += freebsd/sys/dev/usb/usb_device.c
LIB_C_FILES += freebsd/sys/dev/usb/usb_dynamic.c
LIB_C_FILES += freebsd/sys/dev/usb/usb_error.c
LIB_C_FILES += freebsd/sys/dev/usb/usb_generic.c
LIB_C_FILES += freebsd/sys/dev/usb/usb_handle_request.c
LIB_C_FILES += freebsd/sys/dev/usb/usb_hid.c
LIB_C_FILES += freebsd/sys/dev/usb/usb_hub.c
LIB_C_FILES += freebsd/sys/dev/usb/usb_lookup.c
LIB_C_FILES += freebsd/sys/dev/usb/usb_mbuf.c
LIB_C_FILES += freebsd/sys/dev/usb/usb_msctest.c
LIB_C_FILES += freebsd/sys/dev/usb/usb_parse.c
LIB_C_FILES += freebsd/sys/dev/usb/usb_process.c
LIB_C_FILES += freebsd/sys/dev/usb/usb_request.c
LIB_C_FILES += freebsd/sys/dev/usb/usb_transfer.c
LIB_C_FILES += freebsd/sys/dev/usb/usb_util.c
LIB_C_FILES += freebsd/sys/dev/usb/controller/ohci.c
LIB_C_FILES += freebsd/sys/dev/usb/controller/ehci.c
LIB_C_FILES += freebsd/sys/dev/usb/controller/usb_controller.c
LIB_C_FILES += freebsd/sys/dev/usb/quirk/usb_quirk.c
LIB_C_FILES += freebsd/sys/dev/usb/storage/umass.c
LIB_C_FILES += dhcpcd/arp.c
dhcpcd/arp.o: dhcpcd/arp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6 -c $< -o $@
LIB_C_FILES += dhcpcd/auth.c
dhcpcd/auth.o: dhcpcd/auth.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6 -c $< -o $@
LIB_C_FILES += dhcpcd/bpf.c
dhcpcd/bpf.o: dhcpcd/bpf.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6 -c $< -o $@
LIB_C_FILES += dhcpcd/common.c
dhcpcd/common.o: dhcpcd/common.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6 -c $< -o $@
LIB_C_FILES += dhcpcd/dhcp6.c
dhcpcd/dhcp6.o: dhcpcd/dhcp6.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6 -c $< -o $@
LIB_C_FILES += dhcpcd/dhcp.c
dhcpcd/dhcp.o: dhcpcd/dhcp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6 -c $< -o $@
LIB_C_FILES += dhcpcd/dhcpcd.c
dhcpcd/dhcpcd.o: dhcpcd/dhcpcd.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6 -c $< -o $@
LIB_C_FILES += dhcpcd/dhcpcd-embedded.c
dhcpcd/dhcpcd-embedded.o: dhcpcd/dhcpcd-embedded.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6 -c $< -o $@
LIB_C_FILES += dhcpcd/dhcp-common.c
dhcpcd/dhcp-common.o: dhcpcd/dhcp-common.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6 -c $< -o $@
LIB_C_FILES += dhcpcd/duid.c
dhcpcd/duid.o: dhcpcd/duid.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6 -c $< -o $@
LIB_C_FILES += dhcpcd/eloop.c
dhcpcd/eloop.o: dhcpcd/eloop.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6 -c $< -o $@
LIB_C_FILES += dhcpcd/if-bsd.c
dhcpcd/if-bsd.o: dhcpcd/if-bsd.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6 -c $< -o $@
LIB_C_FILES += dhcpcd/if-options.c
dhcpcd/if-options.o: dhcpcd/if-options.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6 -c $< -o $@
LIB_C_FILES += dhcpcd/if-pref.c
dhcpcd/if-pref.o: dhcpcd/if-pref.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6 -c $< -o $@
LIB_C_FILES += dhcpcd/ipv4.c
dhcpcd/ipv4.o: dhcpcd/ipv4.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6 -c $< -o $@
LIB_C_FILES += dhcpcd/ipv4ll.c
dhcpcd/ipv4ll.o: dhcpcd/ipv4ll.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6 -c $< -o $@
LIB_C_FILES += dhcpcd/ipv6.c
dhcpcd/ipv6.o: dhcpcd/ipv6.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6 -c $< -o $@
LIB_C_FILES += dhcpcd/ipv6nd.c
dhcpcd/ipv6nd.o: dhcpcd/ipv6nd.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6 -c $< -o $@
LIB_C_FILES += dhcpcd/net.c
dhcpcd/net.o: dhcpcd/net.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6 -c $< -o $@
LIB_C_FILES += dhcpcd/platform-bsd.c
dhcpcd/platform-bsd.o: dhcpcd/platform-bsd.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6 -c $< -o $@
LIB_C_FILES += dhcpcd/compat/pselect.c
dhcpcd/compat/pselect.o: dhcpcd/compat/pselect.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6 -c $< -o $@
LIB_C_FILES += dhcpcd/crypt/hmac_md5.c
dhcpcd/crypt/hmac_md5.o: dhcpcd/crypt/hmac_md5.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6 -c $< -o $@
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-shell-dhcpcd.c
ifeq ($(RTEMS_CPU), arm)
LIB_C_FILES += freebsd/sys/arm/arm/in_cksum.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), avr)
LIB_C_FILES += freebsd/sys/avr/avr/in_cksum.c
endif
ifeq ($(RTEMS_CPU), bfin)
LIB_C_FILES += freebsd/sys/bfin/bfin/in_cksum.c
endif
ifeq ($(RTEMS_CPU), h8300)
LIB_C_FILES += freebsd/sys/h8300/h8300/in_cksum.c
endif
ifeq ($(RTEMS_CPU), i386)
LIB_C_FILES += freebsd/sys/i386/i386/in_cksum.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), lm32)
LIB_C_FILES += freebsd/sys/lm32/lm32/in_cksum.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), m32c)
LIB_C_FILES += freebsd/sys/m32c/m32c/in_cksum.c
endif
ifeq ($(RTEMS_CPU), m32r)
LIB_C_FILES += freebsd/sys/m32r/m32r/in_cksum.c
endif
ifeq ($(RTEMS_CPU), m68k)
LIB_C_FILES += freebsd/sys/m68k/m68k/in_cksum.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), mips)
LIB_C_FILES += freebsd/sys/mips/mips/in_cksum.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), nios2)
LIB_C_FILES += freebsd/sys/nios2/nios2/in_cksum.c
endif
ifeq ($(RTEMS_CPU), powerpc)
LIB_C_FILES += freebsd/sys/powerpc/powerpc/in_cksum.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), sh)
LIB_C_FILES += freebsd/sys/sh/sh/in_cksum.c
endif
ifeq ($(RTEMS_CPU), sparc)
LIB_C_FILES += freebsd/sys/sparc/sparc/in_cksum.c
LIB_C_FILES += freebsd/sys/mips/mips/in_cksum.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), sparc64)
LIB_C_FILES += freebsd/sys/sparc64/sparc64/in_cksum.c
endif
ifeq ($(RTEMS_CPU), v850)
LIB_C_FILES += freebsd/sys/v850/v850/in_cksum.c
endif
LIB_C_FILES += mDNSResponder/mDNSCore/anonymous.c
LIB_C_FILES += mDNSResponder/mDNSCore/CryptoAlg.c
LIB_C_FILES += mDNSResponder/mDNSCore/DNSCommon.c
LIB_C_FILES += mDNSResponder/mDNSCore/DNSDigest.c
LIB_C_FILES += mDNSResponder/mDNSCore/mDNS.c
LIB_C_FILES += mDNSResponder/mDNSCore/uDNS.c
LIB_C_FILES += mDNSResponder/mDNSShared/dnssd_clientshim.c
LIB_C_FILES += mDNSResponder/mDNSShared/mDNSDebug.c
LIB_C_FILES += mDNSResponder/mDNSShared/PlatformCommon.c
LIB_C_FILES += mDNSResponder/mDNSShared/GenLinkedList.c
LIB_C_FILES += mDNSResponder/mDNSPosix/mDNSPosix.c
LIB_C_FILES += mDNSResponder/mDNSPosix/mDNSUNP.c
LIB_C_FILES += rtemsbsd/mghttpd/mongoose.c
rtemsbsd/mghttpd/mongoose.o: rtemsbsd/mghttpd/mongoose.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DNO_SSL -DNO_POPEN -DNO_CGI -DUSE_WEBSOCKET -c $< -o $@
LIB_C_FILES += freebsd/sys/dev/mmc/mmc.c
LIB_C_FILES += freebsd/sys/dev/mmc/mmcsd.c
LIB_C_FILES += freebsd/sys/dev/sdhci/sdhci.c
LIB_C_FILES += freebsd/sys/kern/sys_socket.c
LIB_C_FILES += freebsd/sys/kern/uipc_syscalls.c
LIB_C_FILES += freebsd/sys/net/bridgestp.c
LIB_C_FILES += freebsd/sys/net/ieee8023ad_lacp.c
LIB_C_FILES += freebsd/sys/net/if_atmsubr.c
LIB_C_FILES += freebsd/sys/net/if.c
LIB_C_FILES += freebsd/sys/net/if_clone.c
LIB_C_FILES += freebsd/sys/net/if_dead.c
LIB_C_FILES += freebsd/sys/net/if_disc.c
LIB_C_FILES += freebsd/sys/net/if_edsc.c
LIB_C_FILES += freebsd/sys/net/if_ef.c
LIB_C_FILES += freebsd/sys/net/if_enc.c
LIB_C_FILES += freebsd/sys/net/if_epair.c
LIB_C_FILES += freebsd/sys/net/if_faith.c
LIB_C_FILES += freebsd/sys/net/if_fddisubr.c
LIB_C_FILES += freebsd/sys/net/if_fwsubr.c
LIB_C_FILES += freebsd/sys/net/if_gif.c
LIB_C_FILES += freebsd/sys/net/if_gre.c
LIB_C_FILES += freebsd/sys/net/if_iso88025subr.c
LIB_C_FILES += freebsd/sys/net/if_lagg.c
LIB_C_FILES += freebsd/sys/net/if_llatbl.c
LIB_C_FILES += freebsd/sys/net/if_loop.c
LIB_C_FILES += freebsd/sys/net/if_media.c
LIB_C_FILES += freebsd/sys/net/if_mib.c
LIB_C_FILES += freebsd/sys/net/if_spppfr.c
LIB_C_FILES += freebsd/sys/net/if_spppsubr.c
LIB_C_FILES += freebsd/sys/net/if_tap.c
LIB_C_FILES += freebsd/sys/net/if_tun.c
LIB_C_FILES += freebsd/sys/net/if_vlan.c
LIB_C_FILES += freebsd/sys/net/pfil.c
LIB_C_FILES += freebsd/sys/net/radix.c
LIB_C_FILES += freebsd/sys/net/radix_mpath.c
LIB_C_FILES += freebsd/sys/net/raw_cb.c
LIB_C_FILES += freebsd/sys/net/raw_usrreq.c
LIB_C_FILES += freebsd/sys/net/route.c
LIB_C_FILES += freebsd/sys/net/rtsock.c
LIB_C_FILES += freebsd/sys/net/slcompress.c
LIB_C_FILES += freebsd/sys/net/bpf_buffer.c
LIB_C_FILES += freebsd/sys/net/bpf.c
LIB_C_FILES += freebsd/sys/net/bpf_filter.c
LIB_C_FILES += freebsd/sys/net/bpf_jitter.c
LIB_C_FILES += freebsd/sys/net/if_arcsubr.c
LIB_C_FILES += freebsd/sys/net/if_bridge.c
LIB_C_FILES += freebsd/sys/net/if_ethersubr.c
LIB_C_FILES += freebsd/sys/net/netisr.c
LIB_C_FILES += freebsd/sys/netinet/accf_data.c
LIB_C_FILES += freebsd/sys/netinet/accf_dns.c
LIB_C_FILES += freebsd/sys/netinet/accf_http.c
LIB_C_FILES += freebsd/sys/netinet/cc/cc.c
LIB_C_FILES += freebsd/sys/netinet/cc/cc_newreno.c
LIB_C_FILES += freebsd/sys/netinet/if_atm.c
LIB_C_FILES += freebsd/sys/netinet/if_ether.c
LIB_C_FILES += freebsd/sys/netinet/igmp.c
LIB_C_FILES += freebsd/sys/netinet/in.c
LIB_C_FILES += freebsd/sys/netinet/in_gif.c
LIB_C_FILES += freebsd/sys/netinet/in_mcast.c
LIB_C_FILES += freebsd/sys/netinet/in_pcb.c
LIB_C_FILES += freebsd/sys/netinet/in_proto.c
LIB_C_FILES += freebsd/sys/netinet/in_rmx.c
LIB_C_FILES += freebsd/sys/netinet/ip_carp.c
LIB_C_FILES += freebsd/sys/netinet/ip_divert.c
LIB_C_FILES += freebsd/sys/netinet/ip_ecn.c
LIB_C_FILES += freebsd/sys/netinet/ip_encap.c
LIB_C_FILES += freebsd/sys/netinet/ip_fastfwd.c
LIB_C_FILES += freebsd/sys/netinet/ip_gre.c
LIB_C_FILES += freebsd/sys/netinet/ip_icmp.c
LIB_C_FILES += freebsd/sys/netinet/ip_id.c
LIB_C_FILES += freebsd/sys/netinet/ip_input.c
LIB_C_FILES += freebsd/sys/netinet/ip_mroute.c
LIB_C_FILES += freebsd/sys/netinet/ip_options.c
LIB_C_FILES += freebsd/sys/netinet/ip_output.c
LIB_C_FILES += freebsd/sys/netinet/raw_ip.c
LIB_C_FILES += freebsd/sys/netinet/sctp_asconf.c
LIB_C_FILES += freebsd/sys/netinet/sctp_auth.c
LIB_C_FILES += freebsd/sys/netinet/sctp_bsd_addr.c
LIB_C_FILES += freebsd/sys/netinet/sctp_cc_functions.c
LIB_C_FILES += freebsd/sys/netinet/sctp_crc32.c
LIB_C_FILES += freebsd/sys/netinet/sctp_indata.c
LIB_C_FILES += freebsd/sys/netinet/sctp_input.c
LIB_C_FILES += freebsd/sys/netinet/sctp_output.c
LIB_C_FILES += freebsd/sys/netinet/sctp_pcb.c
LIB_C_FILES += freebsd/sys/netinet/sctp_peeloff.c
LIB_C_FILES += freebsd/sys/netinet/sctp_sysctl.c
LIB_C_FILES += freebsd/sys/netinet/sctp_timer.c
LIB_C_FILES += freebsd/sys/netinet/sctp_usrreq.c
LIB_C_FILES += freebsd/sys/netinet/sctputil.c
LIB_C_FILES += freebsd/sys/netinet/tcp_debug.c
LIB_C_FILES += freebsd/sys/netinet/tcp_input.c
LIB_C_FILES += freebsd/sys/netinet/tcp_lro.c
LIB_C_FILES += freebsd/sys/netinet/tcp_offload.c
LIB_C_FILES += freebsd/sys/netinet/tcp_output.c
LIB_C_FILES += freebsd/sys/netinet/tcp_reass.c
LIB_C_FILES += freebsd/sys/netinet/tcp_sack.c
LIB_C_FILES += freebsd/sys/netinet/tcp_subr.c
LIB_C_FILES += freebsd/sys/netinet/tcp_syncache.c
LIB_C_FILES += freebsd/sys/netinet/tcp_timer.c
LIB_C_FILES += freebsd/sys/netinet/tcp_timewait.c
LIB_C_FILES += freebsd/sys/netinet/tcp_usrreq.c
LIB_C_FILES += freebsd/sys/netpfil/ipfw/dn_heap.c
LIB_C_FILES += freebsd/sys/netpfil/ipfw/dn_sched_fifo.c
LIB_C_FILES += freebsd/sys/netpfil/ipfw/dn_sched_prio.c
LIB_C_FILES += freebsd/sys/netpfil/ipfw/dn_sched_qfq.c
LIB_C_FILES += freebsd/sys/netpfil/ipfw/dn_sched_rr.c
LIB_C_FILES += freebsd/sys/netpfil/ipfw/dn_sched_wf2q.c
LIB_C_FILES += freebsd/sys/netpfil/ipfw/ip_dn_glue.c
LIB_C_FILES += freebsd/sys/netpfil/ipfw/ip_dn_io.c
LIB_C_FILES += freebsd/sys/netpfil/ipfw/ip_dummynet.c
LIB_C_FILES += freebsd/sys/netpfil/ipfw/ip_fw2.c
LIB_C_FILES += freebsd/sys/netpfil/ipfw/ip_fw_log.c
LIB_C_FILES += freebsd/sys/netpfil/ipfw/ip_fw_nat.c
LIB_C_FILES += freebsd/sys/netpfil/ipfw/ip_fw_pfil.c
LIB_C_FILES += freebsd/sys/netpfil/ipfw/ip_fw_sockopt.c
LIB_C_FILES += freebsd/sys/netpfil/ipfw/ip_fw_table.c
LIB_C_FILES += freebsd/sys/netinet/udp_usrreq.c
LIB_C_FILES += freebsd/sys/netinet/libalias/alias_dummy.c
LIB_C_FILES += freebsd/sys/netinet/libalias/alias_pptp.c
LIB_C_FILES += freebsd/sys/netinet/libalias/alias_smedia.c
LIB_C_FILES += freebsd/sys/netinet/libalias/alias_mod.c
LIB_C_FILES += freebsd/sys/netinet/libalias/alias_cuseeme.c
LIB_C_FILES += freebsd/sys/netinet/libalias/alias_nbt.c
LIB_C_FILES += freebsd/sys/netinet/libalias/alias_irc.c
LIB_C_FILES += freebsd/sys/netinet/libalias/alias_util.c
LIB_C_FILES += freebsd/sys/netinet/libalias/alias_db.c
LIB_C_FILES += freebsd/sys/netinet/libalias/alias_ftp.c
LIB_C_FILES += freebsd/sys/netinet/libalias/alias_proxy.c
LIB_C_FILES += freebsd/sys/netinet/libalias/alias.c
LIB_C_FILES += freebsd/sys/netinet/libalias/alias_skinny.c
LIB_C_FILES += freebsd/sys/netinet/libalias/alias_sctp.c
LIB_C_FILES += freebsd/sys/net/if_stf.c
LIB_C_FILES += freebsd/sys/netinet6/dest6.c
LIB_C_FILES += freebsd/sys/netinet6/frag6.c
LIB_C_FILES += freebsd/sys/netinet6/icmp6.c
LIB_C_FILES += freebsd/sys/netinet6/in6.c
LIB_C_FILES += freebsd/sys/netinet6/in6_cksum.c
LIB_C_FILES += freebsd/sys/netinet6/in6_gif.c
LIB_C_FILES += freebsd/sys/netinet6/in6_ifattach.c
LIB_C_FILES += freebsd/sys/netinet6/in6_mcast.c
LIB_C_FILES += freebsd/sys/netinet6/in6_pcb.c
LIB_C_FILES += freebsd/sys/netinet6/in6_proto.c
LIB_C_FILES += freebsd/sys/netinet6/in6_rmx.c
LIB_C_FILES += freebsd/sys/netinet6/in6_src.c
LIB_C_FILES += freebsd/sys/netinet6/ip6_forward.c
LIB_C_FILES += freebsd/sys/netinet6/ip6_id.c
LIB_C_FILES += freebsd/sys/netinet6/ip6_input.c
LIB_C_FILES += freebsd/sys/netinet6/ip6_mroute.c
LIB_C_FILES += freebsd/sys/netinet6/ip6_output.c
LIB_C_FILES += freebsd/sys/netinet6/mld6.c
LIB_C_FILES += freebsd/sys/netinet6/nd6.c
LIB_C_FILES += freebsd/sys/netinet6/nd6_nbr.c
LIB_C_FILES += freebsd/sys/netinet6/nd6_rtr.c
LIB_C_FILES += freebsd/sys/netinet6/raw_ip6.c
LIB_C_FILES += freebsd/sys/netinet6/route6.c
LIB_C_FILES += freebsd/sys/netinet6/scope6.c
LIB_C_FILES += freebsd/sys/netinet6/sctp6_usrreq.c
LIB_C_FILES += freebsd/sys/netinet6/udp6_usrreq.c
LIB_C_FILES += freebsd/sys/opencrypto/crypto.c
LIB_C_FILES += freebsd/sys/opencrypto/deflate.c
LIB_C_FILES += freebsd/sys/opencrypto/cryptosoft.c
LIB_C_FILES += freebsd/sys/opencrypto/criov.c
LIB_C_FILES += freebsd/sys/opencrypto/rmd160.c
LIB_C_FILES += freebsd/sys/opencrypto/xform.c
LIB_C_FILES += freebsd/sys/opencrypto/skipjack.c
LIB_C_FILES += freebsd/sys/opencrypto/cast.c
LIB_C_FILES += freebsd/sys/dev/pci/pci.c
LIB_C_FILES += freebsd/sys/dev/pci/pci_user.c
LIB_C_FILES += freebsd/sys/dev/pci/pci_pci.c
ifeq ($(RTEMS_CPU), arm)
LIB_C_FILES += freebsd/sys/arm/arm/legacy.c
LIB_C_FILES += freebsd/sys/arm/pci/pci_bus.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), avr)
LIB_C_FILES += freebsd/sys/avr/avr/legacy.c
LIB_C_FILES += freebsd/sys/avr/pci/pci_bus.c
endif
ifeq ($(RTEMS_CPU), bfin)
LIB_C_FILES += freebsd/sys/bfin/bfin/legacy.c
LIB_C_FILES += freebsd/sys/bfin/pci/pci_bus.c
endif
ifeq ($(RTEMS_CPU), h8300)
LIB_C_FILES += freebsd/sys/h8300/h8300/legacy.c
LIB_C_FILES += freebsd/sys/h8300/pci/pci_bus.c
endif
ifeq ($(RTEMS_CPU), i386)
LIB_C_FILES += freebsd/sys/i386/i386/legacy.c
LIB_C_FILES += freebsd/sys/i386/pci/pci_bus.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), lm32)
LIB_C_FILES += freebsd/sys/lm32/lm32/legacy.c
LIB_C_FILES += freebsd/sys/lm32/pci/pci_bus.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), m32c)
LIB_C_FILES += freebsd/sys/m32c/m32c/legacy.c
LIB_C_FILES += freebsd/sys/m32c/pci/pci_bus.c
endif
ifeq ($(RTEMS_CPU), m32r)
LIB_C_FILES += freebsd/sys/m32r/m32r/legacy.c
LIB_C_FILES += freebsd/sys/m32r/pci/pci_bus.c
endif
ifeq ($(RTEMS_CPU), m68k)
LIB_C_FILES += freebsd/sys/m68k/m68k/legacy.c
LIB_C_FILES += freebsd/sys/m68k/pci/pci_bus.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), mips)
LIB_C_FILES += freebsd/sys/mips/mips/legacy.c
LIB_C_FILES += freebsd/sys/mips/pci/pci_bus.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), nios2)
LIB_C_FILES += freebsd/sys/nios2/nios2/legacy.c
LIB_C_FILES += freebsd/sys/nios2/pci/pci_bus.c
endif
ifeq ($(RTEMS_CPU), powerpc)
LIB_C_FILES += freebsd/sys/powerpc/powerpc/legacy.c
LIB_C_FILES += freebsd/sys/powerpc/pci/pci_bus.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), sh)
LIB_C_FILES += freebsd/sys/sh/sh/legacy.c
LIB_C_FILES += freebsd/sys/sh/pci/pci_bus.c
endif
ifeq ($(RTEMS_CPU), sparc)
LIB_C_FILES += freebsd/sys/sparc/sparc/legacy.c
LIB_C_FILES += freebsd/sys/sparc/pci/pci_bus.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), sparc64)
LIB_C_FILES += freebsd/sys/sparc64/sparc64/legacy.c
LIB_C_FILES += freebsd/sys/sparc64/pci/pci_bus.c
endif
ifeq ($(RTEMS_CPU), v850)
LIB_C_FILES += freebsd/sys/v850/v850/legacy.c
LIB_C_FILES += freebsd/sys/v850/pci/pci_bus.c
endif
LIB_C_FILES += freebsd/sys/contrib/pf/net/if_pflog.c
LIB_C_FILES += freebsd/sys/contrib/pf/net/if_pfsync.c
LIB_C_FILES += freebsd/sys/contrib/pf/net/pf.c
LIB_C_FILES += freebsd/sys/contrib/pf/net/pf_if.c
LIB_C_FILES += freebsd/sys/contrib/pf/net/pf_ioctl.c
LIB_C_FILES += freebsd/sys/contrib/pf/net/pf_lb.c
LIB_C_FILES += freebsd/sys/contrib/pf/net/pf_norm.c
LIB_C_FILES += freebsd/sys/contrib/pf/net/pf_osfp.c
LIB_C_FILES += freebsd/sys/contrib/pf/net/pf_ruleset.c
LIB_C_FILES += freebsd/sys/contrib/pf/net/pf_table.c
LIB_C_FILES += freebsd/sys/contrib/pf/netinet/in4_cksum.c
LIB_C_FILES += rtemsbsd/local/bus_if.c
LIB_C_FILES += rtemsbsd/local/cryptodev_if.c
LIB_C_FILES += rtemsbsd/local/device_if.c
LIB_C_FILES += rtemsbsd/local/miibus_if.c
LIB_C_FILES += rtemsbsd/local/pcib_if.c
LIB_C_FILES += rtemsbsd/local/pci_if.c
LIB_C_FILES += rtemsbsd/local/usb_if.c
LIB_C_FILES += rtemsbsd/local/mmcbus_if.c
LIB_C_FILES += rtemsbsd/local/mmcbr_if.c
LIB_C_FILES += rtemsbsd/rtems/ipsec_get_policylen.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-assert.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-arp-processor.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-autoconf.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-bus-dma.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-bus-dma-mbuf.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-cam.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-chunk.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-conf.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-configintrhook.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-delay.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-get-ethernet-addr.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-get-file.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-get-mac-address.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-get-allocator-domain-size.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-get-task-priority.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-get-task-stack-size.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-init.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-jail.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-log.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-malloc.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-mbuf.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-mutex.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-muteximpl.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-newproc.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-nexus.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-page.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-panic.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-pci_bus.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-pci_cfgreg.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-program.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-rwlock.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-shell.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-shell-netcmds.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-signal.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-sx.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-syscall-api.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-sysctlbyname.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-sysctl.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-sysctlnametomib.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-thread.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-timesupport.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsdnet-rtrequest.c
LIB_C_FILES += rtemsbsd/rtems/rtems-kvm.c
LIB_C_FILES += rtemsbsd/rtems/rtems_mii_ioctl_kern.c
LIB_C_FILES += rtemsbsd/rtems/rtems-syslog-initialize.c
LIB_C_FILES += rtemsbsd/rtems/syslog.c
LIB_C_FILES += rtemsbsd/ftpd/ftpd.c
LIB_C_FILES += rtemsbsd/mdns/mdns.c
LIB_C_FILES += rtemsbsd/mdns/mdns-hostname-default.c
LIB_C_FILES += rtemsbsd/pppd/auth.c
LIB_C_FILES += rtemsbsd/pppd/ccp.c
LIB_C_FILES += rtemsbsd/pppd/chap.c
LIB_C_FILES += rtemsbsd/pppd/chap_ms.c
LIB_C_FILES += rtemsbsd/pppd/chat.c
LIB_C_FILES += rtemsbsd/pppd/demand.c
LIB_C_FILES += rtemsbsd/pppd/fsm.c
LIB_C_FILES += rtemsbsd/pppd/ipcp.c
LIB_C_FILES += rtemsbsd/pppd/lcp.c
LIB_C_FILES += rtemsbsd/pppd/magic.c
LIB_C_FILES += rtemsbsd/pppd/options.c
LIB_C_FILES += rtemsbsd/pppd/rtemsmain.c
LIB_C_FILES += rtemsbsd/pppd/rtemspppd.c
LIB_C_FILES += rtemsbsd/pppd/sys-rtems.c
LIB_C_FILES += rtemsbsd/pppd/upap.c
LIB_C_FILES += rtemsbsd/pppd/utils.c
LIB_C_FILES += rtemsbsd/sys/dev/usb/controller/ehci_mpc83xx.c
LIB_C_FILES += rtemsbsd/sys/dev/usb/controller/ohci_lpc.c
LIB_C_FILES += rtemsbsd/sys/dev/usb/controller/usb_otg_transceiver.c
LIB_C_FILES += rtemsbsd/sys/dev/usb/controller/usb_otg_transceiver_dump.c
LIB_C_FILES += rtemsbsd/sys/dev/smc/if_smc_nexus.c
LIB_C_FILES += rtemsbsd/sys/dev/ffec/if_ffec_mcf548x.c
LIB_C_FILES += rtemsbsd/sys/dev/dw_mmc/dw_mmc.c
LIB_C_FILES += rtemsbsd/sys/net/if_ppp.c
LIB_C_FILES += rtemsbsd/sys/net/ppp_tty.c
LIB_C_FILES += rtemsbsd/telnetd/check_passwd.c
LIB_C_FILES += rtemsbsd/telnetd/des.c
LIB_C_FILES += rtemsbsd/telnetd/pty.c
LIB_C_FILES += rtemsbsd/telnetd/telnetd.c
LIB_C_FILES += rtemsbsd/sys/dev/tsec/if_tsec_nexus.c
LIB_GEN_FILES += rtemsbsd/rtems/rtems-kvm-symbols.c
LIB_C_FILES += rtemsbsd/rtems/rtems-kvm-symbols.c
rtemsbsd/rtems/rtems-kvm-symbols.c: rtemsbsd/rtems/generate_kvm_symbols
	./$< > $@
LIB_C_FILES += freebsd/lib/libc/net/nslexer.c
freebsd/lib/libc/net/nslexer.c: freebsd/lib/libc/net/nslexer.l freebsd/lib/libc/net/nsparser.c
	${LEX} -P _nsyy -t $< | sed -e '/YY_BUF_SIZE/s/16384/1024/' > $@
LIB_C_FILES += freebsd/lib/libc/net/nsparser.c
freebsd/lib/libc/net/nsparser.c: freebsd/lib/libc/net/nsparser.y
	yacc -b -b _nsyy -d -p -b _nsyy $<
	sed -e /YY_BUF_SIZE/s/16384/1024/ < -b _nsyy.tab.c > $@
	rm -f -b _nsyy.tab.c
	mv -b _nsyy.tab.h freebsd/lib/libc/net/nsparser.h
LIB_C_FILES += freebsd/lib/libipsec/policy_token.c
freebsd/lib/libipsec/policy_token.c: freebsd/lib/libipsec/policy_token.l freebsd/lib/libipsec/policy_parse.c
	${LEX} -P __libipsecyy -t $< | sed -e '/YY_BUF_SIZE/s/16384/1024/' > $@
LIB_C_FILES += freebsd/lib/libipsec/policy_parse.c
freebsd/lib/libipsec/policy_parse.c: freebsd/lib/libipsec/policy_parse.y
	yacc -b -b __libipsecyy -d -p -b __libipsecyy $<
	sed -e /YY_BUF_SIZE/s/16384/1024/ < -b __libipsecyy.tab.c > $@
	rm -f -b __libipsecyy.tab.c
	mv -b __libipsecyy.tab.h freebsd/lib/libipsec/y.tab.h

TEST_FOOBARCLIENT = testsuite/foobarclient/foobarclient.exe
TEST_FOOBARCLIENT_O_FILES =
TEST_FOOBARCLIENT_D_FILES =
TEST_FOOBARCLIENT_O_FILES += testsuite/foobarclient/test_main.o
TEST_FOOBARCLIENT_D_FILES += testsuite/foobarclient/test_main.d
$(TEST_FOOBARCLIENT): $(TEST_FOOBARCLIENT_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/foobarclient/foobarclient.map $^ -lm -lz -o $@
NET_TESTS += $(TEST_FOOBARCLIENT)
O_FILES += $(TEST_FOOBARCLIENT_O_FILES)
D_FILES += $(TEST_FOOBARCLIENT_D_FILES)

TEST_FOOBARSERVER = testsuite/foobarserver/foobarserver.exe
TEST_FOOBARSERVER_O_FILES =
TEST_FOOBARSERVER_D_FILES =
TEST_FOOBARSERVER_O_FILES += testsuite/foobarserver/test_main.o
TEST_FOOBARSERVER_D_FILES += testsuite/foobarserver/test_main.d
$(TEST_FOOBARSERVER): $(TEST_FOOBARSERVER_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/foobarserver/foobarserver.map $^ -lm -lz -o $@
NET_TESTS += $(TEST_FOOBARSERVER)
O_FILES += $(TEST_FOOBARSERVER_O_FILES)
D_FILES += $(TEST_FOOBARSERVER_D_FILES)

TEST_DHCPCD01 = testsuite/dhcpcd01/dhcpcd01.exe
TEST_DHCPCD01_O_FILES =
TEST_DHCPCD01_D_FILES =
TEST_DHCPCD01_O_FILES += testsuite/dhcpcd01/test_main.o
TEST_DHCPCD01_D_FILES += testsuite/dhcpcd01/test_main.d
$(TEST_DHCPCD01): $(TEST_DHCPCD01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/dhcpcd01/dhcpcd01.map $^ -lm -lz -o $@
NET_TESTS += $(TEST_DHCPCD01)
O_FILES += $(TEST_DHCPCD01_O_FILES)
D_FILES += $(TEST_DHCPCD01_D_FILES)

TEST_DHCPCD02 = testsuite/dhcpcd02/dhcpcd02.exe
TEST_DHCPCD02_O_FILES =
TEST_DHCPCD02_D_FILES =
TEST_DHCPCD02_O_FILES += testsuite/dhcpcd02/test_main.o
TEST_DHCPCD02_D_FILES += testsuite/dhcpcd02/test_main.d
$(TEST_DHCPCD02): $(TEST_DHCPCD02_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/dhcpcd02/dhcpcd02.map $^ -lm -lz -o $@
NET_TESTS += $(TEST_DHCPCD02)
O_FILES += $(TEST_DHCPCD02_O_FILES)
D_FILES += $(TEST_DHCPCD02_D_FILES)

TEST_ARPHOLE = testsuite/arphole/arphole.exe
TEST_ARPHOLE_O_FILES =
TEST_ARPHOLE_D_FILES =
TEST_ARPHOLE_O_FILES += testsuite/arphole/test_main.o
TEST_ARPHOLE_D_FILES += testsuite/arphole/test_main.d
$(TEST_ARPHOLE): $(TEST_ARPHOLE_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/arphole/arphole.map $^ -lm -lz -o $@
NET_TESTS += $(TEST_ARPHOLE)
O_FILES += $(TEST_ARPHOLE_O_FILES)
D_FILES += $(TEST_ARPHOLE_D_FILES)

TEST_TELNETD01 = testsuite/telnetd01/telnetd01.exe
TEST_TELNETD01_O_FILES =
TEST_TELNETD01_D_FILES =
TEST_TELNETD01_O_FILES += testsuite/telnetd01/test_main.o
TEST_TELNETD01_D_FILES += testsuite/telnetd01/test_main.d
$(TEST_TELNETD01): $(TEST_TELNETD01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/telnetd01/telnetd01.map $^ -lm -lz -o $@
NET_TESTS += $(TEST_TELNETD01)
O_FILES += $(TEST_TELNETD01_O_FILES)
D_FILES += $(TEST_TELNETD01_D_FILES)

TEST_UNIX01 = testsuite/unix01/unix01.exe
TEST_UNIX01_O_FILES =
TEST_UNIX01_D_FILES =
TEST_UNIX01_O_FILES += testsuite/unix01/test_main.o
TEST_UNIX01_D_FILES += testsuite/unix01/test_main.d
$(TEST_UNIX01): $(TEST_UNIX01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/unix01/unix01.map $^ -lm -lz -o $@
TESTS += $(TEST_UNIX01)
O_FILES += $(TEST_UNIX01_O_FILES)
D_FILES += $(TEST_UNIX01_D_FILES)
RUN_TESTS += $(TEST_UNIX01)

TEST_FTPD01 = testsuite/ftpd01/ftpd01.exe
TEST_FTPD01_O_FILES =
TEST_FTPD01_D_FILES =
TEST_FTPD01_O_FILES += testsuite/ftpd01/test_main.o
TEST_FTPD01_D_FILES += testsuite/ftpd01/test_main.d
$(TEST_FTPD01): $(TEST_FTPD01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/ftpd01/ftpd01.map $^ -lm -lz -o $@
NET_TESTS += $(TEST_FTPD01)
O_FILES += $(TEST_FTPD01_O_FILES)
D_FILES += $(TEST_FTPD01_D_FILES)
RUN_NET_TESTS += $(TEST_FTPD01)

TEST_PING01 = testsuite/ping01/ping01.exe
TEST_PING01_O_FILES =
TEST_PING01_D_FILES =
TEST_PING01_O_FILES += testsuite/ping01/test_main.o
TEST_PING01_D_FILES += testsuite/ping01/test_main.d
$(TEST_PING01): $(TEST_PING01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/ping01/ping01.map $^ -lm -lz -o $@
NET_TESTS += $(TEST_PING01)
O_FILES += $(TEST_PING01_O_FILES)
D_FILES += $(TEST_PING01_D_FILES)
RUN_NET_TESTS += $(TEST_PING01)

TEST_SELECTPOLLKQUEUE01 = testsuite/selectpollkqueue01/selectpollkqueue01.exe
TEST_SELECTPOLLKQUEUE01_O_FILES =
TEST_SELECTPOLLKQUEUE01_D_FILES =
TEST_SELECTPOLLKQUEUE01_O_FILES += testsuite/selectpollkqueue01/test_main.o
TEST_SELECTPOLLKQUEUE01_D_FILES += testsuite/selectpollkqueue01/test_main.d
$(TEST_SELECTPOLLKQUEUE01): $(TEST_SELECTPOLLKQUEUE01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/selectpollkqueue01/selectpollkqueue01.map $^ -lm -lz -o $@
TESTS += $(TEST_SELECTPOLLKQUEUE01)
O_FILES += $(TEST_SELECTPOLLKQUEUE01_O_FILES)
D_FILES += $(TEST_SELECTPOLLKQUEUE01_D_FILES)
RUN_TESTS += $(TEST_SELECTPOLLKQUEUE01)

TEST_RWLOCK01 = testsuite/rwlock01/rwlock01.exe
TEST_RWLOCK01_O_FILES =
TEST_RWLOCK01_D_FILES =
TEST_RWLOCK01_O_FILES += testsuite/rwlock01/test_main.o
TEST_RWLOCK01_D_FILES += testsuite/rwlock01/test_main.d
$(TEST_RWLOCK01): $(TEST_RWLOCK01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/rwlock01/rwlock01.map $^ -lm -lz -o $@
TESTS += $(TEST_RWLOCK01)
O_FILES += $(TEST_RWLOCK01_O_FILES)
D_FILES += $(TEST_RWLOCK01_D_FILES)
RUN_TESTS += $(TEST_RWLOCK01)

TEST_SLEEP01 = testsuite/sleep01/sleep01.exe
TEST_SLEEP01_O_FILES =
TEST_SLEEP01_D_FILES =
TEST_SLEEP01_O_FILES += testsuite/sleep01/test_main.o
TEST_SLEEP01_D_FILES += testsuite/sleep01/test_main.d
$(TEST_SLEEP01): $(TEST_SLEEP01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/sleep01/sleep01.map $^ -lm -lz -o $@
TESTS += $(TEST_SLEEP01)
O_FILES += $(TEST_SLEEP01_O_FILES)
D_FILES += $(TEST_SLEEP01_D_FILES)
RUN_TESTS += $(TEST_SLEEP01)

TEST_SYSCALLS01 = testsuite/syscalls01/syscalls01.exe
TEST_SYSCALLS01_O_FILES =
TEST_SYSCALLS01_D_FILES =
TEST_SYSCALLS01_O_FILES += testsuite/syscalls01/test_main.o
TEST_SYSCALLS01_D_FILES += testsuite/syscalls01/test_main.d
$(TEST_SYSCALLS01): $(TEST_SYSCALLS01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/syscalls01/syscalls01.map $^ -lm -lz -o $@
TESTS += $(TEST_SYSCALLS01)
O_FILES += $(TEST_SYSCALLS01_O_FILES)
D_FILES += $(TEST_SYSCALLS01_D_FILES)
RUN_TESTS += $(TEST_SYSCALLS01)

TEST_COMMANDS01 = testsuite/commands01/commands01.exe
TEST_COMMANDS01_O_FILES =
TEST_COMMANDS01_D_FILES =
TEST_COMMANDS01_O_FILES += testsuite/commands01/test_main.o
TEST_COMMANDS01_D_FILES += testsuite/commands01/test_main.d
$(TEST_COMMANDS01): $(TEST_COMMANDS01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/commands01/commands01.map $^ -lm -lz -o $@
TESTS += $(TEST_COMMANDS01)
O_FILES += $(TEST_COMMANDS01_O_FILES)
D_FILES += $(TEST_COMMANDS01_D_FILES)
RUN_TESTS += $(TEST_COMMANDS01)

TEST_USB01 = testsuite/usb01/usb01.exe
TEST_USB01_O_FILES =
TEST_USB01_D_FILES =
TEST_USB01_O_FILES += testsuite/usb01/init.o
TEST_USB01_D_FILES += testsuite/usb01/init.d
TEST_USB01_O_FILES += testsuite/usb01/test-file-system.o
TEST_USB01_D_FILES += testsuite/usb01/test-file-system.d
$(TEST_USB01): $(TEST_USB01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/usb01/usb01.map $^ -lm -lz -o $@
TESTS += $(TEST_USB01)
O_FILES += $(TEST_USB01_O_FILES)
D_FILES += $(TEST_USB01_D_FILES)

TEST_LOOPBACK01 = testsuite/loopback01/loopback01.exe
TEST_LOOPBACK01_O_FILES =
TEST_LOOPBACK01_D_FILES =
TEST_LOOPBACK01_O_FILES += testsuite/loopback01/test_main.o
TEST_LOOPBACK01_D_FILES += testsuite/loopback01/test_main.d
$(TEST_LOOPBACK01): $(TEST_LOOPBACK01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/loopback01/loopback01.map $^ -lm -lz -o $@
TESTS += $(TEST_LOOPBACK01)
O_FILES += $(TEST_LOOPBACK01_O_FILES)
D_FILES += $(TEST_LOOPBACK01_D_FILES)
RUN_TESTS += $(TEST_LOOPBACK01)

TEST_NETSHELL01 = testsuite/netshell01/netshell01.exe
TEST_NETSHELL01_O_FILES =
TEST_NETSHELL01_D_FILES =
TEST_NETSHELL01_O_FILES += testsuite/netshell01/test_main.o
TEST_NETSHELL01_D_FILES += testsuite/netshell01/test_main.d
TEST_NETSHELL01_O_FILES += testsuite/netshell01/shellconfig.o
TEST_NETSHELL01_D_FILES += testsuite/netshell01/shellconfig.d
$(TEST_NETSHELL01): $(TEST_NETSHELL01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/netshell01/netshell01.map $^ -lm -lz -o $@
TESTS += $(TEST_NETSHELL01)
O_FILES += $(TEST_NETSHELL01_O_FILES)
D_FILES += $(TEST_NETSHELL01_D_FILES)

TEST_SWI01 = testsuite/swi01/swi01.exe
TEST_SWI01_O_FILES =
TEST_SWI01_D_FILES =
TEST_SWI01_O_FILES += testsuite/swi01/init.o
TEST_SWI01_D_FILES += testsuite/swi01/init.d
TEST_SWI01_O_FILES += testsuite/swi01/swi_test.o
TEST_SWI01_D_FILES += testsuite/swi01/swi_test.d
$(TEST_SWI01): $(TEST_SWI01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/swi01/swi01.map $^ -lm -lz -o $@
TESTS += $(TEST_SWI01)
O_FILES += $(TEST_SWI01_O_FILES)
D_FILES += $(TEST_SWI01_D_FILES)
RUN_TESTS += $(TEST_SWI01)

TEST_TIMEOUT01 = testsuite/timeout01/timeout01.exe
TEST_TIMEOUT01_O_FILES =
TEST_TIMEOUT01_D_FILES =
TEST_TIMEOUT01_O_FILES += testsuite/timeout01/init.o
TEST_TIMEOUT01_D_FILES += testsuite/timeout01/init.d
TEST_TIMEOUT01_O_FILES += testsuite/timeout01/timeout_test.o
TEST_TIMEOUT01_D_FILES += testsuite/timeout01/timeout_test.d
$(TEST_TIMEOUT01): $(TEST_TIMEOUT01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/timeout01/timeout01.map $^ -lm -lz -o $@
TESTS += $(TEST_TIMEOUT01)
O_FILES += $(TEST_TIMEOUT01_O_FILES)
D_FILES += $(TEST_TIMEOUT01_D_FILES)
RUN_TESTS += $(TEST_TIMEOUT01)

TEST_INIT01 = testsuite/init01/init01.exe
TEST_INIT01_O_FILES =
TEST_INIT01_D_FILES =
TEST_INIT01_O_FILES += testsuite/init01/test_main.o
TEST_INIT01_D_FILES += testsuite/init01/test_main.d
$(TEST_INIT01): $(TEST_INIT01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/init01/init01.map $^ -lm -lz -o $@
TESTS += $(TEST_INIT01)
O_FILES += $(TEST_INIT01_O_FILES)
D_FILES += $(TEST_INIT01_D_FILES)
RUN_TESTS += $(TEST_INIT01)

TEST_THREAD01 = testsuite/thread01/thread01.exe
TEST_THREAD01_O_FILES =
TEST_THREAD01_D_FILES =
TEST_THREAD01_O_FILES += testsuite/thread01/test_main.o
TEST_THREAD01_D_FILES += testsuite/thread01/test_main.d
$(TEST_THREAD01): $(TEST_THREAD01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/thread01/thread01.map $^ -lm -lz -o $@
TESTS += $(TEST_THREAD01)
O_FILES += $(TEST_THREAD01_O_FILES)
D_FILES += $(TEST_THREAD01_D_FILES)
RUN_TESTS += $(TEST_THREAD01)

TEST_MUTEX01 = testsuite/mutex01/mutex01.exe
TEST_MUTEX01_O_FILES =
TEST_MUTEX01_D_FILES =
TEST_MUTEX01_O_FILES += testsuite/mutex01/test_main.o
TEST_MUTEX01_D_FILES += testsuite/mutex01/test_main.d
$(TEST_MUTEX01): $(TEST_MUTEX01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/mutex01/mutex01.map $^ -lm -lz -o $@
TESTS += $(TEST_MUTEX01)
O_FILES += $(TEST_MUTEX01_O_FILES)
D_FILES += $(TEST_MUTEX01_D_FILES)
RUN_TESTS += $(TEST_MUTEX01)

TEST_CONDVAR01 = testsuite/condvar01/condvar01.exe
TEST_CONDVAR01_O_FILES =
TEST_CONDVAR01_D_FILES =
TEST_CONDVAR01_O_FILES += testsuite/condvar01/test_main.o
TEST_CONDVAR01_D_FILES += testsuite/condvar01/test_main.d
$(TEST_CONDVAR01): $(TEST_CONDVAR01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/condvar01/condvar01.map $^ -lm -lz -o $@
TESTS += $(TEST_CONDVAR01)
O_FILES += $(TEST_CONDVAR01_O_FILES)
D_FILES += $(TEST_CONDVAR01_D_FILES)
RUN_TESTS += $(TEST_CONDVAR01)

TEST_PPP01 = testsuite/ppp01/ppp01.exe
TEST_PPP01_O_FILES =
TEST_PPP01_D_FILES =
TEST_PPP01_O_FILES += testsuite/ppp01/test_main.o
TEST_PPP01_D_FILES += testsuite/ppp01/test_main.d
$(TEST_PPP01): $(TEST_PPP01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/ppp01/ppp01.map $^ -lm -lz -o $@
TESTS += $(TEST_PPP01)
O_FILES += $(TEST_PPP01_O_FILES)
D_FILES += $(TEST_PPP01_D_FILES)

TEST_ZEROCOPY01 = testsuite/zerocopy01/zerocopy01.exe
TEST_ZEROCOPY01_O_FILES =
TEST_ZEROCOPY01_D_FILES =
TEST_ZEROCOPY01_O_FILES += testsuite/zerocopy01/test_main.o
TEST_ZEROCOPY01_D_FILES += testsuite/zerocopy01/test_main.d
$(TEST_ZEROCOPY01): $(TEST_ZEROCOPY01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/zerocopy01/zerocopy01.map $^ -lm -lz -o $@
NET_TESTS += $(TEST_ZEROCOPY01)
O_FILES += $(TEST_ZEROCOPY01_O_FILES)
D_FILES += $(TEST_ZEROCOPY01_D_FILES)

TEST_SMP01 = testsuite/smp01/smp01.exe
TEST_SMP01_O_FILES =
TEST_SMP01_D_FILES =
TEST_SMP01_O_FILES += testsuite/smp01/test_main.o
TEST_SMP01_D_FILES += testsuite/smp01/test_main.d
$(TEST_SMP01): $(TEST_SMP01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/smp01/smp01.map $^ -lm -lz -o $@
TESTS += $(TEST_SMP01)
O_FILES += $(TEST_SMP01_O_FILES)
D_FILES += $(TEST_SMP01_D_FILES)
RUN_TESTS += $(TEST_SMP01)

TEST_MEDIA01 = testsuite/media01/media01.exe
TEST_MEDIA01_O_FILES =
TEST_MEDIA01_D_FILES =
TEST_MEDIA01_O_FILES += testsuite/media01/test_main.o
TEST_MEDIA01_D_FILES += testsuite/media01/test_main.d
$(TEST_MEDIA01): $(TEST_MEDIA01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/media01/media01.map $^ -lm -lz -o $@
TESTS += $(TEST_MEDIA01)
O_FILES += $(TEST_MEDIA01_O_FILES)
D_FILES += $(TEST_MEDIA01_D_FILES)

TEST_VLAN01 = testsuite/vlan01/vlan01.exe
TEST_VLAN01_O_FILES =
TEST_VLAN01_D_FILES =
TEST_VLAN01_O_FILES += testsuite/vlan01/test_main.o
TEST_VLAN01_D_FILES += testsuite/vlan01/test_main.d
$(TEST_VLAN01): $(TEST_VLAN01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/vlan01/vlan01.map $^ -lm -lz -o $@
NET_TESTS += $(TEST_VLAN01)
O_FILES += $(TEST_VLAN01_O_FILES)
D_FILES += $(TEST_VLAN01_D_FILES)
RUN_NET_TESTS += $(TEST_VLAN01)

TEST_LAGG01 = testsuite/lagg01/lagg01.exe
TEST_LAGG01_O_FILES =
TEST_LAGG01_D_FILES =
TEST_LAGG01_O_FILES += testsuite/lagg01/test_main.o
TEST_LAGG01_D_FILES += testsuite/lagg01/test_main.d
$(TEST_LAGG01): $(TEST_LAGG01_O_FILES) $(LIB)
	$(LINK.c) -Wl,-Map,testsuite/lagg01/lagg01.map $^ -lm -lz -o $@
NET_TESTS += $(TEST_LAGG01)
O_FILES += $(TEST_LAGG01_O_FILES)
D_FILES += $(TEST_LAGG01_D_FILES)
RUN_NET_TESTS += $(TEST_LAGG01)
freebsd/include/rpc/rpcb_prot.h: freebsd/include/rpc/rpcb_prot.x
		rm -f $@
		rpcgen -h -o $@ $<
freebsd/sbin/route/keywords.h: freebsd/sbin/route/keywords
	sed -e '/^#/d' -e '/^$$/d' $< > freebsd/sbin/route/keywords.tmp
	LC_ALL=C tr 'a-z' 'A-Z' < freebsd/sbin/route/keywords.tmp | paste freebsd/sbin/route/keywords.tmp - | \
	awk '{ if (NF > 1) printf "#define\tK_%s\t%d\n\t{\"%s\", K_%s},\n", $$2, NR, $$1, $$2 }' > $@
	rm -f freebsd/sbin/route/keywords.tmp
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_close.c
freebsd/lib/libc/db/btree/bt_close.o: freebsd/lib/libc/db/btree/bt_close.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_conv.c
freebsd/lib/libc/db/btree/bt_conv.o: freebsd/lib/libc/db/btree/bt_conv.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_debug.c
freebsd/lib/libc/db/btree/bt_debug.o: freebsd/lib/libc/db/btree/bt_debug.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_delete.c
freebsd/lib/libc/db/btree/bt_delete.o: freebsd/lib/libc/db/btree/bt_delete.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_get.c
freebsd/lib/libc/db/btree/bt_get.o: freebsd/lib/libc/db/btree/bt_get.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_open.c
freebsd/lib/libc/db/btree/bt_open.o: freebsd/lib/libc/db/btree/bt_open.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_overflow.c
freebsd/lib/libc/db/btree/bt_overflow.o: freebsd/lib/libc/db/btree/bt_overflow.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_page.c
freebsd/lib/libc/db/btree/bt_page.o: freebsd/lib/libc/db/btree/bt_page.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_put.c
freebsd/lib/libc/db/btree/bt_put.o: freebsd/lib/libc/db/btree/bt_put.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_search.c
freebsd/lib/libc/db/btree/bt_search.o: freebsd/lib/libc/db/btree/bt_search.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_seq.c
freebsd/lib/libc/db/btree/bt_seq.o: freebsd/lib/libc/db/btree/bt_seq.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_split.c
freebsd/lib/libc/db/btree/bt_split.o: freebsd/lib/libc/db/btree/bt_split.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_utils.c
freebsd/lib/libc/db/btree/bt_utils.o: freebsd/lib/libc/db/btree/bt_utils.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/db/db.c
freebsd/lib/libc/db/db/db.o: freebsd/lib/libc/db/db/db.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/mpool/mpool.c
freebsd/lib/libc/db/mpool/mpool.o: freebsd/lib/libc/db/mpool/mpool.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/mpool/mpool-compat.c
freebsd/lib/libc/db/mpool/mpool-compat.o: freebsd/lib/libc/db/mpool/mpool-compat.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/recno/rec_close.c
freebsd/lib/libc/db/recno/rec_close.o: freebsd/lib/libc/db/recno/rec_close.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/recno/rec_delete.c
freebsd/lib/libc/db/recno/rec_delete.o: freebsd/lib/libc/db/recno/rec_delete.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/recno/rec_get.c
freebsd/lib/libc/db/recno/rec_get.o: freebsd/lib/libc/db/recno/rec_get.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/recno/rec_open.c
freebsd/lib/libc/db/recno/rec_open.o: freebsd/lib/libc/db/recno/rec_open.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/recno/rec_put.c
freebsd/lib/libc/db/recno/rec_put.o: freebsd/lib/libc/db/recno/rec_put.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/recno/rec_search.c
freebsd/lib/libc/db/recno/rec_search.o: freebsd/lib/libc/db/recno/rec_search.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/recno/rec_seq.c
freebsd/lib/libc/db/recno/rec_seq.o: freebsd/lib/libc/db/recno/rec_seq.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/db/recno/rec_utils.c
freebsd/lib/libc/db/recno/rec_utils.o: freebsd/lib/libc/db/recno/rec_utils.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DBINTERFACE_PRIVATE -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/bin/hostname/hostname.c
freebsd/bin/hostname/hostname.o: freebsd/bin/hostname/hostname.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/gen/err.c
freebsd/lib/libc/gen/err.o: freebsd/lib/libc/gen/err.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/gen/feature_present.c
freebsd/lib/libc/gen/feature_present.o: freebsd/lib/libc/gen/feature_present.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/gen/gethostname.c
freebsd/lib/libc/gen/gethostname.o: freebsd/lib/libc/gen/gethostname.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/gen/sethostname.c
freebsd/lib/libc/gen/sethostname.o: freebsd/lib/libc/gen/sethostname.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/inet/inet_addr.c
freebsd/lib/libc/inet/inet_addr.o: freebsd/lib/libc/inet/inet_addr.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/inet/inet_cidr_ntop.c
freebsd/lib/libc/inet/inet_cidr_ntop.o: freebsd/lib/libc/inet/inet_cidr_ntop.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/inet/inet_cidr_pton.c
freebsd/lib/libc/inet/inet_cidr_pton.o: freebsd/lib/libc/inet/inet_cidr_pton.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/inet/inet_lnaof.c
freebsd/lib/libc/inet/inet_lnaof.o: freebsd/lib/libc/inet/inet_lnaof.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/inet/inet_makeaddr.c
freebsd/lib/libc/inet/inet_makeaddr.o: freebsd/lib/libc/inet/inet_makeaddr.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/inet/inet_neta.c
freebsd/lib/libc/inet/inet_neta.o: freebsd/lib/libc/inet/inet_neta.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/inet/inet_net_ntop.c
freebsd/lib/libc/inet/inet_net_ntop.o: freebsd/lib/libc/inet/inet_net_ntop.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/inet/inet_netof.c
freebsd/lib/libc/inet/inet_netof.o: freebsd/lib/libc/inet/inet_netof.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/inet/inet_net_pton.c
freebsd/lib/libc/inet/inet_net_pton.o: freebsd/lib/libc/inet/inet_net_pton.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/inet/inet_network.c
freebsd/lib/libc/inet/inet_network.o: freebsd/lib/libc/inet/inet_network.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/inet/inet_ntoa.c
freebsd/lib/libc/inet/inet_ntoa.o: freebsd/lib/libc/inet/inet_ntoa.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/inet/inet_ntop.c
freebsd/lib/libc/inet/inet_ntop.o: freebsd/lib/libc/inet/inet_ntop.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/inet/inet_pton.c
freebsd/lib/libc/inet/inet_pton.o: freebsd/lib/libc/inet/inet_pton.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/inet/nsap_addr.c
freebsd/lib/libc/inet/nsap_addr.o: freebsd/lib/libc/inet/nsap_addr.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/isc/ev_streams.c
freebsd/lib/libc/isc/ev_streams.o: freebsd/lib/libc/isc/ev_streams.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/isc/ev_timers.c
freebsd/lib/libc/isc/ev_timers.o: freebsd/lib/libc/isc/ev_timers.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/nameser/ns_name.c
freebsd/lib/libc/nameser/ns_name.o: freebsd/lib/libc/nameser/ns_name.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/nameser/ns_netint.c
freebsd/lib/libc/nameser/ns_netint.o: freebsd/lib/libc/nameser/ns_netint.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/nameser/ns_parse.c
freebsd/lib/libc/nameser/ns_parse.o: freebsd/lib/libc/nameser/ns_parse.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/nameser/ns_print.c
freebsd/lib/libc/nameser/ns_print.o: freebsd/lib/libc/nameser/ns_print.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/nameser/ns_samedomain.c
freebsd/lib/libc/nameser/ns_samedomain.o: freebsd/lib/libc/nameser/ns_samedomain.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/nameser/ns_ttl.c
freebsd/lib/libc/nameser/ns_ttl.o: freebsd/lib/libc/nameser/ns_ttl.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/base64.c
freebsd/lib/libc/net/base64.o: freebsd/lib/libc/net/base64.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/ether_addr.c
freebsd/lib/libc/net/ether_addr.o: freebsd/lib/libc/net/ether_addr.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/gai_strerror.c
freebsd/lib/libc/net/gai_strerror.o: freebsd/lib/libc/net/gai_strerror.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/getaddrinfo.c
freebsd/lib/libc/net/getaddrinfo.o: freebsd/lib/libc/net/getaddrinfo.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/gethostbydns.c
freebsd/lib/libc/net/gethostbydns.o: freebsd/lib/libc/net/gethostbydns.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/gethostbyht.c
freebsd/lib/libc/net/gethostbyht.o: freebsd/lib/libc/net/gethostbyht.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/gethostbynis.c
freebsd/lib/libc/net/gethostbynis.o: freebsd/lib/libc/net/gethostbynis.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/gethostnamadr.c
freebsd/lib/libc/net/gethostnamadr.o: freebsd/lib/libc/net/gethostnamadr.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/getifaddrs.c
freebsd/lib/libc/net/getifaddrs.o: freebsd/lib/libc/net/getifaddrs.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/getifmaddrs.c
freebsd/lib/libc/net/getifmaddrs.o: freebsd/lib/libc/net/getifmaddrs.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/getnameinfo.c
freebsd/lib/libc/net/getnameinfo.o: freebsd/lib/libc/net/getnameinfo.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/getnetbydns.c
freebsd/lib/libc/net/getnetbydns.o: freebsd/lib/libc/net/getnetbydns.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/getnetbyht.c
freebsd/lib/libc/net/getnetbyht.o: freebsd/lib/libc/net/getnetbyht.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/getnetbynis.c
freebsd/lib/libc/net/getnetbynis.o: freebsd/lib/libc/net/getnetbynis.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/getnetnamadr.c
freebsd/lib/libc/net/getnetnamadr.o: freebsd/lib/libc/net/getnetnamadr.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/getproto.c
freebsd/lib/libc/net/getproto.o: freebsd/lib/libc/net/getproto.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/getprotoent.c
freebsd/lib/libc/net/getprotoent.o: freebsd/lib/libc/net/getprotoent.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/getprotoname.c
freebsd/lib/libc/net/getprotoname.o: freebsd/lib/libc/net/getprotoname.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/getservent.c
freebsd/lib/libc/net/getservent.o: freebsd/lib/libc/net/getservent.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/if_indextoname.c
freebsd/lib/libc/net/if_indextoname.o: freebsd/lib/libc/net/if_indextoname.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/if_nameindex.c
freebsd/lib/libc/net/if_nameindex.o: freebsd/lib/libc/net/if_nameindex.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/if_nametoindex.c
freebsd/lib/libc/net/if_nametoindex.o: freebsd/lib/libc/net/if_nametoindex.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/ip6opt.c
freebsd/lib/libc/net/ip6opt.o: freebsd/lib/libc/net/ip6opt.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/linkaddr.c
freebsd/lib/libc/net/linkaddr.o: freebsd/lib/libc/net/linkaddr.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/map_v4v6.c
freebsd/lib/libc/net/map_v4v6.o: freebsd/lib/libc/net/map_v4v6.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/name6.c
freebsd/lib/libc/net/name6.o: freebsd/lib/libc/net/name6.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/nsdispatch.c
freebsd/lib/libc/net/nsdispatch.o: freebsd/lib/libc/net/nsdispatch.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/rcmd.c
freebsd/lib/libc/net/rcmd.o: freebsd/lib/libc/net/rcmd.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/recv.c
freebsd/lib/libc/net/recv.o: freebsd/lib/libc/net/recv.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/rthdr.c
freebsd/lib/libc/net/rthdr.o: freebsd/lib/libc/net/rthdr.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/net/send.c
freebsd/lib/libc/net/send.o: freebsd/lib/libc/net/send.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/posix1e/mac.c
freebsd/lib/libc/posix1e/mac.o: freebsd/lib/libc/posix1e/mac.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/resolv/h_errno.c
freebsd/lib/libc/resolv/h_errno.o: freebsd/lib/libc/resolv/h_errno.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/resolv/herror.c
freebsd/lib/libc/resolv/herror.o: freebsd/lib/libc/resolv/herror.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/resolv/mtctxres.c
freebsd/lib/libc/resolv/mtctxres.o: freebsd/lib/libc/resolv/mtctxres.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/resolv/res_comp.c
freebsd/lib/libc/resolv/res_comp.o: freebsd/lib/libc/resolv/res_comp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/resolv/res_data.c
freebsd/lib/libc/resolv/res_data.o: freebsd/lib/libc/resolv/res_data.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/resolv/res_debug.c
freebsd/lib/libc/resolv/res_debug.o: freebsd/lib/libc/resolv/res_debug.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/resolv/res_findzonecut.c
freebsd/lib/libc/resolv/res_findzonecut.o: freebsd/lib/libc/resolv/res_findzonecut.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/resolv/res_init.c
freebsd/lib/libc/resolv/res_init.o: freebsd/lib/libc/resolv/res_init.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/resolv/res_mkquery.c
freebsd/lib/libc/resolv/res_mkquery.o: freebsd/lib/libc/resolv/res_mkquery.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/resolv/res_mkupdate.c
freebsd/lib/libc/resolv/res_mkupdate.o: freebsd/lib/libc/resolv/res_mkupdate.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/resolv/res_query.c
freebsd/lib/libc/resolv/res_query.o: freebsd/lib/libc/resolv/res_query.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/resolv/res_send.c
freebsd/lib/libc/resolv/res_send.o: freebsd/lib/libc/resolv/res_send.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/resolv/res_state.c
freebsd/lib/libc/resolv/res_state.o: freebsd/lib/libc/resolv/res_state.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/resolv/res_update.c
freebsd/lib/libc/resolv/res_update.o: freebsd/lib/libc/resolv/res_update.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/stdio/fgetln.c
freebsd/lib/libc/stdio/fgetln.o: freebsd/lib/libc/stdio/fgetln.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/stdlib/strtonum.c
freebsd/lib/libc/stdlib/strtonum.o: freebsd/lib/libc/stdlib/strtonum.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libc/string/strsep.c
freebsd/lib/libc/string/strsep.o: freebsd/lib/libc/string/strsep.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libipsec/ipsec_dump_policy.c
freebsd/lib/libipsec/ipsec_dump_policy.o: freebsd/lib/libipsec/ipsec_dump_policy.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libipsec/ipsec_get_policylen.c
freebsd/lib/libipsec/ipsec_get_policylen.o: freebsd/lib/libipsec/ipsec_get_policylen.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libipsec/ipsec_strerror.c
freebsd/lib/libipsec/ipsec_strerror.o: freebsd/lib/libipsec/ipsec_strerror.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libipsec/pfkey.c
freebsd/lib/libipsec/pfkey.o: freebsd/lib/libipsec/pfkey.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libipsec/pfkey_dump.c
freebsd/lib/libipsec/pfkey_dump.o: freebsd/lib/libipsec/pfkey_dump.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libmemstat/memstat_all.c
freebsd/lib/libmemstat/memstat_all.o: freebsd/lib/libmemstat/memstat_all.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libmemstat/memstat.c
freebsd/lib/libmemstat/memstat.o: freebsd/lib/libmemstat/memstat.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libmemstat/memstat_malloc.c
freebsd/lib/libmemstat/memstat_malloc.o: freebsd/lib/libmemstat/memstat_malloc.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libmemstat/memstat_uma.c
freebsd/lib/libmemstat/memstat_uma.o: freebsd/lib/libmemstat/memstat_uma.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libutil/expand_number.c
freebsd/lib/libutil/expand_number.o: freebsd/lib/libutil/expand_number.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libutil/humanize_number.c
freebsd/lib/libutil/humanize_number.o: freebsd/lib/libutil/humanize_number.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/lib/libutil/trimdomain.c
freebsd/lib/libutil/trimdomain.o: freebsd/lib/libutil/trimdomain.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/dhclient/alloc.c
freebsd/sbin/dhclient/alloc.o: freebsd/sbin/dhclient/alloc.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/dhclient/bpf.c
freebsd/sbin/dhclient/bpf.o: freebsd/sbin/dhclient/bpf.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/dhclient/clparse.c
freebsd/sbin/dhclient/clparse.o: freebsd/sbin/dhclient/clparse.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/dhclient/conflex.c
freebsd/sbin/dhclient/conflex.o: freebsd/sbin/dhclient/conflex.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/dhclient/convert.c
freebsd/sbin/dhclient/convert.o: freebsd/sbin/dhclient/convert.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/dhclient/dhclient.c
freebsd/sbin/dhclient/dhclient.o: freebsd/sbin/dhclient/dhclient.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/dhclient/dispatch.c
freebsd/sbin/dhclient/dispatch.o: freebsd/sbin/dhclient/dispatch.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/dhclient/errwarn.c
freebsd/sbin/dhclient/errwarn.o: freebsd/sbin/dhclient/errwarn.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/dhclient/hash.c
freebsd/sbin/dhclient/hash.o: freebsd/sbin/dhclient/hash.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/dhclient/inet.c
freebsd/sbin/dhclient/inet.o: freebsd/sbin/dhclient/inet.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/dhclient/options.c
freebsd/sbin/dhclient/options.o: freebsd/sbin/dhclient/options.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/dhclient/packet.c
freebsd/sbin/dhclient/packet.o: freebsd/sbin/dhclient/packet.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/dhclient/parse.c
freebsd/sbin/dhclient/parse.o: freebsd/sbin/dhclient/parse.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/dhclient/privsep.c
freebsd/sbin/dhclient/privsep.o: freebsd/sbin/dhclient/privsep.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/dhclient/tables.c
freebsd/sbin/dhclient/tables.o: freebsd/sbin/dhclient/tables.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/dhclient/tree.c
freebsd/sbin/dhclient/tree.o: freebsd/sbin/dhclient/tree.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/ifconfig/af_atalk.c
freebsd/sbin/ifconfig/af_atalk.o: freebsd/sbin/ifconfig/af_atalk.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/ifconfig/af_inet6.c
freebsd/sbin/ifconfig/af_inet6.o: freebsd/sbin/ifconfig/af_inet6.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/ifconfig/af_inet.c
freebsd/sbin/ifconfig/af_inet.o: freebsd/sbin/ifconfig/af_inet.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/ifconfig/af_link.c
freebsd/sbin/ifconfig/af_link.o: freebsd/sbin/ifconfig/af_link.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/ifconfig/af_nd6.c
freebsd/sbin/ifconfig/af_nd6.o: freebsd/sbin/ifconfig/af_nd6.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/ifconfig/ifbridge.c
freebsd/sbin/ifconfig/ifbridge.o: freebsd/sbin/ifconfig/ifbridge.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/ifconfig/ifcarp.c
freebsd/sbin/ifconfig/ifcarp.o: freebsd/sbin/ifconfig/ifcarp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/ifconfig/ifclone.c
freebsd/sbin/ifconfig/ifclone.o: freebsd/sbin/ifconfig/ifclone.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/ifconfig/ifconfig.c
freebsd/sbin/ifconfig/ifconfig.o: freebsd/sbin/ifconfig/ifconfig.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/ifconfig/ifgif.c
freebsd/sbin/ifconfig/ifgif.o: freebsd/sbin/ifconfig/ifgif.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/ifconfig/ifgre.c
freebsd/sbin/ifconfig/ifgre.o: freebsd/sbin/ifconfig/ifgre.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/ifconfig/ifgroup.c
freebsd/sbin/ifconfig/ifgroup.o: freebsd/sbin/ifconfig/ifgroup.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/ifconfig/iflagg.c
freebsd/sbin/ifconfig/iflagg.o: freebsd/sbin/ifconfig/iflagg.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/ifconfig/ifmac.c
freebsd/sbin/ifconfig/ifmac.o: freebsd/sbin/ifconfig/ifmac.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/ifconfig/ifmedia.c
freebsd/sbin/ifconfig/ifmedia.o: freebsd/sbin/ifconfig/ifmedia.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/ifconfig/ifpfsync.c
freebsd/sbin/ifconfig/ifpfsync.o: freebsd/sbin/ifconfig/ifpfsync.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/ifconfig/ifvlan.c
freebsd/sbin/ifconfig/ifvlan.o: freebsd/sbin/ifconfig/ifvlan.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/ping6/ping6.c
freebsd/sbin/ping6/ping6.o: freebsd/sbin/ping6/ping6.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/ping/ping.c
freebsd/sbin/ping/ping.o: freebsd/sbin/ping/ping.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/sbin/route/route.c
freebsd/sbin/route/route.o: freebsd/sbin/route/route.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/usr.bin/netstat/atalk.c
freebsd/usr.bin/netstat/atalk.o: freebsd/usr.bin/netstat/atalk.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/usr.bin/netstat/bpf.c
freebsd/usr.bin/netstat/bpf.o: freebsd/usr.bin/netstat/bpf.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/usr.bin/netstat/if.c
freebsd/usr.bin/netstat/if.o: freebsd/usr.bin/netstat/if.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/usr.bin/netstat/inet6.c
freebsd/usr.bin/netstat/inet6.o: freebsd/usr.bin/netstat/inet6.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/usr.bin/netstat/inet.c
freebsd/usr.bin/netstat/inet.o: freebsd/usr.bin/netstat/inet.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/usr.bin/netstat/ipsec.c
freebsd/usr.bin/netstat/ipsec.o: freebsd/usr.bin/netstat/ipsec.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/usr.bin/netstat/main.c
freebsd/usr.bin/netstat/main.o: freebsd/usr.bin/netstat/main.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/usr.bin/netstat/mbuf.c
freebsd/usr.bin/netstat/mbuf.o: freebsd/usr.bin/netstat/mbuf.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/usr.bin/netstat/mroute6.c
freebsd/usr.bin/netstat/mroute6.o: freebsd/usr.bin/netstat/mroute6.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/usr.bin/netstat/mroute.c
freebsd/usr.bin/netstat/mroute.o: freebsd/usr.bin/netstat/mroute.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/usr.bin/netstat/route.c
freebsd/usr.bin/netstat/route.o: freebsd/usr.bin/netstat/route.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/usr.bin/netstat/pfkey.c
freebsd/usr.bin/netstat/pfkey.o: freebsd/usr.bin/netstat/pfkey.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/usr.bin/netstat/sctp.c
freebsd/usr.bin/netstat/sctp.o: freebsd/usr.bin/netstat/sctp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/usr.bin/netstat/unix.c
freebsd/usr.bin/netstat/unix.o: freebsd/usr.bin/netstat/unix.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DINET6 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/addrtoname.c
freebsd/contrib/tcpdump/addrtoname.o: freebsd/contrib/tcpdump/addrtoname.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/af.c
freebsd/contrib/tcpdump/af.o: freebsd/contrib/tcpdump/af.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/bpf_dump.c
freebsd/contrib/tcpdump/bpf_dump.o: freebsd/contrib/tcpdump/bpf_dump.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/checksum.c
freebsd/contrib/tcpdump/checksum.o: freebsd/contrib/tcpdump/checksum.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/cpack.c
freebsd/contrib/tcpdump/cpack.o: freebsd/contrib/tcpdump/cpack.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/gmpls.c
freebsd/contrib/tcpdump/gmpls.o: freebsd/contrib/tcpdump/gmpls.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/gmt2local.c
freebsd/contrib/tcpdump/gmt2local.o: freebsd/contrib/tcpdump/gmt2local.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/in_cksum.c
freebsd/contrib/tcpdump/in_cksum.o: freebsd/contrib/tcpdump/in_cksum.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/ipproto.c
freebsd/contrib/tcpdump/ipproto.o: freebsd/contrib/tcpdump/ipproto.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/machdep.c
freebsd/contrib/tcpdump/machdep.o: freebsd/contrib/tcpdump/machdep.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/nlpid.c
freebsd/contrib/tcpdump/nlpid.o: freebsd/contrib/tcpdump/nlpid.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/l2vpn.c
freebsd/contrib/tcpdump/l2vpn.o: freebsd/contrib/tcpdump/l2vpn.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/oui.c
freebsd/contrib/tcpdump/oui.o: freebsd/contrib/tcpdump/oui.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/parsenfsfh.c
freebsd/contrib/tcpdump/parsenfsfh.o: freebsd/contrib/tcpdump/parsenfsfh.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-802_11.c
freebsd/contrib/tcpdump/print-802_11.o: freebsd/contrib/tcpdump/print-802_11.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-802_15_4.c
freebsd/contrib/tcpdump/print-802_15_4.o: freebsd/contrib/tcpdump/print-802_15_4.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-ah.c
freebsd/contrib/tcpdump/print-ah.o: freebsd/contrib/tcpdump/print-ah.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-aodv.c
freebsd/contrib/tcpdump/print-aodv.o: freebsd/contrib/tcpdump/print-aodv.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-ap1394.c
freebsd/contrib/tcpdump/print-ap1394.o: freebsd/contrib/tcpdump/print-ap1394.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-arcnet.c
freebsd/contrib/tcpdump/print-arcnet.o: freebsd/contrib/tcpdump/print-arcnet.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-arp.c
freebsd/contrib/tcpdump/print-arp.o: freebsd/contrib/tcpdump/print-arp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-ascii.c
freebsd/contrib/tcpdump/print-ascii.o: freebsd/contrib/tcpdump/print-ascii.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-atalk.c
freebsd/contrib/tcpdump/print-atalk.o: freebsd/contrib/tcpdump/print-atalk.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-atm.c
freebsd/contrib/tcpdump/print-atm.o: freebsd/contrib/tcpdump/print-atm.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-babel.c
freebsd/contrib/tcpdump/print-babel.o: freebsd/contrib/tcpdump/print-babel.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-beep.c
freebsd/contrib/tcpdump/print-beep.o: freebsd/contrib/tcpdump/print-beep.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-bfd.c
freebsd/contrib/tcpdump/print-bfd.o: freebsd/contrib/tcpdump/print-bfd.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-bgp.c
freebsd/contrib/tcpdump/print-bgp.o: freebsd/contrib/tcpdump/print-bgp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-bootp.c
freebsd/contrib/tcpdump/print-bootp.o: freebsd/contrib/tcpdump/print-bootp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-bt.c
freebsd/contrib/tcpdump/print-bt.o: freebsd/contrib/tcpdump/print-bt.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-carp.c
freebsd/contrib/tcpdump/print-carp.o: freebsd/contrib/tcpdump/print-carp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-cdp.c
freebsd/contrib/tcpdump/print-cdp.o: freebsd/contrib/tcpdump/print-cdp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-cfm.c
freebsd/contrib/tcpdump/print-cfm.o: freebsd/contrib/tcpdump/print-cfm.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-chdlc.c
freebsd/contrib/tcpdump/print-chdlc.o: freebsd/contrib/tcpdump/print-chdlc.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-cip.c
freebsd/contrib/tcpdump/print-cip.o: freebsd/contrib/tcpdump/print-cip.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-cnfp.c
freebsd/contrib/tcpdump/print-cnfp.o: freebsd/contrib/tcpdump/print-cnfp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-dccp.c
freebsd/contrib/tcpdump/print-dccp.o: freebsd/contrib/tcpdump/print-dccp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-decnet.c
freebsd/contrib/tcpdump/print-decnet.o: freebsd/contrib/tcpdump/print-decnet.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-dhcp6.c
freebsd/contrib/tcpdump/print-dhcp6.o: freebsd/contrib/tcpdump/print-dhcp6.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-domain.c
freebsd/contrib/tcpdump/print-domain.o: freebsd/contrib/tcpdump/print-domain.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-dtp.c
freebsd/contrib/tcpdump/print-dtp.o: freebsd/contrib/tcpdump/print-dtp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-dvmrp.c
freebsd/contrib/tcpdump/print-dvmrp.o: freebsd/contrib/tcpdump/print-dvmrp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-eap.c
freebsd/contrib/tcpdump/print-eap.o: freebsd/contrib/tcpdump/print-eap.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-egp.c
freebsd/contrib/tcpdump/print-egp.o: freebsd/contrib/tcpdump/print-egp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-eigrp.c
freebsd/contrib/tcpdump/print-eigrp.o: freebsd/contrib/tcpdump/print-eigrp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-enc.c
freebsd/contrib/tcpdump/print-enc.o: freebsd/contrib/tcpdump/print-enc.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-esp.c
freebsd/contrib/tcpdump/print-esp.o: freebsd/contrib/tcpdump/print-esp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-ether.c
freebsd/contrib/tcpdump/print-ether.o: freebsd/contrib/tcpdump/print-ether.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-fddi.c
freebsd/contrib/tcpdump/print-fddi.o: freebsd/contrib/tcpdump/print-fddi.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-forces.c
freebsd/contrib/tcpdump/print-forces.o: freebsd/contrib/tcpdump/print-forces.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-fr.c
freebsd/contrib/tcpdump/print-fr.o: freebsd/contrib/tcpdump/print-fr.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-frag6.c
freebsd/contrib/tcpdump/print-frag6.o: freebsd/contrib/tcpdump/print-frag6.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-gre.c
freebsd/contrib/tcpdump/print-gre.o: freebsd/contrib/tcpdump/print-gre.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-hsrp.c
freebsd/contrib/tcpdump/print-hsrp.o: freebsd/contrib/tcpdump/print-hsrp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-icmp.c
freebsd/contrib/tcpdump/print-icmp.o: freebsd/contrib/tcpdump/print-icmp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-icmp6.c
freebsd/contrib/tcpdump/print-icmp6.o: freebsd/contrib/tcpdump/print-icmp6.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-igmp.c
freebsd/contrib/tcpdump/print-igmp.o: freebsd/contrib/tcpdump/print-igmp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-igrp.c
freebsd/contrib/tcpdump/print-igrp.o: freebsd/contrib/tcpdump/print-igrp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-ip.c
freebsd/contrib/tcpdump/print-ip.o: freebsd/contrib/tcpdump/print-ip.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-ip6.c
freebsd/contrib/tcpdump/print-ip6.o: freebsd/contrib/tcpdump/print-ip6.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-ip6opts.c
freebsd/contrib/tcpdump/print-ip6opts.o: freebsd/contrib/tcpdump/print-ip6opts.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-ipcomp.c
freebsd/contrib/tcpdump/print-ipcomp.o: freebsd/contrib/tcpdump/print-ipcomp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-ipfc.c
freebsd/contrib/tcpdump/print-ipfc.o: freebsd/contrib/tcpdump/print-ipfc.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-ipnet.c
freebsd/contrib/tcpdump/print-ipnet.o: freebsd/contrib/tcpdump/print-ipnet.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-ipx.c
freebsd/contrib/tcpdump/print-ipx.o: freebsd/contrib/tcpdump/print-ipx.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-isakmp.c
freebsd/contrib/tcpdump/print-isakmp.o: freebsd/contrib/tcpdump/print-isakmp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-isoclns.c
freebsd/contrib/tcpdump/print-isoclns.o: freebsd/contrib/tcpdump/print-isoclns.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-juniper.c
freebsd/contrib/tcpdump/print-juniper.o: freebsd/contrib/tcpdump/print-juniper.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-krb.c
freebsd/contrib/tcpdump/print-krb.o: freebsd/contrib/tcpdump/print-krb.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-l2tp.c
freebsd/contrib/tcpdump/print-l2tp.o: freebsd/contrib/tcpdump/print-l2tp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-lane.c
freebsd/contrib/tcpdump/print-lane.o: freebsd/contrib/tcpdump/print-lane.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-ldp.c
freebsd/contrib/tcpdump/print-ldp.o: freebsd/contrib/tcpdump/print-ldp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-llc.c
freebsd/contrib/tcpdump/print-llc.o: freebsd/contrib/tcpdump/print-llc.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-lldp.c
freebsd/contrib/tcpdump/print-lldp.o: freebsd/contrib/tcpdump/print-lldp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-lmp.c
freebsd/contrib/tcpdump/print-lmp.o: freebsd/contrib/tcpdump/print-lmp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-lspping.c
freebsd/contrib/tcpdump/print-lspping.o: freebsd/contrib/tcpdump/print-lspping.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-lwapp.c
freebsd/contrib/tcpdump/print-lwapp.o: freebsd/contrib/tcpdump/print-lwapp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-lwres.c
freebsd/contrib/tcpdump/print-lwres.o: freebsd/contrib/tcpdump/print-lwres.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-mobile.c
freebsd/contrib/tcpdump/print-mobile.o: freebsd/contrib/tcpdump/print-mobile.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-mobility.c
freebsd/contrib/tcpdump/print-mobility.o: freebsd/contrib/tcpdump/print-mobility.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-mpcp.c
freebsd/contrib/tcpdump/print-mpcp.o: freebsd/contrib/tcpdump/print-mpcp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-mpls.c
freebsd/contrib/tcpdump/print-mpls.o: freebsd/contrib/tcpdump/print-mpls.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-msdp.c
freebsd/contrib/tcpdump/print-msdp.o: freebsd/contrib/tcpdump/print-msdp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-msnlb.c
freebsd/contrib/tcpdump/print-msnlb.o: freebsd/contrib/tcpdump/print-msnlb.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-netbios.c
freebsd/contrib/tcpdump/print-netbios.o: freebsd/contrib/tcpdump/print-netbios.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-nfs.c
freebsd/contrib/tcpdump/print-nfs.o: freebsd/contrib/tcpdump/print-nfs.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-ntp.c
freebsd/contrib/tcpdump/print-ntp.o: freebsd/contrib/tcpdump/print-ntp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-null.c
freebsd/contrib/tcpdump/print-null.o: freebsd/contrib/tcpdump/print-null.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-olsr.c
freebsd/contrib/tcpdump/print-olsr.o: freebsd/contrib/tcpdump/print-olsr.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-ospf.c
freebsd/contrib/tcpdump/print-ospf.o: freebsd/contrib/tcpdump/print-ospf.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-ospf6.c
freebsd/contrib/tcpdump/print-ospf6.o: freebsd/contrib/tcpdump/print-ospf6.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-otv.c
freebsd/contrib/tcpdump/print-otv.o: freebsd/contrib/tcpdump/print-otv.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-pflog.c
freebsd/contrib/tcpdump/print-pflog.o: freebsd/contrib/tcpdump/print-pflog.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-pfsync.c
freebsd/contrib/tcpdump/print-pfsync.o: freebsd/contrib/tcpdump/print-pfsync.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-pgm.c
freebsd/contrib/tcpdump/print-pgm.o: freebsd/contrib/tcpdump/print-pgm.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-pim.c
freebsd/contrib/tcpdump/print-pim.o: freebsd/contrib/tcpdump/print-pim.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-ppi.c
freebsd/contrib/tcpdump/print-ppi.o: freebsd/contrib/tcpdump/print-ppi.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-ppp.c
freebsd/contrib/tcpdump/print-ppp.o: freebsd/contrib/tcpdump/print-ppp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-pppoe.c
freebsd/contrib/tcpdump/print-pppoe.o: freebsd/contrib/tcpdump/print-pppoe.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-pptp.c
freebsd/contrib/tcpdump/print-pptp.o: freebsd/contrib/tcpdump/print-pptp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-radius.c
freebsd/contrib/tcpdump/print-radius.o: freebsd/contrib/tcpdump/print-radius.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-raw.c
freebsd/contrib/tcpdump/print-raw.o: freebsd/contrib/tcpdump/print-raw.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-rip.c
freebsd/contrib/tcpdump/print-rip.o: freebsd/contrib/tcpdump/print-rip.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-ripng.c
freebsd/contrib/tcpdump/print-ripng.o: freebsd/contrib/tcpdump/print-ripng.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-rpki-rtr.c
freebsd/contrib/tcpdump/print-rpki-rtr.o: freebsd/contrib/tcpdump/print-rpki-rtr.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-rrcp.c
freebsd/contrib/tcpdump/print-rrcp.o: freebsd/contrib/tcpdump/print-rrcp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-rsvp.c
freebsd/contrib/tcpdump/print-rsvp.o: freebsd/contrib/tcpdump/print-rsvp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-rt6.c
freebsd/contrib/tcpdump/print-rt6.o: freebsd/contrib/tcpdump/print-rt6.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-rx.c
freebsd/contrib/tcpdump/print-rx.o: freebsd/contrib/tcpdump/print-rx.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-sctp.c
freebsd/contrib/tcpdump/print-sctp.o: freebsd/contrib/tcpdump/print-sctp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-sflow.c
freebsd/contrib/tcpdump/print-sflow.o: freebsd/contrib/tcpdump/print-sflow.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-sip.c
freebsd/contrib/tcpdump/print-sip.o: freebsd/contrib/tcpdump/print-sip.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-sl.c
freebsd/contrib/tcpdump/print-sl.o: freebsd/contrib/tcpdump/print-sl.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-sll.c
freebsd/contrib/tcpdump/print-sll.o: freebsd/contrib/tcpdump/print-sll.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-slow.c
freebsd/contrib/tcpdump/print-slow.o: freebsd/contrib/tcpdump/print-slow.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-smb.c
freebsd/contrib/tcpdump/print-smb.o: freebsd/contrib/tcpdump/print-smb.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-snmp.c
freebsd/contrib/tcpdump/print-snmp.o: freebsd/contrib/tcpdump/print-snmp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-stp.c
freebsd/contrib/tcpdump/print-stp.o: freebsd/contrib/tcpdump/print-stp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-sunatm.c
freebsd/contrib/tcpdump/print-sunatm.o: freebsd/contrib/tcpdump/print-sunatm.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-symantec.c
freebsd/contrib/tcpdump/print-symantec.o: freebsd/contrib/tcpdump/print-symantec.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-syslog.c
freebsd/contrib/tcpdump/print-syslog.o: freebsd/contrib/tcpdump/print-syslog.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-tcp.c
freebsd/contrib/tcpdump/print-tcp.o: freebsd/contrib/tcpdump/print-tcp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-telnet.c
freebsd/contrib/tcpdump/print-telnet.o: freebsd/contrib/tcpdump/print-telnet.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-tftp.c
freebsd/contrib/tcpdump/print-tftp.o: freebsd/contrib/tcpdump/print-tftp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-timed.c
freebsd/contrib/tcpdump/print-timed.o: freebsd/contrib/tcpdump/print-timed.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-tipc.c
freebsd/contrib/tcpdump/print-tipc.o: freebsd/contrib/tcpdump/print-tipc.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-token.c
freebsd/contrib/tcpdump/print-token.o: freebsd/contrib/tcpdump/print-token.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-udld.c
freebsd/contrib/tcpdump/print-udld.o: freebsd/contrib/tcpdump/print-udld.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-udp.c
freebsd/contrib/tcpdump/print-udp.o: freebsd/contrib/tcpdump/print-udp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-usb.c
freebsd/contrib/tcpdump/print-usb.o: freebsd/contrib/tcpdump/print-usb.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-vjc.c
freebsd/contrib/tcpdump/print-vjc.o: freebsd/contrib/tcpdump/print-vjc.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-vqp.c
freebsd/contrib/tcpdump/print-vqp.o: freebsd/contrib/tcpdump/print-vqp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-vrrp.c
freebsd/contrib/tcpdump/print-vrrp.o: freebsd/contrib/tcpdump/print-vrrp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-vtp.c
freebsd/contrib/tcpdump/print-vtp.o: freebsd/contrib/tcpdump/print-vtp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-vxlan.c
freebsd/contrib/tcpdump/print-vxlan.o: freebsd/contrib/tcpdump/print-vxlan.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-wb.c
freebsd/contrib/tcpdump/print-wb.o: freebsd/contrib/tcpdump/print-wb.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-zephyr.c
freebsd/contrib/tcpdump/print-zephyr.o: freebsd/contrib/tcpdump/print-zephyr.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/print-zeromq.c
freebsd/contrib/tcpdump/print-zeromq.o: freebsd/contrib/tcpdump/print-zeromq.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/setsignal.c
freebsd/contrib/tcpdump/setsignal.o: freebsd/contrib/tcpdump/setsignal.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/signature.c
freebsd/contrib/tcpdump/signature.o: freebsd/contrib/tcpdump/signature.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/smbutil.c
freebsd/contrib/tcpdump/smbutil.o: freebsd/contrib/tcpdump/smbutil.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/tcpdump.c
freebsd/contrib/tcpdump/tcpdump.o: freebsd/contrib/tcpdump/tcpdump.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@
LIB_C_FILES += freebsd/contrib/tcpdump/util.c
freebsd/contrib/tcpdump/util.o: freebsd/contrib/tcpdump/util.c
	$(CC) $(CPPFLAGS) $(CFLAGS) freebsd/contrib/tcpdump freebsd/usr.sbin/tcpdump/tcpdump-DINET6 -D_U_=__attribute__((unused)) -DHAVE_CONFIG_H=1 -DHAVE_NET_PFVAR_H=1 -c $< -o $@

ifeq ($(NEED_DUMMY_PIC_IRQ),yes)
CFLAGS += -I rtems-dummy-pic-irq/include
endif
LIB_O_FILES = $(LIB_C_FILES:%.c=%.o) $(LIB_CXX_FILES:%.cc=%.o)
O_FILES += $(LIB_O_FILES)
D_FILES += $(LIB_C_FILES:%.c=%.d) $(LIB_CXX_FILES:%.cc=%.d)

all: $(LIB) $(TESTS) $(TEST_NETWORK_CONFIG) $(NET_TESTS)

$(LIB): $(LIB_GEN_FILES) $(LIB_O_FILES)
	rm -f $@
	$(AR) rcu $@ $^

run_tests: $(RUN_TESTS)
	$(TEST_RUNNER) $^
	check_endof

run_net_tests: $(RUN_NET_TESTS)
	$(TEST_RUNNER) -N -T $(NET_TAP_INTERFACE) $^
	check_endof

$(TEST_NETWORK_CONFIG): $(TEST_NETWORK_CONFIG).in config.inc
	sed -e 's/@NET_CFG_SELF_IP@/$(NET_CFG_SELF_IP)/' \
	-e 's/@NET_CFG_NETMASK@/$(NET_CFG_NETMASK)/' \
	-e 's/@NET_CFG_PEER_IP@/$(NET_CFG_PEER_IP)/' \
	-e 's/@NET_CFG_GATEWAY_IP@/$(NET_CFG_GATEWAY_IP)/' \
	< $< > $@

CPU_SED  = sed
CPU_SED += -e '/arm/d'
CPU_SED += -e '/i386/d'
CPU_SED += -e '/powerpc/d'
CPU_SED += -e '/mips/d'
CPU_SED += -e '/sparc64/d'

LIB_DIR = $(INSTALL_BASE)/lib
INCLUDE_DIR = $(INSTALL_BASE)/lib/include

install: $(LIB)
	install -d $(LIB_DIR)
	install -m 644 $(LIB) $(LIB_DIR)
	cd rtemsbsd/include ; for i in `find . -type d` ; do \
	  install -d $(INCLUDE_DIR)/$$i ; \
	  install -m 644 $$i/*.h $(INCLUDE_DIR)/$$i ; done
	install -d $(INCLUDE_DIR)/mghttpd
	install -m 644 rtemsbsd/mghttpd/mongoose.h $(INCLUDE_DIR)/mghttpd
	cd freebsd/include ; for i in `find . -type d` ; do \
	  install -d $(INCLUDE_DIR)/$$i ; \
	  install -m 644 $$i/*.h $(INCLUDE_DIR)/$$i ; done
	cd freebsd/sys/contrib/altq ; \
	  install -d $(INCLUDE_DIR)/altq ; \
	  install -m 644 altq/*.h $(INCLUDE_DIR)/altq
	for i in bsm cam net net80211 netatalk netinet netinet6 netipsec sys vm ; do \
	  install -d $(INCLUDE_DIR)/$$i ; \
	  install -m 644 freebsd/sys/$$i/*.h $(INCLUDE_DIR)/$$i ; done
	cd freebsd/sys/dev/mii ; \
	  install -d $(INCLUDE_DIR)/dev/mii ; \
	  install -m 644 *.h $(INCLUDE_DIR)/dev/mii
	install -m 644 mDNSResponder/mDNSCore/mDNSDebug.h $(INCLUDE_DIR)
	install -m 644 mDNSResponder/mDNSCore/mDNSEmbeddedAPI.h $(INCLUDE_DIR)
	install -m 644 mDNSResponder/mDNSShared/dns_sd.h $(INCLUDE_DIR)
	install -m 644 mDNSResponder/mDNSPosix/mDNSPosix.h $(INCLUDE_DIR)

clean:
	rm -f $(LIB_GEN_FILES) $(LIB) $(TESTS) $(O_FILES) $(D_FILES)
	rm -f libbsd.html

-include $(D_FILES)

doc: libbsd.html

libbsd.html: libbsd.txt
	asciidoc -o libbsd.html libbsd.txt
