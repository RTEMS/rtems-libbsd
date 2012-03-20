/**
 * @file
 *
 * @brief File system test implementation.
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

#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/dosfs.h>

#include "test.h"

#define ASSERT_SC(sc) assert((sc) == RTEMS_SUCCESSFUL)

/**
 * @brief Test states.
 *
 * digraph {
 *   INIT -> MOUNT;
 *   MOUNT -> INIT_ROOT;
 *   MOUNT -> FORMAT;
 *   INIT_ROOT -> CHOOSE_DIR_ACTION
 *   INIT_ROOT -> ERROR;
 *   CHOOSE_DIR_ACTION -> DIR_OPEN;
 *   CHOOSE_DIR_ACTION -> DIR_CLOSE;
 *   CHOOSE_DIR_ACTION -> DIR_CREATE;
 *   CHOOSE_DIR_ACTION -> DIR_DELETE;
 *   CHOOSE_DIR_ACTION -> DIR_SLEEP;
 *   DIR_OPEN -> CHOOSE_DIR_ACTION;
 *   DIR_OPEN -> CHOOSE_FILE_ACTION;
 *   DIR_OPEN -> ERROR;
 *   DIR_CLOSE -> CHOOSE_DIR_ACTION;
 *   DIR_CLOSE -> ERROR;
 *   DIR_CREATE -> CHOOSE_DIR_ACTION;
 *   DIR_CREATE -> DIR_DELETE;
 *   DIR_CREATE -> ERROR;
 *   DIR_DELETE -> CHOOSE_DIR_ACTION;
 *   DIR_DELETE -> ERROR;
 *   DIR_SLEEP -> CHOOSE_DIR_ACTION;
 *   CHOOSE_FILE_ACTION -> FILE_CLOSE;
 *   CHOOSE_FILE_ACTION -> FILE_APPEND;
 *   CHOOSE_FILE_ACTION -> FILE_SLEEP;
 *   FILE_CLOSE -> CHOOSE_DIR_ACTION;
 *   FILE_CLOSE -> ERROR;
 *   FILE_APPEND -> CHOOSE_FILE_ACTION;
 *   FILE_APPEND -> ERROR;
 *   FILE_SLEEP -> CHOOSE_FILE_ACTION;
 *   ERROR -> FORMAT;
 *   FORMAT -> MOUNT;
 *   FORMAT -> FINISH;
 * }
 */
typedef enum {
  CHOOSE_DIR_ACTION,
  CHOOSE_FILE_ACTION,
  DIR_CLOSE,
  DIR_CREATE,
  DIR_DELETE,
  DIR_OPEN,
  DIR_SLEEP,
  ERROR,
  FILE_APPEND,
  FILE_CLOSE,
  FILE_SLEEP,
  FINISH,
  FORMAT,
  INIT,
  INIT_ROOT,
  MOUNT
} test_state;

typedef struct {
  DIR *dir;
  unsigned level;
  unsigned content_count;
  int fd;
  int eno;
} fs_state;

static test_state do_format(unsigned index, fs_state *fs, const char *disk_path)
{
  int rv = 0;

  printf("[%02u]: format: %s\n", index, disk_path);

  rv = msdos_format(disk_path, NULL);
  if (rv == 0) {
    return MOUNT;
  } else {
    fs->eno = errno;

    return FINISH;
  }
}

static unsigned get_bucket(unsigned count)
{
  long unsigned unit = (1U << 31) / count;
  long unsigned bucket = (long unsigned) lrand48() / unit;

  if (bucket != count) {
    return bucket;
  } else {
    return bucket - 1;
  }
}

static test_state do_choose_dir_action(void)
{
  switch (get_bucket(8)) {
    case 0:
    case 1:
      return DIR_CLOSE;
    case 2:
    case 3:
      return DIR_CREATE;
    case 4:
    case 5:
      return DIR_OPEN;
    case 6:
      return DIR_DELETE;
    case 7:
      return DIR_SLEEP;
    default:
      assert(false);
      break;
  }
}

static test_state do_choose_file_action(void)
{
  switch (get_bucket(3)) {
    case 0:
      return FILE_CLOSE;
    case 1:
      return FILE_SLEEP;
    case 2:
      return FILE_APPEND;
    default:
      assert(false);
      break;
  }
}

static bool is_normal_entry(const char *entry_name)
{
  return strcmp(entry_name, ".") != 0 && strcmp(entry_name, "..") != 0;
}

static bool open_dir(fs_state *fs, const char *dir_path)
{
  int rv = 0;
  bool change_dir = true;

  if (dir_path == NULL) {
    if (fs->level > 1) {
      rv = chdir("..");
      if (rv != 0) {
        fs->eno = errno;

        return false;
      }

      --fs->level;
    } else {
      return true;
    }
    dir_path = ".";
    change_dir = false;
  }

  if (fs->dir != NULL) {
    rv = closedir(fs->dir);
    if (rv != 0) {
      fs->eno = errno;

      return false;
    }
  }

  fs->content_count = 0;
  fs->dir = opendir(dir_path);

  if (fs->dir != NULL) {
    struct dirent *de = NULL;

    rewinddir(fs->dir);
    while ((de = readdir(fs->dir)) != NULL) {
      if (is_normal_entry(de->d_name)) {
        ++fs->content_count;
      }
    }
  } else {
    fs->eno = errno;

    return false;
  }

  if (change_dir) {
    rv = chdir(dir_path);
    if (rv != 0) {
      fs->eno = errno;

      return false;
    }

    ++fs->level;
  }

  return true;
}

static char *get_dir_entry(fs_state *fs, bool *is_dir)
{
  int rv = 0;
  char *entry_name = NULL;

  if (fs->content_count > 0) {
    struct dirent *de = NULL;
    unsigned bucket = get_bucket(fs->content_count);
    unsigned i = 0;

    rewinddir(fs->dir);
    while ((de = readdir(fs->dir)) != NULL) {
      if (is_normal_entry(de->d_name)) {
        if (i != bucket) {
          ++i;
        } else {
          break;
        }
      }
    }

    if (de != NULL) {
      struct stat st;

      rv = stat(de->d_name, &st);
      if (rv == 0) {
        *is_dir = S_ISDIR(st.st_mode);

        entry_name = strdup(de->d_name);
      }
    }
  }

  return entry_name;
}


static test_state do_init_root(unsigned index, fs_state *fs, const char *mount_path)
{
  printf("[%02u]: init root: %s\n", index, mount_path);

  if (open_dir(fs, mount_path)) {
    return CHOOSE_DIR_ACTION;
  } else {
    return ERROR;
  }
}

static test_state do_dir_close(unsigned index, fs_state *fs)
{
  if (fs->level > 1) {
    printf("[%02u]: close dir\n", index);

    if (open_dir(fs, NULL)) {
      return CHOOSE_DIR_ACTION;
    } else {
      return ERROR;
    }
  } else {
    return CHOOSE_DIR_ACTION;
  }
}

static test_state do_dir_create(unsigned index, fs_state *fs)
{
  int rv = 0;
  test_state state = ERROR;
  long number = lrand48();
  char name [64];

  snprintf(name, sizeof(name), "%li", number);

  if ((number % 2) == 0) {
    printf("[%02u]: create file: %s\n", index, name);

    rv = open(name, O_RDONLY | O_CREAT, 0777);

    if (rv >= 0) {
      rv = close(rv);

      if (rv == 0) {
        state = CHOOSE_DIR_ACTION;
      }
    } else if (errno == ENOSPC) {
      state = DIR_DELETE;
    } else {
      fs->eno = errno;
    }
  } else {
    printf("[%02u]: create dir: %s\n", index, name);

    rv = mkdir(name, 0777);

    if (rv == 0) {
      ++fs->content_count;

      state = CHOOSE_DIR_ACTION;
    } else if (errno == EEXIST) {
      state = CHOOSE_DIR_ACTION;
    } else if (errno == ENOSPC) {
      state = DIR_DELETE;
    } else {
      fs->eno = errno;
    }
  }

  return state;
}

static test_state do_dir_delete(unsigned index, fs_state *fs)
{
  int rv = 0;
  test_state state = ERROR;

  if (fs->content_count > 0) {
    bool is_dir = false;
    char *dir_entry_path = get_dir_entry(fs, &is_dir);

    if (dir_entry_path != NULL) {
      if (is_dir) {
        printf("[%02u]: remove dir: %s\n", index, dir_entry_path);

        rv = rmdir(dir_entry_path);
      } else {
        printf("[%02u]: remove file: %s\n", index, dir_entry_path);

        rv = unlink(dir_entry_path);
      }

      if (rv == 0) {
        --fs->content_count;

        state = CHOOSE_DIR_ACTION;
      } else if (errno == ENOTEMPTY) {
        state = CHOOSE_DIR_ACTION;
      } else {
        fs->eno = errno;
      }

      free(dir_entry_path);
    }
  } else {
    state = CHOOSE_DIR_ACTION;
  }

  return state;
}

static test_state do_dir_open(unsigned index, fs_state *fs)
{
  test_state state = ERROR;

  if (fs->content_count > 0) {
    bool is_dir = false;
    char *dir_entry_path = get_dir_entry(fs, &is_dir);

    if (dir_entry_path != NULL) {
      if (is_dir) {
        printf("[%02u]: open dir: %s\n", index, dir_entry_path);

        if (open_dir(fs, dir_entry_path)) {
          state = CHOOSE_DIR_ACTION;
        }
      } else {
        printf("[%02u]: open file: %s\n", index, dir_entry_path);

        fs->fd = open(dir_entry_path, O_WRONLY | O_APPEND);

        if (fs->fd >= 0) {
          state = CHOOSE_FILE_ACTION;
        } else {
          fs->eno = errno;
        }
      }

      free(dir_entry_path);
    }
  } else {
    state = CHOOSE_DIR_ACTION;
  }

  return state;
}

static test_state do_file_append(unsigned index, fs_state *fs)
{
  static const char buf [511];
  test_state state = ERROR;
  ssize_t n = 0;

  printf("[%02u]: append to file\n", index);

  n = write(fs->fd, buf, sizeof(buf));

  if (n == (ssize_t) sizeof(buf)) {
    state = CHOOSE_FILE_ACTION;
  } else if (n == -1) {
    fs->eno = errno;
  }

  return state;
}

static test_state do_file_close(unsigned index, fs_state *fs)
{
  int rv = 0;
  test_state state = ERROR;

  printf("[%02u]: close file\n", index);

  rv = close(fs->fd);
  fs->fd = -1;

  if (rv == 0) {
    state = CHOOSE_DIR_ACTION;
  } else {
    fs->eno = errno;
  }

  return state;
}

static test_state do_sleep(unsigned index, test_state state)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_interval ms = ((rtems_interval) get_bucket(10) + 1) * 100;
  rtems_interval interval = ms / rtems_configuration_get_milliseconds_per_tick();

  printf("[%02u]: sleep: %" PRIu32 " ms\n", index, ms);

  sc = rtems_task_wake_after(interval);
  ASSERT_SC(sc);

  return state;
}

static test_state do_mount(unsigned index, const char *disk_path, const char *mount_path)
{
  int rv = 0;

  printf("[%02u]: mount: %s -> %s\n", index, disk_path, mount_path);

  rv = mount_and_make_target_path(
    disk_path,
    mount_path,
    RTEMS_FILESYSTEM_TYPE_DOSFS,
    RTEMS_FILESYSTEM_READ_WRITE,
    NULL
  );
  if (rv == 0) {
    return INIT_ROOT;
  } else {
    return FORMAT;
  }
}

static test_state do_error(unsigned index, fs_state *fs, const char *mount_path)
{
  int rv = 0;

  if (fs->eno > 0) {
    printf("[%02u]: error: %s\n", index, strerror(fs->eno));
  } else {
    printf("[%02u]: error\n", index);
  }
  fs->eno = 0;

  if (fs->fd >= 0) {
    close(fs->fd);
    fs->fd = -1;
  }

  if (fs->dir != NULL) {
    closedir(fs->dir);
    fs->dir = NULL;
  }

  chdir("/");
  fs->level = 0;

  rv = unmount(mount_path);
  if (rv == 0) {
    return FORMAT;
  } else {
    return FINISH;
  }
}

void test_file_system(unsigned index, const char *disk_path, const char *mount_path)
{
  test_state state = INIT;
  fs_state fs = {
    .dir = NULL,
    .level = 0,
    .content_count = 0,
    .fd = -1,
    .eno = 0
  };

  printf("[%02u]: start\n", index);

  while (state != FINISH) {
    switch (state) {
      case CHOOSE_DIR_ACTION:
        state = do_choose_dir_action();
        break;
      case CHOOSE_FILE_ACTION:
        state = do_choose_file_action();
        break;
      case DIR_CLOSE:
        state = do_dir_close(index, &fs);
        break;
      case DIR_CREATE:
        state = do_dir_create(index, &fs);
        break;
      case DIR_DELETE:
        state = do_dir_delete(index, &fs);
        break;
      case DIR_OPEN:
        state = do_dir_open(index, &fs);
        break;
      case DIR_SLEEP:
        state = do_sleep(index, CHOOSE_DIR_ACTION);
        break;
      case ERROR:
        state = do_error(index, &fs, mount_path);
        break;
      case FILE_APPEND:
        state = do_file_append(index, &fs);
        break;
      case FILE_CLOSE:
        state = do_file_close(index, &fs);
        break;
      case FILE_SLEEP:
        state = do_sleep(index, CHOOSE_FILE_ACTION);
        break;
      case FORMAT:
        state = do_format(index, &fs, disk_path);
        break;
      case INIT:
        state = MOUNT;
        break;
      case INIT_ROOT:
        state = do_init_root(index, &fs, mount_path);
        break;
      case MOUNT:
        state = do_mount(index, disk_path, mount_path);
        break;
      default:
        assert(false);
        break;
    }
  }

  printf("[%02u]: finish\n", index);
}
