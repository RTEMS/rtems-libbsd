RTEMS LibBSD
============

Welcome to building LibBSD for RTEMS using Waf. This package is a library
containing various parts of the FreeBSD kernel ported to RTEMS. The library
replaces the networking port of FreeBSD in the RTEMS kernel sources. This
package is designed to be updated from the FreeBSD kernel sources and contains
more than just the networking code.

To build this package you need a current RTEMS tool set for your architecture,
and a recent RTEMS kernel for your BSP configured with networking disabled
built and installed. If you already have this you can skip to step 3 of the
build procedure.

Building and Installing LibBSD
------------------------------

The following instructions show you how to build and install RTEMS Tools and
RTEMS kernel for your BSP in separate paths. Using separate paths for the tools
and BSPs lets you manage what you have installed. If you are happy with a
single path you can use the same path in each stage.

The Waf build support for RTEMS requires you provide your BSP name as an
architecture and BSP pair. You must provide both or Waf will generate an error
message during the configure phase.

We will build an Xilinx Zynq QEMU BSP using the name
*arm/xilinx_zynq_a9_qemu*.  You can copy and paste the shell commands below to
do this.  The individual steps are explained afterwards.

```
sandbox="$PWD/sandbox"
mkdir sandbox
cd "$sandbox"
git clone git://git.rtems.org/rtems-source-builder.git
git clone git://git.rtems.org/rtems.git
git clone git://git.rtems.org/rtems-libbsd.git
cd "$sandbox"
cd rtems-source-builder/rtems
../source-builder/sb-set-builder --prefix="$sandbox/rtems/5" 5/rtems-arm
cd "$sandbox"
cd rtems
PATH="$sandbox/rtems/5/bin:$PATH" ./bootstrap
cd "$sandbox"
mkdir b-xilinx_zynq_a9_qemu
cd b-xilinx_zynq_a9_qemu
PATH="$sandbox/rtems/5/bin:$PATH" "$sandbox/rtems/configure" \
  --target=arm-rtems5 --prefix="$sandbox/rtems/5" \
  --disable-networking --enable-rtemsbsp=xilinx_zynq_a9_qemu
PATH="$sandbox/rtems/5/bin:$PATH" make
PATH="$sandbox/rtems/5/bin:$PATH" make install
cd "$sandbox"
cd rtems-libbsd
git submodule init
git submodule update rtems_waf
./waf configure --prefix="$sandbox/rtems/5" \
  --rtems-bsps=arm/xilinx_zynq_a9_qemu \
  --buildset=buildset/default.ini
./waf
./waf install
qemu-system-arm -no-reboot -serial null -serial mon:stdio -net none \
  -nographic -M xilinx-zynq-a9 -m 256M \
  -kernel build/arm-rtems5-xilinx_zynq_a9_qemu-default/selectpollkqueue01.exe
```

1. Create a sandbox directory:

```
$ sandbox="$PWD/sandbox"
$ mkdir sandbox
```

2. Clone the repositories:

```
$ cd "$sandbox"
$ git clone git://git.rtems.org/rtems-source-builder.git
$ git clone git://git.rtems.org/rtems.git
$ git clone git://git.rtems.org/rtems-libbsd.git
```

3. Build and install the tools:

```
$ cd "$sandbox"
$ cd rtems-source-builder/rtems
$ ../source-builder/sb-set-builder --prefix="$sandbox/rtems/5" 5/rtems-arm
```

4. Bootstrap the RTEMS sources:

```
$ cd "$sandbox"
$ cd rtems
$ PATH="$sandbox/rtems/5/bin:$PATH" ./bootstrap
```

5. Build and install the RTEMS Board Support Packages (BSP) you want to use:

```
$ cd "$sandbox"
$ mkdir b-xilinx_zynq_a9_qemu
$ cd b-xilinx_zynq_a9_qemu
$ PATH="$sandbox/rtems/5/bin:$PATH" "$sandbox/rtems/configure" \
    --target=arm-rtems5 --prefix="$sandbox/rtems/5" \
    --disable-networking --enable-rtemsbsp=xilinx_zynq_a9_qemu
$ PATH="$sandbox/rtems/5/bin:$PATH" make
$ PATH="$sandbox/rtems/5/bin:$PATH" make install
```

6. Populate the rtems_waf git submodule.  Note, make sure you specify
   'rtems_waf' or the FreeBSD kernel source will be cloned:

```
$ cd "$sandbox"
$ cd rtems-libbsd
$ git submodule init
$ git submodule update rtems_waf
```

7. Run Waf's configure with your specific settings. In this case the path to
   the tools and RTEMS are provided on the command line and so do not need to
   be in your path or environment [1].  You can use
   '--rtems-archs=arm,sparc,i386' or
   '--rtems-bsps=arm/xilinx_zynq_a9_qemu,sparc/sis,i386/pc586' to build for
   more than BSP at a time.  Note, you must provide the architecture and BSP as
   a pair. Providing just the BSP name will fail. This call also explicitly
   provides a buildset via the '--buildset=buildset/default.ini' option. If no
   buildset is provided the default one (which is the same as the one provided
   explicitly here) will be used. You can also provide multiple buildsets as a
   coma separated list or via multiple '--buildset=x' options.

```
$ cd "$sandbox"
$ cd rtems-libbsd
$ ./waf configure --prefix="$sandbox/rtems/5" \
    --rtems-bsps=arm/xilinx_zynq_a9_qemu \
    --buildset=buildset/default.ini
```

8. Build and install.  The LibBSD package will be installed into the prefix
   provided to configure:

```
$ cd "$sandbox"
$ cd rtems-libbsd
$ ./waf
$ ./waf install
```

9. Run the tests on QEMU, for example:

```
$ qemu-system-arm -no-reboot -serial null -serial mon:stdio -net none \
$   -nographic -M xilinx-zynq-a9 -m 256M \
$   -kernel build/arm-rtems5-xilinx_zynq_a9_qemu-default/selectpollkqueue01.exe
```

[1] It is good practice to keep your environment as empty as possible. Setting
    paths to tools or specific values to configure or control a build is
    dangerous because settings can leak between different builds and change
    what you expect a build to do. The Waf tool used here lets you specify on
    the command line the tools and RTEMS paths and this is embedded in Waf's
    configuration information. If you have a few source trees working at any
    one time with different tool sets or configurations you can easly move
    between them safe in the knowledge that one build will not infect another.

Branches
--------

* master - branch intended for the RTEMS master which tracks the FreeBSD master
  branch.  This branch must be used for libbsd development.  Back ports to the
  5-freebsd-12 are allowed.

* 5-freebsd-12 - branch intended for RTEMS 5 which tracks the FreeBSD stable/12
  branch.  This branch is maintained and regular updates from FreeBSD are
  planned.  It is recommended for production systems.

* freebsd-9.3 - branch for some RTEMS version with a FreeBSD 9.3 baseline.
  This branch is unmaintained.  It is recommended to update to RTEMS 5.

* 4.11 - branch for the RTEMS 4.11 release series.  This branch is
  unmaintained.  It is recommended to update to RTEMS 5.

Updating RTEMS Waf Support
--------------------------

If you have a working libbsd repository and new changes to the `rtems_waf`
submodule has been made, you will need update. A `git status` will indicate
there are new commits with:

```
$ git status
    [ snip output ]
          modified:   rtems_waf (new commits)
    [ snip output ]
```

To update:

```
$ git submodule update rtems_waf
```

Please make sure you use the exact command or you might find you are cloning
the whole of the FreeBSD source tree. If that happens simply git ^C and try
again.

FreeBSD Kernel Options
----------------------

You can set FreeBSD kernel options during build configuration with the
--freebsd-option=a,b,c,... configuration command option.  This is an advanced
option and should only be used if you are familiar with the internals of the
FreeBSD kernel and what these options do.  Each of the comma separated options
is converted to uppercase and passed as a compiler command line define (-D).

The options are listed in:

https://github.com/freebsd/freebsd/blob/master/sys/conf/NOTES

An example to turn on a verbose kernel boot, verbose sysinit and bus debugging
configure with:

```
--freebsd-options=bootverbose,verbose_sysinit,bus_debug
```

To enable kernel internal consistency checking use:

```
--freebsd-options=invariants,invariant_support
```

Qemu and Networking
-------------------

You can use the Qemu simulator to run a LibBSD based application and connect it
to a virtual network on your host.  You have to create a TAP virtual Ethernet
interface for this:

```
sudo tunctl -p -t qtap -u $(whoami)
sudo ip link set dev qtap up
sudo ip addr add 169.254.1.1/16 dev qtap
```

You can show the interface state with the following command:

```
$ ip addr show qtap
27: qtap: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc pfifo_fast state DOWN group default qlen 1000
    link/ether 8e:50:a2:fb:e1:3b brd ff:ff:ff:ff:ff:ff
    inet 169.254.1.1/16 scope global qtap
       valid_lft forever preferred_lft forever
```

You may have to assign the interface to a firewall zone.

The Qemu command line varies by board support package, here is an example for
the arm/xilinx_zynq_a9_qemu BSP:

```
qemu-system-arm -serial null -serial mon:stdio -nographic \
  -M xilinx-zynq-a9 -m 256M \
  -net tap,ifname=qtap,script=no,downscript=no \
  -net nic,model=cadence_gem,macaddr=0e:b0:ba:5e:ba:12 \
  -kernel build/arm-rtems5-xilinx_zynq_a9_qemu-default/media01.exe
```

After some seconds it will acquire a IPv4 link-local address, e.g.

```
info: cgem0: probing for an IPv4LL address
debug: cgem0: checking for 169.254.159.156
```

You can connect to the target via telnet for example:

```
$ telnet 169.254.159.156
Trying 169.254.159.156...
Connected to 169.254.159.156.
Escape character is '^]'.

RTEMS Shell on /dev/pty4. Use 'help' to list commands.
TLNT [/] #
```
