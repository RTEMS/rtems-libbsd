#
#  Copyright (c) 2015-2016, 2018 Chris Johns <chrisj@rtems.org>.
#  All rights reserved.
#
#  Copyright (c) 2009, 2018 embedded brains GmbH.  All rights reserved.
#
#   embedded brains GmbH
#   Dornierstr. 4
#   82178 Puchheim
#   Germany
#   <info@embedded-brains.de>
#
#  Copyright (c) 2012 OAR Corporation. All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import builder

#
# Default configuration.
#
_defaults = {
    #
    # Compile flags
    #
    'common-flags': ['-g',
                     '-fno-strict-aliasing',
                     '-ffreestanding',
                     '-fno-common'],
    'common-warnings' : ['-Wall',
                         '-Wno-format',
                         '-Wno-pointer-sign'],
    'common-no-warnings': ['-w'],
    'cflags': [],
    'cxxflags': [],

    #
    # Includes
    #
    'include-paths': ['rtemsbsd/include',
                      'freebsd/sys',
                      'freebsd/sys/contrib/pf',
                      'freebsd/crypto',
                      'freebsd/sys/net',
                      'freebsd/include',
                      'freebsd/lib',
                      'freebsd/lib/libbsdstat',
                      'freebsd/lib/libcasper',
                      'freebsd/lib/libc/include',
                      'freebsd/lib/libc/isc/include',
                      'freebsd/lib/libc/resolv',
                      'freebsd/lib/libutil',
                      'freebsd/lib/libkvm',
                      'freebsd/lib/libmemstat',
                      'freebsd/contrib/expat/lib',
                      'freebsd/contrib/libpcap',
                      'freebsd/contrib/libxo',
                      'ipsec-tools/src/libipsec',
                      'linux/include',
                      'linux/drivers/net/ethernet/freescale/fman',
                      'rtemsbsd/sys',
                      'mDNSResponder/mDNSCore',
                      'mDNSResponder/mDNSShared',
                      'mDNSResponder/mDNSPosix',
                      'testsuite/include'],
    'cpu-include-paths': ['rtemsbsd/@CPU@/include',
                          'freebsd/sys/@CPU@/include'],

    # The path where headers will be copied during build.
    'build-include-path': ['build-include'],

    #
    # Install headers
    #
    # A list of information about what header files should be installed.
    #
    # The list is also used to find headers with a local path that doesn't
    # match it's dest path. Due to the difference in the path name such files
    # are problematic during the build if they are included using their later
    # installation path (dest path) name. Therefore they are copied into a
    # sub-directory of the build path so that they can be included with their
    # normal installation path.
    #
    'header-paths':
    #  local path                               wildcard                           dest path
    [('rtemsbsd/include',                       '**/*.h',                          ''),
     ('rtemsbsd/@CPU@/include',                 '**/*.h',                          ''),
     ('rtemsbsd/mghttpd',                       'mongoose.h',                      'mghttpd'),
     ('freebsd/include',                        '**/*.h',                          ''),
     ('freebsd/sys/bsm',                        '**/*.h',                          'bsm'),
     ('freebsd/sys/cam',                        '**/*.h',                          'cam'),
     ('freebsd/sys/crypto',                     '*/*.h',                           'openssl'),
     ('freebsd/sys/net',                        '**/*.h',                          'net'),
     ('freebsd/sys/net80211',                   '**/*.h',                          'net80211'),
     ('freebsd/sys/netinet',                    '**/*.h',                          'netinet'),
     ('freebsd/sys/netinet6',                   '**/*.h',                          'netinet6'),
     ('freebsd/sys/netipsec',                   '**/*.h',                          'netipsec'),
     ('freebsd/contrib/libpcap',                '*.h',                             ''),
     ('freebsd/contrib/libpcap/pcap',           '*.h',                             'pcap'),
     ('freebsd/crypto/openssl',                 '*.h',                             'openssl'),
     ('freebsd/crypto/openssl/crypto',          '*.h',                             'openssl'),
     ('freebsd/crypto/openssl/ssl',             '(ssl|kssl|ssl2).h',               'openssl'),
     ('freebsd/crypto/openssl/crypto/aes',      'aes.h',                           'openssl'),
     ('freebsd/crypto/openssl/crypto/err',      'err.h',                           'openssl'),
     ('freebsd/crypto/openssl/crypto/bio',      '*.h',                             'openssl'),
     ('freebsd/crypto/openssl/crypto/dsa',      '*.h',                             'openssl'),
     ('freebsd/crypto/openssl/ssl',             '*.h',                             'openssl'),
     ('freebsd/crypto/openssl/crypto/bn',       'bn.h',                            'openssl'),
     ('freebsd/crypto/openssl/crypto/x509',     'x509.h',                          'openssl'),
     ('freebsd/crypto/openssl/crypto/cast',     'cast.h',                          'openssl'),
     ('freebsd/crypto/openssl/crypto/lhash',    'lhash.h',                         'openssl'),
     ('freebsd/crypto/openssl/crypto/ecdh',     'ecdh.h',                          'openssl'),
     ('freebsd/crypto/openssl/crypto/ecdsa',    'ecdsa.h',                         'openssl'),
     ('freebsd/crypto/openssl/crypto/idea',     'idea.h',                          'openssl'),
     ('freebsd/crypto/openssl/crypto/mdc2',     'mdc2.h',                          'openssl'),
     ('freebsd/crypto/openssl/crypto/md4',      'md4.h',                           'openssl'),
     ('freebsd/crypto/openssl/crypto/md5',      'md5.h',                           'openssl'),
     ('freebsd/crypto/openssl/crypto/rc2',      'rc2.h',                           'openssl'),
     ('freebsd/crypto/openssl/crypto/rc4',      'rc4.h',                           'openssl'),
     ('freebsd/crypto/openssl/crypto/ripemd',   'ripemd.h',                        'openssl'),
     ('freebsd/crypto/openssl/crypto/seed',     'seed.h',                          'openssl'),
     ('freebsd/crypto/openssl/crypto/sha',      'sha.h',                           'openssl'),
     ('freebsd/crypto/openssl/crypto/x509v3',   'x509v3.h',                        'openssl'),
     ('freebsd/crypto/openssl/crypto/x509',     'x509_vfy.h',                      'openssl'),
     ('freebsd/crypto/openssl/crypto/buffer',   'buffer.h',                        'openssl'),
     ('freebsd/crypto/openssl/crypto/comp',     'comp.h',                          'openssl'),
     ('freebsd/crypto/openssl/crypto/asn1',     'asn1_mac.h',                      'openssl'),
     ('freebsd/crypto/openssl/crypto/pem',      '(pem|pem2).h',                    'openssl'),
     ('freebsd/crypto/openssl/crypto/rsa',      'rsa.h',                           'openssl'),
     ('freebsd/crypto/openssl/crypto/evp',      'evp.h',                           'openssl'),
     ('freebsd/crypto/openssl/crypto/ec',       'ec.h',                            'openssl'),
     ('freebsd/crypto/openssl/crypto/engine',   'engine.h',                        'openssl'),
     ('freebsd/crypto/openssl/crypto/pkcs7',    'pkcs7.h',                         'openssl'),
     ('freebsd/crypto/openssl/crypto/hmac',     'hmac.h',                          'openssl'),
     ('freebsd/crypto/openssl/crypto/pqueue',   'pqueue.h',                        'openssl'),
     ('freebsd/crypto/openssl/crypto/ocsp',     'ocsp.h',                          'openssl'),
     ('freebsd/crypto/openssl/crypto/rand',     'rand.h',                          'openssl'),
     ('freebsd/crypto/openssl/crypto/srp',      'srp.h',                           'openssl'),
     ('freebsd/crypto/openssl/crypto/dh',       'dh.h',                            'openssl'),
     ('freebsd/crypto/openssl/crypto/dso',      'dso.h',                           'openssl'),
     ('freebsd/crypto/openssl/crypto/krb5',     'krb5_asn.h',                      'openssl'),
     ('freebsd/crypto/openssl/crypto/cms',      'cms.h',                           'openssl'),
     ('freebsd/crypto/openssl/crypto/txt_db',   'txt_db.h',                        'openssl'),
     ('freebsd/crypto/openssl/crypto/ts',       'ts.h',                            'openssl'),
     ('freebsd/crypto/openssl/crypto/modes',    'modes.h',                         'openssl'),
     ('freebsd/crypto/openssl/crypto/pkcs12',   'pkcs12.h',                        'openssl'),
     ('freebsd/crypto/openssl/crypto/bf',       'blowfish.h',                      'openssl'),
     ('freebsd/crypto/openssl/crypto/cmac',     'cmac.h',                          'openssl'),
     ('freebsd/crypto/openssl/crypto/asn1',     '(asn1|asn1t).h',                  'openssl'),
     ('freebsd/crypto/openssl/crypto/camellia', 'camellia.h',                      'openssl'),
     ('freebsd/crypto/openssl/crypto/objects',  '(objects|obj_mac).h',             'openssl'),
     ('freebsd/crypto/openssl/crypto/conf',     '(conf|conf_api).h',               'openssl'),
     ('freebsd/crypto/openssl/crypto/des',      '(des|des_old).h',                 'openssl'),
     ('freebsd/crypto/openssl/crypto/ui',       '(ui_compat|ui).h',                'openssl'),
     ('freebsd/crypto/openssl/crypto/whrlpool', 'whrlpool.h',                      'openssl'),
     ('freebsd/crypto/openssl/crypto/stack',    '(stack|safestack).h',             'openssl'),
     ('freebsd/crypto/openssl/crypto',          '(opensslconf|opensslv|crypto).h', 'openssl'),
     ('freebsd/sys/rpc',                        '**/*.h',                          'rpc'),
     ('freebsd/sys/sys',                        '**/*.h',                          'sys'),
     ('freebsd/sys/vm',                         '**/*.h',                          'vm'),
     ('freebsd/sys/dev/mii',                    '**/*.h',                          'dev/mii'),
     ('linux/include',                          '**/*.h',                          ''),
     ('mDNSResponder/mDNSCore',                 'mDNSDebug.h',                     ''),
     ('mDNSResponder/mDNSCore',                 'mDNSEmbeddedAPI.h',               ''),
     ('mDNSResponder/mDNSShared',               'dns_sd.h',                        ''),
     ('mDNSResponder/mDNSPosix',                'mDNSPosix.h',                     '')]
}

#
# RTEMS
#
class rtems(builder.Module):

    def __init__(self, manager):
        super(rtems, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addRTEMSSourceFiles(
            [
                'local/bus_if.c',
                'local/cryptodev_if.c',
                'local/device_if.c',
                'local/ifdi_if.c',
                'local/miibus_if.c',
                'local/ofw_bus_if.c',
                'local/ofw_if.c',
                'local/pcib_if.c',
                'local/pci_if.c',
                'local/usb_if.c',
                'local/mmcbus_if.c',
                'local/mmcbr_if.c',
                'local/if_dwc_if.c',
                'local/gpio_if.c',
                'rtems/ipsec_get_policylen.c',
                'rtems/rtems-bsd-arp-processor.c',
                'rtems/rtems-bsd-allocator-domain-size.c',
                'rtems/rtems-bsd-get-allocator-domain-size.c',
                'rtems/rtems-bsd-get-ethernet-addr.c',
                'rtems/rtems-bsd-get-mac-address.c',
                'rtems/rtems-bsd-get-task-priority.c',
                'rtems/rtems-bsd-get-task-stack-size.c',
                'rtems/rtems-bsd-ifconfig.c',
                'rtems/rtems-bsd-ifconfig-lo0.c',
                'rtems/rtems-bsd-init-dhcp.c',
                'rtems/rtems-bsd-rc-conf-net.c',
                'rtems/rtems-bsd-rc-conf-pf.c',
                'rtems/rtems-bsd-rc-conf.c',
                'rtems/rtems-bsd-set-if-input.c',
                'rtems/rtems-bsd-shell-arp.c',
                'rtems/rtems-bsd-shell-ifconfig.c',
                'rtems/rtems-bsd-shell-netstat.c',
                'rtems/rtems-bsd-shell-pfctl.c',
                'rtems/rtems-bsd-shell-ping.c',
                'rtems/rtems-bsd-shell-route.c',
                'rtems/rtems-bsd-shell-stty.c',
                'rtems/rtems-bsd-shell-sysctl.c',
                'rtems/rtems-bsd-shell-tcpdump.c',
                'rtems/rtems-bsd-shell-vmstat.c',
                'rtems/rtems-bsd-shell-wlanstats.c',
                'rtems/rtems-bsd-syscall-api.c',
                'rtems/rtems-kernel-assert.c',
                'rtems/rtems-kernel-autoconf.c',
                'rtems/rtems-kernel-bus-dma.c',
                'rtems/rtems-kernel-bus-dma-mbuf.c',
                'rtems/rtems-kernel-bus-root.c',
                'rtems/rtems-kernel-cam.c',
                'rtems/rtems-kernel-chunk.c',
                'rtems/rtems-kernel-configintrhook.c',
                'rtems/rtems-kernel-delay.c',
                'rtems/rtems-kernel-get-file.c',
                'rtems/rtems-kernel-init.c',
                'rtems/rtems-kernel-irqs.c',
                'rtems/rtems-kernel-jail.c',
                'rtems/rtems-kernel-malloc.c',
                'rtems/rtems-kernel-mbuf.c',
                'rtems/rtems-kernel-mtxpool.c',
                'rtems/rtems-kernel-mutex.c',
                'rtems/rtems-kernel-muteximpl.c',
                'rtems/rtems-kernel-nexus.c',
                'rtems/rtems-kernel-page.c',
                'rtems/rtems-kernel-panic.c',
                'rtems/rtems-kernel-pci_bus.c',
                'rtems/rtems-kernel-pci_cfgreg.c',
                'rtems/rtems-kernel-program.c',
                'rtems/rtems-kernel-rwlock.c',
                'rtems/rtems-kernel-signal.c',
                'rtems/rtems-kernel-sx.c',
                'rtems/rtems-kernel-sysctlbyname.c',
                'rtems/rtems-kernel-sysctl.c',
                'rtems/rtems-kernel-sysctlnametomib.c',
                'rtems/rtems-kernel-timesupport.c',
                'rtems/rtems-kernel-termioskqueuepoll.c',
                'rtems/rtems-kernel-thread.c',
                'rtems/rtems-kernel-vprintf.c',
                'rtems/rtems-kernel-wpa-supplicant.c',
                'rtems/rtems-legacy-rtrequest.c',
                'rtems/rtems-legacy-newproc.c',
                'rtems/rtems-legacy-mii.c',
                'rtems/rtems-kvm.c',
                'rtems/rtems-program.c',
                'rtems/rtems-program-socket.c',
                'rtems/rtems-routes.c',
                'rtems/syslog.c',
                'ftpd/ftpd-service.c',
                'nfsclient/mount_prot_xdr.c',
                'nfsclient/nfs.c',
                'nfsclient/nfs_prot_xdr.c',
                'nfsclient/rpcio.c',
                'pppd/auth.c',
                'pppd/ccp.c',
                'pppd/chap.c',
                'pppd/chap_ms.c',
                'pppd/chat.c',
                'pppd/demand.c',
                'pppd/fsm.c',
                'pppd/ipcp.c',
                'pppd/lcp.c',
                'pppd/magic.c',
                'pppd/options.c',
                'pppd/rtemsmain.c',
                'pppd/rtemspppd.c',
                'pppd/sys-rtems.c',
                'pppd/upap.c',
                'pppd/utils.c',
                'sys/dev/input/touchscreen/tsc_lpc32xx.c',
                'sys/dev/usb/controller/ehci_mpc83xx.c',
                'sys/dev/usb/controller/ohci_lpc.c',
                'sys/dev/usb/controller/ohci_lpc32xx.c',
                'sys/dev/usb/controller/usb_otg_transceiver.c',
                'sys/dev/usb/controller/usb_otg_transceiver_dump.c',
                'sys/dev/smc/if_smc_nexus.c',
                'sys/dev/ffec/if_ffec_mcf548x.c',
                'sys/dev/atsam/if_atsam.c',
                'sys/dev/atsam/if_atsam_media.c',
                'sys/dev/dw_mmc/dw_mmc.c',
                'sys/fs/devfs/devfs_devs.c',
                'sys/net/if_ppp.c',
                'sys/net/ppp_tty.c',
                'telnetd/telnetd-service.c',
                'sys/dev/tsec/if_tsec_nexus.c',
            ],
            mm.generator['source']()
        )
        self.addFile(mm.generator['file']('rtems/rtems-kernel-kvm-symbols.c',
                                          mm.generator['rtems-path'](),
                                          mm.generator['no-convert'](),
                                          mm.generator['no-convert'](),
                                          mm.generator['kvm-symbols'](includes = 'rtemsbsd/rtems')))
        self.addFile(mm.generator['file']('lib/libc/net/nslexer.l',
                                          mm.generator['freebsd-path'](),
                                          mm.generator['convert'](),
                                          mm.generator['convert'](),
                                          mm.generator['lex']('_nsyy', 'nsparser.c')))
        self.addFile(mm.generator['file']('lib/libc/net/nsparser.y',
                                          mm.generator['freebsd-path'](),
                                          mm.generator['convert'](),
                                          mm.generator['convert'](),
                                          mm.generator['yacc']('_nsyy', 'nsparser.h')))


#
# Base
#
class base(builder.Module):

    def __init__(self, manager):
        super(base, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/bsm/audit.h',
                'sys/bsm/audit_kevents.h',
                'sys/fs/devfs/devfs_int.h',
                'sys/security/audit/audit.h',
                'sys/security/mac/mac_framework.h',
                'sys/sys/acl.h',
                'sys/sys/aio.h',
                'sys/sys/blist.h',
                'sys/sys/bufobj.h',
                'sys/sys/buf_ring.h',
                'sys/sys/_bus_dma.h',
                'sys/sys/bus_dma.h',
                'sys/sys/bus.h',
                'sys/sys/_callout.h',
                'sys/sys/callout.h',
                'sys/sys/capability.h',
                'sys/sys/caprights.h',
                'sys/sys/capsicum.h',
                'sys/sys/condvar.h',
                'sys/sys/conf.h',
                'sys/sys/counter.h',
                'sys/sys/cpu.h',
                'sys/sys/ctype.h',
                'sys/sys/domain.h',
                'sys/sys/eventhandler.h',
                'sys/sys/fail.h',
                'sys/sys/filedesc.h',
                'sys/sys/file.h',
                'sys/sys/firmware.h',
                'sys/sys/fnv_hash.h',
                'sys/sys/gpio.h',
                'sys/sys/gtaskqueue.h',
                'sys/sys/hash.h',
                'sys/sys/hhook.h',
                'sys/sys/interrupt.h',
                'sys/sys/jail.h',
                'sys/sys/kernel.h',
                'sys/sys/khelp.h',
                'sys/sys/kobj.h',
                'sys/sys/kthread.h',
                'sys/sys/ktr.h',
                'sys/sys/ktr_class.h',
                'sys/sys/libkern.h',
                'sys/sys/limits.h',
                'sys/sys/linker.h',
                'sys/sys/linker_set.h',
                'sys/sys/_lock.h',
                'sys/sys/_lockmgr.h',
                'sys/sys/lockmgr.h',
                'sys/sys/lock_profile.h',
                'sys/sys/lockstat.h',
                'sys/sys/loginclass.h',
                'sys/sys/mac.h',
                'sys/sys/malloc.h',
                'sys/sys/mbuf.h',
                'sys/sys/module.h',
                'sys/sys/module_khelp.h',
                'sys/sys/mount.h',
                'sys/sys/_mutex.h',
                'sys/sys/mutex.h',
                'sys/sys/_null.h',
                'sys/sys/nv.h',
                'sys/sys/osd.h',
                'sys/sys/pcpu.h',
                'sys/sys/_pctrie.h',
                'sys/sys/pipe.h',
                'sys/sys/priv.h',
                'sys/sys/proc.h',
                'sys/sys/protosw.h',
                'sys/sys/racct.h',
                'sys/sys/random.h',
                'sys/sys/reboot.h',
                'sys/sys/refcount.h',
                'sys/sys/resourcevar.h',
                'sys/sys/rman.h',
                'sys/sys/_rmlock.h',
                'sys/sys/rmlock.h',
                'sys/sys/rtprio.h',
                'sys/sys/runq.h',
                'sys/sys/_rwlock.h',
                'sys/sys/rwlock.h',
                'sys/sys/sbuf.h',
                'sys/sys/sdt.h',
                'sys/sys/selinfo.h',
                'sys/sys/_semaphore.h',
                'sys/sys/seq.h',
                'sys/sys/sf_buf.h',
                'sys/sys/sigio.h',
                'sys/sys/signalvar.h',
                'sys/sys/smp.h',
                'sys/sys/sleepqueue.h',
                'sys/sys/sockbuf.h',
                'sys/sys/socketvar.h',
                'sys/sys/sockopt.h',
                'sys/sys/stddef.h',
                'sys/sys/stdint.h',
                'sys/sys/_sx.h',
                'sys/sys/sx.h',
                'sys/sys/sysctl.h',
                'sys/sys/syslog.h',
                'sys/sys/sysproto.h',
                'sys/sys/systm.h',
                'sys/sys/_task.h',
                'sys/sys/taskqueue.h',
                'sys/sys/nlist_aout.h',
                'sys/rpc/netconfig.h',
                'sys/rpc/types.h',
                'sys/sys/tree.h',
                'sys/sys/ucred.h',
                'sys/sys/un.h',
                'sys/sys/unpcb.h',
                'sys/sys/_unrhdr.h',
                'sys/sys/uuid.h',
                'sys/sys/vmmeter.h',
                'sys/sys/vnode.h',
                'sys/vm/uma_dbg.h',
                'sys/vm/uma.h',
                'sys/vm/uma_int.h',
                'sys/vm/vm_extern.h',
                'sys/vm/vm.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/kern/init_main.c',
                'sys/kern/kern_condvar.c',
                'sys/kern/kern_conf.c',
                'sys/kern/kern_event.c',
                'sys/kern/kern_hhook.c',
                'sys/kern/kern_intr.c',
                'sys/kern/kern_khelp.c',
                'sys/kern/kern_linker.c',
                'sys/kern/kern_mbuf.c',
                'sys/kern/kern_mib.c',
                'sys/kern/kern_module.c',
                'sys/kern/kern_mtxpool.c',
                'sys/kern/kern_osd.c',
                'sys/kern/kern_synch.c',
                'sys/kern/kern_sysctl.c',
                'sys/kern/kern_time.c',
                'sys/kern/kern_timeout.c',
                'sys/kern/kern_uuid.c',
                'sys/kern/subr_blist.c',
                'sys/kern/subr_bufring.c',
                'sys/kern/subr_bus.c',
                'sys/kern/subr_counter.c',
                'sys/kern/subr_eventhandler.c',
                'sys/kern/subr_firmware.c',
                'sys/kern/subr_hash.c',
                'sys/kern/subr_hints.c',
                'sys/kern/subr_kobj.c',
                'sys/kern/subr_lock.c',
                'sys/kern/subr_module.c',
                'sys/kern/subr_pcpu.c',
                'sys/kern/subr_prf.c',
                'sys/kern/subr_rman.c',
                'sys/kern/subr_sbuf.c',
                'sys/kern/subr_sleepqueue.c',
                'sys/kern/subr_taskqueue.c',
                'sys/kern/subr_uio.c',
                'sys/kern/subr_unit.c',
                'sys/kern/sys_generic.c',
                'sys/kern/sys_pipe.c',
                'sys/kern/uipc_accf.c',
                'sys/kern/uipc_domain.c',
                'sys/kern/uipc_mbuf2.c',
                'sys/kern/uipc_mbuf.c',
                'sys/kern/uipc_mbufhash.c',
                'sys/kern/uipc_sockbuf.c',
                'sys/kern/uipc_socket.c',
                'sys/kern/uipc_usrreq.c',
                'sys/libkern/crc32.c',
                'sys/libkern/bcd.c',
                'sys/libkern/inet_ntoa.c',
                'sys/libkern/jenkins_hash.c',
                'sys/libkern/murmur3_32.c',
                'sys/libkern/random.c',
                'sys/fs/devfs/devfs_vnops.c',
                'sys/vm/uma_core.c',
                'sys/vm/uma_dbg.c',
            ],
            mm.generator['source']()
        )

#
# FDT
#
class fdt(builder.Module):

    def __init__(self, manager):
        super(fdt, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/sys/slicer.h',
                'sys/dev/fdt/fdt_common.h',
                'sys/dev/fdt/simplebus.h',
                'sys/dev/mii/mii_fdt.h',
                'sys/dev/ofw/ofw_bus.h',
                'sys/dev/ofw/ofw_bus_subr.h',
                'sys/dev/ofw/ofw_subr.h',
                'sys/dev/ofw/ofw_pci.h',
                'sys/dev/ofw/ofwvar.h',
                'sys/dev/ofw/openfirm.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/dev/fdt/simplebus.c',
                'sys/dev/fdt/fdt_common.c',
                'sys/dev/mii/mii_fdt.c',
                'sys/dev/ofw/ofwbus.c',
                'sys/dev/ofw/openfirm.c',
                'sys/dev/ofw/ofw_fdt.c',
                'sys/dev/ofw/ofw_bus_subr.c',
                'sys/dev/ofw/ofw_subr.c',
            ],
            mm.generator['source']()
        )
        self.addRTEMSSourceFiles(
            [
                'rtems/ofw_machdep.c',
            ],
            mm.generator['source']()
        )

#
# TTY
#
class tty(builder.Module):

    def __init__(self, manager):
        super(tty, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/sys/tty.h',
                'sys/sys/ttyqueue.h',
                'sys/sys/ttydisc.h',
                'sys/sys/ttydevsw.h',
                'sys/sys/ttyhook.h',
                'sys/sys/cons.h',
                'sys/sys/serial.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/kern/tty.c',
                'sys/kern/tty_inq.c',
                'sys/kern/tty_outq.c',
                'sys/kern/tty_ttydisc.c',
            ],
            mm.generator['source']()
        )
#        self.addRTEMSSourceFiles(
#            [
#                'rtems/ofw_machdep.c',
#            ],
#            mm.generator['source']()
#        )

#
# MMC
#
class mmc(builder.Module):

    def __init__(self, manager):
        super(mmc, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/arm/at91/at91_mcireg.h',
                'sys/arm/at91/at91_pdcreg.h',
                'sys/arm/at91/at91reg.h',
                'sys/arm/at91/at91var.h',
                'sys/dev/mmc/bridge.h',
                'sys/dev/mmc/mmcbrvar.h',
                'sys/dev/mmc/mmc_ioctl.h',
                'sys/dev/mmc/mmc_private.h',
                'sys/dev/mmc/mmcreg.h',
                'sys/dev/mmc/mmc_subr.h',
                'sys/dev/mmc/mmcvar.h',
                'sys/sys/watchdog.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/arm/at91/at91_mci.c',
                'sys/dev/mmc/mmc.c',
                'sys/dev/mmc/mmcsd.c',
                'sys/dev/mmc/mmc_subr.c',
            ],
            mm.generator['source']()
        )

#
# MMC
#
class mmc_ti(builder.Module):

    def __init__(self, manager):
        super(mmc_ti, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/arm/ti/ti_cpuid.h',
                'sys/arm/ti/ti_prcm.h',
                'sys/arm/ti/ti_hwmods.h',
                'sys/dev/sdhci/sdhci.h',
                'sys/dev/sdhci/sdhci_fdt_gpio.h',
                'sys/dev/gpio/gpiobusvar.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/dev/sdhci/sdhci.c',
                'sys/arm/ti/ti_sdhci.c',
                'sys/arm/ti/ti_hwmods.c',
                'sys/dev/sdhci/sdhci_fdt_gpio.c',
                'sys/dev/gpio/ofw_gpiobus.c',
                'sys/dev/gpio/gpiobus.c',
            ],
            mm.generator['source']()
        )
        self.addRTEMSSourceFiles(
            [
                'local/sdhci_if.c',
                'local/gpiobus_if.c',
            ],
            mm.generator['source']()
        )

#
# Input
#
class dev_input(builder.Module):

    def __init__(self, manager):
        super(dev_input, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/sys/kbio.h',
                'sys/dev/kbd/kbdreg.h',
                'sys/dev/kbd/kbdtables.h',
                'sys/sys/mouse.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/dev/kbd/kbd.c',
            ],
            mm.generator['source']()
        )

#
# EVDEV
#
class evdev(builder.Module):

    def __init__(self, manager):
        super(evdev, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/dev/evdev/evdev.h',
                'sys/dev/evdev/evdev_private.h',
                'sys/dev/evdev/input.h',
                'sys/dev/evdev/input-event-codes.h',
                'sys/dev/evdev/uinput.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/dev/evdev/cdev.c',
                'sys/dev/evdev/evdev.c',
                'sys/dev/evdev/evdev_mt.c',
                'sys/dev/evdev/evdev_utils.c',
                'sys/dev/evdev/uinput.c',
            ],
            mm.generator['source']()
        )

#
# USB
#
class dev_usb(builder.Module):

    def __init__(self, manager):
        super(dev_usb, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/dev/usb/ufm_ioctl.h',
                'sys/dev/usb/usb_busdma.h',
                'sys/dev/usb/usb_bus.h',
                'sys/dev/usb/usb_cdc.h',
                'sys/dev/usb/usb_controller.h',
                'sys/dev/usb/usb_core.h',
                'sys/dev/usb/usb_debug.h',
                'sys/dev/usb/usb_dev.h',
                'sys/dev/usb/usb_device.h',
                'sys/dev/usb/usbdi.h',
                'sys/dev/usb/usbdi_util.h',
                'sys/dev/usb/usb_dynamic.h',
                'sys/dev/usb/usb_endian.h',
                'sys/dev/usb/usb_freebsd.h',
                'sys/dev/usb/usb_generic.h',
                'sys/dev/usb/usb.h',
                'sys/dev/usb/usbhid.h',
                'sys/dev/usb/usb_hub.h',
                'sys/dev/usb/usb_ioctl.h',
                'sys/dev/usb/usb_mbuf.h',
                'sys/dev/usb/usb_msctest.h',
                'sys/dev/usb/usb_pf.h',
                'sys/dev/usb/usb_process.h',
                'sys/dev/usb/usb_request.h',
                'sys/dev/usb/usb_transfer.h',
                'sys/dev/usb/usb_util.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/dev/usb/usb_busdma.c',
                'sys/dev/usb/usb_core.c',
                'sys/dev/usb/usb_debug.c',
                'sys/dev/usb/usb_dev.c',
                'sys/dev/usb/usb_device.c',
                'sys/dev/usb/usb_dynamic.c',
                'sys/dev/usb/usb_error.c',
                'sys/dev/usb/usb_generic.c',
                'sys/dev/usb/usb_handle_request.c',
                'sys/dev/usb/usb_hid.c',
                'sys/dev/usb/usb_hub.c',
                'sys/dev/usb/usb_lookup.c',
                'sys/dev/usb/usb_mbuf.c',
                'sys/dev/usb/usb_msctest.c',
                'sys/dev/usb/usb_parse.c',
                'sys/dev/usb/usb_process.c',
                'sys/dev/usb/usb_request.c',
                'sys/dev/usb/usb_transfer.c',
                'sys/dev/usb/usb_util.c',
            ],
            mm.generator['source']()
        )

#
# USB Controller.
#
class dev_usb_controller(builder.Module):

    def __init__(self, manager):
        super(dev_usb_controller, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addDependency(mm['dev_usb'])
        self.addKernelSpaceHeaderFiles(
            [
                'sys/dev/usb/controller/ohci.h',
                'sys/dev/usb/controller/ohcireg.h',
                'sys/dev/usb/controller/ehci.h',
                'sys/dev/usb/controller/ehcireg.h',
                'sys/dev/usb/controller/uhcireg.h',
                'sys/dev/usb/controller/xhcireg.h',
                'sys/dev/usb/controller/dwc_otg_fdt.h',
                'sys/dev/usb/controller/dwc_otg.h',
                'sys/dev/usb/controller/dwc_otgreg.h',
                'sys/dev/usb/controller/saf1761_otg.h',
                'sys/dev/usb/controller/saf1761_otg_reg.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/dev/usb/controller/ohci.c',
                'sys/dev/usb/controller/ehci.c',
                'sys/dev/usb/controller/usb_controller.c',
                'sys/dev/usb/controller/dwc_otg.c',
                'sys/dev/usb/controller/dwc_otg_fdt.c',
                'sys/dev/usb/controller/saf1761_otg.c',
                'sys/dev/usb/controller/saf1761_otg_fdt.c',
            ],
            mm.generator['source']()
        )

#
# USB Input
#
class dev_usb_input(builder.Module):

    def __init__(self, manager):
        super(dev_usb_input, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addDependency(mm['dev_usb'])
        self.addKernelSpaceHeaderFiles(
            [
                'sys/dev/usb/input/usb_rdesc.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/dev/usb/input/atp.c',
                'sys/dev/usb/input/uep.c',
                'sys/dev/usb/input/uhid.c',
                'sys/dev/usb/input/ukbd.c',
                'sys/dev/usb/input/ums.c',
                'sys/dev/usb/input/wsp.c',
            ],
            mm.generator['source']()
        )

#
# USB Networking
#
class dev_usb_net(builder.Module):

    def __init__(self, manager):
        super(dev_usb_net, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addDependency(mm['dev_usb'])
        self.addKernelSpaceHeaderFiles(
            [
                'sys/dev/usb/net/if_auereg.h',
                'sys/dev/usb/net/if_axereg.h',
                'sys/dev/usb/net/if_axgereg.h',
                'sys/dev/usb/net/if_cdcereg.h',
                'sys/dev/usb/net/if_cuereg.h',
                'sys/dev/usb/net/if_iphethvar.h',
                'sys/dev/usb/net/if_kuefw.h',
                'sys/dev/usb/net/if_kuereg.h',
                'sys/dev/usb/net/if_mosreg.h',
                'sys/dev/usb/net/if_ruereg.h',
                'sys/dev/usb/net/if_smscreg.h',
                'sys/dev/usb/net/if_udavreg.h',
                'sys/dev/usb/net/if_urereg.h',
                'sys/dev/usb/net/ruephyreg.h',
                'sys/dev/usb/net/usb_ethernet.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/dev/usb/net/if_aue.c',
                'sys/dev/usb/net/if_axe.c',
                'sys/dev/usb/net/if_axge.c',
                'sys/dev/usb/net/if_cdce.c',
                'sys/dev/usb/net/if_cue.c',
                'sys/dev/usb/net/if_ipheth.c',
                'sys/dev/usb/net/if_kue.c',
                'sys/dev/usb/net/if_mos.c',
                'sys/dev/usb/net/if_rue.c',
                'sys/dev/usb/net/if_smsc.c',
                'sys/dev/usb/net/if_udav.c',
                'sys/dev/usb/net/if_ure.c',
                'sys/dev/usb/net/ruephy.c',
                'sys/dev/usb/net/usb_ethernet.c',
            ],
            mm.generator['source']()
        )

#
# USB Quirks
#
class dev_usb_quirk(builder.Module):

    def __init__(self, manager):
        super(dev_usb_quirk, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addDependency(mm['dev_usb'])
        self.addKernelSpaceHeaderFiles(
            [
                'sys/dev/usb/quirk/usb_quirk.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/dev/usb/quirk/usb_quirk.c',
            ],
            mm.generator['source']()
        )

#
# USB Serial
#
class dev_usb_serial(builder.Module):

    def __init__(self, manager):
        super(dev_usb_serial, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addDependency(mm['dev_usb'])
        self.addKernelSpaceHeaderFiles(
            [
                'sys/dev/usb/serial/uftdi_reg.h',
                'sys/dev/usb/serial/umcs.h',
                'sys/dev/usb/serial/usb_serial.h',
                'sys/dev/usb/uftdiio.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/dev/usb/serial/u3g.c',
                'sys/dev/usb/serial/uark.c',
                'sys/dev/usb/serial/ubsa.c',
                'sys/dev/usb/serial/ubser.c',
                'sys/dev/usb/serial/uchcom.c',
                'sys/dev/usb/serial/ucycom.c',
                'sys/dev/usb/serial/ufoma.c',
                'sys/dev/usb/serial/uftdi.c',
                'sys/dev/usb/serial/ugensa.c',
                'sys/dev/usb/serial/uipaq.c',
                'sys/dev/usb/serial/ulpt.c',
                'sys/dev/usb/serial/umcs.c',
                'sys/dev/usb/serial/umct.c',
                'sys/dev/usb/serial/umodem.c',
                'sys/dev/usb/serial/umoscom.c',
                'sys/dev/usb/serial/uplcom.c',
                'sys/dev/usb/serial/usb_serial.c',
                'sys/dev/usb/serial/uslcom.c',
                'sys/dev/usb/serial/uvisor.c',
                'sys/dev/usb/serial/uvscom.c',
            ],
            mm.generator['source']()
        )

#
# USB Storage
#
class dev_usb_storage(builder.Module):

    def __init__(self, manager):
        super(dev_usb_storage, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addDependency(mm['dev_usb'])
        self.addKernelSpaceSourceFiles(
            [
                'sys/dev/usb/storage/umass.c',
            ],
            mm.generator['source']()
        )

#
# BBB USB
#
class dev_usb_controller_bbb(builder.Module):

    def __init__(self, manager):
        super(dev_usb_controller_bbb, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addDependency(mm['dev_usb'])
        self.addKernelSpaceHeaderFiles(
            [
                'sys/arm/ti/ti_cpuid.h',
                'sys/arm/ti/ti_prcm.h',
                'sys/arm/ti/ti_scm.h',
                'sys/arm/ti/tivar.h',
                'sys/arm/ti/am335x/am335x_scm.h',
                'sys/dev/usb/controller/musb_otg.h',
                'sys/sys/timeet.h',
                'sys/sys/watchdog.h',
                'sys/dev/fdt/fdt_pinctrl.h',

        ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/arm/ti/ti_scm.c',
                'sys/arm/ti/am335x/am335x_prcm.c',
                'sys/arm/ti/am335x/am335x_usbss.c',
                'sys/arm/ti/ti_prcm.c',
                'sys/arm/ti/am335x/am335x_musb.c',
                'sys/dev/usb/controller/musb_otg.c',
            ],
            mm.generator['source']()
        )

#
# USB WLAN
#
class dev_usb_wlan(builder.Module):

    def __init__(self, manager):
        super(dev_usb_wlan, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addDependency(mm['dev_usb'])
        self.addKernelSpaceHeaderFiles(
            [
                'sys/dev/usb/wlan/if_rsureg.h',
                'sys/dev/usb/wlan/if_rumfw.h',
                'sys/dev/usb/wlan/if_rumreg.h',
                'sys/dev/usb/wlan/if_rumvar.h',
                'sys/dev/usb/wlan/if_runreg.h',
                'sys/dev/usb/wlan/if_runvar.h',
                'sys/dev/usb/wlan/if_uathreg.h',
                'sys/dev/usb/wlan/if_uathvar.h',
                'sys/dev/usb/wlan/if_upgtvar.h',
                'sys/dev/usb/wlan/if_uralreg.h',
                'sys/dev/usb/wlan/if_uralvar.h',
                'sys/dev/usb/wlan/if_urtwreg.h',
                'sys/dev/usb/wlan/if_urtwvar.h',
                'sys/dev/usb/wlan/if_zydfw.h',
                'sys/dev/usb/wlan/if_zydreg.h',
            ]
        )
        self.addRTEMSSourceFiles(
            [
                'local/runfw.c',
            ],
            mm.generator['source']()
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/dev/usb/wlan/if_rsu.c',
                'sys/dev/usb/wlan/if_rum.c',
                'sys/dev/usb/wlan/if_run.c',
                'sys/dev/usb/wlan/if_uath.c',
                'sys/dev/usb/wlan/if_upgt.c',
                'sys/dev/usb/wlan/if_ural.c',
                'sys/dev/usb/wlan/if_urtw.c',
                'sys/dev/usb/wlan/if_zyd.c',
            ],
            mm.generator['source']()
        )

#
# WLAN RTWN
#
class dev_wlan_rtwn(builder.Module):

    def __init__(self, manager):
        super(dev_wlan_rtwn, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addDependency(mm['dev_usb'])
        self.addKernelSpaceHeaderFiles(
            [
                'sys/dev/rtwn/if_rtwn_beacon.h',
                'sys/dev/rtwn/if_rtwn_calib.h',
                'sys/dev/rtwn/if_rtwn_cam.h',
                'sys/dev/rtwn/if_rtwn_debug.h',
                'sys/dev/rtwn/if_rtwn_efuse.h',
                'sys/dev/rtwn/if_rtwn_fw.h',
                'sys/dev/rtwn/if_rtwn_nop.h',
                'sys/dev/rtwn/if_rtwn_ridx.h',
                'sys/dev/rtwn/if_rtwn_rx.h',
                'sys/dev/rtwn/if_rtwn_task.h',
                'sys/dev/rtwn/if_rtwn_tx.h',
                'sys/dev/rtwn/if_rtwnreg.h',
                'sys/dev/rtwn/if_rtwnvar.h',
                'sys/dev/rtwn/pci/rtwn_pci_attach.h',
                'sys/dev/rtwn/pci/rtwn_pci_reg.h',
                'sys/dev/rtwn/pci/rtwn_pci_rx.h',
                'sys/dev/rtwn/pci/rtwn_pci_tx.h',
                'sys/dev/rtwn/pci/rtwn_pci_var.h',
                'sys/dev/rtwn/rtl8188e/r88e.h',
                'sys/dev/rtwn/rtl8188e/r88e_fw_cmd.h',
                'sys/dev/rtwn/rtl8188e/r88e_priv.h',
                'sys/dev/rtwn/rtl8188e/r88e_reg.h',
                'sys/dev/rtwn/rtl8188e/r88e_rom_defs.h',
                'sys/dev/rtwn/rtl8188e/r88e_rom_image.h',
                'sys/dev/rtwn/rtl8188e/r88e_rx_desc.h',
                'sys/dev/rtwn/rtl8188e/r88e_tx_desc.h',
                'sys/dev/rtwn/rtl8188e/usb/r88eu.h',
                'sys/dev/rtwn/rtl8188e/usb/r88eu_reg.h',
                'sys/dev/rtwn/rtl8192c/pci/r92ce.h',
                'sys/dev/rtwn/rtl8192c/pci/r92ce_priv.h',
                'sys/dev/rtwn/rtl8192c/pci/r92ce_reg.h',
                'sys/dev/rtwn/rtl8192c/pci/r92ce_rx_desc.h',
                'sys/dev/rtwn/rtl8192c/pci/r92ce_tx_desc.h',
                'sys/dev/rtwn/rtl8192c/r92c.h',
                'sys/dev/rtwn/rtl8192c/r92c_fw_cmd.h',
                'sys/dev/rtwn/rtl8192c/r92c_priv.h',
                'sys/dev/rtwn/rtl8192c/r92c_reg.h',
                'sys/dev/rtwn/rtl8192c/r92c_rom_defs.h',
                'sys/dev/rtwn/rtl8192c/r92c_rom_image.h',
                'sys/dev/rtwn/rtl8192c/r92c_rx_desc.h',
                'sys/dev/rtwn/rtl8192c/r92c_tx_desc.h',
                'sys/dev/rtwn/rtl8192c/r92c_var.h',
                'sys/dev/rtwn/rtl8192c/usb/r92cu.h',
                'sys/dev/rtwn/rtl8192c/usb/r92cu_priv.h',
                'sys/dev/rtwn/rtl8192c/usb/r92cu_reg.h',
                'sys/dev/rtwn/rtl8192c/usb/r92cu_tx_desc.h',
                'sys/dev/rtwn/rtl8192e/r92e.h',
                'sys/dev/rtwn/rtl8192e/r92e_priv.h',
                'sys/dev/rtwn/rtl8192e/r92e_reg.h',
                'sys/dev/rtwn/rtl8192e/r92e_rom_defs.h',
                'sys/dev/rtwn/rtl8192e/r92e_rom_image.h',
                'sys/dev/rtwn/rtl8192e/r92e_var.h',
                'sys/dev/rtwn/rtl8192e/usb/r92eu.h',
                'sys/dev/rtwn/rtl8192e/usb/r92eu_reg.h',
                'sys/dev/rtwn/rtl8812a/r12a.h',
                'sys/dev/rtwn/rtl8812a/r12a_fw_cmd.h',
                'sys/dev/rtwn/rtl8812a/r12a_priv.h',
                'sys/dev/rtwn/rtl8812a/r12a_reg.h',
                'sys/dev/rtwn/rtl8812a/r12a_rom_defs.h',
                'sys/dev/rtwn/rtl8812a/r12a_rom_image.h',
                'sys/dev/rtwn/rtl8812a/r12a_rx_desc.h',
                'sys/dev/rtwn/rtl8812a/r12a_tx_desc.h',
                'sys/dev/rtwn/rtl8812a/r12a_var.h',
                'sys/dev/rtwn/rtl8812a/usb/r12au.h',
                'sys/dev/rtwn/rtl8812a/usb/r12au_reg.h',
                'sys/dev/rtwn/rtl8812a/usb/r12au_tx_desc.h',
                'sys/dev/rtwn/rtl8821a/r21a.h',
                'sys/dev/rtwn/rtl8821a/r21a_priv.h',
                'sys/dev/rtwn/rtl8821a/r21a_reg.h',
                'sys/dev/rtwn/rtl8821a/usb/r21au.h',
                'sys/dev/rtwn/rtl8821a/usb/r21au_reg.h',
                'sys/dev/rtwn/usb/rtwn_usb_attach.h',
                'sys/dev/rtwn/usb/rtwn_usb_ep.h',
                'sys/dev/rtwn/usb/rtwn_usb_reg.h',
                'sys/dev/rtwn/usb/rtwn_usb_rx.h',
                'sys/dev/rtwn/usb/rtwn_usb_tx.h',
                'sys/dev/rtwn/usb/rtwn_usb_var.h',
            ]
        )
        self.addRTEMSSourceFiles(
            [
                'local/rtwn-rtl8192cfwT.c',
                'local/rtwn-rtl8188eufw.c',
            ],
            mm.generator['source']()
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/dev/rtwn/if_rtwn.c',
                'sys/dev/rtwn/if_rtwn_beacon.c',
                'sys/dev/rtwn/if_rtwn_calib.c',
                'sys/dev/rtwn/if_rtwn_cam.c',
                'sys/dev/rtwn/if_rtwn_efuse.c',
                'sys/dev/rtwn/if_rtwn_fw.c',
                'sys/dev/rtwn/if_rtwn_rx.c',
                'sys/dev/rtwn/if_rtwn_task.c',
                'sys/dev/rtwn/if_rtwn_tx.c',
                'sys/dev/rtwn/pci/rtwn_pci_attach.c',
                'sys/dev/rtwn/pci/rtwn_pci_reg.c',
                'sys/dev/rtwn/pci/rtwn_pci_rx.c',
                'sys/dev/rtwn/pci/rtwn_pci_tx.c',
                'sys/dev/rtwn/rtl8188e/r88e_beacon.c',
                'sys/dev/rtwn/rtl8188e/r88e_calib.c',
                'sys/dev/rtwn/rtl8188e/r88e_chan.c',
                'sys/dev/rtwn/rtl8188e/r88e_fw.c',
                'sys/dev/rtwn/rtl8188e/r88e_init.c',
                'sys/dev/rtwn/rtl8188e/r88e_led.c',
                'sys/dev/rtwn/rtl8188e/r88e_rf.c',
                'sys/dev/rtwn/rtl8188e/r88e_rom.c',
                'sys/dev/rtwn/rtl8188e/r88e_rx.c',
                'sys/dev/rtwn/rtl8188e/r88e_tx.c',
                'sys/dev/rtwn/rtl8188e/usb/r88eu_attach.c',
                'sys/dev/rtwn/rtl8188e/usb/r88eu_init.c',
                'sys/dev/rtwn/rtl8188e/usb/r88eu_rx.c',
                'sys/dev/rtwn/rtl8192c/pci/r92ce_attach.c',
                'sys/dev/rtwn/rtl8192c/pci/r92ce_calib.c',
                'sys/dev/rtwn/rtl8192c/pci/r92ce_fw.c',
                'sys/dev/rtwn/rtl8192c/pci/r92ce_init.c',
                'sys/dev/rtwn/rtl8192c/pci/r92ce_led.c',
                'sys/dev/rtwn/rtl8192c/pci/r92ce_rx.c',
                'sys/dev/rtwn/rtl8192c/pci/r92ce_tx.c',
                'sys/dev/rtwn/rtl8192c/r92c_attach.c',
                'sys/dev/rtwn/rtl8192c/r92c_beacon.c',
                'sys/dev/rtwn/rtl8192c/r92c_calib.c',
                'sys/dev/rtwn/rtl8192c/r92c_chan.c',
                'sys/dev/rtwn/rtl8192c/r92c_fw.c',
                'sys/dev/rtwn/rtl8192c/r92c_init.c',
                'sys/dev/rtwn/rtl8192c/r92c_llt.c',
                'sys/dev/rtwn/rtl8192c/r92c_rf.c',
                'sys/dev/rtwn/rtl8192c/r92c_rom.c',
                'sys/dev/rtwn/rtl8192c/r92c_rx.c',
                'sys/dev/rtwn/rtl8192c/r92c_tx.c',
                'sys/dev/rtwn/rtl8192c/usb/r92cu_attach.c',
                'sys/dev/rtwn/rtl8192c/usb/r92cu_init.c',
                'sys/dev/rtwn/rtl8192c/usb/r92cu_led.c',
                'sys/dev/rtwn/rtl8192c/usb/r92cu_rx.c',
                'sys/dev/rtwn/rtl8192c/usb/r92cu_tx.c',
                'sys/dev/rtwn/rtl8192e/r92e_chan.c',
                'sys/dev/rtwn/rtl8192e/r92e_fw.c',
                'sys/dev/rtwn/rtl8192e/r92e_init.c',
                'sys/dev/rtwn/rtl8192e/r92e_led.c',
                'sys/dev/rtwn/rtl8192e/r92e_rf.c',
                'sys/dev/rtwn/rtl8192e/r92e_rom.c',
                'sys/dev/rtwn/rtl8192e/r92e_rx.c',
                'sys/dev/rtwn/rtl8192e/usb/r92eu_attach.c',
                'sys/dev/rtwn/rtl8192e/usb/r92eu_init.c',
                'sys/dev/rtwn/rtl8812a/r12a_beacon.c',
                'sys/dev/rtwn/rtl8812a/r12a_calib.c',
                'sys/dev/rtwn/rtl8812a/r12a_caps.c',
                'sys/dev/rtwn/rtl8812a/r12a_chan.c',
                'sys/dev/rtwn/rtl8812a/r12a_fw.c',
                'sys/dev/rtwn/rtl8812a/r12a_init.c',
                'sys/dev/rtwn/rtl8812a/r12a_led.c',
                'sys/dev/rtwn/rtl8812a/r12a_rf.c',
                'sys/dev/rtwn/rtl8812a/r12a_rom.c',
                'sys/dev/rtwn/rtl8812a/r12a_rx.c',
                'sys/dev/rtwn/rtl8812a/r12a_tx.c',
                'sys/dev/rtwn/rtl8812a/usb/r12au_attach.c',
                'sys/dev/rtwn/rtl8812a/usb/r12au_init.c',
                'sys/dev/rtwn/rtl8812a/usb/r12au_rx.c',
                'sys/dev/rtwn/rtl8812a/usb/r12au_tx.c',
                'sys/dev/rtwn/rtl8821a/r21a_beacon.c',
                'sys/dev/rtwn/rtl8821a/r21a_calib.c',
                'sys/dev/rtwn/rtl8821a/r21a_chan.c',
                'sys/dev/rtwn/rtl8821a/r21a_fw.c',
                'sys/dev/rtwn/rtl8821a/r21a_init.c',
                'sys/dev/rtwn/rtl8821a/r21a_led.c',
                'sys/dev/rtwn/rtl8821a/r21a_rom.c',
                'sys/dev/rtwn/rtl8821a/r21a_rx.c',
                'sys/dev/rtwn/rtl8821a/usb/r21au_attach.c',
                'sys/dev/rtwn/rtl8821a/usb/r21au_dfs.c',
                'sys/dev/rtwn/rtl8821a/usb/r21au_init.c',
                'sys/dev/rtwn/usb/rtwn_usb_attach.c',
                'sys/dev/rtwn/usb/rtwn_usb_ep.c',
                'sys/dev/rtwn/usb/rtwn_usb_reg.c',
                'sys/dev/rtwn/usb/rtwn_usb_rx.c',
                'sys/dev/rtwn/usb/rtwn_usb_tx.c',
            ],
            mm.generator['source']()
        )

#
# CAM
#
class cam(builder.Module):

    def __init__(self, manager):
        super(cam, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/cam/ata/ata_all.h',
                'sys/cam/cam_ccb.h',
                'sys/cam/cam_debug.h',
                'sys/cam/cam.h',
                'sys/cam/cam_periph.h',
                'sys/cam/cam_sim.h',
                'sys/cam/cam_xpt.h',
                'sys/cam/cam_xpt_sim.h',
                'sys/cam/mmc/mmc_all.h',
                'sys/cam/mmc/mmc_bus.h',
                'sys/cam/mmc/mmc.h',
                'sys/cam/nvme/nvme_all.h',
                'sys/cam/scsi/scsi_all.h',
                'sys/cam/scsi/scsi_da.h',
                'sys/dev/nvme/nvme.h',
                'sys/sys/ata.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/cam/cam.c',
                'sys/cam/scsi/scsi_all.c',
            ],
            mm.generator['source']()
        )

#
# Networking Devices
#
class dev_net(builder.Module):

    def __init__(self, manager):
        super(dev_net, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/arm/lpc/if_lpereg.h',
                'sys/arm/lpc/lpcreg.h',
                'sys/arm/lpc/lpcvar.h',
                'sys/arm/xilinx/zy7_slcr.h',
                'sys/dev/cadence/if_cgem_hw.h',
                'sys/dev/dwc/if_dwc.h',
                'sys/dev/dwc/if_dwcvar.h',
                'sys/dev/ffec/if_ffecreg.h',
                'sys/dev/led/led.h',
                'sys/dev/mii/brgphyreg.h',
                'sys/dev/mii/e1000phyreg.h',
                'sys/dev/mii/icsphyreg.h',
                'sys/dev/mii/mii_bitbang.h',
                'sys/dev/mii/mii_fdt.h',
                'sys/dev/mii/mii.h',
                'sys/dev/mii/miivar.h',
                'sys/dev/mii/rgephyreg.h',
                'sys/dev/ofw/openfirm.h',
                'sys/dev/tsec/if_tsec.h',
                'sys/dev/tsec/if_tsecreg.h',
                'sys/net/bpf.h',
                'sys/net/dlt.h',
                'sys/net/ethernet.h',
                'sys/net/if_arp.h',
                'sys/net/if_dl.h',
                'sys/net/iflib.h',
                'sys/net/if_media.h',
                'sys/net/ifq.h',
                'sys/net/if_types.h',
                'sys/net/if_var.h',
                'sys/net/vnet.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/dev/ffec/if_ffec.c',
                'sys/dev/mii/mii.c',
                'sys/dev/mii/mii_bitbang.c',
                'sys/dev/mii/mii_physubr.c',
                'sys/dev/mii/icsphy.c',
                'sys/dev/mii/e1000phy.c',
                'sys/dev/mii/brgphy.c',
                'sys/dev/mii/micphy.c',
                'sys/dev/mii/rgephy.c',
                'sys/dev/mii/ukphy.c',
                'sys/dev/mii/ukphy_subr.c',
                'sys/dev/tsec/if_tsec.c',
                'sys/dev/tsec/if_tsec_fdt.c',
                'sys/dev/cadence/if_cgem.c',
                'sys/dev/dwc/if_dwc.c',
                'sys/arm/xilinx/zy7_slcr.c',
                'sys/arm/lpc/lpc_pwr.c',
                'sys/arm/lpc/if_lpe.c',
            ],
            mm.generator['source']()
        )

#
# Network Interface Controllers (NIC)
#
class dev_nic(builder.Module):

    def __init__(self, manager):
        super(dev_nic, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/sys/pciio.h',
                'sys/sys/eventvar.h',
                'sys/sys/kenv.h',
                'sys/isa/isavar.h',
                'sys/isa/pnpvar.h',
                'sys/sys/buf.h',
                'sys/sys/mqueue.h',
                'sys/sys/user.h',
            ]
        )
        self.addCPUDependentFreeBSDHeaderFiles(
            [
                'sys/arm/include/cpufunc.h',
                'sys/i386/include/specialreg.h',
                'sys/i386/include/md_var.h',
                'sys/i386/include/intr_machdep.h',
                'sys/i386/include/cpufunc.h',
                'sys/mips/include/cpufunc.h',
                'sys/mips/include/cpuregs.h',
                'sys/powerpc/include/cpufunc.h',
                'sys/powerpc/include/intr_machdep.h',
                'sys/powerpc/include/psl.h',
                'sys/powerpc/include/spr.h',
                'sys/sparc64/include/cpufunc.h',
                'sys/sparc64/include/asi.h',
                'sys/sparc64/include/pstate.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/netinet/tcp_hostcache.c',
                'sys/dev/led/led.c',
            ],
            mm.generator['source']()
        )

#
# NIC RE
#
class dev_nic_re(builder.Module):

    def __init__(self, manager):
        super(dev_nic_re, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/dev/rl/if_rlreg.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/dev/re/if_re.c',
            ],
            mm.generator['source']()
        )

#
# NIC FXP
#
class dev_nic_fxp(builder.Module):

    def __init__(self, manager):
        super(dev_nic_fxp, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/dev/fxp/if_fxpreg.h',
                'sys/dev/fxp/if_fxpvar.h',
                'sys/dev/fxp/rcvbundl.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/dev/fxp/if_fxp.c',
            ],
            mm.generator['source']()
        )

#
# NIC E1000
#
class dev_nic_e1000(builder.Module):

    def __init__(self, manager):
        super(dev_nic_e1000, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/dev/e1000/e1000_80003es2lan.h',
                'sys/dev/e1000/e1000_82541.h',
                'sys/dev/e1000/e1000_82543.h',
                'sys/dev/e1000/e1000_82571.h',
                'sys/dev/e1000/e1000_82575.h',
                'sys/dev/e1000/e1000_api.h',
                'sys/dev/e1000/e1000_defines.h',
                'sys/dev/e1000/e1000_hw.h',
                'sys/dev/e1000/e1000_i210.h',
                'sys/dev/e1000/e1000_ich8lan.h',
                'sys/dev/e1000/e1000_mac.h',
                'sys/dev/e1000/e1000_manage.h',
                'sys/dev/e1000/e1000_mbx.h',
                'sys/dev/e1000/e1000_nvm.h',
                'sys/dev/e1000/e1000_osdep.h',
                'sys/dev/e1000/e1000_phy.h',
                'sys/dev/e1000/e1000_regs.h',
                'sys/dev/e1000/e1000_vf.h',
                'sys/dev/e1000/if_em.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/dev/e1000/e1000_80003es2lan.c',
                'sys/dev/e1000/e1000_82540.c',
                'sys/dev/e1000/e1000_82541.c',
                'sys/dev/e1000/e1000_82542.c',
                'sys/dev/e1000/e1000_82543.c',
                'sys/dev/e1000/e1000_82571.c',
                'sys/dev/e1000/e1000_82575.c',
                'sys/dev/e1000/e1000_api.c',
                'sys/dev/e1000/e1000_i210.c',
                'sys/dev/e1000/e1000_ich8lan.c',
                'sys/dev/e1000/e1000_mac.c',
                'sys/dev/e1000/e1000_manage.c',
                'sys/dev/e1000/e1000_mbx.c',
                'sys/dev/e1000/e1000_nvm.c',
                'sys/dev/e1000/e1000_osdep.c',
                'sys/dev/e1000/e1000_phy.c',
                'sys/dev/e1000/e1000_vf.c',
                'sys/dev/e1000/em_txrx.c',
                'sys/dev/e1000/if_em.c',
                'sys/dev/e1000/igb_txrx.c',
            ],
            mm.generator['source']()
        )

#
# DEC Tulip aka Intel 21143
#
class dev_nic_dc(builder.Module):

    def __init__(self, manager):
        super(dev_nic_dc, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/dev/dc/if_dcreg.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/dev/dc/dcphy.c',
                'sys/dev/dc/if_dc.c',
                'sys/dev/dc/pnphy.c',
            ],
            mm.generator['source']()
        )

#
# SMC9111x
#
class dev_nic_smc(builder.Module):

    def __init__(self, manager):
        super(dev_nic_smc, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/dev/smc/if_smcreg.h',
                'sys/dev/smc/if_smcvar.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/dev/smc/if_smc.c',
            ],
            mm.generator['source']()
        )

#
# Broadcomm BCE, BFE, BGE - MII is intertwined
#
class dev_nic_broadcomm(builder.Module):

    def __init__(self, manager):
        super(dev_nic_broadcomm, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/dev/bce/if_bcefw.h',
                'sys/dev/bce/if_bcereg.h',
                'sys/dev/bfe/if_bfereg.h',
                'sys/dev/bge/if_bgereg.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/dev/bce/if_bce.c',
                'sys/dev/bfe/if_bfe.c',
                'sys/dev/bge/if_bge.c',
            ],
            mm.generator['source']()
        )

#
# Networking
#
class net(builder.Module):

    def __init__(self, manager):
        super(net, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/net/bpf_buffer.h',
                'sys/net/bpfdesc.h',
                'sys/net/bpf.h',
                'sys/net/bpf_jitter.h',
                'sys/net/bpf_zerocopy.h',
                'sys/net/bridgestp.h',
                'sys/net/ethernet.h',
                'sys/net/fddi.h',
                'sys/net/firewire.h',
                'sys/net/ieee8023ad_lacp.h',
                'sys/net/if_arc.h',
                'sys/net/if_arp.h',
                'sys/net/if_bridgevar.h',
                'sys/net/if_clone.h',
                'sys/net/if_dl.h',
                'sys/net/if_enc.h',
                'sys/net/if_gif.h',
                'sys/net/if_gre.h',
                'sys/net/if_lagg.h',
                'sys/net/if_llatbl.h',
                'sys/net/if_llc.h',
                'sys/net/if_media.h',
                'sys/net/if_mib.h',
                'sys/net/if_sppp.h',
                'sys/net/if_tap.h',
                'sys/net/if_tapvar.h',
                'sys/net/if_tun.h',
                'sys/net/if_types.h',
                'sys/net/if_var.h',
                'sys/net/if_vlan_var.h',
                'sys/net/iso88025.h',
                'sys/net/netisr.h',
                'sys/net/netisr_internal.h',
                'sys/net/pfil.h',
                'sys/net/pfkeyv2.h',
                'sys/net/ppp_defs.h',
                'sys/net/radix.h',
                'sys/net/radix_mpath.h',
                'sys/net/raw_cb.h',
                'sys/net/route.h',
                'sys/net/route_var.h',
                'sys/net/rss_config.h',
                'sys/net/sff8436.h',
                'sys/net/sff8472.h',
                'sys/net/slcompress.h',
                'sys/net/vnet.h',
                'sys/netgraph/ng_socket.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/kern/sys_socket.c',
                'sys/kern/uipc_syscalls.c',
                'sys/net/bridgestp.c',
                'sys/net/ieee8023ad_lacp.c',
                'sys/net/if.c',
                'sys/net/if_clone.c',
                'sys/net/if_dead.c',
                'sys/net/if_disc.c',
                'sys/net/if_edsc.c',
                'sys/net/if_enc.c',
                'sys/net/if_epair.c',
                'sys/net/if_fddisubr.c',
                'sys/net/if_fwsubr.c',
                'sys/net/if_gif.c',
                'sys/net/if_gre.c',
                'sys/net/if_iso88025subr.c',
                'sys/net/if_lagg.c',
                'sys/net/if_llatbl.c',
                'sys/net/if_loop.c',
                'sys/net/if_media.c',
                'sys/net/if_mib.c',
                'sys/net/if_spppfr.c',
                'sys/net/if_spppsubr.c',
                'sys/net/if_tap.c',
                'sys/net/if_tun.c',
                'sys/net/if_vlan.c',
                'sys/net/pfil.c',
                'sys/net/radix.c',
                'sys/net/radix_mpath.c',
                'sys/net/raw_cb.c',
                'sys/net/raw_usrreq.c',
                'sys/net/route.c',
                'sys/net/rtsock.c',
                'sys/net/slcompress.c',
                'sys/net/bpf_buffer.c',
                'sys/net/bpf.c',
                'sys/net/bpf_filter.c',
                'sys/net/bpf_jitter.c',
                'sys/net/if_arcsubr.c',
                'sys/net/if_bridge.c',
                'sys/net/if_ethersubr.c',
                'sys/net/netisr.c',
            ],
            mm.generator['source']()
        )

#
# Internet Networking
#
class netinet(builder.Module):

    def __init__(self, manager):
        super(netinet, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/netinet/cc/cc.h',
                'sys/netinet/cc/cc_module.h',
                'sys/netinet/in_fib.h',
                'sys/netinet/icmp6.h',
                'sys/netinet/icmp_var.h',
                'sys/netinet/if_ether.h',
                'sys/netinet/igmp.h',
                'sys/netinet/igmp_var.h',
                'sys/netinet/in_kdtrace.h',
                'sys/netinet/in_pcb.h',
                'sys/netinet/in_rss.h',
                'sys/netinet/in_systm.h',
                'sys/netinet/in_var.h',
                'sys/netinet/ip6.h',
                'sys/netinet/ip_carp.h',
                'sys/netinet/ip_divert.h',
                'sys/netinet/ip_dummynet.h',
                'sys/netinet/ip_ecn.h',
                'sys/netinet/ip_encap.h',
                'sys/netinet/ip_fw.h',
                'sys/netinet/ip.h',
                'sys/netinet/ip_icmp.h',
                'sys/netinet/ip_mroute.h',
                'sys/netinet/ip_options.h',
                'sys/netinet/ip_var.h',
                'sys/netinet/pim.h',
                'sys/netinet/pim_var.h',
                'sys/netinet/sctp_asconf.h',
                'sys/netinet/sctp_auth.h',
                'sys/netinet/sctp_bsd_addr.h',
                'sys/netinet/sctp_constants.h',
                'sys/netinet/sctp_crc32.h',
                'sys/netinet/sctp_dtrace_declare.h',
                'sys/netinet/sctp_dtrace_define.h',
                'sys/netinet/sctp.h',
                'sys/netinet/sctp_header.h',
                'sys/netinet/sctp_indata.h',
                'sys/netinet/sctp_input.h',
                'sys/netinet/sctp_lock_bsd.h',
                'sys/netinet/sctp_os_bsd.h',
                'sys/netinet/sctp_os.h',
                'sys/netinet/sctp_output.h',
                'sys/netinet/sctp_pcb.h',
                'sys/netinet/sctp_peeloff.h',
                'sys/netinet/sctp_structs.h',
                'sys/netinet/sctp_sysctl.h',
                'sys/netinet/sctp_timer.h',
                'sys/netinet/sctp_uio.h',
                'sys/netinet/sctputil.h',
                'sys/netinet/sctp_var.h',
                'sys/netinet/tcp_debug.h',
                'sys/netinet/tcp_fsm.h',
                'sys/netinet/tcp_hostcache.h',
                'sys/netinet/tcpip.h',
                'sys/netinet/tcp_lro.h',
                'sys/netinet/tcp_offload.h',
                'sys/netinet/tcp_seq.h',
                'sys/netinet/tcp_syncache.h',
                'sys/netinet/tcp_timer.h',
                'sys/netinet/tcp_var.h',
                'sys/netinet/toecore.h',
                'sys/netinet/udp.h',
                'sys/netinet/udplite.h',
                'sys/netinet/udp_var.h',
                'sys/netinet/libalias/alias_local.h',
                'sys/netinet/libalias/alias.h',
                'sys/netinet/libalias/alias_mod.h',
                'sys/netinet/libalias/alias_sctp.h',
            ]
        )
        # in_cksum.c is architecture dependent
        self.addKernelSpaceSourceFiles(
            [
                'sys/netinet/accf_data.c',
                'sys/netinet/accf_dns.c',
                'sys/netinet/accf_http.c',
                'sys/netinet/cc/cc.c',
                'sys/netinet/cc/cc_newreno.c',
                'sys/netinet/if_ether.c',
                'sys/netinet/igmp.c',
                'sys/netinet/in.c',
                'sys/netinet/in_fib.c',
                'sys/netinet/in_gif.c',
                'sys/netinet/in_mcast.c',
                'sys/netinet/in_pcb.c',
                'sys/netinet/in_proto.c',
                'sys/netinet/in_rmx.c',
                'sys/netinet/ip_carp.c',
                'sys/netinet/ip_divert.c',
                'sys/netinet/ip_ecn.c',
                'sys/netinet/ip_encap.c',
                'sys/netinet/ip_fastfwd.c',
                'sys/netinet/ip_gre.c',
                'sys/netinet/ip_icmp.c',
                'sys/netinet/ip_id.c',
                'sys/netinet/ip_input.c',
                'sys/netinet/ip_mroute.c',
                'sys/netinet/ip_options.c',
                'sys/netinet/ip_output.c',
                'sys/netinet/ip_reass.c',
                'sys/netinet/raw_ip.c',
                'sys/netinet/sctp_asconf.c',
                'sys/netinet/sctp_auth.c',
                'sys/netinet/sctp_bsd_addr.c',
                'sys/netinet/sctp_cc_functions.c',
                'sys/netinet/sctp_crc32.c',
                'sys/netinet/sctp_indata.c',
                'sys/netinet/sctp_input.c',
                'sys/netinet/sctp_output.c',
                'sys/netinet/sctp_pcb.c',
                'sys/netinet/sctp_peeloff.c',
                'sys/netinet/sctp_sysctl.c',
                'sys/netinet/sctp_timer.c',
                'sys/netinet/sctp_usrreq.c',
                'sys/netinet/sctputil.c',
                'sys/netinet/tcp_debug.c',
                #'netinet/tcp_hostcache.c',
                'sys/netinet/tcp_input.c',
                'sys/netinet/tcp_lro.c',
                'sys/netinet/tcp_offload.c',
                'sys/netinet/tcp_output.c',
                'sys/netinet/tcp_reass.c',
                'sys/netinet/tcp_sack.c',
                'sys/netinet/tcp_subr.c',
                'sys/netinet/tcp_syncache.c',
                'sys/netinet/tcp_timer.c',
                'sys/netinet/tcp_timewait.c',
                'sys/netinet/tcp_usrreq.c',
                'sys/netinet/udp_usrreq.c',
                'sys/netinet/libalias/alias_dummy.c',
                'sys/netinet/libalias/alias_pptp.c',
                'sys/netinet/libalias/alias_smedia.c',
                'sys/netinet/libalias/alias_mod.c',
                'sys/netinet/libalias/alias_cuseeme.c',
                'sys/netinet/libalias/alias_nbt.c',
                'sys/netinet/libalias/alias_irc.c',
                'sys/netinet/libalias/alias_util.c',
                'sys/netinet/libalias/alias_db.c',
                'sys/netinet/libalias/alias_ftp.c',
                'sys/netinet/libalias/alias_proxy.c',
                'sys/netinet/libalias/alias.c',
                'sys/netinet/libalias/alias_skinny.c',
                'sys/netinet/libalias/alias_sctp.c',
            ],
            mm.generator['source']()
        )

#
# IPv6
#
class netinet6(builder.Module):

    def __init__(self, manager):
        super(netinet6, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/netinet6/icmp6.h',
                'sys/netinet6/in6_fib.h',
                'sys/netinet6/in6_ifattach.h',
                'sys/netinet6/in6_pcb.h',
                'sys/netinet6/in6_rss.h',
                'sys/netinet6/in6_var.h',
                'sys/netinet6/ip6_ecn.h',
                'sys/netinet6/ip6.h',
                'sys/netinet6/ip6_mroute.h',
                'sys/netinet6/ip6protosw.h',
                'sys/netinet6/ip6_var.h',
                'sys/netinet6/mld6.h',
                'sys/netinet6/mld6_var.h',
                'sys/netinet6/nd6.h',
                'sys/netinet6/pim6.h',
                'sys/netinet6/pim6_var.h',
                'sys/netinet6/raw_ip6.h',
                'sys/netinet6/scope6_var.h',
                'sys/netinet6/sctp6_var.h',
                'sys/netinet6/send.h',
                'sys/netinet6/tcp6_var.h',
                'sys/netinet6/udp6_var.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/net/if_stf.c',
                'sys/netinet6/dest6.c',
                'sys/netinet6/frag6.c',
                'sys/netinet6/icmp6.c',
                'sys/netinet6/in6.c',
                'sys/netinet6/in6_cksum.c',
                'sys/netinet6/in6_fib.c',
                'sys/netinet6/in6_gif.c',
                'sys/netinet6/in6_ifattach.c',
                'sys/netinet6/in6_mcast.c',
                'sys/netinet6/in6_pcb.c',
                'sys/netinet6/in6_proto.c',
                'sys/netinet6/in6_rmx.c',
                'sys/netinet6/in6_src.c',
                'sys/netinet6/ip6_fastfwd.c',
                'sys/netinet6/ip6_forward.c',
                'sys/netinet6/ip6_id.c',
                'sys/netinet6/ip6_input.c',
                'sys/netinet6/ip6_mroute.c',
                'sys/netinet6/ip6_output.c',
                'sys/netinet6/mld6.c',
                'sys/netinet6/nd6.c',
                'sys/netinet6/nd6_nbr.c',
                'sys/netinet6/nd6_rtr.c',
                'sys/netinet6/raw_ip6.c',
                'sys/netinet6/route6.c',
                'sys/netinet6/scope6.c',
                'sys/netinet6/sctp6_usrreq.c',
                'sys/netinet6/udp6_usrreq.c',
            ],
            mm.generator['source']()
        )

#
# IPsec
#
class netipsec(builder.Module):

    def __init__(self, manager):
        super(netipsec, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/net/if_ipsec.h',
                'sys/netipsec/ah.h',
                'sys/netipsec/ah_var.h',
                'sys/netipsec/esp.h',
                'sys/netipsec/esp_var.h',
                'sys/netipsec/ipcomp.h',
                'sys/netipsec/ipcomp_var.h',
                'sys/netipsec/ipsec6.h',
                'sys/netipsec/ipsec.h',
                'sys/netipsec/ipsec_support.h',
                'sys/netipsec/keydb.h',
                'sys/netipsec/key_debug.h',
                'sys/netipsec/key.h',
                'sys/netipsec/keysock.h',
                'sys/netipsec/key_var.h',
                'sys/netipsec/xform.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/net/if_ipsec.c',
                'sys/netipsec/ipsec.c',
                'sys/netipsec/ipsec_input.c',
                'sys/netipsec/ipsec_mbuf.c',
                'sys/netipsec/ipsec_mod.c',
                'sys/netipsec/ipsec_output.c',
                'sys/netipsec/ipsec_pcb.c',
                'sys/netipsec/key.c',
                'sys/netipsec/key_debug.c',
                'sys/netipsec/keysock.c',
                'sys/netipsec/subr_ipsec.c',
                'sys/netipsec/udpencap.c',
                'sys/netipsec/xform_ah.c',
                'sys/netipsec/xform_esp.c',
                'sys/netipsec/xform_ipcomp.c',
                'sys/netipsec/xform_tcp.c',
            ],
            mm.generator['source']()
        )

        # libipsec
        libipsec_cflags = ['-D__FreeBSD__=1', '-DHAVE_CONFIG_H=1']
        self.addSourceFiles(
            [
                'ipsec-tools/src/libipsec/ipsec_dump_policy.c',
                'ipsec-tools/src/libipsec/ipsec_get_policylen.c',
                'ipsec-tools/src/libipsec/ipsec_strerror.c',
                'ipsec-tools/src/libipsec/key_debug.c',
                'ipsec-tools/src/libipsec/pfkey.c',
                'ipsec-tools/src/libipsec/pfkey_dump.c',
            ],
            mm.generator['source'](libipsec_cflags)
        )
        self.addFile(mm.generator['file']('ipsec-tools/src/libipsec/policy_token.l',
                                          mm.generator['path'](),
                                          mm.generator['convert'](),
                                          mm.generator['convert'](),
                                          mm.generator['lex']('__libipsec',
                                                              'policy_parse.c',
                                                              libipsec_cflags)))
        self.addFile(mm.generator['file']('ipsec-tools/src/libipsec/policy_parse.y',
                                          mm.generator['path'](),
                                          mm.generator['convert'](),
                                          mm.generator['convert'](),
                                          mm.generator['yacc']('__libipsec',
                                                               'policy_parse.h',
                                                               libipsec_cflags)))

        # raccoon
        racoon_cflags = ['-D__FreeBSD__=1']
        self.addSourceFiles(
            [
                'ipsec-tools/src/racoon/admin.c',
                'ipsec-tools/src/racoon/algorithm.c',
                'ipsec-tools/src/racoon/backupsa.c',
                'ipsec-tools/src/racoon/cfparse_wrapper.c',
                'ipsec-tools/src/racoon/cftoken_wrapper.c',
                'ipsec-tools/src/racoon/crypto_openssl.c',
                'ipsec-tools/src/racoon/dnssec.c',
                'ipsec-tools/src/racoon/evt.c',
                'ipsec-tools/src/racoon/genlist.c',
                'ipsec-tools/src/racoon/getcertsbyname.c',
                'ipsec-tools/src/racoon/grabmyaddr.c',
                'ipsec-tools/src/racoon/gssapi.c',
                'ipsec-tools/src/racoon/handler.c',
                'ipsec-tools/src/racoon/ipsec_doi.c',
                'ipsec-tools/src/racoon/isakmp.c',
                'ipsec-tools/src/racoon/isakmp_agg.c',
                'ipsec-tools/src/racoon/isakmp_base.c',
                'ipsec-tools/src/racoon/isakmp_frag.c',
                'ipsec-tools/src/racoon/isakmp_ident.c',
                'ipsec-tools/src/racoon/isakmp_inf.c',
                'ipsec-tools/src/racoon/isakmp_newg.c',
                'ipsec-tools/src/racoon/isakmp_quick.c',
                'ipsec-tools/src/racoon/localconf.c',
                'ipsec-tools/src/racoon/logger.c',
                'ipsec-tools/src/racoon/main.c',
                'ipsec-tools/src/racoon/misc.c',
                'ipsec-tools/src/racoon/missing/crypto/sha2/sha2.c',
                'ipsec-tools/src/racoon/nattraversal.c',
                'ipsec-tools/src/racoon/oakley.c',
                'ipsec-tools/src/racoon/pfkey.c',
                'ipsec-tools/src/racoon/plog.c',
                'ipsec-tools/src/racoon/policy.c',
                'ipsec-tools/src/racoon/privsep.c',
                'ipsec-tools/src/racoon/proposal.c',
                'ipsec-tools/src/racoon/prsa_par_wrapper.c',
                'ipsec-tools/src/racoon/prsa_tok_wrapper.c',
                'ipsec-tools/src/racoon/remoteconf.c',
                'ipsec-tools/src/racoon/rsalist.c',
                'ipsec-tools/src/racoon/safefile.c',
                'ipsec-tools/src/racoon/sainfo.c',
                'ipsec-tools/src/racoon/schedule.c',
                'ipsec-tools/src/racoon/session.c',
                'ipsec-tools/src/racoon/sockmisc.c',
                'ipsec-tools/src/racoon/str2val.c',
                'ipsec-tools/src/racoon/strnames.c',
                'ipsec-tools/src/racoon/vendorid.c',
                'ipsec-tools/src/racoon/vmbuf.c',
            ],
            mm.generator['source'](racoon_cflags)
        )
        self.addFile(mm.generator['file']('ipsec-tools/src/racoon/cftoken.l',
                                          mm.generator['path'](),
                                          mm.generator['convert'](),
                                          mm.generator['convert'](),
                                          mm.generator['lex']('racoonyy',
                                                              'cftoken.c',
                                                              racoon_cflags,
                                                              build=False)))
        self.addFile(mm.generator['file']('ipsec-tools/src/racoon/cfparse.y',
                                          mm.generator['path'](),
                                          mm.generator['convert'](),
                                          mm.generator['convert'](),
                                          mm.generator['yacc']('racoonyy',
                                                               'cfparse.h',
                                                               racoon_cflags,
                                                               build=False)))
        self.addFile(mm.generator['file']('ipsec-tools/src/racoon/prsa_tok.l',
                                          mm.generator['path'](),
                                          mm.generator['convert'](),
                                          mm.generator['convert'](),
                                          mm.generator['lex']('racoonprsa',
                                                              'prsa_tok.c',
                                                              racoon_cflags,
                                                              build=False)))
        self.addFile(mm.generator['file']('ipsec-tools/src/racoon/prsa_par.y',
                                          mm.generator['path'](),
                                          mm.generator['convert'](),
                                          mm.generator['convert'](),
                                          mm.generator['yacc']('racoonprsa',
                                                               'prsa_par.h',
                                                               racoon_cflags,
                                                               build=False)))
        self.addRTEMSHeaderFiles(
            [
                'rtems/ipsec.h',
            ]
        )
        self.addRTEMSSourceFiles(
            [
                'rtems/rtems-bsd-racoon.c',
                'rtems/rtems-bsd-rc-conf-ipsec.c',
                'rtems/rtems-bsd-shell-racoon.c',
                'rtems/rtems-racoon-mutex.c',
            ],
            mm.generator['source']()
        )

        # setkey
        setkey_cflags = ['-D__FreeBSD__=1', '-DHAVE_CONFIG_H=1']
        self.addSourceFiles(
            [
                'ipsec-tools/src/setkey/parse_wrapper.c',
                'ipsec-tools/src/setkey/setkey.c',
                'ipsec-tools/src/setkey/token_wrapper.c',
            ],
            mm.generator['source'](setkey_cflags)
        )
        self.addFile(mm.generator['file']('ipsec-tools/src/setkey/token.l',
                                          mm.generator['path'](),
                                          mm.generator['convert'](),
                                          mm.generator['convert'](),
                                          mm.generator['lex']('setkeyyy',
                                                              'token.c',
                                                              setkey_cflags,
                                                              build=False)))
        self.addFile(mm.generator['file']('ipsec-tools/src/setkey/parse.y',
                                          mm.generator['path'](),
                                          mm.generator['convert'](),
                                          mm.generator['convert'](),
                                          mm.generator['yacc']('setkeyyy',
                                                               'parse.h',
                                                               setkey_cflags,
                                                               build=False)))
        self.addRTEMSSourceFiles(
            [
                'rtems/rtems-bsd-shell-setkey.c',
            ],
            mm.generator['source']()
        )

#
# IEEE 802.11
#
class net80211(builder.Module):

    def __init__(self, manager):
        super(net80211, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/net80211/ieee80211_action.h',
                'sys/net80211/ieee80211_adhoc.h',
                'sys/net80211/ieee80211_ageq.h',
                'sys/net80211/ieee80211_alq.h',
                'sys/net80211/ieee80211_amrr.h',
                'sys/net80211/ieee80211_crypto.h',
                'sys/net80211/ieee80211_dfs.h',
                'sys/net80211/ieee80211_freebsd.h',
                'sys/net80211/ieee80211.h',
                'sys/net80211/_ieee80211.h',
                'sys/net80211/ieee80211_hostap.h',
                'sys/net80211/ieee80211_ht.h',
                'sys/net80211/ieee80211_input.h',
                'sys/net80211/ieee80211_ioctl.h',
                'sys/net80211/ieee80211_mesh.h',
                'sys/net80211/ieee80211_monitor.h',
                'sys/net80211/ieee80211_node.h',
                'sys/net80211/ieee80211_phy.h',
                'sys/net80211/ieee80211_power.h',
                'sys/net80211/ieee80211_proto.h',
                'sys/net80211/ieee80211_radiotap.h',
                'sys/net80211/ieee80211_ratectl.h',
                'sys/net80211/ieee80211_regdomain.h',
                'sys/net80211/ieee80211_rssadapt.h',
                'sys/net80211/ieee80211_scan.h',
                'sys/net80211/ieee80211_scan_sw.h',
                'sys/net80211/ieee80211_sta.h',
                'sys/net80211/ieee80211_superg.h',
                'sys/net80211/ieee80211_tdma.h',
                'sys/net80211/ieee80211_var.h',
                'sys/net80211/ieee80211_vht.h',
                'sys/net80211/ieee80211_wds.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/net80211/ieee80211_acl.c',
                'sys/net80211/ieee80211_action.c',
                'sys/net80211/ieee80211_adhoc.c',
                'sys/net80211/ieee80211_ageq.c',
                'sys/net80211/ieee80211_amrr.c',
                'sys/net80211/ieee80211.c',
                'sys/net80211/ieee80211_crypto.c',
                'sys/net80211/ieee80211_crypto_ccmp.c',
                'sys/net80211/ieee80211_crypto_none.c',
                'sys/net80211/ieee80211_crypto_tkip.c',
                'sys/net80211/ieee80211_crypto_wep.c',
                'sys/net80211/ieee80211_ddb.c',
                'sys/net80211/ieee80211_dfs.c',
                'sys/net80211/ieee80211_freebsd.c',
                'sys/net80211/ieee80211_hostap.c',
                'sys/net80211/ieee80211_ht.c',
                'sys/net80211/ieee80211_hwmp.c',
                'sys/net80211/ieee80211_input.c',
                'sys/net80211/ieee80211_ioctl.c',
                'sys/net80211/ieee80211_mesh.c',
                'sys/net80211/ieee80211_monitor.c',
                'sys/net80211/ieee80211_node.c',
                'sys/net80211/ieee80211_output.c',
                'sys/net80211/ieee80211_phy.c',
                'sys/net80211/ieee80211_power.c',
                'sys/net80211/ieee80211_proto.c',
                'sys/net80211/ieee80211_radiotap.c',
                'sys/net80211/ieee80211_ratectl.c',
                'sys/net80211/ieee80211_ratectl_none.c',
                'sys/net80211/ieee80211_regdomain.c',
                'sys/net80211/ieee80211_rssadapt.c',
                'sys/net80211/ieee80211_scan.c',
                'sys/net80211/ieee80211_scan_sta.c',
                'sys/net80211/ieee80211_scan_sw.c',
                'sys/net80211/ieee80211_sta.c',
                'sys/net80211/ieee80211_superg.c',
                'sys/net80211/ieee80211_tdma.c',
                'sys/net80211/ieee80211_vht.c',
                'sys/net80211/ieee80211_wds.c',
                'sys/net80211/ieee80211_xauth.c',
            ],
            mm.generator['source']()
        )

#
# Open Crypto
#
class opencrypto(builder.Module):

    def __init__(self, manager):
        super(opencrypto, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/opencrypto/cast.h',
                'sys/opencrypto/castsb.h',
                'sys/opencrypto/cryptodev.h',
                'sys/opencrypto/cryptosoft.h',
                'sys/opencrypto/deflate.h',
                'sys/opencrypto/gfmult.h',
                'sys/opencrypto/gmac.h',
                'sys/opencrypto/rmd160.h',
                'sys/opencrypto/skipjack.h',
                'sys/opencrypto/xform_auth.h',
                'sys/opencrypto/xform_comp.h',
                'sys/opencrypto/xform_enc.h',
                'sys/opencrypto/xform.h',
                'sys/opencrypto/xform_userland.h',
                'sys/opencrypto/xform_aes_icm.c',
                'sys/opencrypto/xform_aes_xts.c',
                'sys/opencrypto/xform_blf.c',
                'sys/opencrypto/xform_cast5.c',
                'sys/opencrypto/xform_cml.c',
                'sys/opencrypto/xform_deflate.c',
                'sys/opencrypto/xform_des1.c',
                'sys/opencrypto/xform_des3.c',
                'sys/opencrypto/xform_gmac.c',
                'sys/opencrypto/xform_md5.c',
                'sys/opencrypto/xform_null.c',
                'sys/opencrypto/xform_rijndael.c',
                'sys/opencrypto/xform_rmd160.c',
                'sys/opencrypto/xform_sha1.c',
                'sys/opencrypto/xform_sha2.c',
                'sys/opencrypto/xform_skipjack.c',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/opencrypto/cast.c',
                'sys/opencrypto/criov.c',
                'sys/opencrypto/crypto.c',
                'sys/opencrypto/cryptodeflate.c',
                'sys/opencrypto/cryptodev.c',
                'sys/opencrypto/cryptosoft.c',
                'sys/opencrypto/gfmult.c',
                'sys/opencrypto/gmac.c',
                'sys/opencrypto/rmd160.c',
                'sys/opencrypto/skipjack.c',
                'sys/opencrypto/xform.c',
            ],
            mm.generator['source']()
        )

#
# Crypto
#
class crypto(builder.Module):

    def __init__(self, manager):
        super(crypto, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/crypto/intake.h',
                'sys/crypto/skein/skein_iv.h',
                'sys/crypto/skein/skein_freebsd.h',
                'sys/crypto/skein/skein.h',
                'sys/crypto/skein/skein_debug.h',
                'sys/crypto/skein/skein_port.h',
                'sys/crypto/rc4/rc4.h',
                'sys/crypto/sha2/sha384.h',
                'sys/crypto/sha2/sha256.h',
                'sys/crypto/sha2/sha512t.h',
                'sys/crypto/sha2/sha512.h',
                'sys/crypto/sha1.h',
                'sys/crypto/siphash/siphash.h',
                'sys/crypto/rijndael/rijndael-api-fst.h',
                'sys/crypto/rijndael/rijndael_local.h',
                'sys/crypto/rijndael/rijndael.h',
                'sys/crypto/camellia/camellia.h',
                'sys/crypto/des/spr.h',
                'sys/crypto/des/des_locl.h',
                'sys/crypto/des/des.h',
                'sys/crypto/des/podd.h',
                'sys/crypto/des/sk.h',
                'sys/crypto/blowfish/bf_pi.h',
                'sys/crypto/blowfish/blowfish.h',
                'sys/crypto/blowfish/bf_locl.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/crypto/skein/skein_block.c',
                'sys/crypto/skein/skein.c',
                'sys/crypto/rc4/rc4.c',
                'sys/crypto/sha2/sha256c.c',
                'sys/crypto/sha2/sha512c.c',
                'sys/crypto/siphash/siphash.c',
                'sys/crypto/sha1.c',
                'sys/crypto/rijndael/rijndael-api.c',
                'sys/crypto/rijndael/rijndael-alg-fst.c',
                'sys/crypto/rijndael/rijndael-api-fst.c',
                'sys/crypto/camellia/camellia-api.c',
                'sys/crypto/camellia/camellia.c',
                'sys/crypto/des/des_enc.c',
                'sys/crypto/des/des_setkey.c',
                'sys/crypto/des/des_ecb.c',
                'sys/crypto/blowfish/bf_skey.c',
                'sys/crypto/blowfish/bf_enc.c',
                'sys/crypto/blowfish/bf_ecb.c',
            ],
            mm.generator['source']()
        )

#
# Altq
#
class altq(builder.Module):

    def __init__(self, manager):
        super(altq, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/net/altq/altq_cbq.h',
                'sys/net/altq/altq_cdnr.h',
                'sys/net/altq/altq_classq.h',
                'sys/net/altq/altq_codel.h',
                'sys/net/altq/altq_fairq.h',
                'sys/net/altq/altq.h',
                'sys/net/altq/altq_hfsc.h',
                'sys/net/altq/altq_priq.h',
                'sys/net/altq/altq_red.h',
                'sys/net/altq/altq_rio.h',
                'sys/net/altq/altq_rmclass_debug.h',
                'sys/net/altq/altq_rmclass.h',
                'sys/net/altq/altq_var.h',
                'sys/net/altq/if_altq.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/net/altq/altq_cbq.c',
                'sys/net/altq/altq_cdnr.c',
                'sys/net/altq/altq_codel.c',
                'sys/net/altq/altq_fairq.c',
                'sys/net/altq/altq_hfsc.c',
                'sys/net/altq/altq_priq.c',
                'sys/net/altq/altq_red.c',
                'sys/net/altq/altq_rio.c',
                'sys/net/altq/altq_rmclass.c',
                'sys/net/altq/altq_subr.c',
            ],
            mm.generator['source']()
        )

#
# Packet filter
#
class pf(builder.Module):

    def __init__(self, manager):
        super(pf, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceHeaderFiles(
            [
                'sys/net/if_pflog.h',
                'sys/net/if_pfsync.h',
                'sys/net/pfvar.h',
                'sys/netpfil/ipfw/ip_fw_private.h',
                'sys/netpfil/pf/pf_altq.h',
                'sys/netpfil/pf/pf.h',
                'sys/netpfil/pf/pf_mtag.h',
            ]
        )
        self.addKernelSpaceSourceFiles(
            [
                'sys/netpfil/pf/if_pflog.c',
                'sys/netpfil/pf/if_pfsync.c',
                'sys/netpfil/pf/in4_cksum.c',
                'sys/netpfil/pf/pf.c',
                'sys/netpfil/pf/pf_if.c',
                'sys/netpfil/pf/pf_ioctl.c',
                'sys/netpfil/pf/pf_lb.c',
                'sys/netpfil/pf/pf_norm.c',
                'sys/netpfil/pf/pf_osfp.c',
                'sys/netpfil/pf/pf_ruleset.c',
                'sys/netpfil/pf/pf_table.c',
            ],
            mm.generator['source']()
        )

#
# PCI
#
class pci(builder.Module):

    def __init__(self, manager):
        super(pci, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addKernelSpaceSourceFiles(
            [
                'sys/dev/pci/pci.c',
                'sys/dev/pci/pci_user.c',
                'sys/dev/pci/pci_pci.c',
            ],
            mm.generator['source']()
        )
        self.addKernelSpaceHeaderFiles(
            [
                'sys/dev/pci/pcib_private.h',
                'sys/dev/pci/pci_private.h',
                'sys/dev/pci/pcireg.h',
                'sys/dev/pci/pcivar.h',
                'sys/dev/pci/pcivar.h',
            ]
        )
        self.addCPUDependentFreeBSDHeaderFiles(
            [
                'sys/i386/include/_bus.h',
                'sys/i386/include/legacyvar.h',
                'sys/x86/include/bus.h',
                'sys/x86/include/pci_cfgreg.h',
            ]
        )
        self.addCPUDependentFreeBSDSourceFiles(
            [ 'i386' ],
            [
                'sys/i386/i386/legacy.c',
                'sys/x86/pci/pci_bus.c',
            ],
            mm.generator['source']()
        )

#
# User space
#
class user_space(builder.Module):

    def __init__(self, manager):
        super(user_space, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addUserSpaceHeaderFiles(
            [
                'bin/stty/extern.h',
                'bin/stty/stty.h',
                'contrib/libxo/libxo/xo_buf.h',
                'contrib/libxo/libxo/xo_encoder.h',
                'contrib/libxo/libxo/xo.h',
                'contrib/libxo/libxo/xo_humanize.h',
                'contrib/libxo/libxo/xo_wcwidth.h',
                'sbin/pfctl/pfctl.h',
                'sbin/pfctl/pfctl_parser.h',
                'include/arpa/nameser_compat.h',
                'include/arpa/nameser.h',
                'include/db.h',
                'include/err.h',
                'include/gssapi/gssapi.h',
                'include/gssapi.h',
                'include/ifaddrs.h',
                'include/mpool.h',
                'include/netconfig.h',
                'include/netdb.h',
                'include/nlist.h',
                'include/nsswitch.h',
                'include/resolv.h',
                'include/res_update.h',
                'include/rpc/clnt.h',
                'include/rpc/pmap_rmt.h',
                'include/rpc/svc_soc.h',
                'include/rpc/nettype.h',
                'include/rpc/xdr.h',
                'include/rpc/svc.h',
                'include/rpc/rpc_msg.h',
                'include/rpc/rpcsec_gss.h',
                'include/rpc/raw.h',
                'include/rpc/clnt_stat.h',
                'include/rpc/auth.h',
                'include/rpc/svc_dg.h',
                'include/rpc/auth_kerb.h',
                'include/rpc/auth_des.h',
                'include/rpc/rpcb_clnt.h',
                'include/rpc/rpc.h',
                'include/rpc/des.h',
                'include/rpc/des_crypt.h',
                'include/rpc/svc_auth.h',
                'include/rpc/pmap_clnt.h',
                'include/rpc/clnt_soc.h',
                'include/rpc/pmap_prot.h',
                'include/rpc/auth_unix.h',
                'include/rpc/rpc_com.h',
                'include/rpc/rpcent.h',
                'include/rpcsvc/nis_db.h',
                'include/rpcsvc/nislib.h',
                'include/rpcsvc/nis_tags.h',
                'include/rpcsvc/ypclnt.h',
                'include/rpcsvc/yp_prot.h',
                'include/sysexits.h',
                'lib/lib80211/lib80211_ioctl.h',
                'lib/lib80211/lib80211_regdomain.h',
                'lib/libcasper/libcasper/libcasper.h',
                'lib/libcasper/services/cap_dns/cap_dns.h',
                'lib/libcasper/services/cap_syslog/cap_syslog.h',
                'lib/libc/db/btree/btree.h',
                'lib/libc/db/btree/extern.h',
                'lib/libc/db/recno/extern.h',
                'lib/libc/db/recno/recno.h',
                'lib/libc/include/isc/eventlib.h',
                'lib/libc/include/isc/list.h',
                'lib/libc/include/isc/platform.h',
                'lib/libc/include/libc_private.h',
                'lib/libc/include/namespace.h',
                'lib/libc/include/nss_tls.h',
                'lib/libc/include/port_after.h',
                'lib/libc/include/port_before.h',
                'lib/libc/include/reentrant.h',
                'lib/libc/include/resolv_mt.h',
                'lib/libc/include/spinlock.h',
                'lib/libc/include/un-namespace.h',
                'lib/libc/isc/eventlib_p.h',
                'lib/libc/net/netdb_private.h',
                'lib/libc/net/nss_backends.h',
                'lib/libc/net/res_config.h',
                'lib/libc/resolv/res_debug.h',
                'lib/libc/resolv/res_private.h',
                'lib/libc/rpc/mt_misc.h',
                'lib/libc/rpc/rpc_com.h',
                'lib/libc/stdio/local.h',
                'lib/libkvm/kvm.h',
                'lib/libmemstat/memstat.h',
                'lib/libmemstat/memstat_internal.h',
                'lib/libutil/libutil.h',
                'sbin/dhclient/dhcpd.h',
                'sbin/dhclient/dhcp.h',
                'sbin/dhclient/dhctoken.h',
                'sbin/dhclient/privsep.h',
                'sbin/dhclient/tree.h',
                'sbin/ifconfig/ifconfig.h',
                'usr.bin/netstat/netstat.h'
            ]
        )
        self.addFile(mm.generator['file']('include/rpc/rpcb_prot.x',
                                          mm.generator['freebsd-path'](),
                                          mm.generator['convert'](),
                                          mm.generator['convert'](),
                                          mm.generator['rpc-gen']()))
        self.addFile(mm.generator['file']('include/rpcsvc/nis.x',
                                          mm.generator['freebsd-path'](),
                                          mm.generator['convert'](),
                                          mm.generator['convert'](),
                                          mm.generator['rpc-gen']()))
        self.addFile(mm.generator['file']('sbin/route/keywords',
                                          mm.generator['freebsd-path'](),
                                          mm.generator['convert'](),
                                          mm.generator['convert'](),
                                          mm.generator['route-keywords']()))
        self.addFile(mm.generator['file']('sbin/pfctl/parse.y',
                                          mm.generator['freebsd-path'](),
                                          mm.generator['convert'](),
                                          mm.generator['convert'](),
                                          mm.generator['yacc']('pfctly',
                                                               'parse.h')))
        self.addUserSpaceSourceFiles(
            [
                'lib/libc/db/btree/bt_close.c',
                'lib/libc/db/btree/bt_conv.c',
                'lib/libc/db/btree/bt_debug.c',
                'lib/libc/db/btree/bt_delete.c',
                'lib/libc/db/btree/bt_get.c',
                'lib/libc/db/btree/bt_open.c',
                'lib/libc/db/btree/bt_overflow.c',
                'lib/libc/db/btree/bt_page.c',
                'lib/libc/db/btree/bt_put.c',
                'lib/libc/db/btree/bt_search.c',
                'lib/libc/db/btree/bt_seq.c',
                'lib/libc/db/btree/bt_split.c',
                'lib/libc/db/btree/bt_utils.c',
                'lib/libc/db/db/db.c',
                'lib/libc/db/mpool/mpool.c',
                'lib/libc/db/mpool/mpool-compat.c',
                'lib/libc/db/recno/rec_close.c',
                'lib/libc/db/recno/rec_delete.c',
                'lib/libc/db/recno/rec_get.c',
                'lib/libc/db/recno/rec_open.c',
                'lib/libc/db/recno/rec_put.c',
                'lib/libc/db/recno/rec_search.c',
                'lib/libc/db/recno/rec_seq.c',
                'lib/libc/db/recno/rec_utils.c',
            ],
            mm.generator['source']('-D__DBINTERFACE_PRIVATE')
        )
        self.addRTEMSHeaderFiles(
            [
                'include/machine/rtems-bsd-regdomain.h',
            ]
        )
        self.addRTEMSSourceFiles(
            [
                'rtems/rtems-bsd-regdomain.c',
            ],
            mm.generator['source']()
        )
        self.addUserSpaceSourceFiles(
            [
                'bin/hostname/hostname.c',
                'bin/stty/cchar.c',
                'bin/stty/gfmt.c',
                'bin/stty/key.c',
                'bin/stty/modes.c',
                'bin/stty/print.c',
                'bin/stty/stty.c',
                'bin/stty/util.c',
                'contrib/libxo/libxo/libxo.c',
                'contrib/libxo/libxo/xo_encoder.c',
                'lib/lib80211/lib80211_ioctl.c',
                'lib/lib80211/lib80211_regdomain.c',
                'lib/libc/gen/err.c',
                'lib/libc/gen/feature_present.c',
                'lib/libc/gen/getdomainname.c',
                'lib/libc/gen/gethostname.c',
                'lib/libc/gen/sethostname.c',
                'lib/libc/inet/inet_addr.c',
                'lib/libc/inet/inet_cidr_ntop.c',
                'lib/libc/inet/inet_cidr_pton.c',
                'lib/libc/inet/inet_lnaof.c',
                'lib/libc/inet/inet_makeaddr.c',
                'lib/libc/inet/inet_neta.c',
                'lib/libc/inet/inet_net_ntop.c',
                'lib/libc/inet/inet_netof.c',
                'lib/libc/inet/inet_net_pton.c',
                'lib/libc/inet/inet_network.c',
                'lib/libc/inet/inet_ntoa.c',
                'lib/libc/inet/inet_ntop.c',
                'lib/libc/inet/inet_pton.c',
                'lib/libc/inet/nsap_addr.c',
                'lib/libc/isc/ev_streams.c',
                'lib/libc/isc/ev_timers.c',
                'lib/libc/nameser/ns_name.c',
                'lib/libc/nameser/ns_netint.c',
                'lib/libc/nameser/ns_parse.c',
                'lib/libc/nameser/ns_print.c',
                'lib/libc/nameser/ns_samedomain.c',
                'lib/libc/nameser/ns_ttl.c',
                'lib/libc/net/base64.c',
                'lib/libc/net/ether_addr.c',
                'lib/libc/net/gai_strerror.c',
                'lib/libc/net/getaddrinfo.c',
                'lib/libc/net/gethostbydns.c',
                'lib/libc/net/gethostbyht.c',
                'lib/libc/net/gethostbynis.c',
                'lib/libc/net/gethostnamadr.c',
                'lib/libc/net/getifaddrs.c',
                'lib/libc/net/getifmaddrs.c',
                'lib/libc/net/getnameinfo.c',
                'lib/libc/net/getnetbydns.c',
                'lib/libc/net/getnetbyht.c',
                'lib/libc/net/getnetbynis.c',
                'lib/libc/net/getnetnamadr.c',
                'lib/libc/net/getproto.c',
                'lib/libc/net/getprotoent.c',
                'lib/libc/net/getprotoname.c',
                'lib/libc/net/getservent.c',
                'lib/libc/net/if_indextoname.c',
                'lib/libc/net/if_nameindex.c',
                'lib/libc/net/if_nametoindex.c',
                'lib/libc/net/ip6opt.c',
                'lib/libc/net/linkaddr.c',
                'lib/libc/net/map_v4v6.c',
                'lib/libc/net/name6.c',
                'lib/libc/net/nsdispatch.c',
                'lib/libc/net/rcmd.c',
                'lib/libc/net/recv.c',
                'lib/libc/net/rthdr.c',
                'lib/libc/net/send.c',
                'lib/libc/net/vars.c',
                'lib/libc/posix1e/mac.c',
                'lib/libc/resolv/h_errno.c',
                'lib/libc/resolv/herror.c',
                'lib/libc/resolv/mtctxres.c',
                'lib/libc/resolv/res_comp.c',
                'lib/libc/resolv/res_data.c',
                'lib/libc/resolv/res_debug.c',
                'lib/libc/resolv/res_findzonecut.c',
                'lib/libc/resolv/res_init.c',
                'lib/libc/resolv/res_mkquery.c',
                'lib/libc/resolv/res_mkupdate.c',
                'lib/libc/resolv/res_query.c',
                'lib/libc/resolv/res_send.c',
                'lib/libc/resolv/res_state.c',
                'lib/libc/resolv/res_update.c',
                'lib/libc/rpc/auth_des.c',
                'lib/libc/rpc/authdes_prot.c',
                'lib/libc/rpc/auth_none.c',
                'lib/libc/rpc/auth_time.c',
                'lib/libc/rpc/auth_unix.c',
                'lib/libc/rpc/authunix_prot.c',
                'lib/libc/rpc/bindresvport.c',
                'lib/libc/rpc/clnt_bcast.c',
                'lib/libc/rpc/clnt_dg.c',
                'lib/libc/rpc/clnt_generic.c',
                'lib/libc/rpc/clnt_perror.c',
                'lib/libc/rpc/clnt_raw.c',
                'lib/libc/rpc/clnt_simple.c',
                'lib/libc/rpc/clnt_vc.c',
                'lib/libc/rpc/crypt_client.c',
                'lib/libc/rpc/des_crypt.c',
                'lib/libc/rpc/des_soft.c',
                'lib/libc/rpc/getnetconfig.c',
                'lib/libc/rpc/getnetpath.c',
                'lib/libc/rpc/getpublickey.c',
                'lib/libc/rpc/getrpcent.c',
                'lib/libc/rpc/getrpcport.c',
                'lib/libc/rpc/key_call.c',
                'lib/libc/rpc/key_prot_xdr.c',
                'lib/libc/rpc/mt_misc.c',
                'lib/libc/rpc/netname.c',
                'lib/libc/rpc/netnamer.c',
                'lib/libc/rpc/pmap_clnt.c',
                'lib/libc/rpc/pmap_getmaps.c',
                'lib/libc/rpc/pmap_getport.c',
                'lib/libc/rpc/pmap_prot2.c',
                'lib/libc/rpc/pmap_prot.c',
                'lib/libc/rpc/pmap_rmt.c',
                'lib/libc/rpc/rpcb_clnt.c',
                'lib/libc/rpc/rpcb_prot.c',
                'lib/libc/rpc/rpcb_st_xdr.c',
                'lib/libc/rpc/rpc_callmsg.c',
                'lib/libc/rpc/rpc_commondata.c',
                'lib/libc/rpc/rpcdname.c',
                'lib/libc/rpc/rpc_dtablesize.c',
                'lib/libc/rpc/rpc_generic.c',
                'lib/libc/rpc/rpc_prot.c',
                'lib/libc/rpc/rpcsec_gss_stub.c',
                'lib/libc/rpc/rpc_soc.c',
                'lib/libc/rpc/rtime.c',
                'lib/libc/rpc/svc_auth.c',
                'lib/libc/rpc/svc_auth_des.c',
                'lib/libc/rpc/svc_auth_unix.c',
                'lib/libc/rpc/svc.c',
                'lib/libc/rpc/svc_dg.c',
                'lib/libc/rpc/svc_generic.c',
                'lib/libc/rpc/svc_raw.c',
                'lib/libc/rpc/svc_run.c',
                'lib/libc/rpc/svc_simple.c',
                'lib/libc/rpc/svc_vc.c',
                'lib/libc/stdio/fgetln.c',
                'lib/libc/stdlib/strtonum.c',
                'lib/libc/string/strsep.c',
                'lib/libc/xdr/xdr_array.c',
                'lib/libc/xdr/xdr.c',
                'lib/libc/xdr/xdr_float.c',
                'lib/libc/xdr/xdr_mem.c',
                'lib/libc/xdr/xdr_rec.c',
                'lib/libc/xdr/xdr_reference.c',
                'lib/libc/xdr/xdr_sizeof.c',
                'lib/libc/xdr/xdr_stdio.c',
                'lib/libmemstat/memstat_all.c',
                'lib/libmemstat/memstat.c',
                'lib/libmemstat/memstat_malloc.c',
                'lib/libmemstat/memstat_uma.c',
                'lib/libutil/expand_number.c',
                'lib/libutil/humanize_number.c',
                'lib/libutil/trimdomain.c',
                'sbin/dhclient/alloc.c',
                'sbin/dhclient/bpf.c',
                'sbin/dhclient/clparse.c',
                'sbin/dhclient/conflex.c',
                'sbin/dhclient/convert.c',
                'sbin/dhclient/dhclient.c',
                'sbin/dhclient/dispatch.c',
                'sbin/dhclient/errwarn.c',
                'sbin/dhclient/hash.c',
                'sbin/dhclient/inet.c',
                'sbin/dhclient/options.c',
                'sbin/dhclient/packet.c',
                'sbin/dhclient/parse.c',
                'sbin/dhclient/privsep.c',
                'sbin/dhclient/tables.c',
                'sbin/dhclient/tree.c',
                'sbin/ifconfig/af_inet6.c',
                'sbin/ifconfig/af_inet.c',
                'sbin/ifconfig/af_link.c',
                'sbin/ifconfig/af_nd6.c',
                'sbin/ifconfig/ifbridge.c',
                'sbin/ifconfig/ifclone.c',
                'sbin/ifconfig/ifconfig.c',
                'sbin/ifconfig/ifgif.c',
                'sbin/ifconfig/ifgre.c',
                'sbin/ifconfig/ifgroup.c',
                'sbin/ifconfig/ifieee80211.c',
                'sbin/ifconfig/iflagg.c',
                'sbin/ifconfig/ifmac.c',
                'sbin/ifconfig/ifmedia.c',
                'sbin/ifconfig/ifpfsync.c',
                'sbin/ifconfig/ifvlan.c',
                'sbin/ifconfig/sfp.c',
                'sbin/pfctl/pfctl_altq.c',
                'sbin/pfctl/pfctl.c',
                'sbin/pfctl/pfctl_optimize.c',
                'sbin/pfctl/pfctl_osfp.c',
                'sbin/pfctl/pfctl_parser.c',
                'sbin/pfctl/pfctl_qstats.c',
                'sbin/pfctl/pfctl_radix.c',
                'sbin/pfctl/pfctl_table.c',
                'sbin/pfctl/pf_print_state.c',
                'sbin/ping6/ping6.c',
                'sbin/ping/ping.c',
                'sbin/route/route.c',
                'sbin/sysctl/sysctl.c',
                'usr.bin/netstat/bpf.c',
                'usr.bin/netstat/if.c',
                'usr.bin/netstat/inet6.c',
                'usr.bin/netstat/inet.c',
                'usr.bin/netstat/ipsec.c',
                'usr.bin/netstat/nl_symbols.c',
                'usr.bin/netstat/main.c',
                'usr.bin/netstat/mbuf.c',
                'usr.bin/netstat/mroute6.c',
                'usr.bin/netstat/mroute.c',
                'usr.bin/netstat/route.c',
                'usr.bin/netstat/pfkey.c',
                'usr.bin/netstat/sctp.c',
                'usr.bin/netstat/unix.c',
                'usr.bin/vmstat/vmstat.c',
                'usr.sbin/arp/arp.c',
            ],
            mm.generator['source'](['-DINET'])
        )

#
# User space: wlanstats utility
#
class user_space_wlanstats(builder.Module):

    def __init__(self, manager):
        super(user_space_wlanstats, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addUserSpaceHeaderFiles(
            [
                'tools/tools/net80211/wlanstats/wlanstats.h',
                'lib/libbsdstat/bsdstat.h',
            ]
        )
        self.addUserSpaceSourceFiles(
            [
                'tools/tools/net80211/wlanstats/main.c',
                'tools/tools/net80211/wlanstats/wlanstats.c',
                'lib/libbsdstat/bsdstat.c',
            ],
            mm.generator['source']([])
        )

#
# /crypto/openssl
#
class crypto_openssl(builder.Module):

    def __init__(self, manager):
        super(crypto_openssl, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addUserSpaceHeaderFiles(
            [
                'crypto/openssl/engines/e_cswift_err.h',
                'crypto/openssl/engines/e_aep_err.h',
                'crypto/openssl/engines/e_gmp_err.h',
                'crypto/openssl/engines/ccgost/gost2001_keyx.h',
                'crypto/openssl/engines/ccgost/e_gost_err.h',
                'crypto/openssl/engines/ccgost/gost_params.h',
                'crypto/openssl/engines/ccgost/gost_lcl.h',
                'crypto/openssl/engines/ccgost/gost89.h',
                'crypto/openssl/engines/ccgost/gost_keywrap.h',
                'crypto/openssl/engines/ccgost/gosthash.h',
                'crypto/openssl/engines/e_chil_err.h',
                'crypto/openssl/engines/e_4758cca_err.h',
                'crypto/openssl/engines/e_capi_err.h',
                'crypto/openssl/engines/e_nuron_err.h',
                'crypto/openssl/engines/e_atalla_err.h',
                'crypto/openssl/engines/vendor_defns/sureware.h',
                'crypto/openssl/engines/vendor_defns/cswift.h',
                'crypto/openssl/engines/vendor_defns/atalla.h',
                'crypto/openssl/engines/vendor_defns/hwcryptohook.h',
                'crypto/openssl/engines/vendor_defns/hw_4758_cca.h',
                'crypto/openssl/engines/vendor_defns/aep.h',
                'crypto/openssl/engines/vendor_defns/hw_ubsec.h',
                'crypto/openssl/engines/e_sureware_err.h',
                'crypto/openssl/engines/e_ubsec_err.h',
                'crypto/openssl/ssl/ssl2.h',
                'crypto/openssl/ssl/ssl3.h',
                'crypto/openssl/ssl/dtls1.h',
                'crypto/openssl/ssl/kssl_lcl.h',
                'crypto/openssl/ssl/srtp.h',
                'crypto/openssl/ssl/tls1.h',
                'crypto/openssl/ssl/ssl23.h',
                'crypto/openssl/ssl/ssl_locl.h',
                'crypto/openssl/ssl/kssl.h',
                'crypto/openssl/ssl/ssl.h',
                'crypto/openssl/e_os.h',
                'crypto/openssl/e_os2.h',
                'crypto/openssl/crypto/bio/bio_lcl.h',
                'crypto/openssl/crypto/bio/bio.h',
                'crypto/openssl/crypto/modes/modes_lcl.h',
                'crypto/openssl/crypto/modes/modes.h',
                'crypto/openssl/crypto/cms/cms.h',
                'crypto/openssl/crypto/cms/cms_lcl.h',
                'crypto/openssl/crypto/bn/bn_prime.h',
                'crypto/openssl/crypto/bn/bn_lcl.h',
                'crypto/openssl/crypto/bn/bn.h',
                'crypto/openssl/crypto/bn/rsaz_exp.h',
                'crypto/openssl/crypto/ecdh/ecdh.h',
                'crypto/openssl/crypto/ecdh/ech_locl.h',
                'crypto/openssl/crypto/dh/dh.h',
                'crypto/openssl/crypto/opensslv.h',
                'crypto/openssl/crypto/ripemd/ripemd.h',
                'crypto/openssl/crypto/ripemd/rmd_locl.h',
                'crypto/openssl/crypto/ripemd/rmdconst.h',
                'crypto/openssl/crypto/hmac/hmac.h',
                'crypto/openssl/crypto/rand/rand_lcl.h',
                'crypto/openssl/crypto/rand/rand.h',
                'crypto/openssl/crypto/stack/safestack.h',
                'crypto/openssl/crypto/stack/stack.h',
                'crypto/openssl/crypto/pem/pem.h',
                'crypto/openssl/crypto/pem/pem2.h',
                'crypto/openssl/crypto/evp/evp_locl.h',
                'crypto/openssl/crypto/evp/evp.h',
                'crypto/openssl/crypto/constant_time_locl.h',
                'crypto/openssl/crypto/txt_db/txt_db.h',
                'crypto/openssl/crypto/ebcdic.h',
                'crypto/openssl/crypto/cmac/cmac.h',
                'crypto/openssl/crypto/ppc_arch.h',
                'crypto/openssl/crypto/dsa/dsa.h',
                'crypto/openssl/crypto/dsa/dsa_locl.h',
                'crypto/openssl/crypto/md4/md4_locl.h',
                'crypto/openssl/crypto/md4/md4.h',
                'crypto/openssl/crypto/sha/sha_locl.h',
                'crypto/openssl/crypto/sha/sha.h',
                'crypto/openssl/crypto/arm_arch.h',
                'crypto/openssl/crypto/o_str.h',
                'crypto/openssl/crypto/ui/ui_locl.h',
                'crypto/openssl/crypto/ui/ui_compat.h',
                'crypto/openssl/crypto/ui/ui.h',
                'crypto/openssl/crypto/md32_common.h',
                'crypto/openssl/crypto/pkcs7/pkcs7.h',
                'crypto/openssl/crypto/crypto.h',
                'crypto/openssl/crypto/ossl_typ.h',
                'crypto/openssl/crypto/err/err.h',
                'crypto/openssl/crypto/sparc_arch.h',
                'crypto/openssl/crypto/dso/dso.h',
                'crypto/openssl/crypto/o_dir.h',
                'crypto/openssl/crypto/idea/idea.h',
                'crypto/openssl/crypto/idea/idea_lcl.h',
                'crypto/openssl/crypto/engine/eng_int.h',
                'crypto/openssl/crypto/engine/engine.h',
                'crypto/openssl/crypto/fips_err.h',
                'crypto/openssl/crypto/comp/comp.h',
                'crypto/openssl/crypto/ecdsa/ecdsa.h',
                'crypto/openssl/crypto/ecdsa/ecs_locl.h',
                'crypto/openssl/crypto/pkcs12/pkcs12.h',
                'crypto/openssl/crypto/srp/srp_lcl.h',
                'crypto/openssl/crypto/srp/srp_grps.h',
                'crypto/openssl/crypto/srp/srp.h',
                'crypto/openssl/crypto/x509v3/x509v3.h',
                'crypto/openssl/crypto/x509v3/pcy_int.h',
                'crypto/openssl/crypto/x509v3/ext_dat.h',
                'crypto/openssl/crypto/symhacks.h',
                'crypto/openssl/crypto/aes/aes.h',
                'crypto/openssl/crypto/aes/aes_locl.h',
                'crypto/openssl/crypto/bf/bf_locl.h',
                'crypto/openssl/crypto/bf/bf_pi.h',
                'crypto/openssl/crypto/bf/blowfish.h',
                'crypto/openssl/crypto/rc4/rc4_locl.h',
                'crypto/openssl/crypto/rc4/rc4.h',
                'crypto/openssl/crypto/ocsp/ocsp.h',
                'crypto/openssl/crypto/rc2/rc2.h',
                'crypto/openssl/crypto/rc2/rc2_locl.h',
                'crypto/openssl/crypto/md5/md5_locl.h',
                'crypto/openssl/crypto/md5/md5.h',
                'crypto/openssl/crypto/des/des_locl.h',
                'crypto/openssl/crypto/des/rpc_des.h',
                'crypto/openssl/crypto/des/des.h',
                'crypto/openssl/crypto/des/spr.h',
                'crypto/openssl/crypto/des/des_ver.h',
                'crypto/openssl/crypto/des/des_old.h',
                'crypto/openssl/crypto/cryptlib.h',
                'crypto/openssl/crypto/krb5/krb5_asn.h',
                'crypto/openssl/crypto/o_time.h',
                'crypto/openssl/crypto/opensslconf.h',
                'crypto/openssl/crypto/camellia/cmll_locl.h',
                'crypto/openssl/crypto/camellia/camellia.h',
                'crypto/openssl/crypto/x509/x509.h',
                'crypto/openssl/crypto/x509/vpm_int.h',
                'crypto/openssl/crypto/x509/x509_vfy.h',
                'crypto/openssl/crypto/seed/seed_locl.h',
                'crypto/openssl/crypto/seed/seed.h',
                'crypto/openssl/crypto/objects/obj_xref.h',
                'crypto/openssl/crypto/objects/obj_mac.h',
                'crypto/openssl/crypto/objects/objects.h',
                'crypto/openssl/crypto/objects/obj_dat.h',
                'crypto/openssl/crypto/ts/ts.h',
                'crypto/openssl/crypto/mdc2/mdc2.h',
                'crypto/openssl/crypto/pqueue/pqueue.h',
                'crypto/openssl/crypto/vms_rms.h',
                'crypto/openssl/crypto/buffer/buffer.h',
                'crypto/openssl/crypto/cast/cast.h',
                'crypto/openssl/crypto/cast/cast_s.h',
                'crypto/openssl/crypto/cast/cast_lcl.h',
                'crypto/openssl/crypto/rsa/rsa.h',
                'crypto/openssl/crypto/rsa/rsa_locl.h',
                'crypto/openssl/crypto/whrlpool/whrlpool.h',
                'crypto/openssl/crypto/whrlpool/wp_locl.h',
                'crypto/openssl/crypto/asn1/asn1.h',
                'crypto/openssl/crypto/asn1/asn1_int.h',
                'crypto/openssl/crypto/asn1/asn1_locl.h',
                'crypto/openssl/crypto/asn1/asn1_mac.h',
                'crypto/openssl/crypto/asn1/asn1t.h',
                'crypto/openssl/crypto/asn1/charmap.h',
                'crypto/openssl/crypto/ec/ec_lcl.h',
                'crypto/openssl/crypto/ec/ec.h',
                'crypto/openssl/crypto/conf/conf.h',
	        'crypto/openssl/crypto/conf/conf_def.h',
                'crypto/openssl/crypto/conf/conf_api.h',
                'crypto/openssl/crypto/lhash/lhash.h',
            ]
        )
        self.addUserSpaceSourceFiles(
            [
                'crypto/openssl/engines/ccgost/e_gost_err.c',
                'crypto/openssl/engines/ccgost/gost_sign.c',
                'crypto/openssl/engines/ccgost/gost_params.c',
                'crypto/openssl/engines/ccgost/gost_md.c',
                'crypto/openssl/engines/ccgost/gost2001_keyx.c',
                'crypto/openssl/engines/ccgost/gostsum.c',
                'crypto/openssl/engines/ccgost/gost_eng.c',
                'crypto/openssl/engines/ccgost/gost_asn1.c',
                'crypto/openssl/engines/ccgost/gost_ameth.c',
                'crypto/openssl/engines/ccgost/gost_keywrap.c',
                'crypto/openssl/engines/ccgost/gost_pmeth.c',
                'crypto/openssl/engines/ccgost/gost2001.c',
                'crypto/openssl/engines/ccgost/gost94_keyx.c',
                'crypto/openssl/engines/ccgost/gost89.c',
                'crypto/openssl/engines/ccgost/gost_crypt.c',
                'crypto/openssl/engines/ccgost/gosthash.c',
                'crypto/openssl/engines/ccgost/gost_ctl.c',
                'crypto/openssl/engines/e_4758cca_err.c',
                'crypto/openssl/engines/e_gmp.c',
                'crypto/openssl/engines/e_sureware_err.c',
                'crypto/openssl/engines/e_ubsec.c',
                'crypto/openssl/engines/e_gmp_err.c',
                'crypto/openssl/engines/e_ubsec_err.c',
                'crypto/openssl/engines/e_cswift_err.c',
                'crypto/openssl/engines/e_chil_err.c',
                'crypto/openssl/engines/e_aep_err.c',
                'crypto/openssl/engines/e_4758cca.c',
                'crypto/openssl/engines/e_chil.c',
                'crypto/openssl/engines/e_nuron.c',
                'crypto/openssl/engines/e_aep.c',
                'crypto/openssl/engines/e_cswift.c',
                'crypto/openssl/engines/e_atalla_err.c',
                'crypto/openssl/engines/e_capi.c',
                'crypto/openssl/engines/e_nuron_err.c',
                'crypto/openssl/engines/e_atalla.c',
                'crypto/openssl/engines/e_capi_err.c',
                'crypto/openssl/engines/e_padlock.c',
                'crypto/openssl/engines/e_sureware.c',
                'crypto/openssl/ssl/ssl_utst.c',
                'crypto/openssl/ssl/t1_ext.c',
                'crypto/openssl/ssl/ssltest.c',
                'crypto/openssl/ssl/s2_pkt.c',
                'crypto/openssl/ssl/s23_pkt.c',
                'crypto/openssl/ssl/t1_enc.c',
                'crypto/openssl/ssl/s23_lib.c',
                'crypto/openssl/ssl/s23_clnt.c',
                'crypto/openssl/ssl/s23_meth.c',
                'crypto/openssl/ssl/ssl_stat.c',
                'crypto/openssl/ssl/s3_srvr.c',
                'crypto/openssl/ssl/s2_clnt.c',
                'crypto/openssl/ssl/t1_reneg.c',
                'crypto/openssl/ssl/s3_pkt.c',
                'crypto/openssl/ssl/s3_cbc.c',
                'crypto/openssl/ssl/t1_clnt.c',
                'crypto/openssl/ssl/kssl.c',
                'crypto/openssl/ssl/s3_both.c',
                'crypto/openssl/ssl/t1_meth.c',
                'crypto/openssl/ssl/s3_enc.c',
                'crypto/openssl/ssl/tls_srp.c',
                'crypto/openssl/ssl/d1_srvr.c',
                'crypto/openssl/ssl/ssl_cert.c',
                'crypto/openssl/ssl/ssl_rsa.c',
                'crypto/openssl/ssl/ssl_lib.c',
                'crypto/openssl/ssl/s2_lib.c',
                'crypto/openssl/ssl/clienthellotest.c',
                'crypto/openssl/ssl/ssl_err2.c',
                'crypto/openssl/ssl/ssl_asn1.c',
                'crypto/openssl/ssl/d1_pkt.c',
                'crypto/openssl/ssl/t1_srvr.c',
                'crypto/openssl/ssl/s2_enc.c',
                'crypto/openssl/ssl/ssl_conf.c',
                'crypto/openssl/ssl/s3_lib.c',
                'crypto/openssl/ssl/sslv2conftest.c',
                'crypto/openssl/ssl/d1_meth.c',
                'crypto/openssl/ssl/ssl_algs.c',
                'crypto/openssl/ssl/t1_lib.c',
                'crypto/openssl/ssl/ssl_err.c',
                'crypto/openssl/ssl/d1_clnt.c',
                'crypto/openssl/ssl/t1_trce.c',
                'crypto/openssl/ssl/s3_meth.c',
                'crypto/openssl/ssl/d1_lib.c',
                'crypto/openssl/ssl/ssl_ciph.c',
                'crypto/openssl/ssl/ssl_sess.c',
                'crypto/openssl/ssl/ssl_txt.c',
                'crypto/openssl/ssl/bio_ssl.c',
                'crypto/openssl/ssl/s2_srvr.c',
                'crypto/openssl/ssl/s3_clnt.c',
                'crypto/openssl/ssl/s23_srvr.c',
                'crypto/openssl/ssl/d1_srtp.c',
                'crypto/openssl/ssl/d1_both.c',
                'crypto/openssl/ssl/s2_meth.c',
                'crypto/openssl/ssl/bad_dtls_test.c',
                'crypto/openssl/crypto/bio/bss_bio.c',
                'crypto/openssl/crypto/bio/bf_lbuf.c',
                'crypto/openssl/crypto/bio/b_dump.c',
                'crypto/openssl/crypto/bio/bf_buff.c',
                'crypto/openssl/crypto/bio/bf_nbio.c',
                'crypto/openssl/crypto/bio/bss_file.c',
                'crypto/openssl/crypto/bio/bss_acpt.c',
                'crypto/openssl/crypto/bio/bf_null.c',
                'crypto/openssl/crypto/bio/bss_dgram.c',
                'crypto/openssl/crypto/bio/bss_fd.c',
                'crypto/openssl/crypto/bio/bss_sock.c',
                'crypto/openssl/crypto/bio/bio_lib.c',
                'crypto/openssl/crypto/bio/b_sock.c',
                'crypto/openssl/crypto/bio/bio_err.c',
                'crypto/openssl/crypto/bio/bio_cb.c',
                'crypto/openssl/crypto/bio/bss_mem.c',
                'crypto/openssl/crypto/bio/bss_conn.c',
                'crypto/openssl/crypto/bio/bss_log.c',
                'crypto/openssl/crypto/bio/b_print.c',
                'crypto/openssl/crypto/bio/bss_null.c',
                'crypto/openssl/crypto/modes/cbc128.c',
                'crypto/openssl/crypto/modes/ccm128.c',
                'crypto/openssl/crypto/modes/wrap128.c',
                'crypto/openssl/crypto/modes/cts128.c',
                'crypto/openssl/crypto/modes/xts128.c',
                'crypto/openssl/crypto/modes/ctr128.c',
                'crypto/openssl/crypto/modes/gcm128.c',
                'crypto/openssl/crypto/modes/ofb128.c',
                'crypto/openssl/crypto/modes/cfb128.c',
                'crypto/openssl/crypto/fips_ers.c',
                'crypto/openssl/crypto/o_time.c',
                'crypto/openssl/crypto/cms/cms_sd.c',
                'crypto/openssl/crypto/cms/cms_enc.c',
                'crypto/openssl/crypto/cms/cms_att.c',
                'crypto/openssl/crypto/cms/cms_env.c',
                'crypto/openssl/crypto/cms/cms_ess.c',
                'crypto/openssl/crypto/cms/cms_kari.c',
                'crypto/openssl/crypto/cms/cms_cd.c',
                'crypto/openssl/crypto/cms/cms_dd.c',
                'crypto/openssl/crypto/cms/cms_io.c',
                'crypto/openssl/crypto/cms/cms_lib.c',
                'crypto/openssl/crypto/cms/cms_err.c',
                'crypto/openssl/crypto/cms/cms_pwri.c',
                'crypto/openssl/crypto/cms/cms_smime.c',
                'crypto/openssl/crypto/cms/cms_asn1.c',
                'crypto/openssl/crypto/uid.c',
                'crypto/openssl/crypto/bn/bn_word.c',
                'crypto/openssl/crypto/bn/bn_mul.c',
                'crypto/openssl/crypto/bn/bn_gf2m.c',
                'crypto/openssl/crypto/bn/bn_gcd.c',
                'crypto/openssl/crypto/bn/bn_exp2.c',
                'crypto/openssl/crypto/bn/expspeed.c',
                'crypto/openssl/crypto/bn/bn_print.c',
                'crypto/openssl/crypto/bn/bn_add.c',
                'crypto/openssl/crypto/bn/bn_mpi.c',
                'crypto/openssl/crypto/bn/exptest.c',
                'crypto/openssl/crypto/bn/bn_prime.c',
                'crypto/openssl/crypto/bn/rsaz_exp.c',
                'crypto/openssl/crypto/bn/bn_lib.c',
                'crypto/openssl/crypto/bn/bn_exp.c',
                'crypto/openssl/crypto/bn/bn_kron.c',
                'crypto/openssl/crypto/bn/bn_const.c',
                'crypto/openssl/crypto/bn/bnspeed.c',
                'crypto/openssl/crypto/bn/bn_recp.c',
                'crypto/openssl/crypto/bn/bn_rand.c',
                'crypto/openssl/crypto/bn/bn_div.c',
                'crypto/openssl/crypto/bn/bn_mod.c',
                'crypto/openssl/crypto/bn/bn_sqr.c',
                'crypto/openssl/crypto/bn/bn_depr.c',
                'crypto/openssl/crypto/bn/bn_shift.c',
                'crypto/openssl/crypto/bn/bn_ctx.c',
                'crypto/openssl/crypto/bn/bn_mont.c',
                'crypto/openssl/crypto/bn/divtest.c',
                'crypto/openssl/crypto/bn/bn_nist.c',
                'crypto/openssl/crypto/bn/bn_sqrt.c',
                'crypto/openssl/crypto/bn/bn_err.c',
                'crypto/openssl/crypto/bn/bn_x931p.c',
                'crypto/openssl/crypto/bn/bn_asm.c',
                'crypto/openssl/crypto/bn/bntest.c',
                'crypto/openssl/crypto/bn/bn_blind.c',
                'crypto/openssl/crypto/mem.c',
                'crypto/openssl/crypto/ecdh/ech_err.c',
                'crypto/openssl/crypto/ecdh/ecdhtest.c',
                'crypto/openssl/crypto/ecdh/ech_ossl.c',
                'crypto/openssl/crypto/ecdh/ech_kdf.c',
                'crypto/openssl/crypto/ecdh/ech_key.c',
                'crypto/openssl/crypto/ecdh/ech_lib.c',
                'crypto/openssl/crypto/dh/dh_rfc5114.c',
                'crypto/openssl/crypto/dh/dh_lib.c',
                'crypto/openssl/crypto/dh/p192.c',
                'crypto/openssl/crypto/dh/p512.c',
                'crypto/openssl/crypto/dh/dh_key.c',
                'crypto/openssl/crypto/dh/p1024.c',
                'crypto/openssl/crypto/dh/dh_kdf.c',
                'crypto/openssl/crypto/dh/dh_gen.c',
                'crypto/openssl/crypto/dh/dh_ameth.c',
                'crypto/openssl/crypto/dh/dh_prn.c',
                'crypto/openssl/crypto/dh/dh_depr.c',
                'crypto/openssl/crypto/dh/dh_err.c',
                'crypto/openssl/crypto/dh/dh_pmeth.c',
                'crypto/openssl/crypto/dh/dh_asn1.c',
                'crypto/openssl/crypto/dh/dh_check.c',
                'crypto/openssl/crypto/dh/dhtest.c',
                'crypto/openssl/crypto/ripemd/rmd_one.c',
                'crypto/openssl/crypto/ripemd/rmd_dgst.c',
                'crypto/openssl/crypto/ripemd/rmdtest.c',
                'crypto/openssl/crypto/ripemd/rmd160.c',
                'crypto/openssl/crypto/hmac/hm_pmeth.c',
                'crypto/openssl/crypto/hmac/hm_ameth.c',
                'crypto/openssl/crypto/hmac/hmactest.c',
                'crypto/openssl/crypto/hmac/hmac.c',
                'crypto/openssl/crypto/rand/md_rand.c',
                'crypto/openssl/crypto/rand/rand_egd.c',
                'crypto/openssl/crypto/rand/randfile.c',
                'crypto/openssl/crypto/rand/rand_err.c',
                'crypto/openssl/crypto/rand/rand_unix.c',
                'crypto/openssl/crypto/rand/randtest.c',
                'crypto/openssl/crypto/rand/rand_lib.c',
                'crypto/openssl/crypto/stack/stack.c',
                'crypto/openssl/crypto/pem/pem_oth.c',
                'crypto/openssl/crypto/pem/pem_all.c',
                'crypto/openssl/crypto/pem/pem_info.c',
                'crypto/openssl/crypto/pem/pem_pk8.c',
                'crypto/openssl/crypto/pem/pem_err.c',
                'crypto/openssl/crypto/pem/pem_sign.c',
                'crypto/openssl/crypto/pem/pem_lib.c',
                'crypto/openssl/crypto/pem/pem_x509.c',
                'crypto/openssl/crypto/pem/pem_seal.c',
                'crypto/openssl/crypto/pem/pvkfmt.c',
                'crypto/openssl/crypto/pem/pem_pkey.c',
                'crypto/openssl/crypto/pem/pem_xaux.c',
                'crypto/openssl/crypto/evp/e_camellia.c',
                'crypto/openssl/crypto/evp/p_enc.c',
                'crypto/openssl/crypto/evp/m_sha.c',
                'crypto/openssl/crypto/evp/e_rc2.c',
                'crypto/openssl/crypto/evp/e_old.c',
                'crypto/openssl/crypto/evp/e_xcbc_d.c',
                'crypto/openssl/crypto/evp/e_des3.c',
                'crypto/openssl/crypto/evp/evp_err.c',
                'crypto/openssl/crypto/evp/p_lib.c',
                'crypto/openssl/crypto/evp/digest.c',
                'crypto/openssl/crypto/evp/m_wp.c',
                'crypto/openssl/crypto/evp/p_seal.c',
                'crypto/openssl/crypto/evp/e_null.c',
                'crypto/openssl/crypto/evp/e_aes.c',
                'crypto/openssl/crypto/evp/m_md2.c',
                'crypto/openssl/crypto/evp/p_dec.c',
                'crypto/openssl/crypto/evp/e_idea.c',
                'crypto/openssl/crypto/evp/evp_pkey.c',
                'crypto/openssl/crypto/evp/p_open.c',
                'crypto/openssl/crypto/evp/c_allc.c',
                'crypto/openssl/crypto/evp/m_mdc2.c',
                'crypto/openssl/crypto/evp/evp_pbe.c',
                'crypto/openssl/crypto/evp/pmeth_lib.c',
                'crypto/openssl/crypto/evp/c_alld.c',
                'crypto/openssl/crypto/evp/e_aes_cbc_hmac_sha1.c',
                'crypto/openssl/crypto/evp/openbsd_hw.c',
                'crypto/openssl/crypto/evp/p_verify.c',
                'crypto/openssl/crypto/evp/evp_acnf.c',
                'crypto/openssl/crypto/evp/evp_enc.c',
                'crypto/openssl/crypto/evp/m_md5.c',
                'crypto/openssl/crypto/evp/evp_extra_test.c',
                'crypto/openssl/crypto/evp/m_ripemd.c',
                'crypto/openssl/crypto/evp/p_sign.c',
                'crypto/openssl/crypto/evp/bio_ok.c',
                'crypto/openssl/crypto/evp/e_rc5.c',
                'crypto/openssl/crypto/evp/e_des.c',
                'crypto/openssl/crypto/evp/evp_cnf.c',
                'crypto/openssl/crypto/evp/e_aes_cbc_hmac_sha256.c',
                'crypto/openssl/crypto/evp/e_cast.c',
                'crypto/openssl/crypto/evp/bio_enc.c',
                'crypto/openssl/crypto/evp/m_md4.c',
                'crypto/openssl/crypto/evp/p5_crpt.c',
                'crypto/openssl/crypto/evp/evp_lib.c',
                'crypto/openssl/crypto/evp/bio_b64.c',
                'crypto/openssl/crypto/evp/evp_test.c',
                'crypto/openssl/crypto/evp/c_all.c',
                'crypto/openssl/crypto/evp/m_null.c',
                'crypto/openssl/crypto/evp/e_rc4.c',
                'crypto/openssl/crypto/evp/m_sigver.c',
                'crypto/openssl/crypto/evp/m_sha1.c',
                'crypto/openssl/crypto/evp/e_seed.c',
                'crypto/openssl/crypto/evp/evp_key.c',
                'crypto/openssl/crypto/evp/e_rc4_hmac_md5.c',
                'crypto/openssl/crypto/evp/e_bf.c',
                'crypto/openssl/crypto/evp/pmeth_fn.c',
                'crypto/openssl/crypto/evp/m_ecdsa.c',
                'crypto/openssl/crypto/evp/pmeth_gn.c',
                'crypto/openssl/crypto/evp/p5_crpt2.c',
                'crypto/openssl/crypto/evp/bio_md.c',
                'crypto/openssl/crypto/evp/names.c',
                'crypto/openssl/crypto/evp/encode.c',
                'crypto/openssl/crypto/evp/m_dss.c',
                'crypto/openssl/crypto/evp/m_dss1.c',
                'crypto/openssl/crypto/ppccap.c',
                'crypto/openssl/crypto/txt_db/txt_db.c',
                'crypto/openssl/crypto/s390xcap.c',
                'crypto/openssl/crypto/cmac/cm_pmeth.c',
                'crypto/openssl/crypto/cmac/cmac.c',
                'crypto/openssl/crypto/cmac/cm_ameth.c',
                'crypto/openssl/crypto/dsa/dsa_key.c',
                'crypto/openssl/crypto/dsa/dsa_err.c',
                'crypto/openssl/crypto/dsa/dsa_gen.c',
                'crypto/openssl/crypto/dsa/dsa_ossl.c',
                'crypto/openssl/crypto/dsa/dsa_lib.c',
                'crypto/openssl/crypto/dsa/dsa_vrf.c',
                'crypto/openssl/crypto/dsa/dsa_sign.c',
                'crypto/openssl/crypto/dsa/dsagen.c',
                'crypto/openssl/crypto/dsa/dsa_ameth.c',
                'crypto/openssl/crypto/dsa/dsa_pmeth.c',
                'crypto/openssl/crypto/dsa/dsa_depr.c',
                'crypto/openssl/crypto/dsa/dsa_asn1.c',
                'crypto/openssl/crypto/dsa/dsa_prn.c',
                'crypto/openssl/crypto/dsa/dsatest.c',
                'crypto/openssl/crypto/md4/md4test.c',
                'crypto/openssl/crypto/md4/md4_dgst.c',
                'crypto/openssl/crypto/md4/md4.c',
                'crypto/openssl/crypto/md4/md4_one.c',
                'crypto/openssl/crypto/sha/shatest.c',
                'crypto/openssl/crypto/sha/sha1.c',
                'crypto/openssl/crypto/sha/sha1test.c',
                'crypto/openssl/crypto/sha/sha_dgst.c',
                'crypto/openssl/crypto/sha/sha512.c',
                'crypto/openssl/crypto/sha/sha_one.c',
                'crypto/openssl/crypto/sha/sha256.c',
                'crypto/openssl/crypto/sha/sha1dgst.c',
                'crypto/openssl/crypto/sha/sha1_one.c',
                'crypto/openssl/crypto/sha/sha.c',
                'crypto/openssl/crypto/sha/sha256t.c',
                'crypto/openssl/crypto/sha/sha512t.c',
                'crypto/openssl/crypto/ui/ui_openssl.c',
                'crypto/openssl/crypto/ui/ui_compat.c',
                'crypto/openssl/crypto/ui/ui_lib.c',
                'crypto/openssl/crypto/ui/ui_err.c',
                'crypto/openssl/crypto/ui/ui_util.c',
                'crypto/openssl/crypto/pkcs7/pkcs7err.c',
                'crypto/openssl/crypto/pkcs7/pk7_dgst.c',
                'crypto/openssl/crypto/pkcs7/pk7_lib.c',
                'crypto/openssl/crypto/pkcs7/pk7_smime.c',
                'crypto/openssl/crypto/pkcs7/pk7_mime.c',
                'crypto/openssl/crypto/pkcs7/bio_pk7.c',
                'crypto/openssl/crypto/pkcs7/pk7_asn1.c',
                'crypto/openssl/crypto/pkcs7/pk7_attr.c',
                'crypto/openssl/crypto/pkcs7/pk7_doit.c',
                'crypto/openssl/crypto/err/err_all.c',
                'crypto/openssl/crypto/err/err.c',
                'crypto/openssl/crypto/err/err_prn.c',
                'crypto/openssl/crypto/ex_data.c',
                'crypto/openssl/crypto/dso/dso_openssl.c',
                'crypto/openssl/crypto/dso/dso_dl.c',
                'crypto/openssl/crypto/dso/dso_beos.c',
                'crypto/openssl/crypto/dso/dso_dlfcn.c',
                'crypto/openssl/crypto/dso/dso_err.c',
                'crypto/openssl/crypto/dso/dso_null.c',
                'crypto/openssl/crypto/dso/dso_lib.c',
                'crypto/openssl/crypto/idea/i_ecb.c',
                'crypto/openssl/crypto/idea/i_cfb64.c',
                'crypto/openssl/crypto/idea/i_ofb64.c',
                'crypto/openssl/crypto/idea/ideatest.c',
                'crypto/openssl/crypto/idea/i_cbc.c',
                'crypto/openssl/crypto/idea/i_skey.c',
                'crypto/openssl/crypto/idea/idea_spd.c',
                'crypto/openssl/crypto/armcap.c',
                'crypto/openssl/crypto/engine/tb_asnmth.c',
                'crypto/openssl/crypto/engine/eng_dyn.c',
                'crypto/openssl/crypto/engine/eng_openssl.c',
                'crypto/openssl/crypto/engine/eng_all.c',
                'crypto/openssl/crypto/engine/tb_ecdsa.c',
                'crypto/openssl/crypto/engine/eng_lib.c',
                'crypto/openssl/crypto/engine/eng_err.c',
                'crypto/openssl/crypto/engine/tb_store.c',
                'crypto/openssl/crypto/engine/eng_init.c',
                'crypto/openssl/crypto/engine/tb_dsa.c',
                'crypto/openssl/crypto/engine/eng_pkey.c',
                'crypto/openssl/crypto/engine/eng_cnf.c',
                'crypto/openssl/crypto/engine/eng_ctrl.c',
                'crypto/openssl/crypto/engine/eng_list.c',
                'crypto/openssl/crypto/engine/tb_cipher.c',
                'crypto/openssl/crypto/engine/tb_dh.c',
                'crypto/openssl/crypto/engine/eng_rdrand.c',
                'crypto/openssl/crypto/engine/tb_rsa.c',
                'crypto/openssl/crypto/engine/tb_pkmeth.c',
                'crypto/openssl/crypto/engine/eng_cryptodev.c',
                'crypto/openssl/crypto/engine/tb_digest.c',
                'crypto/openssl/crypto/engine/tb_ecdh.c',
                'crypto/openssl/crypto/engine/enginetest.c',
                'crypto/openssl/crypto/engine/eng_fat.c',
                'crypto/openssl/crypto/engine/tb_rand.c',
                'crypto/openssl/crypto/engine/eng_table.c',
                'crypto/openssl/crypto/cryptlib.c',
                'crypto/openssl/crypto/comp/comp_lib.c',
                'crypto/openssl/crypto/comp/c_rle.c',
                'crypto/openssl/crypto/comp/c_zlib.c',
                'crypto/openssl/crypto/comp/comp_err.c',
                'crypto/openssl/crypto/ecdsa/ecs_sign.c',
                'crypto/openssl/crypto/ecdsa/ecs_vrf.c',
                'crypto/openssl/crypto/ecdsa/ecdsatest.c',
                'crypto/openssl/crypto/ecdsa/ecs_ossl.c',
                'crypto/openssl/crypto/ecdsa/ecs_asn1.c',
                'crypto/openssl/crypto/ecdsa/ecs_err.c',
                'crypto/openssl/crypto/ecdsa/ecs_lib.c',
                'crypto/openssl/crypto/pkcs12/p12_key.c',
                'crypto/openssl/crypto/pkcs12/pk12err.c',
                'crypto/openssl/crypto/pkcs12/p12_asn.c',
                'crypto/openssl/crypto/pkcs12/p12_crpt.c',
                'crypto/openssl/crypto/pkcs12/p12_init.c',
                'crypto/openssl/crypto/pkcs12/p12_p8e.c',
                'crypto/openssl/crypto/pkcs12/p12_p8d.c',
                'crypto/openssl/crypto/pkcs12/p12_mutl.c',
                'crypto/openssl/crypto/pkcs12/p12_utl.c',
                'crypto/openssl/crypto/pkcs12/p12_kiss.c',
                'crypto/openssl/crypto/pkcs12/p12_decr.c',
                'crypto/openssl/crypto/pkcs12/p12_crt.c',
                'crypto/openssl/crypto/pkcs12/p12_add.c',
                'crypto/openssl/crypto/pkcs12/p12_npas.c',
                'crypto/openssl/crypto/pkcs12/p12_attr.c',
                'crypto/openssl/crypto/srp/srp_lib.c',
                'crypto/openssl/crypto/srp/srp_vfy.c',
                'crypto/openssl/crypto/srp/srptest.c',
                'crypto/openssl/crypto/x509v3/pcy_lib.c',
                'crypto/openssl/crypto/x509v3/v3_pmaps.c',
                'crypto/openssl/crypto/x509v3/v3_akeya.c',
                'crypto/openssl/crypto/x509v3/pcy_cache.c',
                'crypto/openssl/crypto/x509v3/v3_extku.c',
                'crypto/openssl/crypto/x509v3/v3_bcons.c',
                'crypto/openssl/crypto/x509v3/v3_scts.c',
                'crypto/openssl/crypto/x509v3/v3_utl.c',
                'crypto/openssl/crypto/x509v3/v3_ia5.c',
                'crypto/openssl/crypto/x509v3/v3nametest.c',
                'crypto/openssl/crypto/x509v3/v3_pci.c',
                'crypto/openssl/crypto/x509v3/v3_int.c',
                'crypto/openssl/crypto/x509v3/v3_crld.c',
                'crypto/openssl/crypto/x509v3/v3_bitst.c',
                'crypto/openssl/crypto/x509v3/v3_cpols.c',
                'crypto/openssl/crypto/x509v3/v3_sxnet.c',
                'crypto/openssl/crypto/x509v3/v3_alt.c',
                'crypto/openssl/crypto/x509v3/pcy_node.c',
                'crypto/openssl/crypto/x509v3/tabtest.c',
                'crypto/openssl/crypto/x509v3/v3_pcia.c',
                'crypto/openssl/crypto/x509v3/v3_pku.c',
                'crypto/openssl/crypto/x509v3/v3_info.c',
                'crypto/openssl/crypto/x509v3/v3_genn.c',
                'crypto/openssl/crypto/x509v3/pcy_data.c',
                'crypto/openssl/crypto/x509v3/v3_enum.c',
                'crypto/openssl/crypto/x509v3/v3err.c',
                'crypto/openssl/crypto/x509v3/v3_lib.c',
                'crypto/openssl/crypto/x509v3/v3_pcons.c',
                'crypto/openssl/crypto/x509v3/pcy_tree.c',
                'crypto/openssl/crypto/x509v3/v3_conf.c',
                'crypto/openssl/crypto/x509v3/v3_asid.c',
                'crypto/openssl/crypto/x509v3/v3_akey.c',
                'crypto/openssl/crypto/x509v3/v3_addr.c',
                'crypto/openssl/crypto/x509v3/pcy_map.c',
                'crypto/openssl/crypto/x509v3/v3_ocsp.c',
                'crypto/openssl/crypto/x509v3/v3prin.c',
                'crypto/openssl/crypto/x509v3/v3_skey.c',
                'crypto/openssl/crypto/x509v3/v3_purp.c',
                'crypto/openssl/crypto/x509v3/v3_ncons.c',
                'crypto/openssl/crypto/x509v3/v3_prn.c',
                'crypto/openssl/crypto/aes/aes_core.c',
                'crypto/openssl/crypto/aes/aes_ige.c',
                'crypto/openssl/crypto/aes/aes_ofb.c',
                'crypto/openssl/crypto/aes/aes_cfb.c',
                'crypto/openssl/crypto/aes/aes_cbc.c',
                'crypto/openssl/crypto/aes/aes_ctr.c',
                'crypto/openssl/crypto/aes/aes_misc.c',
                'crypto/openssl/crypto/aes/aes_ecb.c',
                'crypto/openssl/crypto/aes/aes_wrap.c',
                'crypto/openssl/crypto/o_dir.c',
                'crypto/openssl/crypto/bf/bf_opts.c',
                'crypto/openssl/crypto/bf/bf_ofb64.c',
                'crypto/openssl/crypto/bf/bf_skey.c',
                'crypto/openssl/crypto/bf/bftest.c',
                'crypto/openssl/crypto/bf/bf_enc.c',
                'crypto/openssl/crypto/bf/bfspeed.c',
                'crypto/openssl/crypto/bf/bf_cfb64.c',
                'crypto/openssl/crypto/bf/bf_ecb.c',
                'crypto/openssl/crypto/rc4/rc4_utl.c',
                'crypto/openssl/crypto/rc4/rc4_enc.c',
                'crypto/openssl/crypto/rc4/rc4speed.c',
                'crypto/openssl/crypto/rc4/rc4_skey.c',
                'crypto/openssl/crypto/rc4/rc4test.c',
                'crypto/openssl/crypto/ocsp/ocsp_err.c',
                'crypto/openssl/crypto/ocsp/ocsp_prn.c',
                'crypto/openssl/crypto/ocsp/ocsp_cl.c',
                'crypto/openssl/crypto/ocsp/ocsp_vfy.c',
                'crypto/openssl/crypto/ocsp/ocsp_lib.c',
                'crypto/openssl/crypto/ocsp/ocsp_srv.c',
                'crypto/openssl/crypto/ocsp/ocsp_asn.c',
                'crypto/openssl/crypto/ocsp/ocsp_ht.c',
                'crypto/openssl/crypto/ocsp/ocsp_ext.c',
                'crypto/openssl/crypto/rc2/rc2speed.c',
                'crypto/openssl/crypto/rc2/rc2_skey.c',
                'crypto/openssl/crypto/rc2/rc2_ecb.c',
                'crypto/openssl/crypto/rc2/rc2ofb64.c',
                'crypto/openssl/crypto/rc2/rc2test.c',
                'crypto/openssl/crypto/rc2/tab.c',
                'crypto/openssl/crypto/rc2/rc2cfb64.c',
                'crypto/openssl/crypto/rc2/rc2_cbc.c',
                'crypto/openssl/crypto/md5/md5_one.c',
                'crypto/openssl/crypto/md5/md5test.c',
                'crypto/openssl/crypto/md5/md5_dgst.c',
                'crypto/openssl/crypto/md5/md5.c',
                'crypto/openssl/crypto/des/rpw.c',
                'crypto/openssl/crypto/des/cbc_cksm.c',
                'crypto/openssl/crypto/des/speed.c',
                'crypto/openssl/crypto/des/str2key.c',
                'crypto/openssl/crypto/des/rpc_enc.c',
                'crypto/openssl/crypto/des/ecb_enc.c',
                'crypto/openssl/crypto/des/cfb64enc.c',
                'crypto/openssl/crypto/des/des_old2.c',
                'crypto/openssl/crypto/des/fcrypt_b.c',
                'crypto/openssl/crypto/des/cfb_enc.c',
                'crypto/openssl/crypto/des/ofb64ede.c',
                'crypto/openssl/crypto/des/cbc_enc.c',
                'crypto/openssl/crypto/des/set_key.c',
                'crypto/openssl/crypto/des/enc_read.c',
                'crypto/openssl/crypto/des/cbc3_enc.c',
                'crypto/openssl/crypto/des/ncbc_enc.c',
                'crypto/openssl/crypto/des/ofb64enc.c',
                'crypto/openssl/crypto/des/qud_cksm.c',
                'crypto/openssl/crypto/des/read_pwd.c',
                'crypto/openssl/crypto/des/cfb64ede.c',
                'crypto/openssl/crypto/des/des.c',
                'crypto/openssl/crypto/des/fcrypt.c',
                'crypto/openssl/crypto/des/ofb_enc.c',
                'crypto/openssl/crypto/des/rand_key.c',
                'crypto/openssl/crypto/des/enc_writ.c',
                'crypto/openssl/crypto/des/xcbc_enc.c',
                'crypto/openssl/crypto/des/ede_cbcm_enc.c',
                'crypto/openssl/crypto/des/read2pwd.c',
                'crypto/openssl/crypto/des/pcbc_enc.c',
                'crypto/openssl/crypto/des/des_enc.c',
                'crypto/openssl/crypto/des/ecb3_enc.c',
                'crypto/openssl/crypto/cpt_err.c',
                'crypto/openssl/crypto/o_init.c',
                'crypto/openssl/crypto/krb5/krb5_asn.c',
                'crypto/openssl/crypto/mem_dbg.c',
                'crypto/openssl/crypto/constant_time_test.c',
                'crypto/openssl/crypto/ebcdic.c',
                'crypto/openssl/crypto/camellia/camellia.c',
                'crypto/openssl/crypto/camellia/cmll_misc.c',
                'crypto/openssl/crypto/camellia/cmll_cbc.c',
                'crypto/openssl/crypto/camellia/cmll_utl.c',
                'crypto/openssl/crypto/camellia/cmll_ofb.c',
                'crypto/openssl/crypto/camellia/cmll_ecb.c',
                'crypto/openssl/crypto/camellia/cmll_ctr.c',
                'crypto/openssl/crypto/camellia/cmll_cfb.c',
                'crypto/openssl/crypto/x509/x509_r2x.c',
                'crypto/openssl/crypto/x509/x509type.c',
                'crypto/openssl/crypto/x509/x509_vfy.c',
                'crypto/openssl/crypto/x509/x509_set.c',
                'crypto/openssl/crypto/x509/x509_lu.c',
                'crypto/openssl/crypto/x509/x509_trs.c',
                'crypto/openssl/crypto/x509/verify_extra_test.c',
                'crypto/openssl/crypto/x509/x509rset.c',
                'crypto/openssl/crypto/x509/x509_ext.c',
                'crypto/openssl/crypto/x509/x_all.c',
                'crypto/openssl/crypto/x509/x509_vpm.c',
                'crypto/openssl/crypto/x509/x509_v3.c',
                'crypto/openssl/crypto/x509/x509_err.c',
                'crypto/openssl/crypto/x509/by_file.c',
                'crypto/openssl/crypto/x509/x509_txt.c',
                'crypto/openssl/crypto/x509/x509_obj.c',
                'crypto/openssl/crypto/x509/x509_att.c',
                'crypto/openssl/crypto/x509/by_dir.c',
                'crypto/openssl/crypto/x509/x509_cmp.c',
                'crypto/openssl/crypto/x509/x509spki.c',
                'crypto/openssl/crypto/x509/x509_def.c',
                'crypto/openssl/crypto/x509/x509_d2.c',
                'crypto/openssl/crypto/x509/x509_req.c',
                'crypto/openssl/crypto/x509/x509cset.c',
                'crypto/openssl/crypto/x509/x509name.c',
                'crypto/openssl/crypto/threads/th-lock.c',
                'crypto/openssl/crypto/threads/mttest.c',
                'crypto/openssl/crypto/seed/seed_cfb.c',
                'crypto/openssl/crypto/seed/seed_cbc.c',
                'crypto/openssl/crypto/seed/seed.c',
                'crypto/openssl/crypto/seed/seed_ecb.c',
                'crypto/openssl/crypto/seed/seed_ofb.c',
                'crypto/openssl/crypto/objects/obj_err.c',
                'crypto/openssl/crypto/objects/o_names.c',
                'crypto/openssl/crypto/objects/obj_dat.c',
                'crypto/openssl/crypto/objects/obj_xref.c',
                'crypto/openssl/crypto/objects/obj_lib.c',
                'crypto/openssl/crypto/ts/ts_asn1.c',
                'crypto/openssl/crypto/ts/ts_req_print.c',
                'crypto/openssl/crypto/ts/ts_rsp_print.c',
                'crypto/openssl/crypto/ts/ts_rsp_sign.c',
                'crypto/openssl/crypto/ts/ts_conf.c',
                'crypto/openssl/crypto/ts/ts_rsp_verify.c',
                'crypto/openssl/crypto/ts/ts_req_utils.c',
                'crypto/openssl/crypto/ts/ts_rsp_utils.c',
                'crypto/openssl/crypto/ts/ts_err.c',
                'crypto/openssl/crypto/ts/ts_verify_ctx.c',
                'crypto/openssl/crypto/ts/ts_lib.c',
                'crypto/openssl/crypto/mdc2/mdc2dgst.c',
                'crypto/openssl/crypto/mdc2/mdc2_one.c',
                'crypto/openssl/crypto/mdc2/mdc2test.c',
                'crypto/openssl/crypto/pqueue/pqueue.c',
                'crypto/openssl/crypto/pqueue/pq_test.c',
                'crypto/openssl/crypto/buffer/buf_str.c',
                'crypto/openssl/crypto/buffer/buffer.c',
                'crypto/openssl/crypto/buffer/buf_err.c',
                'crypto/openssl/crypto/o_fips.c',
                'crypto/openssl/crypto/mem_clr.c',
                'crypto/openssl/crypto/cast/c_skey.c',
                'crypto/openssl/crypto/cast/c_ecb.c',
                'crypto/openssl/crypto/cast/cast_spd.c',
                'crypto/openssl/crypto/cast/c_ofb64.c',
                'crypto/openssl/crypto/cast/c_cfb64.c',
                'crypto/openssl/crypto/cast/c_enc.c',
                'crypto/openssl/crypto/cast/castopts.c',
                'crypto/openssl/crypto/cast/casttest.c',
                'crypto/openssl/crypto/rsa/rsa_gen.c',
                'crypto/openssl/crypto/rsa/rsa_none.c',
                'crypto/openssl/crypto/rsa/rsa_ameth.c',
                'crypto/openssl/crypto/rsa/rsa_crpt.c',
                'crypto/openssl/crypto/rsa/rsa_pss.c',
                'crypto/openssl/crypto/rsa/rsa_pmeth.c',
                'crypto/openssl/crypto/rsa/rsa_sign.c',
                'crypto/openssl/crypto/rsa/rsa_eay.c',
                'crypto/openssl/crypto/rsa/rsa_oaep.c',
                'crypto/openssl/crypto/rsa/rsa_saos.c',
                'crypto/openssl/crypto/rsa/rsa_pk1.c',
                'crypto/openssl/crypto/rsa/rsa_asn1.c',
                'crypto/openssl/crypto/rsa/rsa_depr.c',
                'crypto/openssl/crypto/rsa/rsa_err.c',
                'crypto/openssl/crypto/rsa/rsa_prn.c',
                'crypto/openssl/crypto/rsa/rsa_test.c',
                'crypto/openssl/crypto/rsa/rsa_lib.c',
                'crypto/openssl/crypto/rsa/rsa_chk.c',
                'crypto/openssl/crypto/rsa/rsa_x931.c',
                'crypto/openssl/crypto/rsa/rsa_null.c',
                'crypto/openssl/crypto/rsa/rsa_ssl.c',
                'crypto/openssl/crypto/o_dir_test.c',
                'crypto/openssl/crypto/sparcv9cap.c',
                'crypto/openssl/crypto/whrlpool/wp_dgst.c',
                'crypto/openssl/crypto/whrlpool/wp_block.c',
                'crypto/openssl/crypto/whrlpool/wp_test.c',
                'crypto/openssl/crypto/asn1/f_string.c',
                'crypto/openssl/crypto/asn1/t_req.c',
                'crypto/openssl/crypto/asn1/x_long.c',
                'crypto/openssl/crypto/asn1/tasn_utl.c',
                'crypto/openssl/crypto/asn1/p5_pbev2.c',
                'crypto/openssl/crypto/asn1/a_time.c',
                'crypto/openssl/crypto/asn1/t_crl.c',
                'crypto/openssl/crypto/asn1/p5_pbe.c',
                'crypto/openssl/crypto/asn1/asn1_gen.c',
                'crypto/openssl/crypto/asn1/t_x509.c',
                'crypto/openssl/crypto/asn1/x_bignum.c',
                'crypto/openssl/crypto/asn1/x_attrib.c',
                'crypto/openssl/crypto/asn1/x_x509.c',
                'crypto/openssl/crypto/asn1/x_spki.c',
                'crypto/openssl/crypto/asn1/x_nx509.c',
                'crypto/openssl/crypto/asn1/tasn_prn.c',
                'crypto/openssl/crypto/asn1/x_pkey.c',
                'crypto/openssl/crypto/asn1/x_algor.c',
                'crypto/openssl/crypto/asn1/x_sig.c',
                'crypto/openssl/crypto/asn1/a_i2d_fp.c',
                'crypto/openssl/crypto/asn1/x_exten.c',
                'crypto/openssl/crypto/asn1/bio_asn1.c',
                'crypto/openssl/crypto/asn1/n_pkey.c',
                'crypto/openssl/crypto/asn1/asn1_par.c',
                'crypto/openssl/crypto/asn1/p8_pkey.c',
                'crypto/openssl/crypto/asn1/t_x509a.c',
                'crypto/openssl/crypto/asn1/x_x509a.c',
                'crypto/openssl/crypto/asn1/bio_ndef.c',
                'crypto/openssl/crypto/asn1/a_strnid.c',
                'crypto/openssl/crypto/asn1/x_pubkey.c',
                'crypto/openssl/crypto/asn1/x_name.c',
                'crypto/openssl/crypto/asn1/tasn_dec.c',
                'crypto/openssl/crypto/asn1/f_enum.c',
                'crypto/openssl/crypto/asn1/t_spki.c',
                'crypto/openssl/crypto/asn1/d2i_pu.c',
                'crypto/openssl/crypto/asn1/a_sign.c',
                'crypto/openssl/crypto/asn1/a_bitstr.c',
                'crypto/openssl/crypto/asn1/a_set.c',
                'crypto/openssl/crypto/asn1/a_mbstr.c',
                'crypto/openssl/crypto/asn1/x_info.c',
                'crypto/openssl/crypto/asn1/asn_moid.c',
                'crypto/openssl/crypto/asn1/tasn_typ.c',
                'crypto/openssl/crypto/asn1/a_octet.c',
                'crypto/openssl/crypto/asn1/a_digest.c',
                'crypto/openssl/crypto/asn1/t_pkey.c',
                'crypto/openssl/crypto/asn1/x_crl.c',
                'crypto/openssl/crypto/asn1/i2d_pr.c',
                'crypto/openssl/crypto/asn1/a_gentm.c',
                'crypto/openssl/crypto/asn1/x_req.c',
                'crypto/openssl/crypto/asn1/d2i_pr.c',
                'crypto/openssl/crypto/asn1/nsseq.c',
                'crypto/openssl/crypto/asn1/a_enum.c',
                'crypto/openssl/crypto/asn1/a_bool.c',
                'crypto/openssl/crypto/asn1/a_d2i_fp.c',
                'crypto/openssl/crypto/asn1/tasn_enc.c',
                'crypto/openssl/crypto/asn1/a_type.c',
                'crypto/openssl/crypto/asn1/t_bitst.c',
                'crypto/openssl/crypto/asn1/asn1_lib.c',
                'crypto/openssl/crypto/asn1/a_dup.c',
                'crypto/openssl/crypto/asn1/a_int.c',
                'crypto/openssl/crypto/asn1/a_object.c',
                'crypto/openssl/crypto/asn1/i2d_pu.c',
                'crypto/openssl/crypto/asn1/a_print.c',
                'crypto/openssl/crypto/asn1/x_val.c',
                'crypto/openssl/crypto/asn1/a_utctm.c',
                'crypto/openssl/crypto/asn1/a_utf8.c',
                'crypto/openssl/crypto/asn1/f_int.c',
                'crypto/openssl/crypto/asn1/ameth_lib.c',
                'crypto/openssl/crypto/asn1/asn_pack.c',
                'crypto/openssl/crypto/asn1/a_bytes.c',
                'crypto/openssl/crypto/asn1/asn_mime.c',
                'crypto/openssl/crypto/asn1/a_verify.c',
                'crypto/openssl/crypto/asn1/evp_asn1.c',
                'crypto/openssl/crypto/asn1/a_strex.c',
                'crypto/openssl/crypto/asn1/tasn_new.c',
                'crypto/openssl/crypto/asn1/asn1_err.c',
                'crypto/openssl/crypto/asn1/tasn_fre.c',
                'crypto/openssl/crypto/ec/ecp_smpl.c',
                'crypto/openssl/crypto/ec/ec2_mult.c',
                'crypto/openssl/crypto/ec/ecp_nistz256.c',
                'crypto/openssl/crypto/ec/ecp_nistp521.c',
                'crypto/openssl/crypto/ec/ec2_smpl.c',
                'crypto/openssl/crypto/ec/ec_key.c',
                'crypto/openssl/crypto/ec/ec_cvt.c',
                'crypto/openssl/crypto/ec/ecp_nist.c',
                'crypto/openssl/crypto/ec/ec2_oct.c',
                'crypto/openssl/crypto/ec/ec_ameth.c',
                'crypto/openssl/crypto/ec/ec_pmeth.c',
                'crypto/openssl/crypto/ec/ec_oct.c',
                'crypto/openssl/crypto/ec/ec_err.c',
                'crypto/openssl/crypto/ec/ec_print.c',
                'crypto/openssl/crypto/ec/ecp_oct.c',
                'crypto/openssl/crypto/ec/ec_check.c',
                'crypto/openssl/crypto/ec/ecp_nistp224.c',
                'crypto/openssl/crypto/ec/eck_prn.c',
                'crypto/openssl/crypto/ec/ec_mult.c',
                'crypto/openssl/crypto/ec/ecp_mont.c',
                'crypto/openssl/crypto/ec/ec_curve.c',
                'crypto/openssl/crypto/ec/ecp_nistputil.c',
                'crypto/openssl/crypto/ec/ectest.c',
                'crypto/openssl/crypto/ec/ec_asn1.c',
                'crypto/openssl/crypto/ec/ecp_nistp256.c',
                'crypto/openssl/crypto/ec/ec_lib.c',
                'crypto/openssl/crypto/conf/conf_sap.c',
                'crypto/openssl/crypto/conf/conf_api.c',
                'crypto/openssl/crypto/conf/conf_err.c',
                'crypto/openssl/crypto/conf/conf_mall.c',
                'crypto/openssl/crypto/conf/conf_lib.c',
                'crypto/openssl/crypto/conf/conf_def.c',
                'crypto/openssl/crypto/conf/conf_mod.c',
                'crypto/openssl/crypto/lhash/lhash.c',
                'crypto/openssl/crypto/cversion.c',
                'crypto/openssl/crypto/o_str.c',
            ],
            mm.generator['source'](['-DOPENSSL_NO_EC_NISTP_64_GCC_128=1',
                                    '-DOPENSSL_NO_GMP=1',
                                    '-DOPENSSL_NO_JPAKE=1',
                                    '-DOPENSSL_NO_LIBUNBOUND=1',
                                    '-DOPENSSL_NO_MD2=1',
                                    '-DOPENSSL_NO_RC5=1',
                                    '-DOPENSSL_NO_RFC3779=1',
                                    '-DOPENSSL_NO_SCTP=1',
                                    '-DOPENSSL_NO_SSL_TRACE=1',
                                    '-DOPENSSL_NO_SSL2=1',
                                    '-DOPENSSL_NO_STORE=1',
                                    '-DOPENSSL_NO_UNIT_TEST=1',
                                    '-DOPENSSL_NO_WEAK_SSL_CIPHERS=1',
                                    '-DNO_WINDOWS_BRAINDEATH=1',
                                    '-DOPENSSL_DISABLE_OLD_DES_SUPPORT=1'],
                                   ['freebsd/crypto',
                                    'freebsd/crypto/openssl',
                                    'freebsd/crypto/openssl/crypto',
                                    'freebsd/crypto/openssl/crypto/evp',
                                    'freebsd/crypto/openssl/crypto/asn1',
                                    'freebsd/crypto/openssl/crypto/modes'])
        )
        self.addFile(mm.generator['file']('crypto/openssl/crypto/LPdir_unix.c',
                                          mm.generator['freebsd-path'](),
                                          mm.generator['from-FreeBSD-to-RTEMS-UserSpaceSourceConverter'](),
                                          mm.generator['from-RTEMS-To-FreeBSD-SourceConverter'](),
                                          mm.generator['buildSystemFragmentComposer']()))
        self.addFile(mm.generator['file']('crypto/openssl/crypto/ec/ecp_nistz256_table.c',
                                          mm.generator['freebsd-path'](),
                                          mm.generator['from-FreeBSD-to-RTEMS-UserSpaceSourceConverter'](),
                                          mm.generator['from-RTEMS-To-FreeBSD-SourceConverter'](),
                                          mm.generator['buildSystemFragmentComposer']()))

#
# Contrib expat
#
class contrib_expat(builder.Module):

    def __init__(self, manager):
        super(contrib_expat, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        cflags = ['-DHAVE_MEMMOVE=1']
        self.addRTEMSHeaderFiles(
            [
                'include/bsdxml.h',
            ]
        )
        self.addUserSpaceHeaderFiles(
            [
                'contrib/expat/lib/ascii.h',
                'contrib/expat/lib/asciitab.h',
                'contrib/expat/lib/expat_external.h',
                'contrib/expat/lib/expat.h',
                'contrib/expat/lib/iasciitab.h',
                'contrib/expat/lib/internal.h',
                'contrib/expat/lib/latin1tab.h',
                'contrib/expat/lib/nametab.h',
                'contrib/expat/lib/utf8tab.h',
                'contrib/expat/lib/xmlrole.h',
                'contrib/expat/lib/xmltok.h',
                'contrib/expat/lib/xmltok_impl.h',
            ]
        )
        self.addUserSpaceSourceFiles(
            [
                'contrib/expat/lib/xmlparse.c',
                'contrib/expat/lib/xmlrole.c',
                'contrib/expat/lib/xmltok.c',
                'contrib/expat/lib/xmltok_impl.c',
                'contrib/expat/lib/xmltok_ns.c',
            ],
            mm.generator['source'](cflags)
        )

#
# Contrib libpcap
#
class contrib_libpcap(builder.Module):

    def __init__(self, manager):
        super(contrib_libpcap, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        cflags = ['-D__FreeBSD__=1',
                  '-DBSD=1',
                  '-D_U_=__attribute__((unused))',
                  '-DHAVE_LIMITS_H=1',
                  '-DHAVE_INTTYPES=1',
                  '-DHAVE_STDINT=1',
                  '-DHAVE_STRERROR=1',
                  '-DHAVE_STRLCPY=1',
                  '-DHAVE_SNPRINTF=1',
                  '-DHAVE_VSNPRINTF=1',
                  '-DHAVE_SOCKADDR_SA_LEN=1',
                  #'-DHAVE_ZEROCOPY_BPF=1',
              '-DHAVE_NET_IF_MEDIA_H=1',
                  '-DHAVE_SYS_IOCCOM_H=1']
        self.addUserSpaceHeaderFiles(
            [
                'contrib/libpcap/arcnet.h',
                'contrib/libpcap/atmuni31.h',
                'contrib/libpcap/ethertype.h',
                'contrib/libpcap/extract.h',
                'contrib/libpcap/gencode.h',
                'contrib/libpcap/ieee80211.h',
                'contrib/libpcap/llc.h',
                'contrib/libpcap/nametoaddr.h',
                'contrib/libpcap/nlpid.h',
                'contrib/libpcap/pcap-common.h',
                'contrib/libpcap/pcap-int.h',
                'contrib/libpcap/pcap-namedb.h',
                'contrib/libpcap/pcap.h',
                'contrib/libpcap/pcap/can_socketcan.h',
                'contrib/libpcap/pcap/export-defs.h',
                'contrib/libpcap/pcap/ipnet.h',
                'contrib/libpcap/pcap/namedb.h',
                'contrib/libpcap/pcap/nflog.h',
                'contrib/libpcap/pcap/pcap.h',
                'contrib/libpcap/pcap/sll.h',
                'contrib/libpcap/pcap/usb.h',
                'contrib/libpcap/portability.h',
                'contrib/libpcap/ppp.h',
                'contrib/libpcap/sf-pcap-ng.h',
                'contrib/libpcap/sf-pcap.h',
                'contrib/libpcap/sunatmpos.h',
            ]
        )
        gen_cflags = cflags + ['-DNEED_YYPARSE_WRAPPER=1',
                               '-Dyylval=pcap_lval']
        self.addFile(mm.generator['file']('contrib/libpcap/scanner.l',
                                          mm.generator['freebsd-path'](),
                                          mm.generator['convert'](),
                                          mm.generator['convert'](),
                                          mm.generator['lex']('pcap',
                                                              'scanner.c',
                                                              gen_cflags)))
        self.addFile(mm.generator['file']('contrib/libpcap/grammar.y',
                                          mm.generator['freebsd-path'](),
                                          mm.generator['convert'](),
                                          mm.generator['convert'](),
                                          mm.generator['yacc']('pcap',
                                                               'tokdefs.h',
                                                               gen_cflags)))
        self.addUserSpaceSourceFiles(
            [
                'contrib/libpcap/bpf_image.c',
                'contrib/libpcap/etherent.c',
                'contrib/libpcap/fad-getad.c',
                'contrib/libpcap/fad-helpers.c',
                'contrib/libpcap/gencode.c',
                'contrib/libpcap/inet.c',
                'contrib/libpcap/pcap.c',
                'contrib/libpcap/pcap-bpf.c',
                'contrib/libpcap/pcap-common.c',
                'contrib/libpcap/optimize.c',
                'contrib/libpcap/nametoaddr.c',
                'contrib/libpcap/savefile.c',
                'contrib/libpcap/sf-pcap.c',
                'contrib/libpcap/sf-pcap-ng.c',
            ],
            mm.generator['source'](cflags)
        )

#
# /usr/sbin/tcpdump
#
class usr_sbin_tcpdump(builder.Module):

    def __init__(self, manager):
        super(usr_sbin_tcpdump, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addUserSpaceHeaderFiles(
            [
                'contrib/tcpdump/addrtoname.h',
                'contrib/tcpdump/addrtostr.h',
                'contrib/tcpdump/af.h',
                'contrib/tcpdump/ah.h',
                'contrib/tcpdump/appletalk.h',
                'contrib/tcpdump/ascii_strcasecmp.h',
                'contrib/tcpdump/atm.h',
                'contrib/tcpdump/chdlc.h',
                'contrib/tcpdump/cpack.h',
                'contrib/tcpdump/ether.h',
                'contrib/tcpdump/ethertype.h',
                'contrib/tcpdump/extract.h',
                'contrib/tcpdump/gmpls.h',
                'contrib/tcpdump/gmt2local.h',
                'contrib/tcpdump/interface.h',
                'contrib/tcpdump/ip6.h',
                'contrib/tcpdump/ip.h',
                'contrib/tcpdump/ipproto.h',
                'contrib/tcpdump/l2vpn.h',
                'contrib/tcpdump/llc.h',
                'contrib/tcpdump/machdep.h',
                'contrib/tcpdump/mib.h',
                'contrib/tcpdump/mpls.h',
                'contrib/tcpdump/nameser.h',
                'contrib/tcpdump/netdissect.h',
                'contrib/tcpdump/netdissect-stdinc.h',
                'contrib/tcpdump/nfs.h',
                'contrib/tcpdump/nfsfh.h',
                'contrib/tcpdump/nlpid.h',
                'contrib/tcpdump/openflow.h',
                'contrib/tcpdump/ospf.h',
                'contrib/tcpdump/oui.h',
                'contrib/tcpdump/pcap-missing.h',
                'contrib/tcpdump/ppp.h',
                'contrib/tcpdump/print.h',
                'contrib/tcpdump/rpc_auth.h',
                'contrib/tcpdump/rpc_msg.h',
                'contrib/tcpdump/rpl.h',
                'contrib/tcpdump/setsignal.h',
                'contrib/tcpdump/signature.h',
                'contrib/tcpdump/slcompress.h',
                'contrib/tcpdump/smb.h',
                'contrib/tcpdump/strtoaddr.h',
                'contrib/tcpdump/tcp.h',
                'contrib/tcpdump/timeval-operations.h',
                'contrib/tcpdump/udp.h',
                'usr.sbin/tcpdump/tcpdump/config.h',
            ]
        )
        self.addUserSpaceSourceFiles(
            [
                'contrib/tcpdump/addrtoname.c',
                'contrib/tcpdump/addrtostr.c',
                'contrib/tcpdump/af.c',
                'contrib/tcpdump/ascii_strcasecmp.c',
                'contrib/tcpdump/bpf_dump.c',
                'contrib/tcpdump/checksum.c',
                'contrib/tcpdump/cpack.c',
                'contrib/tcpdump/gmpls.c',
                'contrib/tcpdump/gmt2local.c',
                'contrib/tcpdump/in_cksum.c',
                'contrib/tcpdump/ipproto.c',
                'contrib/tcpdump/l2vpn.c',
                'contrib/tcpdump/machdep.c',
                'contrib/tcpdump/netdissect.c',
                'contrib/tcpdump/nlpid.c',
                'contrib/tcpdump/oui.c',
                'contrib/tcpdump/parsenfsfh.c',
                'contrib/tcpdump/print-802_11.c',
                'contrib/tcpdump/print-802_15_4.c',
                'contrib/tcpdump/print-ah.c',
                'contrib/tcpdump/print-ahcp.c',
                'contrib/tcpdump/print-aodv.c',
                'contrib/tcpdump/print-aoe.c',
                'contrib/tcpdump/print-ap1394.c',
                'contrib/tcpdump/print-arcnet.c',
                'contrib/tcpdump/print-arp.c',
                'contrib/tcpdump/print-ascii.c',
                'contrib/tcpdump/print-atalk.c',
                'contrib/tcpdump/print-atm.c',
                'contrib/tcpdump/print-babel.c',
                'contrib/tcpdump/print-beep.c',
                'contrib/tcpdump/print-bfd.c',
                'contrib/tcpdump/print-bgp.c',
                'contrib/tcpdump/print-bootp.c',
                'contrib/tcpdump/print-bt.c',
                'contrib/tcpdump/print.c',
                'contrib/tcpdump/print-calm-fast.c',
                'contrib/tcpdump/print-carp.c',
                'contrib/tcpdump/print-cdp.c',
                'contrib/tcpdump/print-cfm.c',
                'contrib/tcpdump/print-chdlc.c',
                'contrib/tcpdump/print-cip.c',
                'contrib/tcpdump/print-cnfp.c',
                'contrib/tcpdump/print-dccp.c',
                'contrib/tcpdump/print-decnet.c',
                'contrib/tcpdump/print-dhcp6.c',
                'contrib/tcpdump/print-domain.c',
                'contrib/tcpdump/print-dtp.c',
                'contrib/tcpdump/print-dvmrp.c',
                'contrib/tcpdump/print-eap.c',
                'contrib/tcpdump/print-egp.c',
                'contrib/tcpdump/print-eigrp.c',
                'contrib/tcpdump/print-enc.c',
                'contrib/tcpdump/print-esp.c',
                'contrib/tcpdump/print-ether.c',
                'contrib/tcpdump/print-fddi.c',
                'contrib/tcpdump/print-forces.c',
                'contrib/tcpdump/print-frag6.c',
                'contrib/tcpdump/print-fr.c',
                'contrib/tcpdump/print-ftp.c',
                'contrib/tcpdump/print-geneve.c',
                'contrib/tcpdump/print-geonet.c',
                'contrib/tcpdump/print-gre.c',
                'contrib/tcpdump/print-hncp.c',
                'contrib/tcpdump/print-hsrp.c',
                'contrib/tcpdump/print-http.c',
                'contrib/tcpdump/print-icmp6.c',
                'contrib/tcpdump/print-icmp.c',
                'contrib/tcpdump/print-igmp.c',
                'contrib/tcpdump/print-igrp.c',
                'contrib/tcpdump/print-ip6.c',
                'contrib/tcpdump/print-ip6opts.c',
                'contrib/tcpdump/print-ip.c',
                'contrib/tcpdump/print-ipcomp.c',
                'contrib/tcpdump/print-ipfc.c',
                'contrib/tcpdump/print-ipnet.c',
                'contrib/tcpdump/print-ipx.c',
                'contrib/tcpdump/print-isakmp.c',
                'contrib/tcpdump/print-isoclns.c',
                'contrib/tcpdump/print-juniper.c',
                'contrib/tcpdump/print-krb.c',
                'contrib/tcpdump/print-l2tp.c',
                'contrib/tcpdump/print-lane.c',
                'contrib/tcpdump/print-ldp.c',
                'contrib/tcpdump/print-lisp.c',
                'contrib/tcpdump/print-llc.c',
                'contrib/tcpdump/print-lldp.c',
                'contrib/tcpdump/print-lmp.c',
                'contrib/tcpdump/print-loopback.c',
                'contrib/tcpdump/print-lspping.c',
                'contrib/tcpdump/print-lwapp.c',
                'contrib/tcpdump/print-lwres.c',
                'contrib/tcpdump/print-m3ua.c',
                'contrib/tcpdump/print-medsa.c',
                'contrib/tcpdump/print-mobile.c',
                'contrib/tcpdump/print-mobility.c',
                'contrib/tcpdump/print-mpcp.c',
                'contrib/tcpdump/print-mpls.c',
                'contrib/tcpdump/print-mptcp.c',
                'contrib/tcpdump/print-msdp.c',
                'contrib/tcpdump/print-msnlb.c',
                'contrib/tcpdump/print-nfs.c',
                'contrib/tcpdump/print-nsh.c',
                'contrib/tcpdump/print-ntp.c',
                'contrib/tcpdump/print-null.c',
                'contrib/tcpdump/print-olsr.c',
                'contrib/tcpdump/print-openflow-1.0.c',
                'contrib/tcpdump/print-openflow.c',
                'contrib/tcpdump/print-ospf6.c',
                'contrib/tcpdump/print-ospf.c',
                'contrib/tcpdump/print-otv.c',
                'contrib/tcpdump/print-pflog.c',
                'contrib/tcpdump/print-pfsync.c',
                'contrib/tcpdump/print-pgm.c',
                'contrib/tcpdump/print-pim.c',
                'contrib/tcpdump/print-pktap.c',
                'contrib/tcpdump/print-ppi.c',
                'contrib/tcpdump/print-ppp.c',
                'contrib/tcpdump/print-pppoe.c',
                'contrib/tcpdump/print-pptp.c',
                'contrib/tcpdump/print-radius.c',
                'contrib/tcpdump/print-raw.c',
                'contrib/tcpdump/print-resp.c',
                'contrib/tcpdump/print-rip.c',
                'contrib/tcpdump/print-ripng.c',
                'contrib/tcpdump/print-rpki-rtr.c',
                'contrib/tcpdump/print-rrcp.c',
                'contrib/tcpdump/print-rsvp.c',
                'contrib/tcpdump/print-rt6.c',
                'contrib/tcpdump/print-rtsp.c',
                'contrib/tcpdump/print-rx.c',
                'contrib/tcpdump/print-sctp.c',
                'contrib/tcpdump/print-sflow.c',
                'contrib/tcpdump/print-sip.c',
                'contrib/tcpdump/print-sl.c',
                'contrib/tcpdump/print-sll.c',
                'contrib/tcpdump/print-slow.c',
                'contrib/tcpdump/print-smb.c',
                'contrib/tcpdump/print-smtp.c',
                'contrib/tcpdump/print-snmp.c',
                'contrib/tcpdump/print-stp.c',
                'contrib/tcpdump/print-sunatm.c',
                'contrib/tcpdump/print-symantec.c',
                'contrib/tcpdump/print-syslog.c',
                'contrib/tcpdump/print-tcp.c',
                'contrib/tcpdump/print-telnet.c',
                'contrib/tcpdump/print-tftp.c',
                'contrib/tcpdump/print-timed.c',
                'contrib/tcpdump/print-tipc.c',
                'contrib/tcpdump/print-token.c',
                'contrib/tcpdump/print-udld.c',
                'contrib/tcpdump/print-udp.c',
                'contrib/tcpdump/print-usb.c',
                'contrib/tcpdump/print-vjc.c',
                'contrib/tcpdump/print-vqp.c',
                'contrib/tcpdump/print-vrrp.c',
                'contrib/tcpdump/print-vtp.c',
                'contrib/tcpdump/print-vxlan.c',
                'contrib/tcpdump/print-vxlan-gpe.c',
                'contrib/tcpdump/print-wb.c',
                'contrib/tcpdump/print-zephyr.c',
                'contrib/tcpdump/print-zeromq.c',
                'contrib/tcpdump/setsignal.c',
                'contrib/tcpdump/signature.c',
                'contrib/tcpdump/smbutil.c',
                'contrib/tcpdump/tcpdump.c',
                'contrib/tcpdump/util-print.c',
            ],
            mm.generator['source'](['-D__FreeBSD__=1',
                                    '-D_U_=__attribute__((unused))',
                                    '-DHAVE_CONFIG_H=1',
                                    '-DHAVE_NET_PFVAR_H=1'],
                                   ['freebsd/contrib/tcpdump',
                                    'freebsd/usr.sbin/tcpdump/tcpdump'])
        )

#
# /usr/sbin/wpa_supplicant
#
class usr_sbin_wpa_supplicant(builder.Module):

    def __init__(self, manager):
        super(usr_sbin_wpa_supplicant, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addUserSpaceHeaderFiles(
            [
                'contrib/wpa/wpa_supplicant/ap.h',
                'contrib/wpa/wpa_supplicant/blacklist.h',
                'contrib/wpa/wpa_supplicant/bss.h',
                'contrib/wpa/wpa_supplicant/config.h',
                'contrib/wpa/wpa_supplicant/config_ssid.h',
                'contrib/wpa/wpa_supplicant/ctrl_iface.h',
                'contrib/wpa/wpa_supplicant/driver_i.h',
                'contrib/wpa/wpa_supplicant/gas_query.h',
                'contrib/wpa/wpa_supplicant/hs20_supplicant.h',
                'contrib/wpa/wpa_supplicant/interworking.h',
                'contrib/wpa/wpa_supplicant/mesh.h',
                'contrib/wpa/wpa_supplicant/mesh_mpm.h',
                'contrib/wpa/wpa_supplicant/mesh_rsn.h',
                'contrib/wpa/wpa_supplicant/notify.h',
                'contrib/wpa/wpa_supplicant/offchannel.h',
                'contrib/wpa/wpa_supplicant/scan.h',
                'contrib/wpa/wpa_supplicant/autoscan.h',
                'contrib/wpa/wpa_supplicant/wmm_ac.h',
                'contrib/wpa/wpa_supplicant/wnm_sta.h',
                'contrib/wpa/wpa_supplicant/wpas_glue.h',
                'contrib/wpa/wpa_supplicant/wpas_kay.h',
                'contrib/wpa/wpa_supplicant/wpa_supplicant_i.h',
                'contrib/wpa/wpa_supplicant/wps_supplicant.h',
                'contrib/wpa/wpa_supplicant/ibss_rsn.h',
                'contrib/wpa/wpa_supplicant/p2p_supplicant.h',
                'contrib/wpa/wpa_supplicant/wifi_display.h',
                'contrib/wpa/wpa_supplicant/sme.h',
                'contrib/wpa/wpa_supplicant/bgscan.h',
                'contrib/wpa/wpa_supplicant/dbus/dbus_common.h',
                'contrib/wpa/wpa_supplicant/dbus/dbus_old.h',
                'contrib/wpa/wpa_supplicant/dbus/dbus_new.h',
                'contrib/wpa/src/ap/ap_config.h',
                'contrib/wpa/src/ap/ap_drv_ops.h',
                'contrib/wpa/src/ap/hs20.h',
                'contrib/wpa/src/ap/hostapd.h',
                'contrib/wpa/src/ap/ieee802_11.h',
                'contrib/wpa/src/ap/ieee802_11_auth.h',
                'contrib/wpa/src/ap/p2p_hostapd.h',
                'contrib/wpa/src/ap/pmksa_cache_auth.h',
                'contrib/wpa/src/ap/sta_info.h',
                'contrib/wpa/src/ap/wpa_auth.h',
                'contrib/wpa/src/ap/wpa_auth_i.h',
                'contrib/wpa/src/ap/wpa_auth_ie.h',
                'contrib/wpa/src/ap/wmm.h',
                'contrib/wpa/src/utils/includes.h',
                'contrib/wpa/src/utils/base64.h',
                'contrib/wpa/src/utils/bitfield.h',
                'contrib/wpa/src/utils/build_config.h',
                'contrib/wpa/src/utils/common.h',
                'contrib/wpa/src/utils/eloop.h',
                'contrib/wpa/src/utils/ip_addr.h',
                'contrib/wpa/src/utils/list.h',
                'contrib/wpa/src/utils/os.h',
                'contrib/wpa/src/utils/platform.h',
                'contrib/wpa/src/utils/uuid.h',
                'contrib/wpa/src/utils/wpa_debug.h',
                'contrib/wpa/src/utils/wpabuf.h',
                'contrib/wpa/src/utils/trace.h',
                'contrib/wpa/src/utils/pcsc_funcs.h',
                'contrib/wpa/src/utils/ext_password.h',
                'contrib/wpa/src/utils/state_machine.h',
                'contrib/wpa/src/common/gas.h',
                'contrib/wpa/src/common/hw_features_common.h',
                'contrib/wpa/src/common/ieee802_11_common.h',
                'contrib/wpa/src/common/ieee802_11_defs.h',
                'contrib/wpa/src/common/wpa_common.h',
                'contrib/wpa/src/common/defs.h',
                'contrib/wpa/src/common/qca-vendor.h',
                'contrib/wpa/src/common/eapol_common.h',
                'contrib/wpa/src/common/sae.h',
                'contrib/wpa/src/common/wpa_ctrl.h',
                'contrib/wpa/src/common/version.h',
                'contrib/wpa/src/drivers/driver.h',
                'contrib/wpa/src/drivers/driver_ndis.h',
                'contrib/wpa/src/drivers/driver_nl80211.h',
                'contrib/wpa/src/drivers/linux_defines.h',
                'contrib/wpa/src/wps/http.h',
                'contrib/wpa/src/wps/http_client.h',
                'contrib/wpa/src/wps/http_server.h',
                'contrib/wpa/src/wps/httpread.h',
                'contrib/wpa/src/wps/wps.h',
                'contrib/wpa/src/wps/wps_i.h',
                'contrib/wpa/src/wps/wps_defs.h',
                'contrib/wpa/src/wps/upnp_xml.h',
                'contrib/wpa/src/wps/wps_attr_parse.h',
                'contrib/wpa/src/wps/wps_dev_attr.h',
                'contrib/wpa/src/wps/wps_er.h',
                'contrib/wpa/src/wps/wps_upnp.h',
                'contrib/wpa/src/wps/wps_upnp_i.h',
                'contrib/wpa/src/l2_packet/l2_packet.h',
                'contrib/wpa/src/rsn_supp/peerkey.h',
                'contrib/wpa/src/rsn_supp/pmksa_cache.h',
                'contrib/wpa/src/rsn_supp/preauth.h',
                'contrib/wpa/src/rsn_supp/wpa.h',
                'contrib/wpa/src/rsn_supp/wpa_i.h',
                'contrib/wpa/src/rsn_supp/wpa_ie.h',
                'contrib/wpa/src/p2p/p2p.h',
                'contrib/wpa/src/fst/fst.h',
                'contrib/wpa/src/fst/fst_ctrl_iface.h',
                'contrib/wpa/src/fst/fst_ctrl_aux.h',
                'contrib/wpa/src/crypto/md5.h',
                'contrib/wpa/src/crypto/sha256.h',
                'contrib/wpa/src/crypto/sha384.h',
                'contrib/wpa/src/crypto/aes_wrap.h',
                'contrib/wpa/src/crypto/crypto.h',
                'contrib/wpa/src/crypto/random.h',
                'contrib/wpa/src/crypto/dh_group5.h',
                'contrib/wpa/src/crypto/sha1.h',
                'contrib/wpa/src/crypto/tls.h',
                'contrib/wpa/src/crypto/ms_funcs.h',
                'contrib/wpa/src/crypto/aes.h',
                'contrib/wpa/src/crypto/sha256_i.h',
                'contrib/wpa/src/eapol_auth/eapol_auth_sm.h',
                'contrib/wpa/src/eapol_supp/eapol_supp_sm.h',
                'contrib/wpa/src/eap_peer/eap_config.h',
                'contrib/wpa/src/eap_peer/eap.h',
                'contrib/wpa/src/eap_peer/eap_i.h',
                'contrib/wpa/src/eap_peer/eap_methods.h',
                'contrib/wpa/src/eap_peer/eap_proxy.h',
                'contrib/wpa/src/eap_peer/mschapv2.h',
                'contrib/wpa/src/eap_peer/tncc.h',
                'contrib/wpa/src/eap_peer/eap_tls_common.h',
                'contrib/wpa/src/eap_common/eap_defs.h',
                'contrib/wpa/src/eap_common/eap_wsc_common.h',
                'contrib/wpa/src/eap_common/eap_peap_common.h',
                'contrib/wpa/src/eap_common/eap_psk_common.h',
                'contrib/wpa/src/eap_common/eap_tlv_common.h',
                'contrib/wpa/src/eap_common/eap_common.h',
                'contrib/wpa/src/eap_common/chap.h',
                'contrib/wpa/src/eap_common/eap_ttls.h',
                'contrib/wpa/src/eap_server/eap_methods.h',
                'contrib/wpa/src/eapol_supp/eapol_supp_sm.h',
                'contrib/wpa/src/radius/radius.h',
                'contrib/wpa/src/tls/tlsv1_client.h',
                'contrib/wpa/src/tls/tlsv1_cred.h',
                'contrib/wpa/src/tls/tlsv1_server.h',
                'usr.sbin/wpa/wpa_supplicant/Packet32.h',
                'usr.sbin/wpa/wpa_supplicant/ntddndis.h',
            ]
        )
        self.addUserSpaceSourceFiles(
            [
                'contrib/wpa/wpa_supplicant/blacklist.c',
                'contrib/wpa/wpa_supplicant/bss.c',
                'contrib/wpa/wpa_supplicant/config.c',
                'contrib/wpa/wpa_supplicant/config_file.c',
                'contrib/wpa/wpa_supplicant/ctrl_iface.c',
                'contrib/wpa/wpa_supplicant/ctrl_iface_unix.c',
                'contrib/wpa/wpa_supplicant/eap_register.c',
                'contrib/wpa/wpa_supplicant/events.c',
                'contrib/wpa/wpa_supplicant/gas_query.c',
                'contrib/wpa/wpa_supplicant/hs20_supplicant.c',
                'contrib/wpa/wpa_supplicant/interworking.c',
                'contrib/wpa/wpa_supplicant/main.c',
                'contrib/wpa/wpa_supplicant/notify.c',
                'contrib/wpa/wpa_supplicant/offchannel.c',
                'contrib/wpa/wpa_supplicant/scan.c',
                'contrib/wpa/wpa_supplicant/wmm_ac.c',
                'contrib/wpa/wpa_supplicant/wpa_supplicant.c',
                'contrib/wpa/wpa_supplicant/wpas_glue.c',
                'contrib/wpa/wpa_supplicant/wps_supplicant.c',
                'contrib/wpa/src/ap/ap_drv_ops.c',
                'contrib/wpa/src/ap/hs20.c',
                'contrib/wpa/src/ap/ieee802_11_shared.c',
                'contrib/wpa/src/ap/wpa_auth.c',
                'contrib/wpa/src/ap/wpa_auth_ft.c',
                'contrib/wpa/src/utils/base64.c',
                'contrib/wpa/src/utils/common.c',
                'contrib/wpa/src/utils/eloop.c',
                'contrib/wpa/src/utils/os_unix.c',
                'contrib/wpa/src/utils/uuid.c',
                'contrib/wpa/src/utils/wpa_debug.c',
                'contrib/wpa/src/utils/wpabuf.c',
                'contrib/wpa/src/common/gas.c',
                'contrib/wpa/src/common/hw_features_common.c',
                'contrib/wpa/src/common/ieee802_11_common.c',
                'contrib/wpa/src/common/wpa_common.c',
                'contrib/wpa/src/drivers/driver_bsd.c',
                'contrib/wpa/src/drivers/driver_common.c',
                'contrib/wpa/src/drivers/driver_ndis.c',
                'contrib/wpa/src/drivers/driver_wired.c',
                'contrib/wpa/src/drivers/drivers.c',
                'contrib/wpa/src/wps/http_client.c',
                'contrib/wpa/src/wps/http_server.c',
                'contrib/wpa/src/wps/httpread.c',
                'contrib/wpa/src/wps/upnp_xml.c',
                'contrib/wpa/src/wps/wps.c',
                'contrib/wpa/src/wps/wps_attr_build.c',
                'contrib/wpa/src/wps/wps_attr_parse.c',
                'contrib/wpa/src/wps/wps_attr_process.c',
                'contrib/wpa/src/wps/wps_common.c',
                'contrib/wpa/src/wps/wps_dev_attr.c',
                'contrib/wpa/src/wps/wps_enrollee.c',
                'contrib/wpa/src/wps/wps_registrar.c',
                'contrib/wpa/src/wps/wps_upnp.c',
                'contrib/wpa/src/wps/wps_upnp_ap.c',
                'contrib/wpa/src/wps/wps_upnp_event.c',
                'contrib/wpa/src/wps/wps_upnp_ssdp.c',
                'contrib/wpa/src/wps/wps_upnp_web.c',
                'contrib/wpa/src/l2_packet/l2_packet_freebsd.c',
                'contrib/wpa/src/rsn_supp/peerkey.c',
                'contrib/wpa/src/rsn_supp/pmksa_cache.c',
                'contrib/wpa/src/rsn_supp/preauth.c',
                'contrib/wpa/src/rsn_supp/wpa.c',
                'contrib/wpa/src/rsn_supp/wpa_ft.c',
                'contrib/wpa/src/rsn_supp/wpa_ie.c',
                'contrib/wpa/src/eapol_supp/eapol_supp_sm.c',
                'contrib/wpa/src/eap_peer/eap.c',
                'contrib/wpa/src/eap_peer/eap_methods.c',
                'contrib/wpa/src/eap_peer/eap_mschapv2.c',
                'contrib/wpa/src/eap_peer/mschapv2.c',
                'contrib/wpa/src/eap_peer/eap_md5.c',
                'contrib/wpa/src/eap_peer/eap_tls.c',
                'contrib/wpa/src/eap_peer/eap_peap.c',
                'contrib/wpa/src/eap_peer/eap_ttls.c',
                'contrib/wpa/src/eap_peer/eap_gtc.c',
                'contrib/wpa/src/eap_peer/eap_otp.c',
                'contrib/wpa/src/eap_peer/eap_leap.c',
                'contrib/wpa/src/eap_peer/eap_psk.c',
                'contrib/wpa/src/eap_peer/eap_tls_common.c',
                'contrib/wpa/src/eap_common/eap_common.c',
                'contrib/wpa/src/eap_common/eap_peap_common.c',
                'contrib/wpa/src/eap_common/eap_psk_common.c',
                'contrib/wpa/src/eap_common/chap.c',
                'contrib/wpa/src/crypto/crypto_openssl.c',
                'contrib/wpa/src/crypto/random.c',
                'contrib/wpa/src/crypto/aes-omac1.c',
                'contrib/wpa/src/crypto/sha256-prf.c',
                'contrib/wpa/src/crypto/sha256-internal.c',
                'contrib/wpa/src/crypto/sha1.c',
                'contrib/wpa/src/crypto/sha1-prf.c',
                'contrib/wpa/src/crypto/aes-unwrap.c',
                'contrib/wpa/src/crypto/rc4.c',
                'contrib/wpa/src/crypto/sha1-pbkdf2.c',
                'contrib/wpa/src/crypto/tls_internal.c',
                'contrib/wpa/src/crypto/ms_funcs.c',
                'contrib/wpa/src/crypto/aes-eax.c',
                'contrib/wpa/src/crypto/aes-ctr.c',
                'contrib/wpa/src/crypto/aes-encblock.c',
                'usr.sbin/wpa/wpa_supplicant/Packet32.c',
            ],
            mm.generator['source'](['-D__FreeBSD__=1',
                                    '-DCONFIG_BACKEND_FILE=1',
                                    '-DCONFIG_DEBUG_SYSLOG=1',
                                    '-DCONFIG_DRIVER_BSD=1',
                                    '-DCONFIG_DRIVER_NDIS=1',
                                    '-DCONFIG_DRIVER_WIRED=1',
                                    '-DCONFIG_GAS=1',
                                    '-DCONFIG_HS20=1',
                                    '-DCONFIG_IEEE80211R=1',
                                    '-DCONFIG_INTERWORKING=1',
                                    '-DCONFIG_PEERKEY=1',
                                    '-DCONFIG_PRIVSEP=1',
                                    '-DCONFIG_SMARTCARD=1',
                                    '-DCONFIG_TERMINATE_ONLASTIF=1',
                                    '-DCONFIG_TLS=openssl',
                                    '-DCONFIG_WPS=1',
                                    '-DCONFIG_WPS2=1',
                                    '-DCONFIG_WPS_UPNP=1',
                                    '-DPKCS12_FUNCS=1',
                                    '-DCONFIG_SHA256=1',
                                    '-DCONFIG_CTRL_IFACE=1',
                                    '-DCONFIG_CTRL_IFACE_UNIX=1',
                                    '-DIEEE8021X_EAPOL=1',
                                    '-DEAP_MD5=1',
                                    '-DEAP_GTC=1',
                                    '-DEAP_LEAP=1',
                                    '-DEAP_MSCHAPv2=1',
                                    '-DEAP_OTP=1',
                                    '-DEAP_PEAP=1',
                                    '-DEAP_PSK=1',
                                    '-DEAP_TLS=1',
                                    '-DEAP_TTLS=1'],
                                   ['freebsd/contrib/wpa/src',
                                    'freebsd/contrib/wpa/src/utils',
                                    'freebsd/contrib/wpa/src/eap_peer',
                                    'freebsd/usr.sbin/wpa/wpa_supplicant',
                                    'freebsd/crypto/openssl/crypto'])
        )
        self.addRTEMSSourceFiles(
            [
                'rtems/rtems-bsd-shell-wpa_supplicant.c',
                'rtems/rtems-wpa_supplicant_mutex.c',
                'rtems/rtems-bsd-shell-wpa_supplicant_fork.c',
            ],
            mm.generator['source']()
        )

#
# in_chksum Module
#
class in_cksum(builder.Module):

    def __init__(self, manager):
        super(in_cksum, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addRTEMSHeaderFiles(
            [
            ]
        )
        self.addCPUDependentFreeBSDHeaderFiles(
            [
                'sys/i386/include/in_cksum.h',
                'sys/mips/include/in_cksum.h',
                'sys/powerpc/include/in_cksum.h',
                'sys/sparc64/include/in_cksum.h',
            ]
        )
        self.addTargetSourceCPUDependentHeaderFiles(
            [ 'arm', 'avr', 'bfin', 'h8300', 'lm32', 'm32c', 'm32r', 'm68k',
              'nios2', 'sh', 'sparc', 'v850' ],
            'mips',
            [
                'sys/mips/include/in_cksum.h',
            ]
        )
        self.addCPUDependentFreeBSDSourceFiles(
            [ 'i386' ],
            [
                'sys/i386/i386/in_cksum.c',
            ],
            mm.generator['source']()
        )
        self.addCPUDependentFreeBSDSourceFiles(
            [ 'powerpc' ],
            [
                'sys/powerpc/powerpc/in_cksum.c',
            ],
            mm.generator['source']()
        )
        self.addCPUDependentFreeBSDSourceFiles(
            [ 'sparc64' ],
            [
                'sys/sparc64/sparc64/in_cksum.c',
            ],
            mm.generator['source']()
        )
        self.addCPUDependentFreeBSDSourceFiles(
            [
                'arm', 'avr', 'bfin', 'h8300', 'lm32', 'm32c', 'm32r', 'm68k',
                'mips', 'nios2', 'sh', 'sparc', 'v850'
            ],
            [
                'sys/mips/mips/in_cksum.c',
            ],
            mm.generator['source']()
        )

#
# DHCP
#
class dhcpcd(builder.Module):

    def __init__(self, manager):
        super(dhcpcd, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addSourceFiles(
            [
                'dhcpcd/arp.c',
                'dhcpcd/auth.c',
                'dhcpcd/bpf.c',
                'dhcpcd/common.c',
                'dhcpcd/dhcp6.c',
                'dhcpcd/dhcp.c',
                'dhcpcd/dhcpcd.c',
                'dhcpcd/dhcpcd-embedded.c',
                'dhcpcd/dhcp-common.c',
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
                'dhcpcd/platform-bsd.c',
                'dhcpcd/script.c',
                'dhcpcd/compat/pselect.c',
                'dhcpcd/crypt/hmac_md5.c',
            ],
            mm.generator['source']('-D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET')
        )
        self.addRTEMSSourceFiles(
            [
                'rtems/rtems-bsd-shell-dhcpcd.c',
            ],
            mm.generator['source']()
        )

#
# MDNS Responder
#
class mdnsresponder(builder.Module):

    def __init__(self, manager):
        super(mdnsresponder, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addSourceFiles(
            [
                'mDNSResponder/mDNSCore/anonymous.c',
                'mDNSResponder/mDNSCore/CryptoAlg.c',
                'mDNSResponder/mDNSCore/DNSCommon.c',
                'mDNSResponder/mDNSCore/DNSDigest.c',
                'mDNSResponder/mDNSCore/mDNS.c',
                'mDNSResponder/mDNSCore/uDNS.c',
                'mDNSResponder/mDNSShared/dnssd_clientshim.c',
                'mDNSResponder/mDNSShared/mDNSDebug.c',
                'mDNSResponder/mDNSShared/PlatformCommon.c',
                'mDNSResponder/mDNSShared/GenLinkedList.c',
                'mDNSResponder/mDNSPosix/mDNSPosix.c',
                'mDNSResponder/mDNSPosix/mDNSUNP.c',
                'rtemsbsd/mdns/mDNSResolveService.c',
                'rtemsbsd/mdns/mdns.c',
                'rtemsbsd/mdns/mdns-hostname-default.c',
            ],
            mm.generator['source']()
        )

#
# Mongoose HTTP
#
class mghttpd(builder.Module):

    def __init__(self, manager):
        super(mghttpd, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addSourceFiles(
            [
                'rtemsbsd/mghttpd/mongoose.c',
            ],
            mm.generator['source']('-DNO_SSL -DNO_POPEN -DNO_CGI -DUSE_WEBSOCKET')
        )

class dpaa(builder.Module):

    def __init__(self, manager):
        super(dpaa, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addLinuxHeaderFiles(
            [
                'include/soc/fsl/bman.h',
                'include/soc/fsl/qman.h',
                'drivers/net/ethernet/freescale/fman/mac.h',
                'drivers/net/ethernet/freescale/fman/fman.h',
                'drivers/net/ethernet/freescale/fman/fman_tgec.h',
                'drivers/net/ethernet/freescale/fman/fman_mac.h',
                'drivers/net/ethernet/freescale/fman/fman_muram.h',
                'drivers/net/ethernet/freescale/fman/fman_port.h',
                'drivers/net/ethernet/freescale/fman/fman_dtsec.h',
                'drivers/net/ethernet/freescale/fman/fman_memac.h',
                'drivers/net/ethernet/freescale/fman/fman_sp.h',
                'drivers/net/ethernet/freescale/dpaa/dpaa_eth.h',
                'drivers/net/ethernet/freescale/dpaa/dpaa_eth_trace.h',
                'drivers/net/ethernet/freescale/sdk_dpaa/dpaa_eth.h',
                'drivers/net/ethernet/freescale/sdk_dpaa/dpaa_eth_base.h',
                'drivers/net/ethernet/freescale/sdk_dpaa/dpaa_eth_common.h',
                'drivers/soc/fsl/qbman/qman_priv.h',
                'drivers/soc/fsl/qbman/bman_test.h',
                'drivers/soc/fsl/qbman/dpaa_sys.h',
                'drivers/soc/fsl/qbman/bman_priv.h',
                'drivers/soc/fsl/qbman/qman_test.h',
            ]
        )
        self.addCPUDependentLinuxSourceFiles(
            [ 'powerpc' ],
            [
                'drivers/net/ethernet/freescale/dpaa/dpaa_eth.c',
                'drivers/net/ethernet/freescale/fman/fman.c',
                'drivers/net/ethernet/freescale/fman/fman_dtsec.c',
                'drivers/net/ethernet/freescale/fman/fman_memac.c',
                'drivers/net/ethernet/freescale/fman/fman_port.c',
                'drivers/net/ethernet/freescale/fman/fman_sp.c',
                'drivers/net/ethernet/freescale/fman/fman_tgec.c',
                'drivers/net/ethernet/freescale/fman/mac.c',
                'drivers/net/ethernet/freescale/sdk_dpaa/dpaa_eth_base.c',
                'drivers/net/ethernet/freescale/sdk_dpaa/dpaa_eth_macless.c',
                'drivers/soc/fsl/qbman/bman.c',
                'drivers/soc/fsl/qbman/bman_ccsr.c',
                'drivers/soc/fsl/qbman/bman_test_api.c',
                'drivers/soc/fsl/qbman/qman.c',
                'drivers/soc/fsl/qbman/qman_ccsr.c',
                'drivers/soc/fsl/qbman/qman_portal.c',
                'drivers/soc/fsl/qbman/qman_test_api.c',
                'drivers/soc/fsl/qbman/qman_test_stash.c',
            ],
            mm.generator['source']()
        )
        self.addCPUDependentRTEMSSourceFiles(
            [ 'powerpc' ],
            [
                'sys/powerpc/compat.c',
                'sys/powerpc/fdt_phy.c',
                'sys/powerpc/fman_muram.c',
                'sys/powerpc/linux_compat.c',
                'sys/powerpc/drivers/net/ethernet/freescale/dpaa/if_fmanmac.c',
                'sys/powerpc/drivers/net/ethernet/freescale/sdk_dpaa/if_ml.c',
            ],
            mm.generator['source']()
        )

#
# Tests
#
#  Note: Keep as the last module
#
class tests(builder.Module):

    def __init__(self, manager):
        super(tests, self).__init__(manager, type(self).__name__)

    def generate(self):
        mm = self.manager
        self.addTest(mm.generator['test']('nfs01', ['test_main'], netTest = True))
        self.addTest(mm.generator['test']('foobarclient', ['test_main'],
                                          runTest = False, netTest = True))
        self.addTest(mm.generator['test']('foobarserver', ['test_main'],
                                          runTest = False, netTest = True))
        self.addTest(mm.generator['test']('dhcpcd01', ['test_main'],
                                          runTest = False, netTest = True))
        self.addTest(mm.generator['test']('dhcpcd02', ['test_main'],
                                          runTest = False, netTest = True))
        self.addTest(mm.generator['test']('arphole', ['test_main'],
                                          runTest = False, netTest = True))
        self.addTest(mm.generator['test']('telnetd01', ['test_main'],
                                          runTest = False, netTest = True,
                                          extraLibs = ['telnetd']))
        self.addTest(mm.generator['test']('unix01', ['test_main']))
        self.addTest(mm.generator['test']('ftpd01', ['test_main'],
                                          netTest = True,
                                          extraLibs = ['ftpd']))
        self.addTest(mm.generator['test']('ftpd02', ['test_main'],
                                          extraLibs = ['ftpfs', 'ftpd']))
        self.addTest(mm.generator['test']('ping01', ['test_main'], netTest = True))
        self.addTest(mm.generator['test']('selectpollkqueue01', ['test_main']))
        self.addTest(mm.generator['test']('rwlock01', ['test_main']))
        self.addTest(mm.generator['test']('sleep01', ['test_main']))
        self.addTest(mm.generator['test']('syscalls01', ['test_main']))
        self.addTest(mm.generator['test']('program01', ['test_main']))
        self.addTest(mm.generator['test']('commands01', ['test_main']))
        self.addTest(mm.generator['test']('usb01', ['init'], False))
        self.addTest(mm.generator['test']('usbserial01', ['init'], False))
        self.addTest(mm.generator['test']('usbkbd01', ['init'], False))
        self.addTest(mm.generator['test']('usbmouse01', ['init'], False))
        self.addTest(mm.generator['test']('evdev01', ['init'], False))
        self.addTest(mm.generator['test']('loopback01', ['test_main']))
        self.addTest(mm.generator['test']('netshell01', ['test_main', 'shellconfig'], False))
        self.addTest(mm.generator['test']('swi01', ['init', 'swi_test']))
        self.addTest(mm.generator['test']('timeout01', ['init', 'timeout_test']))
        self.addTest(mm.generator['test']('init01', ['test_main']))
        self.addTest(mm.generator['test']('thread01', ['test_main']))
        self.addTest(mm.generator['test']('mutex01', ['test_main']))
        self.addTest(mm.generator['test']('condvar01', ['test_main']))
        self.addTest(mm.generator['test']('ppp01', ['test_main'], runTest = False,
                                          extraLibs = ['ftpd', 'telnetd']))
        self.addTest(mm.generator['test']('zerocopy01', ['test_main'],
                                          runTest = False, netTest = True,
                                          extraLibs = ['telnetd']))
        self.addTest(mm.generator['test']('smp01', ['test_main']))
        self.addTest(mm.generator['test']('media01', ['test_main'],
                                          runTest = False,
                                          extraLibs = ['ftpd', 'telnetd']))
        self.addTest(mm.generator['test']('vlan01', ['test_main'], netTest = True))
        self.addTest(mm.generator['test']('lagg01', ['test_main'], netTest = True))
        self.addTest(mm.generator['test']('log01', ['test_main']))
        self.addTest(mm.generator['test']('rcconf01', ['test_main']))
        self.addTest(mm.generator['test']('rcconf02', ['test_main'],
                                          extraLibs = ['ftpd', 'telnetd']))
        self.addTest(mm.generator['test']('cdev01', ['test_main', 'test_cdev']))
        self.addTest(mm.generator['test']('pf01', ['test_main'],
                                          extraLibs = ['ftpd', 'telnetd']))
        self.addTest(mm.generator['test']('pf02', ['test_main'],
                                          runTest = False,
                                          extraLibs = ['ftpd', 'telnetd']))
        self.addTest(mm.generator['test']('termios', ['test_main',
                                                      'test_termios_driver',
                                                      'test_termios_utilities']))
        self.addTest(mm.generator['test']('termios01', ['test_main',
                                                        '../termios/test_termios_driver',
                                                        '../termios/test_termios_utilities']))
        self.addTest(mm.generator['test']('termios02', ['test_main',
                                                        '../termios/test_termios_driver',
                                                        '../termios/test_termios_utilities']))
        self.addTest(mm.generator['test']('termios03', ['test_main',
                                                        '../termios/test_termios_driver',
                                                        '../termios/test_termios_utilities']))
        self.addTest(mm.generator['test']('termios04', ['test_main',
                                                        '../termios/test_termios_driver',
                                                        '../termios/test_termios_utilities']))
        self.addTest(mm.generator['test']('termios05', ['test_main',
                                                        '../termios/test_termios_driver',
                                                        '../termios/test_termios_utilities']))
        self.addTest(mm.generator['test']('termios06', ['test_main',
                                                        '../termios/test_termios_driver',
                                                        '../termios/test_termios_utilities']))
        self.addTest(mm.generator['test-if-header']('debugger01', 'rtems/rtems-debugger.h',
                                                    ['test_main'], runTest = False, netTest = True,
                                                    extraLibs = ['debugger']))
        self.addTest(mm.generator['test']('crypto01', ['test_main']))
        self.addTest(mm.generator['test']('ipsec01', ['test_main']))

def load(mm):

    mm.setConfiguration(_defaults)

    mm.addModule(dpaa(mm))
    mm.addModule(rtems(mm))
    mm.addModule(base(mm))

    mm.addModule(fdt(mm))
    mm.addModule(tty(mm))
    mm.addModule(mmc(mm))
    mm.addModule(mmc_ti(mm))
    mm.addModule(dev_input(mm))
    mm.addModule(evdev(mm))

    mm.addModule(dev_usb(mm))
    mm.addModule(dev_usb_controller(mm))
    mm.addModule(dev_usb_quirk(mm))

    mm.addModule(dev_usb_input(mm))
    mm.addModule(dev_usb_serial(mm))
    mm.addModule(dev_usb_net(mm))
    mm.addModule(dev_usb_wlan(mm))
    mm.addModule(dev_wlan_rtwn(mm))

    mm.addModule(cam(mm))
    mm.addModule(dev_usb_storage(mm))
    mm.addModule(dev_usb_controller_bbb(mm))

    mm.addModule(net(mm))
    mm.addModule(netinet(mm))
    mm.addModule(netinet6(mm))
    mm.addModule(netipsec(mm))
    mm.addModule(net80211(mm))
    mm.addModule(opencrypto(mm))
    mm.addModule(crypto(mm))
    mm.addModule(altq(mm))
    mm.addModule(pf(mm))
    mm.addModule(dev_net(mm))

    # Add PCI
    mm.addModule(pci(mm))

    # Add NIC devices
    mm.addModule(dev_nic(mm))
    mm.addModule(dev_nic_re(mm))
    mm.addModule(dev_nic_fxp(mm))
    mm.addModule(dev_nic_e1000(mm))
    mm.addModule(dev_nic_dc(mm))
    mm.addModule(dev_nic_smc(mm))
    mm.addModule(dev_nic_broadcomm(mm))

    # Add in_chksum
    mm.addModule(in_cksum(mm))

    mm.addModule(user_space(mm))
    mm.addModule(user_space_wlanstats(mm))
    mm.addModule(contrib_expat(mm))
    mm.addModule(contrib_libpcap(mm))
    mm.addModule(usr_sbin_tcpdump(mm))
    mm.addModule(usr_sbin_wpa_supplicant(mm))
    mm.addModule(crypto_openssl(mm))

    mm.addModule(dhcpcd(mm))
    mm.addModule(mghttpd(mm))
    mm.addModule(mdnsresponder(mm))

    mm.addModule(tests(mm))

    mm.setModuleConfigiuration()

    # XXX TODO Check that no file is also listed in empty
    # XXX TODO Check that no file in in two modules
