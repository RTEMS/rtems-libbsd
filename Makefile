include config.inc

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

CFLAGS += -ffreestanding
CFLAGS += -Irtemsbsd/include
CFLAGS += -Irtemsbsd/$(RTEMS_CPU)/include
CFLAGS += -Ifreebsd/sys
CFLAGS += -Ifreebsd/sys/$(RTEMS_CPU)/include
CFLAGS += -Ifreebsd/sys/contrib/altq
CFLAGS += -Ifreebsd/sys/contrib/pf
CFLAGS += -Icopied/rtemsbsd/$(RTEMS_CPU)/include
CFLAGS += -Ifreebsd/include
CFLAGS += -Ifreebsd/lib/libc/include
CFLAGS += -Ifreebsd/lib/libc/isc/include
CFLAGS += -Ifreebsd/lib/libc/resolv
CFLAGS += -Ifreebsd/lib/libutil
CFLAGS += -Ifreebsd/lib/libkvm
CFLAGS += -Ifreebsd/lib/libmemstat
CFLAGS += -Ifreebsd/lib/libipsec
CFLAGS += -Itestsuite/include
CFLAGS += -D__FreeBSD__
CFLAGS += -D__DBINTERFACE_PRIVATE
CFLAGS += -w
CFLAGS += -std=gnu99
CFLAGS += -MT $@ -MD -MP -MF $(basename $@).d
NEED_DUMMY_PIC_IRQ=yes

# do nothing default so sed on rtems-bsd-config.h always works.
SED_PATTERN += -e 's/^//'

TESTS =
RUN_TESTS =
O_FILES =
D_FILES =

LIB = libbsd.a
LIB_GEN_FILES = rtemsbsd/include/machine/rtems-bsd-config.h
LIB_C_FILES =
LIB_C_FILES += rtemsbsd/local/bus_if.c
LIB_C_FILES += rtemsbsd/local/cryptodev_if.c
LIB_C_FILES += rtemsbsd/local/device_if.c
LIB_C_FILES += rtemsbsd/local/miibus_if.c
LIB_C_FILES += rtemsbsd/local/pcib_if.c
LIB_C_FILES += rtemsbsd/local/pci_if.c
LIB_C_FILES += rtemsbsd/local/usb_if.c
LIB_C_FILES += rtemsbsd/rtems/ipsec_get_policylen.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-assert.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-autoconf.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-bus-dma.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-bus-dma-mbuf.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-cam.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-chunk.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-condvar.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-conf.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-copyinout.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-delay.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-get-file.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-init.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-init-with-irq.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-jail.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-kern_synch.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-lock.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-log.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-malloc.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-mutex.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-newproc.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-nexus.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-panic.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-pci_bus.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-pci_cfgreg.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-program.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-rwlock.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-shell.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-shell-netcmds.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-signal.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-support.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-sx.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-syscall-api.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-sysctlbyname.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-sysctl.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-sysctlnametomib.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-thread.c
LIB_C_FILES += rtemsbsd/rtems/rtems-bsd-timesupport.c
LIB_C_FILES += rtemsbsd/rtems/rtems-kvm.c
LIB_C_FILES += rtemsbsd/rtems/rtems-net-setup.c
LIB_C_FILES += rtemsbsd/rtems/rtems-syslog-initialize.c
LIB_C_FILES += rtemsbsd/rtems/syslog.c
LIB_C_FILES += rtemsbsd/sys/dev/usb/controller/ehci_mpc83xx.c
LIB_C_FILES += rtemsbsd/sys/dev/usb/controller/ohci_lpc24xx.c
LIB_C_FILES += rtemsbsd/sys/dev/usb/controller/ohci_lpc32xx.c
LIB_GEN_FILES += rtemsbsd/rtems/rtems-kvm-symbols.c
LIB_C_FILES += rtemsbsd/rtems/rtems-kvm-symbols.c
rtemsbsd/rtems/rtems-kvm-symbols.c: rtemsbsd/rtems/generate_kvm_symbols
	./$< > $@
LIB_GEN_FILES += freebsd/lib/libc/net/nslexer.c
LIB_C_FILES += freebsd/lib/libc/net/nslexer.c
freebsd/lib/libc/net/nslexer.c: freebsd/lib/libc/net/nslexer.l freebsd/lib/libc/net/nsparser.c
	${LEX} -P _nsyy -t $< | sed -e '/YY_BUF_SIZE/s/16384/1024/' > $@
LIB_GEN_FILES += freebsd/lib/libc/net/nsparser.c
LIB_C_FILES += freebsd/lib/libc/net/nsparser.c
freebsd/lib/libc/net/nsparser.c: freebsd/lib/libc/net/nsparser.y
	yacc -b _nsyy -d -p _nsyy $<
	sed -e /YY_BUF_SIZE/s/16384/1024/ < _nsyy.tab.c > $@
	rm -f _nsyy.tab.c
	mv _nsyy.tab.h freebsd/lib/libc/net/nsparser.h
LIB_GEN_FILES += freebsd/lib/libipsec/policy_token.c
LIB_C_FILES += freebsd/lib/libipsec/policy_token.c
freebsd/lib/libipsec/policy_token.c: freebsd/lib/libipsec/policy_token.l freebsd/lib/libipsec/policy_parse.c
	${LEX} -P __libipsecyy -t $< | sed -e '/YY_BUF_SIZE/s/16384/1024/' > $@
LIB_GEN_FILES += freebsd/lib/libipsec/policy_parse.c
LIB_C_FILES += freebsd/lib/libipsec/policy_parse.c
freebsd/lib/libipsec/policy_parse.c: freebsd/lib/libipsec/policy_parse.y
	yacc -b __libipsecyy -d -p __libipsecyy $<
	sed -e /YY_BUF_SIZE/s/16384/1024/ < __libipsecyy.tab.c > $@
	rm -f __libipsecyy.tab.c
	mv __libipsecyy.tab.h freebsd/lib/libipsec/y.tab.h
LIB_C_FILES += freebsd/sys/kern/init_main.c
LIB_C_FILES += freebsd/sys/kern/kern_environment.c
LIB_C_FILES += freebsd/sys/kern/kern_event.c
LIB_C_FILES += freebsd/sys/kern/kern_intr.c
LIB_C_FILES += freebsd/sys/kern/kern_linker.c
LIB_C_FILES += freebsd/sys/kern/kern_mbuf.c
LIB_C_FILES += freebsd/sys/kern/kern_mib.c
LIB_C_FILES += freebsd/sys/kern/kern_module.c
LIB_C_FILES += freebsd/sys/kern/kern_mtxpool.c
LIB_C_FILES += freebsd/sys/kern/kern_ntptime.c
LIB_C_FILES += freebsd/sys/kern/kern_subr.c
LIB_C_FILES += freebsd/sys/kern/kern_sysctl.c
LIB_C_FILES += freebsd/sys/kern/kern_tc.c
LIB_C_FILES += freebsd/sys/kern/kern_time.c
LIB_C_FILES += freebsd/sys/kern/kern_timeout.c
LIB_C_FILES += freebsd/sys/kern/subr_bufring.c
LIB_C_FILES += freebsd/sys/kern/subr_bus.c
LIB_C_FILES += freebsd/sys/kern/subr_eventhandler.c
LIB_C_FILES += freebsd/sys/kern/subr_hints.c
LIB_C_FILES += freebsd/sys/kern/subr_kobj.c
LIB_C_FILES += freebsd/sys/kern/subr_module.c
LIB_C_FILES += freebsd/sys/kern/subr_rman.c
LIB_C_FILES += freebsd/sys/kern/subr_sbuf.c
LIB_C_FILES += freebsd/sys/kern/subr_taskqueue.c
LIB_C_FILES += freebsd/sys/kern/subr_unit.c
LIB_C_FILES += freebsd/sys/kern/sys_generic.c
LIB_C_FILES += freebsd/sys/kern/uipc_accf.c
LIB_C_FILES += freebsd/sys/kern/uipc_domain.c
LIB_C_FILES += freebsd/sys/kern/uipc_mbuf2.c
LIB_C_FILES += freebsd/sys/kern/uipc_mbuf.c
LIB_C_FILES += freebsd/sys/kern/uipc_sockbuf.c
LIB_C_FILES += freebsd/sys/kern/uipc_socket.c
LIB_C_FILES += freebsd/sys/libkern/arc4random.c
LIB_C_FILES += freebsd/sys/libkern/fls.c
LIB_C_FILES += freebsd/sys/libkern/inet_ntoa.c
LIB_C_FILES += freebsd/sys/libkern/random.c
LIB_C_FILES += freebsd/sys/vm/uma_core.c
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
LIB_C_FILES += freebsd/sys/netinet/ip_ipsec.c
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
LIB_C_FILES += freebsd/sys/netinet/udp_usrreq.c
LIB_C_FILES += freebsd/sys/netinet/ipfw/dn_sched_fifo.c
LIB_C_FILES += freebsd/sys/netinet/ipfw/dn_sched_rr.c
LIB_C_FILES += freebsd/sys/netinet/ipfw/ip_fw_log.c
LIB_C_FILES += freebsd/sys/netinet/ipfw/dn_sched_qfq.c
LIB_C_FILES += freebsd/sys/netinet/ipfw/dn_sched_prio.c
LIB_C_FILES += freebsd/sys/netinet/ipfw/ip_dn_glue.c
LIB_C_FILES += freebsd/sys/netinet/ipfw/ip_fw2.c
LIB_C_FILES += freebsd/sys/netinet/ipfw/dn_heap.c
LIB_C_FILES += freebsd/sys/netinet/ipfw/ip_dummynet.c
LIB_C_FILES += freebsd/sys/netinet/ipfw/ip_fw_sockopt.c
LIB_C_FILES += freebsd/sys/netinet/ipfw/dn_sched_wf2q.c
LIB_C_FILES += freebsd/sys/netinet/ipfw/ip_fw_nat.c
LIB_C_FILES += freebsd/sys/netinet/ipfw/ip_fw_pfil.c
LIB_C_FILES += freebsd/sys/netinet/ipfw/ip_dn_io.c
LIB_C_FILES += freebsd/sys/netinet/ipfw/ip_fw_table.c
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
ifneq ($(DISABLE_IPV6),yes)
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
LIB_C_FILES += freebsd/sys/netinet6/ip6_ipsec.c
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
else
SED_PATTERN += -e 's/^\#define INET6 1/\/\/ \#define INET6 1/'
endif # DISABLE_IPV6
LIB_C_FILES += freebsd/sys/netipsec/ipsec.c
LIB_C_FILES += freebsd/sys/netipsec/ipsec_input.c
LIB_C_FILES += freebsd/sys/netipsec/ipsec_mbuf.c
LIB_C_FILES += freebsd/sys/netipsec/ipsec_output.c
LIB_C_FILES += freebsd/sys/netipsec/key.c
LIB_C_FILES += freebsd/sys/netipsec/key_debug.c
LIB_C_FILES += freebsd/sys/netipsec/keysock.c
LIB_C_FILES += freebsd/sys/netipsec/xform_ah.c
LIB_C_FILES += freebsd/sys/netipsec/xform_esp.c
LIB_C_FILES += freebsd/sys/netipsec/xform_ipcomp.c
LIB_C_FILES += freebsd/sys/netipsec/xform_ipip.c
LIB_C_FILES += freebsd/sys/netipsec/xform_tcp.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_acl.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_action.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_adhoc.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_ageq.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_amrr.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_crypto.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_crypto_ccmp.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_crypto_none.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_crypto_tkip.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_crypto_wep.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_ddb.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_dfs.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_freebsd.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_hostap.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_ht.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_hwmp.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_input.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_ioctl.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_mesh.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_monitor.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_node.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_output.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_phy.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_power.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_proto.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_radiotap.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_ratectl.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_ratectl_none.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_regdomain.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_rssadapt.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_scan.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_scan_sta.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_sta.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_superg.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_tdma.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_wds.c
LIB_C_FILES += freebsd/sys/net80211/ieee80211_xauth.c
LIB_C_FILES += freebsd/sys/opencrypto/crypto.c
LIB_C_FILES += freebsd/sys/opencrypto/deflate.c
LIB_C_FILES += freebsd/sys/opencrypto/cryptosoft.c
LIB_C_FILES += freebsd/sys/opencrypto/criov.c
LIB_C_FILES += freebsd/sys/opencrypto/rmd160.c
LIB_C_FILES += freebsd/sys/opencrypto/xform.c
LIB_C_FILES += freebsd/sys/opencrypto/skipjack.c
LIB_C_FILES += freebsd/sys/opencrypto/cast.c
LIB_C_FILES += freebsd/sys/opencrypto/cryptodev.c
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
LIB_C_FILES += freebsd/sys/contrib/altq/altq/altq_rmclass.c
LIB_C_FILES += freebsd/sys/contrib/altq/altq/altq_rio.c
LIB_C_FILES += freebsd/sys/contrib/altq/altq/altq_subr.c
LIB_C_FILES += freebsd/sys/contrib/altq/altq/altq_cdnr.c
LIB_C_FILES += freebsd/sys/contrib/altq/altq/altq_priq.c
LIB_C_FILES += freebsd/sys/contrib/altq/altq/altq_cbq.c
LIB_C_FILES += freebsd/sys/contrib/altq/altq/altq_hfsc.c
LIB_C_FILES += freebsd/sys/contrib/altq/altq/altq_red.c
LIB_C_FILES += freebsd/sys/contrib/pf/netinet/in4_cksum.c
LIB_C_FILES += freebsd/sys/contrib/pf/net/pf.c
LIB_C_FILES += freebsd/sys/contrib/pf/net/if_pflog.c
LIB_C_FILES += freebsd/sys/contrib/pf/net/pf_subr.c
LIB_C_FILES += freebsd/sys/contrib/pf/net/pf_ioctl.c
LIB_C_FILES += freebsd/sys/contrib/pf/net/pf_table.c
LIB_C_FILES += freebsd/sys/contrib/pf/net/pf_if.c
LIB_C_FILES += freebsd/sys/contrib/pf/net/pf_osfp.c
LIB_C_FILES += freebsd/sys/contrib/pf/net/pf_norm.c
LIB_C_FILES += freebsd/sys/contrib/pf/net/pf_ruleset.c
LIB_C_FILES += freebsd/sys/contrib/pf/net/if_pfsync.c
LIB_C_FILES += freebsd/sys/dev/mii/mii.c
LIB_C_FILES += freebsd/sys/dev/mii/mii_physubr.c
LIB_C_FILES += freebsd/sys/dev/mii/icsphy.c
LIB_C_FILES += freebsd/sys/dev/mii/brgphy.c
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
LIB_C_FILES += freebsd/sys/dev/usb/quirk/usb_quirk.c
LIB_C_FILES += freebsd/sys/dev/usb/controller/ohci.c
LIB_C_FILES += freebsd/sys/dev/usb/controller/ehci.c
LIB_C_FILES += freebsd/sys/dev/usb/controller/usb_controller.c
LIB_C_FILES += freebsd/sys/cam/cam.c
LIB_C_FILES += freebsd/sys/cam/scsi/scsi_all.c
LIB_C_FILES += freebsd/sys/dev/usb/storage/umass.c
LIB_C_FILES += freebsd/sys/dev/pci/pci.c
LIB_C_FILES += freebsd/sys/dev/pci/pci_user.c
LIB_C_FILES += freebsd/sys/dev/pci/pci_pci.c
ifeq ($(RTEMS_CPU), arm)
LIB_C_FILES += freebsd/sys/arm/pci/pci_bus.c
LIB_C_FILES += freebsd/sys/arm/arm/legacy.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), avr)
LIB_C_FILES += freebsd/sys/avr/pci/pci_bus.c
LIB_C_FILES += freebsd/sys/avr/avr/legacy.c
endif
ifeq ($(RTEMS_CPU), bfin)
LIB_C_FILES += freebsd/sys/bfin/pci/pci_bus.c
LIB_C_FILES += freebsd/sys/bfin/bfin/legacy.c
endif
ifeq ($(RTEMS_CPU), h8300)
LIB_C_FILES += freebsd/sys/h8300/pci/pci_bus.c
LIB_C_FILES += freebsd/sys/h8300/h8300/legacy.c
endif
ifeq ($(RTEMS_CPU), i386)
LIB_C_FILES += freebsd/sys/i386/pci/pci_bus.c
LIB_C_FILES += freebsd/sys/i386/i386/legacy.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), lm32)
LIB_C_FILES += freebsd/sys/lm32/pci/pci_bus.c
LIB_C_FILES += freebsd/sys/lm32/lm32/legacy.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), m32c)
LIB_C_FILES += freebsd/sys/m32c/pci/pci_bus.c
LIB_C_FILES += freebsd/sys/m32c/m32c/legacy.c
endif
ifeq ($(RTEMS_CPU), m32r)
LIB_C_FILES += freebsd/sys/m32r/pci/pci_bus.c
LIB_C_FILES += freebsd/sys/m32r/m32r/legacy.c
endif
ifeq ($(RTEMS_CPU), m68k)
LIB_C_FILES += freebsd/sys/m68k/pci/pci_bus.c
LIB_C_FILES += freebsd/sys/m68k/m68k/legacy.c
endif
ifeq ($(RTEMS_CPU), mips)
LIB_C_FILES += freebsd/sys/mips/pci/pci_bus.c
LIB_C_FILES += freebsd/sys/mips/mips/legacy.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), nios2)
LIB_C_FILES += freebsd/sys/nios2/pci/pci_bus.c
LIB_C_FILES += freebsd/sys/nios2/nios2/legacy.c
endif
ifeq ($(RTEMS_CPU), powerpc)
LIB_C_FILES += freebsd/sys/powerpc/pci/pci_bus.c
LIB_C_FILES += freebsd/sys/powerpc/powerpc/legacy.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), sh)
LIB_C_FILES += freebsd/sys/sh/pci/pci_bus.c
LIB_C_FILES += freebsd/sys/sh/sh/legacy.c
endif
ifeq ($(RTEMS_CPU), sparc)
LIB_C_FILES += freebsd/sys/sparc/pci/pci_bus.c
LIB_C_FILES += freebsd/sys/sparc/sparc/legacy.c
NEED_DUMMY_PIC_IRQ=no
endif
ifeq ($(RTEMS_CPU), sparc64)
LIB_C_FILES += freebsd/sys/sparc64/pci/pci_bus.c
LIB_C_FILES += freebsd/sys/sparc64/sparc64/legacy.c
endif
ifeq ($(RTEMS_CPU), v850)
LIB_C_FILES += freebsd/sys/v850/pci/pci_bus.c
LIB_C_FILES += freebsd/sys/v850/v850/legacy.c
endif
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
LIB_C_FILES += freebsd/sys/dev/re/if_re.c
LIB_C_FILES += freebsd/sys/dev/fxp/if_fxp.c
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
LIB_C_FILES += freebsd/sys/dev/dc/dcphy.c
LIB_C_FILES += freebsd/sys/dev/dc/if_dc.c
LIB_C_FILES += freebsd/sys/dev/dc/pnphy.c
LIB_C_FILES += freebsd/sys/dev/smc/if_smc.c
LIB_C_FILES += freebsd/sys/dev/bce/if_bce.c
LIB_C_FILES += freebsd/sys/dev/bfe/if_bfe.c
LIB_C_FILES += freebsd/sys/dev/bge/if_bge.c
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
LIB_GEN_FILES += freebsd/include/rpc/rpcb_prot.h
freebsd/include/rpc/rpcb_prot.h: freebsd/include/rpc/rpcb_prot.x
		rm -f $@
		rpcgen -h -o $@ $<
LIB_GEN_FILES += freebsd/sbin/route/keywords.h
freebsd/sbin/route/keywords.h: freebsd/sbin/route/keywords
	sed -e '/^#/d' -e '/^$$/d' $< > freebsd/sbin/route/keywords.tmp
	LC_ALL=C tr 'a-z' 'A-Z' < freebsd/sbin/route/keywords.tmp | paste freebsd/sbin/route/keywords.tmp - | \
	awk '{ if (NF > 1) printf "#define\tK_%s\t%d\n\t{\"%s\", K_%s},\n", $$2, NR, $$1, $$2 }' > $@
	rm -f freebsd/sbin/route/keywords.tmp
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_close.c
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_conv.c
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_debug.c
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_delete.c
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_get.c
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_open.c
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_overflow.c
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_page.c
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_put.c
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_search.c
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_seq.c
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_split.c
LIB_C_FILES += freebsd/lib/libc/db/btree/bt_utils.c
LIB_C_FILES += freebsd/lib/libc/db/db/db.c
LIB_C_FILES += freebsd/lib/libc/db/mpool/mpool.c
LIB_C_FILES += freebsd/lib/libc/db/mpool/mpool-compat.c
LIB_C_FILES += freebsd/lib/libc/db/recno/rec_close.c
LIB_C_FILES += freebsd/lib/libc/db/recno/rec_delete.c
LIB_C_FILES += freebsd/lib/libc/db/recno/rec_get.c
LIB_C_FILES += freebsd/lib/libc/db/recno/rec_open.c
LIB_C_FILES += freebsd/lib/libc/db/recno/rec_put.c
LIB_C_FILES += freebsd/lib/libc/db/recno/rec_search.c
LIB_C_FILES += freebsd/lib/libc/db/recno/rec_seq.c
LIB_C_FILES += freebsd/lib/libc/db/recno/rec_utils.c
LIB_C_FILES += freebsd/lib/libc/gen/err.c
LIB_C_FILES += freebsd/lib/libc/gen/gethostname.c
LIB_C_FILES += freebsd/lib/libc/inet/inet_addr.c
LIB_C_FILES += freebsd/lib/libc/inet/inet_cidr_ntop.c
LIB_C_FILES += freebsd/lib/libc/inet/inet_cidr_pton.c
LIB_C_FILES += freebsd/lib/libc/inet/inet_lnaof.c
LIB_C_FILES += freebsd/lib/libc/inet/inet_makeaddr.c
LIB_C_FILES += freebsd/lib/libc/inet/inet_neta.c
LIB_C_FILES += freebsd/lib/libc/inet/inet_net_ntop.c
LIB_C_FILES += freebsd/lib/libc/inet/inet_netof.c
LIB_C_FILES += freebsd/lib/libc/inet/inet_net_pton.c
LIB_C_FILES += freebsd/lib/libc/inet/inet_network.c
LIB_C_FILES += freebsd/lib/libc/inet/inet_ntoa.c
LIB_C_FILES += freebsd/lib/libc/inet/inet_ntop.c
LIB_C_FILES += freebsd/lib/libc/inet/inet_pton.c
LIB_C_FILES += freebsd/lib/libc/inet/nsap_addr.c
LIB_C_FILES += freebsd/lib/libc/isc/ev_streams.c
LIB_C_FILES += freebsd/lib/libc/isc/ev_timers.c
LIB_C_FILES += freebsd/lib/libc/nameser/ns_name.c
LIB_C_FILES += freebsd/lib/libc/nameser/ns_netint.c
LIB_C_FILES += freebsd/lib/libc/nameser/ns_parse.c
LIB_C_FILES += freebsd/lib/libc/nameser/ns_print.c
LIB_C_FILES += freebsd/lib/libc/nameser/ns_samedomain.c
LIB_C_FILES += freebsd/lib/libc/nameser/ns_ttl.c
LIB_C_FILES += freebsd/lib/libc/net/base64.c
LIB_C_FILES += freebsd/lib/libc/net/ether_addr.c
LIB_C_FILES += freebsd/lib/libc/net/gai_strerror.c
LIB_C_FILES += freebsd/lib/libc/net/getaddrinfo.c
LIB_C_FILES += freebsd/lib/libc/net/gethostbydns.c
LIB_C_FILES += freebsd/lib/libc/net/gethostbyht.c
LIB_C_FILES += freebsd/lib/libc/net/gethostbynis.c
LIB_C_FILES += freebsd/lib/libc/net/gethostnamadr.c
LIB_C_FILES += freebsd/lib/libc/net/getifaddrs.c
LIB_C_FILES += freebsd/lib/libc/net/getifmaddrs.c
LIB_C_FILES += freebsd/lib/libc/net/getnameinfo.c
LIB_C_FILES += freebsd/lib/libc/net/getnetbydns.c
LIB_C_FILES += freebsd/lib/libc/net/getnetbyht.c
LIB_C_FILES += freebsd/lib/libc/net/getnetbynis.c
LIB_C_FILES += freebsd/lib/libc/net/getnetnamadr.c
LIB_C_FILES += freebsd/lib/libc/net/getproto.c
LIB_C_FILES += freebsd/lib/libc/net/getprotoent.c
LIB_C_FILES += freebsd/lib/libc/net/getprotoname.c
LIB_C_FILES += freebsd/lib/libc/net/getservent.c
LIB_C_FILES += freebsd/lib/libc/net/if_indextoname.c
LIB_C_FILES += freebsd/lib/libc/net/if_nameindex.c
LIB_C_FILES += freebsd/lib/libc/net/if_nametoindex.c
LIB_C_FILES += freebsd/lib/libc/net/linkaddr.c
LIB_C_FILES += freebsd/lib/libc/net/map_v4v6.c
LIB_C_FILES += freebsd/lib/libc/net/name6.c
LIB_C_FILES += freebsd/lib/libc/net/nsdispatch.c
LIB_C_FILES += freebsd/lib/libc/net/rcmd.c
LIB_C_FILES += freebsd/lib/libc/net/recv.c
LIB_C_FILES += freebsd/lib/libc/net/send.c
LIB_C_FILES += freebsd/lib/libc/net/vars.c
LIB_C_FILES += freebsd/lib/libc/posix1e/mac.c
LIB_C_FILES += freebsd/lib/libc/resolv/h_errno.c
LIB_C_FILES += freebsd/lib/libc/resolv/herror.c
LIB_C_FILES += freebsd/lib/libc/resolv/mtctxres.c
LIB_C_FILES += freebsd/lib/libc/resolv/res_comp.c
LIB_C_FILES += freebsd/lib/libc/resolv/res_data.c
LIB_C_FILES += freebsd/lib/libc/resolv/res_debug.c
LIB_C_FILES += freebsd/lib/libc/resolv/res_findzonecut.c
LIB_C_FILES += freebsd/lib/libc/resolv/res_init.c
LIB_C_FILES += freebsd/lib/libc/resolv/res_mkquery.c
LIB_C_FILES += freebsd/lib/libc/resolv/res_mkupdate.c
LIB_C_FILES += freebsd/lib/libc/resolv/res_query.c
LIB_C_FILES += freebsd/lib/libc/resolv/res_send.c
LIB_C_FILES += freebsd/lib/libc/resolv/res_state.c
LIB_C_FILES += freebsd/lib/libc/resolv/res_update.c
LIB_C_FILES += freebsd/lib/libc/stdio/fgetln.c
LIB_C_FILES += freebsd/lib/libc/stdlib/strtonum.c
LIB_C_FILES += freebsd/lib/libc/string/strsep.c
LIB_C_FILES += freebsd/lib/libipsec/ipsec_dump_policy.c
LIB_C_FILES += freebsd/lib/libipsec/ipsec_get_policylen.c
LIB_C_FILES += freebsd/lib/libipsec/ipsec_strerror.c
LIB_C_FILES += freebsd/lib/libipsec/pfkey.c
LIB_C_FILES += freebsd/lib/libipsec/pfkey_dump.c
LIB_C_FILES += freebsd/lib/libmemstat/memstat_all.c
LIB_C_FILES += freebsd/lib/libmemstat/memstat.c
LIB_C_FILES += freebsd/lib/libmemstat/memstat_malloc.c
LIB_C_FILES += freebsd/lib/libmemstat/memstat_uma.c
LIB_C_FILES += freebsd/lib/libutil/expand_number.c
LIB_C_FILES += freebsd/lib/libutil/humanize_number.c
LIB_C_FILES += freebsd/lib/libutil/trimdomain.c
LIB_C_FILES += freebsd/sbin/dhclient/alloc.c
LIB_C_FILES += freebsd/sbin/dhclient/bpf.c
LIB_C_FILES += freebsd/sbin/dhclient/clparse.c
LIB_C_FILES += freebsd/sbin/dhclient/conflex.c
LIB_C_FILES += freebsd/sbin/dhclient/convert.c
LIB_C_FILES += freebsd/sbin/dhclient/dhclient.c
LIB_C_FILES += freebsd/sbin/dhclient/dispatch.c
LIB_C_FILES += freebsd/sbin/dhclient/errwarn.c
LIB_C_FILES += freebsd/sbin/dhclient/hash.c
LIB_C_FILES += freebsd/sbin/dhclient/inet.c
LIB_C_FILES += freebsd/sbin/dhclient/options.c
LIB_C_FILES += freebsd/sbin/dhclient/packet.c
LIB_C_FILES += freebsd/sbin/dhclient/parse.c
LIB_C_FILES += freebsd/sbin/dhclient/privsep.c
LIB_C_FILES += freebsd/sbin/dhclient/tables.c
LIB_C_FILES += freebsd/sbin/dhclient/tree.c
LIB_C_FILES += freebsd/sbin/ifconfig/af_atalk.c
LIB_C_FILES += freebsd/sbin/ifconfig/af_inet6.c
LIB_C_FILES += freebsd/sbin/ifconfig/af_inet.c
LIB_C_FILES += freebsd/sbin/ifconfig/af_link.c
LIB_C_FILES += freebsd/sbin/ifconfig/af_nd6.c
LIB_C_FILES += freebsd/sbin/ifconfig/ifbridge.c
LIB_C_FILES += freebsd/sbin/ifconfig/ifcarp.c
LIB_C_FILES += freebsd/sbin/ifconfig/ifclone.c
LIB_C_FILES += freebsd/sbin/ifconfig/ifconfig.c
LIB_C_FILES += freebsd/sbin/ifconfig/ifgif.c
LIB_C_FILES += freebsd/sbin/ifconfig/ifgre.c
LIB_C_FILES += freebsd/sbin/ifconfig/ifgroup.c
LIB_C_FILES += freebsd/sbin/ifconfig/iflagg.c
LIB_C_FILES += freebsd/sbin/ifconfig/ifmac.c
LIB_C_FILES += freebsd/sbin/ifconfig/ifmedia.c
LIB_C_FILES += freebsd/sbin/ifconfig/ifpfsync.c
LIB_C_FILES += freebsd/sbin/ifconfig/ifvlan.c
LIB_C_FILES += freebsd/sbin/ping6/ping6.c
LIB_C_FILES += freebsd/sbin/ping/ping.c
LIB_C_FILES += freebsd/sbin/route/route.c
LIB_C_FILES += freebsd/usr.bin/netstat/atalk.c
LIB_C_FILES += freebsd/usr.bin/netstat/bpf.c
LIB_C_FILES += freebsd/usr.bin/netstat/if.c
LIB_C_FILES += freebsd/usr.bin/netstat/inet6.c
LIB_C_FILES += freebsd/usr.bin/netstat/inet.c
LIB_C_FILES += freebsd/usr.bin/netstat/ipsec.c
LIB_C_FILES += freebsd/usr.bin/netstat/main.c
LIB_C_FILES += freebsd/usr.bin/netstat/mbuf.c
LIB_C_FILES += freebsd/usr.bin/netstat/mroute6.c
LIB_C_FILES += freebsd/usr.bin/netstat/mroute.c
LIB_C_FILES += freebsd/usr.bin/netstat/route.c
LIB_C_FILES += freebsd/usr.bin/netstat/pfkey.c
LIB_C_FILES += freebsd/usr.bin/netstat/sctp.c
LIB_C_FILES += freebsd/usr.bin/netstat/unix.c

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
TEST_NETSHELL01_O_FILES += testsuite/netshell01/ns_parser_vars.o
TEST_NETSHELL01_D_FILES += testsuite/netshell01/ns_parser_vars.d
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

ifeq ($(NEED_DUMMY_PIC_IRQ),yes)
CFLAGS += -I rtems-dummy-pic-irq/include
endif
LIB_O_FILES = $(LIB_C_FILES:%.c=%.o)
O_FILES += $(LIB_O_FILES)
D_FILES += $(LIB_C_FILES:%.c=%.d)

all: $(LIB) $(TESTS)

$(LIB): $(LIB_GEN_FILES) $(LIB_O_FILES)
	$(AR) rcu $@ $^
run_tests: $(RUN_TESTS)
	$(TEST_RUNNER) $^
	check_endof

# The following targets use the MIPS Generic in_cksum routine
rtemsbsd/include/machine/rtems-bsd-config.h: rtemsbsd/include/machine/rtems-bsd-config.h.in
	sed $(SED_PATTERN) <$< >$@

CPU_SED  = sed
CPU_SED += -e '/arm/d'
CPU_SED += -e '/i386/d'
CPU_SED += -e '/powerpc/d'
CPU_SED += -e '/mips/d'
CPU_SED += -e '/sparc64/d'

install: $(LIB)
	install -d $(INSTALL_BASE)/include
	install -c -m 644 $(LIB) $(INSTALL_BASE)
	cd rtemsbsd; for i in `find freebsd -name '*.h'` ; do \
	  install -c -m 644 -D "$$i" "$(INSTALL_BASE)/include/$$i" ; done
	cd contrib/altq; for i in `find freebsd -name '*.h'` ; do \
	  install -c -m 644 -D "$$i" "$(INSTALL_BASE)/include/$$i" ; done
	cd contrib/pf; for i in `find freebsd -name '*.h'` ; do \
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

clean:
	rm -f $(LIB_GEN_FILES) $(LIB) $(TESTS) $(O_FILES) $(D_FILES)
	rm -f libbsd.html

-include $(D_FILES)

doc: libbsd.html

libbsd.html: libbsd.txt
	asciidoc -o libbsd.html libbsd.txt
