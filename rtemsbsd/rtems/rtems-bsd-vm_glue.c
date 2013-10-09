/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * COPYRIGHT (c) 2012.
 * On-Line Applications Research Corporation (OAR).
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <machine/rtems-bsd-config.h>

#include <rtems/bsd/sys/types.h>
#include <rtems/bsd/sys/param.h>
#include <rtems/bsd/sys/lock.h>
#include <sys/sched.h>

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");
#include <sys/kernel.h>
#include <vm/uma.h>
#include <vm/uma_int.h>
#include <sys/systm.h>

/*
 * System initialization
 */
static int boot_pages = UMA_BOOT_PAGES;
static void vm_mem_init(void *);
SYSINIT(vm_mem, SI_SUB_VM, SI_ORDER_FIRST, vm_mem_init, NULL);


static void
vm_mem_init(dummy)
        void *dummy;
{
  void        *mapped;
  static void *callwheel_array[270];
  caddr_t      c;
  caddr_t      p;

  /*
   * The values for mapped came from the freeBSD method
   * vm_page_startup() in the freeBSD file vm_page.c.
   * XXX - This may need to be adjusted for our system.
   */
  mapped = calloc( boot_pages * UMA_SLAB_SIZE, 1 );
  uma_startup((void *)mapped, boot_pages);

  /*
   * The following is doing a minimal amount of work from 
   * the method vm_ksubmap_init() in freeBSD vm_init.c.
   */
  c = (caddr_t) callwheel_array;
  p = kern_timeout_callwheel_alloc(c);
  printf( "*** callwheel 0x%x 0x%x 0x%x\n", c, p,  (c + sizeof(callwheel_array)) );
  if ( p > (c + sizeof(callwheel_array)) )
    panic( "*** not enough memory for callwheel_array ***" );
  kern_timeout_callwheel_init();
  uma_startup2();
}

/*
 * MPSAFE
 *
 * WARNING!  This code calls vm_map_check_protection() which only checks
 * the associated vm_map_entry range.  It does not determine whether the
 * contents of the memory is actually readable or writable.  In most cases
 * just checking the vm_map_entry is sufficient within the kernel's address
 * space.
 */
int
kernacc(addr, len, rw)
	void *addr;
	int len, rw;
{
  return 1;
}

/*
 * MPSAFE
 *
 * WARNING!  This code calls vm_map_check_protection() which only checks
 * the associated vm_map_entry range.  It does not determine whether the
 * contents of the memory is actually readable or writable.  vmapbuf(),
 * vm_fault_quick(), or copyin()/copout()/su*()/fu*() functions should be
 * used in conjuction with this call.
 */
int
useracc(addr, len, rw)
	void *addr;
	int len, rw;
{
  return 1;
}

int
vslock(void *addr, size_t len)
{
  return 0;
}

void
vsunlock(void *addr, size_t len)
{
}

/*
 * Destroy the given CPU private mapping and unpin the page that it mapped.
 */
void
vm_imgact_unmap_page(struct sf_buf *sf)
{
}


/*
 * Create the kernel stack (including pcb for i386) for a new thread.
 * This routine directly affects the fork perf for a process and
 * create performance for a thread.
 */
int
vm_thread_new(struct thread *td, int pages)
{
  return (1);
}

/*
 * Dispose of a thread's kernel stack.
 */
void
vm_thread_dispose(struct thread *td)
{
}

/*
 * Allow a thread's kernel stack to be paged out.
 */
void
vm_thread_swapout(struct thread *td)
{
}

/*
 * Bring the kernel stack for a specified thread back in.
 */
void
vm_thread_swapin(struct thread *td)
{
}

/*
 * Implement fork's actions on an address space.
 * Here we arrange for the address space to be copied or referenced,
 * allocate a user struct (pcb and kernel stack), then call the
 * machine-dependent layer to fill those in and make the new process
 * ready to run.  The new process is set up so that it returns directly
 * to user mode to avoid stack copying and relocation problems.
 */
int
vm_forkproc(td, p2, td2, vm2, flags)
	struct thread *td;
	struct proc *p2;
	struct thread *td2;
	struct vmspace *vm2;
	int flags;
{
}

/*
 * Called after process has been wait(2)'ed apon and is being reaped.
 * The idea is to reclaim resources that we could not reclaim while
 * the process was still executing.
 */
void
vm_waitproc(p)
	struct proc *p;
{
}

void
faultin(p)
	struct proc *p;
{
}

void
kick_proc0(void)
{
}
