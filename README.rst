RTEMS LibBSD
************

Welcome to building LibBSD for RTEMS using Waf. This package is a library
containing various parts of the FreeBSD kernel ported to RTEMS. The library
replaces the networking port of FreeBSD in the RTEMS kernel sources. This
package is designed to be updated from the FreeBSD kernel sources and contains
more than just the networking code.

To build this package you need a current RTEMS tool set for your architecture,
and a recent RTEMS kernel for your BSP installed. If you already have this, you
can skip to step 5 of the build procedure.

Building and Installing LibBSD
==============================

The following instructions show you how to build and install the RTEMS Tool
Suite for the ``arm`` target, the RTEMS kernel using the
``arm/xilinx_zynq_a9_qemu`` Board Support Package (BSP), and the LibBSD for this
BSP.

The Waf build support for RTEMS requires you provide your BSP name as an
architecture and BSP pair. You must provide both or Waf will generate an error
message during the configure phase.

We will build an Xilinx Zynq Qemu BSP using the name
``arm/xilinx_zynq_a9_qemu``.  You can copy and paste the shell commands below to
do this.  The individual steps are explained afterwards.

.. code-block:: none

    sandbox="$PWD/sandbox"
    mkdir sandbox
    cd "$sandbox"
    git clone git://git.rtems.org/rtems-source-builder.git
    git clone git://git.rtems.org/rtems.git
    git clone git://git.rtems.org/rtems-libbsd.git
    cd "$sandbox"
    cd rtems-source-builder/rtems
    ../source-builder/sb-set-builder --prefix="$sandbox/rtems/6" 6/rtems-arm
    cd "$sandbox"
    cd rtems
    echo -e "[arm/xilinx_zynq_a9_qemu]\nRTEMS_POSIX_API = True" > config.ini
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

1. Create a sandbox directory:

   .. code-block:: none

       $ sandbox="$PWD/sandbox"
       $ mkdir sandbox

2. Clone the repositories:

   .. code-block:: none

       $ cd "$sandbox"
       $ git clone git://git.rtems.org/rtems-source-builder.git
       $ git clone git://git.rtems.org/rtems.git
       $ git clone git://git.rtems.org/rtems-libbsd.git

3. Build and install the tools:

   .. code-block:: none

       $ cd "$sandbox"
       $ cd rtems-source-builder/rtems
       $ ../source-builder/sb-set-builder --prefix="$sandbox/rtems/6" 6/rtems-arm

4. Build and install the RTEMS Board Support Packages (BSP) you want to use:

   .. code-block:: none

       $ cd "$sandbox"
       $ cd rtems
       $ echo -e "[arm/xilinx_zynq_a9_qemu]\nRTEMS_POSIX_API = True" > config.ini
       $ ./waf configure --prefix "$sandbox/rtems/6"
       $ ./waf
       $ ./waf install

5. Populate the ``rtems_waf`` git submodule.  Note, make sure you specify
   ``rtems_waf`` or the FreeBSD kernel source will be cloned:

   .. code-block:: none

       $ cd "$sandbox"
       $ cd rtems-libbsd
       $ git submodule init
       $ git submodule update rtems_waf

6. Run Waf's configure with your specific settings. In this case the path to
   the tools and RTEMS are provided on the command line and so do not need to
   be in your path or environment, see comment below.  You can use
   ``--rtems-archs=arm,sparc,i386`` or
   ``--rtems-bsps=arm/xilinx_zynq_a9_qemu,sparc/sis,i386/pc586`` to build for
   more than BSP at a time.  Note, you must provide the architecture and BSP as
   a pair. Providing just the BSP name will fail. This call also explicitly
   provides a buildset via the ``--buildset=buildset/default.ini`` option. If no
   buildset is provided the default one (which is the same as the one provided
   explicitly here) will be used. You can also provide multiple buildsets as a
   coma separated list or via multiple ``--buildset=x`` options.

   .. code-block:: none

       $ cd "$sandbox"
       $ cd rtems-libbsd
       $ ./waf configure --prefix="$sandbox/rtems/6" \
           --rtems-bsps=arm/xilinx_zynq_a9_qemu \
           --buildset=buildset/default.ini

7. Build and install.  The LibBSD package will be installed into the prefix
   provided to configure:

   .. code-block:: none

       $ cd "$sandbox"
       $ cd rtems-libbsd
       $ ./waf
       $ ./waf install

9. Run the tests:

   .. code-block:: none

       $ cd "$sandbox"
       $ cd rtems-libbsd
       $ ../rtems/6/bin/rtems-test --rtems-bsp=xilinx_zynq_a9_qemu build

It is good practice to keep your environment as empty as possible. Setting
paths to tools or specific values to configure or control a build is dangerous
because settings can leak between different builds and change what you expect a
build to do. The Waf tool used here lets you specify on the command line the
tools and RTEMS paths and this is embedded in Waf's configuration information.
If you have a few source trees working at any one time with different tool sets
or configurations you can easly move between them safe in the knowledge that
one build will not infect another.

Buildsets
=========

Note that the LibBSD supports different buildsets.  These can be selected with
the ``--buildset=some.ini`` option during the configure phase.  Take a look at
the comments in ``buildset/*.ini`` to see which build sets are officially
supported.

You can also create and provide your own buildset configuration. But remember
that it's quite easy to break something by disabling the wrong modules.  Only
the configurations in the ``buildset`` directory are officially maintained.

Initialization
==============

To initialise the LibBSD create a suitable ``rc.conf`` file. The FreeBSD man
page `RC.CONF(5) <https://www.freebsd.org/cgi/man.cgi?rc.conf>`_ provides the
details needed to create a suitable format file

You can call one of three functions to run the initialisation once LibBSD has
initialised:

* ``rtems_bsd_run_etc_rc_conf()``: Run ``/etc/rc.conf``.
* ``rtems_bsd_run_rc_conf()``: Run a user supplied file.
* ``rtems_bsd_run_rc_conf_script()``: Run the in memory line feed separated text string.

For exapmle:

.. code-block:: c

    void
    network_init(void)
    {
            rtems_status_code sc;

            sc = rtems_bsd_initialize();
            assert(sc == RTEMS_SUCCESSFUL);

            rtems_bsd_run_etc_rc_conf(true); /* verbose = true */
    }

By default the networking support is builtin. Other directives can be added and
are found in ``machine/rtems-bsd-rc-conf-directives.h``. Please check the file
for the list.

The following network names are supported:

.. code-block:: none

    cloned_interfaces
    ifconfig_'interface'
    defaultrouter
    hostname

For example:

.. code-block:: none

    #
    # My BSD initialisation.
    #
    hostname="myhost"
    cloned_interfaces="vlan0 vlan1"
    ifconfig_re0="inet inet 10.10.10.10 netmask 255.255.255.0"
    fconfig_vlan0="inet 10.11.10.10 255.255.255.0 vlan 101 vlandev re0"
    defaultrouter="10.10.10.1"

You can also intialise the LibBSD using code. The following code to
initialize the LibBSD:

.. code-block:: c

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

This performs the basic network stack initialization with a loopback interface.
Further initialization must be done using the standard FreeBSD network
configuration commands
`IFCONFIG(8) <http://www.freebsd.org/cgi/man.cgi?query=ifconfig&sektion=8>`_
using ``rtems_bsd_command_ifconfig()`` and
`ROUTE(8) <http://www.freebsd.org/cgi/man.cgi?query=route&sektion=8>`_
using ``rtems_bsd_command_route()``.  For an example, please have a look at
`default-network-init.h <testsuite/include/rtems/bsd/test/default-network-init.h>`_.

Task Priorities and Stack Size
==============================

The default task priority is 96 for the interrupt server task (name "IRQS"), 98
for the timer server task (name "TIME") and 100 for all other tasks.  The
application may provide their own implementation of the
``rtems_bsd_get_task_priority()`` function if different values are desired (for
example in the translation unit which calls ``rtems_bsd_initialize()``).

The task stack size is determined by the ``rtems_bsd_get_task_stack_size()``
function which may be provided by the application in case the default is not
appropriate.

Size for Allocator Domains
==========================

The size for an allocator domain can be specified via the
``rtems_bsd_get_allocator_domain_size()`` function.  The application may provide
their own implementation of the ``rtems_bsd_get_allocator_domain_size()``
function (for example in the module which calls ``rtems_bsd_initialize()``) if
different values are desired.  The default size is 8MiB for all domains.

Redirecting or Disabling the Output
===================================

A lot of system messages are printed to the ``stdout`` by default. If you want to
redirect them you can overwrite the default print handler. That can even be done
before the libbsd initialization to catch all messages. An example would look
like follows:

.. code-block:: c

    int my_vprintf_handler(int level, const char *fmt, va_list ap) {
            /* Do something with the messages. */

            return number_of_printed_chars;
    }

    ...
            /* In your initialization: */
            rtems_bsd_vprintf_handler old;
            old = rtems_bsd_set_vprintf_handler(my_vprintf_handler);
    ...

As a special case, you can set the ``rtems_bsd_vprintf_handler_mute(...)``
provided by LibBSD to suppress all output.

Branches
========

master
    This branch is intended for the RTEMS master which tracks the FreeBSD
    master branch.  This branch must be used for libbsd development.  Back
    ports to the 6-freebsd-12 are allowed.

6-freebsd-12
    This branch is intended for RTEMS 6 which tracks the FreeBSD stable/12
    branch.  This branch is maintained and regular updates from FreeBSD are
    planned.  It is recommended for production systems.

5-freebsd-12
    This branch belongs to the RTEMS 5 release. It is based on FreeBSD
    stable/12 branch. It is recommended for production systems that use
    RTEMS 5.

5
   This branch belongs to the RTEMS 5 release. It is based on a FreeBSD
   development version.  This branch is unmaintained.  Use 5-freebsd-12 for
   RTEMS 5.

freebsd-9.3
    Is the branch for some RTEMS version with a FreeBSD 9.3 baseline.  This
    branch is unmaintained.  It is recommended to update to RTEMS 5 or 6.

4.11
    Is the branch for the RTEMS 4.11 release series.  This branch is
    unmaintained.  It is recommended to update to RTEMS 5 or 6.

Updating RTEMS Waf Support
==========================

If you have a working libbsd repository and new changes to the ``rtems_waf``
submodule has been made, you will need update. A ``git status`` will indicate
there are new commits with:

.. code-block:: none

    $ git status
        [ snip output ]
              modified:   rtems_waf (new commits)
        [ snip output ]

To update:

.. code-block:: none

    $ git submodule update rtems_waf

Please make sure you use the exact command or you might find you are cloning
the whole of the FreeBSD source tree. If that happens simply git ^C and try
again.

FreeBSD Kernel Options
======================

You can set FreeBSD kernel options during build configuration with the
--freebsd-option=a,b,c,... configuration command option.  This is an advanced
option and should only be used if you are familiar with the internals of the
FreeBSD kernel and what these options do.  Each of the comma separated options
is converted to uppercase and passed as a compiler command line define (-D).

The options are listed in the FreeBSD
`NOTES <https://github.com/freebsd/freebsd/blob/master/sys/conf/NOTES>`_
file.

An example to turn on a verbose kernel boot, verbose sysinit and bus debugging
configure with:

.. code-block:: none

    --freebsd-options=bootverbose,verbose_sysinit,bus_debug,debug_locks,ktr,ktr_verbose

To enable kernel internal consistency checking use:

.. code-block:: none

    --freebsd-options=invariants,invariant_support

The LibBSD Waf support splits the options and converts them to uppercase and
adds them -D options on the compiler command line.  The supported options are:

bootverbose
    Verbose boot of the kernel

verbose_sysinit
    Verbose printing of all the SYSINIT calls

bus_debug
    Bus debugging support

ktr
    Kernel trace

ktr_verbose
    Verbose kernel trace

debug_locks
    FreeBSD locks debugging

invariants
    Invariants build of the kernel

invariant_support
    Support for Invariants (needed with invariants)

rtems_bsd_descrip_trace
    RTEMS BSD descriptor maping trace

rtems_bsd_syscall_trace
    RTEMS BSD system call trace

rtems_bsd_vfs_trace
    RTEMS VFS to libio trace

SMP Requirements
================

In order to support
`EPOCH(9) <https://www.freebsd.org/cgi/man.cgi?query=epoch&apropos=0&sektion=9>`_
a scheduler with thread pinning support is required.  This is the case if you
use the default scheduler configuration.  EPOCH(9) is a central synchronization
mechanism of the network stack.

Configuration for Network Tests
===============================

If you need some other IP configuration for the network tests that use a fixed
IP config you can copy ``config.inc`` to a location outside to the source tree and
adapt it. Then use the option ``--net-test-config=NET_CONFIG`` to pass the file to
Waf's configure command.

.. code-block:: none

    NET_CFG_SELF_IP = 10.0.0.2
    NET_CFG_NETMASK = 255.255.0.0
    NET_CFG_PEER_IP = 10.0.0.1
    NET_CFG_GATEWAY_IP = 10.0.0.1

Qemu and Networking
===================

You can use the Qemu simulator to run a LibBSD based application and connect it
to a virtual network on your host.  You have to create a TAP virtual Ethernet
interface for this:

.. code-block:: none

    sudo tunctl -p -t qtap -u $(whoami)
    sudo ip link set dev qtap up
    sudo ip addr add 169.254.1.1/16 dev qtap

You can show the interface state with the following command:

.. code-block:: none

    $ ip addr show qtap
    27: qtap: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc pfifo_fast state DOWN group default qlen 1000
        link/ether 8e:50:a2:fb:e1:3b brd ff:ff:ff:ff:ff:ff
        inet 169.254.1.1/16 scope global qtap
           valid_lft forever preferred_lft forever

You may have to assign the interface to a firewall zone.

The Qemu command line varies by board support package, here is an example for
the arm/xilinx_zynq_a9_qemu BSP:

.. code-block:: none

    qemu-system-arm -serial null -serial mon:stdio -nographic \
      -M xilinx-zynq-a9 -m 256M \
      -net tap,ifname=qtap,script=no,downscript=no \
      -net nic,model=cadence_gem,macaddr=0e:b0:ba:5e:ba:12 \
      -kernel build/arm-rtems6-xilinx_zynq_a9_qemu-default/media01.exe

After some seconds it will acquire a IPv4 link-local address, e.g.

.. code-block:: none

    info: cgem0: probing for an IPv4LL address
    debug: cgem0: checking for 169.254.159.156

You can connect to the target via telnet for example:

.. code-block:: none

    $ telnet 169.254.159.156
    Trying 169.254.159.156...
    Connected to 169.254.159.156.
    Escape character is '^]'.

    RTEMS Shell on /dev/pty4. Use 'help' to list commands.
    TLNT [/] #
