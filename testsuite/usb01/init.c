/**
 * @file
 *
 * @brief File system test.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>

#include <rtems.h>
#include <rtems/media.h>
#include <rtems/chain.h>
#include <rtems/libio_.h>
#include <rtems/shell.h>
#include <rtems/console.h>
#include <rtems/diskdevs.h>
#include <freebsd/bsd.h>

#include "test.h"

#define ASSERT_SC(sc) assert((sc) == RTEMS_SUCCESSFUL)

#define ASSERT_ENO(eno) assert((eno) == 0)

#define WORKER_COUNT 13

#define WORKER_EVENT RTEMS_EVENT_13

void test_file_system(unsigned index, const char *disk_path, const char *mount_path);

typedef struct {
  rtems_chain_node node;
  char *disk_or_partition_path;
  rtems_id task;
} worker;

static worker worker_table [WORKER_COUNT];

RTEMS_CHAIN_DEFINE_EMPTY(free_worker_list);

static const rtems_name WORKER_NAME = rtems_build_name('W', 'O', 'R', 'K');

static pthread_mutex_t worker_mutex;

static pthread_cond_t worker_changed;

static void add_worker_to_free_list(worker *w)
{
  rtems_chain_append(&free_worker_list, &w->node);
}

static worker *get_free_worker(void)
{
  return (worker *) rtems_chain_get(&free_worker_list);
}

static void activate_worker(char *disk_or_partition_path)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  worker *w = get_free_worker();

  if (w != NULL) {
    w->disk_or_partition_path = disk_or_partition_path;

    sc = rtems_event_send(w->task, WORKER_EVENT);
    ASSERT_SC(sc);
  } else {
    free(disk_or_partition_path);
  }
}

static worker *find_worker_for_disk_or_partition(const char *disk_or_partition_path)
{
  size_t i = 0;

  for (i = 0; i < WORKER_COUNT; ++i) {
    worker *w = &worker_table [i];

    if (w->disk_or_partition_path != NULL && strcmp(w->disk_or_partition_path, disk_or_partition_path) == 0) {
      return w;
    }
  }

  return NULL;
}

static void wait_for_worker_finished(const char *disk_or_partition_path)
{
  int eno = 0;
  worker *w = NULL;

  eno = pthread_mutex_lock(&worker_mutex);
  ASSERT_ENO(eno);

  w = find_worker_for_disk_or_partition(disk_or_partition_path);

  if (w != NULL) {
    while (w->disk_or_partition_path != NULL) {
      eno = pthread_cond_wait(&worker_changed, &worker_mutex);
      ASSERT_ENO(eno);
    }
  }

  eno = pthread_mutex_unlock(&worker_mutex);
  ASSERT_ENO(eno);
}

static rtems_status_code media_listener(rtems_media_event event, rtems_media_state state, const char *src, const char *dest, void *arg)
{
  rtems_status_code rsc = RTEMS_SUCCESSFUL;
  char *disk_or_partition_path = NULL;

  printf("media listener: event = %s, state = %s, src = %s", rtems_media_event_description(event), rtems_media_state_description(state), src);

  if (dest != NULL) {
    printf(", dest = %s", dest);
  }

  if (arg != NULL) {
    printf(", arg = %p\n", arg);
  }

  printf("\n");

  if (state == RTEMS_MEDIA_STATE_INQUIRY) {
    if (event == RTEMS_MEDIA_EVENT_MOUNT) {
      rsc = RTEMS_IO_ERROR;
    } else if (event == RTEMS_MEDIA_EVENT_PARTITION_DETACH || event == RTEMS_MEDIA_EVENT_DISK_DETACH) {
      wait_for_worker_finished(src);
    } else if (event == RTEMS_MEDIA_EVENT_UNMOUNT) {
      assert(false);
    }
  } else if (state == RTEMS_MEDIA_STATE_SUCCESS) {
    if (event == RTEMS_MEDIA_EVENT_PARTITION_ATTACH) {
      disk_or_partition_path = strdup(dest);
    }
  } else if (state == RTEMS_MEDIA_STATE_FAILED) {
    if (event == RTEMS_MEDIA_EVENT_PARTITION_INQUIRY) {
      disk_or_partition_path = strdup(src);
    }
  }

  if (disk_or_partition_path != NULL) {
    activate_worker(disk_or_partition_path);
  }

  return rsc;
}

static void worker_finished(worker *w)
{
  int eno = 0;

  eno = pthread_mutex_lock(&worker_mutex);
  ASSERT_ENO(eno);

  free(w->disk_or_partition_path);
  w->disk_or_partition_path = NULL;

  eno = pthread_cond_broadcast(&worker_changed);
  ASSERT_ENO(eno);

  eno = pthread_mutex_unlock(&worker_mutex);
  ASSERT_ENO(eno);
}

static void worker_task(rtems_task_argument arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  unsigned index = arg;
  worker *self = &worker_table [index];
  char mount_path [] = "/work/????";
  int rv = 0;

  rv = snprintf(mount_path, sizeof(mount_path), "/work/%u", index);
  assert(rv < (int) sizeof(mount_path));

  sc = rtems_libio_set_private_env();
  ASSERT_SC(sc);

  rv = rtems_mkdir(mount_path, S_IRWXU | S_IRWXG | S_IRWXO);
  assert(rv == 0);

  while (true) {
    rtems_event_set events = 0;

    sc = rtems_event_receive(WORKER_EVENT, RTEMS_EVENT_ALL | RTEMS_WAIT, RTEMS_NO_TIMEOUT, &events);
    ASSERT_SC(sc);

    test_file_system(index, self->disk_or_partition_path, mount_path);

    worker_finished(self);

    add_worker_to_free_list(self);
  }
}

static const char mac_address [6] = { 0x00, 0x1a, 0xf1, 0x00, 0x07, 0xa4 };

static void Init(rtems_task_argument arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  int rv = 0;
  int eno = 0;
  rtems_id id = RTEMS_ID_NONE;
  size_t i = 0;

  eno = pthread_mutex_init(&worker_mutex, NULL);
  ASSERT_ENO(eno);

  eno = pthread_cond_init(&worker_changed, NULL);
  ASSERT_ENO(eno);

  for (i = 0; i < WORKER_COUNT; ++i) {
    worker *w = &worker_table [i];

    sc = rtems_task_create(
      WORKER_NAME,
      95 + 10 * (i % 4),
      32 * 1024,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    ASSERT_SC(sc);

    w->task = id;
    add_worker_to_free_list(w);

    sc = rtems_task_start(id, worker_task, i);
    ASSERT_SC(sc);
  }

  sc = rtems_disk_io_initialize();
  ASSERT_SC(sc);

  sc = rtems_media_initialize();
  ASSERT_SC(sc);

  sc = rtems_media_listener_add(media_listener, NULL);
  ASSERT_SC(sc);

  sc = rtems_media_server_initialize(200, 32 * 1024, RTEMS_DEFAULT_MODES, RTEMS_DEFAULT_ATTRIBUTES);
  ASSERT_SC(sc);

  sc = rtems_bsd_initialize_with_interrupt_server();
  ASSERT_SC(sc);

  rtems_bsd_shell_initialize();

  sc = rtems_shell_init(
    "SHLL",
    16 * 1024,
    10,
    CONSOLE_DEVICE_NAME,
    false,
    true,
    NULL
  );
  ASSERT_SC(sc);

  exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_FILESYSTEM_IMFS
#define CONFIGURE_FILESYSTEM_DOSFS

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 32

#define CONFIGURE_BDBUF_BUFFER_MIN_SIZE 512
#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE 512
#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE (WORKER_COUNT * 512)
#define CONFIGURE_BDBUF_MAX_READ_AHEAD_BLOCKS 0

#define CONFIGURE_MAXIMUM_TASKS 64
#define CONFIGURE_MAXIMUM_DRIVERS 4
#define CONFIGURE_MAXIMUM_SEMAPHORES 64
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES 4
#define CONFIGURE_MAXIMUM_TIMERS 64
#define CONFIGURE_MAXIMUM_PERIODS 4
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1
#define CONFIGURE_MAXIMUM_BARRIERS 1

#define CONFIGURE_MAXIMUM_POSIX_MUTEXES 32
#define CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES 64

#define CONFIGURE_EXTRA_TASK_STACKS (1 * 1024 * 1024)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_STACK_CHECKER_ENABLED

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

#define CONFIGURE_SHELL_COMMANDS_INIT

#include "demo-shell.h"
#include "demo-shell-block-devices.h"
#include <rtems/shellconfig.h>

#define USB_SYSINIT_INIT

#include "usb-sysinit.h"
