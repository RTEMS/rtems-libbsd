# RTEMS LibBSD

Welcome to building LibBSD for RTEMS using Waf. This package is a library
containing various parts of the FreeBSD kernel ported to RTEMS. The library
replaces the networking port of FreeBSD in the RTEMS kernel sources. This
package is designed to be updated from the FreeBSD kernel sources and contains
more than just the networking code.

To build this package you need a current RTEMS tool set for your architecture,
and a recent RTEMS kernel for your BSP installed. If you already have this, you
can skip to step 5 of the build procedure.

## Branches

| Branch            | RTEMS Version     | FreeBSD Version       | Development | Active |
| ----------------- | ----------------- | --------------------- | ----------- | ------ |
| `main`            | `main`            | `CURRENT`             | FreeBSD     | Yes    |
| `7-freebsd-14`    | `main`            | `stable/14` `9679df8` | Yes         | Yes    |
| `6-freebsd-14`    | `6`               | `stable/14` `9679df8` | No          | Yes    |
| `6-freebsd-12`    | `6`               | `stable/12` `c6c89ab` | No          | No     |
| `6-freebsd-13`    | `6`               | `5d85e12`             | No          | No     |
| `5-freebsd-12`    | `5`               | `stable/12` `0d1c391` | No          | No     |
| `5`               | `5`               | `6b0307a`             | No          | No     |
| `freebsd-9.3`     | `4.12`            | `99a648a9`            | No          | No     |
| `4.11`            | `4.11`            | `99a648a9`            | No          | No     |

### Update process

`main` and the development branch will get FreeBSD updates. `main` will follow
`CURRENT` on FreeBSD. The development branch will get updated in line with the
latest minor release of the FreeBSD version to ensure stability. When a major
release of FreeBSD occurs two new branches will be created. One with the
development version of RTEMS and the major FreeBSD release and one with the
latest release of RTEMS and the major release of the FreeBSD. The branch with
the development version of RTEMS will become the development branch.

Updates are completed by using `git merge-file`. This takes a common ancestor
file and merges two divergent files. We use the FreeBSD commit that the branch
is based off (`freebsd-org`) as the common ancestor and then the files in
`freebsd` and a separate clone of the FreeBSD repo at the commit we want to
migrate to as the two divergent versions. Details can be found in
`CONTRIBUTING.md`

## Building and Installing LibBSD

The following instructions show you how to build and install the RTEMS Tool
Suite for the `arm` target, the RTEMS kernel using the
`arm/xilinx_zynq_a9_qemu` Board Support Package (BSP), and the LibBSD for this
BSP.

The Waf build support for RTEMS requires you provide your BSP name as an
architecture and BSP pair. You must provide both or Waf will generate an error
message during the configure phase.

We will build an Xilinx Zynq Qemu BSP using the name
`arm/xilinx_zynq_a9_qemu`.  You can copy and paste the shell commands below to
do this.  The individual steps are explained afterwards.

```shell
  sandbox="$PWD/sandbox"
  mkdir sandbox
  cd "$sandbox"
  git clone https://gitlab.rtems.org/rtems/tools/rtems-source-builder.git
  git clone https://gitlab.rtems.org/rtems/rtos/rtems.git
  git clone https://gitlab.rtems.org/rtems/pkg/rtems-libbsd.git
  cd "$sandbox"
  cd rtems-source-builder/rtems
  ../source-builder/sb-set-builder --prefix="$sandbox/rtems/6" 6/rtems-arm
  cd "$sandbox"
  cd rtems
  echo -e "[arm/xilinx_zynq_a9_qemu]" > config.ini
  ./waf configure --prefix "$sandbox/rtems/6"
  ./waf
  ./waf install
  cd "$sandbox"
  cd rtems-libbsd
  git submodule init
  git submodule update rtems_waf
  ./waf configure --prefix="$sandbox/rtems/6" \
    --rtems-bsps=arm/xilinx_zynq_a9_qemu \
    --buildset=buildset/default.ini
  ./waf
  ./waf install
  ../rtems/6/bin/rtems-test --rtems-bsp=xilinx_zynq_a9_qemu build
```

1. Create a sandbox directory.

   ```shell
   $ sandbox="$PWD/sandbox"
   $ mkdir sandbox
   ```

2. Clone the repositories:

   ```shell
   $ cd "$sandbox"
   $ git clone https://gitlab.rtems.org/rtems/tools/rtems-source-builder.git
   $ git clone https://gitlab.rtems.org/rtems/rtos/rtems.git
   $ git clone https://gitlab.rtems.org/rtems/pkg/rtems-libbsd.git
   ```

3. Build and install the tools:

   ```shell
   $ cd "$sandbox"
   $ cd rtems-source-builder/rtems
   $ ../source-builder/sb-set-builder --prefix="$sandbox/rtems/6" 6/rtems-arm
   ```

4. Build and install the RTEMS Board Support Packages (BSP) you want to use:

   ```shell
   $ cd "$sandbox"
   $ cd rtems
   $ echo -e "[arm/xilinx_zynq_a9_qemu]" > config.ini
   $ ./waf configure --prefix "$sandbox/rtems/6"
   $ ./waf
   $ ./waf install
   ```

5. Populate the `rtems_waf` git submodule.  Note, make sure you specify
   `rtems_waf` or the FreeBSD kernel source will be cloned:

   ```shell
   $ cd "$sandbox"
   $ cd rtems-libbsd
   $ git submodule init
   $ git submodule update rtems_waf
   ```

6. Run Waf's configure with your specific settings. In this case the path to
   the tools and RTEMS are provided on the command line and so do not need to
   be in your path or environment, see comment below.  You can use
   `--rtems-archs=arm,sparc,i386` or
   `--rtems-bsps=arm/xilinx_zynq_a9_qemu,sparc/sis,i386/pc586` to build for
   more than BSP at a time.  Note, you must provide the architecture and BSP as
   a pair. Providing just the BSP name will fail. This call also explicitly
   provides a buildset via the `--buildset=buildset/default.ini` option. If no
   buildset is provided the default one (which is the same as the one provided
   explicitly here) will be used. You can also provide multiple buildsets as a
   coma separated list or via multiple `--buildset=x` options.

   ```shell
   $ cd "$sandbox"
   $ cd rtems-libbsd
   $ ./waf configure --prefix="$sandbox/rtems/6" \
       --rtems-bsps=arm/xilinx_zynq_a9_qemu \
       --buildset=buildset/default.ini
   ```

7. Build and install.  The LibBSD package will be installed into the prefix
   provided to configure:

   ```shell
   $ cd "$sandbox"
   $ cd rtems-libbsd
   $ ./waf
   $ ./waf install
   ```

9) Run the tests:

   ```shell
   $ cd "$sandbox"
   $ cd rtems-libbsd
   $ ../rtems/6/bin/rtems-test --rtems-bsp=xilinx_zynq_a9_qemu build
   ```

It is good practice to keep your environment as empty as possible. Setting
paths to tools or specific values to configure or control a build is dangerous
because settings can leak between different builds and change what you expect a
build to do. The Waf tool used here lets you specify on the command line the
tools and RTEMS paths and this is embedded in Waf's configuration information.
If you have a few source trees working at any one time with different tool sets
or configurations you can easly move between them safe in the knowledge that
one build will not infect another.

## Buildsets

Note that the LibBSD supports different buildsets.  These can be selected with
the `--buildset=some.ini` option during the configure phase.  Take a look at
the comments in `buildset/*.ini` to see which build sets are officially
supported.

You can also create and provide your own buildset configuration. But remember
that it's quite easy to break something by disabling the wrong modules.  Only
the configurations in the `buildset` directory are officially maintained.

## Initialization

To initialise the LibBSD create a suitable `rc.conf` file. The FreeBSD man
page [RC.CONF(5)](https://www.freebsd.org/cgi/man.cgi?rc.conf) provides the
details needed to create a suitable format file

You can call one of three functions to run the initialisation once LibBSD has
initialised:

- `rtems_bsd_run_etc_rc_conf()`: Run `/etc/rc.conf`.
- `rtems_bsd_run_rc_conf()`: Run a user supplied file.
- `rtems_bsd_run_rc_conf_script()`: Run the in memory line feed separated text string.

For exapmle:

```c
void
network_init(void)
{
        rtems_status_code sc;

        sc = rtems_bsd_initialize();
        assert(sc == RTEMS_SUCCESSFUL);

        rtems_bsd_run_etc_rc_conf(true); /* verbose = true */
}
```

By default the networking support is builtin. Other directives can be added and
are found in `machine/rtems-bsd-rc-conf-directives.h`. Please check the file
for the list.

The following network names are supported:

```
cloned_interfaces
ifconfig_'interface'
defaultrouter
hostname
```

For example:

```
#
# My BSD initialisation.
#
hostname="myhost"
cloned_interfaces="vlan0 vlan1"
ifconfig_re0="inet inet 10.10.10.10 netmask 255.255.255.0"
fconfig_vlan0="inet 10.11.10.10 255.255.255.0 vlan 101 vlandev re0"
defaultrouter="10.10.10.1"
```

You can also intialise the LibBSD using code. The following code to
initialize the LibBSD:

```c
#include <assert.h>
#include <sysexits.h>

#include <rtems/bsd/bsd.h>

void
network_init(void)
{
        rtems_status_code sc;
        int exit_code;

        sc = rtems_bsd_initialize();
        assert(sc == RTEMS_SUCCESSFUL);

        exit_code = rtems_bsd_ifconfig_lo0();
        assert(exit_code == EX_OK);
}
```

This performs the basic network stack initialization with a loopback interface.
Further initialization must be done using the standard FreeBSD network
configuration commands
[IFCONFIG(8)](http://www.freebsd.org/cgi/man.cgi?query=ifconfig&sektion=8)
using `rtems_bsd_command_ifconfig()` and
[ROUTE(8)](http://www.freebsd.org/cgi/man.cgi?query=route&sektion=8)
using `rtems_bsd_command_route()`.  For an example, please have a look at
[default-network-init.h](testsuite/include/rtems/bsd/test/default-network-init.h).

## Task Priorities and Stack Size

The default task priority is 96 for the interrupt server task (name "IRQS"), 98
for the timer server task (name "TIME") and 100 for all other tasks.  The
application may provide their own implementation of the
`rtems_bsd_get_task_priority()` function if different values are desired (for
example in the translation unit which calls `rtems_bsd_initialize()`).

The task stack size is determined by the `rtems_bsd_get_task_stack_size()`
function which may be provided by the application in case the default is not
appropriate.

## Size for Allocator Domains

The size for an allocator domain can be specified via the
`rtems_bsd_get_allocator_domain_size()` function.  The application may provide
their own implementation of the `rtems_bsd_get_allocator_domain_size()`
function (for example in the module which calls `rtems_bsd_initialize()`) if
different values are desired.  The default size is 8MiB for all domains.

## Redirecting or Disabling the Output

A lot of system messages are printed to the `stdout` by default. If you want to
redirect them you can overwrite the default print handler. That can even be done
before the libbsd initialization to catch all messages. An example would look
like follows:

```c
int my_vprintf_handler(int level, const char *fmt, va_list ap) {
        /* Do something with the messages. */

        return number_of_printed_chars;
}

...
        /* In your initialization: */
        rtems_bsd_vprintf_handler old;
        old = rtems_bsd_set_vprintf_handler(my_vprintf_handler);
...
```

As a special case, you can set the `rtems_bsd_vprintf_handler_mute(...)`
provided by LibBSD to suppress all output.

## Features

The following features are available in LibBSD.  Some features need device
driver support for a particular target platform.

- [BPF(4)](http://www.freebsd.org/cgi/man.cgi?query=bpf&sektion=4): Berkeley Packet Filter
- [DHCPCD(8)](http://roy.marples.name/projects/dhcpcd/index): DHCP client
- [dns_sd.h](mDNSResponder/mDNSShared/dns_sd.h): DNS Service Discovery
- [GETHOSTBYNAME(3)](http://www.freebsd.org/cgi/man.cgi?query=gethostbyname&sektion=3): Get network host entry
- [IF_BRIDGE(4)](http://www.freebsd.org/cgi/man.cgi?query=if_bridge&sektion=4): Network bridge device
- [INET(4)](http://www.freebsd.org/cgi/man.cgi?query=inet&sektion=4): Internet protocol family
- [INET6(4)](http://www.freebsd.org/cgi/man.cgi?query=inet6&sektion=4): Internet protocol version 6 family
- [IPSEC(4)](http://www.freebsd.org/cgi/man.cgi?query=ipsec&sektion=4): Internet Protocol Security protocol
- [KQUEUE(2)](http://www.freebsd.org/cgi/man.cgi?query=kqueue&sektion=2): Kernel event notification mechanism
- [LAGG(4)](http://www.freebsd.org/cgi/man.cgi?query=lagg&sektion=4): Link aggregation and link failover interface
- [mDNSEmbeddedAPI.h](mDNSResponder/mDNSCore/mDNSEmbeddedAPI.h): Multi-Cast DNS
- [MMC(4)](http://www.freebsd.org/cgi/man.cgi?query=mmc&sektion=4): MultiMediaCard and SD Card bus driver
- [NET80211(4)](http://www.freebsd.org/cgi/man.cgi?query=net80211&sektion=4): Standard interface to IEEE 802.11 devices
- [NVME(4)](http://www.freebsd.org/cgi/man.cgi?query=nvme&sektion=4): NVM Express core driver
- [PCI(4)](http://www.freebsd.org/cgi/man.cgi?query=pci&sektion=4): Generic PCI/PCIe bus driver
- [PF(4)](http://www.freebsd.org/cgi/man.cgi?query=pf&sektion=4): Packet filter
- [POLL(2)](http://www.freebsd.org/cgi/man.cgi?query=poll&sektion=2): Synchronous I/O multiplexing
- [RESOLVER(3)](http://www.freebsd.org/cgi/man.cgi?query=resolver&sektion=3): Resolver routines
- [ROUTE(4)](http://www.freebsd.org/cgi/man.cgi?query=route&sektion=4): Kernel packet forwarding database
- [SELECT(2)](http://www.freebsd.org/cgi/man.cgi?query=select&sektion=2): Synchronous I/O multiplexing
- [SOCKET(2)](http://www.freebsd.org/cgi/man.cgi?query=socket&sektion=2): Create an endpoint for communication
- [SSL(7)](http://www.freebsd.org/cgi/man.cgi?query=ssl&sektion=7): OpenSSL SSL/TLS library
- [SYSCTL(3)](http://www.freebsd.org/cgi/man.cgi?query=sysctl&sektion=3): Get or set system information
- [TCP(4)](http://www.freebsd.org/cgi/man.cgi?query=tcp&sektion=4): Internet Transmission Control Protocol
- [UDP(4)](http://www.freebsd.org/cgi/man.cgi?query=udp&sektion=4): Internet User Datagram Protocol
- [UMASS(4)](http://www.freebsd.org/cgi/man.cgi?query=umass&sektion=4): USB Mass Storage Devices driver
- [UNIX(4)](http://www.freebsd.org/cgi/man.cgi?query=unix&sektion=4): UNIX-domain protocol family
- [USB(4)](http://www.freebsd.org/cgi/man.cgi?query=usb&sektion=4): Universal Serial Bus
- [VLAN(4)](http://www.freebsd.org/cgi/man.cgi?query=vlan&sektion=4): IEEE 802.1Q VLAN network interface

## Commands

In LibBSD the following ports of FreeBSD command line tools are available.  You
can invoke the commands in the RTEMS Shell or through function calls, for
example `rtems_bsd_command_ifconfig()`.  The functions declarations are
available through
[#include \<machine/rtems-bsd-commands.h>](rtemsbsd/include/machine/rtems-bsd-commands.h).

- [ARP(8)](http://www.freebsd.org/cgi/man.cgi?query=arp&sektion=8): Address resolution display and control
- [HOSTNAME(1)](http://www.freebsd.org/cgi/man.cgi?query=hostname&sektion=1): Set or print name of current host system
- [IFCONFIG(8)](http://www.freebsd.org/cgi/man.cgi?query=ifconfig&sektion=8): Configure network interface parameters
- [IFMCSTAT(8)](http://www.freebsd.org/cgi/man.cgi?query=ifmcstat&sektion=8): Dump multicast group management statistics per interface
- [NETSTAT(1)](http://www.freebsd.org/cgi/man.cgi?query=netstat&sektion=1): Show network status
- [NVMECONTROL(8)](http://www.freebsd.org/cgi/man.cgi?query=nvmecontrol&sektion=8): NVM Express control utility
- [OPENSSL(1)](http://www.freebsd.org/cgi/man.cgi?query=openssl&sektion=1): OpenSSL command line tool
- [PFCTL(8)](http://www.freebsd.org/cgi/man.cgi?query=pfctl&sektion=8): Control the packet filter (PF) device
- [PING6(8)](http://www.freebsd.org/cgi/man.cgi?query=ping6&sektion=8): Send ICMPv6 ECHO_REQUEST packets to network hosts
- [PING(8)](http://www.freebsd.org/cgi/man.cgi?query=ping&sektion=8): Send ICMP ECHO_REQUEST packets to network hosts
- [ROUTE(8)](http://www.freebsd.org/cgi/man.cgi?query=route&sektion=8): Manually manipulate the routing tables
- [STTY(1)](http://www.freebsd.org/cgi/man.cgi?query=stty&sektion=1): Set the options for a terminal device interface
- [SYSCTL(8)](http://www.freebsd.org/cgi/man.cgi?query=sysctl&sektion=8): Get or set kernel state
- [TCPDUMP(1)](http://www.freebsd.org/cgi/man.cgi?query=tcpdump&sektion=1): Dump traffic on a network
- [VMSTAT(8)](http://www.freebsd.org/cgi/man.cgi?query=vmstat&sektion=8): Report virtual memory statistics
- [WPA_SUPPLICANT(8)](http://www.freebsd.org/cgi/man.cgi?query=wpa_supplicant&sektion=8): WPA/802.11i Supplicant for wireless network devices

Command specific notes are listed below.

HOSTNAME(1)

> In addition to the standard options the RTEMS version of the HOSTNAME(1)
  command supports the -m flag to set/get the multicast hostname of the mDNS
  resolver instance.  See also `rtems_mdns_sethostname()` and
  `rtems_mdns_gethostname()`.

## Packet Filter (PF, Firewall)

It is possible to use PF as a firewall. See the
[FreeBSD Handbook](https://docs.freebsd.org/en/books/handbook/firewalls/#firewalls-pf)
for details on the range of functions and for how to configure the firewall.

### Configuration

The following is necessary to use PF on RTEMS:

- You have to provide a `/etc/pf.os` file. The firewall can use it for passive
  OS fingerprinting. If you don't want to use this feature, the file may contain
  nothing except a line of comment (for example "# empty").

- If some filters use protocol names (like `tcp` or `udp`) you have to provide a
  `/etc/protocols` file.

- If some filters use service names (like `http` or `https`) you have to provide a
  `/etc/services` file.

- Create a rule file (normally `/etc/pf.conf`). See the FreeBSD manual for the
  syntax.

- Load the rule file using the
  [pfctl](http://www.freebsd.org/cgi/man.cgi?query=pfctl&sektion=8)
  command and enable PF. Please note that the pfctl command needs a lot of
  stack. You should use at least RTEMS_MINIMUM_STACK_SIZE + 8192 Bytes of
  stack. An example initialisation can look like follows:

  ```c
  int exit_code;
  char *argv[] = {
          "pfctl",
          "-f",
          "/etc/pf.conf",
          "-e",
          NULL
  };

  exit_code = rtems_bsd_command_pfctl(ARGC(argv), argv);
  assert(exit_code == EXIT_SUCCSESS);
  ```

### Known Restrictions

Currently, PF on RTEMS always uses the configuration for memory restricted
systems (on FreeBSD that means systems with less than 100 MB RAM). This is
fixed in `pfctl_init_options()`.

## Wireless Network (WLAN)

The LibBSD provides a basic support for WLAN. Note that currently this support
is still in an early state. The WLAN support is \_not\_ enabled in the default
buildset. You have to configure LibBSD with the
`--buildset=buildset/everything.ini` to enable that feature.

### Configuration

The following gives a rough overview over the necessary steps to connect to an
encrypted network with an RTL8188EU based WiFi dongle:

- Reference all necessary module for your BSP. For some BSPs this is already
  done in the `nexus-devices.h`:

  ```c
  SYSINIT_MODULE_REFERENCE(wlan_ratectl_none);
  SYSINIT_MODULE_REFERENCE(wlan_sta);
  SYSINIT_MODULE_REFERENCE(wlan_amrr);
  SYSINIT_MODULE_REFERENCE(wlan_wep);
  SYSINIT_MODULE_REFERENCE(wlan_tkip);
  SYSINIT_MODULE_REFERENCE(wlan_ccmp);
  SYSINIT_DRIVER_REFERENCE(rtwn_usb, uhub);
  ```

- Create your wlan device using ifconfig:

  ```shell
  ifconfig wlan0 create wlandev rtwn0 up
  ```

- Start a `wpa_supplicant` instance for that device:

  ```shell
  wpa_supplicant_fork -Dbsd -iwlan0 -c/media/mmcsd-0-0/wpa_supplicant.conf
  ```

Note that the wpa_supplicant will only be active till the device goes down. A
workaround is to just restart it every time it exits.

### Known Restrictions

- The network interface (e.g. wlan0) is currently not automatically created. It
  would be nice, if some service would create it as soon as for example a USB
  device is connected. In FreeBSD the names are assigned via rc.conf with lines
  like `wlans_rtwn0="wlan0"`.
- `wpa_supplicant` hast to be started after the device is created. It has to be
  restarted every time the connection goes down. Instead of this behaviour,
  there should be some service that starts and restarts `wpa_supplicant`
  automatically if a interface is ready. Probably the dhcpcd hooks could be used
  for that.
- The current `wpa_supplicant` implementation is protected with a lock so it can't
  be started more than one time. If multiple interface should be used, all have
  to be handled by that single instance. That makes it hard to add interfaces
  dynamically. `wpa_supplicant` should be reviewed thoroughly whether multiple
  instances could be started in parallel.
- The control interface of `wpa_supplicant` most likely doesn't work. The wpa_cli
  application is not ported.

## Updating RTEMS Waf Support

If you have a working libbsd repository and new changes to the `rtems_waf`
submodule has been made, you will need update. A `git status` will indicate
there are new commits with:

```shell
$ git status
    [ snip output ]
          modified:   rtems_waf (new commits)
    [ snip output ]
```

To update:

```shell
$ git submodule update rtems_waf
```

Please make sure you use the exact command or you might find you are cloning
the whole of the FreeBSD source tree. If that happens simply git ^C and try
again.

## FreeBSD Kernel Options

You can set FreeBSD kernel options during build configuration with the
--freebsd-option=a,b,c,... configuration command option.  This is an advanced
option and should only be used if you are familiar with the internals of the
FreeBSD kernel and what these options do.  Each of the comma separated options
is converted to uppercase and passed as a compiler command line define (-D).

The options are listed in the FreeBSD
[NOTES](https://github.com/freebsd/freebsd/blob/master/sys/conf/NOTES)
file.

An example to turn on a verbose kernel boot, verbose sysinit and bus debugging
configure with:

```shell
--freebsd-options=bootverbose,verbose_sysinit,bus_debug,debug_locks,ktr,ktr_verbose
```

To enable kernel internal consistency checking use:

```shell
--freebsd-options=invariants,invariant_support
```

The LibBSD Waf support splits the options and converts them to uppercase and
adds them -D options on the compiler command line.  The supported options are:

bootverbose

> Verbose boot of the kernel

verbose_sysinit

> Verbose printing of all the SYSINIT calls

bus_debug

> Bus debugging support

ktr

> Kernel trace

ktr_verbose

> Verbose kernel trace

debug_locks

> FreeBSD locks debugging

invariants

> Invariants build of the kernel

invariant_support

> Support for Invariants (needed with invariants)

rtems_bsd_descrip_trace

> RTEMS BSD descriptor maping trace

rtems_bsd_syscall_trace

> RTEMS BSD system call trace

rtems_bsd_vfs_trace

> RTEMS VFS to libio trace

## SMP Requirements

In order to support
[EPOCH(9)](https://www.freebsd.org/cgi/man.cgi?query=epoch&apropos=0&sektion=9)
a scheduler with thread pinning support is required.  This is the case if you
use the default scheduler configuration.  EPOCH(9) is a central synchronization
mechanism of the network stack.

## Configuration for Network Tests

If you need some other IP configuration for the network tests that use a fixed
IP config you can copy `config.inc` to a location outside to the source tree and
adapt it. Then use the option `--net-test-config=NET_CONFIG` to pass the file to
Waf's configure command.

```shell
NET_CFG_SELF_IP = 10.0.0.2
NET_CFG_NETMASK = 255.255.0.0
NET_CFG_PEER_IP = 10.0.0.1
NET_CFG_GATEWAY_IP = 10.0.0.1
```

## Qemu and Networking

You can use the Qemu simulator to run a LibBSD based application and connect it
to a virtual network on your host.

### Networking with TAP Interface

One option for networking with Qemu is using a TAP interface (virtual
Ethernet).  You can create a TAP interface with these commands on Linux:

```shell
sudo ip tuntap add qtap mode tap user $(whoami)
sudo ip link set dev qtap up
sudo ip addr add 169.254.1.1/16 dev qtap
```

You can show the interface state with the following command:

```shell
$ ip addr show qtap
27: qtap: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc pfifo_fast state DOWN group default qlen 1000
    link/ether 8e:50:a2:fb:e1:3b brd ff:ff:ff:ff:ff:ff
    inet 169.254.1.1/16 scope global qtap
       valid_lft forever preferred_lft forever
```

You may have to assign the interface to a firewall zone.

The Qemu command line varies by board support package, here is an example for
the arm/xilinx_zynq_a9_qemu BSP:

```shell
qemu-system-arm -serial null -serial mon:stdio -nographic \
  -M xilinx-zynq-a9 -m 256M \
  -net nic,model=cadence_gem \
  -net tap,ifname=qtap,script=no,downscript=no \
  -kernel build/arm-rtems6-xilinx_zynq_a9_qemu-default/media01.exe
```

Make sure that each Qemu instance uses its own MAC address to avoid an address
conflict (or otherwise use it as a test).  After some seconds it will acquire a
IPv4 link-local address, for example:

```shell
info: cgem0: probing for an IPv4LL address
debug: cgem0: checking for 169.254.159.156
```

You can connect to the target via telnet, for example:

```shell
$ telnet 169.254.159.156
Trying 169.254.159.156...
Connected to 169.254.159.156.
Escape character is '^]'.

RTEMS Shell on /dev/pty4. Use 'help' to list commands.
TLNT [/] #
```

### Virtual Distributed Ethernet (VDE)

You can use a Virtual Distributed Ethernet (VDE) to create a network
environment that does not need to run Qemu as root or needing to drop the tap's
privileges to run Qemu.

VDE creates a software switch with a default of 32 ports which means a single
kernel tap can support 32 Qemu networking sessions.

To use VDE you need to build Qemu with VDE support. The RSB can detect a VDE
plug and enable VDE support in Qemu when building. On FreeBSD install the VDE
support with:

```shell
pkg install -u vde2
```

Build Qemu with the RSB.

To network create a bridge and a tap. The network is 10.10.1.0/24. On FreeBSD
add to your `/etc/rc.conf`:

```shell
cloned_interfaces="bridge0 tap0"
autobridge_interfaces="bridge0"
autobridge_bridge0="re0 tap0"
ifconfig_re0="up"
ifconfig_tap0="up"
ifconfig_bridge0="inet 10.1.1.2 netmask 255.255.255.0"
defaultrouter="10.10.1.1"
```

Start the VDE switch as root:

```shell
sysctl net.link.tap.user_open=1
sysctl net.link.tap.up_on_open=1
vde_switch -d -s /tmp/vde1 -M /tmp/mgmt1 -tap tap0 -m 660 --mgmtmode 660
chmod 660 /dev/tap0
```

You can connect to the VDE switch's management channel using:

```shell
vdeterm /tmp/mgmt1
```

To run Qemu:

```shell
qemu-system-arm -serial null -serial mon:stdio -nographic \
  -M xilinx-zynq-a9 -m 256M \
  -net nic,model=cadence_gem \
  -net vde,id=vde0,sock=/tmp/vde1
  -kernel build/arm-rtems6-xilinx_zynq_a9_qemu-default/rcconf02.exe
```
