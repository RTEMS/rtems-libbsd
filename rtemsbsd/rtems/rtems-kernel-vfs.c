/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief VFS to RTEMS LibIO interface.
 */

/*
 * Copyright (c) 2020 Chris Johns.  All rights reserved.
 *
 *  Contemporary Software
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

#include <sys/param.h>
#include <sys/buf.h>
#include <sys/capsicum.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/file.h>
#include <sys/filedesc.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/mount.h>
#include <sys/namei.h>
#include <sys/proc.h>
#include <sys/syscallsubr.h>
#include <sys/sysproto.h>
#include <sys/time.h>
#include <sys/vnode.h>

#include <rtems/libio.h>
#include <rtems/seterr.h>

const rtems_filesystem_operations_table rtems_bsd_vfsops = {
	.lock_h = rtems_bsd_vfs_mt_entry_lock,
	.unlock_h = rtems_bsd_vfs_mt_entry_unlock,
	.eval_path_h = rtems_bsd_vfs_eval_path,
	.link_h = rtems_bsd_vfs_link,
	.are_nodes_equal_h = rtems_bsd_vfs_are_nodes_equal,
	.mknod_h = rtems_bsd_vfs_mknod,
	.rmnod_h = rtems_bsd_vfs_rmnod,
	.fchmod_h = rtems_bsd_vfs_fchmod,
	.chown_h = rtems_bsd_vfs_chown,
	.clonenod_h = rtems_bsd_vfs_clonenode,
	.freenod_h = rtems_bsd_vfs_freenode,
	.mount_h = rtems_bsd_vfs_mount,
	.unmount_h = rtems_bsd_vfs_unmount,
	.fsunmount_me_h = rtems_bsd_vfs_fsunmount_me,
	.utimens_h = rtems_bsd_vfs_utimens,
	.symlink_h = rtems_bsd_vfs_symlink,
	.readlink_h = rtems_bsd_vfs_readlink,
	.rename_h = rtems_bsd_vfs_rename,
	.statvfs_h = rtems_bsd_vfs_statvfs
};

int
rtems_bsd_vfs_mount_init(rtems_filesystem_mount_table_entry_t *mt_entry)
{
	mt_entry->fs_info = NULL;
	mt_entry->no_regular_file_mknod = true;
	mt_entry->ops = &rtems_bsd_vfsops;
	mt_entry->mt_fs_root->location.node_access = 0;
	mt_entry->mt_fs_root->location.handlers = &rtems_bsd_sysgen_dirops;
	return 0;
}

static void
rtems_bsd_vfs_loc_vnode_hold(
    rtems_filesystem_location_info_t *loc, struct vnode *vp)
{
	if (vp != NULL) {
		VREF(vp);
		if (RTEMS_BSD_VFS_TRACE)
			printf("bsd: vfs: loc: hold loc=%p vn=%p\n", loc, vp);
	}
}

static void
rtems_bsd_vfs_loc_vnode_drop(
    rtems_filesystem_location_info_t *loc, struct vnode *vp)
{
	if (vp != NULL) {
		vrele(vp);
		if (RTEMS_BSD_VFS_TRACE) {
			printf("bsd: vfs: loc: drop loc=%p vn=%p\n", loc, vp);
		}
	}
}

static void
rtems_bsd_vfs_loc_hold(rtems_filesystem_location_info_t *loc)
{
	if (loc != NULL) {
		if (RTEMS_BSD_VFS_TRACE)
			printf("bsd: vfs: loc: hold loc=%p vn=%p vdp=%p\n", loc,
			    rtems_bsd_libio_loc_to_vnode(loc),
			    rtems_bsd_libio_loc_to_vnode_dir(loc));
		rtems_bsd_vfs_loc_vnode_hold(
		    loc, rtems_bsd_libio_loc_to_vnode(loc));
		rtems_bsd_vfs_loc_vnode_hold(
		    loc, rtems_bsd_libio_loc_to_vnode_dir(loc));
	}
}

static void
rtems_bsd_vfs_loc_drop(rtems_filesystem_location_info_t *loc)
{
	if (loc != NULL) {
		if (RTEMS_BSD_VFS_TRACE)
			printf("bsd: vfs: loc: drop loc=%p vn=%p vdp=%p\n", loc,
			    rtems_bsd_libio_loc_to_vnode(loc),
			    rtems_bsd_libio_loc_to_vnode_dir(loc));
		rtems_bsd_libio_loc_set_vnode(loc, NULL);
		rtems_bsd_libio_loc_set_vnode_dir(loc, NULL);
	}
}

void
rtems_bsd_vfs_mt_entry_lock(
    const rtems_filesystem_mount_table_entry_t *mt_entry)
{
	struct vnode *vp = rtems_bsd_libio_loc_to_vnode(
	    &mt_entry->mt_fs_root->location);
	if (RTEMS_BSD_VFS_TRACE) {
		printf("bsd: vfs: lock: vn=%p\n", vp);
	}
	VREF(vp);
}

void
rtems_bsd_vfs_mt_entry_unlock(
    const rtems_filesystem_mount_table_entry_t *mt_entry)
{
	struct vnode *vp = rtems_bsd_libio_loc_to_vnode(
	    &mt_entry->mt_fs_root->location);
	if (RTEMS_BSD_VFS_TRACE) {
		printf("bsd: vfs: unlock: vn=%p\n", vp);
	}
	vrele(vp);
}

static void
rtems_bsd_vfs_componentname(struct componentname *cnd, u_long nameiop,
    char *name, u_int namelen, struct ucred *cred)
{
	memset(cnd, 0, sizeof(*cnd));
	cnd->cn_nameiop = nameiop;
	cnd->cn_nameptr = name;
	cnd->cn_namelen = namelen;
	cnd->cn_cred = cred;
}

static int
rtems_bsd_vfs_vnode_componentname(struct componentname *cnd, struct vnode *vp,
    u_long nameiop, char *name, u_int namelen, struct ucred *cred)
{
	struct vnode *tvp;
	const u_int namemax = namelen - 1;
	char *namep;
	int error;
	name[namemax] = '\0';
	namelen = namemax;
	tvp = vp;
	error = vn_vptocnp(&tvp, NULL, name, &namelen);
	if (error == 0) {
		name = &name[namelen];
		namelen = namemax - namelen;
	} else {
		name = NULL;
		namelen = 0;
	}
	rtems_bsd_vfs_componentname(cnd, nameiop, name, namelen, cred);
	return error;
}

static bool
rtems_bsd_vfs_vnode_is_directory(
    rtems_filesystem_eval_path_context_t *ctx, void *arg)
{
	struct vnode *vp = *((struct vnode **)arg);
	return vp->v_type == VDIR;
}

static rtems_filesystem_eval_path_generic_status
rtems_bsd_vfs_eval_token(rtems_filesystem_eval_path_context_t *ctx, void *arg,
    const char *token, size_t tokenlen)
{
	rtems_filesystem_location_info_t *currentloc;
	struct thread *td = curthread;
	struct filedesc *fdp = td->td_proc->p_fd;
	struct nameidata nd;
	struct vnode **vpp = arg;
	struct vnode *vp;
	struct vnode *dvp;
	struct vnode *cdir;
	struct vnode *rdir;
	char ntoken[NAME_MAX + 1];
	u_long op = LOOKUP;
	u_long flags = 0;
	int eval_flags;
	bool no_more_path;
	rtems_filesystem_location_info_t *rootloc;
	int error;

	currentloc = rtems_filesystem_eval_path_get_currentloc(ctx);
	no_more_path = !rtems_filesystem_eval_path_has_path(ctx);

	if (RTEMS_BSD_VFS_TRACE) {
		printf("bsd: vfs: eval_token: t=%s:%d vp=%p\n", token, tokenlen,
		    *vpp);
	}

	if (tokenlen > NAME_MAX) {
		rtems_filesystem_eval_path_error(ctx, E2BIG);
		return RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_DONE;
	}

	if (rtems_filesystem_is_current_directory(token, tokenlen)) {
		rtems_filesystem_eval_path_clear_token(ctx);
		return no_more_path ?
		    RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_DONE :
		    RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_CONTINUE;
	}

	/*
	 * Have we reached the root vnode. Note the namei call will
	 * successfully lookup `..` when we are at the mount point
	 * because VFS mounts are held under the root pseudofs file
	 * system.
	 */
	rootloc = &currentloc->mt_entry->mt_fs_root->location;
	if (*vpp == rtems_bsd_libio_loc_to_vnode(rootloc)) {
		if (rtems_filesystem_is_parent_directory(token, tokenlen)) {
			if (RTEMS_BSD_VFS_TRACE) {
				printf(
				    "bsd: vfs: parent from root: vp=%p rvp=%p\n",
				    *vpp,
				    rtems_bsd_libio_loc_to_vnode(rootloc));
			}
			rtems_filesystem_eval_path_put_back_token(ctx);
			return RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_CONTINUE;
		}
	}

	eval_flags = rtems_filesystem_eval_path_get_flags(ctx);
	flags |= (eval_flags & RTEMS_FS_FOLLOW_LINK) != 0 ? FOLLOW : NOFOLLOW;

	if (no_more_path && ((eval_flags & RTEMS_FS_MAKE) != 0)) {
		op = CREATE;
		flags |= LOCKPARENT;
	}

	flags |= WANTPARENT;

	FILEDESC_XLOCK(fdp);
	rdir = fdp->fd_rdir;
	cdir = fdp->fd_cdir;
	fdp->fd_rdir = rootvnode;
	fdp->fd_cdir = *vpp;
	vref(*vpp);
	FILEDESC_XUNLOCK(fdp);

	bcopy(token, ntoken, tokenlen);
	ntoken[tokenlen] = '\0';

	NDINIT_ATVP(&nd, op, flags, UIO_USERSPACE, ntoken, *vpp, td);
	error = namei(&nd);

	if (RTEMS_BSD_VFS_TRACE) {
		printf(
		    "bsd: vfs: eval_token: namei=%d:%s token=%s cwd=%p vp=%p dvp=%p mp=%p %s\n",
		    error, strerror(error), ntoken, *vpp, nd.ni_vp, nd.ni_dvp,
		    nd.ni_vp ? nd.ni_vp->v_mountedhere : NULL,
		    no_more_path ? "no-more-path" : "more-path");
	}

	if (error != 0) {
		NDFREE(&nd, NDF_ONLY_PNBUF);
		rtems_filesystem_eval_path_error(ctx, error);
		return RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_DONE;
	}

	FILEDESC_XLOCK(fdp);
	fdp->fd_rdir = rdir;
	fdp->fd_cdir = cdir;
	FILEDESC_XUNLOCK(fdp);

	/*
	 * If there is no more path and this is the last token and the lookup
	 * with CREATE failed to find a vnode it does not exist and needs to be
	 * created. Leave the currentloc where it is.
	 */
	if (nd.ni_vp != NULL) {
		rtems_bsd_libio_loc_set_vnode(currentloc, nd.ni_vp);
		rtems_bsd_libio_loc_set_vnode_dir(currentloc, nd.ni_dvp);
	}

	*vpp = nd.ni_vp;

	NDFREE(&nd, 0);

	if (*vpp == rtems_bsd_libio_loc_to_vnode(rootloc)) {
		if (RTEMS_BSD_VFS_TRACE) {
			printf(
			    "bsd: vfs: eval_token: cross-mount vp=%p rvp=%p\n",
			    *vpp, rtems_bsd_libio_loc_to_vnode(rootloc));
		}
		rtems_filesystem_eval_path_clear_token(ctx);
		return RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_CONTINUE;
	}

	if (*vpp != NULL) {
		rtems_filesystem_eval_path_clear_token(ctx);
	}

	return no_more_path ? RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_DONE :
			      RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_CONTINUE;
}

static const rtems_filesystem_eval_path_generic_config
    rtems_bsd_vfs_eval_config = {
	    .is_directory = rtems_bsd_vfs_vnode_is_directory,
	    .eval_token = rtems_bsd_vfs_eval_token
    };

void
rtems_bsd_vfs_eval_path(rtems_filesystem_eval_path_context_t *ctx)
{
	rtems_filesystem_location_info_t *currentloc;
	struct vnode *vp;

	errno = 0;

	currentloc = rtems_filesystem_eval_path_get_currentloc(ctx);
	vp = rtems_bsd_libio_loc_to_vnode(currentloc);

	if (RTEMS_BSD_VFS_TRACE) {
		printf("bsd: vfs: eval_path: t=%s:%d (%s) cloc=%p\n",
		    ctx->token, ctx->tokenlen, ctx->path, vp);
	}

	/*
	 * For locking
	 */
	rtems_bsd_libio_loc_set_vnode(currentloc, vp);
	rtems_bsd_libio_loc_set_vnode_dir(currentloc, vp);

	rtems_filesystem_eval_path_generic(
	    ctx, &vp, &rtems_bsd_vfs_eval_config);

	if (RTEMS_BSD_VFS_TRACE) {
		printf("bsd: vfs: eval_path: e=%d:%s vp=%p cloc=%p lookup=%p\n",
		    errno, strerror(errno), vp,
		    rtems_bsd_libio_loc_to_vnode(currentloc),
		    rtems_bsd_libio_loc_to_vnode_dir(currentloc));
	}
}

bool
rtems_bsd_vfs_are_nodes_equal(const rtems_filesystem_location_info_t *a,
    const rtems_filesystem_location_info_t *b)
{
	struct vnode *avp = rtems_bsd_libio_loc_to_vnode(a);
	struct vnode *bvp = rtems_bsd_libio_loc_to_vnode(b);
	struct vnode *advp = rtems_bsd_libio_loc_to_vnode_dir(a);
	struct vnode *bdvp = rtems_bsd_libio_loc_to_vnode_dir(b);
	bool eq;
	if (advp != NULL && bdvp != NULL) {
		eq = avp == bvp && advp == bdvp;
	} else {
		eq = avp == bvp;
	}
	if (RTEMS_BSD_VFS_TRACE) {
		printf(
		    "bsd: vfs: nodes_equal: %s vn:%s (%p == %p) dvn:%s (%p == %p)\n",
		    eq ? "EQ" : "NE", avp == bvp ? "equal" : "not-equal", avp,
		    bvp, advp == bdvp ? "equal" : "not-equal", advp, bdvp);
	}
	return eq;
}

int
rtems_bsd_vfs_clonenode(rtems_filesystem_location_info_t *loc)
{
	rtems_bsd_vfs_loc_hold(loc);
	if (RTEMS_BSD_VFS_TRACE) {
		struct vnode *vp = rtems_bsd_libio_loc_to_vnode(loc);
		struct vnode *dvp = rtems_bsd_libio_loc_to_vnode_dir(loc);
		printf("bsd: vfs: clonenode: %p vn=%p dvp=%p\n", loc, vp, dvp);
	}
	return 0;
}

void
rtems_bsd_vfs_freenode(const rtems_filesystem_location_info_t *loc)
{
	if (RTEMS_BSD_VFS_TRACE) {
		struct vnode *vp = rtems_bsd_libio_loc_to_vnode(loc);
		struct vnode *dp = rtems_bsd_libio_loc_to_vnode_dir(loc);
		printf("bsd: vfs: freenode: %p vn=%p dp=%p\n", loc, vp, dp);
	}
	rtems_bsd_vfs_loc_drop(
	    RTEMS_DECONST(rtems_filesystem_location_info_t *, loc));
}

int
rtems_bsd_vfs_link(const rtems_filesystem_location_info_t *targetdirloc,
    const rtems_filesystem_location_info_t *sourceloc, const char *name,
    size_t namelen)
{
	struct thread *td = curthread;
	struct mount *mp;
	struct vnode *tdvp = rtems_bsd_libio_loc_to_vnode(targetdirloc);
	struct vnode *svp = rtems_bsd_libio_loc_to_vnode(sourceloc);
	struct componentname cn;
	int error;
	if (RTEMS_BSD_VFS_TRACE) {
		printf(
		    "bsd: vfs: link tdvn=%p svn=%p name=%s\n", tdvp, svp, name);
	}
	if (td == NULL) {
		if (RTEMS_BSD_VFS_TRACE) {
			printf("bsd: vfs: link: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	if (svp->v_mount != tdvp->v_mount) {
		if (RTEMS_BSD_VFS_TRACE)
			printf("bsd: vfs: link: crossmounts\n");
		error = EXDEV;
		goto out;
	}
	vref(tdvp);
	vn_lock(tdvp, LK_EXCLUSIVE | LK_RETRY);
	error = vn_start_write(tdvp, &mp, V_NOWAIT);
	if (error != 0) {
		vput(tdvp);
		error = vn_start_write(NULL, &mp, V_XSLEEP | PCATCH);
		if (error == 0)
			error = EAGAIN;
		goto out;
	}
	rtems_bsd_vfs_componentname(
	    &cn, LOOKUP, RTEMS_DECONST(char *, name), namelen, td->td_ucred);
	error = VOP_LINK(tdvp, svp, &cn);
	VOP_UNLOCK(tdvp, 0);
out:
	return rtems_bsd_error_to_status_and_errno(error);
}

int
rtems_bsd_vfs_fchmod(const rtems_filesystem_location_info_t *loc, mode_t mode)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct vnode *vp = rtems_bsd_libio_loc_to_vnode(loc);
	int error;
	if (RTEMS_BSD_VFS_TRACE) {
		printf("bsd: vfs: fchmod: vn=%p mode=%x\n", vp, mode);
	}
	if (td == NULL) {
		if (RTEMS_BSD_VFS_TRACE) {
			printf("bsd: vfs: fchmod: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	error = setfmode(td, NULL, vp, mode);
	return rtems_bsd_error_to_status_and_errno(error);
}

int
rtems_bsd_vfs_chown(
    const rtems_filesystem_location_info_t *loc, uid_t owner, gid_t group)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct vnode *vp = rtems_bsd_libio_loc_to_vnode(loc);
	int error;
	if (RTEMS_BSD_VFS_TRACE) {
		printf("bsd: vfs: chown: vn=%p owner=%d group=%d\n", vp, owner,
		    group);
	}
	if (td == NULL) {
		if (RTEMS_BSD_VFS_TRACE) {
			printf("bsd: vfs: chown: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	error = setfown(td, NULL, vp, owner, group);
	return rtems_bsd_error_to_status_and_errno(error);
}

int
rtems_bsd_vfs_mount(rtems_filesystem_mount_table_entry_t *mt_entry)
{
	int error;
	if (RTEMS_BSD_VFS_TRACE) {
		printf("bsd: vfs: mount\n");
	}
	error = EOPNOTSUPP;
	return rtems_bsd_error_to_status_and_errno(error);
}

int
rtems_bsd_vfs_fsmount_me(
    rtems_filesystem_mount_table_entry_t *mt_entry, const void *data)
{
	struct thread *td = curthread;
	struct mount_args *args = (struct mount_args *)data;
	int error;
	if (RTEMS_BSD_VFS_TRACE) {
		printf("bsd: vfs: fsmount_me\n");
	}
	if (td == NULL) {
		if (RTEMS_BSD_VFS_TRACE) {
			printf("bsd: vfs: fsmount_me: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	error = sys_mount(td, args);
	if (RTEMS_BSD_VFS_TRACE) {
		printf("bsd: vfs: fsmount_me: error=%d %s\n", error,
		    strerror(error));
	}
	return rtems_bsd_error_to_status_and_errno(error);
}

int
rtems_bsd_vfs_unmount(rtems_filesystem_mount_table_entry_t *mt_entry)
{
	int error;
	if (RTEMS_BSD_VFS_TRACE) {
		printf("bsd: vfs: unmount\n");
	}
	error = EOPNOTSUPP;
	return rtems_bsd_error_to_status_and_errno(error);
}

void
rtems_bsd_vfs_fsunmount_me(rtems_filesystem_mount_table_entry_t *mt_entry)
{
	int error;
	if (RTEMS_BSD_VFS_TRACE) {
		printf("bsd: vfs: fsunmount_me: %p\n",
		    rtems_bsd_libio_loc_to_vnode(
			&mt_entry->mt_fs_root->location));
	}
	rtems_bsd_vfs_loc_drop(&mt_entry->mt_fs_root->location);
}

int
rtems_bsd_vfs_mknod(const rtems_filesystem_location_info_t *parentloc,
    const char *name, size_t namelen, mode_t mode, dev_t dev)
{
	struct thread *td = curthread;
	struct filedesc *fdp = td->td_proc->p_fd;
	struct vnode *vn = rtems_bsd_libio_loc_to_vnode(parentloc);
	char *path = RTEMS_DECONST(char *, name);
	int error;

	if (td == NULL) {
		if (RTEMS_BSD_VFS_TRACE) {
			printf("bsd: vfs: mknod: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}

	if (RTEMS_BSD_VFS_TRACE) {
		const char *type;
		if ((mode & S_IFMT) == S_IFREG)
			type = "REG";
		else if ((mode & S_IFMT) == S_IFDIR)
			type = "DIR";
		else
			type = "DEV";
		printf(
		    "bsd: vfs: mknod: mode=%s name=%s (%d) mode=%08x dev=%08llx parent=%p\n",
		    type, name, namelen, mode, dev, vn);
	}

	fdp->fd_cdir = vn;

	switch (mode & S_IFMT) {
	case S_IFREG:
		error = 0;
		break;
	case S_IFDIR:
		VREF(vn);
		error = kern_mkdirat(td, AT_FDCWD, path, UIO_USERSPACE, mode);
		vrele(vn);
		break;
	default:
		VREF(vn);
		error = kern_mknodat(
		    td, AT_FDCWD, path, UIO_USERSPACE, mode, dev);
		vrele(vn);
		break;
	}

	if (RTEMS_BSD_VFS_TRACE) {
		printf(
		    "bsd: vfs: mknod: error=%s (%d)\n", strerror(error), error);
	}

	return rtems_bsd_error_to_status_and_errno(error);
}

int
rtems_bsd_vfs_rmnod(const rtems_filesystem_location_info_t *parentloc,
    const rtems_filesystem_location_info_t *loc)
{
	struct thread *td = curthread;
	struct mount *mp;
	struct vnode *vp = rtems_bsd_libio_loc_to_vnode(loc);
	struct vnode *dvp = rtems_bsd_libio_loc_to_vnode(parentloc);
	struct vnode *tvp;
	struct componentname cn;
	char name[255];
	int error;
	if (RTEMS_BSD_VFS_TRACE) {
		printf("bsd: vfs: rmnod vn=%p at=%p\n", vp, dvp);
	}
	if (td == NULL) {
		if (RTEMS_BSD_VFS_TRACE) {
			printf("bsd: vfs: rmnod: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	error = rtems_bsd_vfs_vnode_componentname(
	    &cn, vp, DELETE, name, sizeof(name), td->td_ucred);
	if (error != 0) {
		if (RTEMS_BSD_VFS_TRACE) {
			printf(
			    "bsd: vfs: rmnod: componentname lookup: (%d) %s\n",
			    error, strerror(error));
		}
		goto out;
	}
	if (RTEMS_BSD_VFS_TRACE) {
		printf("bsd: vfs: rmnod name=%s\n", cn.cn_nameptr);
	}
	vref(vp);
restart:
	bwillwrite();
	if (vp->v_vflag & VV_ROOT) {
		error = EBUSY;
		goto out;
	}
	if (vn_start_write(dvp, &mp, V_NOWAIT) != 0) {
		if ((error = vn_start_write(NULL, &mp, V_XSLEEP | PCATCH)) !=
		    0) {
			return rtems_bsd_error_to_status_and_errno(error);
		}
		goto restart;
	}
	vfs_notify_upper(vp, VFS_NOTIFY_UPPER_UNLINK);
	error = VOP_RMDIR(dvp, vp, &cn);
	vn_finished_write(mp);
out:
	return rtems_bsd_error_to_status_and_errno(error);
}

int
rtems_bsd_vfs_utimens(
    const rtems_filesystem_location_info_t *loc, time_t actime, time_t modtime)
{
	int error;
	if (RTEMS_BSD_VFS_TRACE) {
		printf("bsd: vfs: utimens\n");
	}
	error = EOPNOTSUPP;
	return rtems_bsd_error_to_status_and_errno(error);
}

int
rtems_bsd_vfs_symlink(const rtems_filesystem_location_info_t *targetdirloc,
    const char *name, size_t namelen, const char *target)
{
	struct thread *td = curthread;
	struct filedesc *fdp;
	struct mount *mp;
	struct vnode *tdvp = rtems_bsd_libio_loc_to_vnode(targetdirloc);
	struct vattr vattr;
	struct nameidata nd;
	int error;
	if (RTEMS_BSD_VFS_TRACE) {
		printf("bsd: vfs: symlink tdvn=%p name=%s target=%s\n", tdvp,
		    name, target);
	}
	if (td == NULL) {
		if (RTEMS_BSD_VFS_TRACE) {
			printf("bsd: vfs: symlink: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	fdp = td->td_proc->p_fd;
	fdp->fd_cdir = tdvp;
restart:
	bwillwrite();
	NDINIT_ATRIGHTS(&nd, CREATE,
	    LOCKPARENT | SAVENAME | AUDITVNODE1 | NOCACHE, UIO_SYSSPACE, name,
	    AT_FDCWD, &cap_symlinkat_rights, td);
	error = namei(&nd);
	if (error != 0) {
		goto out;
	}
	if (nd.ni_vp != NULL) {
		NDFREE(&nd, NDF_ONLY_PNBUF);
		if (nd.ni_vp == nd.ni_dvp)
			vrele(nd.ni_dvp);
		else
			vput(nd.ni_dvp);
		vrele(nd.ni_vp);
		error = EEXIST;
		goto out;
	}
	error = vn_start_write(nd.ni_dvp, &mp, V_NOWAIT);
	if (error != 0) {
		NDFREE(&nd, NDF_ONLY_PNBUF);
		vput(nd.ni_dvp);
		error = vn_start_write(NULL, &mp, V_XSLEEP | PCATCH);
		if (error != 0) {
			goto out;
		}
		goto restart;
	}
	VATTR_NULL(&vattr);
	vattr.va_mode = ACCESSPERMS & ~td->td_proc->p_fd->fd_cmask;
	error = VOP_SYMLINK(nd.ni_dvp, &nd.ni_vp, &nd.ni_cnd, &vattr,
	    RTEMS_DECONST(char *, target));
	if (error != 0) {
		goto out;
	}
	NDFREE(&nd, NDF_ONLY_PNBUF);
	vput(nd.ni_dvp);
	vn_finished_write(mp);
out:
	return rtems_bsd_error_to_status_and_errno(error);
}

ssize_t
rtems_bsd_vfs_readlink(
    const rtems_filesystem_location_info_t *loc, char *buf, size_t bufsize)
{
	struct thread *td = curthread;
	struct vnode *vp = rtems_bsd_libio_loc_to_vnode(loc);
	struct iovec aiov;
	struct uio auio;
	int error;
	if (RTEMS_BSD_VFS_TRACE) {
		printf("bsd: vfs: readlink vn=%p\n", vp);
	}
	if (td == NULL) {
		if (RTEMS_BSD_VFS_TRACE) {
			printf("bsd: vfs: readlink: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	vn_lock(vp, LK_EXCLUSIVE | LK_RETRY);
	if (vp->v_type != VLNK && (vp->v_vflag & VV_READLINK) == 0) {
		error = EINVAL;
		goto out;
	}
	aiov.iov_base = buf;
	aiov.iov_len = bufsize;
	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	auio.uio_offset = 0;
	auio.uio_rw = UIO_READ;
	auio.uio_segflg = UIO_SYSSPACE;
	auio.uio_td = td;
	auio.uio_resid = bufsize;
	error = VOP_READLINK(vp, &auio, td->td_ucred);
	td->td_retval[0] = bufsize - auio.uio_resid;
out:
	VOP_UNLOCK(vp, 0);
	return rtems_bsd_error_to_status_and_errno(error);
}

int
rtems_bsd_vfs_rename(const rtems_filesystem_location_info_t *oldparentloc,
    const rtems_filesystem_location_info_t *oldloc,
    const rtems_filesystem_location_info_t *newparentloc, const char *name,
    size_t namelen)
{
	struct thread *td = curthread;
	struct mount *mp;
	struct vnode *olddvp = rtems_bsd_libio_loc_to_vnode(oldparentloc);
	struct vnode *oldvp = rtems_bsd_libio_loc_to_vnode(oldloc);
	struct vnode *newdvp = rtems_bsd_libio_loc_to_vnode(newparentloc);
	struct vnode *fvp;
	struct vnode *tdvp;
	struct vnode *tvp;
	struct nameidata fromnd;
	struct nameidata tond;
	struct componentname cn;
	char fromname[255];
	int error;
	if (RTEMS_BSD_VFS_TRACE) {
		printf("bsd: vfs: rename from=%p/%p to=%p/%s\n", olddvp, oldvp,
		    newdvp, name);
	}
	if (td == NULL) {
		if (RTEMS_BSD_VFS_TRACE) {
			printf("bsd: vfs: rename: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	error = rtems_bsd_vfs_vnode_componentname(
	    &cn, oldvp, DELETE, fromname, sizeof(fromname), td->td_ucred);
	if (error != 0) {
		if (RTEMS_BSD_VFS_TRACE) {
			printf(
			    "bsd: vfs: rename: componentname lookup: %p: (%d) %s\n",
			    oldvp, error, strerror(error));
		}
		goto out2;
	}
	if (RTEMS_BSD_VFS_TRACE) {
		printf("bsd: vfs: rename fromname=%s\n", cn.cn_nameptr);
	}
again:
	bwillwrite();
	NDINIT_ATVP(&fromnd, DELETE, WANTPARENT | SAVESTART | AUDITVNODE1,
	    UIO_SYSSPACE, cn.cn_nameptr, olddvp, td);
	error = namei(&fromnd);
	if (error != 0) {
		goto out2;
	}
	fvp = fromnd.ni_vp;
	NDINIT_ATVP(&tond, RENAME,
	    LOCKPARENT | LOCKLEAF | NOCACHE | SAVESTART | AUDITVNODE2,
	    UIO_SYSSPACE, name, newdvp, td);
	if (fromnd.ni_vp->v_type == VDIR)
		tond.ni_cnd.cn_flags |= WILLBEDIR;
	error = namei(&tond);
	if (error != 0) {
		/* Translate error code for rename("dir1", "dir2/."). */
		if (error == EISDIR && fvp->v_type == VDIR)
			error = EINVAL;
		NDFREE(&fromnd, NDF_ONLY_PNBUF);
		vrele(fromnd.ni_dvp);
		vrele(fvp);
		goto out1;
	}
	tdvp = tond.ni_dvp;
	tvp = tond.ni_vp;
	error = vn_start_write(fvp, &mp, V_NOWAIT);
	if (error != 0) {
		NDFREE(&fromnd, NDF_ONLY_PNBUF);
		NDFREE(&tond, NDF_ONLY_PNBUF);
		if (tvp != NULL)
			vput(tvp);
		if (tdvp == tvp)
			vrele(tdvp);
		else
			vput(tdvp);
		vrele(fromnd.ni_dvp);
		vrele(fvp);
		vrele(tond.ni_startdir);
		if (fromnd.ni_startdir != NULL)
			vrele(fromnd.ni_startdir);
		error = vn_start_write(NULL, &mp, V_XSLEEP | PCATCH);
		if (error != 0)
			return (error);
		goto again;
	}
	if (tvp != NULL) {
		if (fvp->v_type == VDIR && tvp->v_type != VDIR) {
			error = ENOTDIR;
			goto out;
		} else if (fvp->v_type != VDIR && tvp->v_type == VDIR) {
			error = EISDIR;
			goto out;
		}
	}
	if (fvp == tdvp) {
		error = EINVAL;
		goto out;
	}
out:
	if (error == 0) {
		error = VOP_RENAME(fromnd.ni_dvp, fromnd.ni_vp, &fromnd.ni_cnd,
		    tond.ni_dvp, tond.ni_vp, &tond.ni_cnd);
		NDFREE(&fromnd, NDF_ONLY_PNBUF);
		NDFREE(&tond, NDF_ONLY_PNBUF);
	} else {
		NDFREE(&fromnd, NDF_ONLY_PNBUF);
		NDFREE(&tond, NDF_ONLY_PNBUF);
		if (tvp != NULL)
			vput(tvp);
		if (tdvp == tvp)
			vrele(tdvp);
		else
			vput(tdvp);
		vrele(fromnd.ni_dvp);
		vrele(fvp);
	}
	vrele(tond.ni_startdir);
	vn_finished_write(mp);
out1:
	if (fromnd.ni_startdir)
		vrele(fromnd.ni_startdir);
	if (error == -1)
		error = 0;
out2:
	return rtems_bsd_error_to_status_and_errno(error);
}

int
rtems_bsd_vfs_statvfs(
    const rtems_filesystem_location_info_t *loc, struct statvfs *buf)
{
	struct thread *td = curthread;
	struct vnode *vp = rtems_bsd_libio_loc_to_vnode(loc);
	struct statfs *sp;
	struct mount *mp;
	int error;
	if (RTEMS_BSD_VFS_TRACE) {
		printf("bsd: vfs: statvfs\n");
	}
	if (td == NULL) {
		if (RTEMS_BSD_VFS_TRACE) {
			printf("bsd: vfs: statvfs: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	mp = vp->v_mount;
	sp = &mp->mnt_stat;
	sp->f_version = STATFS_VERSION;
	sp->f_namemax = NAME_MAX;
	sp->f_flags = mp->mnt_flag & MNT_VISFLAGMASK;
	error = VFS_STATFS(mp, sp);
	if (error == 0) {
		buf->f_bsize = sp->f_bsize;
		buf->f_frsize = sp->f_bsize;
		buf->f_blocks = sp->f_blocks;
		buf->f_bfree = sp->f_bfree;
		buf->f_bavail = sp->f_bavail;
		buf->f_files = sp->f_files;
		buf->f_ffree = sp->f_ffree;
		buf->f_fsid = sp->f_fsid.val[0];
		buf->f_flag = sp->f_flags;
		buf->f_namemax = sp->f_namemax;
	}
	return rtems_bsd_error_to_status_and_errno(error);
}
