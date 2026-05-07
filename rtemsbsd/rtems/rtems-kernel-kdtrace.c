/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief Kernel Dynamic Trace support
 */

/*
 * Copyright (c) 2026 Chris Johns <chris@contemporary.siftware>.
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

#include <machine/rtems-bsd-kernel-space.h>
#include <machine/rtems-bsd-support.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/kthread.h>
#include <sys/malloc.h>
#include <sys/selinfo.h>
#include <sys/sleepqueue.h>
#include <sys/sdt.h>

#include <inttypes.h>

#if defined(KDTRACE_HOOKS)

#include "rtems-kernel-kdtrace.h"

static sdt_probe_func_t old_sdt_probe_func;

SDT_PROVIDER_DEFINE(sched);
SDT_PROBE_DEFINE(sched, , , sleep);
SDT_PROBE_DEFINE2(sched, , , wakeup, "struct thread *",
    "struct proc *");

SDT_PROBE_DECLARE(sdt, , , m__init);
SDT_PROBE_DECLARE(sdt, , , m__gethdr_raw);
SDT_PROBE_DECLARE(sdt, , , m__gethdr);
SDT_PROBE_DECLARE(sdt, , , m__get_raw);
SDT_PROBE_DECLARE(sdt, , , m__get);
SDT_PROBE_DECLARE(sdt, , , m__getcl);
SDT_PROBE_DECLARE(sdt, , , m__getjcl);
SDT_PROBE_DECLARE(sdt, , , m__clget);
SDT_PROBE_DECLARE(sdt, , , m__cljget);
SDT_PROBE_DECLARE(sdt, , , m__cljset);
SDT_PROBE_DECLARE(sdt, , , m__free);
SDT_PROBE_DECLARE(sdt, , , m__freem);
SDT_PROBE_DECLARE(sdt, , , m__freemp);

SDT_PROVIDER_DECLARE(vfs);
SDT_PROBE_DECLARE(vfs, namecache, enter, done);
SDT_PROBE_DECLARE(vfs, namecache, enter, duplicate);
SDT_PROBE_DECLARE(vfs, namecache, enter_negative, done);
SDT_PROBE_DECLARE(vfs, namecache, enter_negative, done);
SDT_PROBE_DECLARE(vfs, namecache, fullpath_smr, hit);
SDT_PROBE_DECLARE(vfs, namecache, fullpath_smr, miss);
SDT_PROBE_DECLARE(vfs, namecache, fullpath, entry);
SDT_PROBE_DECLARE(vfs, namecache, fullpath, hit);
SDT_PROBE_DECLARE(vfs, namecache, fullpath, miss);
SDT_PROBE_DECLARE(vfs, namecache, fullpath, return);
SDT_PROBE_DECLARE(vfs, namecache, lookup, hit);
SDT_PROBE_DECLARE(vfs, namecache, lookup, hit__negative);
SDT_PROBE_DECLARE(vfs, namecache, lookup, miss);
SDT_PROBE_DECLARE(vfs, namecache, purge, done);
SDT_PROBE_DECLARE(vfs, namecache, purge, batch);
SDT_PROBE_DECLARE(vfs, namecache, purge_negative, done);
SDT_PROBE_DECLARE(vfs, namecache, purgevfs, done);
SDT_PROBE_DECLARE(vfs, namecache, zap, done);
SDT_PROBE_DECLARE(vfs, namecache, zap_negative, done);
SDT_PROBE_DECLARE(vfs, namecache, evict_negative, done);
SDT_PROBE_DECLARE(vfs, namecache, symlink, alloc__fail);
SDT_PROBE_DECLARE(vfs, namecache, symlink, alloc__fail);
SDT_PROBE_DECLARE(vfs, fplookup, lookup, done);
SDT_PROBE_DECLARE(vfs, namei, lookup, entry);
SDT_PROBE_DECLARE(vfs, namei, lookup, return);
SDT_PROBE_DECLARE(vfs, namei, lookup, entry);
SDT_PROBE_DECLARE(vfs, namei, lookup, return);
SDT_PROBE_DECLARE(vfs, vop, vop_islocked, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_islocked, return);
SDT_PROBE_DECLARE(vfs, vop, vop_lookup, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_lookup, return);
SDT_PROBE_DECLARE(vfs, vop, vop_cachedlookup, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_cachedlookup, return);
SDT_PROBE_DECLARE(vfs, vop, vop_create, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_create, return);
SDT_PROBE_DECLARE(vfs, vop, vop_whiteout, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_whiteout, return);
SDT_PROBE_DECLARE(vfs, vop, vop_mknod, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_mknod, return);
SDT_PROBE_DECLARE(vfs, vop, vop_open, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_open, return);
SDT_PROBE_DECLARE(vfs, vop, vop_close, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_close, return);
SDT_PROBE_DECLARE(vfs, vop, vop_fplookup_vexec, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_fplookup_vexec, return);
SDT_PROBE_DECLARE(vfs, vop, vop_fplookup_symlink, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_fplookup_symlink, return);
SDT_PROBE_DECLARE(vfs, vop, vop_access, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_access, return);
SDT_PROBE_DECLARE(vfs, vop, vop_accessx, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_accessx, return);
SDT_PROBE_DECLARE(vfs, vop, vop_stat, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_stat, return);
SDT_PROBE_DECLARE(vfs, vop, vop_getattr, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_getattr, return);
SDT_PROBE_DECLARE(vfs, vop, vop_setattr, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_setattr, return);
SDT_PROBE_DECLARE(vfs, vop, vop_mmapped, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_mmapped, return);
SDT_PROBE_DECLARE(vfs, vop, vop_read, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_read, return);
SDT_PROBE_DECLARE(vfs, vop, vop_read_pgcache, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_read_pgcache, return);
SDT_PROBE_DECLARE(vfs, vop, vop_write, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_write, return);
SDT_PROBE_DECLARE(vfs, vop, vop_ioctl, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_ioctl, return);
SDT_PROBE_DECLARE(vfs, vop, vop_poll, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_poll, return);
SDT_PROBE_DECLARE(vfs, vop, vop_kqfilter, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_kqfilter, return);
SDT_PROBE_DECLARE(vfs, vop, vop_revoke, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_revoke, return);
SDT_PROBE_DECLARE(vfs, vop, vop_fsync, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_fsync, return);
SDT_PROBE_DECLARE(vfs, vop, vop_remove, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_remove, return);
SDT_PROBE_DECLARE(vfs, vop, vop_link, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_link, return);
SDT_PROBE_DECLARE(vfs, vop, vop_rename, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_rename, return);
SDT_PROBE_DECLARE(vfs, vop, vop_mkdir, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_mkdir, return);
SDT_PROBE_DECLARE(vfs, vop, vop_rmdir, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_rmdir, return);
SDT_PROBE_DECLARE(vfs, vop, vop_symlink, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_symlink, return);
SDT_PROBE_DECLARE(vfs, vop, vop_readdir, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_readdir, return);
SDT_PROBE_DECLARE(vfs, vop, vop_readlink, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_readlink, return);
SDT_PROBE_DECLARE(vfs, vop, vop_inactive, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_inactive, return);
SDT_PROBE_DECLARE(vfs, vop, vop_need_inactive, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_need_inactive, return);
SDT_PROBE_DECLARE(vfs, vop, vop_reclaim, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_reclaim, return);
SDT_PROBE_DECLARE(vfs, vop, vop_lock1, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_lock1, return);
SDT_PROBE_DECLARE(vfs, vop, vop_unlock, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_unlock, return);
SDT_PROBE_DECLARE(vfs, vop, vop_bmap, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_bmap, return);
SDT_PROBE_DECLARE(vfs, vop, vop_strategy, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_strategy, return);
SDT_PROBE_DECLARE(vfs, vop, vop_getwritemount, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_getwritemount, return);
SDT_PROBE_DECLARE(vfs, vop, vop_getlowvnode, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_getlowvnode, return);
SDT_PROBE_DECLARE(vfs, vop, vop_print, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_print, return);
SDT_PROBE_DECLARE(vfs, vop, vop_pathconf, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_pathconf, return);
SDT_PROBE_DECLARE(vfs, vop, vop_advlock, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_advlock, return);
SDT_PROBE_DECLARE(vfs, vop, vop_advlockasync, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_advlockasync, return);
SDT_PROBE_DECLARE(vfs, vop, vop_advlockpurge, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_advlockpurge, return);
SDT_PROBE_DECLARE(vfs, vop, vop_reallocblks, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_reallocblks, return);
SDT_PROBE_DECLARE(vfs, vop, vop_getpages, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_getpages, return);
SDT_PROBE_DECLARE(vfs, vop, vop_getpages_async, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_getpages_async, return);
SDT_PROBE_DECLARE(vfs, vop, vop_putpages, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_putpages, return);
SDT_PROBE_DECLARE(vfs, vop, vop_getacl, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_getacl, return);
SDT_PROBE_DECLARE(vfs, vop, vop_setacl, entry);
SDT_PROBE_DECLARE(vfs, vop, vop_setacl, return);

#define SDT_PROBE_ADDRESS(_prov, _mod, _func, _name) \
  &_SDT_PROBE_NAME(_prov, _mod, _func, _name)[0]

static struct sdt_probe* probes[] = {
  SDT_PROBE_ADDRESS(sdt, , , m__init),
  SDT_PROBE_ADDRESS(sdt, , , m__gethdr_raw),
  SDT_PROBE_ADDRESS(sdt, , , m__gethdr),
  SDT_PROBE_ADDRESS(sdt, , , m__get_raw),
  SDT_PROBE_ADDRESS(sdt, , , m__get),
  SDT_PROBE_ADDRESS(sdt, , , m__getcl),
  SDT_PROBE_ADDRESS(sdt, , , m__getjcl),
  SDT_PROBE_ADDRESS(sdt, , , m__clget),
  SDT_PROBE_ADDRESS(sdt, , , m__cljget),
  SDT_PROBE_ADDRESS(sdt, , , m__cljset),
  SDT_PROBE_ADDRESS(sdt, , , m__free),
  SDT_PROBE_ADDRESS(sdt, , , m__freem),
  SDT_PROBE_ADDRESS(sdt, , , m__freemp),
  SDT_PROBE_ADDRESS(vfs, namecache, enter, done),
  SDT_PROBE_ADDRESS(vfs, namecache, enter, duplicate),
  SDT_PROBE_ADDRESS(vfs, namecache, enter_negative, done),
  SDT_PROBE_ADDRESS(vfs, namecache, enter_negative, done),
  SDT_PROBE_ADDRESS(vfs, namecache, fullpath_smr, hit),
  SDT_PROBE_ADDRESS(vfs, namecache, fullpath_smr, miss),
  SDT_PROBE_ADDRESS(vfs, namecache, fullpath, entry),
  SDT_PROBE_ADDRESS(vfs, namecache, fullpath, hit),
  SDT_PROBE_ADDRESS(vfs, namecache, fullpath, miss),
  SDT_PROBE_ADDRESS(vfs, namecache, fullpath, return),
  SDT_PROBE_ADDRESS(vfs, namecache, lookup, hit),
  SDT_PROBE_ADDRESS(vfs, namecache, lookup, hit__negative),
  SDT_PROBE_ADDRESS(vfs, namecache, lookup, miss),
  SDT_PROBE_ADDRESS(vfs, namecache, purge, done),
  SDT_PROBE_ADDRESS(vfs, namecache, purge, batch),
  SDT_PROBE_ADDRESS(vfs, namecache, purge_negative, done),
  SDT_PROBE_ADDRESS(vfs, namecache, purgevfs, done),
  SDT_PROBE_ADDRESS(vfs, namecache, zap, done),
  SDT_PROBE_ADDRESS(vfs, namecache, zap_negative, done),
  SDT_PROBE_ADDRESS(vfs, namecache, evict_negative, done),
  SDT_PROBE_ADDRESS(vfs, namecache, symlink, alloc__fail),
  SDT_PROBE_ADDRESS(vfs, namecache, symlink, alloc__fail),
  SDT_PROBE_ADDRESS(vfs, fplookup, lookup, done),
  SDT_PROBE_ADDRESS(vfs, namei, lookup, entry),
  SDT_PROBE_ADDRESS(vfs, namei, lookup, return),
  SDT_PROBE_ADDRESS(vfs, namei, lookup, entry),
  SDT_PROBE_ADDRESS(vfs, namei, lookup, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_islocked, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_islocked, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_lookup, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_lookup, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_cachedlookup, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_cachedlookup, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_create, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_create, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_whiteout, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_whiteout, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_mknod, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_mknod, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_open, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_open, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_close, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_close, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_fplookup_vexec, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_fplookup_vexec, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_fplookup_symlink, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_fplookup_symlink, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_access, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_access, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_accessx, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_accessx, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_stat, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_stat, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_getattr, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_getattr, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_setattr, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_setattr, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_mmapped, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_mmapped, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_read, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_read, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_read_pgcache, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_read_pgcache, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_write, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_write, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_ioctl, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_ioctl, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_poll, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_poll, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_kqfilter, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_kqfilter, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_revoke, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_revoke, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_fsync, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_fsync, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_remove, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_remove, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_link, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_link, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_rename, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_rename, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_mkdir, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_mkdir, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_rmdir, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_rmdir, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_symlink, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_symlink, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_readdir, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_readdir, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_readlink, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_readlink, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_inactive, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_inactive, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_need_inactive, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_need_inactive, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_reclaim, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_reclaim, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_lock1, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_lock1, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_unlock, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_unlock, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_bmap, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_bmap, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_strategy, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_strategy, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_getwritemount, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_getwritemount, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_getlowvnode, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_getlowvnode, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_print, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_print, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_pathconf, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_pathconf, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_advlock, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_advlock, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_advlockasync, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_advlockasync, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_advlockpurge, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_advlockpurge, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_reallocblks, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_reallocblks, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_getpages, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_getpages, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_getpages_async, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_getpages_async, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_putpages, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_putpages, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_getacl, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_getacl, return),
  SDT_PROBE_ADDRESS(vfs, vop, vop_setacl, entry),
  SDT_PROBE_ADDRESS(vfs, vop, vop_setacl, return),
};

#undef calloc
#undef free

typedef struct {
  uint32_t id;
  uintptr_t args[5];
  void* call_addr;
  uint64_t timestamp;
} dtrace_record;

typedef struct {
  size_t num_records;
  size_t active_triggers;
  struct sdt_provider* triggers[2];
  dtrace_record* records;
  size_t level;
  size_t out;
  size_t overflows;
} dtrace_record_manager;

static rtems_mutex rec_lock = RTEMS_MUTEX_INITIALIZER("dtrace");
static dtrace_record_manager* manager;

#define DT_NUMOF(_s) (sizeof(_s) / sizeof(_s[0]))

static void sdt_probe_trace(
  uint32_t id, uintptr_t arg0, uintptr_t arg1,
  uintptr_t arg2, uintptr_t arg3, uintptr_t arg4)
{
  if (id > 0 && id <= DT_NUMOF(probes)) {
    rtems_mutex_lock(&rec_lock);
    if (manager != NULL) {
      if (manager->level < manager->num_records) {
        struct sdt_probe* probe = probes[id - 1];
        bool found = manager->active_triggers == 0;
        if (!found) {
          for (int p = 0; p < DT_NUMOF(manager->triggers); ++p) {
            if (probe->prov == manager->triggers[p]) {
              found = true;
              break;
            }
          }
        }
        if (found) {
          dtrace_record* rec = &manager->records[manager->level++];
          rec->id = id;
          rec->args[0] = arg0;
          rec->args[1] = arg1;
          rec->args[2] = arg2;
          rec->args[3] = arg3;
          rec->args[4] = arg4;
          rec->call_addr = __builtin_return_address(0);
          rec->timestamp = rtems_clock_get_uptime_nanoseconds();
        }
      } else {
        ++manager->overflows;
      }
    }
    rtems_mutex_unlock(&rec_lock);
  }
}

int rtems_sdt_open(size_t records) {
  rtems_mutex_lock(&rec_lock);
  if (manager != NULL) {
    rtems_mutex_unlock(&rec_lock);
    errno = ENOENT;
    return -1;
  }
  manager = calloc(1, sizeof(dtrace_record_manager));
  if (manager == NULL) {
    rtems_mutex_unlock(&rec_lock);
    errno = ENOMEM;
    return -1;
  }
  manager->records = calloc(records, sizeof(dtrace_record));
  if (manager->records == NULL) {
    free(manager);
    manager = NULL;
    rtems_mutex_unlock(&rec_lock);
    errno = ENOMEM;
    return -1;
  }
  manager->num_records = records;
  rtems_mutex_unlock(&rec_lock);
  return 0;
}

int rtems_sdt_close(void) {
  rtems_mutex_lock(&rec_lock);
  if (sdt_probes_enabled) {
    sdt_probes_enabled = 0;
    sdt_probe_func = old_sdt_probe_func;
  }
  if (manager != NULL) {
    free(manager->records);
    free(manager);
    manager = NULL;
  }
  rtems_mutex_unlock(&rec_lock);
  return 0;
}

int rtems_sdt_reset(void) {
  rtems_mutex_lock(&rec_lock);
  if (manager != NULL) {
    manager->level = 0;
    manager->out = 0;
    manager->overflows = 0;
  }
  rtems_mutex_unlock(&rec_lock);
  return 0;
}

int rtems_sdt_clear(void) {
  rtems_mutex_lock(&rec_lock);
  if (manager != NULL) {
    manager->out = 0;
  }
  rtems_mutex_unlock(&rec_lock);
  return 0;
}

int rtems_sdt_enable(void) {
  rtems_mutex_lock(&rec_lock);
  if (manager == NULL) {
    rtems_mutex_unlock(&rec_lock);
    errno = ENOENT;
    return -1;
  }
  if (!sdt_probes_enabled) {
    int p;
    for (p = 0; p < DT_NUMOF(probes); ++p) {
      probes[p]->id = p + 1;
      probes[p]->n_args = 5;
    }
    old_sdt_probe_func = sdt_probe_func;
    sdt_probe_func = sdt_probe_trace;
    sdt_probes_enabled = 1;
  }
  rtems_mutex_unlock(&rec_lock);
}

int rtems_sdt_disable(void) {
  rtems_mutex_lock(&rec_lock);
  if (manager == NULL) {
    rtems_mutex_unlock(&rec_lock);
    errno = ENOENT;
    return -1;
  }
  if (sdt_probes_enabled) {
    sdt_probes_enabled = 0;
    sdt_probe_func = old_sdt_probe_func;
  }
  rtems_mutex_unlock(&rec_lock);
}

int rtems_sdt_status(void) {
  int r = -1;
  rtems_mutex_lock(&rec_lock);
  if (sdt_probes_enabled && manager != NULL) {
    r = manager->level - manager->out;
  }
  rtems_mutex_unlock(&rec_lock);
  return r;
}

int rtems_sdt_set_trigger(const char* prov) {
  rtems_mutex_lock(&rec_lock);
  if (manager != NULL) {
    for (int p = 0; p < DT_NUMOF(probes); ++p) {
      struct sdt_probe* probe = probes[p];
      if (strcmp(prov, probe->prov->name) == 0) {
        for (int t = 0; t < DT_NUMOF(manager->triggers); ++t) {
          if (probe->prov == manager->triggers[t]) {
            rtems_mutex_unlock(&rec_lock);
            return 0;
          }
        }
        for (int t = 0; t < DT_NUMOF(manager->triggers); ++t) {
          if (manager->triggers[t] == NULL) {
            manager->triggers[t] = probe->prov;
            ++manager->active_triggers;
            rtems_mutex_unlock(&rec_lock);
            return 0;
          }
        }
      }
    }
  }
  rtems_mutex_unlock(&rec_lock);
  return -1;
}

int rtems_sdt_clear_trigger(const char* prov) {
  rtems_mutex_lock(&rec_lock);
  if (manager != NULL) {
    for (int p = 0; p < DT_NUMOF(probes); ++p) {
      struct sdt_probe* probe = probes[p];
      if (strcmp(prov, probe->prov->name) == 0) {
        for (int t = 0; t < DT_NUMOF(manager->triggers); ++t) {
          if (probe->prov == manager->triggers[t]) {
            --manager->active_triggers;
            manager->triggers[t] = NULL;
            rtems_mutex_unlock(&rec_lock);
            return 0;
          }
        }
      }
    }
  }
  rtems_mutex_unlock(&rec_lock);
  return -1;
}

static const char* safe_str(const char* unsafe) {
  if (unsafe == NULL) {
    return "";
  }
  return unsafe;
}

int rtems_sdt_print(FILE* file) {
  while (true) {
    rtems_mutex_lock(&rec_lock);
    if (manager->out == manager->level) {
      rtems_mutex_unlock(&rec_lock);
      return 0;
    }
    dtrace_record rec = manager->records[manager->out++];
    rtems_mutex_unlock(&rec_lock);
    if (rec.id <= 0) {
      continue;
    }
    struct sdt_probe* probe = probes[rec.id - 1];
    fprintf(
      file, "[%s:%s] %s: %s(",
      safe_str(probe->prov->name), safe_str(probe->mod),
      safe_str(probe->name), safe_str(probe->func));
    for (int a = 0; a < probe->n_args; ++a) {
      if (a > 0) {
        fprintf(file, ", ");
      }
      fprintf(file, "0x%" PRIxPTR "", rec.args[a]);
    }
    fprintf(
      file, ") from=%p ts=%" PRIu64 "\n", rec.call_addr, rec.timestamp);
  }
  return 0;
}

int rtems_sdt_json(FILE* file) {
  bool first = true;
  fprintf(file, "[");
  while (true) {
    rtems_mutex_lock(&rec_lock);
    if (manager->out == manager->level) {
      rtems_mutex_unlock(&rec_lock);
      fprintf(file, "]\n");
      return 0;
    }
    dtrace_record rec = manager->records[manager->out++];
    rtems_mutex_unlock(&rec_lock);
    if (rec.id <= 0) {
      continue;
    }
    if (!first) {
      fprintf(file, ",\n");
    }
    struct sdt_probe* probe = probes[rec.id - 1];
    fprintf(
      file,
      "{\"id\":%" PRIu32","
      "\"prov\":\"%s\","
      "\"mod\":\"%s\","
      "\"func\":\"%s\","
      "\"name\":\"%s\","
      "\"arg0\":%" PRIuPTR ","
      "\"arg1\":%" PRIuPTR ","
      "\"arg2\":%" PRIuPTR ","
      "\"arg3\":%" PRIuPTR ","
      "\"arg4\":%" PRIuPTR ","
      "\"caller\":%" PRIuPTR ","
      "\"timestamp\":%" PRIu64 "}",
      rec.id, safe_str(probe->prov->name), safe_str(probe->mod),
      safe_str(probe->func), safe_str(probe->name),
      rec.args[0], rec.args[1], rec.args[2], rec.args[3], rec.args[4],
      (uintptr_t) rec.call_addr, rec.timestamp);
    first = false;
  }
  return 0;
}

/*
 * Set PROBE_FROM_START to 1 to enable probing automatically
 */
#define PROBE_FROM_START 0
#if PROBE_FROM_START
static void dtrace_auto_config(void* dummy) {
  rtems_sdt_open(5000);
  rtems_sdt_set_trigger("vfs");
  rtems_sdt_enable();
}
SYSINIT(dtrace_auto_config, SI_SUB_CONFIGURE, SI_ORDER_EIGHTH, dtrace_auto_config, NULL);
#endif

#endif /* KDTRACE_HOOKS */
