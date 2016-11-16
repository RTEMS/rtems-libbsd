#
#  Copyright (c) 2015-2016 Chris Johns <chrisj@rtems.org>. All rights reserved.
#
#  Copyright (c) 2009, 2016 embedded brains GmbH.  All rights reserved.
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
# RTEMS version
#
def rtems_version():
    return '4.12'

#
# RTEMS
#
def rtems(mm):
    mod = builder.Module('rtems')
    mod.addRTEMSSourceFiles(
        [
            'local/bus_if.c',
            'local/cryptodev_if.c',
            'local/device_if.c',
            'local/miibus_if.c',
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
            'rtems/rtems-bsd-rc-conf-net.c',
            'rtems/rtems-bsd-rc-conf-pf.c',
            'rtems/rtems-bsd-rc-conf.c',
            'rtems/rtems-bsd-shell-arp.c',
            'rtems/rtems-bsd-shell-ifconfig.c',
            'rtems/rtems-bsd-shell-netstat.c',
            'rtems/rtems-bsd-shell-pfctl.c',
            'rtems/rtems-bsd-shell-ping.c',
            'rtems/rtems-bsd-shell-route.c',
            'rtems/rtems-bsd-shell-sysctl.c',
            'rtems/rtems-bsd-shell-tcpdump.c',
            'rtems/rtems-bsd-shell-vmstat.c',
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
            'rtems/rtems-kernel-thread.c',
            'rtems/rtems-kernel-timesupport.c',
            'rtems/rtems-kernel-vprintf.c',
            'rtems/rtems-legacy-rtrequest.c',
            'rtems/rtems-legacy-newproc.c',
            'rtems/rtems-legacy-mii.c',
            'rtems/rtems-kvm.c',
            'rtems/rtems-program.c',
            'rtems/rtems-program-socket.c',
            'rtems/rtems-routes.c',
            'rtems/syslog.c',
            'ftpd/ftpd.c',
            'ftpd/ftpd-init.c',
            'ftpd/ftpd-service.c',
            'ftpfs/ftpfs.c',
            'mdns/mdns.c',
            'mdns/mdns-hostname-default.c',
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
            'sys/dev/usb/controller/ehci_mpc83xx.c',
            'sys/dev/usb/controller/ohci_lpc.c',
            'sys/dev/usb/controller/dwc_otg_nexus.c',
            'sys/dev/usb/controller/usb_otg_transceiver.c',
            'sys/dev/usb/controller/usb_otg_transceiver_dump.c',
            'sys/dev/smc/if_smc_nexus.c',
            'sys/dev/ffec/if_ffec_mcf548x.c',
            'sys/dev/dw_mmc/dw_mmc.c',
            'sys/fs/devfs/devfs_devs.c',
            'sys/net/if_ppp.c',
            'sys/net/ppp_tty.c',
            'telnetd/check_passwd.c',
            'telnetd/des.c',
            'telnetd/pty.c',
            'telnetd/telnetd.c',
            'telnetd/telnetd-init.c',
            'telnetd/telnetd-service.c',
            'sys/dev/tsec/if_tsec_nexus.c',
        ],
        mm.generator['source']()
    )
    mod.addRTEMSSourceFiles(
        [
            'debugger/rtems-debugger-remote-tcp.c',
        ],
        mm.generator['source-if-header']('rtems/rtems-debugger.h')
    )
    mod.addFile(mm.generator['file']('rtems/rtems-kernel-kvm-symbols.c',
                                     mm.generator['rtems-path'](),
                                     mm.generator['no-convert'](),
                                     mm.generator['no-convert'](),
                                     mm.generator['kvm-symbols'](includes = 'rtemsbsd/rtems')))
    mod.addFile(mm.generator['file']('lib/libc/net/nslexer.l',
                                     mm.generator['freebsd-path'](),
                                     mm.generator['convert'](),
                                     mm.generator['convert'](),
                                     mm.generator['lex']('_nsyy', 'nsparser.c')))
    mod.addFile(mm.generator['file']('lib/libc/net/nsparser.y',
                                     mm.generator['freebsd-path'](),
                                     mm.generator['convert'](),
                                     mm.generator['convert'](),
                                     mm.generator['yacc']('_nsyy', 'nsparser.h')))
    mod.addFile(mm.generator['file']('lib/libipsec/policy_token.l',
                                     mm.generator['freebsd-path'](),
                                     mm.generator['convert'](),
                                     mm.generator['convert'](),
                                     mm.generator['lex']('__libipsecyy', 'policy_parse.c')))
    mod.addFile(mm.generator['file']('lib/libipsec/policy_parse.y',
                                     mm.generator['freebsd-path'](),
                                     mm.generator['convert'](),
                                     mm.generator['convert'](),
                                     mm.generator['yacc']('__libipsecyy', 'y.tab.h')))
    return mod

#
# Base
#
def base(mm):
    mod = builder.Module('base')
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/bsm/audit.h',
            'sys/bsm/audit_kevents.h',
            'sys/fs/devfs/devfs_int.h',
            'sys/security/audit/audit.h',
            'sys/security/mac/mac_framework.h',
            'sys/sys/acl.h',
            'sys/sys/aio.h',
            'sys/sys/_bitset.h',
            'sys/sys/bitset.h',
            'sys/sys/bitstring.h',
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
            'sys/sys/_cpuset.h',
            'sys/sys/ctype.h',
            'sys/sys/domain.h',
            'sys/sys/eventhandler.h',
            'sys/sys/fail.h',
            'sys/sys/filedesc.h',
            'sys/sys/file.h',
            'sys/sys/firmware.h',
            'sys/sys/fnv_hash.h',
            'sys/sys/gpio.h',
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
            'sys/sys/_sockaddr_storage.h',
            'sys/sys/sockbuf.h',
            'sys/sys/socket.h',
            'sys/sys/socketvar.h',
            'sys/sys/sockopt.h',
            'sys/sys/sockstate.h',
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
    mod.addKernelSpaceSourceFiles(
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
            'sys/libkern/bcd.c',
            'sys/libkern/inet_ntoa.c',
            'sys/libkern/jenkins_hash.c',
            'sys/libkern/murmur3_32.c',
            'sys/libkern/random.c',
            'sys/vm/uma_core.c',
            'sys/vm/uma_dbg.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# MMC
#
def mmc(mm):
    mod = builder.Module('mmc')
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/dev/mmc/bridge.h',
            'sys/dev/mmc/mmcbrvar.h',
            'sys/dev/mmc/mmcreg.h',
            'sys/dev/mmc/mmcvar.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
            'sys/dev/mmc/mmc.c',
            'sys/dev/mmc/mmcsd.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# USB
#
def dev_usb(mm):
    mod = builder.Module('dev_usb')
    mod.addKernelSpaceHeaderFiles(
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
    mod.addKernelSpaceSourceFiles(
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
    return mod

#
# USB Add Ons
#
def dev_usb_add_on(mm):
    mod.Module('dev_usb_add_on')
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/dev/usb/usb_pci.h',
            'sys/dev/usb/usb_compat_linux.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
            'sys/dev/usb/usb_compat_linux.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# USB Bluetooth
#
def dev_usb_bluetooth(mm):
    mod = builder.Module('dev_usb_bluetooth')
    mod.addDependency(mm['dev_usb'])
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/dev/usb/bluetooth/ng_ubt_var.h'
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
            'sys/dev/usb/bluetooth/ng_ubt.c',
            'sys/dev/usb/bluetooth/ubtbcmfw.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# USB Controller.
#
def dev_usb_controller(mm):
    mod = builder.Module('dev_usb_controller')
    mod.addDependency(mm['dev_usb'])
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/dev/usb/controller/ohci.h',
            'sys/dev/usb/controller/ohcireg.h',
            'sys/dev/usb/controller/ehci.h',
            'sys/dev/usb/controller/ehcireg.h',
            'sys/dev/usb/controller/uhcireg.h',
            'sys/dev/usb/controller/xhcireg.h',
            'sys/dev/usb/controller/dwc_otg.h',
            'sys/dev/usb/controller/dwc_otgreg.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
            'sys/dev/usb/controller/ohci.c',
            'sys/dev/usb/controller/ehci.c',
            'sys/dev/usb/controller/usb_controller.c',
            'sys/dev/usb/controller/dwc_otg.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# USB Controller Add Ons
#
def dev_usb_controller_add_on(mm):
    mod = builder.Module('dev_usb_controller_add_on')
    mod.addDependency(mm['dev_usb'])
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/dev/usb/controller/at91dci.h',
            'sys/dev/usb/controller/atmegadci.h',
            'sys/dev/usb/controller/musb_otg.h',
            'sys/dev/usb/controller/uss820dci.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
            'sys/dev/usb/controller/at91dci_atmelarm.c',
            'sys/dev/usb/controller/at91dci.c',
            'sys/dev/usb/controller/atmegadci_atmelarm.c',
            'sys/dev/usb/controller/atmegadci.c',
            'sys/dev/usb/controller/ehci_ixp4xx.c',
            'sys/dev/usb/controller/ehci_pci.c',
            'sys/dev/usb/controller/musb_otg.c',
            'sys/dev/usb/controller/ehci_mbus.c',
            'sys/dev/usb/controller/musb_otg_atmelarm.c',
            'sys/dev/usb/controller/ohci_atmelarm.c',
            'sys/dev/usb/controller/ohci_pci.c',
            'sys/dev/usb/controller/uhci_pci.c',
            'sys/dev/usb/controller/uss820dci_atmelarm.c',
            'sys/dev/usb/controller/uss820dci.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# USB Input
#
def dev_usb_input(mm):
    mod = builder.Module('dev_usb_input')
    mod.addDependency(mm['dev_usb'])
    mod.addKernelSpaceHeaderFiles(
        [
    'sys/dev/usb/input/usb_rdesc.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
            'sys/dev/usb/input/uhid.c',
            'sys/dev/usb/input/ukbd.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# USB Mouse
#
def dev_usb_mouse(mm):
    mod = builder.Module('dev_usb_mouse')
    mod.addDependency(mm['dev_usb'])
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/sys/tty.h',
            'sys/sys/mouse.h',
            'sys/sys/ttyqueue.h',
            'sys/sys/ttydefaults.h',
            'sys/sys/ttydisc.h',
            'sys/sys/ttydevsw.h',
            'sys/sys/ttyhook.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
            'sys/dev/usb/input/ums.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# USB Misc.
#
def dev_usb_misc(mm):
    mod = builder.Module('dev_usb_misc')
    mod.addDependency(mm['dev_usb'])
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/dev/usb/misc/udbp.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
            'sys/dev/usb/misc/udbp.c',
            'sys/dev/usb/misc/ufm.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# USB Networking
#
def dev_usb_net(mm):
    mod = builder.Module('dev_usb_net')
    mod.addDependency(mm['dev_usb'])
    mod.addKernelSpaceHeaderFiles(
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
    mod.addKernelSpaceSourceFiles(
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
    return mod

#
# USB Quirks
#
def dev_usb_quirk(mm):
    mod = builder.Module('dev_usb_quirk')
    mod.addDependency(mm['dev_usb'])
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/dev/usb/quirk/usb_quirk.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
            'sys/dev/usb/quirk/usb_quirk.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# USB Serial
#
def dev_usb_serial(mm):
    mod = builder.Module('dev_usb_serial')
    mod.addDependency(mm['dev_usb'])
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/dev/usb/serial/uftdi_reg.h',
            'sys/dev/usb/serial/usb_serial.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
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
    return mod

#
# USB Storage
#
def dev_usb_storage(mm):
    mod = builder.Module('dev_usb_storage')
    mod.addDependency(mm['dev_usb'])
    mod.addKernelSpaceSourceFiles(
        [
            'sys/dev/usb/storage/umass.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# USB Storage Add Ons
#
def dev_usb_storage_add_on(mm):
    mod = builder.Module('dev_usb_storage_add_on')
    mod.addDependency(mm['dev_usb'])
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/dev/usb/storage/rio500_usb.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
            'sys/dev/usb/storage/urio.c',
            'sys/dev/usb/storage/ustorage_fs.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# USB Template
#
def dev_usb_template(mm):
    mod = builder.Module('dev_usb_template')
    mod.addDependency(mm['dev_usb'])
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/dev/usb/template/usb_template.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
            'sys/dev/usb/template/usb_template.c',
            'sys/dev/usb/template/usb_template_cdce.c',
            'sys/dev/usb/template/usb_template_msc.c',
            'sys/dev/usb/template/usb_template_mtp.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# USB WLAN
#
def dev_usb_wlan(mm):
    mod = builder.Module('dev_usb_wlan')
    mod.addDependency(mm['dev_usb'])
    mod.addKernelSpaceHeaderFiles(
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
    mod.addKernelSpaceSourceFiles(
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
    return mod

#
# CAM
#
def cam(mm):
    mod = builder.Module('cam')
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/dev/nvme/nvme.h',
            'sys/sys/ata.h',
            'sys/cam/cam.h',
            'sys/cam/cam_ccb.h',
            'sys/cam/cam_sim.h',
            'sys/cam/cam_xpt_sim.h',
            'sys/cam/nvme/nvme_all.h',
            'sys/cam/scsi/scsi_all.h',
            'sys/cam/scsi/scsi_da.h',
            'sys/cam/ata/ata_all.h',
            'sys/cam/cam_periph.h',
            'sys/cam/cam_debug.h',
            'sys/cam/cam_xpt.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
            'sys/cam/cam.c',
            'sys/cam/scsi/scsi_all.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# Networking Devices
#
def dev_net(mm):
    mod = builder.Module('dev_net')
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/dev/mii/mii.h',
            'sys/dev/mii/mii_bitbang.h',
            'sys/dev/mii/miivar.h',
            'sys/dev/mii/brgphyreg.h',
            'sys/dev/mii/e1000phyreg.h',
            'sys/dev/mii/icsphyreg.h',
            'sys/dev/mii/rgephyreg.h',
            'sys/dev/led/led.h',
            'sys/net/bpf.h',
            'sys/net/ethernet.h',
            'sys/net/if_arp.h',
            'sys/net/if_dl.h',
            'sys/net/if.h',
            'sys/net/if_media.h',
            'sys/net/ifq.h',
            'sys/net/if_types.h',
            'sys/net/if_var.h',
            'sys/net/vnet.h',
            'sys/dev/ofw/openfirm.h',
            'sys/dev/tsec/if_tsec.h',
            'sys/dev/tsec/if_tsecreg.h',
            'sys/dev/cadence/if_cgem_hw.h',
            'sys/dev/dwc/if_dwc.h',
            'sys/dev/dwc/if_dwcvar.h',
            'sys/arm/xilinx/zy7_slcr.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
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
            'sys/dev/cadence/if_cgem.c',
            'sys/dev/dwc/if_dwc.c',
            'sys/arm/xilinx/zy7_slcr.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# Network Interface Controllers (NIC)
#
def dev_nic(mm):
    mod = builder.Module('dev_nic')
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/sys/pciio.h',
            'sys/sys/eventvar.h',
            'sys/sys/kenv.h',
            'sys/isa/isavar.h',
            'sys/isa/pnpvar.h',
            'sys/sys/buf.h',
            'sys/sys/mqueue.h',
            'sys/sys/tty.h',
            'sys/sys/ttyqueue.h',
            'sys/sys/ttydisc.h',
            'sys/sys/ttydevsw.h',
            'sys/sys/ttyhook.h',
            'sys/sys/user.h',
        ]
    )
    mod.addCPUDependentHeaderFiles(
        [
            'sys/arm/include/cpufunc.h',
            'sys/i386/include/specialreg.h',
            'sys/i386/include/md_var.h',
            'sys/i386/include/intr_machdep.h',
            'sys/i386/include/cpufunc.h',
            'sys/mips/include/cpufunc.h',
            'sys/mips/include/cpuregs.h',
            'sys/powerpc/include/cpufunc.h',
            'sys/powerpc/include/psl.h',
            'sys/powerpc/include/spr.h',
            'sys/sparc64/include/cpufunc.h',
            'sys/sparc64/include/asi.h',
            'sys/sparc64/include/pstate.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
            'sys/netinet/tcp_hostcache.c',
            'sys/dev/led/led.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# NIC RE
#
def dev_nic_re(mm):
    mod = builder.Module('dev_nic_re')
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/dev/rl/if_rlreg.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
            'sys/dev/re/if_re.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# NIC FXP
#
def dev_nic_fxp(mm):
    mod = builder.Module('dev_nic_fxp')
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/dev/fxp/if_fxpreg.h',
            'sys/dev/fxp/if_fxpvar.h',
            'sys/dev/fxp/rcvbundl.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
            'sys/dev/fxp/if_fxp.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# NIC E1000
#
def dev_nic_e1000(mm):
    mod = builder.Module('dev_nic_e1000')
    mod.addKernelSpaceHeaderFiles(
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
            'sys/dev/e1000/if_igb.h',
            'sys/dev/e1000/if_lem.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
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
            'sys/dev/e1000/if_em.c',
            'sys/dev/e1000/if_igb.c',
            'sys/dev/e1000/if_lem.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# DEC Tulip aka Intel 21143
#
def dev_nic_dc(mm):
    mod = builder.Module('dev_nic_dc')
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/dev/dc/if_dcreg.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
            'sys/dev/dc/dcphy.c',
            'sys/dev/dc/if_dc.c',
            'sys/dev/dc/pnphy.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# SMC9111x
#
def dev_nic_smc(mm):
    mod = builder.Module('dev_nic_smc')
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/dev/smc/if_smcreg.h',
            'sys/dev/smc/if_smcvar.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
            'sys/dev/smc/if_smc.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# Crystal Semiconductor CS8900
#
def dev_nic_cs(mm):
    mod = builder.Module('dev_nic_cs')
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/dev/cs/if_csreg.h',
            'sys/dev/cs/if_csvar.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
            'sys/dev/cs/if_cs.c',
            'sys/dev/cs/if_cs_isa.c',
            'sys/dev/cs/if_cs_pccard.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# Broadcomm BCE, BFE, BGE - MII is intertwined
#
def dev_nic_broadcomm(mm):
    mod = builder.Module('dev_nic_broadcomm')
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/dev/bce/if_bcefw.h',
            'sys/dev/bce/if_bcereg.h',
            'sys/dev/bfe/if_bfereg.h',
            'sys/dev/bge/if_bgereg.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
            'sys/dev/bce/if_bce.c',
            'sys/dev/bfe/if_bfe.c',
            'sys/dev/bge/if_bge.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# Networking
#
def net(mm):
    mod = builder.Module('net')
    mod.addKernelSpaceHeaderFiles(
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
            'sys/net/flowtable.h',
            'sys/net/ieee8023ad_lacp.h',
            'sys/net/if_arc.h',
            'sys/net/if_arp.h',
            'sys/net/if_atm.h',
            'sys/net/if_bridgevar.h',
            'sys/net/if_clone.h',
            'sys/net/if_dl.h',
            'sys/net/if_enc.h',
            'sys/net/if_gif.h',
            'sys/net/if_gre.h',
            'sys/net/if.h',
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
    mod.addKernelSpaceSourceFiles(
        [
            'sys/kern/sys_socket.c',
            'sys/kern/uipc_syscalls.c',
            'sys/net/bridgestp.c',
            'sys/net/ieee8023ad_lacp.c',
            'sys/net/if_atmsubr.c',
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
    return mod

#
# Internet Networking
#
def netinet(mm):
    mod = builder.Module('netinet')
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/netinet/cc/cc.h',
            'sys/netinet/cc/cc_module.h',
            'sys/netinet/in_fib.h',
            'sys/netinet/icmp6.h',
            'sys/netinet/icmp_var.h',
            'sys/netinet/if_atm.h',
            'sys/netinet/if_ether.h',
            'sys/netinet/igmp.h',
            'sys/netinet/igmp_var.h',
            'sys/netinet/in.h',
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
            'sys/netinet/ip_ipsec.h',
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
            'sys/netinet/tcp.h',
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
    mod.addKernelSpaceSourceFiles(
        [
            'sys/netinet/accf_data.c',
            'sys/netinet/accf_dns.c',
            'sys/netinet/accf_http.c',
            'sys/netinet/cc/cc.c',
            'sys/netinet/cc/cc_newreno.c',
            'sys/netinet/if_atm.c',
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
    return mod

#
# IPv6
#
def netinet6(mm):
    mod = builder.Module('netinet6')
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/netinet6/icmp6.h',
            'sys/netinet6/in6_fib.h',
            'sys/netinet6/in6.h',
            'sys/netinet6/in6_ifattach.h',
            'sys/netinet6/in6_pcb.h',
            'sys/netinet6/in6_rss.h',
            'sys/netinet6/in6_var.h',
            'sys/netinet6/ip6_ecn.h',
            'sys/netinet6/ip6.h',
            'sys/netinet6/ip6_ipsec.h',
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
    mod.addKernelSpaceSourceFiles(
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
    return mod

#
# IPsec
#
def netipsec(mm):
    mod = builder.Module('netipsec')
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/netipsec/ah.h',
            'sys/netipsec/ah_var.h',
            'sys/netipsec/esp.h',
            'sys/netipsec/esp_var.h',
            'sys/netipsec/ipcomp.h',
            'sys/netipsec/ipcomp_var.h',
            'sys/netipsec/ipip_var.h',
            'sys/netipsec/ipsec6.h',
            'sys/netipsec/ipsec.h',
            'sys/netipsec/keydb.h',
            'sys/netipsec/key_debug.h',
            'sys/netipsec/key.h',
            'sys/netipsec/keysock.h',
            'sys/netipsec/key_var.h',
            'sys/netipsec/xform.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
            'sys/netinet/ip_ipsec.c',
            'sys/netinet6/ip6_ipsec.c',
            'sys/netipsec/ipsec.c',
            'sys/netipsec/ipsec_input.c',
            'sys/netipsec/ipsec_mbuf.c',
            'sys/netipsec/ipsec_output.c',
            'sys/netipsec/key.c',
            'sys/netipsec/key_debug.c',
            'sys/netipsec/keysock.c',
            'sys/netipsec/xform_ah.c',
            'sys/netipsec/xform_esp.c',
            'sys/netipsec/xform_ipcomp.c',
            'sys/netipsec/xform_ipip.c',
            'sys/netipsec/xform_tcp.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# IEEE 802.11
#
def net80211(mm):
    mod = builder.Module('net80211')
    mod.addKernelSpaceHeaderFiles(
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
            'sys/net80211/ieee80211_wds.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
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
            'sys/net80211/ieee80211_wds.c',
            'sys/net80211/ieee80211_xauth.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# Open Crypto
#
def opencrypto(mm):
    mod = builder.Module('opencrypto')
    mod.addKernelSpaceHeaderFiles(
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
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
            'sys/opencrypto/cast.c',
            'sys/opencrypto/criov.c',
            'sys/opencrypto/crypto.c',
            'sys/opencrypto/cryptodeflate.c',
            'sys/opencrypto/cryptosoft.c',
            'sys/opencrypto/gfmult.c',
            'sys/opencrypto/gmac.c',
            'sys/opencrypto/rmd160.c',
            'sys/opencrypto/skipjack.c',
            'sys/opencrypto/xform.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# Crypto
#
def crypto(mm):
    mod = builder.Module('crypto')
    mod.addKernelSpaceHeaderFiles(
        [
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
    mod.addKernelSpaceSourceFiles(
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
    return mod

#
# Altq
#
def altq(mm):
    mod = builder.Module('altq')
    mod.addKernelSpaceHeaderFiles(
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
    mod.addKernelSpaceSourceFiles(
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
    return mod

#
# Packet filter
#
def pf(mm):
    mod = builder.Module('pf')
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/net/if_pflog.h',
            'sys/net/if_pfsync.h',
            'sys/net/pfvar.h',
            'sys/netpfil/pf/pf_altq.h',
            'sys/netpfil/pf/pf.h',
            'sys/netpfil/pf/pf_mtag.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
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
    return mod

def ipfw(mm):
    mod = builder.Module('ipfw')
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/netinet6/ip_fw_nat64.h',
            'sys/netinet6/ip_fw_nptv6.h',
            'sys/netpfil/ipfw/dn_aqm_codel.h',
            'sys/netpfil/ipfw/dn_aqm.h',
            'sys/netpfil/ipfw/dn_aqm_pie.h',
            'sys/netpfil/ipfw/dn_heap.h',
            'sys/netpfil/ipfw/dn_sched_fq_codel.h',
            'sys/netpfil/ipfw/dn_sched_fq_codel_helper.h',
            'sys/netpfil/ipfw/dn_sched.h',
            'sys/netpfil/ipfw/ip_dn_private.h',
            'sys/netpfil/ipfw/ip_fw_private.h',
            'sys/netpfil/ipfw/ip_fw_table.h',
            'sys/netpfil/ipfw/nat64/ip_fw_nat64.h',
            'sys/netpfil/ipfw/nat64/nat64lsn.h',
            'sys/netpfil/ipfw/nat64/nat64stl.h',
            'sys/netpfil/ipfw/nat64/nat64_translate.h',
            'sys/netpfil/ipfw/nptv6/nptv6.h',
        ]
    )
    mod.addKernelSpaceSourceFiles(
        [
            'sys/netpfil/ipfw/ip_fw2.c',
            'sys/netpfil/ipfw/ip_fw_bpf.c',
            'sys/netpfil/ipfw/ip_fw_dynamic.c',
            'sys/netpfil/ipfw/ip_fw_eaction.c',
            'sys/netpfil/ipfw/ip_fw_iface.c',
            'sys/netpfil/ipfw/ip_fw_log.c',
            'sys/netpfil/ipfw/ip_fw_nat.c',
            'sys/netpfil/ipfw/ip_fw_pfil.c',
            'sys/netpfil/ipfw/ip_fw_sockopt.c',
            'sys/netpfil/ipfw/ip_fw_table_algo.c',
            'sys/netpfil/ipfw/ip_fw_table.c',
            'sys/netpfil/ipfw/ip_fw_table_value.c',
            'sys/netpfil/ipfw/nat64/ip_fw_nat64.c',
            'sys/netpfil/ipfw/nat64/nat64lsn.c',
            'sys/netpfil/ipfw/nat64/nat64lsn_control.c',
            'sys/netpfil/ipfw/nat64/nat64stl.c',
            'sys/netpfil/ipfw/nat64/nat64stl_control.c',
            'sys/netpfil/ipfw/nat64/nat64_translate.c',
            'sys/netpfil/ipfw/nptv6/ip_fw_nptv6.c',
            'sys/netpfil/ipfw/nptv6/nptv6.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# PCI
#
def pci(mm):
    mod = builder.Module('pci')
    mod.addKernelSpaceSourceFiles(
        [
            'sys/dev/pci/pci.c',
            'sys/dev/pci/pci_user.c',
            'sys/dev/pci/pci_pci.c',
        ],
        mm.generator['source']()
    )
    mod.addKernelSpaceHeaderFiles(
        [
            'sys/dev/pci/pcib_private.h',
            'sys/dev/pci/pci_private.h',
            'sys/dev/pci/pcireg.h',
            'sys/dev/pci/pcivar.h',
            'sys/dev/pci/pcivar.h',
        ]
    )
    mod.addCPUDependentHeaderFiles(
        [
            'sys/i386/include/_bus.h',
            'sys/i386/include/legacyvar.h',
            'sys/x86/include/bus.h',
            'sys/x86/include/pci_cfgreg.h',
        ]
    )
    mod.addCPUDependentSourceFiles(
        [ 'i386' ],
        [
            'sys/i386/i386/legacy.c',
            'sys/x86/pci/pci_bus.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# User space
#
def user_space(mm):
    mod = builder.Module('user_space')
    mod.addUserSpaceHeaderFiles(
        [
            'contrib/libxo/libxo/xo_buf.h',
            'contrib/libxo/libxo/xo_encoder.h',
            'contrib/libxo/libxo/xo.h',
            'contrib/libxo/libxo/xo_humanize.h',
            'contrib/libxo/libxo/xo_wcwidth.h',
            'sbin/pfctl/pfctl.h',
            'sbin/pfctl/pfctl_parser.h',
            'include/arpa/ftp.h',
            'include/arpa/inet.h',
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
            'lib/libipsec/ipsec_strerror.h',
            'lib/libipsec/libpfkey.h',
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
    mod.addFile(mm.generator['file']('include/rpc/rpcb_prot.x',
                                     mm.generator['freebsd-path'](),
                                     mm.generator['convert'](),
                                     mm.generator['convert'](),
                                     mm.generator['rpc-gen']()))
    mod.addFile(mm.generator['file']('include/rpcsvc/nis.x',
                                     mm.generator['freebsd-path'](),
                                     mm.generator['convert'](),
                                     mm.generator['convert'](),
                                     mm.generator['rpc-gen']()))
    mod.addFile(mm.generator['file']('sbin/route/keywords',
                                     mm.generator['freebsd-path'](),
                                     mm.generator['convert'](),
                                     mm.generator['convert'](),
                                     mm.generator['route-keywords']()))
    mod.addFile(mm.generator['file']('sbin/pfctl/parse.y',
                                     mm.generator['freebsd-path'](),
                                     mm.generator['convert'](),
                                     mm.generator['convert'](),
                                     mm.generator['yacc']('pfctly',
                                                          'parse.h')))
    mod.addUserSpaceSourceFiles(
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
        mm.generator['source']('-D__DBINTERFACE_PRIVATE -DINET6')
    )
    mod.addUserSpaceSourceFiles(
        [
            'bin/hostname/hostname.c',
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
            'lib/libc/stdlib/strtoimax.c',
            'lib/libc/stdlib/strtonum.c',
            'lib/libc/stdlib/strtoumax.c',
            'lib/libc/string/strsep.c',
            'lib/libc/xdr/xdr_array.c',
            'lib/libc/xdr/xdr.c',
            'lib/libc/xdr/xdr_float.c',
            'lib/libc/xdr/xdr_mem.c',
            'lib/libc/xdr/xdr_rec.c',
            'lib/libc/xdr/xdr_reference.c',
            'lib/libc/xdr/xdr_sizeof.c',
            'lib/libc/xdr/xdr_stdio.c',
            'lib/libipsec/ipsec_dump_policy.c',
            'lib/libipsec/ipsec_get_policylen.c',
            'lib/libipsec/ipsec_strerror.c',
            'lib/libipsec/pfkey.c',
            'lib/libipsec/pfkey_dump.c',
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
            'usr.bin/netstat/flowtable.c',
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
        mm.generator['source'](['-DINET6', '-DINET'])
    )
    return mod

#
# Contrib expat
#
def contrib_expat(mm):
    mod = builder.Module('contrib_expat')
    cflags = ['-DHAVE_MEMMOVE=1']
    mod.addRTEMSHeaderFiles(
        [
            'include/bsdxml.h',
        ]
    )
    mod.addUserSpaceHeaderFiles(
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
    mod.addUserSpaceSourceFiles(
        [
            'contrib/expat/lib/xmlparse.c',
            'contrib/expat/lib/xmlrole.c',
            'contrib/expat/lib/xmltok.c',
            'contrib/expat/lib/xmltok_impl.c',
            'contrib/expat/lib/xmltok_ns.c',
        ],
        mm.generator['source'](cflags)
    )
    return mod

#
# Contrib libpcap
#
def contrib_libpcap(mm):
    mod = builder.Module('contrib_libpcap')
    cflags = ['-D__FreeBSD__=1',
              '-DBSD=1',
              '-DINET6',
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
    mod.addUserSpaceHeaderFiles(
        [
            'contrib/libpcap/arcnet.h',
            'contrib/libpcap/atmuni31.h',
            'contrib/libpcap/ethertype.h',
            'contrib/libpcap/gencode.h',
            'contrib/libpcap/ieee80211.h',
            'contrib/libpcap/llc.h',
            'contrib/libpcap/nlpid.h',
            'contrib/libpcap/pcap-common.h',
            'contrib/libpcap/pcap-int.h',
            'contrib/libpcap/pcap-namedb.h',
            'contrib/libpcap/pcap.h',
            'contrib/libpcap/pcap/ipnet.h',
            'contrib/libpcap/pcap/namedb.h',
            'contrib/libpcap/pcap/pcap.h',
            'contrib/libpcap/pcap/sll.h',
            'contrib/libpcap/pcap/usb.h',
            'contrib/libpcap/ppp.h',
            'contrib/libpcap/sf-pcap-ng.h',
            'contrib/libpcap/sf-pcap.h',
            'contrib/libpcap/sunatmpos.h',
        ]
    )
    gen_cflags = cflags + ['-DNEED_YYPARSE_WRAPPER=1',
                           '-Dyylval=pcap_lval']
    mod.addFile(mm.generator['file']('contrib/libpcap/scanner.l',
                                     mm.generator['freebsd-path'](),
                                     mm.generator['convert'](),
                                     mm.generator['convert'](),
                                     mm.generator['lex']('pcap',
                                                         'scanner.c',
                                                         gen_cflags)))
    mod.addFile(mm.generator['file']('contrib/libpcap/grammar.y',
                                     mm.generator['freebsd-path'](),
                                     mm.generator['convert'](),
                                     mm.generator['convert'](),
                                     mm.generator['yacc']('pcap',
                                                          'tokdefs.h',
                                                          gen_cflags)))
    mod.addUserSpaceSourceFiles(
        [
            'contrib/libpcap/bpf_image.c',
            'contrib/libpcap/etherent.c',
            'contrib/libpcap/fad-getad.c',
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
    return mod

#
# /usr/sbin/tcpdump
#
def usr_sbin_tcpdump(mm):
    mod = builder.Module('usr_sbin_tcpdump')
    mod.addUserSpaceHeaderFiles(
        [
            'contrib/tcpdump/addrtoname.h',
            'contrib/tcpdump/af.h',
            'contrib/tcpdump/ah.h',
            'contrib/tcpdump/aodv.h',
            'contrib/tcpdump/appletalk.h',
            'contrib/tcpdump/arcnet.h',
            'contrib/tcpdump/atm.h',
            'contrib/tcpdump/bgp.h',
            'contrib/tcpdump/bootp.h',
            'contrib/tcpdump/chdlc.h',
            'contrib/tcpdump/cpack.h',
            'contrib/tcpdump/dccp.h',
            'contrib/tcpdump/decnet.h',
            'contrib/tcpdump/decode_prefix.h',
            'contrib/tcpdump/enc.h',
            'contrib/tcpdump/esp.h',
            'contrib/tcpdump/ether.h',
            'contrib/tcpdump/ethertype.h',
            'contrib/tcpdump/extract.h',
            'contrib/tcpdump/fddi.h',
            'contrib/tcpdump/forces.h',
            'contrib/tcpdump/gmpls.h',
            'contrib/tcpdump/gmt2local.h',
            'contrib/tcpdump/icmp6.h',
            'contrib/tcpdump/ieee802_11.h',
            'contrib/tcpdump/ieee802_11_radio.h',
            'contrib/tcpdump/igrp.h',
            'contrib/tcpdump/interface.h',
            'contrib/tcpdump/ip.h',
            'contrib/tcpdump/ip6.h',
            'contrib/tcpdump/ipfc.h',
            'contrib/tcpdump/ipnet.h',
            'contrib/tcpdump/ipproto.h',
            'contrib/tcpdump/ipsec_doi.h',
            'contrib/tcpdump/ipx.h',
            'contrib/tcpdump/isakmp.h',
            'contrib/tcpdump/l2tp.h',
            'contrib/tcpdump/l2vpn.h',
            'contrib/tcpdump/lane.h',
            'contrib/tcpdump/llc.h',
            'contrib/tcpdump/machdep.h',
            'contrib/tcpdump/mib.h',
            'contrib/tcpdump/mpls.h',
            'contrib/tcpdump/nameser.h',
            'contrib/tcpdump/netbios.h',
            'contrib/tcpdump/netdissect.h',
            'contrib/tcpdump/nfs.h',
            'contrib/tcpdump/nfsfh.h',
            'contrib/tcpdump/nlpid.h',
            'contrib/tcpdump/ntp.h',
            'contrib/tcpdump/oakley.h',
            'contrib/tcpdump/ospf.h',
            'contrib/tcpdump/ospf6.h',
            'contrib/tcpdump/oui.h',
            'contrib/tcpdump/pcap-missing.h',
            'contrib/tcpdump/pmap_prot.h',
            'contrib/tcpdump/ppi.h',
            'contrib/tcpdump/ppp.h',
            'contrib/tcpdump/route6d.h',
            'contrib/tcpdump/rpc_auth.h',
            'contrib/tcpdump/rpc_msg.h',
            'contrib/tcpdump/rx.h',
            'contrib/tcpdump/sctpConstants.h',
            'contrib/tcpdump/sctpHeader.h',
            'contrib/tcpdump/setsignal.h',
            'contrib/tcpdump/signature.h',
            'contrib/tcpdump/slcompress.h',
            'contrib/tcpdump/slip.h',
            'contrib/tcpdump/sll.h',
            'contrib/tcpdump/smb.h',
            'contrib/tcpdump/tcp.h',
            'contrib/tcpdump/tcpdump-stdinc.h',
            'contrib/tcpdump/telnet.h',
            'contrib/tcpdump/tftp.h',
            'contrib/tcpdump/timed.h',
            'contrib/tcpdump/token.h',
            'contrib/tcpdump/udp.h',
            'usr.sbin/tcpdump/tcpdump/config.h',
        ]
    )
    mod.addUserSpaceSourceFiles(
        [
            'contrib/tcpdump/addrtoname.c',
            'contrib/tcpdump/af.c',
            'contrib/tcpdump/bpf_dump.c',
            'contrib/tcpdump/checksum.c',
            'contrib/tcpdump/cpack.c',
            'contrib/tcpdump/gmpls.c',
            'contrib/tcpdump/gmt2local.c',
            'contrib/tcpdump/in_cksum.c',
            'contrib/tcpdump/ipproto.c',
            'contrib/tcpdump/machdep.c',
            'contrib/tcpdump/nlpid.c',
            'contrib/tcpdump/l2vpn.c',
            'contrib/tcpdump/oui.c',
            'contrib/tcpdump/parsenfsfh.c',
            'contrib/tcpdump/print-802_11.c',
            'contrib/tcpdump/print-802_15_4.c',
            'contrib/tcpdump/print-ah.c',
            'contrib/tcpdump/print-aodv.c',
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
            'contrib/tcpdump/print-fr.c',
            'contrib/tcpdump/print-frag6.c',
            'contrib/tcpdump/print-gre.c',
            'contrib/tcpdump/print-hsrp.c',
            'contrib/tcpdump/print-icmp.c',
            'contrib/tcpdump/print-icmp6.c',
            'contrib/tcpdump/print-igmp.c',
            'contrib/tcpdump/print-igrp.c',
            'contrib/tcpdump/print-ip.c',
            'contrib/tcpdump/print-ip6.c',
            'contrib/tcpdump/print-ip6opts.c',
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
            'contrib/tcpdump/print-llc.c',
            'contrib/tcpdump/print-lldp.c',
            'contrib/tcpdump/print-lmp.c',
            'contrib/tcpdump/print-lspping.c',
            'contrib/tcpdump/print-lwapp.c',
            'contrib/tcpdump/print-lwres.c',
            'contrib/tcpdump/print-mobile.c',
            'contrib/tcpdump/print-mobility.c',
            'contrib/tcpdump/print-mpcp.c',
            'contrib/tcpdump/print-mpls.c',
            'contrib/tcpdump/print-msdp.c',
            'contrib/tcpdump/print-msnlb.c',
            'contrib/tcpdump/print-netbios.c',
            'contrib/tcpdump/print-nfs.c',
            'contrib/tcpdump/print-ntp.c',
            'contrib/tcpdump/print-null.c',
            'contrib/tcpdump/print-olsr.c',
            'contrib/tcpdump/print-ospf.c',
            'contrib/tcpdump/print-ospf6.c',
            'contrib/tcpdump/print-otv.c',
            'contrib/tcpdump/print-pflog.c',
            'contrib/tcpdump/print-pfsync.c',
            'contrib/tcpdump/print-pgm.c',
            'contrib/tcpdump/print-pim.c',
            'contrib/tcpdump/print-ppi.c',
            'contrib/tcpdump/print-ppp.c',
            'contrib/tcpdump/print-pppoe.c',
            'contrib/tcpdump/print-pptp.c',
            'contrib/tcpdump/print-radius.c',
            'contrib/tcpdump/print-raw.c',
            'contrib/tcpdump/print-rip.c',
            'contrib/tcpdump/print-ripng.c',
            'contrib/tcpdump/print-rpki-rtr.c',
            'contrib/tcpdump/print-rrcp.c',
            'contrib/tcpdump/print-rsvp.c',
            'contrib/tcpdump/print-rt6.c',
            'contrib/tcpdump/print-rx.c',
            'contrib/tcpdump/print-sctp.c',
            'contrib/tcpdump/print-sflow.c',
            'contrib/tcpdump/print-sip.c',
            'contrib/tcpdump/print-sl.c',
            'contrib/tcpdump/print-sll.c',
            'contrib/tcpdump/print-slow.c',
            'contrib/tcpdump/print-smb.c',
            'contrib/tcpdump/print-snmp.c',
            'contrib/tcpdump/print-stp.c',
            'contrib/tcpdump/print-sunatm.c',
            #'contrib/tcpdump/print-sunrpc.c',
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
            'contrib/tcpdump/print-wb.c',
            'contrib/tcpdump/print-zephyr.c',
            'contrib/tcpdump/print-zeromq.c',
            'contrib/tcpdump/setsignal.c',
            'contrib/tcpdump/signature.c',
            'contrib/tcpdump/smbutil.c',
            'contrib/tcpdump/tcpdump.c',
            'contrib/tcpdump/util.c',
        ],
        mm.generator['source'](['-D__FreeBSD__=1',
                                '-DINET6',
                                '-D_U_=__attribute__((unused))',
                                '-DHAVE_CONFIG_H=1',
                                '-DHAVE_NET_PFVAR_H=1'],
                               ['freebsd/contrib/tcpdump',
                                'freebsd/usr.sbin/tcpdump/tcpdump'])
    )
    return mod

#
# in_chksum Module
#
def in_cksum(mm):
    mod = builder.Module('in_cksum')
    mod.addRTEMSHeaderFiles(
        [
        ]
    )
    mod.addCPUDependentHeaderFiles(
        [
            'sys/i386/include/in_cksum.h',
            'sys/mips/include/in_cksum.h',
            'sys/powerpc/include/in_cksum.h',
            'sys/sparc64/include/in_cksum.h',
        ]
    )
    mod.addTargetSourceCPUDependentHeaderFiles(
        [ 'arm', 'avr', 'bfin', 'h8300', 'lm32', 'm32c', 'm32r', 'm68k',
          'nios2', 'sh', 'sparc', 'v850' ],
        'mips',
        [
            'sys/mips/include/in_cksum.h',
        ]
    )
    mod.addCPUDependentSourceFiles(
        [ 'i386' ],
        [
            'sys/i386/i386/in_cksum.c',
        ],
        mm.generator['source']()
    )
    mod.addCPUDependentSourceFiles(
        [ 'powerpc' ],
        [
            'sys/powerpc/powerpc/in_cksum.c',
        ],
        mm.generator['source']()
    )
    mod.addCPUDependentSourceFiles(
        [ 'sparc64' ],
        [
            'sys/sparc64/sparc64/in_cksum.c',
        ],
        mm.generator['source']()
    )
    mod.addCPUDependentSourceFiles(
        [
            'arm', 'avr', 'bfin', 'h8300', 'lm32', 'm32c', 'm32r', 'm68k',
            'mips', 'nios2', 'sh', 'sparc', 'v850'
        ],
        [
            'sys/mips/mips/in_cksum.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# Tests
#
def tests(mm):
    mod = builder.Module('tests')
    mod.addTest(mm.generator['test']('nfs01', ['test_main'], netTest = True))
    mod.addTest(mm.generator['test']('foobarclient', ['test_main'],
                                     runTest = False, netTest = True))
    mod.addTest(mm.generator['test']('foobarserver', ['test_main'],
                                     runTest = False, netTest = True))
    mod.addTest(mm.generator['test']('dhcpcd01', ['test_main'],
                                     runTest = False, netTest = True))
    mod.addTest(mm.generator['test']('dhcpcd02', ['test_main'],
                                     runTest = False, netTest = True))
    mod.addTest(mm.generator['test']('arphole', ['test_main'],
                                     runTest = False, netTest = True))
    mod.addTest(mm.generator['test']('telnetd01', ['test_main'],
                                     runTest = False, netTest = True))
    mod.addTest(mm.generator['test']('unix01', ['test_main']))
    mod.addTest(mm.generator['test']('ftpd01', ['test_main'], netTest = True))
    mod.addTest(mm.generator['test']('ftpd02', ['test_main']))
    mod.addTest(mm.generator['test']('ping01', ['test_main'], netTest = True))
    mod.addTest(mm.generator['test']('selectpollkqueue01', ['test_main']))
    mod.addTest(mm.generator['test']('rwlock01', ['test_main']))
    mod.addTest(mm.generator['test']('sleep01', ['test_main']))
    mod.addTest(mm.generator['test']('syscalls01', ['test_main']))
    mod.addTest(mm.generator['test']('program01', ['test_main']))
    mod.addTest(mm.generator['test']('commands01', ['test_main']))
    mod.addTest(mm.generator['test']('usb01', ['init'], False))
    mod.addTest(mm.generator['test']('loopback01', ['test_main']))
    mod.addTest(mm.generator['test']('netshell01', ['test_main', 'shellconfig'], False))
    mod.addTest(mm.generator['test']('swi01', ['init', 'swi_test']))
    mod.addTest(mm.generator['test']('timeout01', ['init', 'timeout_test']))
    mod.addTest(mm.generator['test']('init01', ['test_main']))
    mod.addTest(mm.generator['test']('thread01', ['test_main']))
    mod.addTest(mm.generator['test']('mutex01', ['test_main']))
    mod.addTest(mm.generator['test']('condvar01', ['test_main']))
    mod.addTest(mm.generator['test']('ppp01', ['test_main'], runTest = False))
    mod.addTest(mm.generator['test']('zerocopy01', ['test_main'],
                                     runTest = False, netTest = True))
    mod.addTest(mm.generator['test']('smp01', ['test_main']))
    mod.addTest(mm.generator['test']('media01', ['test_main'], runTest = False))
    mod.addTest(mm.generator['test']('vlan01', ['test_main'], netTest = True))
    mod.addTest(mm.generator['test']('lagg01', ['test_main'], netTest = True))
    mod.addTest(mm.generator['test']('log01', ['test_main']))
    mod.addTest(mm.generator['test']('rcconf01', ['test_main']))
    mod.addTest(mm.generator['test']('rcconf02', ['test_main']))
    mod.addTest(mm.generator['test']('cdev01', ['test_main', 'test_cdev']))
    mod.addTest(mm.generator['test']('pf01', ['test_main']))
    mod.addTest(mm.generator['test']('pf02', ['test_main'], runTest = False))
    mod.addTest(mm.generator['test-if-header']('debugger01', 'rtems/rtems-debugger.h',
                                               ['test_main'], runTest = False, netTest = True))
    return mod

#
# DHCP
#
def dhcpcd(mm):
    mod = builder.Module('dhcpcd')
    mod.addSourceFiles(
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
            'dhcpcd/compat/pselect.c',
            'dhcpcd/crypt/hmac_md5.c',
        ],
        mm.generator['source']('-D__FreeBSD__ -DTHERE_IS_NO_FORK -DMASTER_ONLY -DINET -DINET6')
    )
    mod.addRTEMSSourceFiles(
        [
            'rtems/rtems-bsd-shell-dhcpcd.c',
        ],
        mm.generator['source']()
    )
    return mod

#
# MDNS Responder
#
def mdnsresponder(mm):
    mod = builder.Module('mdnsresponder')
    mod.addSourceFiles(
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
        ],
        mm.generator['source']()
    )
    return mod

#
# Mongoose HTTP
#
def mghttpd(mm):
    mod = builder.Module('mghttpd')
    mod.addSourceFiles(
        [
            'rtemsbsd/mghttpd/mongoose.c',
        ],
        mm.generator['source']('-DNO_SSL -DNO_POPEN -DNO_CGI -DUSE_WEBSOCKET')
    )
    return mod

def sources(mm):
    mm.addModule(rtems(mm))
    mm.addModule(base(mm))

    mm.addModule(mmc(mm))

    mm.addModule(dev_usb(mm))
    #mm.addModule(dev_usb_add_on(mm))
    mm.addModule(dev_usb_controller(mm))
    #mm.addModule(dev_usb_controller_add_on(mm))
    mm.addModule(dev_usb_quirk(mm))
    #mm.addModule(dev_usb_misc(mm))

    #mm.addModule(dev_usb_bluetooth(mm))
    #mm.addModule(dev_usb_input(mm))
    #mm.addModule(dev_usb_mouse(mm))
    #mm.addModule(dev_usb_serial(mm))
    mm.addModule(dev_usb_net(mm))
    mm.addModule(dev_usb_wlan(mm))

    mm.addModule(cam(mm))
    mm.addModule(dev_usb_storage(mm))
    #mm.addModule(dev_usb_storage_add_on(mm))

    #mm.addModule(dev_usb_template(mm))

    mm.addModule(net(mm))
    mm.addModule(netinet(mm))
    mm.addModule(netinet6(mm))
    #mm.addModule(netipsec(mm))
    mm.addModule(net80211(mm))
    mm.addModule(opencrypto(mm))
    mm.addModule(crypto(mm))
    mm.addModule(altq(mm))
    mm.addModule(pf(mm))
    mm.addModule(ipfw(mm))
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
    # TBD Requires ISA and PCCard Support to be pulled in.
    # mm.addModule(dev_nic_cs(mm))

    # Add in_chksum
    mm.addModule(in_cksum(mm))

    mm.addModule(user_space(mm))
    mm.addModule(contrib_expat(mm))
    mm.addModule(contrib_libpcap(mm))
    mm.addModule(usr_sbin_tcpdump(mm))

    mm.addModule(tests(mm))
    mm.addModule(dhcpcd(mm))
    mm.addModule(mghttpd(mm))
    mm.addModule(mdnsresponder(mm))

    # XXX TODO Check that no file is also listed in empty
    # XXX TODO Check that no file in in two modules
