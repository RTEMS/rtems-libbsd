/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 1982, 1986, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)mman.h	8.2 (Berkeley) 1/9/95
 */

/*
 * Taken from FreeBSD-14. This is the _KERNEL defined fragments of
 * sys/sys/mman.h.
 */

#if defined(_KERNEL) || defined(_WANT_FILE)
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/queue.h>
#include <sys/rangelock.h>
#include <vm/vm.h>

struct file;

struct shmfd {
	vm_ooffset_t	shm_size;
	vm_object_t	shm_object;
	vm_pindex_t	shm_pages;	/* allocated pages */
	int		shm_refs;
	uid_t		shm_uid;
	gid_t		shm_gid;
	mode_t		shm_mode;
	int		shm_kmappings;

	/*
	 * Values maintained solely to make this a better-behaved file
	 * descriptor for fstat() to run on.
	 */
	struct timespec	shm_atime;
	struct timespec	shm_mtime;
	struct timespec	shm_ctime;
	struct timespec	shm_birthtime;
	ino_t		shm_ino;

	struct label	*shm_label;		/* MAC label */
	const char	*shm_path;

#ifndef __rtems__
	struct rangelock shm_rl;
#endif /* __rtems__ */
	struct mtx	shm_mtx;

	int		shm_flags;
	int		shm_seals;

	/* largepage config */
	int		shm_lp_psind;
	int		shm_lp_alloc_policy;
};
#endif

struct prison;

int	shm_map(struct file *fp, size_t size, off_t offset, void **memp);
int	shm_unmap(struct file *fp, void *mem, size_t size);

int	shm_access(struct shmfd *shmfd, struct ucred *ucred, int flags);
struct shmfd *shm_alloc(struct ucred *ucred, mode_t mode, bool largepage);
struct shmfd *shm_hold(struct shmfd *shmfd);
void	shm_drop(struct shmfd *shmfd);
int	shm_dotruncate(struct shmfd *shmfd, off_t length);
bool	shm_largepage(struct shmfd *shmfd);
void	shm_remove_prison(struct prison *pr);

extern struct fileops shm_ops;

#define	MAP_32BIT_MAX_ADDR	((vm_offset_t)1 << 31)

