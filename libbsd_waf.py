#
# RTEMS Project (https://www.rtems.org)
#
# Generated waf script. Do not edit, run ./freebsd-to-rtems.py -m
#
# To use see README.waf shipped with this file.
#

from __future__ import print_function

import os
import os.path
import rtems_waf.rtems as rtems

windows = os.name == "nt"

if windows:
    host_shell = "sh -c "
else:
    host_shell = ""

def init(ctx):
    pass

def options(opt):
    pass

def bsp_configure(conf, arch_bsp):
    conf.check(header_name = "rtems/rtems-debugger.h", features = "c", includes = conf.env.IFLAGS, mandatory = False)

def configure(conf):
    rtems.configure(conf, bsp_configure)

def build(bld):
    # C/C++ flags
    common_flags = []
    common_flags += ["-O2"]
    common_flags += ["-g"]
    common_flags += ["-fno-strict-aliasing"]
    common_flags += ["-ffreestanding"]
    common_flags += ["-fno-common"]
    if bld.env.WARNINGS:
        common_flags += ["-Wall"]
        common_flags += ["-Wno-format"]
        common_flags += ["-Wno-pointer-sign"]
    else:
        common_flags += ["-w"]
    cflags = ['-std=gnu11'] + common_flags
    cxxflags = ['-std=gnu++11'] + common_flags

    # Defines
    defines = []
    if len(bld.env.FREEBSD_OPTIONS) > 0:
        for o in bld.env.FREEBSD_OPTIONS.split(","):
            defines += ["%s=1" % (o.strip().upper())]

    # Include paths
    includes = []
    for i in ['-Irtemsbsd/@CPU@/include', '-Ifreebsd/sys/@CPU@/include']:
        includes += ["%s" % (i[2:].replace("@CPU@", bld.get_env()["RTEMS_ARCH"]))]
    if bld.get_env()["RTEMS_ARCH"] == "i386":
        for i in ['-Irtemsbsd/@CPU@/include', '-Ifreebsd/sys/@CPU@/include']:
            includes += ["%s" % (i[2:].replace("@CPU@", "x86"))]
    includes += ["rtemsbsd/include"]
    includes += ["freebsd/sys"]
    includes += ["freebsd/sys/contrib/pf"]
    includes += ["freebsd/sys/net"]
    includes += ["freebsd/include"]
    includes += ["freebsd/lib/libc/include"]
    includes += ["freebsd/lib/libc/isc/include"]
    includes += ["freebsd/lib/libc/resolv"]
    includes += ["freebsd/lib/libutil"]
    includes += ["freebsd/lib/libkvm"]
    includes += ["freebsd/lib/libmemstat"]
    includes += ["freebsd/lib/libipsec"]
    includes += ["freebsd/contrib/libpcap"]
    includes += ["freebsd/contrib/libxo"]
    includes += ["rtemsbsd/sys"]
    includes += ["mDNSResponder/mDNSCore"]
    includes += ["mDNSResponder/mDNSShared"]
    includes += ["mDNSResponder/mDNSPosix"]
    includes += ["testsuite/include"]

    # Collect the libbsd uses
    libbsd_use = []

    # Network test configuration
    if not os.path.exists(bld.env.NET_CONFIG):
        bld.fatal("network configuraiton '%s' not found" % (bld.env.NET_CONFIG))
    net_cfg_self_ip = None
    net_cfg_netmask = None
    net_cfg_peer_ip = None
    net_cfg_gateway_ip = None
    net_tap_interface = None
    try:
        net_cfg_lines = open(bld.env.NET_CONFIG).readlines()
    except:
        bld.fatal("network configuraiton '%s' read failed" % (bld.env.NET_CONFIG))
    lc = 0
    for l in net_cfg_lines:
        lc += 1
        if l.strip().startswith("NET_CFG_"):
            ls = l.split("=")
            if len(ls) != 2:
                bld.fatal("network configuraiton '%s' parse error: %d: %s" % (bld.env.NET_CONFIG, lc, l))
            lhs = ls[0].strip()
            rhs = ls[1].strip()
            if lhs == "NET_CFG_SELF_IP":
                net_cfg_self_ip = rhs
            if lhs == "NET_CFG_NETMASK":
                net_cfg_netmask = rhs
            if lhs == "NET_CFG_PEER_IP":
                net_cfg_peer_ip = rhs
            if lhs == "NET_CFG_GATEWAY_IP":
                net_cfg_gateway_ip = rhs
            if lhs == "NET_TAP_INTERFACE":
                net_tap_interface = rhs
    bld(target = "testsuite/include/rtems/bsd/test/network-config.h",
        source = "testsuite/include/rtems/bsd/test/network-config.h.in",
        rule = "sed -e 's/@NET_CFG_SELF_IP@/%s/' -e 's/@NET_CFG_NETMASK@/%s/' -e 's/@NET_CFG_PEER_IP@/%s/' -e 's/@NET_CFG_GATEWAY_IP@/%s/' < ${SRC} > ${TGT}" % (net_cfg_self_ip, net_cfg_netmask, net_cfg_peer_ip, net_cfg_gateway_ip),
        update_outputs = True)

    # KVM Symbols
    bld(target = "rtemsbsd/rtems/rtems-kernel-kvm-symbols.c",
        source = "rtemsbsd/rtems/generate_kvm_symbols",
        rule = host_shell + "./${SRC} > ${TGT}",
        update_outputs = True)
    bld.objects(target = "kvmsymbols",
                features = "c",
                cflags = cflags,
                includes = ['rtemsbsd/rtems'] + includes,
                source = "rtemsbsd/rtems/rtems-kernel-kvm-symbols.c")
    libbsd_use += ["kvmsymbols"]

    bld.add_group()
    # RPC Generation
    if bld.env.AUTO_REGEN:
        bld(target = "freebsd/include/rpc/rpcb_prot.h",
            source = "freebsd/include/rpc/rpcb_prot.x",
            rule = host_shell + "${RPCGEN} -h -o ${TGT} ${SRC}")

    # Route keywords
    if bld.env.AUTO_REGEN:
        rkw_rule = host_shell + "cat ${SRC} | awk 'BEGIN { r = 0 } { if (NF == 1) printf \"#define\\tK_%%s\\t%%d\\n\\t{\\\"%%s\\\", K_%%s},\\n\", toupper($1), ++r, $1, toupper($1)}' > ${TGT}"
        bld(target = "freebsd/sbin/route/keywords.h",
            source = "freebsd/sbin/route/keywords",
            rule = rkw_rule)

    # Lex
    if bld.env.AUTO_REGEN:
        bld(target = "freebsd/contrib/libpcap/scanner.c",
            source = "freebsd/contrib/libpcap/scanner.l",
            rule = host_shell + "${LEX} -P pcap -t ${SRC} | sed -e '/YY_BUF_SIZE/s/16384/1024/' > ${TGT}")
    bld.objects(target = "lex_pcap",
                features = "c",
                cflags = cflags,
                includes = [] + includes,
                defines = defines + ['__FreeBSD__=1', 'BSD=1', 'INET6', '_U_=__attribute__((unused))', 'HAVE_LIMITS_H=1', 'HAVE_INTTYPES=1', 'HAVE_STDINT=1', 'HAVE_STRERROR=1', 'HAVE_STRLCPY=1', 'HAVE_SNPRINTF=1', 'HAVE_VSNPRINTF=1', 'HAVE_SOCKADDR_SA_LEN=1', 'HAVE_NET_IF_MEDIA_H=1', 'HAVE_SYS_IOCCOM_H=1', 'NEED_YYPARSE_WRAPPER=1', 'yylval=pcap_lval'],
                source = "freebsd/contrib/libpcap/scanner.c")
    libbsd_use += ["lex_pcap"]

    if bld.env.AUTO_REGEN:
        bld(target = "freebsd/lib/libc/net/nslexer.c",
            source = "freebsd/lib/libc/net/nslexer.l",
            rule = host_shell + "${LEX} -P _nsyy -t ${SRC} | sed -e '/YY_BUF_SIZE/s/16384/1024/' > ${TGT}")
    bld.objects(target = "lex__nsyy",
                features = "c",
                cflags = cflags,
                includes = [] + includes,
                defines = defines + [],
                source = "freebsd/lib/libc/net/nslexer.c")
    libbsd_use += ["lex__nsyy"]

    if bld.env.AUTO_REGEN:
        bld(target = "freebsd/lib/libipsec/policy_token.c",
            source = "freebsd/lib/libipsec/policy_token.l",
            rule = host_shell + "${LEX} -P __libipsecyy -t ${SRC} | sed -e '/YY_BUF_SIZE/s/16384/1024/' > ${TGT}")
    bld.objects(target = "lex___libipsecyy",
                features = "c",
                cflags = cflags,
                includes = [] + includes,
                defines = defines + [],
                source = "freebsd/lib/libipsec/policy_token.c")
    libbsd_use += ["lex___libipsecyy"]

    # Yacc
    if bld.env.AUTO_REGEN:
        bld(target = "freebsd/contrib/libpcap/grammar.c",
            source = "freebsd/contrib/libpcap/grammar.y",
            rule = host_shell + "${YACC} -b pcap -d -p pcap ${SRC} && sed -e '/YY_BUF_SIZE/s/16384/1024/' < pcap.tab.c > ${TGT} && rm -f pcap.tab.c && mv pcap.tab.h freebsd/contrib/libpcap/tokdefs.h")
    bld.objects(target = "yacc_pcap",
                features = "c",
                cflags = cflags,
                includes = [] + includes,
                defines = defines + ['__FreeBSD__=1', 'BSD=1', 'INET6', '_U_=__attribute__((unused))', 'HAVE_LIMITS_H=1', 'HAVE_INTTYPES=1', 'HAVE_STDINT=1', 'HAVE_STRERROR=1', 'HAVE_STRLCPY=1', 'HAVE_SNPRINTF=1', 'HAVE_VSNPRINTF=1', 'HAVE_SOCKADDR_SA_LEN=1', 'HAVE_NET_IF_MEDIA_H=1', 'HAVE_SYS_IOCCOM_H=1', 'NEED_YYPARSE_WRAPPER=1', 'yylval=pcap_lval'],
                source = "freebsd/contrib/libpcap/grammar.c")
    libbsd_use += ["yacc_pcap"]
    if bld.env.AUTO_REGEN:
        bld(target = "freebsd/lib/libc/net/nsparser.c",
            source = "freebsd/lib/libc/net/nsparser.y",
            rule = host_shell + "${YACC} -b _nsyy -d -p _nsyy ${SRC} && sed -e '/YY_BUF_SIZE/s/16384/1024/' < _nsyy.tab.c > ${TGT} && rm -f _nsyy.tab.c && mv _nsyy.tab.h freebsd/lib/libc/net/nsparser.h")
    bld.objects(target = "yacc__nsyy",
                features = "c",
                cflags = cflags,
                includes = [] + includes,
                defines = defines + [],
                source = "freebsd/lib/libc/net/nsparser.c")
    libbsd_use += ["yacc__nsyy"]
    if bld.env.AUTO_REGEN:
        bld(target = "freebsd/lib/libipsec/policy_parse.c",
            source = "freebsd/lib/libipsec/policy_parse.y",
            rule = host_shell + "${YACC} -b __libipsecyy -d -p __libipsecyy ${SRC} && sed -e '/YY_BUF_SIZE/s/16384/1024/' < __libipsecyy.tab.c > ${TGT} && rm -f __libipsecyy.tab.c && mv __libipsecyy.tab.h freebsd/lib/libipsec/y.tab.h")
    bld.objects(target = "yacc___libipsecyy",
                features = "c",
                cflags = cflags,
                includes = [] + includes,
                defines = defines + [],
                source = "freebsd/lib/libipsec/policy_parse.c")
    libbsd_use += ["yacc___libipsecyy"]
    if bld.env.AUTO_REGEN:
        bld(target = "freebsd/sbin/pfctl/parse.c",
            source = "freebsd/sbin/pfctl/parse.y",
            rule = host_shell + "${YACC} -b pfctly -d -p pfctly ${SRC} && sed -e '/YY_BUF_SIZE/s/16384/1024/' < pfctly.tab.c > ${TGT} && rm -f pfctly.tab.c && mv pfctly.tab.h freebsd/sbin/pfctl/parse.h")
    bld.objects(target = "yacc_pfctly",
                features = "c",
                cflags = cflags,
                includes = [] + includes,
                defines = defines + [],
                source = "freebsd/sbin/pfctl/parse.c")
    libbsd_use += ["yacc_pfctly"]

    # Objects built with different CFLAGS
    objs01_source = ['freebsd/bin/hostname/hostname.c',
                     'freebsd/contrib/libxo/libxo/libxo.c',
                     'freebsd/contrib/libxo/libxo/xo_encoder.c',
                     'freebsd/lib/libc/gen/err.c',
                     'freebsd/lib/libc/gen/feature_present.c',
                     'freebsd/lib/libc/gen/getdomainname.c',
                     'freebsd/lib/libc/gen/gethostname.c',
                     'freebsd/lib/libc/gen/sethostname.c',
                     'freebsd/lib/libc/inet/inet_addr.c',
                     'freebsd/lib/libc/inet/inet_cidr_ntop.c',
                     'freebsd/lib/libc/inet/inet_cidr_pton.c',
                     'freebsd/lib/libc/inet/inet_lnaof.c',
                     'freebsd/lib/libc/inet/inet_makeaddr.c',
                     'freebsd/lib/libc/inet/inet_net_ntop.c',
                     'freebsd/lib/libc/inet/inet_net_pton.c',
                     'freebsd/lib/libc/inet/inet_neta.c',
                     'freebsd/lib/libc/inet/inet_netof.c',
                     'freebsd/lib/libc/inet/inet_network.c',
                     'freebsd/lib/libc/inet/inet_ntoa.c',
                     'freebsd/lib/libc/inet/inet_ntop.c',
                     'freebsd/lib/libc/inet/inet_pton.c',
                     'freebsd/lib/libc/inet/nsap_addr.c',
                     'freebsd/lib/libc/isc/ev_streams.c',
                     'freebsd/lib/libc/isc/ev_timers.c',
                     'freebsd/lib/libc/nameser/ns_name.c',
                     'freebsd/lib/libc/nameser/ns_netint.c',
                     'freebsd/lib/libc/nameser/ns_parse.c',
                     'freebsd/lib/libc/nameser/ns_print.c',
                     'freebsd/lib/libc/nameser/ns_samedomain.c',
                     'freebsd/lib/libc/nameser/ns_ttl.c',
                     'freebsd/lib/libc/net/base64.c',
                     'freebsd/lib/libc/net/ether_addr.c',
                     'freebsd/lib/libc/net/gai_strerror.c',
                     'freebsd/lib/libc/net/getaddrinfo.c',
                     'freebsd/lib/libc/net/gethostbydns.c',
                     'freebsd/lib/libc/net/gethostbyht.c',
                     'freebsd/lib/libc/net/gethostbynis.c',
                     'freebsd/lib/libc/net/gethostnamadr.c',
                     'freebsd/lib/libc/net/getifaddrs.c',
                     'freebsd/lib/libc/net/getifmaddrs.c',
                     'freebsd/lib/libc/net/getnameinfo.c',
                     'freebsd/lib/libc/net/getnetbydns.c',
                     'freebsd/lib/libc/net/getnetbyht.c',
                     'freebsd/lib/libc/net/getnetbynis.c',
                     'freebsd/lib/libc/net/getnetnamadr.c',
                     'freebsd/lib/libc/net/getproto.c',
                     'freebsd/lib/libc/net/getprotoent.c',
                     'freebsd/lib/libc/net/getprotoname.c',
                     'freebsd/lib/libc/net/getservent.c',
                     'freebsd/lib/libc/net/if_indextoname.c',
                     'freebsd/lib/libc/net/if_nameindex.c',
                     'freebsd/lib/libc/net/if_nametoindex.c',
                     'freebsd/lib/libc/net/ip6opt.c',
                     'freebsd/lib/libc/net/linkaddr.c',
                     'freebsd/lib/libc/net/map_v4v6.c',
                     'freebsd/lib/libc/net/name6.c',
                     'freebsd/lib/libc/net/nsdispatch.c',
                     'freebsd/lib/libc/net/rcmd.c',
                     'freebsd/lib/libc/net/recv.c',
                     'freebsd/lib/libc/net/rthdr.c',
                     'freebsd/lib/libc/net/send.c',
                     'freebsd/lib/libc/net/vars.c',
                     'freebsd/lib/libc/posix1e/mac.c',
                     'freebsd/lib/libc/resolv/h_errno.c',
                     'freebsd/lib/libc/resolv/herror.c',
                     'freebsd/lib/libc/resolv/mtctxres.c',
                     'freebsd/lib/libc/resolv/res_comp.c',
                     'freebsd/lib/libc/resolv/res_data.c',
                     'freebsd/lib/libc/resolv/res_debug.c',
                     'freebsd/lib/libc/resolv/res_findzonecut.c',
                     'freebsd/lib/libc/resolv/res_init.c',
                     'freebsd/lib/libc/resolv/res_mkquery.c',
                     'freebsd/lib/libc/resolv/res_mkupdate.c',
                     'freebsd/lib/libc/resolv/res_query.c',
                     'freebsd/lib/libc/resolv/res_send.c',
                     'freebsd/lib/libc/resolv/res_state.c',
                     'freebsd/lib/libc/resolv/res_update.c',
                     'freebsd/lib/libc/rpc/auth_des.c',
                     'freebsd/lib/libc/rpc/auth_none.c',
                     'freebsd/lib/libc/rpc/auth_time.c',
                     'freebsd/lib/libc/rpc/auth_unix.c',
                     'freebsd/lib/libc/rpc/authdes_prot.c',
                     'freebsd/lib/libc/rpc/authunix_prot.c',
                     'freebsd/lib/libc/rpc/bindresvport.c',
                     'freebsd/lib/libc/rpc/clnt_bcast.c',
                     'freebsd/lib/libc/rpc/clnt_dg.c',
                     'freebsd/lib/libc/rpc/clnt_generic.c',
                     'freebsd/lib/libc/rpc/clnt_perror.c',
                     'freebsd/lib/libc/rpc/clnt_raw.c',
                     'freebsd/lib/libc/rpc/clnt_simple.c',
                     'freebsd/lib/libc/rpc/clnt_vc.c',
                     'freebsd/lib/libc/rpc/crypt_client.c',
                     'freebsd/lib/libc/rpc/des_crypt.c',
                     'freebsd/lib/libc/rpc/des_soft.c',
                     'freebsd/lib/libc/rpc/getnetconfig.c',
                     'freebsd/lib/libc/rpc/getnetpath.c',
                     'freebsd/lib/libc/rpc/getpublickey.c',
                     'freebsd/lib/libc/rpc/getrpcent.c',
                     'freebsd/lib/libc/rpc/getrpcport.c',
                     'freebsd/lib/libc/rpc/key_call.c',
                     'freebsd/lib/libc/rpc/key_prot_xdr.c',
                     'freebsd/lib/libc/rpc/mt_misc.c',
                     'freebsd/lib/libc/rpc/netname.c',
                     'freebsd/lib/libc/rpc/netnamer.c',
                     'freebsd/lib/libc/rpc/pmap_clnt.c',
                     'freebsd/lib/libc/rpc/pmap_getmaps.c',
                     'freebsd/lib/libc/rpc/pmap_getport.c',
                     'freebsd/lib/libc/rpc/pmap_prot.c',
                     'freebsd/lib/libc/rpc/pmap_prot2.c',
                     'freebsd/lib/libc/rpc/pmap_rmt.c',
                     'freebsd/lib/libc/rpc/rpc_callmsg.c',
                     'freebsd/lib/libc/rpc/rpc_commondata.c',
                     'freebsd/lib/libc/rpc/rpc_dtablesize.c',
                     'freebsd/lib/libc/rpc/rpc_generic.c',
                     'freebsd/lib/libc/rpc/rpc_prot.c',
                     'freebsd/lib/libc/rpc/rpc_soc.c',
                     'freebsd/lib/libc/rpc/rpcb_clnt.c',
                     'freebsd/lib/libc/rpc/rpcb_prot.c',
                     'freebsd/lib/libc/rpc/rpcb_st_xdr.c',
                     'freebsd/lib/libc/rpc/rpcdname.c',
                     'freebsd/lib/libc/rpc/rpcsec_gss_stub.c',
                     'freebsd/lib/libc/rpc/rtime.c',
                     'freebsd/lib/libc/rpc/svc.c',
                     'freebsd/lib/libc/rpc/svc_auth.c',
                     'freebsd/lib/libc/rpc/svc_auth_des.c',
                     'freebsd/lib/libc/rpc/svc_auth_unix.c',
                     'freebsd/lib/libc/rpc/svc_dg.c',
                     'freebsd/lib/libc/rpc/svc_generic.c',
                     'freebsd/lib/libc/rpc/svc_raw.c',
                     'freebsd/lib/libc/rpc/svc_run.c',
                     'freebsd/lib/libc/rpc/svc_simple.c',
                     'freebsd/lib/libc/rpc/svc_vc.c',
                     'freebsd/lib/libc/stdio/fgetln.c',
                     'freebsd/lib/libc/stdlib/strtoimax.c',
                     'freebsd/lib/libc/stdlib/strtonum.c',
                     'freebsd/lib/libc/stdlib/strtoumax.c',
                     'freebsd/lib/libc/string/strsep.c',
                     'freebsd/lib/libc/xdr/xdr.c',
                     'freebsd/lib/libc/xdr/xdr_array.c',
                     'freebsd/lib/libc/xdr/xdr_float.c',
                     'freebsd/lib/libc/xdr/xdr_mem.c',
                     'freebsd/lib/libc/xdr/xdr_rec.c',
                     'freebsd/lib/libc/xdr/xdr_reference.c',
                     'freebsd/lib/libc/xdr/xdr_sizeof.c',
                     'freebsd/lib/libc/xdr/xdr_stdio.c',
                     'freebsd/lib/libipsec/ipsec_dump_policy.c',
                     'freebsd/lib/libipsec/ipsec_get_policylen.c',
                     'freebsd/lib/libipsec/ipsec_strerror.c',
                     'freebsd/lib/libipsec/pfkey.c',
                     'freebsd/lib/libipsec/pfkey_dump.c',
                     'freebsd/lib/libmemstat/memstat.c',
                     'freebsd/lib/libmemstat/memstat_all.c',
                     'freebsd/lib/libmemstat/memstat_malloc.c',
                     'freebsd/lib/libmemstat/memstat_uma.c',
                     'freebsd/lib/libutil/expand_number.c',
                     'freebsd/lib/libutil/humanize_number.c',
                     'freebsd/lib/libutil/trimdomain.c',
                     'freebsd/sbin/dhclient/alloc.c',
                     'freebsd/sbin/dhclient/bpf.c',
                     'freebsd/sbin/dhclient/clparse.c',
                     'freebsd/sbin/dhclient/conflex.c',
                     'freebsd/sbin/dhclient/convert.c',
                     'freebsd/sbin/dhclient/dhclient.c',
                     'freebsd/sbin/dhclient/dispatch.c',
                     'freebsd/sbin/dhclient/errwarn.c',
                     'freebsd/sbin/dhclient/hash.c',
                     'freebsd/sbin/dhclient/inet.c',
                     'freebsd/sbin/dhclient/options.c',
                     'freebsd/sbin/dhclient/packet.c',
                     'freebsd/sbin/dhclient/parse.c',
                     'freebsd/sbin/dhclient/privsep.c',
                     'freebsd/sbin/dhclient/tables.c',
                     'freebsd/sbin/dhclient/tree.c',
                     'freebsd/sbin/ifconfig/af_inet.c',
                     'freebsd/sbin/ifconfig/af_inet6.c',
                     'freebsd/sbin/ifconfig/af_link.c',
                     'freebsd/sbin/ifconfig/af_nd6.c',
                     'freebsd/sbin/ifconfig/ifbridge.c',
                     'freebsd/sbin/ifconfig/ifclone.c',
                     'freebsd/sbin/ifconfig/ifconfig.c',
                     'freebsd/sbin/ifconfig/ifgif.c',
                     'freebsd/sbin/ifconfig/ifgre.c',
                     'freebsd/sbin/ifconfig/ifgroup.c',
                     'freebsd/sbin/ifconfig/iflagg.c',
                     'freebsd/sbin/ifconfig/ifmac.c',
                     'freebsd/sbin/ifconfig/ifmedia.c',
                     'freebsd/sbin/ifconfig/ifpfsync.c',
                     'freebsd/sbin/ifconfig/ifvlan.c',
                     'freebsd/sbin/ifconfig/sfp.c',
                     'freebsd/sbin/pfctl/pf_print_state.c',
                     'freebsd/sbin/pfctl/pfctl.c',
                     'freebsd/sbin/pfctl/pfctl_altq.c',
                     'freebsd/sbin/pfctl/pfctl_optimize.c',
                     'freebsd/sbin/pfctl/pfctl_osfp.c',
                     'freebsd/sbin/pfctl/pfctl_parser.c',
                     'freebsd/sbin/pfctl/pfctl_qstats.c',
                     'freebsd/sbin/pfctl/pfctl_radix.c',
                     'freebsd/sbin/pfctl/pfctl_table.c',
                     'freebsd/sbin/ping/ping.c',
                     'freebsd/sbin/ping6/ping6.c',
                     'freebsd/sbin/route/route.c',
                     'freebsd/sbin/sysctl/sysctl.c',
                     'freebsd/usr.bin/netstat/bpf.c',
                     'freebsd/usr.bin/netstat/flowtable.c',
                     'freebsd/usr.bin/netstat/if.c',
                     'freebsd/usr.bin/netstat/inet.c',
                     'freebsd/usr.bin/netstat/inet6.c',
                     'freebsd/usr.bin/netstat/ipsec.c',
                     'freebsd/usr.bin/netstat/main.c',
                     'freebsd/usr.bin/netstat/mbuf.c',
                     'freebsd/usr.bin/netstat/mroute.c',
                     'freebsd/usr.bin/netstat/mroute6.c',
                     'freebsd/usr.bin/netstat/nl_symbols.c',
                     'freebsd/usr.bin/netstat/pfkey.c',
                     'freebsd/usr.bin/netstat/route.c',
                     'freebsd/usr.bin/netstat/sctp.c',
                     'freebsd/usr.bin/netstat/unix.c']
    bld.objects(target = "objs01",
                features = "c",
                cflags = cflags,
                includes = [] + includes,
                defines = defines + ['INET', 'INET6'],
                source = objs01_source)
    libbsd_use += ["objs01"]

    objs02_source = ['rtemsbsd/mghttpd/mongoose.c']
    bld.objects(target = "objs02",
                features = "c",
                cflags = cflags,
                includes = [] + includes,
                defines = defines + ['NO_CGI', 'NO_POPEN', 'NO_SSL', 'USE_WEBSOCKET'],
                source = objs02_source)
    libbsd_use += ["objs02"]

    objs03_source = ['freebsd/lib/libc/db/btree/bt_close.c',
                     'freebsd/lib/libc/db/btree/bt_conv.c',
                     'freebsd/lib/libc/db/btree/bt_debug.c',
                     'freebsd/lib/libc/db/btree/bt_delete.c',
                     'freebsd/lib/libc/db/btree/bt_get.c',
                     'freebsd/lib/libc/db/btree/bt_open.c',
                     'freebsd/lib/libc/db/btree/bt_overflow.c',
                     'freebsd/lib/libc/db/btree/bt_page.c',
                     'freebsd/lib/libc/db/btree/bt_put.c',
                     'freebsd/lib/libc/db/btree/bt_search.c',
                     'freebsd/lib/libc/db/btree/bt_seq.c',
                     'freebsd/lib/libc/db/btree/bt_split.c',
                     'freebsd/lib/libc/db/btree/bt_utils.c',
                     'freebsd/lib/libc/db/db/db.c',
                     'freebsd/lib/libc/db/mpool/mpool-compat.c',
                     'freebsd/lib/libc/db/mpool/mpool.c',
                     'freebsd/lib/libc/db/recno/rec_close.c',
                     'freebsd/lib/libc/db/recno/rec_delete.c',
                     'freebsd/lib/libc/db/recno/rec_get.c',
                     'freebsd/lib/libc/db/recno/rec_open.c',
                     'freebsd/lib/libc/db/recno/rec_put.c',
                     'freebsd/lib/libc/db/recno/rec_search.c',
                     'freebsd/lib/libc/db/recno/rec_seq.c',
                     'freebsd/lib/libc/db/recno/rec_utils.c']
    bld.objects(target = "objs03",
                features = "c",
                cflags = cflags,
                includes = [] + includes,
                defines = defines + ['INET6', '__DBINTERFACE_PRIVATE'],
                source = objs03_source)
    libbsd_use += ["objs03"]

    objs04_source = ['dhcpcd/arp.c',
                     'dhcpcd/auth.c',
                     'dhcpcd/bpf.c',
                     'dhcpcd/common.c',
                     'dhcpcd/compat/pselect.c',
                     'dhcpcd/crypt/hmac_md5.c',
                     'dhcpcd/dhcp-common.c',
                     'dhcpcd/dhcp.c',
                     'dhcpcd/dhcp6.c',
                     'dhcpcd/dhcpcd-embedded.c',
                     'dhcpcd/dhcpcd.c',
                     'dhcpcd/duid.c',
                     'dhcpcd/eloop.c',
                     'dhcpcd/if-bsd.c',
                     'dhcpcd/if-options.c',
                     'dhcpcd/if-pref.c',
                     'dhcpcd/ipv4.c',
                     'dhcpcd/ipv4ll.c',
                     'dhcpcd/ipv6.c',
                     'dhcpcd/ipv6nd.c',
                     'dhcpcd/net.c',
                     'dhcpcd/platform-bsd.c']
    bld.objects(target = "objs04",
                features = "c",
                cflags = cflags,
                includes = [] + includes,
                defines = defines + ['INET', 'INET6', 'MASTER_ONLY', 'THERE_IS_NO_FORK', '__FreeBSD__'],
                source = objs04_source)
    libbsd_use += ["objs04"]

    objs05_source = ['freebsd/contrib/libpcap/bpf_image.c',
                     'freebsd/contrib/libpcap/etherent.c',
                     'freebsd/contrib/libpcap/fad-getad.c',
                     'freebsd/contrib/libpcap/gencode.c',
                     'freebsd/contrib/libpcap/inet.c',
                     'freebsd/contrib/libpcap/nametoaddr.c',
                     'freebsd/contrib/libpcap/optimize.c',
                     'freebsd/contrib/libpcap/pcap-bpf.c',
                     'freebsd/contrib/libpcap/pcap-common.c',
                     'freebsd/contrib/libpcap/pcap.c',
                     'freebsd/contrib/libpcap/savefile.c',
                     'freebsd/contrib/libpcap/sf-pcap-ng.c',
                     'freebsd/contrib/libpcap/sf-pcap.c']
    bld.objects(target = "objs05",
                features = "c",
                cflags = cflags,
                includes = [] + includes,
                defines = defines + ['BSD=1', 'HAVE_INTTYPES=1', 'HAVE_LIMITS_H=1', 'HAVE_NET_IF_MEDIA_H=1', 'HAVE_SNPRINTF=1', 'HAVE_SOCKADDR_SA_LEN=1', 'HAVE_STDINT=1', 'HAVE_STRERROR=1', 'HAVE_STRLCPY=1', 'HAVE_SYS_IOCCOM_H=1', 'HAVE_VSNPRINTF=1', 'INET6', '_U_=__attribute__((unused))', '__FreeBSD__=1'],
                source = objs05_source)
    libbsd_use += ["objs05"]

    objs06_source = ['freebsd/contrib/tcpdump/addrtoname.c',
                     'freebsd/contrib/tcpdump/af.c',
                     'freebsd/contrib/tcpdump/bpf_dump.c',
                     'freebsd/contrib/tcpdump/checksum.c',
                     'freebsd/contrib/tcpdump/cpack.c',
                     'freebsd/contrib/tcpdump/gmpls.c',
                     'freebsd/contrib/tcpdump/gmt2local.c',
                     'freebsd/contrib/tcpdump/in_cksum.c',
                     'freebsd/contrib/tcpdump/ipproto.c',
                     'freebsd/contrib/tcpdump/l2vpn.c',
                     'freebsd/contrib/tcpdump/machdep.c',
                     'freebsd/contrib/tcpdump/nlpid.c',
                     'freebsd/contrib/tcpdump/oui.c',
                     'freebsd/contrib/tcpdump/parsenfsfh.c',
                     'freebsd/contrib/tcpdump/print-802_11.c',
                     'freebsd/contrib/tcpdump/print-802_15_4.c',
                     'freebsd/contrib/tcpdump/print-ah.c',
                     'freebsd/contrib/tcpdump/print-aodv.c',
                     'freebsd/contrib/tcpdump/print-ap1394.c',
                     'freebsd/contrib/tcpdump/print-arcnet.c',
                     'freebsd/contrib/tcpdump/print-arp.c',
                     'freebsd/contrib/tcpdump/print-ascii.c',
                     'freebsd/contrib/tcpdump/print-atalk.c',
                     'freebsd/contrib/tcpdump/print-atm.c',
                     'freebsd/contrib/tcpdump/print-babel.c',
                     'freebsd/contrib/tcpdump/print-beep.c',
                     'freebsd/contrib/tcpdump/print-bfd.c',
                     'freebsd/contrib/tcpdump/print-bgp.c',
                     'freebsd/contrib/tcpdump/print-bootp.c',
                     'freebsd/contrib/tcpdump/print-bt.c',
                     'freebsd/contrib/tcpdump/print-carp.c',
                     'freebsd/contrib/tcpdump/print-cdp.c',
                     'freebsd/contrib/tcpdump/print-cfm.c',
                     'freebsd/contrib/tcpdump/print-chdlc.c',
                     'freebsd/contrib/tcpdump/print-cip.c',
                     'freebsd/contrib/tcpdump/print-cnfp.c',
                     'freebsd/contrib/tcpdump/print-dccp.c',
                     'freebsd/contrib/tcpdump/print-decnet.c',
                     'freebsd/contrib/tcpdump/print-dhcp6.c',
                     'freebsd/contrib/tcpdump/print-domain.c',
                     'freebsd/contrib/tcpdump/print-dtp.c',
                     'freebsd/contrib/tcpdump/print-dvmrp.c',
                     'freebsd/contrib/tcpdump/print-eap.c',
                     'freebsd/contrib/tcpdump/print-egp.c',
                     'freebsd/contrib/tcpdump/print-eigrp.c',
                     'freebsd/contrib/tcpdump/print-enc.c',
                     'freebsd/contrib/tcpdump/print-esp.c',
                     'freebsd/contrib/tcpdump/print-ether.c',
                     'freebsd/contrib/tcpdump/print-fddi.c',
                     'freebsd/contrib/tcpdump/print-forces.c',
                     'freebsd/contrib/tcpdump/print-fr.c',
                     'freebsd/contrib/tcpdump/print-frag6.c',
                     'freebsd/contrib/tcpdump/print-gre.c',
                     'freebsd/contrib/tcpdump/print-hsrp.c',
                     'freebsd/contrib/tcpdump/print-icmp.c',
                     'freebsd/contrib/tcpdump/print-icmp6.c',
                     'freebsd/contrib/tcpdump/print-igmp.c',
                     'freebsd/contrib/tcpdump/print-igrp.c',
                     'freebsd/contrib/tcpdump/print-ip.c',
                     'freebsd/contrib/tcpdump/print-ip6.c',
                     'freebsd/contrib/tcpdump/print-ip6opts.c',
                     'freebsd/contrib/tcpdump/print-ipcomp.c',
                     'freebsd/contrib/tcpdump/print-ipfc.c',
                     'freebsd/contrib/tcpdump/print-ipnet.c',
                     'freebsd/contrib/tcpdump/print-ipx.c',
                     'freebsd/contrib/tcpdump/print-isakmp.c',
                     'freebsd/contrib/tcpdump/print-isoclns.c',
                     'freebsd/contrib/tcpdump/print-juniper.c',
                     'freebsd/contrib/tcpdump/print-krb.c',
                     'freebsd/contrib/tcpdump/print-l2tp.c',
                     'freebsd/contrib/tcpdump/print-lane.c',
                     'freebsd/contrib/tcpdump/print-ldp.c',
                     'freebsd/contrib/tcpdump/print-llc.c',
                     'freebsd/contrib/tcpdump/print-lldp.c',
                     'freebsd/contrib/tcpdump/print-lmp.c',
                     'freebsd/contrib/tcpdump/print-lspping.c',
                     'freebsd/contrib/tcpdump/print-lwapp.c',
                     'freebsd/contrib/tcpdump/print-lwres.c',
                     'freebsd/contrib/tcpdump/print-mobile.c',
                     'freebsd/contrib/tcpdump/print-mobility.c',
                     'freebsd/contrib/tcpdump/print-mpcp.c',
                     'freebsd/contrib/tcpdump/print-mpls.c',
                     'freebsd/contrib/tcpdump/print-msdp.c',
                     'freebsd/contrib/tcpdump/print-msnlb.c',
                     'freebsd/contrib/tcpdump/print-netbios.c',
                     'freebsd/contrib/tcpdump/print-nfs.c',
                     'freebsd/contrib/tcpdump/print-ntp.c',
                     'freebsd/contrib/tcpdump/print-null.c',
                     'freebsd/contrib/tcpdump/print-olsr.c',
                     'freebsd/contrib/tcpdump/print-ospf.c',
                     'freebsd/contrib/tcpdump/print-ospf6.c',
                     'freebsd/contrib/tcpdump/print-otv.c',
                     'freebsd/contrib/tcpdump/print-pflog.c',
                     'freebsd/contrib/tcpdump/print-pfsync.c',
                     'freebsd/contrib/tcpdump/print-pgm.c',
                     'freebsd/contrib/tcpdump/print-pim.c',
                     'freebsd/contrib/tcpdump/print-ppi.c',
                     'freebsd/contrib/tcpdump/print-ppp.c',
                     'freebsd/contrib/tcpdump/print-pppoe.c',
                     'freebsd/contrib/tcpdump/print-pptp.c',
                     'freebsd/contrib/tcpdump/print-radius.c',
                     'freebsd/contrib/tcpdump/print-raw.c',
                     'freebsd/contrib/tcpdump/print-rip.c',
                     'freebsd/contrib/tcpdump/print-ripng.c',
                     'freebsd/contrib/tcpdump/print-rpki-rtr.c',
                     'freebsd/contrib/tcpdump/print-rrcp.c',
                     'freebsd/contrib/tcpdump/print-rsvp.c',
                     'freebsd/contrib/tcpdump/print-rt6.c',
                     'freebsd/contrib/tcpdump/print-rx.c',
                     'freebsd/contrib/tcpdump/print-sctp.c',
                     'freebsd/contrib/tcpdump/print-sflow.c',
                     'freebsd/contrib/tcpdump/print-sip.c',
                     'freebsd/contrib/tcpdump/print-sl.c',
                     'freebsd/contrib/tcpdump/print-sll.c',
                     'freebsd/contrib/tcpdump/print-slow.c',
                     'freebsd/contrib/tcpdump/print-smb.c',
                     'freebsd/contrib/tcpdump/print-snmp.c',
                     'freebsd/contrib/tcpdump/print-stp.c',
                     'freebsd/contrib/tcpdump/print-sunatm.c',
                     'freebsd/contrib/tcpdump/print-symantec.c',
                     'freebsd/contrib/tcpdump/print-syslog.c',
                     'freebsd/contrib/tcpdump/print-tcp.c',
                     'freebsd/contrib/tcpdump/print-telnet.c',
                     'freebsd/contrib/tcpdump/print-tftp.c',
                     'freebsd/contrib/tcpdump/print-timed.c',
                     'freebsd/contrib/tcpdump/print-tipc.c',
                     'freebsd/contrib/tcpdump/print-token.c',
                     'freebsd/contrib/tcpdump/print-udld.c',
                     'freebsd/contrib/tcpdump/print-udp.c',
                     'freebsd/contrib/tcpdump/print-usb.c',
                     'freebsd/contrib/tcpdump/print-vjc.c',
                     'freebsd/contrib/tcpdump/print-vqp.c',
                     'freebsd/contrib/tcpdump/print-vrrp.c',
                     'freebsd/contrib/tcpdump/print-vtp.c',
                     'freebsd/contrib/tcpdump/print-vxlan.c',
                     'freebsd/contrib/tcpdump/print-wb.c',
                     'freebsd/contrib/tcpdump/print-zephyr.c',
                     'freebsd/contrib/tcpdump/print-zeromq.c',
                     'freebsd/contrib/tcpdump/setsignal.c',
                     'freebsd/contrib/tcpdump/signature.c',
                     'freebsd/contrib/tcpdump/smbutil.c',
                     'freebsd/contrib/tcpdump/tcpdump.c',
                     'freebsd/contrib/tcpdump/util.c']
    bld.objects(target = "objs06",
                features = "c",
                cflags = cflags,
                includes = ['freebsd/contrib/tcpdump', 'freebsd/usr.sbin/tcpdump/tcpdump'] + includes,
                defines = defines + ['HAVE_CONFIG_H=1', 'HAVE_NET_PFVAR_H=1', 'INET6', '_U_=__attribute__((unused))', '__FreeBSD__=1'],
                source = objs06_source)
    libbsd_use += ["objs06"]

    source = ['freebsd/sys/arm/xilinx/zy7_slcr.c',
              'freebsd/sys/cam/cam.c',
              'freebsd/sys/cam/scsi/scsi_all.c',
              'freebsd/sys/crypto/blowfish/bf_ecb.c',
              'freebsd/sys/crypto/blowfish/bf_enc.c',
              'freebsd/sys/crypto/blowfish/bf_skey.c',
              'freebsd/sys/crypto/camellia/camellia-api.c',
              'freebsd/sys/crypto/camellia/camellia.c',
              'freebsd/sys/crypto/des/des_ecb.c',
              'freebsd/sys/crypto/des/des_enc.c',
              'freebsd/sys/crypto/des/des_setkey.c',
              'freebsd/sys/crypto/rc4/rc4.c',
              'freebsd/sys/crypto/rijndael/rijndael-alg-fst.c',
              'freebsd/sys/crypto/rijndael/rijndael-api-fst.c',
              'freebsd/sys/crypto/rijndael/rijndael-api.c',
              'freebsd/sys/crypto/sha1.c',
              'freebsd/sys/crypto/sha2/sha256c.c',
              'freebsd/sys/crypto/sha2/sha512c.c',
              'freebsd/sys/crypto/siphash/siphash.c',
              'freebsd/sys/crypto/skein/skein.c',
              'freebsd/sys/crypto/skein/skein_block.c',
              'freebsd/sys/dev/bce/if_bce.c',
              'freebsd/sys/dev/bfe/if_bfe.c',
              'freebsd/sys/dev/bge/if_bge.c',
              'freebsd/sys/dev/cadence/if_cgem.c',
              'freebsd/sys/dev/dc/dcphy.c',
              'freebsd/sys/dev/dc/if_dc.c',
              'freebsd/sys/dev/dc/pnphy.c',
              'freebsd/sys/dev/dwc/if_dwc.c',
              'freebsd/sys/dev/e1000/e1000_80003es2lan.c',
              'freebsd/sys/dev/e1000/e1000_82540.c',
              'freebsd/sys/dev/e1000/e1000_82541.c',
              'freebsd/sys/dev/e1000/e1000_82542.c',
              'freebsd/sys/dev/e1000/e1000_82543.c',
              'freebsd/sys/dev/e1000/e1000_82571.c',
              'freebsd/sys/dev/e1000/e1000_82575.c',
              'freebsd/sys/dev/e1000/e1000_api.c',
              'freebsd/sys/dev/e1000/e1000_i210.c',
              'freebsd/sys/dev/e1000/e1000_ich8lan.c',
              'freebsd/sys/dev/e1000/e1000_mac.c',
              'freebsd/sys/dev/e1000/e1000_manage.c',
              'freebsd/sys/dev/e1000/e1000_mbx.c',
              'freebsd/sys/dev/e1000/e1000_nvm.c',
              'freebsd/sys/dev/e1000/e1000_osdep.c',
              'freebsd/sys/dev/e1000/e1000_phy.c',
              'freebsd/sys/dev/e1000/e1000_vf.c',
              'freebsd/sys/dev/e1000/if_em.c',
              'freebsd/sys/dev/e1000/if_igb.c',
              'freebsd/sys/dev/e1000/if_lem.c',
              'freebsd/sys/dev/fxp/if_fxp.c',
              'freebsd/sys/dev/led/led.c',
              'freebsd/sys/dev/mii/brgphy.c',
              'freebsd/sys/dev/mii/e1000phy.c',
              'freebsd/sys/dev/mii/icsphy.c',
              'freebsd/sys/dev/mii/micphy.c',
              'freebsd/sys/dev/mii/mii.c',
              'freebsd/sys/dev/mii/mii_bitbang.c',
              'freebsd/sys/dev/mii/mii_physubr.c',
              'freebsd/sys/dev/mii/rgephy.c',
              'freebsd/sys/dev/mii/ukphy.c',
              'freebsd/sys/dev/mii/ukphy_subr.c',
              'freebsd/sys/dev/mmc/mmc.c',
              'freebsd/sys/dev/mmc/mmcsd.c',
              'freebsd/sys/dev/pci/pci.c',
              'freebsd/sys/dev/pci/pci_pci.c',
              'freebsd/sys/dev/pci/pci_user.c',
              'freebsd/sys/dev/re/if_re.c',
              'freebsd/sys/dev/smc/if_smc.c',
              'freebsd/sys/dev/tsec/if_tsec.c',
              'freebsd/sys/dev/usb/controller/dwc_otg.c',
              'freebsd/sys/dev/usb/controller/ehci.c',
              'freebsd/sys/dev/usb/controller/ohci.c',
              'freebsd/sys/dev/usb/controller/usb_controller.c',
              'freebsd/sys/dev/usb/quirk/usb_quirk.c',
              'freebsd/sys/dev/usb/storage/umass.c',
              'freebsd/sys/dev/usb/usb_busdma.c',
              'freebsd/sys/dev/usb/usb_core.c',
              'freebsd/sys/dev/usb/usb_debug.c',
              'freebsd/sys/dev/usb/usb_dev.c',
              'freebsd/sys/dev/usb/usb_device.c',
              'freebsd/sys/dev/usb/usb_dynamic.c',
              'freebsd/sys/dev/usb/usb_error.c',
              'freebsd/sys/dev/usb/usb_generic.c',
              'freebsd/sys/dev/usb/usb_handle_request.c',
              'freebsd/sys/dev/usb/usb_hid.c',
              'freebsd/sys/dev/usb/usb_hub.c',
              'freebsd/sys/dev/usb/usb_lookup.c',
              'freebsd/sys/dev/usb/usb_mbuf.c',
              'freebsd/sys/dev/usb/usb_msctest.c',
              'freebsd/sys/dev/usb/usb_parse.c',
              'freebsd/sys/dev/usb/usb_process.c',
              'freebsd/sys/dev/usb/usb_request.c',
              'freebsd/sys/dev/usb/usb_transfer.c',
              'freebsd/sys/dev/usb/usb_util.c',
              'freebsd/sys/kern/init_main.c',
              'freebsd/sys/kern/kern_condvar.c',
              'freebsd/sys/kern/kern_conf.c',
              'freebsd/sys/kern/kern_event.c',
              'freebsd/sys/kern/kern_hhook.c',
              'freebsd/sys/kern/kern_intr.c',
              'freebsd/sys/kern/kern_khelp.c',
              'freebsd/sys/kern/kern_linker.c',
              'freebsd/sys/kern/kern_mbuf.c',
              'freebsd/sys/kern/kern_mib.c',
              'freebsd/sys/kern/kern_module.c',
              'freebsd/sys/kern/kern_mtxpool.c',
              'freebsd/sys/kern/kern_osd.c',
              'freebsd/sys/kern/kern_synch.c',
              'freebsd/sys/kern/kern_sysctl.c',
              'freebsd/sys/kern/kern_time.c',
              'freebsd/sys/kern/kern_timeout.c',
              'freebsd/sys/kern/kern_uuid.c',
              'freebsd/sys/kern/subr_bufring.c',
              'freebsd/sys/kern/subr_bus.c',
              'freebsd/sys/kern/subr_counter.c',
              'freebsd/sys/kern/subr_eventhandler.c',
              'freebsd/sys/kern/subr_hash.c',
              'freebsd/sys/kern/subr_hints.c',
              'freebsd/sys/kern/subr_kobj.c',
              'freebsd/sys/kern/subr_lock.c',
              'freebsd/sys/kern/subr_module.c',
              'freebsd/sys/kern/subr_pcpu.c',
              'freebsd/sys/kern/subr_prf.c',
              'freebsd/sys/kern/subr_rman.c',
              'freebsd/sys/kern/subr_sbuf.c',
              'freebsd/sys/kern/subr_sleepqueue.c',
              'freebsd/sys/kern/subr_taskqueue.c',
              'freebsd/sys/kern/subr_uio.c',
              'freebsd/sys/kern/subr_unit.c',
              'freebsd/sys/kern/sys_generic.c',
              'freebsd/sys/kern/sys_pipe.c',
              'freebsd/sys/kern/sys_socket.c',
              'freebsd/sys/kern/uipc_accf.c',
              'freebsd/sys/kern/uipc_domain.c',
              'freebsd/sys/kern/uipc_mbuf.c',
              'freebsd/sys/kern/uipc_mbuf2.c',
              'freebsd/sys/kern/uipc_mbufhash.c',
              'freebsd/sys/kern/uipc_sockbuf.c',
              'freebsd/sys/kern/uipc_socket.c',
              'freebsd/sys/kern/uipc_syscalls.c',
              'freebsd/sys/kern/uipc_usrreq.c',
              'freebsd/sys/libkern/bcd.c',
              'freebsd/sys/libkern/inet_ntoa.c',
              'freebsd/sys/libkern/jenkins_hash.c',
              'freebsd/sys/libkern/murmur3_32.c',
              'freebsd/sys/libkern/random.c',
              'freebsd/sys/net/altq/altq_cbq.c',
              'freebsd/sys/net/altq/altq_cdnr.c',
              'freebsd/sys/net/altq/altq_codel.c',
              'freebsd/sys/net/altq/altq_fairq.c',
              'freebsd/sys/net/altq/altq_hfsc.c',
              'freebsd/sys/net/altq/altq_priq.c',
              'freebsd/sys/net/altq/altq_red.c',
              'freebsd/sys/net/altq/altq_rio.c',
              'freebsd/sys/net/altq/altq_rmclass.c',
              'freebsd/sys/net/altq/altq_subr.c',
              'freebsd/sys/net/bpf.c',
              'freebsd/sys/net/bpf_buffer.c',
              'freebsd/sys/net/bpf_filter.c',
              'freebsd/sys/net/bpf_jitter.c',
              'freebsd/sys/net/bridgestp.c',
              'freebsd/sys/net/ieee8023ad_lacp.c',
              'freebsd/sys/net/if.c',
              'freebsd/sys/net/if_arcsubr.c',
              'freebsd/sys/net/if_atmsubr.c',
              'freebsd/sys/net/if_bridge.c',
              'freebsd/sys/net/if_clone.c',
              'freebsd/sys/net/if_dead.c',
              'freebsd/sys/net/if_disc.c',
              'freebsd/sys/net/if_edsc.c',
              'freebsd/sys/net/if_enc.c',
              'freebsd/sys/net/if_epair.c',
              'freebsd/sys/net/if_ethersubr.c',
              'freebsd/sys/net/if_fddisubr.c',
              'freebsd/sys/net/if_fwsubr.c',
              'freebsd/sys/net/if_gif.c',
              'freebsd/sys/net/if_gre.c',
              'freebsd/sys/net/if_iso88025subr.c',
              'freebsd/sys/net/if_lagg.c',
              'freebsd/sys/net/if_llatbl.c',
              'freebsd/sys/net/if_loop.c',
              'freebsd/sys/net/if_media.c',
              'freebsd/sys/net/if_mib.c',
              'freebsd/sys/net/if_spppfr.c',
              'freebsd/sys/net/if_spppsubr.c',
              'freebsd/sys/net/if_stf.c',
              'freebsd/sys/net/if_tap.c',
              'freebsd/sys/net/if_tun.c',
              'freebsd/sys/net/if_vlan.c',
              'freebsd/sys/net/netisr.c',
              'freebsd/sys/net/pfil.c',
              'freebsd/sys/net/radix.c',
              'freebsd/sys/net/radix_mpath.c',
              'freebsd/sys/net/raw_cb.c',
              'freebsd/sys/net/raw_usrreq.c',
              'freebsd/sys/net/route.c',
              'freebsd/sys/net/rtsock.c',
              'freebsd/sys/net/slcompress.c',
              'freebsd/sys/netinet/accf_data.c',
              'freebsd/sys/netinet/accf_dns.c',
              'freebsd/sys/netinet/accf_http.c',
              'freebsd/sys/netinet/cc/cc.c',
              'freebsd/sys/netinet/cc/cc_newreno.c',
              'freebsd/sys/netinet/if_atm.c',
              'freebsd/sys/netinet/if_ether.c',
              'freebsd/sys/netinet/igmp.c',
              'freebsd/sys/netinet/in.c',
              'freebsd/sys/netinet/in_fib.c',
              'freebsd/sys/netinet/in_gif.c',
              'freebsd/sys/netinet/in_mcast.c',
              'freebsd/sys/netinet/in_pcb.c',
              'freebsd/sys/netinet/in_proto.c',
              'freebsd/sys/netinet/in_rmx.c',
              'freebsd/sys/netinet/ip_carp.c',
              'freebsd/sys/netinet/ip_divert.c',
              'freebsd/sys/netinet/ip_ecn.c',
              'freebsd/sys/netinet/ip_encap.c',
              'freebsd/sys/netinet/ip_fastfwd.c',
              'freebsd/sys/netinet/ip_gre.c',
              'freebsd/sys/netinet/ip_icmp.c',
              'freebsd/sys/netinet/ip_id.c',
              'freebsd/sys/netinet/ip_input.c',
              'freebsd/sys/netinet/ip_mroute.c',
              'freebsd/sys/netinet/ip_options.c',
              'freebsd/sys/netinet/ip_output.c',
              'freebsd/sys/netinet/ip_reass.c',
              'freebsd/sys/netinet/libalias/alias.c',
              'freebsd/sys/netinet/libalias/alias_cuseeme.c',
              'freebsd/sys/netinet/libalias/alias_db.c',
              'freebsd/sys/netinet/libalias/alias_dummy.c',
              'freebsd/sys/netinet/libalias/alias_ftp.c',
              'freebsd/sys/netinet/libalias/alias_irc.c',
              'freebsd/sys/netinet/libalias/alias_mod.c',
              'freebsd/sys/netinet/libalias/alias_nbt.c',
              'freebsd/sys/netinet/libalias/alias_pptp.c',
              'freebsd/sys/netinet/libalias/alias_proxy.c',
              'freebsd/sys/netinet/libalias/alias_sctp.c',
              'freebsd/sys/netinet/libalias/alias_skinny.c',
              'freebsd/sys/netinet/libalias/alias_smedia.c',
              'freebsd/sys/netinet/libalias/alias_util.c',
              'freebsd/sys/netinet/raw_ip.c',
              'freebsd/sys/netinet/sctp_asconf.c',
              'freebsd/sys/netinet/sctp_auth.c',
              'freebsd/sys/netinet/sctp_bsd_addr.c',
              'freebsd/sys/netinet/sctp_cc_functions.c',
              'freebsd/sys/netinet/sctp_crc32.c',
              'freebsd/sys/netinet/sctp_indata.c',
              'freebsd/sys/netinet/sctp_input.c',
              'freebsd/sys/netinet/sctp_output.c',
              'freebsd/sys/netinet/sctp_pcb.c',
              'freebsd/sys/netinet/sctp_peeloff.c',
              'freebsd/sys/netinet/sctp_sysctl.c',
              'freebsd/sys/netinet/sctp_timer.c',
              'freebsd/sys/netinet/sctp_usrreq.c',
              'freebsd/sys/netinet/sctputil.c',
              'freebsd/sys/netinet/tcp_debug.c',
              'freebsd/sys/netinet/tcp_hostcache.c',
              'freebsd/sys/netinet/tcp_input.c',
              'freebsd/sys/netinet/tcp_lro.c',
              'freebsd/sys/netinet/tcp_offload.c',
              'freebsd/sys/netinet/tcp_output.c',
              'freebsd/sys/netinet/tcp_reass.c',
              'freebsd/sys/netinet/tcp_sack.c',
              'freebsd/sys/netinet/tcp_subr.c',
              'freebsd/sys/netinet/tcp_syncache.c',
              'freebsd/sys/netinet/tcp_timer.c',
              'freebsd/sys/netinet/tcp_timewait.c',
              'freebsd/sys/netinet/tcp_usrreq.c',
              'freebsd/sys/netinet/udp_usrreq.c',
              'freebsd/sys/netinet6/dest6.c',
              'freebsd/sys/netinet6/frag6.c',
              'freebsd/sys/netinet6/icmp6.c',
              'freebsd/sys/netinet6/in6.c',
              'freebsd/sys/netinet6/in6_cksum.c',
              'freebsd/sys/netinet6/in6_fib.c',
              'freebsd/sys/netinet6/in6_gif.c',
              'freebsd/sys/netinet6/in6_ifattach.c',
              'freebsd/sys/netinet6/in6_mcast.c',
              'freebsd/sys/netinet6/in6_pcb.c',
              'freebsd/sys/netinet6/in6_proto.c',
              'freebsd/sys/netinet6/in6_rmx.c',
              'freebsd/sys/netinet6/in6_src.c',
              'freebsd/sys/netinet6/ip6_forward.c',
              'freebsd/sys/netinet6/ip6_id.c',
              'freebsd/sys/netinet6/ip6_input.c',
              'freebsd/sys/netinet6/ip6_mroute.c',
              'freebsd/sys/netinet6/ip6_output.c',
              'freebsd/sys/netinet6/mld6.c',
              'freebsd/sys/netinet6/nd6.c',
              'freebsd/sys/netinet6/nd6_nbr.c',
              'freebsd/sys/netinet6/nd6_rtr.c',
              'freebsd/sys/netinet6/raw_ip6.c',
              'freebsd/sys/netinet6/route6.c',
              'freebsd/sys/netinet6/scope6.c',
              'freebsd/sys/netinet6/sctp6_usrreq.c',
              'freebsd/sys/netinet6/udp6_usrreq.c',
              'freebsd/sys/netpfil/ipfw/ip_fw2.c',
              'freebsd/sys/netpfil/ipfw/ip_fw_bpf.c',
              'freebsd/sys/netpfil/ipfw/ip_fw_dynamic.c',
              'freebsd/sys/netpfil/ipfw/ip_fw_eaction.c',
              'freebsd/sys/netpfil/ipfw/ip_fw_iface.c',
              'freebsd/sys/netpfil/ipfw/ip_fw_log.c',
              'freebsd/sys/netpfil/ipfw/ip_fw_nat.c',
              'freebsd/sys/netpfil/ipfw/ip_fw_pfil.c',
              'freebsd/sys/netpfil/ipfw/ip_fw_sockopt.c',
              'freebsd/sys/netpfil/ipfw/ip_fw_table.c',
              'freebsd/sys/netpfil/ipfw/ip_fw_table_algo.c',
              'freebsd/sys/netpfil/ipfw/ip_fw_table_value.c',
              'freebsd/sys/netpfil/ipfw/nat64/ip_fw_nat64.c',
              'freebsd/sys/netpfil/ipfw/nat64/nat64_translate.c',
              'freebsd/sys/netpfil/ipfw/nat64/nat64lsn.c',
              'freebsd/sys/netpfil/ipfw/nat64/nat64lsn_control.c',
              'freebsd/sys/netpfil/ipfw/nat64/nat64stl.c',
              'freebsd/sys/netpfil/ipfw/nat64/nat64stl_control.c',
              'freebsd/sys/netpfil/ipfw/nptv6/ip_fw_nptv6.c',
              'freebsd/sys/netpfil/ipfw/nptv6/nptv6.c',
              'freebsd/sys/netpfil/pf/if_pflog.c',
              'freebsd/sys/netpfil/pf/if_pfsync.c',
              'freebsd/sys/netpfil/pf/in4_cksum.c',
              'freebsd/sys/netpfil/pf/pf.c',
              'freebsd/sys/netpfil/pf/pf_if.c',
              'freebsd/sys/netpfil/pf/pf_ioctl.c',
              'freebsd/sys/netpfil/pf/pf_lb.c',
              'freebsd/sys/netpfil/pf/pf_norm.c',
              'freebsd/sys/netpfil/pf/pf_osfp.c',
              'freebsd/sys/netpfil/pf/pf_ruleset.c',
              'freebsd/sys/netpfil/pf/pf_table.c',
              'freebsd/sys/opencrypto/cast.c',
              'freebsd/sys/opencrypto/criov.c',
              'freebsd/sys/opencrypto/crypto.c',
              'freebsd/sys/opencrypto/cryptodeflate.c',
              'freebsd/sys/opencrypto/cryptosoft.c',
              'freebsd/sys/opencrypto/gfmult.c',
              'freebsd/sys/opencrypto/gmac.c',
              'freebsd/sys/opencrypto/rmd160.c',
              'freebsd/sys/opencrypto/skipjack.c',
              'freebsd/sys/opencrypto/xform.c',
              'freebsd/sys/vm/uma_core.c',
              'freebsd/sys/vm/uma_dbg.c',
              'mDNSResponder/mDNSCore/CryptoAlg.c',
              'mDNSResponder/mDNSCore/DNSCommon.c',
              'mDNSResponder/mDNSCore/DNSDigest.c',
              'mDNSResponder/mDNSCore/anonymous.c',
              'mDNSResponder/mDNSCore/mDNS.c',
              'mDNSResponder/mDNSCore/uDNS.c',
              'mDNSResponder/mDNSPosix/mDNSPosix.c',
              'mDNSResponder/mDNSPosix/mDNSUNP.c',
              'mDNSResponder/mDNSShared/GenLinkedList.c',
              'mDNSResponder/mDNSShared/PlatformCommon.c',
              'mDNSResponder/mDNSShared/dnssd_clientshim.c',
              'mDNSResponder/mDNSShared/mDNSDebug.c',
              'rtemsbsd/ftpd/ftpd-init.c',
              'rtemsbsd/ftpd/ftpd-service.c',
              'rtemsbsd/ftpd/ftpd.c',
              'rtemsbsd/ftpfs/ftpfs.c',
              'rtemsbsd/local/bus_if.c',
              'rtemsbsd/local/cryptodev_if.c',
              'rtemsbsd/local/device_if.c',
              'rtemsbsd/local/gpio_if.c',
              'rtemsbsd/local/if_dwc_if.c',
              'rtemsbsd/local/miibus_if.c',
              'rtemsbsd/local/mmcbr_if.c',
              'rtemsbsd/local/mmcbus_if.c',
              'rtemsbsd/local/pci_if.c',
              'rtemsbsd/local/pcib_if.c',
              'rtemsbsd/local/usb_if.c',
              'rtemsbsd/mdns/mdns-hostname-default.c',
              'rtemsbsd/mdns/mdns.c',
              'rtemsbsd/nfsclient/mount_prot_xdr.c',
              'rtemsbsd/nfsclient/nfs.c',
              'rtemsbsd/nfsclient/nfs_prot_xdr.c',
              'rtemsbsd/nfsclient/rpcio.c',
              'rtemsbsd/pppd/auth.c',
              'rtemsbsd/pppd/ccp.c',
              'rtemsbsd/pppd/chap.c',
              'rtemsbsd/pppd/chap_ms.c',
              'rtemsbsd/pppd/chat.c',
              'rtemsbsd/pppd/demand.c',
              'rtemsbsd/pppd/fsm.c',
              'rtemsbsd/pppd/ipcp.c',
              'rtemsbsd/pppd/lcp.c',
              'rtemsbsd/pppd/magic.c',
              'rtemsbsd/pppd/options.c',
              'rtemsbsd/pppd/rtemsmain.c',
              'rtemsbsd/pppd/rtemspppd.c',
              'rtemsbsd/pppd/sys-rtems.c',
              'rtemsbsd/pppd/upap.c',
              'rtemsbsd/pppd/utils.c',
              'rtemsbsd/rtems/ipsec_get_policylen.c',
              'rtemsbsd/rtems/rtems-bsd-allocator-domain-size.c',
              'rtemsbsd/rtems/rtems-bsd-arp-processor.c',
              'rtemsbsd/rtems/rtems-bsd-get-allocator-domain-size.c',
              'rtemsbsd/rtems/rtems-bsd-get-ethernet-addr.c',
              'rtemsbsd/rtems/rtems-bsd-get-mac-address.c',
              'rtemsbsd/rtems/rtems-bsd-get-task-priority.c',
              'rtemsbsd/rtems/rtems-bsd-get-task-stack-size.c',
              'rtemsbsd/rtems/rtems-bsd-rc-conf-net.c',
              'rtemsbsd/rtems/rtems-bsd-rc-conf-pf.c',
              'rtemsbsd/rtems/rtems-bsd-rc-conf.c',
              'rtemsbsd/rtems/rtems-bsd-shell-dhcpcd.c',
              'rtemsbsd/rtems/rtems-bsd-shell-ifconfig.c',
              'rtemsbsd/rtems/rtems-bsd-shell-netstat.c',
              'rtemsbsd/rtems/rtems-bsd-shell-pfctl.c',
              'rtemsbsd/rtems/rtems-bsd-shell-ping.c',
              'rtemsbsd/rtems/rtems-bsd-shell-route.c',
              'rtemsbsd/rtems/rtems-bsd-shell-sysctl.c',
              'rtemsbsd/rtems/rtems-bsd-shell-tcpdump.c',
              'rtemsbsd/rtems/rtems-bsd-shell.c',
              'rtemsbsd/rtems/rtems-bsd-syscall-api.c',
              'rtemsbsd/rtems/rtems-kernel-assert.c',
              'rtemsbsd/rtems/rtems-kernel-autoconf.c',
              'rtemsbsd/rtems/rtems-kernel-bus-dma-mbuf.c',
              'rtemsbsd/rtems/rtems-kernel-bus-dma.c',
              'rtemsbsd/rtems/rtems-kernel-bus-root.c',
              'rtemsbsd/rtems/rtems-kernel-cam.c',
              'rtemsbsd/rtems/rtems-kernel-chunk.c',
              'rtemsbsd/rtems/rtems-kernel-configintrhook.c',
              'rtemsbsd/rtems/rtems-kernel-delay.c',
              'rtemsbsd/rtems/rtems-kernel-get-file.c',
              'rtemsbsd/rtems/rtems-kernel-init.c',
              'rtemsbsd/rtems/rtems-kernel-irqs.c',
              'rtemsbsd/rtems/rtems-kernel-jail.c',
              'rtemsbsd/rtems/rtems-kernel-malloc.c',
              'rtemsbsd/rtems/rtems-kernel-mbuf.c',
              'rtemsbsd/rtems/rtems-kernel-mutex.c',
              'rtemsbsd/rtems/rtems-kernel-muteximpl.c',
              'rtemsbsd/rtems/rtems-kernel-nexus.c',
              'rtemsbsd/rtems/rtems-kernel-page.c',
              'rtemsbsd/rtems/rtems-kernel-panic.c',
              'rtemsbsd/rtems/rtems-kernel-pci_bus.c',
              'rtemsbsd/rtems/rtems-kernel-pci_cfgreg.c',
              'rtemsbsd/rtems/rtems-kernel-program.c',
              'rtemsbsd/rtems/rtems-kernel-rwlock.c',
              'rtemsbsd/rtems/rtems-kernel-signal.c',
              'rtemsbsd/rtems/rtems-kernel-sx.c',
              'rtemsbsd/rtems/rtems-kernel-sysctl.c',
              'rtemsbsd/rtems/rtems-kernel-sysctlbyname.c',
              'rtemsbsd/rtems/rtems-kernel-sysctlnametomib.c',
              'rtemsbsd/rtems/rtems-kernel-thread.c',
              'rtemsbsd/rtems/rtems-kernel-timesupport.c',
              'rtemsbsd/rtems/rtems-kernel-vprintf.c',
              'rtemsbsd/rtems/rtems-kvm.c',
              'rtemsbsd/rtems/rtems-legacy-mii.c',
              'rtemsbsd/rtems/rtems-legacy-newproc.c',
              'rtemsbsd/rtems/rtems-legacy-rtrequest.c',
              'rtemsbsd/rtems/rtems-program-socket.c',
              'rtemsbsd/rtems/rtems-program.c',
              'rtemsbsd/rtems/rtems-routes.c',
              'rtemsbsd/rtems/syslog.c',
              'rtemsbsd/sys/dev/dw_mmc/dw_mmc.c',
              'rtemsbsd/sys/dev/ffec/if_ffec_mcf548x.c',
              'rtemsbsd/sys/dev/smc/if_smc_nexus.c',
              'rtemsbsd/sys/dev/tsec/if_tsec_nexus.c',
              'rtemsbsd/sys/dev/usb/controller/dwc_otg_nexus.c',
              'rtemsbsd/sys/dev/usb/controller/ehci_mpc83xx.c',
              'rtemsbsd/sys/dev/usb/controller/ohci_lpc.c',
              'rtemsbsd/sys/dev/usb/controller/usb_otg_transceiver.c',
              'rtemsbsd/sys/dev/usb/controller/usb_otg_transceiver_dump.c',
              'rtemsbsd/sys/fs/devfs/devfs_devs.c',
              'rtemsbsd/sys/net/if_ppp.c',
              'rtemsbsd/sys/net/ppp_tty.c',
              'rtemsbsd/telnetd/check_passwd.c',
              'rtemsbsd/telnetd/des.c',
              'rtemsbsd/telnetd/pty.c',
              'rtemsbsd/telnetd/telnetd-init.c',
              'rtemsbsd/telnetd/telnetd-service.c',
              'rtemsbsd/telnetd/telnetd.c']
    if bld.env["HAVE_RTEMS_RTEMS_DEBUGGER_H"]:
        source += ['rtemsbsd/debugger/rtems-debugger-remote-tcp.c']
    if bld.get_env()["RTEMS_ARCH"] == "arm":
        source += ['freebsd/sys/mips/mips/in_cksum.c']
    if bld.get_env()["RTEMS_ARCH"] == "avr":
        source += ['freebsd/sys/mips/mips/in_cksum.c']
    if bld.get_env()["RTEMS_ARCH"] == "bfin":
        source += ['freebsd/sys/mips/mips/in_cksum.c']
    if bld.get_env()["RTEMS_ARCH"] == "cflags":
        source += ['default']
    if bld.get_env()["RTEMS_ARCH"] == "h8300":
        source += ['freebsd/sys/mips/mips/in_cksum.c']
    if bld.get_env()["RTEMS_ARCH"] == "i386":
        source += ['freebsd/sys/i386/i386/in_cksum.c',
                   'freebsd/sys/i386/i386/legacy.c',
                   'freebsd/sys/x86/pci/pci_bus.c']
    if bld.get_env()["RTEMS_ARCH"] == "lm32":
        source += ['freebsd/sys/mips/mips/in_cksum.c']
    if bld.get_env()["RTEMS_ARCH"] == "m32c":
        source += ['freebsd/sys/mips/mips/in_cksum.c']
    if bld.get_env()["RTEMS_ARCH"] == "m32r":
        source += ['freebsd/sys/mips/mips/in_cksum.c']
    if bld.get_env()["RTEMS_ARCH"] == "m68k":
        source += ['freebsd/sys/mips/mips/in_cksum.c']
    if bld.get_env()["RTEMS_ARCH"] == "mips":
        source += ['freebsd/sys/mips/mips/in_cksum.c']
    if bld.get_env()["RTEMS_ARCH"] == "nios2":
        source += ['freebsd/sys/mips/mips/in_cksum.c']
    if bld.get_env()["RTEMS_ARCH"] == "powerpc":
        source += ['freebsd/sys/powerpc/powerpc/in_cksum.c']
    if bld.get_env()["RTEMS_ARCH"] == "sh":
        source += ['freebsd/sys/mips/mips/in_cksum.c']
    if bld.get_env()["RTEMS_ARCH"] == "sparc":
        source += ['freebsd/sys/mips/mips/in_cksum.c']
    if bld.get_env()["RTEMS_ARCH"] == "sparc64":
        source += ['freebsd/sys/sparc64/sparc64/in_cksum.c']
    if bld.get_env()["RTEMS_ARCH"] == "v850":
        source += ['freebsd/sys/mips/mips/in_cksum.c']
    bld.stlib(target = "bsd",
              features = "c cxx",
              cflags = cflags,
              cxxflags = cxxflags,
              includes = includes,
              defines = defines,
              source = source,
              use = libbsd_use)

    # Installs.    
    bld.install_files("${PREFIX}/" + rtems.arch_bsp_lib_path(bld.env.RTEMS_VERSION, bld.env.RTEMS_ARCH_BSP), ["libbsd.a"])
    header_paths = [('rtemsbsd/include', '*.h', ''),
                     ('rtemsbsd/mghttpd', 'mongoose.h', 'mghttpd'),
                     ('freebsd/include', '*.h', ''),
                     ('freebsd/sys/bsm', '*.h', 'bsm'),
                     ('freebsd/sys/cam', '*.h', 'cam'),
                     ('freebsd/sys/net', '*.h', 'net'),
                     ('freebsd/sys/net80211', '*.h', 'net80211'),
                     ('freebsd/sys/netinet', '*.h', 'netinet'),
                     ('freebsd/sys/netinet6', '*.h', 'netinet6'),
                     ('freebsd/sys/netipsec', '*.h', 'netipsec'),
                     ('freebsd/sys/rpc', '*.h', 'rpc'),
                     ('freebsd/sys/sys', '*.h', 'sys'),
                     ('freebsd/sys/vm', '*.h', 'vm'),
                     ('freebsd/sys/dev/mii', '*.h', 'dev/mii'),
                     ('mDNSResponder/mDNSCore', 'mDNSDebug.h', ''),
                     ('mDNSResponder/mDNSCore', 'mDNSEmbeddedAPI.h', ''),
                     ('mDNSResponder/mDNSShared', 'dns_sd.h', ''),
                     ('mDNSResponder/mDNSPosix', 'mDNSPosix.h', '')]
    for headers in header_paths:
        ipath = os.path.join(rtems.arch_bsp_include_path(bld.env.RTEMS_VERSION, bld.env.RTEMS_ARCH_BSP), headers[2])
        start_dir = bld.path.find_dir(headers[0])
        bld.install_files("${PREFIX}/" + ipath,
                          start_dir.ant_glob("**/" + headers[1]),
                          cwd = start_dir,
                          relative_trick = True)

    # Tests
    test_arphole = ['testsuite/arphole/test_main.c']
    bld.program(target = "arphole.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_arphole,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_cdev01 = ['testsuite/cdev01/test_cdev.c',
                   'testsuite/cdev01/test_main.c']
    bld.program(target = "cdev01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_cdev01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_commands01 = ['testsuite/commands01/test_main.c']
    bld.program(target = "commands01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_commands01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_condvar01 = ['testsuite/condvar01/test_main.c']
    bld.program(target = "condvar01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_condvar01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    if bld.env["HAVE_RTEMS_RTEMS_DEBUGGER_H"]:
        test_debugger01 = ['testsuite/debugger01/test_main.c']
        bld.program(target = "debugger01.exe",
                    features = "cprogram",
                    cflags = cflags,
                    includes = includes,
                    source = test_debugger01,
                    use = ["bsd"],
                    lib = ["m", "z"],
                    install_path = None)

    test_dhcpcd01 = ['testsuite/dhcpcd01/test_main.c']
    bld.program(target = "dhcpcd01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_dhcpcd01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_dhcpcd02 = ['testsuite/dhcpcd02/test_main.c']
    bld.program(target = "dhcpcd02.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_dhcpcd02,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_foobarclient = ['testsuite/foobarclient/test_main.c']
    bld.program(target = "foobarclient.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_foobarclient,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_foobarserver = ['testsuite/foobarserver/test_main.c']
    bld.program(target = "foobarserver.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_foobarserver,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_ftpd01 = ['testsuite/ftpd01/test_main.c']
    bld.program(target = "ftpd01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_ftpd01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_ftpd02 = ['testsuite/ftpd02/test_main.c']
    bld.program(target = "ftpd02.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_ftpd02,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_init01 = ['testsuite/init01/test_main.c']
    bld.program(target = "init01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_init01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_lagg01 = ['testsuite/lagg01/test_main.c']
    bld.program(target = "lagg01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_lagg01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_log01 = ['testsuite/log01/test_main.c']
    bld.program(target = "log01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_log01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_loopback01 = ['testsuite/loopback01/test_main.c']
    bld.program(target = "loopback01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_loopback01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_media01 = ['testsuite/media01/test_main.c']
    bld.program(target = "media01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_media01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_mutex01 = ['testsuite/mutex01/test_main.c']
    bld.program(target = "mutex01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_mutex01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_netshell01 = ['testsuite/netshell01/shellconfig.c',
                       'testsuite/netshell01/test_main.c']
    bld.program(target = "netshell01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_netshell01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_nfs01 = ['testsuite/nfs01/test_main.c']
    bld.program(target = "nfs01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_nfs01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_pf01 = ['testsuite/pf01/test_main.c']
    bld.program(target = "pf01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_pf01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_pf02 = ['testsuite/pf02/test_main.c']
    bld.program(target = "pf02.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_pf02,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_ping01 = ['testsuite/ping01/test_main.c']
    bld.program(target = "ping01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_ping01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_ppp01 = ['testsuite/ppp01/test_main.c']
    bld.program(target = "ppp01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_ppp01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_program01 = ['testsuite/program01/test_main.c']
    bld.program(target = "program01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_program01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_rcconf01 = ['testsuite/rcconf01/test_main.c']
    bld.program(target = "rcconf01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_rcconf01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_rcconf02 = ['testsuite/rcconf02/test_main.c']
    bld.program(target = "rcconf02.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_rcconf02,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_rwlock01 = ['testsuite/rwlock01/test_main.c']
    bld.program(target = "rwlock01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_rwlock01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_selectpollkqueue01 = ['testsuite/selectpollkqueue01/test_main.c']
    bld.program(target = "selectpollkqueue01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_selectpollkqueue01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_sleep01 = ['testsuite/sleep01/test_main.c']
    bld.program(target = "sleep01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_sleep01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_smp01 = ['testsuite/smp01/test_main.c']
    bld.program(target = "smp01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_smp01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_swi01 = ['testsuite/swi01/init.c',
                  'testsuite/swi01/swi_test.c']
    bld.program(target = "swi01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_swi01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_syscalls01 = ['testsuite/syscalls01/test_main.c']
    bld.program(target = "syscalls01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_syscalls01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_telnetd01 = ['testsuite/telnetd01/test_main.c']
    bld.program(target = "telnetd01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_telnetd01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_thread01 = ['testsuite/thread01/test_main.c']
    bld.program(target = "thread01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_thread01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_timeout01 = ['testsuite/timeout01/init.c',
                      'testsuite/timeout01/timeout_test.c']
    bld.program(target = "timeout01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_timeout01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_unix01 = ['testsuite/unix01/test_main.c']
    bld.program(target = "unix01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_unix01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_usb01 = ['testsuite/usb01/init.c']
    bld.program(target = "usb01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_usb01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_vlan01 = ['testsuite/vlan01/test_main.c']
    bld.program(target = "vlan01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_vlan01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

    test_zerocopy01 = ['testsuite/zerocopy01/test_main.c']
    bld.program(target = "zerocopy01.exe",
                features = "cprogram",
                cflags = cflags,
                includes = includes,
                source = test_zerocopy01,
                use = ["bsd"],
                lib = ["m", "z"],
                install_path = None)

