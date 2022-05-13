Guidelines for Developing and Contributing Code
***********************************************

Introduction
============

This guide aims to help developing and contributing code to the LibBSD.  One
goal of the LibBSD is to stay in synchronization with FreeBSD.  This is only
feasible if certain rules are in place.  Otherwise, managing more than a
thousand imported source files will become too labour intensive eventually.

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

Updates to FreeBSD or RTEMS Kernel Support
==========================================

If you update code or change any defines that effect the generated
code in the following paths:

* ``freebsd/sys/*.[ch]``
* ``rtemsbsd/rtems/rtems-kernel-*.c``

you need to see if any new kernel symbols have been generated or
exposed. The tool ``rtems-kern-symbols`` command supports checking and
updating the kernel symbol namespace.

The public (global) kernel symbosl need to reside in a private
namespace to avoid clashing with symbols in the user space code or
applications. The FreeBSD kernel names functions and variables
assuming a private kernel only symbols space. RTEMS builds FreeBSD
kernel and user space code in the same symbols space so there can be
clashes. We manage this by maintaining a header file that maps the
global kernel symbols to a private namespace. For example ``malloc`` is
mapped to ``_bsd_malloc``.

The set of symbols to map is not easy to obtain because symbols may be
the result of complex preprocessing of the source, the code is
specific to a BSP or the code is controlled by a buildset.

The approach we use is to not remove symbols unless you are certain
the symbols have been removed from the FreeBSD kernel source. This is
a manual operation.

You are required to check symbols with a 32bit and 64bit
architecture.

If you are working on a specific BSP and related drivers please make
sure the kernel symbols are checked. It is too much to ask every
developer to build all BSPs and check.

RTEMS Kernel Symbols Tool
=========================

The python tool ``rtems-kern-symbols`` can read a kernel header loading
a previously generated version. This maintains the current symbol set
without you needing to build the object files previously scanned.

The kernel namespace header can be regenerated from the original
header. This checks the kernel header is already sorted. If you think
there is a sorting issue in the existing header please regenerate
without adding new symbols.

.. code-block:: none

    ./rtems-kern-symbols --regenerate --output=tmp.h

This command needs access to your built RTEMS tools. You can set your
environment ``PATH`` variable or you can specify the top level path as an argument:

.. code-block:: none

    ./rtems-kern-symbols --rtems-tools=/opt/work/rtems/6

Options:

* You can provide a different kernel header using the ``--kern-header``
argument. The default is the LibbSD header.

* The ``--report`` option provides a report.

* The ``--diff`` option provides a unified diff of any changes.

* The ``--write`` option is needed to write any changes

* The ``--output`` option lets you control the output kernel header file
  change are written too

The tool manages a number of exlcuded symbols. These are symbols in
the kernel space that are not mapped to the RTEMS kernel namespace.

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
* Run `./rtems-kern-symbols` as discussed above
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

Do not format original FreeBSD code.  Do not perform white space changes even
if you get git commit warnings.  Check your editor settings so that it doesn't
perform white space changes automatically, for example adding a newline to the
end of the file.  White space changes may result in conflicts during updates,
especially changes at the end of a file.

Automatically Generated FreeBSD Files
=====================================

Some source and header files are automatically generated during the FreeBSD
build process.  The ``Makefile.todo`` file performs this manually.  The should be
included in ``freebsd-to-rtems.py`` script some time in the future.  For details,
see also
`KOBJ(9) <http://www.freebsd.org/cgi/man.cgi?query=kobj&sektion=9&apropos=0>`_.
