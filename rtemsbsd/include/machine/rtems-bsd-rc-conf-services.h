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
 * The services available to rc.conf. You define the list you wish to have
 * available in your application and they will be available.
 *
 * A service is a functional unit that can be started.
 */

#ifndef _RTEMS_BSD_RC_CONF_SERVICES_h
#define _RTEMS_BSD_RC_CONF_SERVICES_h

#include <sys/queue.h>
#include <sys/kernel.h>

#include <machine/rtems-bsd-rc-conf.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Structure to pass argc and argv between functions.
 */
typedef struct rtems_bsd_rc_conf_argc_argv {
  uint32_t     marker;
  char*        command;
  int          argc;
  const char** argv;
} rtems_bsd_rc_conf_argc_argv;

/*
 * A service controls a peice of functionalty. It loosely follows the things
 * found in /etc/rc.d in FreeBSD. A service can query the rc.conf file for
 * items of interest.
 */
typedef int (*rtems_bsd_rc_conf_service)(rtems_bsd_rc_conf* rc_conf);

/*
 * Register a service. The name is the name of the service and the control
 * string contains the before, after and requires string.
 *
 * The control string has the format of:
 *
 *     key:item,item,item;
 *
 * where the keys are:
 *
 *  before
 *  after
 *  require
 *
 * For example "before:telnet;after:net;require:net"
 *
 * Notes
 *
 *  1. The parsing of this string is simple and will not handle any
 *     white-space. The tokens are delmiter to delmiter.
 *
 *  2. If there are competing positions in the control string the last one
 *     processed is used. After is processed first then 'before'.
 */
extern int rtems_bsd_rc_conf_service_add(const char*               name,
                                         const char*               control,
                                         rtems_bsd_rc_conf_service service);

/*
 * Remove a registered a service.
 */
extern int rtems_bsd_rc_conf_service_remove(const char* name);

/*
 * Return the name of the file being processed.
 */
extern const char* rtems_bsd_rc_conf_name(rtems_bsd_rc_conf* rc_conf);

/*
 * Search for the regular expression in the file for the first occurrence.
 */
extern int rtems_bsd_rc_conf_find(rtems_bsd_rc_conf*           rc_conf,
                                  const char*                  expression,
                                  rtems_bsd_rc_conf_argc_argv* argc_argv);

/*
 * Search for the next occurance of the regular expression in the file.
 */
extern int rtems_bsd_rc_conf_find_next(rtems_bsd_rc_conf*           rc_conf,
                                       rtems_bsd_rc_conf_argc_argv* argc_argv);

/*
 * Create an argc/argv structure. You can reuse this for repeated calls and the
 * rc.conf code manage the resources it uses. Destroy the structure when you
 * have finished.
 */
extern rtems_bsd_rc_conf_argc_argv* rtems_bsd_rc_conf_argc_argv_create(void);

/*
 * Destroy the argc/argv structure.
 */
extern void rtems_bsd_rc_conf_argc_argv_destroy(rtems_bsd_rc_conf_argc_argv* argc_argv);

/*
 * Return the name of the rc.conf file.
 */
extern const char* rtems_bsd_rc_conf_name(rtems_bsd_rc_conf* rc_conf);

/*
 * Return the current find line.
 */
extern int rtems_bsd_rc_conf_line(rtems_bsd_rc_conf* rc_conf);

/*
 * Return the verbose state.
 */
extern bool rtems_bsd_rc_conf_verbose(rtems_bsd_rc_conf* rc_conf);

/*
 * Print the argv list. Helper for verbose modes.
 */
extern void rtems_bsd_rc_conf_print_cmd(rtems_bsd_rc_conf* rc_conf,
                                        const char*        name,
                                        int                arvc,
                                        const char**       argv);

/*
 * Use the same SYSINT for all directives.
 */
#define RTEMS_BSD_RC_CONF_SYSINT(_n) \
  SYSINIT(_n, SI_SUB_CREATE_INIT, SI_ORDER_ANY, _n ## _init, NULL)

/*
 * Decls for the handlers.
 */
void rc_conf_net_init(void* arg);   /* Installed by default. */

/*
 * Added services.
 */
/* add here */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
