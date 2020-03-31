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

#include <sys/malloc.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <rtems/console.h>
#include <rtems/shell.h>
#include <rtems/bsd/bsd.h>
#include <rtems/test.h>

#define TEST_NAME "LIBBSD USB SERIAL"

#define USB_SERIAL_TEST_BUFFER_SIZE 48
#define PRIO_OPEN  (RTEMS_MAXIMUM_PRIORITY - 12)
#define PRIO_READ  (RTEMS_MAXIMUM_PRIORITY - 11)
#define PRIO_WRITE (RTEMS_MAXIMUM_PRIORITY - 10)

struct usb_test_message {
	int fd;
	char rbuf[USB_SERIAL_TEST_BUFFER_SIZE];
	char wbuf[USB_SERIAL_TEST_BUFFER_SIZE];
};

static rtems_id oid, rid, wid, omid, rmid, wmid;
static volatile bool kill_otask, kill_rtask, kill_wtask;
static volatile bool otask_active, rtask_active, wtask_active;

static void
usb_serial_read_task(rtems_task_argument arg)
{
	struct usb_test_message msg;
	size_t size;
	uint32_t end_time;
	int bytes, index;

	rtask_active = true;
	kill_rtask = false;
	while (!kill_rtask) {
		rtems_message_queue_receive(rmid, &msg, &size, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
		index = 0;
		msg.rbuf[0] = 0;
		end_time = rtems_clock_get_ticks_since_boot() + RTEMS_MILLISECONDS_TO_TICKS(500);
		while ((rtems_clock_get_ticks_since_boot() < end_time) && (index < sizeof(msg.rbuf))) {
			bytes = read(msg.fd, &msg.rbuf[index], sizeof(msg.rbuf) - index - 1);
			if (bytes < 0) {
				msg.fd = -1;
				rtems_message_queue_send(omid, &msg, sizeof(msg));
				printf("serial device read error: %d\n", errno);
			} else if (bytes > 0) {
				index += bytes;
				msg.rbuf[index] = 0;
				if (strcmp(msg.rbuf, msg.wbuf) == 0) {
					break;
				}
			}
			rtems_task_wake_after(RTEMS_MILLISECONDS_TO_TICKS(10));
		}
		printf("serial device read: %s - %s\n", msg.rbuf, strcmp(msg.rbuf, msg.wbuf) == 0 ? "PASS" : "FAIL");
	}
	rtask_active = false;
}

static void
usb_serial_write_task(rtems_task_argument arg)
{
	struct usb_test_message msg;
	size_t size;
	int bytes, write_len, count = 0;

	wtask_active = true;
	kill_wtask = false;
	while (!kill_wtask) {
		rtems_message_queue_receive(wmid, &msg, &size, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
		count++;
		sprintf(msg.wbuf, "Hello World %d", count);
		write_len = strlen(msg.wbuf);
		bytes = write(msg.fd, msg.wbuf, write_len);
		if (bytes < 0) {
			msg.fd = -1;
			rtems_message_queue_send(omid, &msg, sizeof(msg));
			printf("serial device write error: %d\n", errno);
		} else if (bytes != write_len) {
			printf("serial device failed to write all bytes: %d\n", bytes);
		} else {
			printf("serial device write: %s\n", msg.wbuf);
			rtems_message_queue_send(rmid, &msg, sizeof(msg));
		}
	}
	wtask_active = false;
}

static void
usb_serial_open_task(rtems_task_argument arg)
{
	rtems_status_code sc;
	struct usb_test_message msg;
	struct termios t;
	size_t size;
	int fd, iret;

	fd = -2;
	otask_active = true;
	kill_otask = false;
	while (!kill_otask) {
		sc = rtems_message_queue_receive(omid, &msg, &size, RTEMS_WAIT, RTEMS_MILLISECONDS_TO_TICKS(1000));
		if (sc == RTEMS_SUCCESSFUL) {
			if (fd >= 0) {
				close(fd);
				printf("serial device closed\n");
			}
			fd = msg.fd;
		}
		if (fd == -1) {
			fd = open("/dev/ttyU0", LIBIO_FLAGS_READ_WRITE);
			if (fd != -1) {
				printf("serial device opened\n");
				//get terminal settings
				iret = tcgetattr (fd, &t);
				assert(iret != -1);
				//set timeouts to zero for non-blocking mode
				t.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
				//t.c_iflag &= ~(BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP);
				//t.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP);
				t.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
				t.c_iflag |= IXON;    // Enable XON/XOFF flow control on output.
				t.c_cflag &= ~(CSIZE | PARENB);
				t.c_oflag &= ~(OPOST);
				t.c_cflag |= (CS8 | CREAD);
				t.c_cc[VMIN] = 0;
				t.c_cc[VTIME] = 0;
				//save settings
				iret = tcsetattr (fd, TCSANOW, &t);
				assert(iret != -1);
				msg.fd = fd;
				rtems_message_queue_send(wmid, &msg, sizeof(msg));
			}
		} else {
			msg.fd = fd;
			rtems_message_queue_send(wmid, &msg, sizeof(msg));
		}
	}
	otask_active = false;
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

	sc = rtems_message_queue_create(
		rtems_build_name ('M', 'U', 'W', 'R'),
		16,
		sizeof(struct usb_test_message),
		RTEMS_PRIORITY,
		&wmid
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
		rtems_build_name('U', 'S', 'B', 'W'),
		PRIO_WRITE,
		RTEMS_MINIMUM_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_FLOATING_POINT,
		&wid
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

	sc = rtems_task_start(rid, usb_serial_read_task, 0);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_start(oid, usb_serial_open_task, 0);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_start(wid, usb_serial_write_task, 0);
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
	kill_wtask = true;
	while (otask_active || rtask_active || wtask_active) {
		rtems_task_wake_after(RTEMS_MILLISECONDS_TO_TICKS(10));
	}

	sc = rtems_message_queue_delete(wmid);
	assert(sc == RTEMS_SUCCESSFUL);

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

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 32

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

SYSINIT_DRIVER_REFERENCE(uplcom, uhub);

#define CONFIGURE_SHELL_COMMANDS_INIT

#include <bsp/irq-info.h>

#include <rtems/netcmds-config.h>

#define CONFIGURE_SHELL_USER_COMMANDS \
	&bsp_interrupt_shell_command,       \
	&rtems_shell_STTY_Command,          \
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
