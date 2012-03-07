/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from $FreeBSD$
 */

extern struct vnodeop_desc vop_default_desc;
#include <rtems/freebsd/local/vnode_if_typedef.h>
#include <rtems/freebsd/local/vnode_if_newproto.h>
struct vop_islocked_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
};

extern struct vnodeop_desc vop_islocked_desc;

int VOP_ISLOCKED_AP(struct vop_islocked_args *);
int VOP_ISLOCKED_APV(struct vop_vector *vop, struct vop_islocked_args *);

static __inline int VOP_ISLOCKED(
	struct vnode *vp)
{
	struct vop_islocked_args a;

	a.a_gen.a_desc = &vop_islocked_desc;
	a.a_vp = vp;
	return (VOP_ISLOCKED_APV(vp->v_op, &a));
}

struct vop_lookup_args {
	struct vop_generic_args a_gen;
	struct vnode *a_dvp;
	struct vnode **a_vpp;
	struct componentname *a_cnp;
};

extern struct vnodeop_desc vop_lookup_desc;

int VOP_LOOKUP_AP(struct vop_lookup_args *);
int VOP_LOOKUP_APV(struct vop_vector *vop, struct vop_lookup_args *);

static __inline int VOP_LOOKUP(
	struct vnode *dvp,
	struct vnode **vpp,
	struct componentname *cnp)
{
	struct vop_lookup_args a;

	a.a_gen.a_desc = &vop_lookup_desc;
	a.a_dvp = dvp;
	a.a_vpp = vpp;
	a.a_cnp = cnp;
	return (VOP_LOOKUP_APV(dvp->v_op, &a));
}

struct vop_cachedlookup_args {
	struct vop_generic_args a_gen;
	struct vnode *a_dvp;
	struct vnode **a_vpp;
	struct componentname *a_cnp;
};

extern struct vnodeop_desc vop_cachedlookup_desc;

int VOP_CACHEDLOOKUP_AP(struct vop_cachedlookup_args *);
int VOP_CACHEDLOOKUP_APV(struct vop_vector *vop, struct vop_cachedlookup_args *);

static __inline int VOP_CACHEDLOOKUP(
	struct vnode *dvp,
	struct vnode **vpp,
	struct componentname *cnp)
{
	struct vop_cachedlookup_args a;

	a.a_gen.a_desc = &vop_cachedlookup_desc;
	a.a_dvp = dvp;
	a.a_vpp = vpp;
	a.a_cnp = cnp;
	return (VOP_CACHEDLOOKUP_APV(dvp->v_op, &a));
}

struct vop_create_args {
	struct vop_generic_args a_gen;
	struct vnode *a_dvp;
	struct vnode **a_vpp;
	struct componentname *a_cnp;
	struct vattr *a_vap;
};

extern struct vnodeop_desc vop_create_desc;

int VOP_CREATE_AP(struct vop_create_args *);
int VOP_CREATE_APV(struct vop_vector *vop, struct vop_create_args *);

static __inline int VOP_CREATE(
	struct vnode *dvp,
	struct vnode **vpp,
	struct componentname *cnp,
	struct vattr *vap)
{
	struct vop_create_args a;

	a.a_gen.a_desc = &vop_create_desc;
	a.a_dvp = dvp;
	a.a_vpp = vpp;
	a.a_cnp = cnp;
	a.a_vap = vap;
	return (VOP_CREATE_APV(dvp->v_op, &a));
}

struct vop_whiteout_args {
	struct vop_generic_args a_gen;
	struct vnode *a_dvp;
	struct componentname *a_cnp;
	int a_flags;
};

extern struct vnodeop_desc vop_whiteout_desc;

int VOP_WHITEOUT_AP(struct vop_whiteout_args *);
int VOP_WHITEOUT_APV(struct vop_vector *vop, struct vop_whiteout_args *);

static __inline int VOP_WHITEOUT(
	struct vnode *dvp,
	struct componentname *cnp,
	int flags)
{
	struct vop_whiteout_args a;

	a.a_gen.a_desc = &vop_whiteout_desc;
	a.a_dvp = dvp;
	a.a_cnp = cnp;
	a.a_flags = flags;
	return (VOP_WHITEOUT_APV(dvp->v_op, &a));
}

struct vop_mknod_args {
	struct vop_generic_args a_gen;
	struct vnode *a_dvp;
	struct vnode **a_vpp;
	struct componentname *a_cnp;
	struct vattr *a_vap;
};

extern struct vnodeop_desc vop_mknod_desc;

int VOP_MKNOD_AP(struct vop_mknod_args *);
int VOP_MKNOD_APV(struct vop_vector *vop, struct vop_mknod_args *);

static __inline int VOP_MKNOD(
	struct vnode *dvp,
	struct vnode **vpp,
	struct componentname *cnp,
	struct vattr *vap)
{
	struct vop_mknod_args a;

	a.a_gen.a_desc = &vop_mknod_desc;
	a.a_dvp = dvp;
	a.a_vpp = vpp;
	a.a_cnp = cnp;
	a.a_vap = vap;
	return (VOP_MKNOD_APV(dvp->v_op, &a));
}

struct vop_open_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	int a_mode;
	struct ucred *a_cred;
	struct thread *a_td;
	struct file *a_fp;
};

extern struct vnodeop_desc vop_open_desc;

int VOP_OPEN_AP(struct vop_open_args *);
int VOP_OPEN_APV(struct vop_vector *vop, struct vop_open_args *);

static __inline int VOP_OPEN(
	struct vnode *vp,
	int mode,
	struct ucred *cred,
	struct thread *td,
	struct file *fp)
{
	struct vop_open_args a;

	a.a_gen.a_desc = &vop_open_desc;
	a.a_vp = vp;
	a.a_mode = mode;
	a.a_cred = cred;
	a.a_td = td;
	a.a_fp = fp;
	return (VOP_OPEN_APV(vp->v_op, &a));
}

struct vop_close_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	int a_fflag;
	struct ucred *a_cred;
	struct thread *a_td;
};

extern struct vnodeop_desc vop_close_desc;

int VOP_CLOSE_AP(struct vop_close_args *);
int VOP_CLOSE_APV(struct vop_vector *vop, struct vop_close_args *);

static __inline int VOP_CLOSE(
	struct vnode *vp,
	int fflag,
	struct ucred *cred,
	struct thread *td)
{
	struct vop_close_args a;

	a.a_gen.a_desc = &vop_close_desc;
	a.a_vp = vp;
	a.a_fflag = fflag;
	a.a_cred = cred;
	a.a_td = td;
	return (VOP_CLOSE_APV(vp->v_op, &a));
}

struct vop_access_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	accmode_t a_accmode;
	struct ucred *a_cred;
	struct thread *a_td;
};

extern struct vnodeop_desc vop_access_desc;

int VOP_ACCESS_AP(struct vop_access_args *);
int VOP_ACCESS_APV(struct vop_vector *vop, struct vop_access_args *);

static __inline int VOP_ACCESS(
	struct vnode *vp,
	accmode_t accmode,
	struct ucred *cred,
	struct thread *td)
{
	struct vop_access_args a;

	a.a_gen.a_desc = &vop_access_desc;
	a.a_vp = vp;
	a.a_accmode = accmode;
	a.a_cred = cred;
	a.a_td = td;
	return (VOP_ACCESS_APV(vp->v_op, &a));
}

struct vop_accessx_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	accmode_t a_accmode;
	struct ucred *a_cred;
	struct thread *a_td;
};

extern struct vnodeop_desc vop_accessx_desc;

int VOP_ACCESSX_AP(struct vop_accessx_args *);
int VOP_ACCESSX_APV(struct vop_vector *vop, struct vop_accessx_args *);

static __inline int VOP_ACCESSX(
	struct vnode *vp,
	accmode_t accmode,
	struct ucred *cred,
	struct thread *td)
{
	struct vop_accessx_args a;

	a.a_gen.a_desc = &vop_accessx_desc;
	a.a_vp = vp;
	a.a_accmode = accmode;
	a.a_cred = cred;
	a.a_td = td;
	return (VOP_ACCESSX_APV(vp->v_op, &a));
}

struct vop_getattr_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	struct vattr *a_vap;
	struct ucred *a_cred;
};

extern struct vnodeop_desc vop_getattr_desc;

int VOP_GETATTR_AP(struct vop_getattr_args *);
int VOP_GETATTR_APV(struct vop_vector *vop, struct vop_getattr_args *);

static __inline int VOP_GETATTR(
	struct vnode *vp,
	struct vattr *vap,
	struct ucred *cred)
{
	struct vop_getattr_args a;

	a.a_gen.a_desc = &vop_getattr_desc;
	a.a_vp = vp;
	a.a_vap = vap;
	a.a_cred = cred;
	return (VOP_GETATTR_APV(vp->v_op, &a));
}

struct vop_setattr_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	struct vattr *a_vap;
	struct ucred *a_cred;
};

extern struct vnodeop_desc vop_setattr_desc;

int VOP_SETATTR_AP(struct vop_setattr_args *);
int VOP_SETATTR_APV(struct vop_vector *vop, struct vop_setattr_args *);

static __inline int VOP_SETATTR(
	struct vnode *vp,
	struct vattr *vap,
	struct ucred *cred)
{
	struct vop_setattr_args a;

	a.a_gen.a_desc = &vop_setattr_desc;
	a.a_vp = vp;
	a.a_vap = vap;
	a.a_cred = cred;
	return (VOP_SETATTR_APV(vp->v_op, &a));
}

struct vop_markatime_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
};

extern struct vnodeop_desc vop_markatime_desc;

int VOP_MARKATIME_AP(struct vop_markatime_args *);
int VOP_MARKATIME_APV(struct vop_vector *vop, struct vop_markatime_args *);

static __inline int VOP_MARKATIME(
	struct vnode *vp)
{
	struct vop_markatime_args a;

	a.a_gen.a_desc = &vop_markatime_desc;
	a.a_vp = vp;
	return (VOP_MARKATIME_APV(vp->v_op, &a));
}

struct vop_read_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	struct uio *a_uio;
	int a_ioflag;
	struct ucred *a_cred;
};

extern struct vnodeop_desc vop_read_desc;

int VOP_READ_AP(struct vop_read_args *);
int VOP_READ_APV(struct vop_vector *vop, struct vop_read_args *);

static __inline int VOP_READ(
	struct vnode *vp,
	struct uio *uio,
	int ioflag,
	struct ucred *cred)
{
	struct vop_read_args a;

	a.a_gen.a_desc = &vop_read_desc;
	a.a_vp = vp;
	a.a_uio = uio;
	a.a_ioflag = ioflag;
	a.a_cred = cred;
	return (VOP_READ_APV(vp->v_op, &a));
}

struct vop_write_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	struct uio *a_uio;
	int a_ioflag;
	struct ucred *a_cred;
};

extern struct vnodeop_desc vop_write_desc;

int VOP_WRITE_AP(struct vop_write_args *);
int VOP_WRITE_APV(struct vop_vector *vop, struct vop_write_args *);

static __inline int VOP_WRITE(
	struct vnode *vp,
	struct uio *uio,
	int ioflag,
	struct ucred *cred)
{
	struct vop_write_args a;

	a.a_gen.a_desc = &vop_write_desc;
	a.a_vp = vp;
	a.a_uio = uio;
	a.a_ioflag = ioflag;
	a.a_cred = cred;
	return (VOP_WRITE_APV(vp->v_op, &a));
}

struct vop_ioctl_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	u_long a_command;
	void *a_data;
	int a_fflag;
	struct ucred *a_cred;
	struct thread *a_td;
};

extern struct vnodeop_desc vop_ioctl_desc;

int VOP_IOCTL_AP(struct vop_ioctl_args *);
int VOP_IOCTL_APV(struct vop_vector *vop, struct vop_ioctl_args *);

static __inline int VOP_IOCTL(
	struct vnode *vp,
	u_long command,
	void *data,
	int fflag,
	struct ucred *cred,
	struct thread *td)
{
	struct vop_ioctl_args a;

	a.a_gen.a_desc = &vop_ioctl_desc;
	a.a_vp = vp;
	a.a_command = command;
	a.a_data = data;
	a.a_fflag = fflag;
	a.a_cred = cred;
	a.a_td = td;
	return (VOP_IOCTL_APV(vp->v_op, &a));
}

struct vop_poll_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	int a_events;
	struct ucred *a_cred;
	struct thread *a_td;
};

extern struct vnodeop_desc vop_poll_desc;

int VOP_POLL_AP(struct vop_poll_args *);
int VOP_POLL_APV(struct vop_vector *vop, struct vop_poll_args *);

static __inline int VOP_POLL(
	struct vnode *vp,
	int events,
	struct ucred *cred,
	struct thread *td)
{
	struct vop_poll_args a;

	a.a_gen.a_desc = &vop_poll_desc;
	a.a_vp = vp;
	a.a_events = events;
	a.a_cred = cred;
	a.a_td = td;
	return (VOP_POLL_APV(vp->v_op, &a));
}

struct vop_kqfilter_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	struct knote *a_kn;
};

extern struct vnodeop_desc vop_kqfilter_desc;

int VOP_KQFILTER_AP(struct vop_kqfilter_args *);
int VOP_KQFILTER_APV(struct vop_vector *vop, struct vop_kqfilter_args *);

static __inline int VOP_KQFILTER(
	struct vnode *vp,
	struct knote *kn)
{
	struct vop_kqfilter_args a;

	a.a_gen.a_desc = &vop_kqfilter_desc;
	a.a_vp = vp;
	a.a_kn = kn;
	return (VOP_KQFILTER_APV(vp->v_op, &a));
}

struct vop_revoke_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	int a_flags;
};

extern struct vnodeop_desc vop_revoke_desc;

int VOP_REVOKE_AP(struct vop_revoke_args *);
int VOP_REVOKE_APV(struct vop_vector *vop, struct vop_revoke_args *);

static __inline int VOP_REVOKE(
	struct vnode *vp,
	int flags)
{
	struct vop_revoke_args a;

	a.a_gen.a_desc = &vop_revoke_desc;
	a.a_vp = vp;
	a.a_flags = flags;
	return (VOP_REVOKE_APV(vp->v_op, &a));
}

struct vop_fsync_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	int a_waitfor;
	struct thread *a_td;
};

extern struct vnodeop_desc vop_fsync_desc;

int VOP_FSYNC_AP(struct vop_fsync_args *);
int VOP_FSYNC_APV(struct vop_vector *vop, struct vop_fsync_args *);

static __inline int VOP_FSYNC(
	struct vnode *vp,
	int waitfor,
	struct thread *td)
{
	struct vop_fsync_args a;

	a.a_gen.a_desc = &vop_fsync_desc;
	a.a_vp = vp;
	a.a_waitfor = waitfor;
	a.a_td = td;
	return (VOP_FSYNC_APV(vp->v_op, &a));
}

struct vop_remove_args {
	struct vop_generic_args a_gen;
	struct vnode *a_dvp;
	struct vnode *a_vp;
	struct componentname *a_cnp;
};

extern struct vnodeop_desc vop_remove_desc;

int VOP_REMOVE_AP(struct vop_remove_args *);
int VOP_REMOVE_APV(struct vop_vector *vop, struct vop_remove_args *);

static __inline int VOP_REMOVE(
	struct vnode *dvp,
	struct vnode *vp,
	struct componentname *cnp)
{
	struct vop_remove_args a;

	a.a_gen.a_desc = &vop_remove_desc;
	a.a_dvp = dvp;
	a.a_vp = vp;
	a.a_cnp = cnp;
	return (VOP_REMOVE_APV(dvp->v_op, &a));
}

struct vop_link_args {
	struct vop_generic_args a_gen;
	struct vnode *a_tdvp;
	struct vnode *a_vp;
	struct componentname *a_cnp;
};

extern struct vnodeop_desc vop_link_desc;

int VOP_LINK_AP(struct vop_link_args *);
int VOP_LINK_APV(struct vop_vector *vop, struct vop_link_args *);

static __inline int VOP_LINK(
	struct vnode *tdvp,
	struct vnode *vp,
	struct componentname *cnp)
{
	struct vop_link_args a;

	a.a_gen.a_desc = &vop_link_desc;
	a.a_tdvp = tdvp;
	a.a_vp = vp;
	a.a_cnp = cnp;
	return (VOP_LINK_APV(tdvp->v_op, &a));
}

struct vop_rename_args {
	struct vop_generic_args a_gen;
	struct vnode *a_fdvp;
	struct vnode *a_fvp;
	struct componentname *a_fcnp;
	struct vnode *a_tdvp;
	struct vnode *a_tvp;
	struct componentname *a_tcnp;
};

extern struct vnodeop_desc vop_rename_desc;

int VOP_RENAME_AP(struct vop_rename_args *);
int VOP_RENAME_APV(struct vop_vector *vop, struct vop_rename_args *);

static __inline int VOP_RENAME(
	struct vnode *fdvp,
	struct vnode *fvp,
	struct componentname *fcnp,
	struct vnode *tdvp,
	struct vnode *tvp,
	struct componentname *tcnp)
{
	struct vop_rename_args a;

	a.a_gen.a_desc = &vop_rename_desc;
	a.a_fdvp = fdvp;
	a.a_fvp = fvp;
	a.a_fcnp = fcnp;
	a.a_tdvp = tdvp;
	a.a_tvp = tvp;
	a.a_tcnp = tcnp;
	return (VOP_RENAME_APV(fdvp->v_op, &a));
}

struct vop_mkdir_args {
	struct vop_generic_args a_gen;
	struct vnode *a_dvp;
	struct vnode **a_vpp;
	struct componentname *a_cnp;
	struct vattr *a_vap;
};

extern struct vnodeop_desc vop_mkdir_desc;

int VOP_MKDIR_AP(struct vop_mkdir_args *);
int VOP_MKDIR_APV(struct vop_vector *vop, struct vop_mkdir_args *);

static __inline int VOP_MKDIR(
	struct vnode *dvp,
	struct vnode **vpp,
	struct componentname *cnp,
	struct vattr *vap)
{
	struct vop_mkdir_args a;

	a.a_gen.a_desc = &vop_mkdir_desc;
	a.a_dvp = dvp;
	a.a_vpp = vpp;
	a.a_cnp = cnp;
	a.a_vap = vap;
	return (VOP_MKDIR_APV(dvp->v_op, &a));
}

struct vop_rmdir_args {
	struct vop_generic_args a_gen;
	struct vnode *a_dvp;
	struct vnode *a_vp;
	struct componentname *a_cnp;
};

extern struct vnodeop_desc vop_rmdir_desc;

int VOP_RMDIR_AP(struct vop_rmdir_args *);
int VOP_RMDIR_APV(struct vop_vector *vop, struct vop_rmdir_args *);

static __inline int VOP_RMDIR(
	struct vnode *dvp,
	struct vnode *vp,
	struct componentname *cnp)
{
	struct vop_rmdir_args a;

	a.a_gen.a_desc = &vop_rmdir_desc;
	a.a_dvp = dvp;
	a.a_vp = vp;
	a.a_cnp = cnp;
	return (VOP_RMDIR_APV(dvp->v_op, &a));
}

struct vop_symlink_args {
	struct vop_generic_args a_gen;
	struct vnode *a_dvp;
	struct vnode **a_vpp;
	struct componentname *a_cnp;
	struct vattr *a_vap;
	char *a_target;
};

extern struct vnodeop_desc vop_symlink_desc;

int VOP_SYMLINK_AP(struct vop_symlink_args *);
int VOP_SYMLINK_APV(struct vop_vector *vop, struct vop_symlink_args *);

static __inline int VOP_SYMLINK(
	struct vnode *dvp,
	struct vnode **vpp,
	struct componentname *cnp,
	struct vattr *vap,
	char *target)
{
	struct vop_symlink_args a;

	a.a_gen.a_desc = &vop_symlink_desc;
	a.a_dvp = dvp;
	a.a_vpp = vpp;
	a.a_cnp = cnp;
	a.a_vap = vap;
	a.a_target = target;
	return (VOP_SYMLINK_APV(dvp->v_op, &a));
}

struct vop_readdir_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	struct uio *a_uio;
	struct ucred *a_cred;
	int *a_eofflag;
	int *a_ncookies;
	u_long **a_cookies;
};

extern struct vnodeop_desc vop_readdir_desc;

int VOP_READDIR_AP(struct vop_readdir_args *);
int VOP_READDIR_APV(struct vop_vector *vop, struct vop_readdir_args *);

static __inline int VOP_READDIR(
	struct vnode *vp,
	struct uio *uio,
	struct ucred *cred,
	int *eofflag,
	int *ncookies,
	u_long **cookies)
{
	struct vop_readdir_args a;

	a.a_gen.a_desc = &vop_readdir_desc;
	a.a_vp = vp;
	a.a_uio = uio;
	a.a_cred = cred;
	a.a_eofflag = eofflag;
	a.a_ncookies = ncookies;
	a.a_cookies = cookies;
	return (VOP_READDIR_APV(vp->v_op, &a));
}

struct vop_readlink_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	struct uio *a_uio;
	struct ucred *a_cred;
};

extern struct vnodeop_desc vop_readlink_desc;

int VOP_READLINK_AP(struct vop_readlink_args *);
int VOP_READLINK_APV(struct vop_vector *vop, struct vop_readlink_args *);

static __inline int VOP_READLINK(
	struct vnode *vp,
	struct uio *uio,
	struct ucred *cred)
{
	struct vop_readlink_args a;

	a.a_gen.a_desc = &vop_readlink_desc;
	a.a_vp = vp;
	a.a_uio = uio;
	a.a_cred = cred;
	return (VOP_READLINK_APV(vp->v_op, &a));
}

struct vop_inactive_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	struct thread *a_td;
};

extern struct vnodeop_desc vop_inactive_desc;

int VOP_INACTIVE_AP(struct vop_inactive_args *);
int VOP_INACTIVE_APV(struct vop_vector *vop, struct vop_inactive_args *);

static __inline int VOP_INACTIVE(
	struct vnode *vp,
	struct thread *td)
{
	struct vop_inactive_args a;

	a.a_gen.a_desc = &vop_inactive_desc;
	a.a_vp = vp;
	a.a_td = td;
	return (VOP_INACTIVE_APV(vp->v_op, &a));
}

struct vop_reclaim_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	struct thread *a_td;
};

extern struct vnodeop_desc vop_reclaim_desc;

int VOP_RECLAIM_AP(struct vop_reclaim_args *);
int VOP_RECLAIM_APV(struct vop_vector *vop, struct vop_reclaim_args *);

static __inline int VOP_RECLAIM(
	struct vnode *vp,
	struct thread *td)
{
	struct vop_reclaim_args a;

	a.a_gen.a_desc = &vop_reclaim_desc;
	a.a_vp = vp;
	a.a_td = td;
	return (VOP_RECLAIM_APV(vp->v_op, &a));
}

struct vop_lock1_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	int a_flags;
	char *a_file;
	int a_line;
};

extern struct vnodeop_desc vop_lock1_desc;

int VOP_LOCK1_AP(struct vop_lock1_args *);
int VOP_LOCK1_APV(struct vop_vector *vop, struct vop_lock1_args *);

static __inline int VOP_LOCK1(
	struct vnode *vp,
	int flags,
	char *file,
	int line)
{
	struct vop_lock1_args a;

	a.a_gen.a_desc = &vop_lock1_desc;
	a.a_vp = vp;
	a.a_flags = flags;
	a.a_file = file;
	a.a_line = line;
	return (VOP_LOCK1_APV(vp->v_op, &a));
}

struct vop_unlock_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	int a_flags;
};

extern struct vnodeop_desc vop_unlock_desc;

int VOP_UNLOCK_AP(struct vop_unlock_args *);
int VOP_UNLOCK_APV(struct vop_vector *vop, struct vop_unlock_args *);

static __inline int VOP_UNLOCK(
	struct vnode *vp,
	int flags)
{
	struct vop_unlock_args a;

	a.a_gen.a_desc = &vop_unlock_desc;
	a.a_vp = vp;
	a.a_flags = flags;
	return (VOP_UNLOCK_APV(vp->v_op, &a));
}

struct vop_bmap_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	daddr_t a_bn;
	struct bufobj **a_bop;
	daddr_t *a_bnp;
	int *a_runp;
	int *a_runb;
};

extern struct vnodeop_desc vop_bmap_desc;

int VOP_BMAP_AP(struct vop_bmap_args *);
int VOP_BMAP_APV(struct vop_vector *vop, struct vop_bmap_args *);

static __inline int VOP_BMAP(
	struct vnode *vp,
	daddr_t bn,
	struct bufobj **bop,
	daddr_t *bnp,
	int *runp,
	int *runb)
{
	struct vop_bmap_args a;

	a.a_gen.a_desc = &vop_bmap_desc;
	a.a_vp = vp;
	a.a_bn = bn;
	a.a_bop = bop;
	a.a_bnp = bnp;
	a.a_runp = runp;
	a.a_runb = runb;
	return (VOP_BMAP_APV(vp->v_op, &a));
}

struct vop_strategy_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	struct buf *a_bp;
};

extern struct vnodeop_desc vop_strategy_desc;

int VOP_STRATEGY_AP(struct vop_strategy_args *);
int VOP_STRATEGY_APV(struct vop_vector *vop, struct vop_strategy_args *);

static __inline int VOP_STRATEGY(
	struct vnode *vp,
	struct buf *bp)
{
	struct vop_strategy_args a;

	a.a_gen.a_desc = &vop_strategy_desc;
	a.a_vp = vp;
	a.a_bp = bp;
	return (VOP_STRATEGY_APV(vp->v_op, &a));
}

struct vop_getwritemount_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	struct mount **a_mpp;
};

extern struct vnodeop_desc vop_getwritemount_desc;

int VOP_GETWRITEMOUNT_AP(struct vop_getwritemount_args *);
int VOP_GETWRITEMOUNT_APV(struct vop_vector *vop, struct vop_getwritemount_args *);

static __inline int VOP_GETWRITEMOUNT(
	struct vnode *vp,
	struct mount **mpp)
{
	struct vop_getwritemount_args a;

	a.a_gen.a_desc = &vop_getwritemount_desc;
	a.a_vp = vp;
	a.a_mpp = mpp;
	return (VOP_GETWRITEMOUNT_APV(vp->v_op, &a));
}

struct vop_print_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
};

extern struct vnodeop_desc vop_print_desc;

int VOP_PRINT_AP(struct vop_print_args *);
int VOP_PRINT_APV(struct vop_vector *vop, struct vop_print_args *);

static __inline int VOP_PRINT(
	struct vnode *vp)
{
	struct vop_print_args a;

	a.a_gen.a_desc = &vop_print_desc;
	a.a_vp = vp;
	return (VOP_PRINT_APV(vp->v_op, &a));
}

struct vop_pathconf_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	int a_name;
	register_t *a_retval;
};

extern struct vnodeop_desc vop_pathconf_desc;

int VOP_PATHCONF_AP(struct vop_pathconf_args *);
int VOP_PATHCONF_APV(struct vop_vector *vop, struct vop_pathconf_args *);

static __inline int VOP_PATHCONF(
	struct vnode *vp,
	int name,
	register_t *retval)
{
	struct vop_pathconf_args a;

	a.a_gen.a_desc = &vop_pathconf_desc;
	a.a_vp = vp;
	a.a_name = name;
	a.a_retval = retval;
	return (VOP_PATHCONF_APV(vp->v_op, &a));
}

struct vop_advlock_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	void *a_id;
	int a_op;
	struct flock *a_fl;
	int a_flags;
};

extern struct vnodeop_desc vop_advlock_desc;

int VOP_ADVLOCK_AP(struct vop_advlock_args *);
int VOP_ADVLOCK_APV(struct vop_vector *vop, struct vop_advlock_args *);

static __inline int VOP_ADVLOCK(
	struct vnode *vp,
	void *id,
	int op,
	struct flock *fl,
	int flags)
{
	struct vop_advlock_args a;

	a.a_gen.a_desc = &vop_advlock_desc;
	a.a_vp = vp;
	a.a_id = id;
	a.a_op = op;
	a.a_fl = fl;
	a.a_flags = flags;
	return (VOP_ADVLOCK_APV(vp->v_op, &a));
}

struct vop_advlockasync_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	void *a_id;
	int a_op;
	struct flock *a_fl;
	int a_flags;
	struct task *a_task;
	void **a_cookiep;
};

extern struct vnodeop_desc vop_advlockasync_desc;

int VOP_ADVLOCKASYNC_AP(struct vop_advlockasync_args *);
int VOP_ADVLOCKASYNC_APV(struct vop_vector *vop, struct vop_advlockasync_args *);

static __inline int VOP_ADVLOCKASYNC(
	struct vnode *vp,
	void *id,
	int op,
	struct flock *fl,
	int flags,
	struct task *task,
	void **cookiep)
{
	struct vop_advlockasync_args a;

	a.a_gen.a_desc = &vop_advlockasync_desc;
	a.a_vp = vp;
	a.a_id = id;
	a.a_op = op;
	a.a_fl = fl;
	a.a_flags = flags;
	a.a_task = task;
	a.a_cookiep = cookiep;
	return (VOP_ADVLOCKASYNC_APV(vp->v_op, &a));
}

struct vop_reallocblks_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	struct cluster_save *a_buflist;
};

extern struct vnodeop_desc vop_reallocblks_desc;

int VOP_REALLOCBLKS_AP(struct vop_reallocblks_args *);
int VOP_REALLOCBLKS_APV(struct vop_vector *vop, struct vop_reallocblks_args *);

static __inline int VOP_REALLOCBLKS(
	struct vnode *vp,
	struct cluster_save *buflist)
{
	struct vop_reallocblks_args a;

	a.a_gen.a_desc = &vop_reallocblks_desc;
	a.a_vp = vp;
	a.a_buflist = buflist;
	return (VOP_REALLOCBLKS_APV(vp->v_op, &a));
}

struct vop_getpages_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	vm_page_t *a_m;
	int a_count;
	int a_reqpage;
	vm_ooffset_t a_offset;
};

extern struct vnodeop_desc vop_getpages_desc;

int VOP_GETPAGES_AP(struct vop_getpages_args *);
int VOP_GETPAGES_APV(struct vop_vector *vop, struct vop_getpages_args *);

static __inline int VOP_GETPAGES(
	struct vnode *vp,
	vm_page_t *m,
	int count,
	int reqpage,
	vm_ooffset_t offset)
{
	struct vop_getpages_args a;

	a.a_gen.a_desc = &vop_getpages_desc;
	a.a_vp = vp;
	a.a_m = m;
	a.a_count = count;
	a.a_reqpage = reqpage;
	a.a_offset = offset;
	return (VOP_GETPAGES_APV(vp->v_op, &a));
}

struct vop_putpages_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	vm_page_t *a_m;
	int a_count;
	int a_sync;
	int *a_rtvals;
	vm_ooffset_t a_offset;
};

extern struct vnodeop_desc vop_putpages_desc;

int VOP_PUTPAGES_AP(struct vop_putpages_args *);
int VOP_PUTPAGES_APV(struct vop_vector *vop, struct vop_putpages_args *);

static __inline int VOP_PUTPAGES(
	struct vnode *vp,
	vm_page_t *m,
	int count,
	int sync,
	int *rtvals,
	vm_ooffset_t offset)
{
	struct vop_putpages_args a;

	a.a_gen.a_desc = &vop_putpages_desc;
	a.a_vp = vp;
	a.a_m = m;
	a.a_count = count;
	a.a_sync = sync;
	a.a_rtvals = rtvals;
	a.a_offset = offset;
	return (VOP_PUTPAGES_APV(vp->v_op, &a));
}

struct vop_getacl_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	acl_type_t a_type;
	struct acl *a_aclp;
	struct ucred *a_cred;
	struct thread *a_td;
};

extern struct vnodeop_desc vop_getacl_desc;

int VOP_GETACL_AP(struct vop_getacl_args *);
int VOP_GETACL_APV(struct vop_vector *vop, struct vop_getacl_args *);

static __inline int VOP_GETACL(
	struct vnode *vp,
	acl_type_t type,
	struct acl *aclp,
	struct ucred *cred,
	struct thread *td)
{
	struct vop_getacl_args a;

	a.a_gen.a_desc = &vop_getacl_desc;
	a.a_vp = vp;
	a.a_type = type;
	a.a_aclp = aclp;
	a.a_cred = cred;
	a.a_td = td;
	return (VOP_GETACL_APV(vp->v_op, &a));
}

struct vop_setacl_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	acl_type_t a_type;
	struct acl *a_aclp;
	struct ucred *a_cred;
	struct thread *a_td;
};

extern struct vnodeop_desc vop_setacl_desc;

int VOP_SETACL_AP(struct vop_setacl_args *);
int VOP_SETACL_APV(struct vop_vector *vop, struct vop_setacl_args *);

static __inline int VOP_SETACL(
	struct vnode *vp,
	acl_type_t type,
	struct acl *aclp,
	struct ucred *cred,
	struct thread *td)
{
	struct vop_setacl_args a;

	a.a_gen.a_desc = &vop_setacl_desc;
	a.a_vp = vp;
	a.a_type = type;
	a.a_aclp = aclp;
	a.a_cred = cred;
	a.a_td = td;
	return (VOP_SETACL_APV(vp->v_op, &a));
}

struct vop_aclcheck_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	acl_type_t a_type;
	struct acl *a_aclp;
	struct ucred *a_cred;
	struct thread *a_td;
};

extern struct vnodeop_desc vop_aclcheck_desc;

int VOP_ACLCHECK_AP(struct vop_aclcheck_args *);
int VOP_ACLCHECK_APV(struct vop_vector *vop, struct vop_aclcheck_args *);

static __inline int VOP_ACLCHECK(
	struct vnode *vp,
	acl_type_t type,
	struct acl *aclp,
	struct ucred *cred,
	struct thread *td)
{
	struct vop_aclcheck_args a;

	a.a_gen.a_desc = &vop_aclcheck_desc;
	a.a_vp = vp;
	a.a_type = type;
	a.a_aclp = aclp;
	a.a_cred = cred;
	a.a_td = td;
	return (VOP_ACLCHECK_APV(vp->v_op, &a));
}

struct vop_closeextattr_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	int a_commit;
	struct ucred *a_cred;
	struct thread *a_td;
};

extern struct vnodeop_desc vop_closeextattr_desc;

int VOP_CLOSEEXTATTR_AP(struct vop_closeextattr_args *);
int VOP_CLOSEEXTATTR_APV(struct vop_vector *vop, struct vop_closeextattr_args *);

static __inline int VOP_CLOSEEXTATTR(
	struct vnode *vp,
	int commit,
	struct ucred *cred,
	struct thread *td)
{
	struct vop_closeextattr_args a;

	a.a_gen.a_desc = &vop_closeextattr_desc;
	a.a_vp = vp;
	a.a_commit = commit;
	a.a_cred = cred;
	a.a_td = td;
	return (VOP_CLOSEEXTATTR_APV(vp->v_op, &a));
}

struct vop_getextattr_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	int a_attrnamespace;
	const char *a_name;
	struct uio *a_uio;
	size_t *a_size;
	struct ucred *a_cred;
	struct thread *a_td;
};

extern struct vnodeop_desc vop_getextattr_desc;

int VOP_GETEXTATTR_AP(struct vop_getextattr_args *);
int VOP_GETEXTATTR_APV(struct vop_vector *vop, struct vop_getextattr_args *);

static __inline int VOP_GETEXTATTR(
	struct vnode *vp,
	int attrnamespace,
	const char *name,
	struct uio *uio,
	size_t *size,
	struct ucred *cred,
	struct thread *td)
{
	struct vop_getextattr_args a;

	a.a_gen.a_desc = &vop_getextattr_desc;
	a.a_vp = vp;
	a.a_attrnamespace = attrnamespace;
	a.a_name = name;
	a.a_uio = uio;
	a.a_size = size;
	a.a_cred = cred;
	a.a_td = td;
	return (VOP_GETEXTATTR_APV(vp->v_op, &a));
}

struct vop_listextattr_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	int a_attrnamespace;
	struct uio *a_uio;
	size_t *a_size;
	struct ucred *a_cred;
	struct thread *a_td;
};

extern struct vnodeop_desc vop_listextattr_desc;

int VOP_LISTEXTATTR_AP(struct vop_listextattr_args *);
int VOP_LISTEXTATTR_APV(struct vop_vector *vop, struct vop_listextattr_args *);

static __inline int VOP_LISTEXTATTR(
	struct vnode *vp,
	int attrnamespace,
	struct uio *uio,
	size_t *size,
	struct ucred *cred,
	struct thread *td)
{
	struct vop_listextattr_args a;

	a.a_gen.a_desc = &vop_listextattr_desc;
	a.a_vp = vp;
	a.a_attrnamespace = attrnamespace;
	a.a_uio = uio;
	a.a_size = size;
	a.a_cred = cred;
	a.a_td = td;
	return (VOP_LISTEXTATTR_APV(vp->v_op, &a));
}

struct vop_openextattr_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	struct ucred *a_cred;
	struct thread *a_td;
};

extern struct vnodeop_desc vop_openextattr_desc;

int VOP_OPENEXTATTR_AP(struct vop_openextattr_args *);
int VOP_OPENEXTATTR_APV(struct vop_vector *vop, struct vop_openextattr_args *);

static __inline int VOP_OPENEXTATTR(
	struct vnode *vp,
	struct ucred *cred,
	struct thread *td)
{
	struct vop_openextattr_args a;

	a.a_gen.a_desc = &vop_openextattr_desc;
	a.a_vp = vp;
	a.a_cred = cred;
	a.a_td = td;
	return (VOP_OPENEXTATTR_APV(vp->v_op, &a));
}

struct vop_deleteextattr_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	int a_attrnamespace;
	const char *a_name;
	struct ucred *a_cred;
	struct thread *a_td;
};

extern struct vnodeop_desc vop_deleteextattr_desc;

int VOP_DELETEEXTATTR_AP(struct vop_deleteextattr_args *);
int VOP_DELETEEXTATTR_APV(struct vop_vector *vop, struct vop_deleteextattr_args *);

static __inline int VOP_DELETEEXTATTR(
	struct vnode *vp,
	int attrnamespace,
	const char *name,
	struct ucred *cred,
	struct thread *td)
{
	struct vop_deleteextattr_args a;

	a.a_gen.a_desc = &vop_deleteextattr_desc;
	a.a_vp = vp;
	a.a_attrnamespace = attrnamespace;
	a.a_name = name;
	a.a_cred = cred;
	a.a_td = td;
	return (VOP_DELETEEXTATTR_APV(vp->v_op, &a));
}

struct vop_setextattr_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	int a_attrnamespace;
	const char *a_name;
	struct uio *a_uio;
	struct ucred *a_cred;
	struct thread *a_td;
};

extern struct vnodeop_desc vop_setextattr_desc;

int VOP_SETEXTATTR_AP(struct vop_setextattr_args *);
int VOP_SETEXTATTR_APV(struct vop_vector *vop, struct vop_setextattr_args *);

static __inline int VOP_SETEXTATTR(
	struct vnode *vp,
	int attrnamespace,
	const char *name,
	struct uio *uio,
	struct ucred *cred,
	struct thread *td)
{
	struct vop_setextattr_args a;

	a.a_gen.a_desc = &vop_setextattr_desc;
	a.a_vp = vp;
	a.a_attrnamespace = attrnamespace;
	a.a_name = name;
	a.a_uio = uio;
	a.a_cred = cred;
	a.a_td = td;
	return (VOP_SETEXTATTR_APV(vp->v_op, &a));
}

struct vop_setlabel_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	struct label *a_label;
	struct ucred *a_cred;
	struct thread *a_td;
};

extern struct vnodeop_desc vop_setlabel_desc;

int VOP_SETLABEL_AP(struct vop_setlabel_args *);
int VOP_SETLABEL_APV(struct vop_vector *vop, struct vop_setlabel_args *);

static __inline int VOP_SETLABEL(
	struct vnode *vp,
	struct label *label,
	struct ucred *cred,
	struct thread *td)
{
	struct vop_setlabel_args a;

	a.a_gen.a_desc = &vop_setlabel_desc;
	a.a_vp = vp;
	a.a_label = label;
	a.a_cred = cred;
	a.a_td = td;
	return (VOP_SETLABEL_APV(vp->v_op, &a));
}

struct vop_vptofh_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	struct fid *a_fhp;
};

extern struct vnodeop_desc vop_vptofh_desc;

int VOP_VPTOFH_AP(struct vop_vptofh_args *);
int VOP_VPTOFH_APV(struct vop_vector *vop, struct vop_vptofh_args *);

static __inline int VOP_VPTOFH(
	struct vnode *vp,
	struct fid *fhp)
{
	struct vop_vptofh_args a;

	a.a_gen.a_desc = &vop_vptofh_desc;
	a.a_vp = vp;
	a.a_fhp = fhp;
	return (VOP_VPTOFH_APV(vp->v_op, &a));
}

struct vop_vptocnp_args {
	struct vop_generic_args a_gen;
	struct vnode *a_vp;
	struct vnode **a_vpp;
	struct ucred *a_cred;
	char *a_buf;
	int *a_buflen;
};

extern struct vnodeop_desc vop_vptocnp_desc;

int VOP_VPTOCNP_AP(struct vop_vptocnp_args *);
int VOP_VPTOCNP_APV(struct vop_vector *vop, struct vop_vptocnp_args *);

static __inline int VOP_VPTOCNP(
	struct vnode *vp,
	struct vnode **vpp,
	struct ucred *cred,
	char *buf,
	int *buflen)
{
	struct vop_vptocnp_args a;

	a.a_gen.a_desc = &vop_vptocnp_desc;
	a.a_vp = vp;
	a.a_vpp = vpp;
	a.a_cred = cred;
	a.a_buf = buf;
	a.a_buflen = buflen;
	return (VOP_VPTOCNP_APV(vp->v_op, &a));
}

