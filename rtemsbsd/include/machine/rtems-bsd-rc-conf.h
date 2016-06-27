/*
 * Copyright (c) 2016 Chris Johns <chrisj@rtems.org>.  All rights reserved.
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

/*
 * FreeBSD /etc/rc.conf initialisation. Initialise the software stack using the
 * the configuration options held in your rc.conf file. The file conforms to the
 * format defined in the FreeBSD man page rc.conf(5).
 *
 * The services and configuration present in your rc.conf will only work if the
 * services have been configured into your application.
 */

#ifndef _RTEMS_BSD_RC_CONF_h
#define _RTEMS_BSD_RC_CONF_h

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Maximum size of an rc.conf file.
 */
#define RTEMS_BSD_RC_CONF_MAX_SIZE (8 * 1024)

/*
 * The rc.conf  data. It is externally opaque.
 */
typedef struct rtems_bsd_rc_conf_ rtems_bsd_rc_conf;

/*
 * Run an rc.conf script loaded into memory.
 *
 * The timeout can be -1 for no wait, 0 to wait forever or a timeout in seconds.
 */
extern int rtems_bsd_run_rc_conf_script(const char* name,
                                        const char* text,
                                        int         timeout,
                                        bool        verbose);

/*
 * Run the rc.conf file.
 *
 * The timeout can be -1 for no wait, 0 to wait forever or a timeout in seconds.
 */
extern int rtems_bsd_run_rc_conf(const char* name, int timeout, bool verbose);

/*
 * Run /etc/rc.conf.
 *
 * The timeout can be -1 for no wait, 0 to wait forever or a timeout in seconds.
 */
extern int rtems_bsd_run_etc_rc_conf(int timeout, bool verbose);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
