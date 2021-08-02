/**
 * @file
 *
 * @ingroup rtems_bsd
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009, 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
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

#ifndef _RTEMS_BSD_BSD_H_
#define _RTEMS_BSD_BSD_H_

#include <sys/cdefs.h>
#include <sys/queue.h>
#include <sys/kernel.h>

#include <stdarg.h>

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * The default memory size. Do not change, use
 * RTEMS_BSD_CONFIG_DOMAIN_PAGE_MBUFS_SIZE to override for your application.
 */
#define RTEMS_BSD_ALLOCATOR_DOMAIN_PAGE_MBUF_DEFAULT (8 * 1024 * 1024)

/*
 * The default block IO buffer memory size. Do not change, use
 * RTEMS_BSD_CONFIG_DOMAIN_BIO_SIZE to override for your application.
 */
#define RTEMS_BSD_ALLOCATOR_DOMAIN_BIO_DEFAULT (4 * 1024 * 1024)

typedef enum {
	RTEMS_BSD_RES_IRQ = 1,
	RTEMS_BSD_RES_MEMORY = 3
} rtems_bsd_device_resource_type;

typedef struct {
	rtems_bsd_device_resource_type type;
	unsigned long start_request;
	unsigned long start_actual;
} rtems_bsd_device_resource;

typedef struct {
	const char *name;
	int unit;
	size_t resource_count;
	const rtems_bsd_device_resource *resources;
	const struct sysinit *driver_reference;
} rtems_bsd_device;

#define RTEMS_BSD_DEFINE_NEXUS_DEVICE(name, unit, resource_count, resources) \
    extern struct sysinit SYSINIT_ENTRY_NAME(name##_nexusmodule); \
    RTEMS_BSD_DEFINE_SET_ITEM(nexus, name##unit, rtems_bsd_device) = \
        { #name, unit, (resource_count), (resources), \
            &SYSINIT_ENTRY_NAME(name##_nexusmodule) }

#define RTEMS_BSD_DEFINE_NEXUS_DEVICE_ORDERED(name, unit, order, resource_count, resources) \
    extern struct sysinit SYSINIT_ENTRY_NAME(name##_nexusmodule); \
    RTEMS_BSD_DEFINE_SET_ITEM_ORDERED(nexus, name##unit, order, rtems_bsd_device) = \
        { #name, unit, (resource_count), (resources), \
            &SYSINIT_ENTRY_NAME(name##_nexusmodule) }

rtems_status_code rtems_bsd_initialize(void);

/**
 * @brief Initializes the libbsd and starts a DHCPCD task.
 *
 * The libbsd is initialized via rtems_bsd_initialize().  If this is
 * successful, then the loop back interfaces are created.  If this is
 * successful, then a DHCPCD task is started at the least important priority.
 *
 * The default devices of the BSP are initialized.  Support for
 *   - IF_BRIDGE(4),
 *   - LAGG(4),
 *   - multicast routing,
 *   - UNIX(4), and
 *   - VLAN(4),
 * is enabled.
 *
 * No RTEMS shell commands are registered.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval otherwise An error occurred.
 */
rtems_status_code rtems_bsd_initialize_dhcp(void);

/**
 * @brief Configures the lo0 (loopback) interface.
 *
 * @return Returns an exit code, see also <sysexits.h>.
 */
int rtems_bsd_ifconfig_lo0(void);

/**
 * @brief Configures an interface.
 *
 * In case addr_gateway is not NULL, then it will be used as the default
 * gateway.
 *
 * @param[in] ifname The interface name.
 * @param[in] addr_self The address of the interface.
 * @param[in] netmask The sub-network mask.
 * @param[in] addr_gateway The address of the gateway.  May be NULL.
 *
 * @return Returns an exit code, see also <sysexits.h>.
 */
int rtems_bsd_ifconfig(const char *ifname, const char *addr_self,
    const char *netmask, const char *addr_gateway);

/**
 * @brief Returns the initial priority for a task specified by its name.
 *
 * Applications may provide their own implementation of this function.  For
 * example they can define their implementation in the same module which calls
 * rtems_bsd_initialize().
 *
 * @param[in] name The task name.
 *
 * @return The desired initial task priority.
 */
rtems_task_priority rtems_bsd_get_task_priority(const char *name);

/**
 * @brief Returns the stack size for a task specified by its name.
 *
 * Applications may provide their own implementation of this function.  For
 * example they can define their implementation in the same module which calls
 * rtems_bsd_initialize().
 *
 * @param[in] name The task name.
 *
 * @return The desired task stack size.
 */
size_t rtems_bsd_get_task_stack_size(const char *name);

typedef enum {
	RTEMS_BSD_ALLOCATOR_DOMAIN_PAGE,
	RTEMS_BSD_ALLOCATOR_DOMAIN_MBUF,
	RTEMS_BSD_ALLOCATOR_DOMAIN_BIO,
	RTEMS_BSD_ALLOCATOR_DOMAIN_MALLOC
} rtems_bsd_allocator_domain;

/**
 * @brief The size for the page/mbufs default allocator domain.
 *
 * Applications may set this value to change the value returned by the default.
 */
extern uintptr_t rtems_bsd_allocator_domain_page_mbuf_size;

/**
 * @brief The size for the block IO default buffer memory.
 *
 * Applications may set this value to change the value returned by the default.
 */
extern uintptr_t rtems_bsd_allocator_domain_bio_size;

/**
 * @brief Returns the size for a specific allocator domain.
 *
 * Applications may provide their own implementation of this function.  For
 * example they can define their implementation in the same module which calls
 * rtems_bsd_initialize().
 *
 * @param[in] domain The allocator domain.
 *
 * @return The desired size for the specified allocator domain.
 */
uintptr_t rtems_bsd_get_allocator_domain_size(
    rtems_bsd_allocator_domain domain);

/**
 * @brief Returns the Ethernet MAC address for a specified device.
 *
 * Applications may provide their own implementation of this function.  For
 * example they can define their implementation in the same module which calls
 * rtems_bsd_initialize().
 *
 * The default implementation returns
 * { 0x0e, 0xb0, 0xba, 0x5e, 0xba, 0x11 + unit }.
 *
 * This is a workaround.  A better approach would be to use a device tree and a
 * bus device similar to the FreeBSD SIMPLEBUS(4).
 *
 * @param[in] name The device name.
 * @param[in] unit The device unit.
 * @param[out] mac_addr The Ethernet MAC address.
 */
void rtems_bsd_get_mac_address(const char *name, int unit,
    uint8_t mac_addr[6]);

/**
 * @defgroup BSDBusRoot Bus Root Functions
 *
 * @brief Functions to perform bus root operations.
 *
 * Suspend and resume can be used to go into or exit a power saving state.
 * Detach may be used to shutdown the system and do a warm reset.
 *
 * All functions must be called from task context.  They perform complex
 * operations affecting all devices of the bus tree and work only if all
 * drivers are written properly and no resources used by the drivers are
 * blocked.
 *
 * @{
 */

/**
 * @brief Attach the root bus.
 *
 * @retval 0 Successful operation.
 * @retval errno Otherwise.
 */
int rtems_bsd_bus_root_attach(void);

/**
 * @brief Suspend the root bus.
 *
 * @retval 0 Successful operation.
 * @retval errno Otherwise.
 */
int rtems_bsd_bus_root_suspend(void);

/**
 * @brief Resume the root bus.
 *
 * @retval 0 Successful operation.
 * @retval errno Otherwise.
 */
int rtems_bsd_bus_root_resume(void);

/**
 * @brief Detach the root bus.
 *
 * @retval 0 Successful operation.
 * @retval errno Otherwise.
 */
int rtems_bsd_bus_root_detach(void);

/**
 * @brief The output back-end for logging functions.
 */
typedef int (*rtems_bsd_vprintf_handler)(int, const char *, va_list);

/**
 * @brief Sets the output back-end for logging functions.
 *
 * @param new_handler The new output back-end for logging functions.
 *
 * @return The previous handler.
 *
 * @see rtems_bsd_vprintf().
 */
rtems_bsd_vprintf_handler rtems_bsd_set_vprintf_handler(
    rtems_bsd_vprintf_handler new_handler);

/**
 * @brief Output back-end for logging functions.
 *
 * Used by kernel space printf(), vprintf(), log() and vlog().  Used by user
 * space syslog() and vsyslog().
 *
 * The default uses putchar() and uses a mutex to serialize the output.  It may
 * be customized via rtems_bsd_set_vprintf_handler().
 */
int rtems_bsd_vprintf(int level, const char *fmt, va_list ap);

/**
 * @brief Set the syslog priority. See syslog.h for the names.
 *
 * @param priority One of the standard names.
 * @retval 0 Priority set.
 * @retval errno Otherwise.
 */
int rtems_bsd_setlogpriority(const char* priority);

/**
 * @brief Set this vprintf handler to suppress all output.
 *
 * @retval Allways 0.
 *
 * @see rtems_bsd_vprintf() for the parameters.
 */
int rtems_bsd_vprintf_handler_mute(int level, const char *fmt, va_list ap);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_BSD_BSD_H_ */
