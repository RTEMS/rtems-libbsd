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
 * Parse a FreeBSD /etc/rc.conf format file and execute the configuration
 * options we want to support.
 */

#ifndef _RTEMS_BSP_RC_CONF_h
#define _RTEMS_BSP_RC_CONF_h

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Maximum size of an rc.conf file.
 */
#define RTEMS_BSD_RC_CONF_MAX_SIZE (8 * 1024)

/*
 * Directive processing data. This data is opaque externally.
 */
typedef struct rtems_bsd_rc_conf_ rtems_bsd_rc_conf;

/*
 * A directive is a line in rc.conf and is 'name=value'. The handler is invoked
 * if the name matches directive's regular expression.
 */
typedef int (*rtems_bsd_rc_conf_directive)(rtems_bsd_rc_conf* rc_conf,
                                           int                argc,
                                           const char**       argv);

/*
 * Register a directive handler.
 */
extern int rtems_bsd_rc_conf_directive_add(const char*  dir_regex,
                                           rtems_bsd_rc_conf_directive handler);

/*
 * Run an rc.conf script loaded into memory.
 */
extern int rtems_bsd_run_rc_conf_script(const char* name, const char* text, bool verbose);

/*
 * Run the rc.conf file.
 */
extern int rtems_bsd_run_rc_conf(const char* name, bool verbose);

/*
 * Run /etc/rc.conf.
 */
extern int rtems_bsd_run_etc_rc_conf(bool verbose);

/*
 * Return the name of the file being processed.
 */
extern const char* rtems_bsd_rc_conf_name(rtems_bsd_rc_conf* rc_conf);

/*
 * Return the line number being processed.
 */
extern int rtems_bsd_rc_conf_line(rtems_bsd_rc_conf* rc_conf);

/*
 * Print the argv list. Helper for verbose modes.
 */
extern void rtems_bsd_rc_conf_print_cmd(rtems_bsd_rc_conf* rc_conf,
                                        const char*        name,
                                        int                argc,
                                        const char**       argv);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
