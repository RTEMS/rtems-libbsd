/**
 * @file
 *
 * @brief Different event devices (evdev) are tested in different RTEMS tasks.
 */

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
#include <sys/stat.h>
#include <dev/evdev/input.h>
#include <dev/evdev/evdev.h>
#include <fcntl.h>
#include <dirent.h>

#include <rtems/console.h>
#include <rtems/shell.h>
#include <rtems/bsd/bsd.h>

#define TEST_NAME "LIBBSD EVDEV"
#define TEST_STATE_USER_INPUT 1
#include <rtems/test.h>

#define PRIO_OPEN           (RTEMS_MAXIMUM_PRIORITY - 12)
#define PRIO_KEYBOARD       (RTEMS_MAXIMUM_PRIORITY - 11)
#define PRIO_MOUSE          (RTEMS_MAXIMUM_PRIORITY - 10)
#define PRIO_TOUCH          (RTEMS_MAXIMUM_PRIORITY - 9)
#define MAX_ACTIVE_DEVICES  256

#define DEVICE_INPUT_DIR "/dev/input"

#define LONG_BITS (sizeof(long) * 8)
#define LONG_FIELD_SIZE(bits) ((bits / LONG_BITS) + 1)

struct evdev_test_message {
	int fd;
	char device[256];
};

static rtems_id oid, omid, kid, kmid, mid, mmid, tid, tmid, psema;
static volatile bool kill_otask, kill_ktask, kill_mtask, kill_ttask;
static volatile bool otask_active, ktask_active, mtask_active, ttask_active;
static char* active_devices[MAX_ACTIVE_DEVICES];
static int active_device_count = 0;

static void evdev_keyboard_task(rtems_task_argument arg);
static void evdev_mouse_task(rtems_task_argument arg);
static void evdev_touch_task(rtems_task_argument arg);

static void
add_active_device(char* dev)
{
	rtems_semaphore_obtain(psema, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	if (active_device_count < MAX_ACTIVE_DEVICES) {
		active_devices[active_device_count++] = strdup(dev);
	}
	rtems_semaphore_release(psema);
}

static void
remove_active_device(char* dev)
{
	int ii;
	bool found = false;

	rtems_semaphore_obtain(psema, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	for (ii = 0; ii < active_device_count; ii++) {
		if (strcmp(dev, active_devices[ii]) == 0) {
			free(active_devices[ii]);
			found = true;
			break;
		}
	}
	if (found) {
		ii++;
		for (; ii < active_device_count; ii++) {
			active_devices[ii] = active_devices[ii + 1];
		}
		active_device_count--;
	}
	rtems_semaphore_release(psema);
}

static bool
check_device_active(char* dev)
{
	int ii;

	rtems_semaphore_obtain(psema, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	for (ii = 0; ii < active_device_count; ii++) {
		if (strcmp(dev, active_devices[ii]) == 0) {
			rtems_semaphore_release(psema);
			return true;
		}
	}
	rtems_semaphore_release(psema);
	return false;
}

static void
delete_active_devices(void)
{
	int ii;

	rtems_semaphore_obtain(psema, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	for (ii = 0; ii < active_device_count; ii++) {
		free(active_devices[ii]);
	}
	rtems_semaphore_release(psema);
}

static void
print_message(char *fmt, ...)
{
	va_list ap;

	rtems_semaphore_obtain(psema, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	rtems_semaphore_release(psema);
}

static int
get_input_device_count(void)
{
	DIR *dir;
	struct dirent *entry;
	struct stat sbuf;
	int file_count = 0;
	int rv;
	char fname[256];

	if ((dir = opendir(DEVICE_INPUT_DIR)) != NULL) {
		/* print all the files and directories within directory */
		while ((entry = readdir(dir)) != NULL) {
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;
			strcpy(fname, DEVICE_INPUT_DIR);
			strcat(fname, "/");
			strcat(fname, entry->d_name);
			rv = stat(fname, &sbuf);
			assert(rv != -1);
			if (S_ISCHR(sbuf.st_mode)) {
				file_count++;
			}
		}
		closedir (dir);
	}
	return file_count;
}

static char**
get_input_device_names(int file_count)
{
	DIR *dir;
	struct dirent *entry;
	struct stat sbuf;
	char** s_arr = NULL;
	int current_count = 0;
	int rv;
	char fname[256];

	if (file_count > 0) {
		s_arr = malloc(file_count * sizeof(char*));
		if ((dir = opendir(DEVICE_INPUT_DIR)) != NULL) {
			/* print all the files and directories within directory */
			while ((entry = readdir(dir)) != NULL) {
				if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
					continue;
				strcpy(fname, DEVICE_INPUT_DIR);
				strcat(fname, "/");
				strcat(fname, entry->d_name);
				rv = stat(fname, &sbuf);
				assert(rv != -1);
				if (S_ISCHR(sbuf.st_mode)) {
					s_arr[current_count] = malloc((strlen(fname) + 1) * sizeof(char));
					strcpy(s_arr[current_count], fname);
					current_count++;
				}
			}
			closedir (dir);
		}
	}
	return s_arr;
}

static bool
test_bit(long bit, const long *field)
{
	return (field[bit / LONG_BITS] >> bit % LONG_BITS) & 1;
}

static int
check_device_type(char* device)
{
	rtems_status_code sc;
	struct evdev_test_message msg;
	long bitsAbs[LONG_FIELD_SIZE(ABS_CNT)];
	long bitsKey[LONG_FIELD_SIZE(KEY_CNT)];
	long bitsRel[LONG_FIELD_SIZE(REL_CNT)];
	int ret = -1;
	int rv;
	int fd;

	if (!check_device_active(device)) {
		fd = open(device, O_RDONLY);
		if (fd != -1) {
			memset(bitsAbs, 0, sizeof(bitsAbs));
			memset(bitsKey, 0, sizeof(bitsKey));
			memset(bitsRel, 0, sizeof(bitsRel));

			rv = ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(bitsAbs)), bitsAbs);
			assert(rv != -1);
			rv = ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(bitsKey)), bitsKey);
			assert(rv != -1);
			rv = ioctl(fd, EVIOCGBIT(EV_REL, sizeof(bitsRel)), bitsRel);
			assert(rv != -1);

			close(fd);

			if (test_bit(KEY_Q, bitsKey)) {
				printf("Found keyboard(%d) at %s\n", fd, device);
				add_active_device(device);
				strcpy(msg.device, device);
				sc = rtems_message_queue_send(kmid, &msg, sizeof(msg));
				assert(sc == RTEMS_SUCCESSFUL);
				ret = 0;
			}
			if (test_bit(REL_X, bitsRel) && test_bit(REL_Y, bitsRel) && test_bit(BTN_MOUSE, bitsKey)) {
				printf("Found mouse(%d) at %s\n", fd, device);
				add_active_device(device);
				strcpy(msg.device, device);
				sc = rtems_message_queue_send(mmid, &msg, sizeof(msg));
				assert(sc == RTEMS_SUCCESSFUL);
				ret = 0;
			}
			if (test_bit(ABS_X, bitsAbs) && test_bit(ABS_Y, bitsAbs)) {
				if (test_bit(BTN_TOOL_FINGER, bitsKey)) {
					printf("Found touchpad(%d) at %s\n", fd, device);
					add_active_device(device);
					ret = 0;
				} else if (test_bit(BTN_TOUCH, bitsKey)) {
					printf("Found touchscreen(%d) at %s\n", fd, device);
					add_active_device(device);
					strcpy(msg.device, device);
					sc = rtems_message_queue_send(tmid, &msg, sizeof(msg));
					assert(sc == RTEMS_SUCCESSFUL);
					ret = 0;
				} else if ((test_bit(BTN_STYLUS, bitsKey) || test_bit(BTN_TOOL_PEN, bitsKey))) {
					printf("Found tablet(%d) at %s\n", fd, device);
					add_active_device(device);
					ret = 0;
				}
			} else if (test_bit(ABS_MT_POSITION_X, bitsAbs) && test_bit(ABS_MT_POSITION_Y, bitsAbs)) {
				printf("Found new-style touchscreen(%d) at %s\n", fd, device);
				add_active_device(device);
				strcpy(msg.device, device);
				sc = rtems_message_queue_send(tmid, &msg, sizeof(msg));
				assert(sc == RTEMS_SUCCESSFUL);
				ret = 0;
			}
			if (test_bit(BTN_A, bitsKey) || test_bit(BTN_TRIGGER, bitsKey) || test_bit(ABS_RX, bitsAbs)) {
				printf("Found joystick/gamepad(%d) at %s\n", fd, device);
				add_active_device(device);
				ret = 0;
			}
			if (ret == -1) {
				printf("Unknown device(%d) at %s\n", fd, device);
			}
		}
	}
	return ret;
}

static void
scan_for_devices(void)
{
	int device_count;
	char** s_arr;
	int ii;

	device_count = get_input_device_count();
	if (device_count > 0) {
		s_arr = get_input_device_names(device_count);
		if (s_arr != NULL) {
			for (ii = 0; ii < device_count; ii++) {
				check_device_type(s_arr[ii]);
				free(s_arr[ii]);
			}
			free(s_arr);
		}
	}
}

static void
evdev_scan_task(rtems_task_argument arg)
{
	rtems_status_code sc;
	struct evdev_test_message msg;
	size_t size;

	otask_active = true;
	kill_otask = false;
	while (!kill_otask) {
		sc = rtems_message_queue_receive(omid, &msg, &size, RTEMS_WAIT, RTEMS_MILLISECONDS_TO_TICKS(1000));
		if (sc == RTEMS_SUCCESSFUL) {
			while (!kill_otask) {
				scan_for_devices();
				rtems_task_wake_after(RTEMS_MILLISECONDS_TO_TICKS(1000));
			}
		}
	}
	otask_active = false;
	rtems_task_delete(RTEMS_SELF);
}

static void
process_keyboard_input_event(struct input_event *data)
{
	if (data->type != EV_KEY)
		return;
	print_message("Keyboard: %d,%d\n", data->code, data->value);
}

static void
evdev_keyboard_task(rtems_task_argument arg)
{
	int fd, bytes, n, i, rv, grab;
	struct input_event buffer[32];
	rtems_status_code sc;
	struct evdev_test_message msg;
	size_t size;

	ktask_active = true;
	kill_ktask = false;
	while (!kill_ktask) {
		sc = rtems_message_queue_receive(kmid, &msg, &size, RTEMS_WAIT, RTEMS_MILLISECONDS_TO_TICKS(1000));
		if (sc == RTEMS_SUCCESSFUL) {
			fd = open(msg.device, O_RDONLY | O_NONBLOCK);
			if (fd != -1) {
				grab = 1;
				rv = ioctl(fd, EVIOCGRAB, &grab);
				assert(rv == 0);
				while (!kill_ttask) {
					n = 0;
					while (!kill_ttask) {
						bytes = read(fd, (char*)buffer + n, sizeof(buffer) - n);
						if (bytes == 0) {
							print_message("evdevkeyboard: Got EOF from the input device\n");
						} else if (bytes < 0) {
							if (errno != EINTR && errno != EAGAIN) {
								print_message("evdevkeyboard: Could not read from input device\n");
								goto err;
							}
							rtems_task_wake_after(RTEMS_MILLISECONDS_TO_TICKS(10));
						} else {
							n += bytes;
							if (n % sizeof(buffer[0]) == 0)
								break;
						}
					}
					n /= sizeof(buffer[0]);

					for (i = 0; i < n; ++i)
						process_keyboard_input_event(&buffer[i]);
				}
err:
				close(fd);
			}
			remove_active_device(msg.device);
		}
	}
	ktask_active = false;
	rtems_task_delete(RTEMS_SELF);
}

static void
process_mouse_input_event(struct input_event *data)
{
	if (data->type == EV_ABS) {
		/*Touchpads: store the absolute position for now, will calculate a relative one later.*/
		if (data->code == ABS_X) {
			print_message("Mouse ABS: X = %d\n", data->value);
		} else if (data->code == ABS_Y) {
			print_message("Mouse ABS: Y = %d\n", data->value);
		}
	} else if (data->type == EV_REL) {
		if (data->code == REL_X) {
			print_message("Mouse REL: X = %d\n", data->value);
		} else if (data->code == REL_Y) {
			print_message("Mouse REL: Y = %d\n", data->value);
		} else if (data->code == ABS_WHEEL) { /* vertical scroll */
			print_message("Mouse REL: W = %d\n", data->value);
		} else if (data->code == ABS_THROTTLE) { /* horizontal scroll */
			print_message("Mouse REL: T = %d\n", data->value);
		}
	} else if (data->type == EV_KEY && data->code == BTN_TOUCH) {
		print_message("Mouse Key: B = %d\n", data->value);
	} else if (data->type == EV_KEY && data->code >= BTN_LEFT && data->code <= BTN_JOYSTICK) {
		print_message("Mouse Key: %d,%d\n", data->code, data->value);
	} else if (data->type == EV_SYN && data->code == SYN_REPORT) {
		print_message("Mouse Sync Report: %d\n", data->value);
	} else if (data->type == EV_MSC && data->code == MSC_SCAN) {
		print_message("Mouse MSC: %d,%d\n", data->code, data->value);
	}
}

static void
evdev_mouse_task(rtems_task_argument arg)
{
	int fd, bytes, n, i, rv, grab;
	struct input_event buffer[32];
	rtems_status_code sc;
	struct evdev_test_message msg;
	size_t size;

	mtask_active = true;
	kill_mtask = false;
	while (!kill_mtask) {
		sc = rtems_message_queue_receive(mmid, &msg, &size, RTEMS_WAIT, RTEMS_MILLISECONDS_TO_TICKS(1000));
		if (sc == RTEMS_SUCCESSFUL) {
			fd = open(msg.device, O_RDONLY | O_NONBLOCK);
			if (fd != -1) {
				grab = 1;
				rv = ioctl(fd, EVIOCGRAB, &grab);
				assert(rv == 0);
				while (!kill_ttask) {
					n = 0;
					while (!kill_ttask) {
						bytes = read(fd, (char*)buffer + n, sizeof(buffer) - n);
						if (bytes == 0) {
							print_message("evdevmouse: Got EOF from the input device\n");
						} else if (bytes < 0) {
							if (errno != EINTR && errno != EAGAIN) {
								print_message("evdevmouse: Could not read from input device\n");
								goto err;
							}
							rtems_task_wake_after(RTEMS_MILLISECONDS_TO_TICKS(10));
						} else {
							n += bytes;
							if (n % sizeof(buffer[0]) == 0)
								break;
						}
					}
					n /= sizeof(buffer[0]);

					for (i = 0; i < n; ++i)
						process_mouse_input_event(&buffer[i]);
				}
err:
				close(fd);
			}
			remove_active_device(msg.device);
		}
	}
	mtask_active = false;
	rtems_task_delete(RTEMS_SELF);
}

static void
process_touch_input_event(struct input_event *data)
{
	if (data->type == EV_ABS) {
		if (data->code == ABS_MT_POSITION_X || (data->code == ABS_X)) {
			print_message("Touch ABS: X = %d\n", data->value);
		} else if (data->code == ABS_MT_POSITION_Y || (data->code == ABS_Y)) {
			print_message("Touch ABS: Y = %d\n", data->value);
		} else if (data->code == ABS_MT_TRACKING_ID) {
			print_message("Touch ABS: T = %d\n", data->value);
		} else if (data->code == ABS_MT_TOUCH_MAJOR) {
			print_message("Touch ABS: TM = %d\n", data->value);
		} else if (data->code == ABS_PRESSURE) {
			print_message("Touch ABS: P = %d\n", data->value);
		} else if (data->code == ABS_MT_SLOT) {
			print_message("Touch ABS: S = %d\n", data->value);
		}
	} else if (data->type == EV_KEY) {
		if (data->code == BTN_TOUCH)
			print_message("Touch Key: %d\n", data->value);
	} else if (data->type == EV_SYN && data->code == SYN_REPORT) {
		print_message("Touch Sync Report: %d\n", data->value);
	}
}

static void
evdev_touch_task(rtems_task_argument arg)
{
	int fd, n, i, bytes, rv, grab;
	struct input_event buffer[32];
	rtems_status_code sc;
	struct evdev_test_message msg;
	size_t size;

	ttask_active = true;
	kill_ttask = false;
	while (!kill_ttask) {
		sc = rtems_message_queue_receive(tmid, &msg, &size, RTEMS_WAIT, RTEMS_MILLISECONDS_TO_TICKS(1000));
		if (sc == RTEMS_SUCCESSFUL) {
			fd = open(msg.device, O_RDONLY | O_NONBLOCK);
			if (fd != -1) {
				grab = 1;
				rv = ioctl(fd, EVIOCGRAB, &grab);
				assert(rv == 0);
				while (!kill_ttask) {
					n = 0;
					while (!kill_ttask) {
						bytes = read(fd, (char*)buffer + n, sizeof(buffer) - n);
						if (bytes == 0) {
							print_message("evdevtouch: Got EOF from the input device\n");
						} else if (bytes < 0) {
							if (errno != EINTR && errno != EAGAIN) {
								print_message("evdevtouch: Could not read from input device\n");
								goto err;
							}
							rtems_task_wake_after(RTEMS_MILLISECONDS_TO_TICKS(10));
						} else {
							n += bytes;
							if (n % sizeof(struct input_event) == 0)
								break;
						}
					}
					n /= sizeof(struct input_event);

					for (i = 0; i < n; ++i)
						process_touch_input_event(&buffer[i]);
				}
err:
				close(fd);
			}
			remove_active_device(msg.device);
		}
	}
	ttask_active = false;
	rtems_task_delete(RTEMS_SELF);
}

static void
Init(rtems_task_argument arg)
{
	rtems_status_code sc;
	struct evdev_test_message msg;

	(void) arg;
	rtems_test_begin(TEST_NAME, TEST_STATE);

	sc = rtems_semaphore_create(
		rtems_build_name('E', 'V', 'D', 'S'),
		1,
		RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
		RTEMS_NO_PRIORITY,
		&psema);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_message_queue_create(
		rtems_build_name ('E', 'V', 'O', 'M'),
		16,
		sizeof(struct evdev_test_message),
		RTEMS_PRIORITY,
		&omid
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_message_queue_create(
		rtems_build_name ('E', 'V', 'K', 'M'),
		16,
		sizeof(struct evdev_test_message),
		RTEMS_PRIORITY,
		&kmid
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_message_queue_create(
		rtems_build_name ('E', 'V', 'M', 'M'),
		16,
		sizeof(struct evdev_test_message),
		RTEMS_PRIORITY,
		&mmid
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_message_queue_create(
		rtems_build_name ('E', 'V', 'T', 'M'),
		16,
		sizeof(struct evdev_test_message),
		RTEMS_PRIORITY,
		&tmid
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_create(
		rtems_build_name('E', 'V', 'D', 'K'),
		PRIO_KEYBOARD,
		RTEMS_MINIMUM_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_FLOATING_POINT,
		&kid
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_create(
		rtems_build_name('E', 'V', 'D', 'M'),
		PRIO_MOUSE,
		RTEMS_MINIMUM_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_FLOATING_POINT,
		&mid
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_create(
		rtems_build_name('E', 'V', 'D', 'T'),
		PRIO_TOUCH,
		RTEMS_MINIMUM_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_FLOATING_POINT,
		&tid
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_create(
		rtems_build_name('E', 'V', 'D', 'O'),
		PRIO_OPEN,
		RTEMS_MINIMUM_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_FLOATING_POINT,
		&oid
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_start(oid, evdev_scan_task, 0);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_start(kid, evdev_keyboard_task, 0);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_start(mid, evdev_mouse_task, 0);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_start(tid, evdev_touch_task, 0);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_bsd_initialize();
	assert(sc == RTEMS_SUCCESSFUL);

	msg.fd = -1;
	rtems_message_queue_send(omid, &msg, sizeof(msg));

	sc = rtems_shell_init("SHLL", 16 * 1024, 1, CONSOLE_DEVICE_NAME,
	    false, true, NULL);
	assert(sc == RTEMS_SUCCESSFUL);

	kill_otask = true;
	kill_ktask = true;
	kill_mtask = true;
	kill_ttask = true;
	while (otask_active || ktask_active || mtask_active || ttask_active) {
		rtems_task_wake_after(RTEMS_MILLISECONDS_TO_TICKS(10));
	}

	delete_active_devices();

	sc = rtems_message_queue_delete(omid);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_message_queue_delete(kmid);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_message_queue_delete(mmid);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_message_queue_delete(tmid);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_semaphore_delete(psema);
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

SYSINIT_DRIVER_REFERENCE(ukbd, uhub);
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
