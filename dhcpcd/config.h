#ifndef DHCPCD_CONFIG_H
#define DHCPCD_CONFIG_H
/* RTEMS */
#define PLATFORM_BSD
#define SYSCONFDIR	"/etc"
#define SBINDIR		"/sbin"
#define LIBDIR		"/lib"
#define LIBEXECDIR	"/libexec"
#define DBDIR		"/var/db"
#define RUNDIR		"/var/run"
#include <rtems/bsd/sys/time.h>
#include <machine/rtems-bsd-program.h>
#include <machine/rtems-bsd-commands.h>
#include <spawn.h>
#include <stdint.h>
#include "compat/pollts.h"
uint32_t arc4random(void);
static inline int dhcpcd_flock(int a, int b) { return -1; }
#define flock(a, b) dhcpcd_flock(a, b)
#define getline __getline
#define closefrom(x) do { } while (0)
#define _PATH_BPF "/dev/bpf"
#ifdef __need_getopt_newlib
#define optind dhcpcd_getopt_data.optind
#define optarg dhcpcd_getopt_data.optarg
#define opterr dhcpcd_getopt_data.opterr
#define optopt dhcpcd_getopt_data.optopt
#define getopt_long(argc, argv, so, lo, li) getopt_long_r(argc, argv, so, lo, li, &dhcpcd_getopt_data)
extern struct getopt_data dhcpcd_getopt_data;
#endif /* __need_getopt_newlib */
#include "namespace.h"
#endif /* DHCPCD_CONFIG_H */
