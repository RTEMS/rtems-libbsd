include config.inc

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

CFLAGS += -ffreestanding 
CFLAGS += -I . 
CFLAGS += -I rtemsbsd 
CFLAGS += -I rtemsbsd/$(RTEMS_CPU)/include 
CFLAGS += -I freebsd/$(RTEMS_CPU)/include 
CFLAGS += -I contrib/altq 
CFLAGS += -I contrib/pf 
CFLAGS += -I copied/rtemsbsd/$(RTEMS_CPU)/include 
CFLAGS += -w 
CFLAGS += -std=gnu99
CFLAGS += -MT $@ -MD -MP -MF $(basename $@).d
NEED_DUMMY_PIC_IRQ=yes

GENERATED_FILES =

C_FILES =
C_FILES += rtemsbsd/dev/usb/controller/ohci_lpc24xx.c
C_FILES += rtemsbsd/dev/usb/controller/ohci_lpc32xx.c
C_FILES += rtemsbsd/dev/usb/controller/ehci_mpc83xx.c
C_FILES += rtemsbsd/src/rtems-bsd-thread0-ucred.c
C_FILES += rtemsbsd/src/rtems-bsd-cam.c
C_FILES += rtemsbsd/src/rtems-bsd-nexus.c
C_FILES += rtemsbsd/src/rtems-bsd-autoconf.c
C_FILES += rtemsbsd/src/rtems-bsd-delay.c
C_FILES += rtemsbsd/src/rtems-bsd-mutex.c
C_FILES += rtemsbsd/src/rtems-bsd-thread.c
C_FILES += rtemsbsd/src/rtems-bsd-condvar.c
C_FILES += rtemsbsd/src/rtems-bsd-lock.c
C_FILES += rtemsbsd/src/rtems-bsd-log.c
C_FILES += rtemsbsd/src/rtems-bsd-sx.c
C_FILES += rtemsbsd/src/rtems-bsd-rwlock.c
C_FILES += rtemsbsd/src/rtems-bsd-generic.c
C_FILES += rtemsbsd/src/rtems-bsd-panic.c
C_FILES += rtemsbsd/src/rtems-bsd-synch.c
C_FILES += rtemsbsd/src/rtems-bsd-signal.c
C_FILES += rtemsbsd/src/rtems-bsd-init.c
C_FILES += rtemsbsd/src/rtems-bsd-init-with-irq.c
C_FILES += rtemsbsd/src/rtems-bsd-assert.c
C_FILES += rtemsbsd/src/rtems-bsd-jail.c
C_FILES += rtemsbsd/src/rtems-bsd-shell.c
C_FILES += rtemsbsd/src/rtems-bsd-syscalls.c
C_FILES += rtemsbsd/src/rtems-bsd-smp.c
C_FILES += rtemsbsd/src/rtems-bsd-malloc.c
C_FILES += rtemsbsd/src/rtems-bsd-support.c
C_FILES += rtemsbsd/src/rtems-bsd-bus-dma.c
C_FILES += rtemsbsd/src/rtems-bsd-bus-dma-mbuf.c
C_FILES += rtemsbsd/src/rtems-bsd-sysctl.c
C_FILES += rtemsbsd/src/rtems-bsd-sysctlbyname.c
C_FILES += rtemsbsd/src/rtems-bsd-sysctlnametomib.c
C_FILES += rtemsbsd/src/rtems-bsd-taskqueue.c
C_FILES += rtemsbsd/src/rtems-bsd-timesupport.c
C_FILES += rtemsbsd/src/rtems-bsd-newproc.c
C_FILES += rtemsbsd/src/rtems-bsd-vm_glue.c
C_FILES += rtemsbsd/src/rtems-bsd-copyinout.c
C_FILES += rtemsbsd/src/rtems-bsd-descrip.c
C_FILES += rtemsbsd/src/rtems-bsd-conf.c
C_FILES += rtemsbsd/src/rtems-bsd-subr_param.c
C_FILES += rtemsbsd/src/rtems-bsd-pci_cfgreg.c
C_FILES += rtemsbsd/src/rtems-bsd-pci_bus.c
C_FILES += freebsd/kern/subr_eventhandler.c
C_FILES += freebsd/kern/kern_subr.c
C_FILES += freebsd/kern/kern_tc.c
C_FILES += freebsd/libkern/fls.c
C_FILES += freebsd/net/bridgestp.c
C_FILES += freebsd/net/ieee8023ad_lacp.c
C_FILES += freebsd/net/if_atmsubr.c
C_FILES += freebsd/net/if.c
C_FILES += freebsd/net/if_clone.c
C_FILES += freebsd/net/if_dead.c
C_FILES += freebsd/net/if_disc.c
C_FILES += freebsd/net/if_edsc.c
C_FILES += freebsd/net/if_ef.c
C_FILES += freebsd/net/if_enc.c
C_FILES += freebsd/net/if_epair.c
C_FILES += freebsd/net/if_faith.c
C_FILES += freebsd/net/if_fddisubr.c
C_FILES += freebsd/net/if_fwsubr.c
C_FILES += freebsd/net/if_gif.c
C_FILES += freebsd/net/if_gre.c
C_FILES += freebsd/net/if_iso88025subr.c
C_FILES += freebsd/net/if_lagg.c
C_FILES += freebsd/net/if_llatbl.c
C_FILES += freebsd/net/if_loop.c
C_FILES += freebsd/net/if_media.c
C_FILES += freebsd/net/if_mib.c
C_FILES += freebsd/net/if_spppfr.c
C_FILES += freebsd/net/if_spppsubr.c
C_FILES += freebsd/net/if_stf.c
C_FILES += freebsd/net/if_tap.c
C_FILES += freebsd/net/if_tun.c
C_FILES += freebsd/net/if_vlan.c
C_FILES += freebsd/net/pfil.c
C_FILES += freebsd/net/radix.c
C_FILES += freebsd/net/radix_mpath.c
C_FILES += freebsd/net/raw_cb.c
C_FILES += freebsd/net/raw_usrreq.c
C_FILES += freebsd/net/route.c
C_FILES += freebsd/net/rtsock.c
C_FILES += freebsd/net/slcompress.c
C_FILES += freebsd/net/zlib.c
C_FILES += freebsd/net/bpf_buffer.c
C_FILES += freebsd/net/bpf.c
C_FILES += freebsd/net/bpf_filter.c
C_FILES += freebsd/net/bpf_jitter.c
C_FILES += freebsd/net/if_arcsubr.c
C_FILES += freebsd/net/if_bridge.c
C_FILES += freebsd/net/if_ethersubr.c
C_FILES += freebsd/net/netisr.c
C_FILES += freebsd/netinet/accf_data.c
C_FILES += freebsd/netinet/accf_dns.c
C_FILES += freebsd/netinet/accf_http.c
C_FILES += freebsd/netinet/if_atm.c
C_FILES += freebsd/netinet/if_ether.c
C_FILES += freebsd/netinet/igmp.c
C_FILES += freebsd/netinet/in.c
C_FILES += freebsd/netinet/in_gif.c
C_FILES += freebsd/netinet/in_mcast.c
C_FILES += freebsd/netinet/in_pcb.c
C_FILES += freebsd/netinet/in_proto.c
C_FILES += freebsd/netinet/in_rmx.c
C_FILES += freebsd/netinet/ip_carp.c
C_FILES += freebsd/netinet/ip_divert.c
C_FILES += freebsd/netinet/ip_ecn.c
C_FILES += freebsd/netinet/ip_encap.c
C_FILES += freebsd/netinet/ip_fastfwd.c
C_FILES += freebsd/netinet/ip_gre.c
C_FILES += freebsd/netinet/ip_icmp.c
C_FILES += freebsd/netinet/ip_id.c
C_FILES += freebsd/netinet/ip_input.c
C_FILES += freebsd/netinet/ip_ipsec.c
C_FILES += freebsd/netinet/ip_mroute.c
C_FILES += freebsd/netinet/ip_options.c
C_FILES += freebsd/netinet/ip_output.c
C_FILES += freebsd/netinet/raw_ip.c
C_FILES += freebsd/netinet/sctp_asconf.c
C_FILES += freebsd/netinet/sctp_auth.c
C_FILES += freebsd/netinet/sctp_bsd_addr.c
C_FILES += freebsd/netinet/sctp_cc_functions.c
C_FILES += freebsd/netinet/sctp_crc32.c
C_FILES += freebsd/netinet/sctp_indata.c
C_FILES += freebsd/netinet/sctp_input.c
C_FILES += freebsd/netinet/sctp_output.c
C_FILES += freebsd/netinet/sctp_pcb.c
C_FILES += freebsd/netinet/sctp_peeloff.c
C_FILES += freebsd/netinet/sctp_sysctl.c
C_FILES += freebsd/netinet/sctp_timer.c
C_FILES += freebsd/netinet/sctp_usrreq.c
C_FILES += freebsd/netinet/sctputil.c
C_FILES += freebsd/netinet/tcp_debug.c
C_FILES += freebsd/netinet/tcp_input.c
C_FILES += freebsd/netinet/tcp_lro.c
C_FILES += freebsd/netinet/tcp_offload.c
C_FILES += freebsd/netinet/tcp_output.c
C_FILES += freebsd/netinet/tcp_reass.c
C_FILES += freebsd/netinet/tcp_sack.c
C_FILES += freebsd/netinet/tcp_subr.c
C_FILES += freebsd/netinet/tcp_syncache.c
C_FILES += freebsd/netinet/tcp_timer.c
C_FILES += freebsd/netinet/tcp_timewait.c
C_FILES += freebsd/netinet/tcp_usrreq.c
C_FILES += freebsd/netinet/udp_usrreq.c
C_FILES += freebsd/netinet/ipfw/dn_sched_fifo.c
C_FILES += freebsd/netinet/ipfw/dn_sched_rr.c
C_FILES += freebsd/netinet/ipfw/ip_fw_log.c
C_FILES += freebsd/netinet/ipfw/dn_sched_qfq.c
C_FILES += freebsd/netinet/ipfw/dn_sched_prio.c
C_FILES += freebsd/netinet/ipfw/ip_dn_glue.c
C_FILES += freebsd/netinet/ipfw/ip_fw2.c
C_FILES += freebsd/netinet/ipfw/dn_heap.c
C_FILES += freebsd/netinet/ipfw/ip_dummynet.c
C_FILES += freebsd/netinet/ipfw/ip_fw_sockopt.c
C_FILES += freebsd/netinet/ipfw/dn_sched_wf2q.c
C_FILES += freebsd/netinet/ipfw/ip_fw_nat.c
C_FILES += freebsd/netinet/ipfw/ip_fw_pfil.c
C_FILES += freebsd/netinet/ipfw/ip_dn_io.c
C_FILES += freebsd/netinet/ipfw/ip_fw_table.c
C_FILES += freebsd/netinet/libalias/alias_dummy.c
C_FILES += freebsd/netinet/libalias/alias_pptp.c
C_FILES += freebsd/netinet/libalias/alias_smedia.c
C_FILES += freebsd/netinet/libalias/alias_mod.c
C_FILES += freebsd/netinet/libalias/alias_cuseeme.c
C_FILES += freebsd/netinet/libalias/alias_nbt.c
C_FILES += freebsd/netinet/libalias/alias_irc.c
C_FILES += freebsd/netinet/libalias/alias_util.c
C_FILES += freebsd/netinet/libalias/alias_db.c
C_FILES += freebsd/netinet/libalias/alias_ftp.c
C_FILES += freebsd/netinet/libalias/alias_proxy.c
C_FILES += freebsd/netinet/libalias/alias.c
C_FILES += freebsd/netinet/libalias/alias_skinny.c
C_FILES += freebsd/netinet/libalias/alias_sctp.c
C_FILES += freebsd/netinet6/dest6.c
C_FILES += freebsd/netinet6/frag6.c
C_FILES += freebsd/netinet6/icmp6.c
C_FILES += freebsd/netinet6/in6.c
C_FILES += freebsd/netinet6/in6_cksum.c
C_FILES += freebsd/netinet6/in6_gif.c
C_FILES += freebsd/netinet6/in6_ifattach.c
C_FILES += freebsd/netinet6/in6_mcast.c
C_FILES += freebsd/netinet6/in6_pcb.c
C_FILES += freebsd/netinet6/in6_proto.c
C_FILES += freebsd/netinet6/in6_rmx.c
C_FILES += freebsd/netinet6/in6_src.c
C_FILES += freebsd/netinet6/ip6_forward.c
C_FILES += freebsd/netinet6/ip6_id.c
C_FILES += freebsd/netinet6/ip6_input.c
C_FILES += freebsd/netinet6/ip6_ipsec.c
C_FILES += freebsd/netinet6/ip6_mroute.c
C_FILES += freebsd/netinet6/ip6_output.c
C_FILES += freebsd/netinet6/mld6.c
C_FILES += freebsd/netinet6/nd6.c
C_FILES += freebsd/netinet6/nd6_nbr.c
C_FILES += freebsd/netinet6/nd6_rtr.c
C_FILES += freebsd/netinet6/raw_ip6.c
C_FILES += freebsd/netinet6/route6.c
C_FILES += freebsd/netinet6/scope6.c
C_FILES += freebsd/netinet6/sctp6_usrreq.c
C_FILES += freebsd/netinet6/udp6_usrreq.c
C_FILES += freebsd/netipsec/ipsec.c
C_FILES += freebsd/netipsec/ipsec_input.c
C_FILES += freebsd/netipsec/ipsec_mbuf.c
C_FILES += freebsd/netipsec/ipsec_output.c
C_FILES += freebsd/netipsec/key.c
C_FILES += freebsd/netipsec/key_debug.c
C_FILES += freebsd/netipsec/keysock.c
C_FILES += freebsd/netipsec/xform_ah.c
C_FILES += freebsd/netipsec/xform_esp.c
C_FILES += freebsd/netipsec/xform_ipcomp.c
C_FILES += freebsd/netipsec/xform_ipip.c
C_FILES += freebsd/netipsec/xform_tcp.c
C_FILES += freebsd/net80211/ieee80211_acl.c
C_FILES += freebsd/net80211/ieee80211_action.c
C_FILES += freebsd/net80211/ieee80211_adhoc.c
C_FILES += freebsd/net80211/ieee80211_ageq.c
C_FILES += freebsd/net80211/ieee80211_amrr.c
C_FILES += freebsd/net80211/ieee80211.c
C_FILES += freebsd/net80211/ieee80211_crypto.c
C_FILES += freebsd/net80211/ieee80211_crypto_ccmp.c
C_FILES += freebsd/net80211/ieee80211_crypto_none.c
C_FILES += freebsd/net80211/ieee80211_crypto_tkip.c
C_FILES += freebsd/net80211/ieee80211_crypto_wep.c
C_FILES += freebsd/net80211/ieee80211_ddb.c
C_FILES += freebsd/net80211/ieee80211_dfs.c
C_FILES += freebsd/net80211/ieee80211_freebsd.c
C_FILES += freebsd/net80211/ieee80211_hostap.c
C_FILES += freebsd/net80211/ieee80211_ht.c
C_FILES += freebsd/net80211/ieee80211_hwmp.c
C_FILES += freebsd/net80211/ieee80211_input.c
C_FILES += freebsd/net80211/ieee80211_ioctl.c
C_FILES += freebsd/net80211/ieee80211_mesh.c
C_FILES += freebsd/net80211/ieee80211_monitor.c
C_FILES += freebsd/net80211/ieee80211_node.c
C_FILES += freebsd/net80211/ieee80211_output.c
C_FILES += freebsd/net80211/ieee80211_phy.c
C_FILES += freebsd/net80211/ieee80211_power.c
C_FILES += freebsd/net80211/ieee80211_proto.c
C_FILES += freebsd/net80211/ieee80211_radiotap.c
C_FILES += freebsd/net80211/ieee80211_ratectl.c
C_FILES += freebsd/net80211/ieee80211_ratectl_none.c
C_FILES += freebsd/net80211/ieee80211_regdomain.c
C_FILES += freebsd/net80211/ieee80211_rssadapt.c
C_FILES += freebsd/net80211/ieee80211_scan.c
C_FILES += freebsd/net80211/ieee80211_scan_sta.c
C_FILES += freebsd/net80211/ieee80211_sta.c
C_FILES += freebsd/net80211/ieee80211_superg.c
C_FILES += freebsd/net80211/ieee80211_tdma.c
C_FILES += freebsd/net80211/ieee80211_wds.c
C_FILES += freebsd/net80211/ieee80211_xauth.c
C_FILES += freebsd/opencrypto/crypto.c
C_FILES += freebsd/opencrypto/deflate.c
C_FILES += freebsd/opencrypto/cryptosoft.c
C_FILES += freebsd/opencrypto/criov.c
C_FILES += freebsd/opencrypto/rmd160.c
C_FILES += freebsd/opencrypto/xform.c
C_FILES += freebsd/opencrypto/skipjack.c
C_FILES += freebsd/opencrypto/cast.c
C_FILES += freebsd/opencrypto/cryptodev.c
C_FILES += freebsd/crypto/sha1.c
C_FILES += freebsd/crypto/sha2/sha2.c
C_FILES += freebsd/crypto/rijndael/rijndael-alg-fst.c
C_FILES += freebsd/crypto/rijndael/rijndael-api.c
C_FILES += freebsd/crypto/rijndael/rijndael-api-fst.c
C_FILES += freebsd/crypto/des/des_setkey.c
C_FILES += freebsd/crypto/des/des_enc.c
C_FILES += freebsd/crypto/des/des_ecb.c
C_FILES += freebsd/crypto/blowfish/bf_enc.c
C_FILES += freebsd/crypto/blowfish/bf_skey.c
C_FILES += freebsd/crypto/blowfish/bf_ecb.c
C_FILES += freebsd/crypto/rc4/rc4.c
C_FILES += freebsd/crypto/camellia/camellia-api.c
C_FILES += freebsd/crypto/camellia/camellia.c
C_FILES += contrib/altq/freebsd/altq/altq_rmclass.c
C_FILES += contrib/altq/freebsd/altq/altq_rio.c
C_FILES += contrib/altq/freebsd/altq/altq_subr.c
C_FILES += contrib/altq/freebsd/altq/altq_cdnr.c
C_FILES += contrib/altq/freebsd/altq/altq_priq.c
C_FILES += contrib/altq/freebsd/altq/altq_cbq.c
C_FILES += contrib/altq/freebsd/altq/altq_hfsc.c
C_FILES += contrib/altq/freebsd/altq/altq_red.c
C_FILES += contrib/pf/freebsd/netinet/in4_cksum.c
C_FILES += contrib/pf/freebsd/net/pf.c
C_FILES += contrib/pf/freebsd/net/if_pflog.c
C_FILES += contrib/pf/freebsd/net/pf_subr.c
C_FILES += contrib/pf/freebsd/net/pf_ioctl.c
C_FILES += contrib/pf/freebsd/net/pf_table.c
C_FILES += contrib/pf/freebsd/net/pf_if.c
C_FILES += contrib/pf/freebsd/net/pf_osfp.c
C_FILES += contrib/pf/freebsd/net/pf_norm.c
C_FILES += contrib/pf/freebsd/net/pf_ruleset.c
C_FILES += contrib/pf/freebsd/net/if_pfsync.c
C_FILES += freebsd/dev/mii/mii.c
C_FILES += freebsd/dev/mii/mii_physubr.c
C_FILES += freebsd/dev/mii/icsphy.c
C_FILES += freebsd/dev/mii/brgphy.c
C_FILES += freebsd/local/usb_if.c
C_FILES += freebsd/local/bus_if.c
C_FILES += freebsd/local/device_if.c
C_FILES += freebsd/local/cryptodev_if.c
C_FILES += freebsd/local/miibus_if.c
C_FILES += freebsd/local/pci_if.c
C_FILES += freebsd/local/pcib_if.c
C_FILES += freebsd/kern/init_main.c
C_FILES += freebsd/kern/kern_linker.c
C_FILES += freebsd/kern/kern_mib.c
C_FILES += freebsd/kern/kern_timeout.c
C_FILES += freebsd/kern/kern_mbuf.c
C_FILES += freebsd/kern/kern_module.c
C_FILES += freebsd/kern/kern_sysctl.c
C_FILES += freebsd/kern/subr_bus.c
C_FILES += freebsd/kern/subr_kobj.c
C_FILES += freebsd/kern/uipc_mbuf.c
C_FILES += freebsd/kern/uipc_mbuf2.c
C_FILES += freebsd/kern/uipc_socket.c
C_FILES += freebsd/kern/uipc_sockbuf.c
C_FILES += freebsd/kern/uipc_domain.c
C_FILES += freebsd/vm/uma_core.c
C_FILES += freebsd/dev/usb/usb_busdma.c
C_FILES += freebsd/dev/usb/usb_core.c
C_FILES += freebsd/dev/usb/usb_debug.c
C_FILES += freebsd/dev/usb/usb_dev.c
C_FILES += freebsd/dev/usb/usb_device.c
C_FILES += freebsd/dev/usb/usb_dynamic.c
C_FILES += freebsd/dev/usb/usb_error.c
C_FILES += freebsd/dev/usb/usb_generic.c
C_FILES += freebsd/dev/usb/usb_handle_request.c
C_FILES += freebsd/dev/usb/usb_hid.c
C_FILES += freebsd/dev/usb/usb_hub.c
C_FILES += freebsd/dev/usb/usb_lookup.c
C_FILES += freebsd/dev/usb/usb_mbuf.c
C_FILES += freebsd/dev/usb/usb_msctest.c
C_FILES += freebsd/dev/usb/usb_parse.c
C_FILES += freebsd/dev/usb/usb_process.c
C_FILES += freebsd/dev/usb/usb_request.c
C_FILES += freebsd/dev/usb/usb_transfer.c
C_FILES += freebsd/dev/usb/usb_util.c
C_FILES += freebsd/dev/usb/quirk/usb_quirk.c
C_FILES += freebsd/dev/usb/controller/ohci.c
C_FILES += freebsd/dev/usb/controller/ehci.c
C_FILES += freebsd/dev/usb/controller/usb_controller.c
C_FILES += freebsd/cam/cam.c
C_FILES += freebsd/cam/scsi/scsi_all.c
C_FILES += freebsd/dev/usb/storage/umass.c
C_FILES += freebsd/kern/subr_hints.c
C_FILES += freebsd/dev/random/harvest.c
C_FILES += freebsd/libkern/random.c
C_FILES += freebsd/libkern/arc4random.c
C_FILES += freebsd/kern/subr_pcpu.c
C_FILES += freebsd/kern/subr_sbuf.c
C_FILES += freebsd/kern/subr_rman.c
C_FILES += freebsd/kern/subr_module.c
C_FILES += freebsd/libkern/inet_ntoa.c
C_FILES += freebsd/kern/kern_prot.c
C_FILES += freebsd/kern/kern_proc.c
C_FILES += freebsd/kern/kern_time.c
C_FILES += freebsd/kern/kern_event.c
C_FILES += freebsd/netinet/tcp_hostcache.c
C_FILES += freebsd/dev/pci/pci.c
C_FILES += freebsd/dev/pci/pci_user.c
C_FILES += freebsd/kern/uipc_accf.c
C_FILES += freebsd/kern/kern_ntptime.c
C_FILES += freebsd/kern/kern_environment.c
C_FILES += freebsd/kern/kern_intr.c
C_FILES += freebsd/kern/kern_resource.c
C_FILES += freebsd/kern/subr_bufring.c
C_FILES += freebsd/dev/led/led.c
C_FILES += freebsd/kern/subr_unit.c
C_FILES += freebsd/dev/pci/pci_pci.c
ifeq ($(RTEMS_CPU), i386)
C_FILES += freebsd/i386/pci/pci_bus.c
C_FILES += freebsd/i386/i386/legacy.c
NEED_DUMMY_PIC_IRQ=no
endif
C_FILES += freebsd/dev/re/if_re.c
C_FILES += freebsd/dev/fxp/if_fxp.c
C_FILES += freebsd/dev/e1000/e1000_80003es2lan.c
C_FILES += freebsd/dev/e1000/e1000_82542.c
C_FILES += freebsd/dev/e1000/e1000_82575.c
C_FILES += freebsd/dev/e1000/e1000_mac.c
C_FILES += freebsd/dev/e1000/e1000_nvm.c
C_FILES += freebsd/dev/e1000/e1000_vf.c
C_FILES += freebsd/dev/e1000/if_lem.c
C_FILES += freebsd/dev/e1000/e1000_82540.c
C_FILES += freebsd/dev/e1000/e1000_82543.c
C_FILES += freebsd/dev/e1000/e1000_api.c
C_FILES += freebsd/dev/e1000/e1000_manage.c
C_FILES += freebsd/dev/e1000/e1000_osdep.c
C_FILES += freebsd/dev/e1000/if_em.c
C_FILES += freebsd/dev/e1000/e1000_82541.c
C_FILES += freebsd/dev/e1000/e1000_82571.c
C_FILES += freebsd/dev/e1000/e1000_ich8lan.c
C_FILES += freebsd/dev/e1000/e1000_mbx.c
C_FILES += freebsd/dev/e1000/e1000_phy.c
C_FILES += freebsd/dev/e1000/if_igb.c
C_FILES += freebsd/dev/dc/dcphy.c
C_FILES += freebsd/dev/dc/if_dc.c
C_FILES += freebsd/dev/dc/pnphy.c
C_FILES += freebsd/dev/smc/if_smc.c
C_FILES += freebsd/dev/bce/if_bce.c
C_FILES += freebsd/dev/bfe/if_bfe.c
C_FILES += freebsd/dev/bge/if_bge.c
ifeq ($(RTEMS_CPU), arm)
C_FILES += freebsd/arm/arm/in_cksum.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), i386)
C_FILES += freebsd/i386/i386/in_cksum.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), mips)
C_FILES += freebsd/mips/mips/in_cksum.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), powerpc)
C_FILES += freebsd/powerpc/powerpc/in_cksum.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), sparc)
C_FILES += freebsd/mips/mips/in_cksum.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), sparc64)
C_FILES += freebsd/sparc64/sparc64/in_cksum.c
endif
ifeq ($(RTEMS_CPU), avr)
GENERATED_FILES += copied/rtemsbsd/avr/avr/in_cksum.c
GENERATED_FILES += copied/rtemsbsd/avr/include/freebsd/machine/in_cksum.h
GENERATED_FILES += copied/rtemsbsd/avr/avr/in_cksum.c
C_FILES += copied/rtemsbsd/avr/avr/in_cksum.c
endif
ifeq ($(RTEMS_CPU), bfin)
GENERATED_FILES += copied/rtemsbsd/bfin/bfin/in_cksum.c
GENERATED_FILES += copied/rtemsbsd/bfin/include/freebsd/machine/in_cksum.h
GENERATED_FILES += copied/rtemsbsd/bfin/bfin/in_cksum.c
C_FILES += copied/rtemsbsd/bfin/bfin/in_cksum.c
endif
ifeq ($(RTEMS_CPU), h8300)
GENERATED_FILES += copied/rtemsbsd/h8300/h8300/in_cksum.c
GENERATED_FILES += copied/rtemsbsd/h8300/include/freebsd/machine/in_cksum.h
GENERATED_FILES += copied/rtemsbsd/h8300/h8300/in_cksum.c
C_FILES += copied/rtemsbsd/h8300/h8300/in_cksum.c
endif
ifeq ($(RTEMS_CPU), lm32)
GENERATED_FILES += copied/rtemsbsd/lm32/lm32/in_cksum.c
GENERATED_FILES += copied/rtemsbsd/lm32/include/freebsd/machine/in_cksum.h
GENERATED_FILES += copied/rtemsbsd/lm32/lm32/in_cksum.c
C_FILES += copied/rtemsbsd/lm32/lm32/in_cksum.c
endif
ifeq ($(RTEMS_CPU), m32c)
GENERATED_FILES += copied/rtemsbsd/m32c/m32c/in_cksum.c
GENERATED_FILES += copied/rtemsbsd/m32c/include/freebsd/machine/in_cksum.h
GENERATED_FILES += copied/rtemsbsd/m32c/m32c/in_cksum.c
C_FILES += copied/rtemsbsd/m32c/m32c/in_cksum.c
endif
ifeq ($(RTEMS_CPU), m32r)
GENERATED_FILES += copied/rtemsbsd/m32r/m32r/in_cksum.c
GENERATED_FILES += copied/rtemsbsd/m32r/include/freebsd/machine/in_cksum.h
GENERATED_FILES += copied/rtemsbsd/m32r/m32r/in_cksum.c
C_FILES += copied/rtemsbsd/m32r/m32r/in_cksum.c
endif
ifeq ($(RTEMS_CPU), m68k)
GENERATED_FILES += copied/rtemsbsd/m68k/m68k/in_cksum.c
GENERATED_FILES += copied/rtemsbsd/m68k/include/freebsd/machine/in_cksum.h
GENERATED_FILES += copied/rtemsbsd/m68k/m68k/in_cksum.c
C_FILES += copied/rtemsbsd/m68k/m68k/in_cksum.c
endif
ifeq ($(RTEMS_CPU), nios2)
GENERATED_FILES += copied/rtemsbsd/nios2/nios2/in_cksum.c
GENERATED_FILES += copied/rtemsbsd/nios2/include/freebsd/machine/in_cksum.h
GENERATED_FILES += copied/rtemsbsd/nios2/nios2/in_cksum.c
C_FILES += copied/rtemsbsd/nios2/nios2/in_cksum.c
endif
ifeq ($(RTEMS_CPU), sh)
GENERATED_FILES += copied/rtemsbsd/sh/sh/in_cksum.c
GENERATED_FILES += copied/rtemsbsd/sh/include/freebsd/machine/in_cksum.h
GENERATED_FILES += copied/rtemsbsd/sh/sh/in_cksum.c
C_FILES += copied/rtemsbsd/sh/sh/in_cksum.c
endif
ifeq ($(RTEMS_CPU), sparc)
GENERATED_FILES += copied/rtemsbsd/sparc/sparc/in_cksum.c
GENERATED_FILES += copied/rtemsbsd/sparc/include/freebsd/machine/in_cksum.h
GENERATED_FILES += copied/rtemsbsd/sparc/sparc/in_cksum.c
C_FILES += copied/rtemsbsd/sparc/sparc/in_cksum.c
endif
ifeq ($(RTEMS_CPU), v850)
GENERATED_FILES += copied/rtemsbsd/v850/v850/in_cksum.c
GENERATED_FILES += copied/rtemsbsd/v850/include/freebsd/machine/in_cksum.h
GENERATED_FILES += copied/rtemsbsd/v850/v850/in_cksum.c
C_FILES += copied/rtemsbsd/v850/v850/in_cksum.c
endif
ifeq ($(RTEMS_CPU), arm)
GENERATED_FILES += copied/rtemsbsd/arm/include/freebsd/machine/legacyvar.h
GENERATED_FILES += copied/rtemsbsd/arm/include/freebsd/machine/pci_cfgreg.h
GENERATED_FILES += copied/freebsd/arm/pci/pci_bus.c
GENERATED_FILES += copied/freebsd/arm/arm/legacy.c
C_FILES += copied/freebsd/arm/pci/pci_bus.c
C_FILES += copied/freebsd/arm/arm/legacy.c
endif
ifeq ($(RTEMS_CPU), avr)
GENERATED_FILES += copied/rtemsbsd/avr/include/freebsd/machine/legacyvar.h
GENERATED_FILES += copied/rtemsbsd/avr/include/freebsd/machine/pci_cfgreg.h
GENERATED_FILES += copied/freebsd/avr/pci/pci_bus.c
GENERATED_FILES += copied/freebsd/avr/avr/legacy.c
C_FILES += copied/freebsd/avr/pci/pci_bus.c
C_FILES += copied/freebsd/avr/avr/legacy.c
endif
ifeq ($(RTEMS_CPU), bfin)
GENERATED_FILES += copied/rtemsbsd/bfin/include/freebsd/machine/legacyvar.h
GENERATED_FILES += copied/rtemsbsd/bfin/include/freebsd/machine/pci_cfgreg.h
GENERATED_FILES += copied/freebsd/bfin/pci/pci_bus.c
GENERATED_FILES += copied/freebsd/bfin/bfin/legacy.c
C_FILES += copied/freebsd/bfin/pci/pci_bus.c
C_FILES += copied/freebsd/bfin/bfin/legacy.c
endif
ifeq ($(RTEMS_CPU), h8300)
GENERATED_FILES += copied/rtemsbsd/h8300/include/freebsd/machine/legacyvar.h
GENERATED_FILES += copied/rtemsbsd/h8300/include/freebsd/machine/pci_cfgreg.h
GENERATED_FILES += copied/freebsd/h8300/pci/pci_bus.c
GENERATED_FILES += copied/freebsd/h8300/h8300/legacy.c
C_FILES += copied/freebsd/h8300/pci/pci_bus.c
C_FILES += copied/freebsd/h8300/h8300/legacy.c
endif
ifeq ($(RTEMS_CPU), lm32)
GENERATED_FILES += copied/rtemsbsd/lm32/include/freebsd/machine/legacyvar.h
GENERATED_FILES += copied/rtemsbsd/lm32/include/freebsd/machine/pci_cfgreg.h
GENERATED_FILES += copied/freebsd/lm32/pci/pci_bus.c
GENERATED_FILES += copied/freebsd/lm32/lm32/legacy.c
C_FILES += copied/freebsd/lm32/pci/pci_bus.c
C_FILES += copied/freebsd/lm32/lm32/legacy.c
endif
ifeq ($(RTEMS_CPU), m32c)
GENERATED_FILES += copied/rtemsbsd/m32c/include/freebsd/machine/legacyvar.h
GENERATED_FILES += copied/rtemsbsd/m32c/include/freebsd/machine/pci_cfgreg.h
GENERATED_FILES += copied/freebsd/m32c/pci/pci_bus.c
GENERATED_FILES += copied/freebsd/m32c/m32c/legacy.c
C_FILES += copied/freebsd/m32c/pci/pci_bus.c
C_FILES += copied/freebsd/m32c/m32c/legacy.c
endif
ifeq ($(RTEMS_CPU), m32r)
GENERATED_FILES += copied/rtemsbsd/m32r/include/freebsd/machine/legacyvar.h
GENERATED_FILES += copied/rtemsbsd/m32r/include/freebsd/machine/pci_cfgreg.h
GENERATED_FILES += copied/freebsd/m32r/pci/pci_bus.c
GENERATED_FILES += copied/freebsd/m32r/m32r/legacy.c
C_FILES += copied/freebsd/m32r/pci/pci_bus.c
C_FILES += copied/freebsd/m32r/m32r/legacy.c
endif
ifeq ($(RTEMS_CPU), m68k)
GENERATED_FILES += copied/rtemsbsd/m68k/include/freebsd/machine/legacyvar.h
GENERATED_FILES += copied/rtemsbsd/m68k/include/freebsd/machine/pci_cfgreg.h
GENERATED_FILES += copied/freebsd/m68k/pci/pci_bus.c
GENERATED_FILES += copied/freebsd/m68k/m68k/legacy.c
C_FILES += copied/freebsd/m68k/pci/pci_bus.c
C_FILES += copied/freebsd/m68k/m68k/legacy.c
endif
ifeq ($(RTEMS_CPU), mips)
GENERATED_FILES += copied/rtemsbsd/mips/include/freebsd/machine/legacyvar.h
GENERATED_FILES += copied/rtemsbsd/mips/include/freebsd/machine/pci_cfgreg.h
GENERATED_FILES += copied/freebsd/mips/pci/pci_bus.c
GENERATED_FILES += copied/freebsd/mips/mips/legacy.c
C_FILES += copied/freebsd/mips/pci/pci_bus.c
C_FILES += copied/freebsd/mips/mips/legacy.c
endif
ifeq ($(RTEMS_CPU), nios2)
GENERATED_FILES += copied/rtemsbsd/nios2/include/freebsd/machine/legacyvar.h
GENERATED_FILES += copied/rtemsbsd/nios2/include/freebsd/machine/pci_cfgreg.h
GENERATED_FILES += copied/freebsd/nios2/pci/pci_bus.c
GENERATED_FILES += copied/freebsd/nios2/nios2/legacy.c
C_FILES += copied/freebsd/nios2/pci/pci_bus.c
C_FILES += copied/freebsd/nios2/nios2/legacy.c
endif
ifeq ($(RTEMS_CPU), powerpc)
GENERATED_FILES += copied/rtemsbsd/powerpc/include/freebsd/machine/legacyvar.h
GENERATED_FILES += copied/rtemsbsd/powerpc/include/freebsd/machine/pci_cfgreg.h
GENERATED_FILES += copied/freebsd/powerpc/pci/pci_bus.c
GENERATED_FILES += copied/freebsd/powerpc/powerpc/legacy.c
C_FILES += copied/freebsd/powerpc/pci/pci_bus.c
C_FILES += copied/freebsd/powerpc/powerpc/legacy.c
endif
ifeq ($(RTEMS_CPU), sh)
GENERATED_FILES += copied/rtemsbsd/sh/include/freebsd/machine/legacyvar.h
GENERATED_FILES += copied/rtemsbsd/sh/include/freebsd/machine/pci_cfgreg.h
GENERATED_FILES += copied/freebsd/sh/pci/pci_bus.c
GENERATED_FILES += copied/freebsd/sh/sh/legacy.c
C_FILES += copied/freebsd/sh/pci/pci_bus.c
C_FILES += copied/freebsd/sh/sh/legacy.c
endif
ifeq ($(RTEMS_CPU), sparc)
GENERATED_FILES += copied/rtemsbsd/sparc/include/freebsd/machine/legacyvar.h
GENERATED_FILES += copied/rtemsbsd/sparc/include/freebsd/machine/pci_cfgreg.h
GENERATED_FILES += copied/freebsd/sparc/pci/pci_bus.c
GENERATED_FILES += copied/freebsd/sparc/sparc/legacy.c
C_FILES += copied/freebsd/sparc/pci/pci_bus.c
C_FILES += copied/freebsd/sparc/sparc/legacy.c
endif
ifeq ($(RTEMS_CPU), sparc64)
GENERATED_FILES += copied/rtemsbsd/sparc64/include/freebsd/machine/legacyvar.h
GENERATED_FILES += copied/rtemsbsd/sparc64/include/freebsd/machine/pci_cfgreg.h
GENERATED_FILES += copied/freebsd/sparc64/pci/pci_bus.c
GENERATED_FILES += copied/freebsd/sparc64/sparc64/legacy.c
C_FILES += copied/freebsd/sparc64/pci/pci_bus.c
C_FILES += copied/freebsd/sparc64/sparc64/legacy.c
endif
ifeq ($(RTEMS_CPU), v850)
GENERATED_FILES += copied/rtemsbsd/v850/include/freebsd/machine/legacyvar.h
GENERATED_FILES += copied/rtemsbsd/v850/include/freebsd/machine/pci_cfgreg.h
GENERATED_FILES += copied/freebsd/v850/pci/pci_bus.c
GENERATED_FILES += copied/freebsd/v850/v850/legacy.c
C_FILES += copied/freebsd/v850/pci/pci_bus.c
C_FILES += copied/freebsd/v850/v850/legacy.c
endif

ifeq ($(NEED_DUMMY_PIC_IRQ),yes)
CFLAGS += -I rtems-dummy-pic-irq/include
endif
C_O_FILES = $(C_FILES:%.c=%.o)
C_D_FILES = $(C_FILES:%.c=%.d)

LIB = libbsd.a

all: $(GENERATED_FILES) $(LIB) lib_user

$(LIB): $(C_O_FILES)
	$(AR) rcu $@ $^

lib_user: $(LIB) install_bsd
	$(MAKE) -C freebsd-userspace

# The following targets use the MIPS Generic in_cksum routine
copied/rtemsbsd/avr/avr/in_cksum.c: freebsd/mips/mips/in_cksum.c
	test -d copied/rtemsbsd/avr/avr/ || mkdir -p copied/rtemsbsd/avr/avr/
	cp $< $@

copied/rtemsbsd/avr/include/freebsd/machine/in_cksum.h: freebsd/mips/include/freebsd/machine/in_cksum.h
	test -d copied/rtemsbsd/avr/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/avr/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/bfin/bfin/in_cksum.c: freebsd/mips/mips/in_cksum.c
	test -d copied/rtemsbsd/bfin/bfin/ || mkdir -p copied/rtemsbsd/bfin/bfin/
	cp $< $@

copied/rtemsbsd/bfin/include/freebsd/machine/in_cksum.h: freebsd/mips/include/freebsd/machine/in_cksum.h
	test -d copied/rtemsbsd/bfin/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/bfin/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/h8300/h8300/in_cksum.c: freebsd/mips/mips/in_cksum.c
	test -d copied/rtemsbsd/h8300/h8300/ || mkdir -p copied/rtemsbsd/h8300/h8300/
	cp $< $@

copied/rtemsbsd/h8300/include/freebsd/machine/in_cksum.h: freebsd/mips/include/freebsd/machine/in_cksum.h
	test -d copied/rtemsbsd/h8300/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/h8300/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/lm32/lm32/in_cksum.c: freebsd/mips/mips/in_cksum.c
	test -d copied/rtemsbsd/lm32/lm32/ || mkdir -p copied/rtemsbsd/lm32/lm32/
	cp $< $@

copied/rtemsbsd/lm32/include/freebsd/machine/in_cksum.h: freebsd/mips/include/freebsd/machine/in_cksum.h
	test -d copied/rtemsbsd/lm32/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/lm32/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/m32c/m32c/in_cksum.c: freebsd/mips/mips/in_cksum.c
	test -d copied/rtemsbsd/m32c/m32c/ || mkdir -p copied/rtemsbsd/m32c/m32c/
	cp $< $@

copied/rtemsbsd/m32c/include/freebsd/machine/in_cksum.h: freebsd/mips/include/freebsd/machine/in_cksum.h
	test -d copied/rtemsbsd/m32c/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/m32c/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/m32r/m32r/in_cksum.c: freebsd/mips/mips/in_cksum.c
	test -d copied/rtemsbsd/m32r/m32r/ || mkdir -p copied/rtemsbsd/m32r/m32r/
	cp $< $@

copied/rtemsbsd/m32r/include/freebsd/machine/in_cksum.h: freebsd/mips/include/freebsd/machine/in_cksum.h
	test -d copied/rtemsbsd/m32r/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/m32r/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/m68k/m68k/in_cksum.c: freebsd/mips/mips/in_cksum.c
	test -d copied/rtemsbsd/m68k/m68k/ || mkdir -p copied/rtemsbsd/m68k/m68k/
	cp $< $@

copied/rtemsbsd/m68k/include/freebsd/machine/in_cksum.h: freebsd/mips/include/freebsd/machine/in_cksum.h
	test -d copied/rtemsbsd/m68k/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/m68k/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/nios2/nios2/in_cksum.c: freebsd/mips/mips/in_cksum.c
	test -d copied/rtemsbsd/nios2/nios2/ || mkdir -p copied/rtemsbsd/nios2/nios2/
	cp $< $@

copied/rtemsbsd/nios2/include/freebsd/machine/in_cksum.h: freebsd/mips/include/freebsd/machine/in_cksum.h
	test -d copied/rtemsbsd/nios2/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/nios2/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/sh/sh/in_cksum.c: freebsd/mips/mips/in_cksum.c
	test -d copied/rtemsbsd/sh/sh/ || mkdir -p copied/rtemsbsd/sh/sh/
	cp $< $@

copied/rtemsbsd/sh/include/freebsd/machine/in_cksum.h: freebsd/mips/include/freebsd/machine/in_cksum.h
	test -d copied/rtemsbsd/sh/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/sh/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/sparc/sparc/in_cksum.c: freebsd/mips/mips/in_cksum.c
	test -d copied/rtemsbsd/sparc/sparc/ || mkdir -p copied/rtemsbsd/sparc/sparc/
	cp $< $@

copied/rtemsbsd/sparc/include/freebsd/machine/in_cksum.h: freebsd/mips/include/freebsd/machine/in_cksum.h
	test -d copied/rtemsbsd/sparc/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/sparc/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/v850/v850/in_cksum.c: freebsd/mips/mips/in_cksum.c
	test -d copied/rtemsbsd/v850/v850/ || mkdir -p copied/rtemsbsd/v850/v850/
	cp $< $@

copied/rtemsbsd/v850/include/freebsd/machine/in_cksum.h: freebsd/mips/include/freebsd/machine/in_cksum.h
	test -d copied/rtemsbsd/v850/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/v850/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/arm/include/freebsd/machine/legacyvar.h: freebsd/i386/include/freebsd/machine/legacyvar.h
	test -d copied/rtemsbsd/arm/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/arm/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/arm/include/freebsd/machine/pci_cfgreg.h: freebsd/i386/include/freebsd/machine/pci_cfgreg.h
	test -d copied/rtemsbsd/arm/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/arm/include/freebsd/machine/
	cp $< $@

copied/freebsd/arm/pci/pci_bus.c: freebsd/i386/pci/pci_bus.c
	test -d copied/freebsd/arm/pci/ || mkdir -p copied/freebsd/arm/pci/
	cp $< $@

copied/freebsd/arm/arm/legacy.c: freebsd/i386/i386/legacy.c
	test -d copied/freebsd/arm/arm/ || mkdir -p copied/freebsd/arm/arm/
	cp $< $@

copied/rtemsbsd/avr/include/freebsd/machine/legacyvar.h: freebsd/i386/include/freebsd/machine/legacyvar.h
	test -d copied/rtemsbsd/avr/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/avr/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/avr/include/freebsd/machine/pci_cfgreg.h: freebsd/i386/include/freebsd/machine/pci_cfgreg.h
	test -d copied/rtemsbsd/avr/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/avr/include/freebsd/machine/
	cp $< $@

copied/freebsd/avr/pci/pci_bus.c: freebsd/i386/pci/pci_bus.c
	test -d copied/freebsd/avr/pci/ || mkdir -p copied/freebsd/avr/pci/
	cp $< $@

copied/freebsd/avr/avr/legacy.c: freebsd/i386/i386/legacy.c
	test -d copied/freebsd/avr/avr/ || mkdir -p copied/freebsd/avr/avr/
	cp $< $@

copied/rtemsbsd/bfin/include/freebsd/machine/legacyvar.h: freebsd/i386/include/freebsd/machine/legacyvar.h
	test -d copied/rtemsbsd/bfin/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/bfin/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/bfin/include/freebsd/machine/pci_cfgreg.h: freebsd/i386/include/freebsd/machine/pci_cfgreg.h
	test -d copied/rtemsbsd/bfin/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/bfin/include/freebsd/machine/
	cp $< $@

copied/freebsd/bfin/pci/pci_bus.c: freebsd/i386/pci/pci_bus.c
	test -d copied/freebsd/bfin/pci/ || mkdir -p copied/freebsd/bfin/pci/
	cp $< $@

copied/freebsd/bfin/bfin/legacy.c: freebsd/i386/i386/legacy.c
	test -d copied/freebsd/bfin/bfin/ || mkdir -p copied/freebsd/bfin/bfin/
	cp $< $@

copied/rtemsbsd/h8300/include/freebsd/machine/legacyvar.h: freebsd/i386/include/freebsd/machine/legacyvar.h
	test -d copied/rtemsbsd/h8300/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/h8300/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/h8300/include/freebsd/machine/pci_cfgreg.h: freebsd/i386/include/freebsd/machine/pci_cfgreg.h
	test -d copied/rtemsbsd/h8300/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/h8300/include/freebsd/machine/
	cp $< $@

copied/freebsd/h8300/pci/pci_bus.c: freebsd/i386/pci/pci_bus.c
	test -d copied/freebsd/h8300/pci/ || mkdir -p copied/freebsd/h8300/pci/
	cp $< $@

copied/freebsd/h8300/h8300/legacy.c: freebsd/i386/i386/legacy.c
	test -d copied/freebsd/h8300/h8300/ || mkdir -p copied/freebsd/h8300/h8300/
	cp $< $@

copied/rtemsbsd/lm32/include/freebsd/machine/legacyvar.h: freebsd/i386/include/freebsd/machine/legacyvar.h
	test -d copied/rtemsbsd/lm32/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/lm32/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/lm32/include/freebsd/machine/pci_cfgreg.h: freebsd/i386/include/freebsd/machine/pci_cfgreg.h
	test -d copied/rtemsbsd/lm32/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/lm32/include/freebsd/machine/
	cp $< $@

copied/freebsd/lm32/pci/pci_bus.c: freebsd/i386/pci/pci_bus.c
	test -d copied/freebsd/lm32/pci/ || mkdir -p copied/freebsd/lm32/pci/
	cp $< $@

copied/freebsd/lm32/lm32/legacy.c: freebsd/i386/i386/legacy.c
	test -d copied/freebsd/lm32/lm32/ || mkdir -p copied/freebsd/lm32/lm32/
	cp $< $@

copied/rtemsbsd/m32c/include/freebsd/machine/legacyvar.h: freebsd/i386/include/freebsd/machine/legacyvar.h
	test -d copied/rtemsbsd/m32c/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/m32c/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/m32c/include/freebsd/machine/pci_cfgreg.h: freebsd/i386/include/freebsd/machine/pci_cfgreg.h
	test -d copied/rtemsbsd/m32c/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/m32c/include/freebsd/machine/
	cp $< $@

copied/freebsd/m32c/pci/pci_bus.c: freebsd/i386/pci/pci_bus.c
	test -d copied/freebsd/m32c/pci/ || mkdir -p copied/freebsd/m32c/pci/
	cp $< $@

copied/freebsd/m32c/m32c/legacy.c: freebsd/i386/i386/legacy.c
	test -d copied/freebsd/m32c/m32c/ || mkdir -p copied/freebsd/m32c/m32c/
	cp $< $@

copied/rtemsbsd/m32r/include/freebsd/machine/legacyvar.h: freebsd/i386/include/freebsd/machine/legacyvar.h
	test -d copied/rtemsbsd/m32r/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/m32r/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/m32r/include/freebsd/machine/pci_cfgreg.h: freebsd/i386/include/freebsd/machine/pci_cfgreg.h
	test -d copied/rtemsbsd/m32r/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/m32r/include/freebsd/machine/
	cp $< $@

copied/freebsd/m32r/pci/pci_bus.c: freebsd/i386/pci/pci_bus.c
	test -d copied/freebsd/m32r/pci/ || mkdir -p copied/freebsd/m32r/pci/
	cp $< $@

copied/freebsd/m32r/m32r/legacy.c: freebsd/i386/i386/legacy.c
	test -d copied/freebsd/m32r/m32r/ || mkdir -p copied/freebsd/m32r/m32r/
	cp $< $@

copied/rtemsbsd/m68k/include/freebsd/machine/legacyvar.h: freebsd/i386/include/freebsd/machine/legacyvar.h
	test -d copied/rtemsbsd/m68k/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/m68k/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/m68k/include/freebsd/machine/pci_cfgreg.h: freebsd/i386/include/freebsd/machine/pci_cfgreg.h
	test -d copied/rtemsbsd/m68k/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/m68k/include/freebsd/machine/
	cp $< $@

copied/freebsd/m68k/pci/pci_bus.c: freebsd/i386/pci/pci_bus.c
	test -d copied/freebsd/m68k/pci/ || mkdir -p copied/freebsd/m68k/pci/
	cp $< $@

copied/freebsd/m68k/m68k/legacy.c: freebsd/i386/i386/legacy.c
	test -d copied/freebsd/m68k/m68k/ || mkdir -p copied/freebsd/m68k/m68k/
	cp $< $@

copied/rtemsbsd/mips/include/freebsd/machine/legacyvar.h: freebsd/i386/include/freebsd/machine/legacyvar.h
	test -d copied/rtemsbsd/mips/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/mips/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/mips/include/freebsd/machine/pci_cfgreg.h: freebsd/i386/include/freebsd/machine/pci_cfgreg.h
	test -d copied/rtemsbsd/mips/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/mips/include/freebsd/machine/
	cp $< $@

copied/freebsd/mips/pci/pci_bus.c: freebsd/i386/pci/pci_bus.c
	test -d copied/freebsd/mips/pci/ || mkdir -p copied/freebsd/mips/pci/
	cp $< $@

copied/freebsd/mips/mips/legacy.c: freebsd/i386/i386/legacy.c
	test -d copied/freebsd/mips/mips/ || mkdir -p copied/freebsd/mips/mips/
	cp $< $@

copied/rtemsbsd/nios2/include/freebsd/machine/legacyvar.h: freebsd/i386/include/freebsd/machine/legacyvar.h
	test -d copied/rtemsbsd/nios2/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/nios2/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/nios2/include/freebsd/machine/pci_cfgreg.h: freebsd/i386/include/freebsd/machine/pci_cfgreg.h
	test -d copied/rtemsbsd/nios2/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/nios2/include/freebsd/machine/
	cp $< $@

copied/freebsd/nios2/pci/pci_bus.c: freebsd/i386/pci/pci_bus.c
	test -d copied/freebsd/nios2/pci/ || mkdir -p copied/freebsd/nios2/pci/
	cp $< $@

copied/freebsd/nios2/nios2/legacy.c: freebsd/i386/i386/legacy.c
	test -d copied/freebsd/nios2/nios2/ || mkdir -p copied/freebsd/nios2/nios2/
	cp $< $@

copied/rtemsbsd/powerpc/include/freebsd/machine/legacyvar.h: freebsd/i386/include/freebsd/machine/legacyvar.h
	test -d copied/rtemsbsd/powerpc/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/powerpc/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/powerpc/include/freebsd/machine/pci_cfgreg.h: freebsd/i386/include/freebsd/machine/pci_cfgreg.h
	test -d copied/rtemsbsd/powerpc/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/powerpc/include/freebsd/machine/
	cp $< $@

copied/freebsd/powerpc/pci/pci_bus.c: freebsd/i386/pci/pci_bus.c
	test -d copied/freebsd/powerpc/pci/ || mkdir -p copied/freebsd/powerpc/pci/
	cp $< $@

copied/freebsd/powerpc/powerpc/legacy.c: freebsd/i386/i386/legacy.c
	test -d copied/freebsd/powerpc/powerpc/ || mkdir -p copied/freebsd/powerpc/powerpc/
	cp $< $@

copied/rtemsbsd/sh/include/freebsd/machine/legacyvar.h: freebsd/i386/include/freebsd/machine/legacyvar.h
	test -d copied/rtemsbsd/sh/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/sh/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/sh/include/freebsd/machine/pci_cfgreg.h: freebsd/i386/include/freebsd/machine/pci_cfgreg.h
	test -d copied/rtemsbsd/sh/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/sh/include/freebsd/machine/
	cp $< $@

copied/freebsd/sh/pci/pci_bus.c: freebsd/i386/pci/pci_bus.c
	test -d copied/freebsd/sh/pci/ || mkdir -p copied/freebsd/sh/pci/
	cp $< $@

copied/freebsd/sh/sh/legacy.c: freebsd/i386/i386/legacy.c
	test -d copied/freebsd/sh/sh/ || mkdir -p copied/freebsd/sh/sh/
	cp $< $@

copied/rtemsbsd/sparc/include/freebsd/machine/legacyvar.h: freebsd/i386/include/freebsd/machine/legacyvar.h
	test -d copied/rtemsbsd/sparc/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/sparc/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/sparc/include/freebsd/machine/pci_cfgreg.h: freebsd/i386/include/freebsd/machine/pci_cfgreg.h
	test -d copied/rtemsbsd/sparc/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/sparc/include/freebsd/machine/
	cp $< $@

copied/freebsd/sparc/pci/pci_bus.c: freebsd/i386/pci/pci_bus.c
	test -d copied/freebsd/sparc/pci/ || mkdir -p copied/freebsd/sparc/pci/
	cp $< $@

copied/freebsd/sparc/sparc/legacy.c: freebsd/i386/i386/legacy.c
	test -d copied/freebsd/sparc/sparc/ || mkdir -p copied/freebsd/sparc/sparc/
	cp $< $@

copied/rtemsbsd/sparc64/include/freebsd/machine/legacyvar.h: freebsd/i386/include/freebsd/machine/legacyvar.h
	test -d copied/rtemsbsd/sparc64/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/sparc64/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/sparc64/include/freebsd/machine/pci_cfgreg.h: freebsd/i386/include/freebsd/machine/pci_cfgreg.h
	test -d copied/rtemsbsd/sparc64/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/sparc64/include/freebsd/machine/
	cp $< $@

copied/freebsd/sparc64/pci/pci_bus.c: freebsd/i386/pci/pci_bus.c
	test -d copied/freebsd/sparc64/pci/ || mkdir -p copied/freebsd/sparc64/pci/
	cp $< $@

copied/freebsd/sparc64/sparc64/legacy.c: freebsd/i386/i386/legacy.c
	test -d copied/freebsd/sparc64/sparc64/ || mkdir -p copied/freebsd/sparc64/sparc64/
	cp $< $@

copied/rtemsbsd/v850/include/freebsd/machine/legacyvar.h: freebsd/i386/include/freebsd/machine/legacyvar.h
	test -d copied/rtemsbsd/v850/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/v850/include/freebsd/machine/
	cp $< $@

copied/rtemsbsd/v850/include/freebsd/machine/pci_cfgreg.h: freebsd/i386/include/freebsd/machine/pci_cfgreg.h
	test -d copied/rtemsbsd/v850/include/freebsd/machine/ || mkdir -p copied/rtemsbsd/v850/include/freebsd/machine/
	cp $< $@

copied/freebsd/v850/pci/pci_bus.c: freebsd/i386/pci/pci_bus.c
	test -d copied/freebsd/v850/pci/ || mkdir -p copied/freebsd/v850/pci/
	cp $< $@

copied/freebsd/v850/v850/legacy.c: freebsd/i386/i386/legacy.c
	test -d copied/freebsd/v850/v850/ || mkdir -p copied/freebsd/v850/v850/
	cp $< $@

CPU_SED  = sed
CPU_SED += -e '/arm/d'
CPU_SED += -e '/i386/d'
CPU_SED += -e '/powerpc/d'
CPU_SED += -e '/mips/d'
CPU_SED += -e '/sparc64/d'

install: $(LIB) install_bsd lib_user install_user

install_bsd: $(LIB)
	install -d $(INSTALL_BASE)/include
	install -c -m 644 $(LIB) $(INSTALL_BASE)
	cd rtemsbsd; for i in `find freebsd -name '*.h'` ; do \
	  install -c -m 644 -D "$$i" "$(INSTALL_BASE)/include/$$i" ; done
	for i in `find freebsd -name '*.h' | $(CPU_SED)` ; do \
	  install -c -m 644 -D "$$i" "$(INSTALL_BASE)/include/$$i" ; done
	-cd freebsd/$(RTEMS_CPU)/include && for i in `find . -name '*.h'` ; do \
	  install -c -m 644 -D "$$i" "$(INSTALL_BASE)/include/$$i" ; done
	-cd rtemsbsd/$(RTEMS_CPU)/include && \
	  for i in `find . -name '*.h' | $(CPU_SED)` ; do \
	    install -c -m 644 -D "$$i" "$(INSTALL_BASE)/include/$$i" ; done
	-cd copied/rtemsbsd/$(RTEMS_CPU)/include && for i in `find . -name '*.h'` ; do \
	  install -c -m 644 -D "$$i" "$(INSTALL_BASE)/include/$$i" ; done

install_user:
	$(MAKE) -C freebsd-userspace install

clean:
	rm -f -r $(PROJECT_INCLUDE)/rtems/freebsd
	rm -f $(LIB) $(C_O_FILES) $(C_D_FILES) $(GENERATED_FILES)
	rm -f libbsd.html
	rm -rf copied
	$(MAKE) -C freebsd-userspace clean

-include $(C_D_FILES)

doc: libbsd.html

libbsd.html: libbsd.txt
	asciidoc -o libbsd.html libbsd.txt
