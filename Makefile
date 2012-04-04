include config.inc

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

CFLAGS += -ffreestanding 
CFLAGS += -I . 
CFLAGS += -I rtemsbsd 
CFLAGS += -I freebsd/$(RTEMS_CPU)/include 
CFLAGS += -I contrib/altq 
CFLAGS += -I contrib/pf 
CFLAGS += -B $(INSTALL_BASE) 
CFLAGS += -w 
CFLAGS += -std=gnu99

C_FILES = \
	freebsd/kern/subr_eventhandler.c \
	freebsd/kern/kern_subr.c \
	freebsd/kern/kern_tc.c \
	freebsd/net/bridgestp.c \
	freebsd/net/ieee8023ad_lacp.c \
	freebsd/net/if_atmsubr.c \
	freebsd/net/if.c \
	freebsd/net/if_clone.c \
	freebsd/net/if_dead.c \
	freebsd/net/if_disc.c \
	freebsd/net/if_edsc.c \
	freebsd/net/if_ef.c \
	freebsd/net/if_enc.c \
	freebsd/net/if_epair.c \
	freebsd/net/if_faith.c \
	freebsd/net/if_fddisubr.c \
	freebsd/net/if_fwsubr.c \
	freebsd/net/if_gif.c \
	freebsd/net/if_gre.c \
	freebsd/net/if_iso88025subr.c \
	freebsd/net/if_lagg.c \
	freebsd/net/if_llatbl.c \
	freebsd/net/if_loop.c \
	freebsd/net/if_media.c \
	freebsd/net/if_mib.c \
	freebsd/net/if_spppfr.c \
	freebsd/net/if_spppsubr.c \
	freebsd/net/if_stf.c \
	freebsd/net/if_tap.c \
	freebsd/net/if_tun.c \
	freebsd/net/if_vlan.c \
	freebsd/net/pfil.c \
	freebsd/net/radix.c \
	freebsd/net/radix_mpath.c \
	freebsd/net/raw_cb.c \
	freebsd/net/raw_usrreq.c \
	freebsd/net/route.c \
	freebsd/net/rtsock.c \
	freebsd/net/slcompress.c \
	freebsd/net/zlib.c \
	freebsd/net/bpf_buffer.c \
	freebsd/net/bpf.c \
	freebsd/net/bpf_filter.c \
	freebsd/net/bpf_jitter.c \
	freebsd/net/if_arcsubr.c \
	freebsd/net/if_bridge.c \
	freebsd/net/if_ethersubr.c \
	freebsd/net/netisr.c \
	freebsd/netinet/accf_data.c \
	freebsd/netinet/accf_dns.c \
	freebsd/netinet/accf_http.c \
	freebsd/netinet/if_atm.c \
	freebsd/netinet/if_ether.c \
	freebsd/netinet/igmp.c \
	freebsd/netinet/in.c \
	freebsd/netinet/in_gif.c \
	freebsd/netinet/in_mcast.c \
	freebsd/netinet/in_pcb.c \
	freebsd/netinet/in_proto.c \
	freebsd/netinet/in_rmx.c \
	freebsd/netinet/ip_carp.c \
	freebsd/netinet/ip_divert.c \
	freebsd/netinet/ip_ecn.c \
	freebsd/netinet/ip_encap.c \
	freebsd/netinet/ip_fastfwd.c \
	freebsd/netinet/ip_gre.c \
	freebsd/netinet/ip_icmp.c \
	freebsd/netinet/ip_id.c \
	freebsd/netinet/ip_input.c \
	freebsd/netinet/ip_ipsec.c \
	freebsd/netinet/ip_mroute.c \
	freebsd/netinet/ip_options.c \
	freebsd/netinet/ip_output.c \
	freebsd/netinet/raw_ip.c \
	freebsd/netinet/sctp_asconf.c \
	freebsd/netinet/sctp_auth.c \
	freebsd/netinet/sctp_bsd_addr.c \
	freebsd/netinet/sctp_cc_functions.c \
	freebsd/netinet/sctp_crc32.c \
	freebsd/netinet/sctp_indata.c \
	freebsd/netinet/sctp_input.c \
	freebsd/netinet/sctp_output.c \
	freebsd/netinet/sctp_pcb.c \
	freebsd/netinet/sctp_peeloff.c \
	freebsd/netinet/sctp_sysctl.c \
	freebsd/netinet/sctp_timer.c \
	freebsd/netinet/sctp_usrreq.c \
	freebsd/netinet/sctputil.c \
	freebsd/netinet/tcp_debug.c \
	freebsd/netinet/tcp_input.c \
	freebsd/netinet/tcp_lro.c \
	freebsd/netinet/tcp_offload.c \
	freebsd/netinet/tcp_output.c \
	freebsd/netinet/tcp_reass.c \
	freebsd/netinet/tcp_sack.c \
	freebsd/netinet/tcp_subr.c \
	freebsd/netinet/tcp_syncache.c \
	freebsd/netinet/tcp_timer.c \
	freebsd/netinet/tcp_timewait.c \
	freebsd/netinet/tcp_usrreq.c \
	freebsd/netinet/udp_usrreq.c \
	freebsd/netinet/ipfw/dn_sched_fifo.c \
	freebsd/netinet/ipfw/dn_sched_rr.c \
	freebsd/netinet/ipfw/ip_fw_log.c \
	freebsd/netinet/ipfw/dn_sched_qfq.c \
	freebsd/netinet/ipfw/dn_sched_prio.c \
	freebsd/netinet/ipfw/ip_dn_glue.c \
	freebsd/netinet/ipfw/ip_fw2.c \
	freebsd/netinet/ipfw/dn_heap.c \
	freebsd/netinet/ipfw/ip_dummynet.c \
	freebsd/netinet/ipfw/ip_fw_sockopt.c \
	freebsd/netinet/ipfw/dn_sched_wf2q.c \
	freebsd/netinet/ipfw/ip_fw_nat.c \
	freebsd/netinet/ipfw/ip_fw_pfil.c \
	freebsd/netinet/ipfw/ip_dn_io.c \
	freebsd/netinet/ipfw/ip_fw_table.c \
	freebsd/netinet/libalias/alias_dummy.c \
	freebsd/netinet/libalias/alias_pptp.c \
	freebsd/netinet/libalias/alias_smedia.c \
	freebsd/netinet/libalias/alias_mod.c \
	freebsd/netinet/libalias/alias_cuseeme.c \
	freebsd/netinet/libalias/alias_nbt.c \
	freebsd/netinet/libalias/alias_irc.c \
	freebsd/netinet/libalias/alias_util.c \
	freebsd/netinet/libalias/alias_db.c \
	freebsd/netinet/libalias/alias_ftp.c \
	freebsd/netinet/libalias/alias_proxy.c \
	freebsd/netinet/libalias/alias.c \
	freebsd/netinet/libalias/alias_skinny.c \
	freebsd/netinet/libalias/alias_sctp.c \
	freebsd/netinet6/dest6.c \
	freebsd/netinet6/frag6.c \
	freebsd/netinet6/icmp6.c \
	freebsd/netinet6/in6.c \
	freebsd/netinet6/in6_cksum.c \
	freebsd/netinet6/in6_gif.c \
	freebsd/netinet6/in6_ifattach.c \
	freebsd/netinet6/in6_mcast.c \
	freebsd/netinet6/in6_pcb.c \
	freebsd/netinet6/in6_proto.c \
	freebsd/netinet6/in6_rmx.c \
	freebsd/netinet6/in6_src.c \
	freebsd/netinet6/ip6_forward.c \
	freebsd/netinet6/ip6_id.c \
	freebsd/netinet6/ip6_input.c \
	freebsd/netinet6/ip6_ipsec.c \
	freebsd/netinet6/ip6_mroute.c \
	freebsd/netinet6/ip6_output.c \
	freebsd/netinet6/mld6.c \
	freebsd/netinet6/nd6.c \
	freebsd/netinet6/nd6_nbr.c \
	freebsd/netinet6/nd6_rtr.c \
	freebsd/netinet6/raw_ip6.c \
	freebsd/netinet6/route6.c \
	freebsd/netinet6/scope6.c \
	freebsd/netinet6/sctp6_usrreq.c \
	freebsd/netinet6/udp6_usrreq.c \
	freebsd/netipsec/ipsec.c \
	freebsd/netipsec/ipsec_input.c \
	freebsd/netipsec/ipsec_mbuf.c \
	freebsd/netipsec/ipsec_output.c \
	freebsd/netipsec/key.c \
	freebsd/netipsec/key_debug.c \
	freebsd/netipsec/keysock.c \
	freebsd/netipsec/xform_ah.c \
	freebsd/netipsec/xform_esp.c \
	freebsd/netipsec/xform_ipcomp.c \
	freebsd/netipsec/xform_ipip.c \
	freebsd/netipsec/xform_tcp.c \
	freebsd/net80211/ieee80211_acl.c \
	freebsd/net80211/ieee80211_action.c \
	freebsd/net80211/ieee80211_adhoc.c \
	freebsd/net80211/ieee80211_ageq.c \
	freebsd/net80211/ieee80211_amrr.c \
	freebsd/net80211/ieee80211.c \
	freebsd/net80211/ieee80211_crypto.c \
	freebsd/net80211/ieee80211_crypto_ccmp.c \
	freebsd/net80211/ieee80211_crypto_none.c \
	freebsd/net80211/ieee80211_crypto_tkip.c \
	freebsd/net80211/ieee80211_crypto_wep.c \
	freebsd/net80211/ieee80211_ddb.c \
	freebsd/net80211/ieee80211_dfs.c \
	freebsd/net80211/ieee80211_freebsd.c \
	freebsd/net80211/ieee80211_hostap.c \
	freebsd/net80211/ieee80211_ht.c \
	freebsd/net80211/ieee80211_hwmp.c \
	freebsd/net80211/ieee80211_input.c \
	freebsd/net80211/ieee80211_ioctl.c \
	freebsd/net80211/ieee80211_mesh.c \
	freebsd/net80211/ieee80211_monitor.c \
	freebsd/net80211/ieee80211_node.c \
	freebsd/net80211/ieee80211_output.c \
	freebsd/net80211/ieee80211_phy.c \
	freebsd/net80211/ieee80211_power.c \
	freebsd/net80211/ieee80211_proto.c \
	freebsd/net80211/ieee80211_radiotap.c \
	freebsd/net80211/ieee80211_ratectl.c \
	freebsd/net80211/ieee80211_ratectl_none.c \
	freebsd/net80211/ieee80211_regdomain.c \
	freebsd/net80211/ieee80211_rssadapt.c \
	freebsd/net80211/ieee80211_scan.c \
	freebsd/net80211/ieee80211_scan_sta.c \
	freebsd/net80211/ieee80211_sta.c \
	freebsd/net80211/ieee80211_superg.c \
	freebsd/net80211/ieee80211_tdma.c \
	freebsd/net80211/ieee80211_wds.c \
	freebsd/net80211/ieee80211_xauth.c \
	freebsd/opencrypto/crypto.c \
	freebsd/opencrypto/deflate.c \
	freebsd/opencrypto/cryptosoft.c \
	freebsd/opencrypto/criov.c \
	freebsd/opencrypto/rmd160.c \
	freebsd/opencrypto/xform.c \
	freebsd/opencrypto/skipjack.c \
	freebsd/opencrypto/cast.c \
	freebsd/opencrypto/cryptodev.c \
	freebsd/crypto/sha1.c \
	freebsd/crypto/sha2/sha2.c \
	freebsd/crypto/rijndael/rijndael-alg-fst.c \
	freebsd/crypto/rijndael/rijndael-api.c \
	freebsd/crypto/rijndael/rijndael-api-fst.c \
	freebsd/crypto/des/des_setkey.c \
	freebsd/crypto/des/des_enc.c \
	freebsd/crypto/des/des_ecb.c \
	freebsd/crypto/blowfish/bf_enc.c \
	freebsd/crypto/blowfish/bf_skey.c \
	freebsd/crypto/blowfish/bf_ecb.c \
	freebsd/crypto/rc4/rc4.c \
	freebsd/crypto/camellia/camellia-api.c \
	freebsd/crypto/camellia/camellia.c \
	contrib/altq/freebsd/altq/altq_rmclass.c \
	contrib/altq/freebsd/altq/altq_rio.c \
	contrib/altq/freebsd/altq/altq_subr.c \
	contrib/altq/freebsd/altq/altq_cdnr.c \
	contrib/altq/freebsd/altq/altq_priq.c \
	contrib/altq/freebsd/altq/altq_cbq.c \
	contrib/altq/freebsd/altq/altq_hfsc.c \
	contrib/altq/freebsd/altq/altq_red.c \
	contrib/pf/freebsd/netinet/in4_cksum.c \
	contrib/pf/freebsd/net/pf.c \
	contrib/pf/freebsd/net/if_pflog.c \
	contrib/pf/freebsd/net/pf_subr.c \
	contrib/pf/freebsd/net/pf_ioctl.c \
	contrib/pf/freebsd/net/pf_table.c \
	contrib/pf/freebsd/net/pf_if.c \
	contrib/pf/freebsd/net/pf_osfp.c \
	contrib/pf/freebsd/net/pf_norm.c \
	contrib/pf/freebsd/net/pf_ruleset.c \
	contrib/pf/freebsd/net/if_pfsync.c \
	freebsd/dev/mii/mii.c \
	freebsd/dev/mii/mii_physubr.c \
	freebsd/dev/mii/icsphy.c \
	freebsd/dev/mii/brgphy.c \
	freebsd/local/usb_if.c \
	freebsd/local/bus_if.c \
	freebsd/local/device_if.c \
	freebsd/local/cryptodev_if.c \
	freebsd/local/miibus_if.c \
	freebsd/kern/init_main.c \
	freebsd/kern/kern_mib.c \
	freebsd/kern/kern_mbuf.c \
	freebsd/kern/kern_module.c \
	freebsd/kern/kern_sysctl.c \
	freebsd/kern/subr_bus.c \
	freebsd/kern/subr_kobj.c \
	freebsd/kern/uipc_mbuf.c \
	freebsd/kern/uipc_mbuf2.c \
	freebsd/kern/uipc_socket.c \
	freebsd/kern/uipc_sockbuf.c \
	freebsd/kern/uipc_domain.c \
	freebsd/dev/usb/usb_busdma.c \
	freebsd/dev/usb/usb_core.c \
	freebsd/dev/usb/usb_debug.c \
	freebsd/dev/usb/usb_dev.c \
	freebsd/dev/usb/usb_device.c \
	freebsd/dev/usb/usb_dynamic.c \
	freebsd/dev/usb/usb_error.c \
	freebsd/dev/usb/usb_generic.c \
	freebsd/dev/usb/usb_handle_request.c \
	freebsd/dev/usb/usb_hid.c \
	freebsd/dev/usb/usb_hub.c \
	freebsd/dev/usb/usb_lookup.c \
	freebsd/dev/usb/usb_mbuf.c \
	freebsd/dev/usb/usb_msctest.c \
	freebsd/dev/usb/usb_parse.c \
	freebsd/dev/usb/usb_process.c \
	freebsd/dev/usb/usb_request.c \
	freebsd/dev/usb/usb_transfer.c \
	freebsd/dev/usb/usb_util.c \
	freebsd/dev/usb/quirk/usb_quirk.c \
	freebsd/dev/usb/controller/ohci.c \
	freebsd/dev/usb/controller/ehci.c \
	freebsd/dev/usb/controller/usb_controller.c \
	freebsd/cam/cam.c \
	freebsd/cam/scsi/scsi_all.c \
	freebsd/dev/usb/storage/umass.c \
	freebsd/dev/random/harvest.c \
	freebsd/libkern/random.c \
	freebsd/libkern/arc4random.c \
	freebsd/kern/subr_pcpu.c \
	freebsd/libkern/inet_ntoa.c \
	freebsd/kern/kern_prot.c \
	freebsd/dev/re/if_re.c \
	freebsd/dev/fxp/if_fxp.c \
	freebsd/dev/e1000/e1000_80003es2lan.c \
	freebsd/dev/e1000/e1000_82542.c \
	freebsd/dev/e1000/e1000_82575.c \
	freebsd/dev/e1000/e1000_mac.c \
	freebsd/dev/e1000/e1000_nvm.c \
	freebsd/dev/e1000/e1000_vf.c \
	freebsd/dev/e1000/if_lem.c \
	freebsd/dev/e1000/e1000_82540.c \
	freebsd/dev/e1000/e1000_82543.c \
	freebsd/dev/e1000/e1000_api.c \
	freebsd/dev/e1000/e1000_manage.c \
	freebsd/dev/e1000/e1000_osdep.c \
	freebsd/dev/e1000/if_em.c \
	freebsd/dev/e1000/e1000_82541.c \
	freebsd/dev/e1000/e1000_82571.c \
	freebsd/dev/e1000/e1000_ich8lan.c \
	freebsd/dev/e1000/e1000_mbx.c \
	freebsd/dev/e1000/e1000_phy.c \
	freebsd/dev/e1000/if_igb.c \
	freebsd/dev/dc/dcphy.c \
	freebsd/dev/dc/if_dc.c \
	freebsd/dev/dc/pnphy.c \
	freebsd/dev/smc/if_smc.c \
	freebsd/dev/bce/if_bce.c \
	freebsd/dev/bfe/if_bfe.c \
	freebsd/dev/bge/if_bge.c
# RTEMS Project Owned Files
C_FILES += \
	rtemsbsd/dev/usb/controller/ohci_lpc24xx.c \
	rtemsbsd/dev/usb/controller/ohci_lpc32xx.c \
	rtemsbsd/dev/usb/controller/ehci_mpc83xx.c \
	rtemsbsd/src/rtems-bsd-cam.c \
	rtemsbsd/src/rtems-bsd-nexus.c \
	rtemsbsd/src/rtems-bsd-autoconf.c \
	rtemsbsd/src/rtems-bsd-delay.c \
	rtemsbsd/src/rtems-bsd-mutex.c \
	rtemsbsd/src/rtems-bsd-thread.c \
	rtemsbsd/src/rtems-bsd-condvar.c \
	rtemsbsd/src/rtems-bsd-lock.c \
	rtemsbsd/src/rtems-bsd-log.c \
	rtemsbsd/src/rtems-bsd-sx.c \
	rtemsbsd/src/rtems-bsd-rmlock.c \
	rtemsbsd/src/rtems-bsd-rwlock.c \
	rtemsbsd/src/rtems-bsd-generic.c \
	rtemsbsd/src/rtems-bsd-panic.c \
	rtemsbsd/src/rtems-bsd-synch.c \
	rtemsbsd/src/rtems-bsd-signal.c \
	rtemsbsd/src/rtems-bsd-callout.c \
	rtemsbsd/src/rtems-bsd-init.c \
	rtemsbsd/src/rtems-bsd-init-with-irq.c \
	rtemsbsd/src/rtems-bsd-assert.c \
	rtemsbsd/src/rtems-bsd-prot.c \
	rtemsbsd/src/rtems-bsd-resource.c \
	rtemsbsd/src/rtems-bsd-jail.c \
	rtemsbsd/src/rtems-bsd-shell.c \
	rtemsbsd/src/rtems-bsd-syscalls.c \
	rtemsbsd/src/rtems-bsd-smp.c \
	rtemsbsd/src/rtems-bsd-malloc.c \
	rtemsbsd/src/rtems-bsd-support.c \
	rtemsbsd/src/rtems-bsd-bus-dma.c \
	rtemsbsd/src/rtems-bsd-sysctl.c \
	rtemsbsd/src/rtems-bsd-sysctlbyname.c \
	rtemsbsd/src/rtems-bsd-sysctlnametomib.c \
	rtemsbsd/src/rtems-bsd-uma.c \
	rtemsbsd/src/rtems-bsd-taskqueue.c \
	rtemsbsd/src/rtems-bsd-newproc.c \
	rtemsbsd/src/rtems-bsd-vm_glue.c

ifeq ($(RTEMS_CPU),arm)
C_FILES += \
	freebsd/arm/arm/in_cksum.c \
	freebsd/arm/arm/in_cksum_arm.S
endif
ifeq ($(RTEMS_CPU),i386)
C_FILES += \
	freebsd/i386/i386/in_cksum.c
endif
ifeq ($(RTEMS_CPU),mips)
C_FILES += \
	freebsd/mips/mips/in_cksum.c
endif
ifeq ($(RTEMS_CPU),powerpc)
C_FILES += \
	freebsd/powerpc/powerpc/in_cksum.c
endif
ifeq ($(RTEMS_CPU),sparc64)
C_FILES += \
	freebsd/sparc64/sparc64/in_cksum.c
endif

C_O_FILES = $(C_FILES:%.c=%.o)
C_DEP_FILES = $(C_FILES:%.c=%.dep)

LIB = libbsd.a

all: lib_usb

$(LIB): $(C_O_FILES)
	$(AR) rcu $@ $^

lib_usb:
	$(MAKE) $(LIB)

install: $(LIB)
	install -d $(INSTALL_BASE)/include
	install -c -m 644 $(LIB) $(INSTALL_BASE)
	cd rtemsbsd; for i in `find . -name '*.h'` ; do \
	  install -c -m 644 -D "$$i" "$(INSTALL_BASE)/include/$$i" ; done
	for i in `find contrib freebsd -name '*.h'` ; do \
	  install -c -m 644 -D "$$i" "$(INSTALL_BASE)/include/$$i" ; done

clean:
	rm -f -r $(PROJECT_INCLUDE)/rtems/freebsd
	rm -f $(LIB) $(C_O_FILES) $(C_DEP_FILES)
	rm -f libbsd.html

-include $(C_DEP_FILES)

doc: libbsd.html

libbsd.html: libbsd.txt
	asciidoc -o libbsd.html libbsd.txt
