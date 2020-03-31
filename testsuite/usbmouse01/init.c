/*-
 * COPYRIGHT (c) 2017 Kevin Kirspel
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

/*
 * This test requires a USB mouse to be attached to a USB port.
 */

#include <sys/malloc.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mouse.h>

#include <rtems/console.h>
#include <rtems/shell.h>
#include <rtems/bsd/bsd.h>

#define TEST_NAME "LIBBSD USB MOUSE"
#define TEST_STATE_USER_INPUT 1
#include <rtems/test.h>

#define USB_SERIAL_TEST_BUFFER_SIZE 48
#define PRIO_OPEN  (RTEMS_MAXIMUM_PRIORITY - 12)
#define PRIO_READ  (RTEMS_MAXIMUM_PRIORITY - 11)

struct usb_test_message {
	int fd;
	char rbuf[USB_SERIAL_TEST_BUFFER_SIZE];
};

static rtems_id oid, rid, omid, rmid;
static volatile bool kill_otask, kill_rtask;
static volatile bool otask_active, rtask_active;

static void
usb_mouse_read_task(rtems_task_argument arg)
{
	rtems_status_code sc;
	struct usb_test_message msg;
	uint32_t size;
	int bytes;

	rtask_active = true;
	kill_rtask = false;
	while (!kill_rtask) {
		while (!kill_rtask) {
			sc = rtems_message_queue_receive(rmid, &msg, &size, RTEMS_WAIT, RTEMS_MILLISECONDS_TO_TICKS(1000));
			if (sc == RTEMS_SUCCESSFUL) {
				if (msg.fd > 0) {
					break;
				} else {
					printf("Invalid file descriptor\n");
				}
			}
	  }
		while (!kill_rtask) {
			msg.rbuf[0] = 0;
			bytes = read(msg.fd, &msg.rbuf[0], 1);
			if (bytes == 0) {
				printf("Got EOF from the input device\n");
			} else if (bytes < 0) {
				if (errno != EINTR && errno != EAGAIN) {
					printf("Could not read from input device\n");
					break;
				}
				rtems_task_wake_after(RTEMS_MILLISECONDS_TO_TICKS(10));
			} else {
				printf("received char: 0x%02X\n", msg.rbuf[0]);
			}
	  }
	  msg.fd = -1;
	  rtems_message_queue_send(omid, &msg, sizeof(msg));
	}

	rtask_active = false;
	rtems_task_delete(RTEMS_SELF);
}

static void
usb_mouse_open_task(rtems_task_argument arg)
{
	rtems_status_code sc;
	struct usb_test_message msg;
	uint32_t size;
	int fd;

	fd = -2;
	otask_active = true;
	kill_otask = false;
	while (!kill_otask) {
		sc = rtems_message_queue_receive(omid, &msg, &size, RTEMS_WAIT, RTEMS_MILLISECONDS_TO_TICKS(1000));
		if (sc == RTEMS_SUCCESSFUL) {
			if (fd >= 0) {
				close(fd);
				printf("mouse device closed\n");
			}
			fd = msg.fd;
		}
		if (fd == -1) {
			fd = open("/dev/ums0", O_RDWR | O_NONBLOCK);
			if (fd != -1) {
				printf("mouse device opened: %d\n", fd);
				msg.fd = fd;
				rtems_message_queue_send(rmid, &msg, sizeof(msg));
			}
			else {
				/*printf("mouse device open failed: %d\n", errno);*/
			}
		}
	}
	if (fd >= 0) {
		close(fd);
		printf("mouse device closed\n");
	}
	otask_active = false;
	rtems_task_delete(RTEMS_SELF);
}

static void
Init(rtems_task_argument arg)
{
	rtems_status_code sc;
	struct usb_test_message msg;

	(void) arg;
	rtems_test_begin(TEST_NAME, TEST_STATE);

	sc = rtems_message_queue_create(
		rtems_build_name ('M', 'U', 'O', 'P'),
		16,
		sizeof(struct usb_test_message),
		RTEMS_PRIORITY,
		&omid
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_message_queue_create(
		rtems_build_name ('M', 'U', 'R', 'D'),
		16,
		sizeof(struct usb_test_message),
		RTEMS_PRIORITY,
		&rmid
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_create(
		rtems_build_name('U', 'S', 'B', 'R'),
		PRIO_READ,
		RTEMS_MINIMUM_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_FLOATING_POINT,
		&rid
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_create(
		rtems_build_name('U', 'S', 'B', 'O'),
		PRIO_OPEN,
		RTEMS_MINIMUM_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_FLOATING_POINT,
		&oid
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_start(rid, usb_mouse_read_task, 0);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_start(oid, usb_mouse_open_task, 0);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_bsd_initialize();
	assert(sc == RTEMS_SUCCESSFUL);

	msg.fd = -1;
	rtems_message_queue_send(omid, &msg, sizeof(msg));

	sc = rtems_shell_init("SHLL", 16 * 1024, 1, CONSOLE_DEVICE_NAME,
	    false, true, NULL);
	assert(sc == RTEMS_SUCCESSFUL);

	kill_otask = true;
	kill_rtask = true;
	while (otask_active || rtask_active) {
		rtems_task_wake_after(RTEMS_MILLISECONDS_TO_TICKS(10));
	}

	sc = rtems_message_queue_delete(rmid);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_message_queue_delete(omid);
	assert(sc == RTEMS_SUCCESSFUL);

	rtems_test_end(TEST_NAME);

	exit(0);
}

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_ZERO_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_MAXIMUM_DRIVERS 32

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 32

#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1

#define CONFIGURE_UNLIMITED_OBJECTS
#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_STACK_CHECKER_ENABLED

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_STACK_SIZE (32 * 1024)
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

#include <bsp/nexus-devices.h>

SYSINIT_DRIVER_REFERENCE(ums, uhub);

#define CONFIGURE_SHELL_COMMANDS_INIT

#include <bsp/irq-info.h>

#include <rtems/netcmds-config.h>

#define CONFIGURE_SHELL_USER_COMMANDS \
  &bsp_interrupt_shell_command, \
  &rtems_shell_SYSCTL_Command

#define CONFIGURE_SHELL_COMMAND_CPUUSE
#define CONFIGURE_SHELL_COMMAND_PERIODUSE
#define CONFIGURE_SHELL_COMMAND_STACKUSE
#define CONFIGURE_SHELL_COMMAND_PROFREPORT

#define CONFIGURE_SHELL_COMMAND_CP
#define CONFIGURE_SHELL_COMMAND_PWD
#define CONFIGURE_SHELL_COMMAND_LS
#define CONFIGURE_SHELL_COMMAND_LN
#define CONFIGURE_SHELL_COMMAND_LSOF
#define CONFIGURE_SHELL_COMMAND_CHDIR
#define CONFIGURE_SHELL_COMMAND_CD
#define CONFIGURE_SHELL_COMMAND_MKDIR
#define CONFIGURE_SHELL_COMMAND_RMDIR
#define CONFIGURE_SHELL_COMMAND_CAT
#define CONFIGURE_SHELL_COMMAND_MV
#define CONFIGURE_SHELL_COMMAND_RM
#define CONFIGURE_SHELL_COMMAND_MALLOC_INFO

#include <rtems/shellconfig.h>
