Guidelines for Developing and Contributing Code
***********************************************

Introduction
============

This guide aims to help developing and contributing code to the LibBSD.  One
goal of the LibBSD is to stay in synchronization with FreeBSD.  This is only
feasible if certain rules are in place.  Otherwise, managing more than a
thousand imported source files will become too labour intensive eventually.

The LibBSD makes FreeBSD subsystems like TCP/IP, USB, SD/MMC, PCIe, and some
more usable for RTEMS.  It tries to follow the FreeBSD development as close as
possible and therefore is updated to the latest FreeBSD HEAD revision of the
associated FreeBSD branch from time to time.  To find out which version of
FreeBSD is currently used as the base version for LibBSD please take a look at
the ``freebsd-org`` submodule.

This guide captures information on the process of merging code from FreeBSD,
RTEMS specific support files, general guidelines on what modifications to the
FreeBSD source are permitted, and some other topics.  For building the library,
see the `README <README.rst>`_.

Goals of the LibBSD activity are

* provide functionality from FreeBSD to RTEMS,
* ease updating to future FreeBSD versions,
* ease tracking changes in FreeBSD code,
* minimize manual changes in FreeBSD code.

We will work to push our changes upstream to the FreeBSD Project and minimize
changes required at each update point.

What is in the Git Repository
=============================

The LibBSD a self-contained kit with FreeBSD and RTEMS components
pre-merged. The Waf wscript in LibBSD automatically generates the build when
you run waf by reading the modules and module's source, header, defines and
special flags from ``libbsd.py``. This is the same module data used to manage
the FreeBSD source.

Any changes to source in the ``freebsd`` directories will need to be merged
upstream into our master FreeBSD checkout, the ``freebsd-org`` submodule.

The repository contains two FreeBSD source trees.  In the ``freebsd`` directory
are the so called *managed* FreeBSD sources used to build the BSD library.
The FreeBSD source in ``freebsd-org`` is the *master* version.  The
``freebsd-to-rtems.py`` script is used to transfer files between the two trees
using the module defnitions in ``libbsd.py``.  In general terms, if you have
modified managed FreeBSD sources, you will need to run the script in *revert*
or *reverse* mode using the ``-R`` switch.  This will copy the source back to
your local copy of the master FreeBSD source so you can run ``git diff`` against
the upstream FreeBSD source.  If you want to transfer source files from the
master FreeBSD source to the manged FreeBSD sources, then you must run the
script in *forward* mode (the default).

Kernel and User Space
=====================

FreeBSD uses virtual memory to run separate address spaces. The kernel is one
address space and each process the kernel runs is another separate address
space. The FreeBSD build system understands the separation and separately
linked executable for the kernel and user land maintains the separation.

RTEMS is a single address space operating system and that means the kernel and
user space code have to be linked to together and be able to run side by
side. This creates additional complexity when working with the FreeBSD code,
for example the FreeBSD kernel has a ``malloc`` call with a different signature
to the user land ``malloc`` call. The RTEMS LibBSD support code provides
structured ways to manage the separation.

LibBSD manages the integration of kernel and user code by knowing the context
of the source code. This lets the merge process handle specific changes each
type of file needs. The build system also uses this information to control the
include paths a source file sees. The kernel code sees the kernel, CPU
specific and build system generated include paths in that order. User code
sees the user include paths then the kernel, CPU specific and build system
generated include paths in that order. The FreeBSD OS include path
``/usr/include`` has a mix of kernel and user space header files. The kernel
headers let user space code cleanly access structures the kernel exports. If a
user header file has the same name as a kernel header file the user file will
be used in the user code rather than the kernel file. If the user code
includes a kernel header that file will be found and included.

Organization
============

The top level directory contains a few directories and files. The following
are important to understand

* ``freebsd-to-rtems.py`` - script to convert to and free FreeBSD and RTEMS trees,
* ``create-kernel-namespace.sh`` - script to create the kernel namespace header ``<machine/rtems-bsd-kernel-namespace.h>``,
* ``wscript`` - automatically generates the build from libbsd.py,
* ``libbsd.py`` - modules, sources, compile flags, and dependencies
* ``freebsd/`` - from FreeBSD by script,
* ``rtemsbsd/`` - RTEMS specific implementations of FreeBSD kernel support routines,
* ``testsuite/`` - RTEMS specific tests, and
* ``libbsd.txt`` - documentation in Asciidoc.

Moving Code Between Managed and Master FreeBSD Source
=====================================================

The script ``freebsd-to-rtems.py`` is used to copy code from FreeBSD to the
rtems-libbsd tree and to reverse this process. This script attempts to
automate this process as much as possible and performs some transformations
on the FreeBSD code. Its command line arguments are shown below:

.. code-block:: none

    freebsd-to-rtems.py [args]
      -?|-h|--help      print this and exit
      -d|--dry-run      run program but no modifications
      -D|--diff         provide diff of files between trees
      -e|--early-exit   evaluate arguments, print results, and exit
      -m|--makefile     Warning: depreciated and will be removed
      -b|--buildscripts just generate the build scripts
      -S|--stats        Print a statistics report
      -R|--reverse      default FreeBSD -> RTEMS, reverse that
      -r|--rtems        RTEMS Libbsd directory (default: '.')
      -f|--freebsd      FreeBSD SVN directory (default: 'freebsd-org')
      -c|--config       Output the configuration then exit
      -v|--verbose      enable verbose output mode

In its default mode of operation, ``freebsd-to-rtems.py`` is used to copy code
from FreeBSD to the rtems-libbsd tree and perform transformations.

In *reverse mode*, this script undoes those transformations and copies
the source code back to the *master* FreeBSD tree. This allows us to do
'git diff', evaluate changes made by the RTEMS Project, and report changes
back to FreeBSD upstream.

In either mode, the script may be asked to perform a dry-run or be verbose.
Also, in either mode, the script is also smart enough to avoid copying over
files which have not changed. This means that the timestamps of files are
not changed unless the contents change. The script will also report the
number of files which changed. In verbose mode, the script will print
the name of the files which are changed.

To add or update files in the RTEMS FreeBSD tree first run the *reverse mode*
and move the current set of patches FreeBSD. The script may warn you if a file
is not present at the destination for the direction. This can happen as files
not avaliable at the FreeBSD snapshot point have been specially added to the
RTEMS FreeBSD tree. Warnings can also appear if you have changed the list of
files in libbsd.py. The reverse mode will result in the FreeBSD having
uncommitted changes. You can ignore these. Once the reverse process has
finished edit libbsd.py and add any new files then run the forwad mode to bring
those files into the RTEMS FreeBSD tree.

The following is an example forward run with no changes.

.. code-block:: none

    $ ./freebsd-to-rtems.py -v
    Verbose:                     yes (1)
    Dry Run:                     no
    Diff Mode Enabled:           no
    Only Generate Build Scripts: no
    RTEMS Libbsd Directory:      .
    FreeBSD SVN Directory:       freebsd-org
    Direction:                   forward
    Forward from FreeBSD GIT into  .
    0 file(s) were changed:

The script may also be used to generate a diff in either forward or reverse
direction.

You can add more than one verbose option (-v) to the command line and get more
detail and debug level information from the command.

FreeBSD Baseline
================

Use

.. code-block:: none

    $ git log freebsd-org

to figure out the current FreeBSD baseline.

How to Import Code from FreeBSD
===============================

* In case you import files from a special FreeBSD version, then update the list above.
* Run ``git status`` and make sure your working directory is clean.
* Run ``./freebsd-to-rtems.py -R``
* Run ``./freebsd-to-rtems.py``
* Run ``git status`` and make sure your working directory is clean.  If you see modified files, then the ``freebsd-to-rtems.py`` script needs to be fixed first.
* Add the files to import to ``libbsd.py`` and your intended build set (for example ``buildset/default.ini``.
* Run ``./freebsd-to-rtems.py``
* Immediately check in the imported files without the changes to ``libbsd.py`` and the buildsets.  Do not touch the imported files yourself at this point.
* Port the imported files to RTEMS.  See 'Rules for Modifying FreeBSD Source'.
* Add a test to the testsuite if possible.
* Run ``./create-kernel-namespace.sh`` if you imported kernel space headers.  Add only your new defines via ``git add -p rtemsbsd/include/machine/rtems-bsd-kernel-namespace.h``.
* Create one commit from this.

The -S or --stats option generates reports the changes we have made to
FreeBSD. If the code has been reserved into the original FreeBSD tree it will
show nothing has changed. To see what we have change:

.. code-block:: none

    $ cd freebsd-org
    $ git checkout -- .
    $ cd ..
    $ ./freebsd-to-rtems.py -R -S -d

The report lists the files change based on the opacity level. The opacity is a
measure on how much of a file differs from the original FreeBSD source. The
lower the value the more transparent the source file it.

Porting of User-Space Utilities
===============================

The theory behind the described method is to put all BSS and initialized data
objects into a named section. This section then will be saved before the code is
executed and restored after it has finished. This method limits to a single
threaded execution of the application but minimizes the necessary changes to the
original FreeBSD code.

* Import and commit the unchanged source files like described above.
* Add the files to the `<libbsd.py>`_ and build them.
* Check the sources for everything that can be made const. This type of patches
  should go back to the upstream FreeBSD sources.
* Move static variables out of functions if necessary (search for
  "\tstatic"). These patches most likely will not be accepted into FreeBSD.
* Add a rtems_bsd_command_PROGNAME() wrapper function to the source file
  containing the main function (e.g. PROGNAME = pfctl). For an example look at
  ``rtems_bsd_command_pfctl()`` in `pfctl.c <freebsd/sbin/pfctl/pfctl.c>`_.
* You probably have to use getopt_r() instead of getopt(). Have a look at
  `pfctl.c <freebsd/sbin/pfctl/pfctl.c>`_.
* Build the LibBSD without optimization.
* Use the ``userspace-header-gen.py`` to generate some necessary header
  files. It will generate one ``rtems-bsd-PROGNAME-MODULE-data.h`` per object file, one
  ``rtems-bsd-PROGNAME-namespace.h`` and one ``rtems-bsd-PROGNAME-data.h``. To call
  the script, you have to compile the objects and afterwards run the helper
  script with a call similar to this one:
  ``python ./userspace-header-gen.py build/arm-rtems4.12-xilinx_zynq_a9_qemu/freebsd/sbin/pfctl/*.o -p pfctl``
  Replace the name (given via -p option) by the name of the userspace tool. It
  has to match the name that is used in the RTEMS linker set further below.
  ``Note:`` the script ``userspace-header-gen.py`` depends on pyelftools. It can be
  installed using pip:
  ``pip install --user pyelftools``
* If you regenerated files that have already been generated, you may have to
  remove RTEMS-specific names from the namespace. The defaults (linker set names
  and rtems_bsd_program_.*) should already be filtered.
* Put the generated header files into the same folder like the source files.
* At the top of each source file place the following right after the user-space header:

  .. code-block:: c

       #ifdef __rtems__
       #include <machine/rtems-bsd-program.h>
       #include "rtems-bsd-PROGNAME-namespace.h"
       #endif /* __rtems__ */

  The following command may be useful:

  .. code-block:: none

       sed -i 's%#include <machine/rtems-bsd-user-space.h>%#include <machine/rtems-bsd-user-space.h>\n\n#ifdef __rtems__\n#include <machine/rtems-bsd-program.h>\n#include "rtems-bsd-PROGNAME-namespace.h"\n#endif /* __rtems__ */%' *.c

* At the bottom of each source file place the follwing:

  .. code-block:: c

       #ifdef __rtems__
       #include "rtems-bsd-PROGNAME-FILE-data.h"
       #endif /* __rtems__ */

  The following command may be useful:

  .. code-block:: none

       for i in *.c ; do n=$(basename $i .c) ; echo -e "#ifdef __rtems__\n#include \"rtems-bsd-PROGNAME-$n-data.h\"\n#endif /* __rtems__ */" >> $i ; done
* Create one compilable commit.

Rules for Modifying FreeBSD Source
==================================

Do not reformat original FreeBSD code.  Do not perform white space changes even
if you get git commit warnings.  **Check your editor settings so that it does
not perform white space changes automatically**, for example adding a newline
to the end of the file.  White space changes may result in conflicts during
updates, especially changes at the end of a file.

Changes in FreeBSD files must be done using ``__rtems__`` C pre-processor guards.
This makes synchronization with the FreeBSD upstream easier and is very
important.  Patches which do not follow these rules will be rejected.  Only add
lines.  If your patch contains lines starting with a ``-``, then this is wrong.
Subtract code by added ``#ifndef __rtems__``.  For example:

.. code-block:: c

     /* Global variables for the kernel. */

     #ifndef __rtems__
     /* 1.1 */
     extern char kernelname[MAXPATHLEN];
     #endif /* __rtems__ */

     extern int tick;			/* usec per tick (1000000 / hz) */

.. code-block:: c

     #if defined(_KERNEL) || defined(_WANT_FILE)
     #ifdef __rtems__
     #include <rtems/libio_.h>
     #include <sys/fcntl.h>
     #endif /* __rtems__ */
     /*
      * Kernel descriptor table.
      * One entry for each open kernel vnode and socket.
      *
      * Below is the list of locks that protects members in struct file.
      *
      * (f) protected with mtx_lock(mtx_pool_find(fp))
      * (d) cdevpriv_mtx
      * none	not locked
      */

.. code-block:: c

     extern int profprocs;			/* number of process's profiling */
     #ifndef __rtems__
     extern volatile int ticks;
     #else /* __rtems__ */
     #include <rtems/score/watchdogimpl.h>
     #define ticks _Watchdog_Ticks_since_boot
     #endif /* __rtems__ */

     #endif /* _KERNEL */

Add nothing (even blank lines) before or after the ``__rtems__`` guards.  Always
include a ``__rtems__`` in the guards to make searches easy, so use

* ``#ifndef __rtems__``,
* ``#ifdef __rtems__``,
* ``#else /* __rtems__ */``, and
* ``#endif /* __rtems__ */``.

The guards must start at the begin of the line.  Examples for wrong guards:

.. code-block:: c

    static void
    guards_must_start_at_the_begin_of_the_line(int j)
    {

            /* WRONG */
            #ifdef __rtems__
            return (j + 1);
            #else /* __rtems__ */
            return (j + 2);
            #endif /* __rtems__ */
    }

    static void
    missing_rtems_comments_in_the_guards(int j)
    {

    #ifdef __rtems__
            return (j + 3);
    /* WRONG */
    #else
            return (j + 4);
    #endif
    }

The FreeBSD build and configuration system uses option header files, e.g.
``#include "opt_xyz.h"`` in an unmodified FreeBSD file.  This include is
transformed by the import script into ``#include <rtems/bsd/local/opt_xyz.h>``.  Do
not disable option header includes via guards.  Instead, add an empty option
header, e.g. ``touch rtemsbsd/include/rtems/bsd/local/opt_xyz.h``.

.. code-block:: c

    /* WRONG */
    #ifndef __rtems__
    #include <rtems/bsd/local/opt_xyz.h>
    #endif /* __rtems__ */

In general, provide empty header files and do not guard includes.

For new code use
`STYLE(9) <http://www.freebsd.org/cgi/man.cgi?query=style&apropos=0&sektion=9>`_.

Update FreeBSD Baseline
=======================

Perform the following steps to do a FreeBSD baseline update:

* Update ``__FreeBSD_version`` in ``rtemsbsd/include/machine/rtems-bsd-version.h``

* Update the namespace header file.

* Review all code blocks with the ``REVIEW-AFTER-FREEBSD-BASELINE-UPDATE`` tag.

Automatically Generated FreeBSD Files
=====================================

Some source and header files are automatically generated during the FreeBSD
build process.  The ``Makefile.todo`` file performs this manually.  The should be
included in ``freebsd-to-rtems.py`` script some time in the future.  For details,
see also
`KOBJ(9) <http://www.freebsd.org/cgi/man.cgi?query=kobj&sektion=9&apropos=0>`_.

Reference Board Support Package
===============================

The reference BSP for LibBSD development is ``arm/xilinx_zynq_a9_qemu``.  All
patches shall be tested for this BSP.  The BSP runs on the Qemu simulator which
has some benefits for development and test of the LibBSD

* ``NULL`` pointer read and write protection,
* Qemu is a fast simulator,
* Qemu provides support for GDB watchpoints,
* Qemu provides support for virtual Ethernet networks, e.g. TUN and bridge
  devices (you can run multiple test instances on one virtual network).

Board Support Package Requirements
==================================

In FreeBSD, interrupt handler may use mutexes.  In RTEMS, using mutexes from
within interrupt context is not allowed, so the Board Support Package (BSP)
should support the
`Interrupt Manager <https://docs.rtems.org/branches/master/c-user/interrupt/directives.html#rtems-interrupt-server-handler-install>`_
in general.

Network Interface Drivers Hints
===============================

Link Up/Down Events
-------------------

You can notifiy the application space of link up/down events in your network
interface driver via the
``if_link_state_change(LINK_STATE_UP/LINK_STATE_DOWN)`` function.  The
DHCPCD(8) client is a consumer of these events for example.  Make sure that the
interface flag ``IFF_UP`` and the interface driver flag ``IFF_DRV_RUNNING`` is
set in case the link is up, otherwise ``ether_output()`` will return the error
status ``ENETDOWN``.

FreeBSD Kernel Features Ported to LibBSD
========================================

All lock based synchronization primitives are implemented through mutexes using
the priority inheritance protocol.

* `BUS_DMA(9) <http://www.freebsd.org/cgi/man.cgi?query=bus_dma&sektion=9>`_: Bus and Machine Independent DMA Mapping Interface
* `BUS_SPACE(9) <http://www.freebsd.org/cgi/man.cgi?query=bus_space&sektion=9>`_: Bus space manipulation functions
* `CALLOUT(9) <http://www.freebsd.org/cgi/man.cgi?query=callout&sektion=9>`_: Execute a function after a specified length of time
* `CONDVAR(9) <http://www.freebsd.org/cgi/man.cgi?query=condvar&sektion=9>`_: Kernel condition variable
* `DEVICE(9) <http://www.freebsd.org/cgi/man.cgi?query=device&sektion=9>`_: An abstract representation of a device
* `DRIVER(9) <http://www.freebsd.org/cgi/man.cgi?query=driver&sektion=9>`_: Structure describing a device driver
* `EPOCH(9) <http://www.freebsd.org/cgi/man.cgi?query=epoch&sektion=9>`_: Kernel epoch based reclamation
* `MUTEX(9) <http://www.freebsd.org/cgi/man.cgi?query=mutex&sektion=9>`_: Kernel synchronization primitives
* `RMAN(9) <http://www.freebsd.org/cgi/man.cgi?query=rman&sektion=9>`_: Resource management functions
* `RMLOCK(9) <http://www.freebsd.org/cgi/man.cgi?query=rmlock&sektion=9>`_: Kernel reader/writer lock optimized for read-mostly access patterns
* `RWLOCK(9) <http://www.freebsd.org/cgi/man.cgi?query=rwlock&sektion=9>`_: Kernel reader/writer lock
* `SX(9) <http://www.freebsd.org/cgi/man.cgi?query=sx&sektion=9>`_: Kernel shared/exclusive lock
* `SYSCTL(9) <http://www.freebsd.org/cgi/man.cgi?query=SYSCTL_DECL&sektion=9>`_: Dynamic and static sysctl MIB creation functions
* `SYSINIT(9) <http://www.freebsd.org/cgi/man.cgi?query=sysinit&sektion=9>`_: A framework for dynamic kernel initialization
* `TASKQUEUE(9) <http://www.freebsd.org/cgi/man.cgi?query=taskqueue&sektion=9>`_: Asynchronous task execution
* `UMA(9) <http://www.freebsd.org/cgi/man.cgi?query=uma&sektion=9>`_: General-purpose kernel object allocator

LibBSD Initialization Details
=============================

The initialization of LibBSD is based on the FreeBSD
`SYSINIT(9) <http://www.freebsd.org/cgi/man.cgi?query=sysinit&sektion=9>`_
infrastructure.  The key to initializing a system is to ensure that the desired
device drivers are explicitly pulled into the linked application.  This plus
linking against the LibBSD (``libbsd.a``) will pull in the necessary FreeBSD
infrastructure.

The FreeBSD kernel is not a library like the RTEMS kernel.  It is a bunch of
object files linked together.  If we have a library, then creating the
executable is simple.  We begin with a start symbol and recursively resolve all
references.  With a bunch of object files linked together we need a different
mechanism.  Most object files don't know each other.  Lets say we have a driver
module.  The rest of the system has no references to this driver module.  The
driver module needs a way to tell the rest of the system: Hey, kernel I am
here, please use my services!

This registration of independent components is performed by SYSINIT(9) and
specializations

The SYSINIT(9) uses some global data structures that are placed in a certain
section.  In the linker command file we need this:

.. code-block:: none

    .rtemsroset : {
            KEEP (*(SORT(.rtemsroset.*)))
    }

    .rtemsrwset : {
            KEEP (*(SORT(.rtemsrwset.*)))
    }

This results for example in this executable layout:

.. code-block:: none

    [...]
     *(SORT(.rtemsroset.*))
     .rtemsroset.bsd.modmetadata_set.begin
                    0x000000000025fe00        0x0 libbsd.a(rtems-bsd-init.o)
                    0x000000000025fe00                _bsd__start_set_modmetadata_set
     .rtemsroset.bsd.modmetadata_set.content
                    0x000000000025fe00        0x8 libbsd.a(rtems-bsd-nexus.o)
     .rtemsroset.bsd.modmetadata_set.content
                    0x000000000025fe08        0x4 libbsd.a(kern_module.o)
    [...]
     .rtemsroset.bsd.modmetadata_set.content
                    0x000000000025fe68        0x4 libbsd.a(mii.o)
     .rtemsroset.bsd.modmetadata_set.content
                    0x000000000025fe6c        0x4 libbsd.a(mii_bitbang.o)
     .rtemsroset.bsd.modmetadata_set.end
                    0x000000000025fe70        0x0 libbsd.a(rtems-bsd-init.o)
                    0x000000000025fe70                _bsd__stop_set_modmetadata_set
    [...]
    .rtemsrwset     0x000000000030bad0      0x290
     *(SORT(.rtemsrwset.*))
     .rtemsrwset.bsd.sysinit_set.begin
                    0x000000000030bad0        0x0 libbsd.a(rtems-bsd-init.o)
                    0x000000000030bad0                _bsd__start_set_sysinit_set
     .rtemsrwset.bsd.sysinit_set.content
                    0x000000000030bad0        0x4 libbsd.a(rtems-bsd-nexus.o)
     .rtemsrwset.bsd.sysinit_set.content
                    0x000000000030bad4        0x8 libbsd.a(rtems-bsd-thread.o)
     .rtemsrwset.bsd.sysinit_set.content
                    0x000000000030badc        0x4 libbsd.a(init_main.o)
    [...]
     .rtemsrwset.bsd.sysinit_set.content
                    0x000000000030bd54        0x4 libbsd.a(frag6.o)
     .rtemsrwset.bsd.sysinit_set.content
                    0x000000000030bd58        0x8 libbsd.a(uipc_accf.o)
     .rtemsrwset.bsd.sysinit_set.end
                    0x000000000030bd60        0x0 libbsd.a(rtems-bsd-init.o)
                    0x000000000030bd60                _bsd__stop_set_sysinit_set
    [...]

Here you can see, that some global data structures are collected into
continuous memory areas.  This memory area can be identified by start and stop
symbols.  This constructs a table of uniform items.

The low level FreeBSD code calls at some time during the initialization the
mi_startup() function (machine independent startup).  This function will sort
the SYSINIT(9) set and call handler functions which perform further
initialization.  The last step is the scheduler invocation.

The SYSINIT(9) routines are run in ``mi_startup()`` which is called by
``rtems_bsd_initialize()``.  This is also explained in "The Design and
Implementation of the FreeBSD Operating System" section 14.3 "Kernel
Initialization".

In RTEMS, we have a library and not a bunch of object files.  Thus we need a
way to pull-in the desired services out of the libbsd.  Here the
``rtems-bsd-sysinit.h`` comes into play.  The SYSINIT(9) macros have been
modified and extended for RTEMS in ``<sys/kernel.h>``:

.. code-block:: none

    #ifndef __rtems__
    #define C_SYSINIT(uniquifier, subsystem, order, func, ident)    \
            static struct sysinit uniquifier ## _sys_init = {       \
                    subsystem,                                      \
                    order,                                          \
                    func,                                           \
                    (ident)                                         \
            };                                                      \
            DATA_SET(sysinit_set,uniquifier ## _sys_init)
    #else /* __rtems__ */
    #define SYSINIT_ENTRY_NAME(uniquifier)                          \
            _bsd_ ## uniquifier ## _sys_init
    #define SYSINIT_REFERENCE_NAME(uniquifier)                      \
            _bsd_ ## uniquifier ## _sys_init_ref
    #define C_SYSINIT(uniquifier, subsystem, order, func, ident)    \
            struct sysinit SYSINIT_ENTRY_NAME(uniquifier) = {       \
                    subsystem,                                      \
                    order,                                          \
                    func,                                           \
                    (ident)                                         \
            };                                                      \
            RWDATA_SET(sysinit_set,SYSINIT_ENTRY_NAME(uniquifier))
    #define SYSINIT_REFERENCE(uniquifier)                           \
            extern struct sysinit SYSINIT_ENTRY_NAME(uniquifier);   \
            static struct sysinit const * const                     \
            SYSINIT_REFERENCE_NAME(uniquifier) __used               \
            = &SYSINIT_ENTRY_NAME(uniquifier)
    #define SYSINIT_MODULE_REFERENCE(mod)                           \
            SYSINIT_REFERENCE(mod ## module)
    #define SYSINIT_DRIVER_REFERENCE(driver, bus)                   \
            SYSINIT_MODULE_REFERENCE(driver ## _ ## bus)
    #define SYSINIT_DOMAIN_REFERENCE(dom)                           \
            SYSINIT_REFERENCE(domain_add_ ## dom)
    #endif /* __rtems__ */

Here you see that the SYSINIT(9) entries are no longer static.  The
``*_REFERENCE()`` macros will create references to the corresponding modules
which are later resolved by the linker.  The application has to provide an
object file with references to all required FreeBSD modules.

System Control Hints
====================

If you get undefined references to ``_bsd_sysctl_*`` symbols, then you have to
locate and add the associated system control node, see
`SYSCTL(9) <http://www.freebsd.org/cgi/man.cgi?query=SYSCTL_DECL&sektion=9>`_.

Issues and TODO
===============

* PCI support on x86 uses a quick and dirty hack, see pci_reserve_map().

* Priority queues are broken with clustered scheduling.

* Per-CPU data should be enabled once the new stack is ready for SMP.

* Per-CPU NETISR(9) should be enabled onece the new stack is ready for SMP.

* Multiple routing tables are not supported.  Every FIB value is set to zero
  (= BSD_DEFAULT_FIB).

* Process identifiers are not supported.  Every PID value is set to zero
  (= BSD_DEFAULT_PID).

* User credentials are not supported.  The following functions allow the
  operation for everyone

  * prison_equal_ip4(),
  * chgsbsize(),
  * cr_cansee(),
  * cr_canseesocket() and
  * cr_canseeinpcb().

* A basic USB functionality test that is known to work on Qemu is desirable.

* Adapt generic IRQ PIC interface code to Simple Vectored Interrupt Model
  so that those architectures can use new TCP/IP and USB code.

* freebsd-userspace/rtems/include/sys/syslog.h is a copy from the old
  RTEMS TCP/IP stack. For some reason, the __printflike markers do not
  compile in this environment. We may want to use the FreeBSD syslog.h
  and get this addressed.

* in_cksum implementations for architectures not supported by FreeBSD.
  This will require figuring out where to put implementations that do
  not originate from FreeBSD and are populated via the script.

* MAC support functions are not thread-safe ("freebsd/lib/libc/posix1e/mac.c").

* IFCONFIG(8): IEEE80211 support is disabled.  This module depends on a XML
  parser and mmap().

* get_cyclecount(): The implementation is a security problem.

* What to do with the priority parameter present in the FreeBSD synchronization
  primitives and the thread creation functions?

* TASKQUEUE(9): Support spin mutexes.

* ZONE(9): Review allocator lock usage in rtems-bsd-chunk.c.

* KQUEUE(2): Choose proper lock for global kqueue list.

* TIMEOUT(9): Maybe use special task instead of timer server to call
  callout_tick().

* sysctl_handle_opaque(): Implement reliable snapshots.

* PING6(8): What to do with SIGALARM?

* <sys/param.h>: Update Newlib to use a MSIZE of 256.

* BPF(4): Add support for zero-copy buffers.

* UNIX(4): Fix race conditions in the area of socket object and file node
  destruction.  Add support for file descriptor transmission via control
  messages.

* PRINTF(9): Add support for log(), the %D format specifier is missing in the
  normal printf() family.

* Why is the interrupt server used?  The BSD interrupt handlers can block on
  synchronization primitives like mutexes.  This is in contrast to RTEMS
  interrupt service routines.  The BSPs using the generic interrupt support
  must implement the ``bsp_interrupt_vector_enable()`` and
  ``bsp_interrupt_vector_disable()`` routines.  They normally enable/disable a
  particular interrupt source at the interrupt controller.  This can be used to
  implement the interrupt server.  The interrupt server is a task that wakes-up
  in case an associated interrupt happens.  The interrupt source is disabled in
  a generic interrupt handler that wakes-up the interrupt server task.   Once
  the postponed interrupt processing is performed in the interrupt server the
  interrupt source is enabled again.

* Convert all BSP linkcmds to use a linkcmds.base so the sections are
  easier to insert.

* NIC Device Drivers
* Only common PCI NIC drivers have been included in the initial set. These
  do not include any system on chip or ISA drivers.
* PCI configuration probe does not appear to happen to determine if a
  NIC is in I/O or memory space. We have worked around this by using a
  static hint to tell the fxp driver the correct mode. But this needs to
  be addressed.
* The ISA drivers require more BSD infrastructure to be addressed. This was
  outside the scope of the initial porting effort.

* devfs (Device file system): There is a minimal implementation based on IMFS.
  The mount point is fixed to "/dev". Note that the devfs is only used by the
  cdev subsystem. cdev has been adapted so that the full path (including the
  leading "/dev") is given to devfs.  This saves some copy operations.

  devfs_create() first creates the full path and then creates an IMFS generic
  node for the device.

  TBD: remove empty paths on devfs_destroy().

* altq_subr.c - Arbitrary choices were made in this file that RTEMS would not
  support tsc frequency change.  Additionally, the clock frequency for
  machclk_freq is always measured for RTEMS.

* conf.h - In order to add make_dev and destroy_dev, variables in the cdev
  structure that were not being used were conditionally compiled out. The
  capability of supporting children did not appear to be needed and was not
  implemented in the rtems version of these routines.

* Problem to report to FreeBSD: The MMAP_NOT_AVAILABLE define is inverted on
  its usage.  When it is defined the mmap method is called. Additionally, it is
  not used thoroughly. It is not used in the unmap portion of the source.  The
  file rec_open.c uses the define MMAP_NOT_AVAILABLE to wrap the call to mmap
  and file rec_close.c uses the munmap method.
