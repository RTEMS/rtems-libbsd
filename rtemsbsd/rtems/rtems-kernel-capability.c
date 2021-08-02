/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2020 Chris Johns
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

#include <machine/rtems-bsd-kernel-space.h>

#include <sys/types.h>
#include <sys/caprights.h>

cap_rights_t cap_no_rights;

#define RTEMS_BSD_CAP_ALIAS(var, tar) \
	extern int __attribute__((alias("_bsd_" #var))) _bsd_##tar;

#ifdef INVARIANT_SUPPORT
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_rights_is_valid);
#endif
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_accept_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_bind_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_connect_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_event_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_fcntl_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_fstat_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_fsync_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_ftruncate_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_getpeername_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_getsockname_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_getsockopt_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_ioctl_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_listen_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_mkdirat_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_mknodat_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_read_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_recv_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_renameat_source_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_renameat_target_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_seek_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_setsockopt_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_shutdown_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_symlinkat_rights);
RTEMS_BSD_CAP_ALIAS(cap_no_rights, cap_write_rights);
