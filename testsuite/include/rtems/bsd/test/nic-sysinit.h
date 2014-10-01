/**
 * @file
 *
 * @ingroup demo
 *
 * @brief TODO.
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


#include <machine/rtems-bsd-sysinit.h>

/*
 *  We "read" that and generate references and nexus devices
 */

#if defined(CONFIGURE_NEED_USB)
  SYSINIT_NEED_USB_CORE;
#endif
#if defined(CONFIGURE_NEED_PCIB)
  SYSINIT_NEED_PCIB;
#endif
#if defined(CONFIGURE_NEED_NET_IF_FXP)
  SYSINIT_NEED_NET_IF_FXP;
#endif
#if defined(CONFIGURE_NEED_NET_IF_DC)
  SYSINIT_NEED_NET_IF_DC;
#endif
#if defined(CONFIGURE_NEED_NET_IF_BFE)
  SYSINIT_NEED_NET_IF_BFE;
#endif
#if defined(CONFIGURE_NEED_NET_IF_RE)
  SYSINIT_NEED_NET_IF_RE;
#endif
#if defined(CONFIGURE_NEED_NET_IF_EM)
  SYSINIT_NEED_NET_IF_EM;
#endif
#if defined(CONFIGURE_NEED_NET_IF_IGB)
  SYSINIT_NEED_NET_IF_IGB;
#endif
#if defined(CONFIGURE_NEED_NET_IF_BCE)
  SYSINIT_NEED_NET_IF_BCE;
#endif
#if defined(CONFIGURE_NEED_NET_IF_LEM)
  SYSINIT_NEED_NET_IF_LEM;
#endif
#if defined(CONFIGURE_NEED_NET_IF_BGE)
  SYSINIT_NEED_NET_IF_BGE; // does not link 29 June 2012
#endif
