/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief FreeBSD uma source used pages which where there was an
 * assumption of page alignment.  Doing this alignment would waste
 * more memory than we were willing to do.  Therefore, a set of
 * rtems-bsd-page routines track the allocation of pages and the
 * small sections of source in the uma source were modified to use
 * these methods on an rtems system.
 */

/*
 * COPYRIGHT (c) 2012. On-Line Applications Research Corporation (OAR).
 * All rights reserved.
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

#include <freebsd/machine/rtems-bsd-config.h>

#include <freebsd/sys/types.h>
#include <freebsd/sys/systm.h>
#include <freebsd/sys/malloc.h>
#include <rtems/chain.h>


#define RTEMS_PAGE_COUNT  100

typedef struct {
  rtems_chain_node node;
  void             *page;
  void             *end;

} rtems_page_t;

// chain of pages that have been allocated
static rtems_chain_control rtems_page_list =  RTEMS_CHAIN_INITIALIZER_EMPTY( rtems_page_list );

void *
rtems_page_alloc(int bytes)
{
  static void         *p;
  static rtems_page_t *page;

  bytes = round_page(bytes);
  p     = (void *) malloc(bytes, M_TEMP, M_NOWAIT | M_ZERO);

  page = (rtems_page_t *) malloc(sizeof(rtems_page_t), M_TEMP, M_NOWAIT | M_ZERO);
  page->page = p;
  page->end  = p + bytes;

  rtems_chain_append( &rtems_page_list, page );

  return p;
}

rtems_page_t *rtems_page_t_find( void *address )
{
  rtems_chain_node *the_node;
  rtems_page_t     *the_page = NULL;

  for (the_node = rtems_chain_first( &rtems_page_list );
       !rtems_chain_is_tail(&rtems_page_list, the_node);
       the_node = rtems_chain_next(the_node)) {
    the_page = the_node;
  
    if ((address >= the_page->page) && 
        (address <= the_page->end))
      return the_page;
  }
  return NULL;
}

void *rtems_page_find( void *address )
{
  rtems_page_t *ptr;

  ptr = rtems_page_t_find( address );
  
  if (ptr)
    return ptr->page;

  return ptr;
}

void rtems_page_free( void *address )
{
  rtems_page_t *ptr;

  ptr = rtems_page_t_find( address );
  KASSERT(ptr != NULL, ("Unable to locate page for freed element"));
  
  free( ptr->page, M_TEMP );
  free( ptr, M_TEMP );
}
