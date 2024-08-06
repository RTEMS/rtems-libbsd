#include <machine/rtems-bsd-kernel-space.h>

/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 */

#include <sys/param.h>
#include <sys/event.h>
#include <sys/kernel.h>
#include <sys/mount.h>
#include <sys/sdt.h>
#include <sys/signalvar.h>
#include <sys/systm.h>
#include <sys/vnode.h>

SDT_PROVIDER_DECLARE(vfs);

struct vnodeop_desc vop_default_desc = {
	"default",
	0,
   0,
	(vop_bypass_t *)vop_panic,
	NULL,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_islocked_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_islocked_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_islocked, entry, "struct vnode *", "struct vop_islocked_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_islocked, return, "struct vnode *", "struct vop_islocked_args *", "int");


int
VOP_ISLOCKED_AP(struct vop_islocked_args *a)
{

	return(VOP_ISLOCKED_APV(a->a_vp->v_op, a));
}

int
VOP_ISLOCKED_APV(struct vop_vector *vop, struct vop_islocked_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_islocked_desc, a->a_vp,
	    ("Wrong a_desc in vop_islocked(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_islocked(%p, %p)", a->a_vp, a));
	KTR_START1(KTR_VOP, "VOP", "VOP_ISLOCKED", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_islocked(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_islocked, entry, a->a_vp, a);
		rc = vop->vop_islocked(a);
		SDT_PROBE3(vfs, vop, vop_islocked, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
	} else {
	}
	KTR_STOP1(KTR_VOP, "VOP", "VOP_ISLOCKED", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	return (rc);
}

struct vnodeop_desc vop_islocked_desc = {
	"vop_islocked",
	0,
	__offsetof(struct vop_vector, vop_islocked),
	(vop_bypass_t *)VOP_ISLOCKED_AP,
	vop_islocked_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_lookup_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_lookup_args,a_dvp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_lookup, entry, "struct vnode *", "struct vop_lookup_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_lookup, return, "struct vnode *", "struct vop_lookup_args *", "int");


int
VOP_LOOKUP_AP(struct vop_lookup_args *a)
{

	return(VOP_LOOKUP_APV(a->a_dvp->v_op, a));
}

int
VOP_LOOKUP_APV(struct vop_vector *vop, struct vop_lookup_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_lookup_desc, a->a_dvp,
	    ("Wrong a_desc in vop_lookup(%p, %p)", a->a_dvp, a));
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_lookup(%p, %p)", a->a_dvp, a));
	KTR_START3(KTR_VOP, "VOP", "VOP_LOOKUP", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp);
	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_LOOKUP Entry (dvp)");
	ASSERT_VOP_LOCKED(a->a_dvp, "VOP_LOOKUP Entry (dvp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_lookup(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_lookup, entry, a->a_dvp, a);
		rc = vop->vop_lookup(a);
		SDT_PROBE3(vfs, vop, vop_lookup, return, a->a_dvp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_LOOKUP OK (dvp)");
		ASSERT_VOP_LOCKED(a->a_dvp, "VOP_LOOKUP OK (dvp)");
		ASSERT_VI_UNLOCKED(*a->a_vpp, "VOP_LOOKUP OK (vpp)");
		ASSERT_VOP_LOCKED(*a->a_vpp, "VOP_LOOKUP OK (vpp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_LOOKUP Error (dvp)");
		ASSERT_VOP_LOCKED(a->a_dvp, "VOP_LOOKUP Error (dvp)");
	}
	KTR_STOP3(KTR_VOP, "VOP", "VOP_LOOKUP", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp);
	return (rc);
}

struct vnodeop_desc vop_lookup_desc = {
	"vop_lookup",
	0,
	__offsetof(struct vop_vector, vop_lookup),
	(vop_bypass_t *)VOP_LOOKUP_AP,
	vop_lookup_vp_offsets,
	VOPARG_OFFSETOF(struct vop_lookup_args,a_vpp),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_lookup_args,a_cnp),
};

static int vop_cachedlookup_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_cachedlookup_args,a_dvp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_cachedlookup, entry, "struct vnode *", "struct vop_cachedlookup_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_cachedlookup, return, "struct vnode *", "struct vop_cachedlookup_args *", "int");


int
VOP_CACHEDLOOKUP_AP(struct vop_cachedlookup_args *a)
{

	return(VOP_CACHEDLOOKUP_APV(a->a_dvp->v_op, a));
}

int
VOP_CACHEDLOOKUP_APV(struct vop_vector *vop, struct vop_cachedlookup_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_cachedlookup_desc, a->a_dvp,
	    ("Wrong a_desc in vop_cachedlookup(%p, %p)", a->a_dvp, a));
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_cachedlookup(%p, %p)", a->a_dvp, a));
	KTR_START3(KTR_VOP, "VOP", "VOP_CACHEDLOOKUP", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp);
	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_CACHEDLOOKUP Entry (dvp)");
	ASSERT_VOP_LOCKED(a->a_dvp, "VOP_CACHEDLOOKUP Entry (dvp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_cachedlookup(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_cachedlookup, entry, a->a_dvp, a);
		rc = vop->vop_cachedlookup(a);
		SDT_PROBE3(vfs, vop, vop_cachedlookup, return, a->a_dvp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_CACHEDLOOKUP OK (dvp)");
		ASSERT_VOP_LOCKED(a->a_dvp, "VOP_CACHEDLOOKUP OK (dvp)");
		ASSERT_VI_UNLOCKED(*a->a_vpp, "VOP_CACHEDLOOKUP OK (vpp)");
		ASSERT_VOP_LOCKED(*a->a_vpp, "VOP_CACHEDLOOKUP OK (vpp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_CACHEDLOOKUP Error (dvp)");
		ASSERT_VOP_LOCKED(a->a_dvp, "VOP_CACHEDLOOKUP Error (dvp)");
	}
	KTR_STOP3(KTR_VOP, "VOP", "VOP_CACHEDLOOKUP", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp);
	return (rc);
}

struct vnodeop_desc vop_cachedlookup_desc = {
	"vop_cachedlookup",
	0,
	__offsetof(struct vop_vector, vop_cachedlookup),
	(vop_bypass_t *)VOP_CACHEDLOOKUP_AP,
	vop_cachedlookup_vp_offsets,
	VOPARG_OFFSETOF(struct vop_cachedlookup_args,a_vpp),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_cachedlookup_args,a_cnp),
};

static int vop_create_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_create_args,a_dvp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_create, entry, "struct vnode *", "struct vop_create_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_create, return, "struct vnode *", "struct vop_create_args *", "int");


int
VOP_CREATE_AP(struct vop_create_args *a)
{

	return(VOP_CREATE_APV(a->a_dvp->v_op, a));
}

int
VOP_CREATE_APV(struct vop_vector *vop, struct vop_create_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_create_desc, a->a_dvp,
	    ("Wrong a_desc in vop_create(%p, %p)", a->a_dvp, a));
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_create(%p, %p)", a->a_dvp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_CREATE", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp, "vap:0x%jX", a->a_vap);
	vop_create_pre(a);
	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_CREATE Entry (dvp)");
	ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_CREATE Entry (dvp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_create(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_create, entry, a->a_dvp, a);
		rc = vop->vop_create(a);
		SDT_PROBE3(vfs, vop, vop_create, return, a->a_dvp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_CREATE OK (dvp)");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_CREATE OK (dvp)");
		ASSERT_VI_UNLOCKED(*a->a_vpp, "VOP_CREATE OK (vpp)");
		ASSERT_VOP_LOCKED(*a->a_vpp, "VOP_CREATE OK (vpp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_CREATE Error (dvp)");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_CREATE Error (dvp)");
	}
	vop_create_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_CREATE", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp, "vap:0x%jX", a->a_vap);
	return (rc);
}

struct vnodeop_desc vop_create_desc = {
	"vop_create",
	0,
	__offsetof(struct vop_vector, vop_create),
	(vop_bypass_t *)VOP_CREATE_AP,
	vop_create_vp_offsets,
	VOPARG_OFFSETOF(struct vop_create_args,a_vpp),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_create_args,a_cnp),
};

static int vop_whiteout_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_whiteout_args,a_dvp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_whiteout, entry, "struct vnode *", "struct vop_whiteout_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_whiteout, return, "struct vnode *", "struct vop_whiteout_args *", "int");


int
VOP_WHITEOUT_AP(struct vop_whiteout_args *a)
{

	return(VOP_WHITEOUT_APV(a->a_dvp->v_op, a));
}

int
VOP_WHITEOUT_APV(struct vop_vector *vop, struct vop_whiteout_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_whiteout_desc, a->a_dvp,
	    ("Wrong a_desc in vop_whiteout(%p, %p)", a->a_dvp, a));
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_whiteout(%p, %p)", a->a_dvp, a));
	KTR_START3(KTR_VOP, "VOP", "VOP_WHITEOUT", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "cnp:0x%jX", a->a_cnp, "flags:0x%jX", a->a_flags);
	vop_whiteout_pre(a);
	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_WHITEOUT Entry (dvp)");
	ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_WHITEOUT Entry (dvp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_whiteout(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_whiteout, entry, a->a_dvp, a);
		rc = vop->vop_whiteout(a);
		SDT_PROBE3(vfs, vop, vop_whiteout, return, a->a_dvp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_WHITEOUT OK (dvp)");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_WHITEOUT OK (dvp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_WHITEOUT Error (dvp)");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_WHITEOUT Error (dvp)");
	}
	vop_whiteout_post(a, rc);
	KTR_STOP3(KTR_VOP, "VOP", "VOP_WHITEOUT", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "cnp:0x%jX", a->a_cnp, "flags:0x%jX", a->a_flags);
	return (rc);
}

struct vnodeop_desc vop_whiteout_desc = {
	"vop_whiteout",
	0,
	__offsetof(struct vop_vector, vop_whiteout),
	(vop_bypass_t *)VOP_WHITEOUT_AP,
	vop_whiteout_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_whiteout_args,a_cnp),
};

static int vop_mknod_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_mknod_args,a_dvp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_mknod, entry, "struct vnode *", "struct vop_mknod_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_mknod, return, "struct vnode *", "struct vop_mknod_args *", "int");


int
VOP_MKNOD_AP(struct vop_mknod_args *a)
{

	return(VOP_MKNOD_APV(a->a_dvp->v_op, a));
}

int
VOP_MKNOD_APV(struct vop_vector *vop, struct vop_mknod_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_mknod_desc, a->a_dvp,
	    ("Wrong a_desc in vop_mknod(%p, %p)", a->a_dvp, a));
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_mknod(%p, %p)", a->a_dvp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_MKNOD", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp, "vap:0x%jX", a->a_vap);
	vop_mknod_pre(a);
	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_MKNOD Entry (dvp)");
	ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_MKNOD Entry (dvp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_mknod(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_mknod, entry, a->a_dvp, a);
		rc = vop->vop_mknod(a);
		SDT_PROBE3(vfs, vop, vop_mknod, return, a->a_dvp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_MKNOD OK (dvp)");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_MKNOD OK (dvp)");
		ASSERT_VI_UNLOCKED(*a->a_vpp, "VOP_MKNOD OK (vpp)");
		ASSERT_VOP_LOCKED(*a->a_vpp, "VOP_MKNOD OK (vpp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_MKNOD Error (dvp)");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_MKNOD Error (dvp)");
	}
	vop_mknod_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_MKNOD", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp, "vap:0x%jX", a->a_vap);
	return (rc);
}

struct vnodeop_desc vop_mknod_desc = {
	"vop_mknod",
	0,
	__offsetof(struct vop_vector, vop_mknod),
	(vop_bypass_t *)VOP_MKNOD_AP,
	vop_mknod_vp_offsets,
	VOPARG_OFFSETOF(struct vop_mknod_args,a_vpp),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_mknod_args,a_cnp),
};

static int vop_open_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_open_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_open, entry, "struct vnode *", "struct vop_open_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_open, return, "struct vnode *", "struct vop_open_args *", "int");


int
VOP_OPEN_AP(struct vop_open_args *a)
{

	return(VOP_OPEN_APV(a->a_vp->v_op, a));
}

int
VOP_OPEN_APV(struct vop_vector *vop, struct vop_open_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_open_desc, a->a_vp,
	    ("Wrong a_desc in vop_open(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_open(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_OPEN", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "mode:0x%jX", a->a_mode, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_OPEN Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_OPEN Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_open(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_open, entry, a->a_vp, a);
		rc = vop->vop_open(a);
		SDT_PROBE3(vfs, vop, vop_open, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_OPEN OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_OPEN OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_OPEN Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_OPEN Error (vp)");
	}
	vop_open_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_OPEN", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "mode:0x%jX", a->a_mode, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	return (rc);
}

struct vnodeop_desc vop_open_desc = {
	"vop_open",
	0,
	__offsetof(struct vop_vector, vop_open),
	(vop_bypass_t *)VOP_OPEN_AP,
	vop_open_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_open_args,a_cred),
	VOPARG_OFFSETOF(struct vop_open_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_close_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_close_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_close, entry, "struct vnode *", "struct vop_close_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_close, return, "struct vnode *", "struct vop_close_args *", "int");


int
VOP_CLOSE_AP(struct vop_close_args *a)
{

	return(VOP_CLOSE_APV(a->a_vp->v_op, a));
}

int
VOP_CLOSE_APV(struct vop_vector *vop, struct vop_close_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_close_desc, a->a_vp,
	    ("Wrong a_desc in vop_close(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_close(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_CLOSE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "fflag:0x%jX", a->a_fflag, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_CLOSE Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_CLOSE Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_close(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_close, entry, a->a_vp, a);
		rc = vop->vop_close(a);
		SDT_PROBE3(vfs, vop, vop_close, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_CLOSE OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_CLOSE OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_CLOSE Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_CLOSE Error (vp)");
	}
	vop_close_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_CLOSE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "fflag:0x%jX", a->a_fflag, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	return (rc);
}

struct vnodeop_desc vop_close_desc = {
	"vop_close",
	0,
	__offsetof(struct vop_vector, vop_close),
	(vop_bypass_t *)VOP_CLOSE_AP,
	vop_close_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_close_args,a_cred),
	VOPARG_OFFSETOF(struct vop_close_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_fplookup_vexec_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_fplookup_vexec_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_fplookup_vexec, entry, "struct vnode *", "struct vop_fplookup_vexec_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_fplookup_vexec, return, "struct vnode *", "struct vop_fplookup_vexec_args *", "int");


int
VOP_FPLOOKUP_VEXEC_AP(struct vop_fplookup_vexec_args *a)
{

	return(VOP_FPLOOKUP_VEXEC_APV(a->a_vp->v_op, a));
}

int
VOP_FPLOOKUP_VEXEC_APV(struct vop_vector *vop, struct vop_fplookup_vexec_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_fplookup_vexec_desc, a->a_vp,
	    ("Wrong a_desc in vop_fplookup_vexec(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_fplookup_vexec(%p, %p)", a->a_vp, a));
	KTR_START2(KTR_VOP, "VOP", "VOP_FPLOOKUP_VEXEC", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "cred:0x%jX", a->a_cred);
#ifdef DEBUG_VFS_LOCKS
	vop_fplookup_vexec_debugpre(a);
#endif
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_fplookup_vexec(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_fplookup_vexec, entry, a->a_vp, a);
		rc = vop->vop_fplookup_vexec(a);
		SDT_PROBE3(vfs, vop, vop_fplookup_vexec, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
	} else {
	}
#ifdef DEBUG_VFS_LOCKS
	vop_fplookup_vexec_debugpost(a, rc);
#endif
	KTR_STOP2(KTR_VOP, "VOP", "VOP_FPLOOKUP_VEXEC", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "cred:0x%jX", a->a_cred);
	return (rc);
}

struct vnodeop_desc vop_fplookup_vexec_desc = {
	"vop_fplookup_vexec",
	0,
	__offsetof(struct vop_vector, vop_fplookup_vexec),
	(vop_bypass_t *)VOP_FPLOOKUP_VEXEC_AP,
	vop_fplookup_vexec_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_fplookup_vexec_args,a_cred),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_fplookup_symlink_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_fplookup_symlink_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_fplookup_symlink, entry, "struct vnode *", "struct vop_fplookup_symlink_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_fplookup_symlink, return, "struct vnode *", "struct vop_fplookup_symlink_args *", "int");


int
VOP_FPLOOKUP_SYMLINK_AP(struct vop_fplookup_symlink_args *a)
{

	return(VOP_FPLOOKUP_SYMLINK_APV(a->a_vp->v_op, a));
}

int
VOP_FPLOOKUP_SYMLINK_APV(struct vop_vector *vop, struct vop_fplookup_symlink_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_fplookup_symlink_desc, a->a_vp,
	    ("Wrong a_desc in vop_fplookup_symlink(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_fplookup_symlink(%p, %p)", a->a_vp, a));
	KTR_START2(KTR_VOP, "VOP", "VOP_FPLOOKUP_SYMLINK", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "fpl:0x%jX", a->a_fpl);
#ifdef DEBUG_VFS_LOCKS
	vop_fplookup_symlink_debugpre(a);
#endif
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_fplookup_symlink(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_fplookup_symlink, entry, a->a_vp, a);
		rc = vop->vop_fplookup_symlink(a);
		SDT_PROBE3(vfs, vop, vop_fplookup_symlink, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
	} else {
	}
#ifdef DEBUG_VFS_LOCKS
	vop_fplookup_symlink_debugpost(a, rc);
#endif
	KTR_STOP2(KTR_VOP, "VOP", "VOP_FPLOOKUP_SYMLINK", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "fpl:0x%jX", a->a_fpl);
	return (rc);
}

struct vnodeop_desc vop_fplookup_symlink_desc = {
	"vop_fplookup_symlink",
	0,
	__offsetof(struct vop_vector, vop_fplookup_symlink),
	(vop_bypass_t *)VOP_FPLOOKUP_SYMLINK_AP,
	vop_fplookup_symlink_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_access_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_access_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_access, entry, "struct vnode *", "struct vop_access_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_access, return, "struct vnode *", "struct vop_access_args *", "int");


int
VOP_ACCESS_AP(struct vop_access_args *a)
{

	return(VOP_ACCESS_APV(a->a_vp->v_op, a));
}

int
VOP_ACCESS_APV(struct vop_vector *vop, struct vop_access_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_access_desc, a->a_vp,
	    ("Wrong a_desc in vop_access(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_access(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_ACCESS", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "accmode:0x%jX", a->a_accmode, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACCESS Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_ACCESS Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_access(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_access, entry, a->a_vp, a);
		rc = vop->vop_access(a);
		SDT_PROBE3(vfs, vop, vop_access, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACCESS OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_ACCESS OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACCESS Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_ACCESS Error (vp)");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_ACCESS", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "accmode:0x%jX", a->a_accmode, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	return (rc);
}

struct vnodeop_desc vop_access_desc = {
	"vop_access",
	0,
	__offsetof(struct vop_vector, vop_access),
	(vop_bypass_t *)VOP_ACCESS_AP,
	vop_access_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_access_args,a_cred),
	VOPARG_OFFSETOF(struct vop_access_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_accessx_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_accessx_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_accessx, entry, "struct vnode *", "struct vop_accessx_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_accessx, return, "struct vnode *", "struct vop_accessx_args *", "int");


int
VOP_ACCESSX_AP(struct vop_accessx_args *a)
{

	return(VOP_ACCESSX_APV(a->a_vp->v_op, a));
}

int
VOP_ACCESSX_APV(struct vop_vector *vop, struct vop_accessx_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_accessx_desc, a->a_vp,
	    ("Wrong a_desc in vop_accessx(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_accessx(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_ACCESSX", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "accmode:0x%jX", a->a_accmode, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACCESSX Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_ACCESSX Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_accessx(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_accessx, entry, a->a_vp, a);
		rc = vop->vop_accessx(a);
		SDT_PROBE3(vfs, vop, vop_accessx, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACCESSX OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_ACCESSX OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACCESSX Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_ACCESSX Error (vp)");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_ACCESSX", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "accmode:0x%jX", a->a_accmode, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	return (rc);
}

struct vnodeop_desc vop_accessx_desc = {
	"vop_accessx",
	0,
	__offsetof(struct vop_vector, vop_accessx),
	(vop_bypass_t *)VOP_ACCESSX_AP,
	vop_accessx_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_accessx_args,a_cred),
	VOPARG_OFFSETOF(struct vop_accessx_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_stat_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_stat_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_stat, entry, "struct vnode *", "struct vop_stat_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_stat, return, "struct vnode *", "struct vop_stat_args *", "int");


int
VOP_STAT_AP(struct vop_stat_args *a)
{

	return(VOP_STAT_APV(a->a_vp->v_op, a));
}

int
VOP_STAT_APV(struct vop_vector *vop, struct vop_stat_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_stat_desc, a->a_vp,
	    ("Wrong a_desc in vop_stat(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_stat(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_STAT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "sb:0x%jX", a->a_sb, "active_cred:0x%jX", a->a_active_cred, "file_cred:0x%jX", a->a_file_cred);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_STAT Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_STAT Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_stat(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_stat, entry, a->a_vp, a);
		rc = vop->vop_stat(a);
		SDT_PROBE3(vfs, vop, vop_stat, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_STAT OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_STAT OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_STAT Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_STAT Error (vp)");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_STAT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "sb:0x%jX", a->a_sb, "active_cred:0x%jX", a->a_active_cred, "file_cred:0x%jX", a->a_file_cred);
	return (rc);
}

struct vnodeop_desc vop_stat_desc = {
	"vop_stat",
	0,
	__offsetof(struct vop_vector, vop_stat),
	(vop_bypass_t *)VOP_STAT_AP,
	vop_stat_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_stat_args,a_active_cred),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_getattr_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_getattr_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_getattr, entry, "struct vnode *", "struct vop_getattr_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_getattr, return, "struct vnode *", "struct vop_getattr_args *", "int");


int
VOP_GETATTR_AP(struct vop_getattr_args *a)
{

	return(VOP_GETATTR_APV(a->a_vp->v_op, a));
}

int
VOP_GETATTR_APV(struct vop_vector *vop, struct vop_getattr_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_getattr_desc, a->a_vp,
	    ("Wrong a_desc in vop_getattr(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_getattr(%p, %p)", a->a_vp, a));
	KTR_START3(KTR_VOP, "VOP", "VOP_GETATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "vap:0x%jX", a->a_vap, "cred:0x%jX", a->a_cred);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETATTR Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETATTR Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_getattr(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_getattr, entry, a->a_vp, a);
		rc = vop->vop_getattr(a);
		SDT_PROBE3(vfs, vop, vop_getattr, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETATTR OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETATTR OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETATTR Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETATTR Error (vp)");
	}
	KTR_STOP3(KTR_VOP, "VOP", "VOP_GETATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "vap:0x%jX", a->a_vap, "cred:0x%jX", a->a_cred);
	return (rc);
}

struct vnodeop_desc vop_getattr_desc = {
	"vop_getattr",
	0,
	__offsetof(struct vop_vector, vop_getattr),
	(vop_bypass_t *)VOP_GETATTR_AP,
	vop_getattr_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_getattr_args,a_cred),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_setattr_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_setattr_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_setattr, entry, "struct vnode *", "struct vop_setattr_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_setattr, return, "struct vnode *", "struct vop_setattr_args *", "int");


int
VOP_SETATTR_AP(struct vop_setattr_args *a)
{

	return(VOP_SETATTR_APV(a->a_vp->v_op, a));
}

int
VOP_SETATTR_APV(struct vop_vector *vop, struct vop_setattr_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_setattr_desc, a->a_vp,
	    ("Wrong a_desc in vop_setattr(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_setattr(%p, %p)", a->a_vp, a));
	KTR_START3(KTR_VOP, "VOP", "VOP_SETATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "vap:0x%jX", a->a_vap, "cred:0x%jX", a->a_cred);
	vop_setattr_pre(a);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETATTR Entry (vp)");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETATTR Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_setattr(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_setattr, entry, a->a_vp, a);
		rc = vop->vop_setattr(a);
		SDT_PROBE3(vfs, vop, vop_setattr, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETATTR OK (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETATTR OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETATTR Error (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETATTR Error (vp)");
	}
	vop_setattr_post(a, rc);
	KTR_STOP3(KTR_VOP, "VOP", "VOP_SETATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "vap:0x%jX", a->a_vap, "cred:0x%jX", a->a_cred);
	return (rc);
}

struct vnodeop_desc vop_setattr_desc = {
	"vop_setattr",
	0,
	__offsetof(struct vop_vector, vop_setattr),
	(vop_bypass_t *)VOP_SETATTR_AP,
	vop_setattr_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_setattr_args,a_cred),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_mmapped_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_mmapped_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_mmapped, entry, "struct vnode *", "struct vop_mmapped_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_mmapped, return, "struct vnode *", "struct vop_mmapped_args *", "int");


int
VOP_MMAPPED_AP(struct vop_mmapped_args *a)
{

	return(VOP_MMAPPED_APV(a->a_vp->v_op, a));
}

int
VOP_MMAPPED_APV(struct vop_vector *vop, struct vop_mmapped_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_mmapped_desc, a->a_vp,
	    ("Wrong a_desc in vop_mmapped(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_mmapped(%p, %p)", a->a_vp, a));
	KTR_START1(KTR_VOP, "VOP", "VOP_MMAPPED", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_MMAPPED Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_MMAPPED Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_mmapped(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_mmapped, entry, a->a_vp, a);
		rc = vop->vop_mmapped(a);
		SDT_PROBE3(vfs, vop, vop_mmapped, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_MMAPPED OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_MMAPPED OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_MMAPPED Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_MMAPPED Error (vp)");
	}
	KTR_STOP1(KTR_VOP, "VOP", "VOP_MMAPPED", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	return (rc);
}

struct vnodeop_desc vop_mmapped_desc = {
	"vop_mmapped",
	0,
	__offsetof(struct vop_vector, vop_mmapped),
	(vop_bypass_t *)VOP_MMAPPED_AP,
	vop_mmapped_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_read_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_read_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_read, entry, "struct vnode *", "struct vop_read_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_read, return, "struct vnode *", "struct vop_read_args *", "int");


int
VOP_READ_AP(struct vop_read_args *a)
{

	return(VOP_READ_APV(a->a_vp->v_op, a));
}

int
VOP_READ_APV(struct vop_vector *vop, struct vop_read_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_read_desc, a->a_vp,
	    ("Wrong a_desc in vop_read(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_read(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_READ", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "uio:0x%jX", a->a_uio, "ioflag:0x%jX", a->a_ioflag, "cred:0x%jX", a->a_cred);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READ Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_READ Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_read(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_read, entry, a->a_vp, a);
		rc = vop->vop_read(a);
		SDT_PROBE3(vfs, vop, vop_read, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READ OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_READ OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READ Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_READ Error (vp)");
	}
	vop_read_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_READ", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "uio:0x%jX", a->a_uio, "ioflag:0x%jX", a->a_ioflag, "cred:0x%jX", a->a_cred);
	return (rc);
}

struct vnodeop_desc vop_read_desc = {
	"vop_read",
	0,
	__offsetof(struct vop_vector, vop_read),
	(vop_bypass_t *)VOP_READ_AP,
	vop_read_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_read_args,a_cred),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_read_pgcache_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_read_pgcache_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_read_pgcache, entry, "struct vnode *", "struct vop_read_pgcache_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_read_pgcache, return, "struct vnode *", "struct vop_read_pgcache_args *", "int");


int
VOP_READ_PGCACHE_AP(struct vop_read_pgcache_args *a)
{

	return(VOP_READ_PGCACHE_APV(a->a_vp->v_op, a));
}

int
VOP_READ_PGCACHE_APV(struct vop_vector *vop, struct vop_read_pgcache_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_read_pgcache_desc, a->a_vp,
	    ("Wrong a_desc in vop_read_pgcache(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_read_pgcache(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_READ_PGCACHE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "uio:0x%jX", a->a_uio, "ioflag:0x%jX", a->a_ioflag, "cred:0x%jX", a->a_cred);
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_read_pgcache(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_read_pgcache, entry, a->a_vp, a);
		rc = vop->vop_read_pgcache(a);
		SDT_PROBE3(vfs, vop, vop_read_pgcache, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
	} else {
	}
	vop_read_pgcache_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_READ_PGCACHE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "uio:0x%jX", a->a_uio, "ioflag:0x%jX", a->a_ioflag, "cred:0x%jX", a->a_cred);
	return (rc);
}

struct vnodeop_desc vop_read_pgcache_desc = {
	"vop_read_pgcache",
	0,
	__offsetof(struct vop_vector, vop_read_pgcache),
	(vop_bypass_t *)VOP_READ_PGCACHE_AP,
	vop_read_pgcache_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_read_pgcache_args,a_cred),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_write_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_write_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_write, entry, "struct vnode *", "struct vop_write_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_write, return, "struct vnode *", "struct vop_write_args *", "int");


int
VOP_WRITE_AP(struct vop_write_args *a)
{

	return(VOP_WRITE_APV(a->a_vp->v_op, a));
}

int
VOP_WRITE_APV(struct vop_vector *vop, struct vop_write_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_write_desc, a->a_vp,
	    ("Wrong a_desc in vop_write(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_write(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_WRITE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "uio:0x%jX", a->a_uio, "ioflag:0x%jX", a->a_ioflag, "cred:0x%jX", a->a_cred);
	VOP_WRITE_PRE(a);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_WRITE Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_WRITE Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_write(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_write, entry, a->a_vp, a);
		rc = vop->vop_write(a);
		SDT_PROBE3(vfs, vop, vop_write, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_WRITE OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_WRITE OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_WRITE Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_WRITE Error (vp)");
	}
	VOP_WRITE_POST(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_WRITE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "uio:0x%jX", a->a_uio, "ioflag:0x%jX", a->a_ioflag, "cred:0x%jX", a->a_cred);
	return (rc);
}

struct vnodeop_desc vop_write_desc = {
	"vop_write",
	0,
	__offsetof(struct vop_vector, vop_write),
	(vop_bypass_t *)VOP_WRITE_AP,
	vop_write_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_write_args,a_cred),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_ioctl_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_ioctl_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_ioctl, entry, "struct vnode *", "struct vop_ioctl_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_ioctl, return, "struct vnode *", "struct vop_ioctl_args *", "int");


int
VOP_IOCTL_AP(struct vop_ioctl_args *a)
{

	return(VOP_IOCTL_APV(a->a_vp->v_op, a));
}

int
VOP_IOCTL_APV(struct vop_vector *vop, struct vop_ioctl_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_ioctl_desc, a->a_vp,
	    ("Wrong a_desc in vop_ioctl(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_ioctl(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_IOCTL", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "command:0x%jX", a->a_command, "data:0x%jX", a->a_data, "fflag:0x%jX", a->a_fflag);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_IOCTL Entry (vp)");
	ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_IOCTL Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_ioctl(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_ioctl, entry, a->a_vp, a);
		rc = vop->vop_ioctl(a);
		SDT_PROBE3(vfs, vop, vop_ioctl, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_IOCTL OK (vp)");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_IOCTL OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_IOCTL Error (vp)");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_IOCTL Error (vp)");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_IOCTL", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "command:0x%jX", a->a_command, "data:0x%jX", a->a_data, "fflag:0x%jX", a->a_fflag);
	return (rc);
}

struct vnodeop_desc vop_ioctl_desc = {
	"vop_ioctl",
	0,
	__offsetof(struct vop_vector, vop_ioctl),
	(vop_bypass_t *)VOP_IOCTL_AP,
	vop_ioctl_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_ioctl_args,a_cred),
	VOPARG_OFFSETOF(struct vop_ioctl_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_poll_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_poll_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_poll, entry, "struct vnode *", "struct vop_poll_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_poll, return, "struct vnode *", "struct vop_poll_args *", "int");


int
VOP_POLL_AP(struct vop_poll_args *a)
{

	return(VOP_POLL_APV(a->a_vp->v_op, a));
}

int
VOP_POLL_APV(struct vop_vector *vop, struct vop_poll_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_poll_desc, a->a_vp,
	    ("Wrong a_desc in vop_poll(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_poll(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_POLL", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "events:0x%jX", a->a_events, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_POLL Entry (vp)");
	ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_POLL Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_poll(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_poll, entry, a->a_vp, a);
		rc = vop->vop_poll(a);
		SDT_PROBE3(vfs, vop, vop_poll, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_POLL OK (vp)");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_POLL OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_POLL Error (vp)");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_POLL Error (vp)");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_POLL", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "events:0x%jX", a->a_events, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	return (rc);
}

struct vnodeop_desc vop_poll_desc = {
	"vop_poll",
	0,
	__offsetof(struct vop_vector, vop_poll),
	(vop_bypass_t *)VOP_POLL_AP,
	vop_poll_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_poll_args,a_cred),
	VOPARG_OFFSETOF(struct vop_poll_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_kqfilter_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_kqfilter_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_kqfilter, entry, "struct vnode *", "struct vop_kqfilter_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_kqfilter, return, "struct vnode *", "struct vop_kqfilter_args *", "int");


int
VOP_KQFILTER_AP(struct vop_kqfilter_args *a)
{

	return(VOP_KQFILTER_APV(a->a_vp->v_op, a));
}

int
VOP_KQFILTER_APV(struct vop_vector *vop, struct vop_kqfilter_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_kqfilter_desc, a->a_vp,
	    ("Wrong a_desc in vop_kqfilter(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_kqfilter(%p, %p)", a->a_vp, a));
	KTR_START2(KTR_VOP, "VOP", "VOP_KQFILTER", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "kn:0x%jX", a->a_kn);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_KQFILTER Entry (vp)");
	ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_KQFILTER Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_kqfilter(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_kqfilter, entry, a->a_vp, a);
		rc = vop->vop_kqfilter(a);
		SDT_PROBE3(vfs, vop, vop_kqfilter, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_KQFILTER OK (vp)");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_KQFILTER OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_KQFILTER Error (vp)");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_KQFILTER Error (vp)");
	}
	KTR_STOP2(KTR_VOP, "VOP", "VOP_KQFILTER", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "kn:0x%jX", a->a_kn);
	return (rc);
}

struct vnodeop_desc vop_kqfilter_desc = {
	"vop_kqfilter",
	0,
	__offsetof(struct vop_vector, vop_kqfilter),
	(vop_bypass_t *)VOP_KQFILTER_AP,
	vop_kqfilter_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_revoke_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_revoke_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_revoke, entry, "struct vnode *", "struct vop_revoke_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_revoke, return, "struct vnode *", "struct vop_revoke_args *", "int");


int
VOP_REVOKE_AP(struct vop_revoke_args *a)
{

	return(VOP_REVOKE_APV(a->a_vp->v_op, a));
}

int
VOP_REVOKE_APV(struct vop_vector *vop, struct vop_revoke_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_revoke_desc, a->a_vp,
	    ("Wrong a_desc in vop_revoke(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_revoke(%p, %p)", a->a_vp, a));
	KTR_START2(KTR_VOP, "VOP", "VOP_REVOKE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "flags:0x%jX", a->a_flags);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REVOKE Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_REVOKE Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_revoke(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_revoke, entry, a->a_vp, a);
		rc = vop->vop_revoke(a);
		SDT_PROBE3(vfs, vop, vop_revoke, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REVOKE OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_REVOKE OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REVOKE Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_REVOKE Error (vp)");
	}
	KTR_STOP2(KTR_VOP, "VOP", "VOP_REVOKE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "flags:0x%jX", a->a_flags);
	return (rc);
}

struct vnodeop_desc vop_revoke_desc = {
	"vop_revoke",
	0,
	__offsetof(struct vop_vector, vop_revoke),
	(vop_bypass_t *)VOP_REVOKE_AP,
	vop_revoke_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_fsync_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_fsync_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_fsync, entry, "struct vnode *", "struct vop_fsync_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_fsync, return, "struct vnode *", "struct vop_fsync_args *", "int");


int
VOP_FSYNC_AP(struct vop_fsync_args *a)
{

	return(VOP_FSYNC_APV(a->a_vp->v_op, a));
}

int
VOP_FSYNC_APV(struct vop_vector *vop, struct vop_fsync_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_fsync_desc, a->a_vp,
	    ("Wrong a_desc in vop_fsync(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_fsync(%p, %p)", a->a_vp, a));
	KTR_START3(KTR_VOP, "VOP", "VOP_FSYNC", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "waitfor:0x%jX", a->a_waitfor, "td:0x%jX", a->a_td);
	vop_fsync_debugpre(a);
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_fsync(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_fsync, entry, a->a_vp, a);
		rc = vop->vop_fsync(a);
		SDT_PROBE3(vfs, vop, vop_fsync, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
	} else {
	}
	vop_fsync_debugpost(a, rc);
	KTR_STOP3(KTR_VOP, "VOP", "VOP_FSYNC", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "waitfor:0x%jX", a->a_waitfor, "td:0x%jX", a->a_td);
	return (rc);
}

struct vnodeop_desc vop_fsync_desc = {
	"vop_fsync",
	0,
	__offsetof(struct vop_vector, vop_fsync),
	(vop_bypass_t *)VOP_FSYNC_AP,
	vop_fsync_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_fsync_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_remove_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_remove_args,a_dvp),
	VOPARG_OFFSETOF(struct vop_remove_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_remove, entry, "struct vnode *", "struct vop_remove_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_remove, return, "struct vnode *", "struct vop_remove_args *", "int");


int
VOP_REMOVE_AP(struct vop_remove_args *a)
{

	return(VOP_REMOVE_APV(a->a_dvp->v_op, a));
}

int
VOP_REMOVE_APV(struct vop_vector *vop, struct vop_remove_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_remove_desc, a->a_dvp,
	    ("Wrong a_desc in vop_remove(%p, %p)", a->a_dvp, a));
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_remove(%p, %p)", a->a_dvp, a));
	KTR_START3(KTR_VOP, "VOP", "VOP_REMOVE", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vp:0x%jX", a->a_vp, "cnp:0x%jX", a->a_cnp);
	vop_remove_pre(a);
	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_REMOVE Entry (dvp)");
	ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_REMOVE Entry (dvp)");
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REMOVE Entry (vp)");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_REMOVE Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_remove(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_remove, entry, a->a_dvp, a);
		rc = vop->vop_remove(a);
		SDT_PROBE3(vfs, vop, vop_remove, return, a->a_dvp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_REMOVE OK (dvp)");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_REMOVE OK (dvp)");
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REMOVE OK (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_REMOVE OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_REMOVE Error (dvp)");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_REMOVE Error (dvp)");
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REMOVE Error (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_REMOVE Error (vp)");
	}
	vop_remove_post(a, rc);
	KTR_STOP3(KTR_VOP, "VOP", "VOP_REMOVE", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vp:0x%jX", a->a_vp, "cnp:0x%jX", a->a_cnp);
	return (rc);
}

struct vnodeop_desc vop_remove_desc = {
	"vop_remove",
	0,
	__offsetof(struct vop_vector, vop_remove),
	(vop_bypass_t *)VOP_REMOVE_AP,
	vop_remove_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_remove_args,a_cnp),
};

static int vop_link_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_link_args,a_tdvp),
	VOPARG_OFFSETOF(struct vop_link_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_link, entry, "struct vnode *", "struct vop_link_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_link, return, "struct vnode *", "struct vop_link_args *", "int");


int
VOP_LINK_AP(struct vop_link_args *a)
{

	return(VOP_LINK_APV(a->a_tdvp->v_op, a));
}

int
VOP_LINK_APV(struct vop_vector *vop, struct vop_link_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_link_desc, a->a_tdvp,
	    ("Wrong a_desc in vop_link(%p, %p)", a->a_tdvp, a));
	VNASSERT(vop != NULL, a->a_tdvp, ("No vop_link(%p, %p)", a->a_tdvp, a));
	KTR_START3(KTR_VOP, "VOP", "VOP_LINK", (uintptr_t)a,
	    "tdvp:0x%jX", (uintptr_t)a->a_tdvp, "vp:0x%jX", a->a_vp, "cnp:0x%jX", a->a_cnp);
	vop_link_pre(a);
	ASSERT_VI_UNLOCKED(a->a_tdvp, "VOP_LINK Entry (tdvp)");
	ASSERT_VOP_ELOCKED(a->a_tdvp, "VOP_LINK Entry (tdvp)");
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_LINK Entry (vp)");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_LINK Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_link(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_link, entry, a->a_tdvp, a);
		rc = vop->vop_link(a);
		SDT_PROBE3(vfs, vop, vop_link, return, a->a_tdvp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_tdvp, "VOP_LINK OK (tdvp)");
		ASSERT_VOP_ELOCKED(a->a_tdvp, "VOP_LINK OK (tdvp)");
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_LINK OK (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_LINK OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_tdvp, "VOP_LINK Error (tdvp)");
		ASSERT_VOP_ELOCKED(a->a_tdvp, "VOP_LINK Error (tdvp)");
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_LINK Error (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_LINK Error (vp)");
	}
	vop_link_post(a, rc);
	KTR_STOP3(KTR_VOP, "VOP", "VOP_LINK", (uintptr_t)a,
	    "tdvp:0x%jX", (uintptr_t)a->a_tdvp, "vp:0x%jX", a->a_vp, "cnp:0x%jX", a->a_cnp);
	return (rc);
}

struct vnodeop_desc vop_link_desc = {
	"vop_link",
	0,
	__offsetof(struct vop_vector, vop_link),
	(vop_bypass_t *)VOP_LINK_AP,
	vop_link_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_link_args,a_cnp),
};

static int vop_rename_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_rename_args,a_fdvp),
	VOPARG_OFFSETOF(struct vop_rename_args,a_fvp),
	VOPARG_OFFSETOF(struct vop_rename_args,a_tdvp),
	VOPARG_OFFSETOF(struct vop_rename_args,a_tvp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_rename, entry, "struct vnode *", "struct vop_rename_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_rename, return, "struct vnode *", "struct vop_rename_args *", "int");


int
VOP_RENAME_AP(struct vop_rename_args *a)
{

	return(VOP_RENAME_APV(a->a_fdvp->v_op, a));
}

int
VOP_RENAME_APV(struct vop_vector *vop, struct vop_rename_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_rename_desc, a->a_fdvp,
	    ("Wrong a_desc in vop_rename(%p, %p)", a->a_fdvp, a));
	VNASSERT(vop != NULL, a->a_fdvp, ("No vop_rename(%p, %p)", a->a_fdvp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_RENAME", (uintptr_t)a,
	    "fdvp:0x%jX", (uintptr_t)a->a_fdvp, "fvp:0x%jX", a->a_fvp, "fcnp:0x%jX", a->a_fcnp, "tdvp:0x%jX", a->a_tdvp);
	vop_rename_pre(a);
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_rename(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_rename, entry, a->a_fdvp, a);
		rc = vop->vop_rename(a);
		SDT_PROBE3(vfs, vop, vop_rename, return, a->a_fdvp, a, rc);
	}
	if (rc == 0) {
	} else {
	}
	vop_rename_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_RENAME", (uintptr_t)a,
	    "fdvp:0x%jX", (uintptr_t)a->a_fdvp, "fvp:0x%jX", a->a_fvp, "fcnp:0x%jX", a->a_fcnp, "tdvp:0x%jX", a->a_tdvp);
	return (rc);
}

struct vnodeop_desc vop_rename_desc = {
	"vop_rename",
	VDESC_VP0_WILLRELE|VDESC_VP1_WILLRELE|VDESC_VP2_WILLRELE|VDESC_VP3_WILLRELE,
	__offsetof(struct vop_vector, vop_rename),
	(vop_bypass_t *)VOP_RENAME_AP,
	vop_rename_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_rename_args,a_fcnp),
};

static int vop_mkdir_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_mkdir_args,a_dvp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_mkdir, entry, "struct vnode *", "struct vop_mkdir_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_mkdir, return, "struct vnode *", "struct vop_mkdir_args *", "int");


int
VOP_MKDIR_AP(struct vop_mkdir_args *a)
{

	return(VOP_MKDIR_APV(a->a_dvp->v_op, a));
}

int
VOP_MKDIR_APV(struct vop_vector *vop, struct vop_mkdir_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_mkdir_desc, a->a_dvp,
	    ("Wrong a_desc in vop_mkdir(%p, %p)", a->a_dvp, a));
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_mkdir(%p, %p)", a->a_dvp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_MKDIR", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp, "vap:0x%jX", a->a_vap);
	vop_mkdir_pre(a);
	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_MKDIR Entry (dvp)");
	ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_MKDIR Entry (dvp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_mkdir(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_mkdir, entry, a->a_dvp, a);
		rc = vop->vop_mkdir(a);
		SDT_PROBE3(vfs, vop, vop_mkdir, return, a->a_dvp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_MKDIR OK (dvp)");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_MKDIR OK (dvp)");
		ASSERT_VI_UNLOCKED(*a->a_vpp, "VOP_MKDIR OK (vpp)");
		ASSERT_VOP_ELOCKED(*a->a_vpp, "VOP_MKDIR OK (vpp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_MKDIR Error (dvp)");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_MKDIR Error (dvp)");
	}
	vop_mkdir_post(a, rc);
#ifdef DEBUG_VFS_LOCKS
	vop_mkdir_debugpost(a, rc);
#endif
	KTR_STOP4(KTR_VOP, "VOP", "VOP_MKDIR", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp, "vap:0x%jX", a->a_vap);
	return (rc);
}

struct vnodeop_desc vop_mkdir_desc = {
	"vop_mkdir",
	0,
	__offsetof(struct vop_vector, vop_mkdir),
	(vop_bypass_t *)VOP_MKDIR_AP,
	vop_mkdir_vp_offsets,
	VOPARG_OFFSETOF(struct vop_mkdir_args,a_vpp),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_mkdir_args,a_cnp),
};

static int vop_rmdir_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_rmdir_args,a_dvp),
	VOPARG_OFFSETOF(struct vop_rmdir_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_rmdir, entry, "struct vnode *", "struct vop_rmdir_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_rmdir, return, "struct vnode *", "struct vop_rmdir_args *", "int");


int
VOP_RMDIR_AP(struct vop_rmdir_args *a)
{

	return(VOP_RMDIR_APV(a->a_dvp->v_op, a));
}

int
VOP_RMDIR_APV(struct vop_vector *vop, struct vop_rmdir_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_rmdir_desc, a->a_dvp,
	    ("Wrong a_desc in vop_rmdir(%p, %p)", a->a_dvp, a));
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_rmdir(%p, %p)", a->a_dvp, a));
	KTR_START3(KTR_VOP, "VOP", "VOP_RMDIR", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vp:0x%jX", a->a_vp, "cnp:0x%jX", a->a_cnp);
	vop_rmdir_pre(a);
	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_RMDIR Entry (dvp)");
	ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_RMDIR Entry (dvp)");
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_RMDIR Entry (vp)");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_RMDIR Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_rmdir(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_rmdir, entry, a->a_dvp, a);
		rc = vop->vop_rmdir(a);
		SDT_PROBE3(vfs, vop, vop_rmdir, return, a->a_dvp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_RMDIR OK (dvp)");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_RMDIR OK (dvp)");
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_RMDIR OK (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_RMDIR OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_RMDIR Error (dvp)");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_RMDIR Error (dvp)");
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_RMDIR Error (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_RMDIR Error (vp)");
	}
	vop_rmdir_post(a, rc);
	KTR_STOP3(KTR_VOP, "VOP", "VOP_RMDIR", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vp:0x%jX", a->a_vp, "cnp:0x%jX", a->a_cnp);
	return (rc);
}

struct vnodeop_desc vop_rmdir_desc = {
	"vop_rmdir",
	0,
	__offsetof(struct vop_vector, vop_rmdir),
	(vop_bypass_t *)VOP_RMDIR_AP,
	vop_rmdir_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_rmdir_args,a_cnp),
};

static int vop_symlink_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_symlink_args,a_dvp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_symlink, entry, "struct vnode *", "struct vop_symlink_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_symlink, return, "struct vnode *", "struct vop_symlink_args *", "int");


int
VOP_SYMLINK_AP(struct vop_symlink_args *a)
{

	return(VOP_SYMLINK_APV(a->a_dvp->v_op, a));
}

int
VOP_SYMLINK_APV(struct vop_vector *vop, struct vop_symlink_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_symlink_desc, a->a_dvp,
	    ("Wrong a_desc in vop_symlink(%p, %p)", a->a_dvp, a));
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_symlink(%p, %p)", a->a_dvp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_SYMLINK", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp, "vap:0x%jX", a->a_vap);
	vop_symlink_pre(a);
	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_SYMLINK Entry (dvp)");
	ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_SYMLINK Entry (dvp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_symlink(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_symlink, entry, a->a_dvp, a);
		rc = vop->vop_symlink(a);
		SDT_PROBE3(vfs, vop, vop_symlink, return, a->a_dvp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_SYMLINK OK (dvp)");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_SYMLINK OK (dvp)");
		ASSERT_VI_UNLOCKED(*a->a_vpp, "VOP_SYMLINK OK (vpp)");
		ASSERT_VOP_ELOCKED(*a->a_vpp, "VOP_SYMLINK OK (vpp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_SYMLINK Error (dvp)");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_SYMLINK Error (dvp)");
	}
	vop_symlink_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_SYMLINK", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp, "vap:0x%jX", a->a_vap);
	return (rc);
}

struct vnodeop_desc vop_symlink_desc = {
	"vop_symlink",
	0,
	__offsetof(struct vop_vector, vop_symlink),
	(vop_bypass_t *)VOP_SYMLINK_AP,
	vop_symlink_vp_offsets,
	VOPARG_OFFSETOF(struct vop_symlink_args,a_vpp),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_symlink_args,a_cnp),
};

static int vop_readdir_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_readdir_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_readdir, entry, "struct vnode *", "struct vop_readdir_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_readdir, return, "struct vnode *", "struct vop_readdir_args *", "int");


int
VOP_READDIR_AP(struct vop_readdir_args *a)
{

	return(VOP_READDIR_APV(a->a_vp->v_op, a));
}

int
VOP_READDIR_APV(struct vop_vector *vop, struct vop_readdir_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_readdir_desc, a->a_vp,
	    ("Wrong a_desc in vop_readdir(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_readdir(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_READDIR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "uio:0x%jX", a->a_uio, "cred:0x%jX", a->a_cred, "eofflag:0x%jX", a->a_eofflag);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READDIR Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_READDIR Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_readdir(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_readdir, entry, a->a_vp, a);
		rc = vop->vop_readdir(a);
		SDT_PROBE3(vfs, vop, vop_readdir, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READDIR OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_READDIR OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READDIR Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_READDIR Error (vp)");
	}
	vop_readdir_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_READDIR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "uio:0x%jX", a->a_uio, "cred:0x%jX", a->a_cred, "eofflag:0x%jX", a->a_eofflag);
	return (rc);
}

struct vnodeop_desc vop_readdir_desc = {
	"vop_readdir",
	0,
	__offsetof(struct vop_vector, vop_readdir),
	(vop_bypass_t *)VOP_READDIR_AP,
	vop_readdir_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_readdir_args,a_cred),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_readlink_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_readlink_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_readlink, entry, "struct vnode *", "struct vop_readlink_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_readlink, return, "struct vnode *", "struct vop_readlink_args *", "int");


int
VOP_READLINK_AP(struct vop_readlink_args *a)
{

	return(VOP_READLINK_APV(a->a_vp->v_op, a));
}

int
VOP_READLINK_APV(struct vop_vector *vop, struct vop_readlink_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_readlink_desc, a->a_vp,
	    ("Wrong a_desc in vop_readlink(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_readlink(%p, %p)", a->a_vp, a));
	KTR_START3(KTR_VOP, "VOP", "VOP_READLINK", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "uio:0x%jX", a->a_uio, "cred:0x%jX", a->a_cred);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READLINK Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_READLINK Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_readlink(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_readlink, entry, a->a_vp, a);
		rc = vop->vop_readlink(a);
		SDT_PROBE3(vfs, vop, vop_readlink, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READLINK OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_READLINK OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READLINK Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_READLINK Error (vp)");
	}
	KTR_STOP3(KTR_VOP, "VOP", "VOP_READLINK", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "uio:0x%jX", a->a_uio, "cred:0x%jX", a->a_cred);
	return (rc);
}

struct vnodeop_desc vop_readlink_desc = {
	"vop_readlink",
	0,
	__offsetof(struct vop_vector, vop_readlink),
	(vop_bypass_t *)VOP_READLINK_AP,
	vop_readlink_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_readlink_args,a_cred),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_inactive_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_inactive_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_inactive, entry, "struct vnode *", "struct vop_inactive_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_inactive, return, "struct vnode *", "struct vop_inactive_args *", "int");


int
VOP_INACTIVE_AP(struct vop_inactive_args *a)
{

	return(VOP_INACTIVE_APV(a->a_vp->v_op, a));
}

int
VOP_INACTIVE_APV(struct vop_vector *vop, struct vop_inactive_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_inactive_desc, a->a_vp,
	    ("Wrong a_desc in vop_inactive(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_inactive(%p, %p)", a->a_vp, a));
	KTR_START1(KTR_VOP, "VOP", "VOP_INACTIVE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_INACTIVE Entry (vp)");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_INACTIVE Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_inactive(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_inactive, entry, a->a_vp, a);
		rc = vop->vop_inactive(a);
		SDT_PROBE3(vfs, vop, vop_inactive, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_INACTIVE OK (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_INACTIVE OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_INACTIVE Error (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_INACTIVE Error (vp)");
	}
	KTR_STOP1(KTR_VOP, "VOP", "VOP_INACTIVE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	return (rc);
}

struct vnodeop_desc vop_inactive_desc = {
	"vop_inactive",
	0,
	__offsetof(struct vop_vector, vop_inactive),
	(vop_bypass_t *)VOP_INACTIVE_AP,
	vop_inactive_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_need_inactive_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_need_inactive_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_need_inactive, entry, "struct vnode *", "struct vop_need_inactive_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_need_inactive, return, "struct vnode *", "struct vop_need_inactive_args *", "int");


int
VOP_NEED_INACTIVE_AP(struct vop_need_inactive_args *a)
{

	return(VOP_NEED_INACTIVE_APV(a->a_vp->v_op, a));
}

int
VOP_NEED_INACTIVE_APV(struct vop_vector *vop, struct vop_need_inactive_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_need_inactive_desc, a->a_vp,
	    ("Wrong a_desc in vop_need_inactive(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_need_inactive(%p, %p)", a->a_vp, a));
	KTR_START1(KTR_VOP, "VOP", "VOP_NEED_INACTIVE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
#ifdef DEBUG_VFS_LOCKS
	vop_need_inactive_debugpre(a);
#endif
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_need_inactive(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_need_inactive, entry, a->a_vp, a);
		rc = vop->vop_need_inactive(a);
		SDT_PROBE3(vfs, vop, vop_need_inactive, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
	} else {
	}
#ifdef DEBUG_VFS_LOCKS
	vop_need_inactive_debugpost(a, rc);
#endif
	KTR_STOP1(KTR_VOP, "VOP", "VOP_NEED_INACTIVE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	return (rc);
}

struct vnodeop_desc vop_need_inactive_desc = {
	"vop_need_inactive",
	0,
	__offsetof(struct vop_vector, vop_need_inactive),
	(vop_bypass_t *)VOP_NEED_INACTIVE_AP,
	vop_need_inactive_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_reclaim_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_reclaim_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_reclaim, entry, "struct vnode *", "struct vop_reclaim_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_reclaim, return, "struct vnode *", "struct vop_reclaim_args *", "int");


int
VOP_RECLAIM_AP(struct vop_reclaim_args *a)
{

	return(VOP_RECLAIM_APV(a->a_vp->v_op, a));
}

int
VOP_RECLAIM_APV(struct vop_vector *vop, struct vop_reclaim_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_reclaim_desc, a->a_vp,
	    ("Wrong a_desc in vop_reclaim(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_reclaim(%p, %p)", a->a_vp, a));
	KTR_START1(KTR_VOP, "VOP", "VOP_RECLAIM", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_RECLAIM Entry (vp)");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_RECLAIM Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_reclaim(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_reclaim, entry, a->a_vp, a);
		rc = vop->vop_reclaim(a);
		SDT_PROBE3(vfs, vop, vop_reclaim, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_RECLAIM OK (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_RECLAIM OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_RECLAIM Error (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_RECLAIM Error (vp)");
	}
	vop_reclaim_post(a, rc);
	KTR_STOP1(KTR_VOP, "VOP", "VOP_RECLAIM", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	return (rc);
}

struct vnodeop_desc vop_reclaim_desc = {
	"vop_reclaim",
	0,
	__offsetof(struct vop_vector, vop_reclaim),
	(vop_bypass_t *)VOP_RECLAIM_AP,
	vop_reclaim_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_lock1_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_lock1_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_lock1, entry, "struct vnode *", "struct vop_lock1_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_lock1, return, "struct vnode *", "struct vop_lock1_args *", "int");


int
VOP_LOCK1_AP(struct vop_lock1_args *a)
{

	return(VOP_LOCK1_APV(a->a_vp->v_op, a));
}

int
VOP_LOCK1_APV(struct vop_vector *vop, struct vop_lock1_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_lock1_desc, a->a_vp,
	    ("Wrong a_desc in vop_lock1(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_lock1(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_LOCK1", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "flags:0x%jX", a->a_flags, "file:0x%jX", a->a_file, "line:0x%jX", a->a_line);
#ifdef DEBUG_VFS_LOCKS
	vop_lock_debugpre(a);
#endif
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_lock1(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_lock1, entry, a->a_vp, a);
		rc = vop->vop_lock1(a);
		SDT_PROBE3(vfs, vop, vop_lock1, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
	} else {
	}
#ifdef DEBUG_VFS_LOCKS
	vop_lock_debugpost(a, rc);
#endif
	KTR_STOP4(KTR_VOP, "VOP", "VOP_LOCK1", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "flags:0x%jX", a->a_flags, "file:0x%jX", a->a_file, "line:0x%jX", a->a_line);
	return (rc);
}

struct vnodeop_desc vop_lock1_desc = {
	"vop_lock1",
	0,
	__offsetof(struct vop_vector, vop_lock1),
	(vop_bypass_t *)VOP_LOCK1_AP,
	vop_lock1_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_unlock_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_unlock_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_unlock, entry, "struct vnode *", "struct vop_unlock_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_unlock, return, "struct vnode *", "struct vop_unlock_args *", "int");


int
VOP_UNLOCK_AP(struct vop_unlock_args *a)
{

	return(VOP_UNLOCK_APV(a->a_vp->v_op, a));
}

int
VOP_UNLOCK_APV(struct vop_vector *vop, struct vop_unlock_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_unlock_desc, a->a_vp,
	    ("Wrong a_desc in vop_unlock(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_unlock(%p, %p)", a->a_vp, a));
	KTR_START1(KTR_VOP, "VOP", "VOP_UNLOCK", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
#ifdef DEBUG_VFS_LOCKS
	vop_unlock_debugpre(a);
#endif
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_unlock(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_unlock, entry, a->a_vp, a);
		rc = vop->vop_unlock(a);
		SDT_PROBE3(vfs, vop, vop_unlock, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
	} else {
	}
	KTR_STOP1(KTR_VOP, "VOP", "VOP_UNLOCK", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	return (rc);
}

struct vnodeop_desc vop_unlock_desc = {
	"vop_unlock",
	0,
	__offsetof(struct vop_vector, vop_unlock),
	(vop_bypass_t *)VOP_UNLOCK_AP,
	vop_unlock_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_bmap_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_bmap_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_bmap, entry, "struct vnode *", "struct vop_bmap_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_bmap, return, "struct vnode *", "struct vop_bmap_args *", "int");


int
VOP_BMAP_AP(struct vop_bmap_args *a)
{

	return(VOP_BMAP_APV(a->a_vp->v_op, a));
}

int
VOP_BMAP_APV(struct vop_vector *vop, struct vop_bmap_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_bmap_desc, a->a_vp,
	    ("Wrong a_desc in vop_bmap(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_bmap(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_BMAP", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "bn:0x%jX", a->a_bn, "bop:0x%jX", a->a_bop, "bnp:0x%jX", a->a_bnp);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_BMAP Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_BMAP Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_bmap(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_bmap, entry, a->a_vp, a);
		rc = vop->vop_bmap(a);
		SDT_PROBE3(vfs, vop, vop_bmap, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_BMAP OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_BMAP OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_BMAP Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_BMAP Error (vp)");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_BMAP", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "bn:0x%jX", a->a_bn, "bop:0x%jX", a->a_bop, "bnp:0x%jX", a->a_bnp);
	return (rc);
}

struct vnodeop_desc vop_bmap_desc = {
	"vop_bmap",
	0,
	__offsetof(struct vop_vector, vop_bmap),
	(vop_bypass_t *)VOP_BMAP_AP,
	vop_bmap_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_strategy_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_strategy_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_strategy, entry, "struct vnode *", "struct vop_strategy_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_strategy, return, "struct vnode *", "struct vop_strategy_args *", "int");


int
VOP_STRATEGY_AP(struct vop_strategy_args *a)
{

	return(VOP_STRATEGY_APV(a->a_vp->v_op, a));
}

int
VOP_STRATEGY_APV(struct vop_vector *vop, struct vop_strategy_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_strategy_desc, a->a_vp,
	    ("Wrong a_desc in vop_strategy(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_strategy(%p, %p)", a->a_vp, a));
	KTR_START2(KTR_VOP, "VOP", "VOP_STRATEGY", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "bp:0x%jX", a->a_bp);
#ifdef DEBUG_VFS_LOCKS
	vop_strategy_debugpre(a);
#endif
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_STRATEGY Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_STRATEGY Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_strategy(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_strategy, entry, a->a_vp, a);
		rc = vop->vop_strategy(a);
		SDT_PROBE3(vfs, vop, vop_strategy, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_STRATEGY OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_STRATEGY OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_STRATEGY Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_STRATEGY Error (vp)");
	}
	KTR_STOP2(KTR_VOP, "VOP", "VOP_STRATEGY", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "bp:0x%jX", a->a_bp);
	return (rc);
}

struct vnodeop_desc vop_strategy_desc = {
	"vop_strategy",
	0,
	__offsetof(struct vop_vector, vop_strategy),
	(vop_bypass_t *)VOP_STRATEGY_AP,
	vop_strategy_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_getwritemount_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_getwritemount_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_getwritemount, entry, "struct vnode *", "struct vop_getwritemount_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_getwritemount, return, "struct vnode *", "struct vop_getwritemount_args *", "int");


int
VOP_GETWRITEMOUNT_AP(struct vop_getwritemount_args *a)
{

	return(VOP_GETWRITEMOUNT_APV(a->a_vp->v_op, a));
}

int
VOP_GETWRITEMOUNT_APV(struct vop_vector *vop, struct vop_getwritemount_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_getwritemount_desc, a->a_vp,
	    ("Wrong a_desc in vop_getwritemount(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_getwritemount(%p, %p)", a->a_vp, a));
	KTR_START2(KTR_VOP, "VOP", "VOP_GETWRITEMOUNT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "mpp:0x%jX", a->a_mpp);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETWRITEMOUNT Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_getwritemount(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_getwritemount, entry, a->a_vp, a);
		rc = vop->vop_getwritemount(a);
		SDT_PROBE3(vfs, vop, vop_getwritemount, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETWRITEMOUNT OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETWRITEMOUNT Error (vp)");
	}
	KTR_STOP2(KTR_VOP, "VOP", "VOP_GETWRITEMOUNT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "mpp:0x%jX", a->a_mpp);
	return (rc);
}

struct vnodeop_desc vop_getwritemount_desc = {
	"vop_getwritemount",
	0,
	__offsetof(struct vop_vector, vop_getwritemount),
	(vop_bypass_t *)VOP_GETWRITEMOUNT_AP,
	vop_getwritemount_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_getlowvnode_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_getlowvnode_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_getlowvnode, entry, "struct vnode *", "struct vop_getlowvnode_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_getlowvnode, return, "struct vnode *", "struct vop_getlowvnode_args *", "int");


int
VOP_GETLOWVNODE_AP(struct vop_getlowvnode_args *a)
{

	return(VOP_GETLOWVNODE_APV(a->a_vp->v_op, a));
}

int
VOP_GETLOWVNODE_APV(struct vop_vector *vop, struct vop_getlowvnode_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_getlowvnode_desc, a->a_vp,
	    ("Wrong a_desc in vop_getlowvnode(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_getlowvnode(%p, %p)", a->a_vp, a));
	KTR_START3(KTR_VOP, "VOP", "VOP_GETLOWVNODE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "vplp:0x%jX", a->a_vplp, "flags:0x%jX", a->a_flags);
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_getlowvnode(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_getlowvnode, entry, a->a_vp, a);
		rc = vop->vop_getlowvnode(a);
		SDT_PROBE3(vfs, vop, vop_getlowvnode, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
	} else {
	}
	KTR_STOP3(KTR_VOP, "VOP", "VOP_GETLOWVNODE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "vplp:0x%jX", a->a_vplp, "flags:0x%jX", a->a_flags);
	return (rc);
}

struct vnodeop_desc vop_getlowvnode_desc = {
	"vop_getlowvnode",
	0,
	__offsetof(struct vop_vector, vop_getlowvnode),
	(vop_bypass_t *)VOP_GETLOWVNODE_AP,
	vop_getlowvnode_vp_offsets,
	VOPARG_OFFSETOF(struct vop_getlowvnode_args,a_vplp),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_print_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_print_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_print, entry, "struct vnode *", "struct vop_print_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_print, return, "struct vnode *", "struct vop_print_args *", "int");


int
VOP_PRINT_AP(struct vop_print_args *a)
{

	return(VOP_PRINT_APV(a->a_vp->v_op, a));
}

int
VOP_PRINT_APV(struct vop_vector *vop, struct vop_print_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_print_desc, a->a_vp,
	    ("Wrong a_desc in vop_print(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_print(%p, %p)", a->a_vp, a));
	KTR_START1(KTR_VOP, "VOP", "VOP_PRINT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_print(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_print, entry, a->a_vp, a);
		rc = vop->vop_print(a);
		SDT_PROBE3(vfs, vop, vop_print, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
	} else {
	}
	KTR_STOP1(KTR_VOP, "VOP", "VOP_PRINT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	return (rc);
}

struct vnodeop_desc vop_print_desc = {
	"vop_print",
	0,
	__offsetof(struct vop_vector, vop_print),
	(vop_bypass_t *)VOP_PRINT_AP,
	vop_print_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_pathconf_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_pathconf_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_pathconf, entry, "struct vnode *", "struct vop_pathconf_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_pathconf, return, "struct vnode *", "struct vop_pathconf_args *", "int");


int
VOP_PATHCONF_AP(struct vop_pathconf_args *a)
{

	return(VOP_PATHCONF_APV(a->a_vp->v_op, a));
}

int
VOP_PATHCONF_APV(struct vop_vector *vop, struct vop_pathconf_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_pathconf_desc, a->a_vp,
	    ("Wrong a_desc in vop_pathconf(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_pathconf(%p, %p)", a->a_vp, a));
	KTR_START3(KTR_VOP, "VOP", "VOP_PATHCONF", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "name:0x%jX", a->a_name, "retval:0x%jX", a->a_retval);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_PATHCONF Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_PATHCONF Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_pathconf(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_pathconf, entry, a->a_vp, a);
		rc = vop->vop_pathconf(a);
		SDT_PROBE3(vfs, vop, vop_pathconf, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_PATHCONF OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_PATHCONF OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_PATHCONF Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_PATHCONF Error (vp)");
	}
	KTR_STOP3(KTR_VOP, "VOP", "VOP_PATHCONF", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "name:0x%jX", a->a_name, "retval:0x%jX", a->a_retval);
	return (rc);
}

struct vnodeop_desc vop_pathconf_desc = {
	"vop_pathconf",
	0,
	__offsetof(struct vop_vector, vop_pathconf),
	(vop_bypass_t *)VOP_PATHCONF_AP,
	vop_pathconf_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_advlock_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_advlock_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_advlock, entry, "struct vnode *", "struct vop_advlock_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_advlock, return, "struct vnode *", "struct vop_advlock_args *", "int");


int
VOP_ADVLOCK_AP(struct vop_advlock_args *a)
{

	return(VOP_ADVLOCK_APV(a->a_vp->v_op, a));
}

int
VOP_ADVLOCK_APV(struct vop_vector *vop, struct vop_advlock_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_advlock_desc, a->a_vp,
	    ("Wrong a_desc in vop_advlock(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_advlock(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_ADVLOCK", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "id:0x%jX", a->a_id, "op:0x%jX", a->a_op, "fl:0x%jX", a->a_fl);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCK Entry (vp)");
	ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVLOCK Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_advlock(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_advlock, entry, a->a_vp, a);
		rc = vop->vop_advlock(a);
		SDT_PROBE3(vfs, vop, vop_advlock, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCK OK (vp)");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVLOCK OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCK Error (vp)");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVLOCK Error (vp)");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_ADVLOCK", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "id:0x%jX", a->a_id, "op:0x%jX", a->a_op, "fl:0x%jX", a->a_fl);
	return (rc);
}

struct vnodeop_desc vop_advlock_desc = {
	"vop_advlock",
	0,
	__offsetof(struct vop_vector, vop_advlock),
	(vop_bypass_t *)VOP_ADVLOCK_AP,
	vop_advlock_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_advlockasync_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_advlockasync_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_advlockasync, entry, "struct vnode *", "struct vop_advlockasync_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_advlockasync, return, "struct vnode *", "struct vop_advlockasync_args *", "int");


int
VOP_ADVLOCKASYNC_AP(struct vop_advlockasync_args *a)
{

	return(VOP_ADVLOCKASYNC_APV(a->a_vp->v_op, a));
}

int
VOP_ADVLOCKASYNC_APV(struct vop_vector *vop, struct vop_advlockasync_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_advlockasync_desc, a->a_vp,
	    ("Wrong a_desc in vop_advlockasync(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_advlockasync(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_ADVLOCKASYNC", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "id:0x%jX", a->a_id, "op:0x%jX", a->a_op, "fl:0x%jX", a->a_fl);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCKASYNC Entry (vp)");
	ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVLOCKASYNC Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_advlockasync(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_advlockasync, entry, a->a_vp, a);
		rc = vop->vop_advlockasync(a);
		SDT_PROBE3(vfs, vop, vop_advlockasync, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCKASYNC OK (vp)");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVLOCKASYNC OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCKASYNC Error (vp)");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVLOCKASYNC Error (vp)");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_ADVLOCKASYNC", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "id:0x%jX", a->a_id, "op:0x%jX", a->a_op, "fl:0x%jX", a->a_fl);
	return (rc);
}

struct vnodeop_desc vop_advlockasync_desc = {
	"vop_advlockasync",
	0,
	__offsetof(struct vop_vector, vop_advlockasync),
	(vop_bypass_t *)VOP_ADVLOCKASYNC_AP,
	vop_advlockasync_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_advlockpurge_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_advlockpurge_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_advlockpurge, entry, "struct vnode *", "struct vop_advlockpurge_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_advlockpurge, return, "struct vnode *", "struct vop_advlockpurge_args *", "int");


int
VOP_ADVLOCKPURGE_AP(struct vop_advlockpurge_args *a)
{

	return(VOP_ADVLOCKPURGE_APV(a->a_vp->v_op, a));
}

int
VOP_ADVLOCKPURGE_APV(struct vop_vector *vop, struct vop_advlockpurge_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_advlockpurge_desc, a->a_vp,
	    ("Wrong a_desc in vop_advlockpurge(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_advlockpurge(%p, %p)", a->a_vp, a));
	KTR_START1(KTR_VOP, "VOP", "VOP_ADVLOCKPURGE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCKPURGE Entry (vp)");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_ADVLOCKPURGE Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_advlockpurge(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_advlockpurge, entry, a->a_vp, a);
		rc = vop->vop_advlockpurge(a);
		SDT_PROBE3(vfs, vop, vop_advlockpurge, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCKPURGE OK (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_ADVLOCKPURGE OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCKPURGE Error (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_ADVLOCKPURGE Error (vp)");
	}
	KTR_STOP1(KTR_VOP, "VOP", "VOP_ADVLOCKPURGE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	return (rc);
}

struct vnodeop_desc vop_advlockpurge_desc = {
	"vop_advlockpurge",
	0,
	__offsetof(struct vop_vector, vop_advlockpurge),
	(vop_bypass_t *)VOP_ADVLOCKPURGE_AP,
	vop_advlockpurge_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_reallocblks_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_reallocblks_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_reallocblks, entry, "struct vnode *", "struct vop_reallocblks_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_reallocblks, return, "struct vnode *", "struct vop_reallocblks_args *", "int");


int
VOP_REALLOCBLKS_AP(struct vop_reallocblks_args *a)
{

	return(VOP_REALLOCBLKS_APV(a->a_vp->v_op, a));
}

int
VOP_REALLOCBLKS_APV(struct vop_vector *vop, struct vop_reallocblks_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_reallocblks_desc, a->a_vp,
	    ("Wrong a_desc in vop_reallocblks(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_reallocblks(%p, %p)", a->a_vp, a));
	KTR_START2(KTR_VOP, "VOP", "VOP_REALLOCBLKS", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "buflist:0x%jX", a->a_buflist);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REALLOCBLKS Entry (vp)");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_REALLOCBLKS Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_reallocblks(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_reallocblks, entry, a->a_vp, a);
		rc = vop->vop_reallocblks(a);
		SDT_PROBE3(vfs, vop, vop_reallocblks, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REALLOCBLKS OK (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_REALLOCBLKS OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REALLOCBLKS Error (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_REALLOCBLKS Error (vp)");
	}
	KTR_STOP2(KTR_VOP, "VOP", "VOP_REALLOCBLKS", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "buflist:0x%jX", a->a_buflist);
	return (rc);
}

struct vnodeop_desc vop_reallocblks_desc = {
	"vop_reallocblks",
	0,
	__offsetof(struct vop_vector, vop_reallocblks),
	(vop_bypass_t *)VOP_REALLOCBLKS_AP,
	vop_reallocblks_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_getpages_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_getpages_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_getpages, entry, "struct vnode *", "struct vop_getpages_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_getpages, return, "struct vnode *", "struct vop_getpages_args *", "int");


int
VOP_GETPAGES_AP(struct vop_getpages_args *a)
{

	return(VOP_GETPAGES_APV(a->a_vp->v_op, a));
}

int
VOP_GETPAGES_APV(struct vop_vector *vop, struct vop_getpages_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_getpages_desc, a->a_vp,
	    ("Wrong a_desc in vop_getpages(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_getpages(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_GETPAGES", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "m:0x%jX", a->a_m, "count:0x%jX", a->a_count, "rbehind:0x%jX", a->a_rbehind);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETPAGES Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETPAGES Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_getpages(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_getpages, entry, a->a_vp, a);
		rc = vop->vop_getpages(a);
		SDT_PROBE3(vfs, vop, vop_getpages, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETPAGES OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETPAGES OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETPAGES Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETPAGES Error (vp)");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_GETPAGES", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "m:0x%jX", a->a_m, "count:0x%jX", a->a_count, "rbehind:0x%jX", a->a_rbehind);
	return (rc);
}

struct vnodeop_desc vop_getpages_desc = {
	"vop_getpages",
	0,
	__offsetof(struct vop_vector, vop_getpages),
	(vop_bypass_t *)VOP_GETPAGES_AP,
	vop_getpages_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_getpages_async_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_getpages_async_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_getpages_async, entry, "struct vnode *", "struct vop_getpages_async_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_getpages_async, return, "struct vnode *", "struct vop_getpages_async_args *", "int");


int
VOP_GETPAGES_ASYNC_AP(struct vop_getpages_async_args *a)
{

	return(VOP_GETPAGES_ASYNC_APV(a->a_vp->v_op, a));
}

int
VOP_GETPAGES_ASYNC_APV(struct vop_vector *vop, struct vop_getpages_async_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_getpages_async_desc, a->a_vp,
	    ("Wrong a_desc in vop_getpages_async(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_getpages_async(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_GETPAGES_ASYNC", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "m:0x%jX", a->a_m, "count:0x%jX", a->a_count, "rbehind:0x%jX", a->a_rbehind);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETPAGES_ASYNC Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETPAGES_ASYNC Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_getpages_async(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_getpages_async, entry, a->a_vp, a);
		rc = vop->vop_getpages_async(a);
		SDT_PROBE3(vfs, vop, vop_getpages_async, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETPAGES_ASYNC OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETPAGES_ASYNC OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETPAGES_ASYNC Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETPAGES_ASYNC Error (vp)");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_GETPAGES_ASYNC", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "m:0x%jX", a->a_m, "count:0x%jX", a->a_count, "rbehind:0x%jX", a->a_rbehind);
	return (rc);
}

struct vnodeop_desc vop_getpages_async_desc = {
	"vop_getpages_async",
	0,
	__offsetof(struct vop_vector, vop_getpages_async),
	(vop_bypass_t *)VOP_GETPAGES_ASYNC_AP,
	vop_getpages_async_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_putpages_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_putpages_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_putpages, entry, "struct vnode *", "struct vop_putpages_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_putpages, return, "struct vnode *", "struct vop_putpages_args *", "int");


int
VOP_PUTPAGES_AP(struct vop_putpages_args *a)
{

	return(VOP_PUTPAGES_APV(a->a_vp->v_op, a));
}

int
VOP_PUTPAGES_APV(struct vop_vector *vop, struct vop_putpages_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_putpages_desc, a->a_vp,
	    ("Wrong a_desc in vop_putpages(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_putpages(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_PUTPAGES", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "m:0x%jX", a->a_m, "count:0x%jX", a->a_count, "sync:0x%jX", a->a_sync);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_PUTPAGES Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_PUTPAGES Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_putpages(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_putpages, entry, a->a_vp, a);
		rc = vop->vop_putpages(a);
		SDT_PROBE3(vfs, vop, vop_putpages, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_PUTPAGES OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_PUTPAGES OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_PUTPAGES Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_PUTPAGES Error (vp)");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_PUTPAGES", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "m:0x%jX", a->a_m, "count:0x%jX", a->a_count, "sync:0x%jX", a->a_sync);
	return (rc);
}

struct vnodeop_desc vop_putpages_desc = {
	"vop_putpages",
	0,
	__offsetof(struct vop_vector, vop_putpages),
	(vop_bypass_t *)VOP_PUTPAGES_AP,
	vop_putpages_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_getacl_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_getacl_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_getacl, entry, "struct vnode *", "struct vop_getacl_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_getacl, return, "struct vnode *", "struct vop_getacl_args *", "int");


int
VOP_GETACL_AP(struct vop_getacl_args *a)
{

	return(VOP_GETACL_APV(a->a_vp->v_op, a));
}

int
VOP_GETACL_APV(struct vop_vector *vop, struct vop_getacl_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_getacl_desc, a->a_vp,
	    ("Wrong a_desc in vop_getacl(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_getacl(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_GETACL", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "type:0x%jX", a->a_type, "aclp:0x%jX", a->a_aclp, "cred:0x%jX", a->a_cred);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETACL Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETACL Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_getacl(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_getacl, entry, a->a_vp, a);
		rc = vop->vop_getacl(a);
		SDT_PROBE3(vfs, vop, vop_getacl, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETACL OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETACL OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETACL Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETACL Error (vp)");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_GETACL", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "type:0x%jX", a->a_type, "aclp:0x%jX", a->a_aclp, "cred:0x%jX", a->a_cred);
	return (rc);
}

struct vnodeop_desc vop_getacl_desc = {
	"vop_getacl",
	0,
	__offsetof(struct vop_vector, vop_getacl),
	(vop_bypass_t *)VOP_GETACL_AP,
	vop_getacl_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_getacl_args,a_cred),
	VOPARG_OFFSETOF(struct vop_getacl_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_setacl_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_setacl_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_setacl, entry, "struct vnode *", "struct vop_setacl_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_setacl, return, "struct vnode *", "struct vop_setacl_args *", "int");


int
VOP_SETACL_AP(struct vop_setacl_args *a)
{

	return(VOP_SETACL_APV(a->a_vp->v_op, a));
}

int
VOP_SETACL_APV(struct vop_vector *vop, struct vop_setacl_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_setacl_desc, a->a_vp,
	    ("Wrong a_desc in vop_setacl(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_setacl(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_SETACL", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "type:0x%jX", a->a_type, "aclp:0x%jX", a->a_aclp, "cred:0x%jX", a->a_cred);
	vop_setacl_pre(a);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETACL Entry (vp)");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETACL Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_setacl(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_setacl, entry, a->a_vp, a);
		rc = vop->vop_setacl(a);
		SDT_PROBE3(vfs, vop, vop_setacl, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETACL OK (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETACL OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETACL Error (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETACL Error (vp)");
	}
	vop_setacl_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_SETACL", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "type:0x%jX", a->a_type, "aclp:0x%jX", a->a_aclp, "cred:0x%jX", a->a_cred);
	return (rc);
}

struct vnodeop_desc vop_setacl_desc = {
	"vop_setacl",
	0,
	__offsetof(struct vop_vector, vop_setacl),
	(vop_bypass_t *)VOP_SETACL_AP,
	vop_setacl_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_setacl_args,a_cred),
	VOPARG_OFFSETOF(struct vop_setacl_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_aclcheck_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_aclcheck_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_aclcheck, entry, "struct vnode *", "struct vop_aclcheck_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_aclcheck, return, "struct vnode *", "struct vop_aclcheck_args *", "int");


int
VOP_ACLCHECK_AP(struct vop_aclcheck_args *a)
{

	return(VOP_ACLCHECK_APV(a->a_vp->v_op, a));
}

int
VOP_ACLCHECK_APV(struct vop_vector *vop, struct vop_aclcheck_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_aclcheck_desc, a->a_vp,
	    ("Wrong a_desc in vop_aclcheck(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_aclcheck(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_ACLCHECK", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "type:0x%jX", a->a_type, "aclp:0x%jX", a->a_aclp, "cred:0x%jX", a->a_cred);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACLCHECK Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_aclcheck(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_aclcheck, entry, a->a_vp, a);
		rc = vop->vop_aclcheck(a);
		SDT_PROBE3(vfs, vop, vop_aclcheck, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACLCHECK OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACLCHECK Error (vp)");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_ACLCHECK", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "type:0x%jX", a->a_type, "aclp:0x%jX", a->a_aclp, "cred:0x%jX", a->a_cred);
	return (rc);
}

struct vnodeop_desc vop_aclcheck_desc = {
	"vop_aclcheck",
	0,
	__offsetof(struct vop_vector, vop_aclcheck),
	(vop_bypass_t *)VOP_ACLCHECK_AP,
	vop_aclcheck_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_aclcheck_args,a_cred),
	VOPARG_OFFSETOF(struct vop_aclcheck_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_closeextattr_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_closeextattr_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_closeextattr, entry, "struct vnode *", "struct vop_closeextattr_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_closeextattr, return, "struct vnode *", "struct vop_closeextattr_args *", "int");


int
VOP_CLOSEEXTATTR_AP(struct vop_closeextattr_args *a)
{

	return(VOP_CLOSEEXTATTR_APV(a->a_vp->v_op, a));
}

int
VOP_CLOSEEXTATTR_APV(struct vop_vector *vop, struct vop_closeextattr_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_closeextattr_desc, a->a_vp,
	    ("Wrong a_desc in vop_closeextattr(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_closeextattr(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_CLOSEEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "commit:0x%jX", a->a_commit, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_CLOSEEXTATTR Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_CLOSEEXTATTR Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_closeextattr(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_closeextattr, entry, a->a_vp, a);
		rc = vop->vop_closeextattr(a);
		SDT_PROBE3(vfs, vop, vop_closeextattr, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_CLOSEEXTATTR OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_CLOSEEXTATTR OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_CLOSEEXTATTR Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_CLOSEEXTATTR Error (vp)");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_CLOSEEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "commit:0x%jX", a->a_commit, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	return (rc);
}

struct vnodeop_desc vop_closeextattr_desc = {
	"vop_closeextattr",
	0,
	__offsetof(struct vop_vector, vop_closeextattr),
	(vop_bypass_t *)VOP_CLOSEEXTATTR_AP,
	vop_closeextattr_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_closeextattr_args,a_cred),
	VOPARG_OFFSETOF(struct vop_closeextattr_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_getextattr_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_getextattr_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_getextattr, entry, "struct vnode *", "struct vop_getextattr_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_getextattr, return, "struct vnode *", "struct vop_getextattr_args *", "int");


int
VOP_GETEXTATTR_AP(struct vop_getextattr_args *a)
{

	return(VOP_GETEXTATTR_APV(a->a_vp->v_op, a));
}

int
VOP_GETEXTATTR_APV(struct vop_vector *vop, struct vop_getextattr_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_getextattr_desc, a->a_vp,
	    ("Wrong a_desc in vop_getextattr(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_getextattr(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_GETEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "attrnamespace:0x%jX", a->a_attrnamespace, "name:0x%jX", a->a_name, "uio:0x%jX", a->a_uio);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETEXTATTR Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETEXTATTR Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_getextattr(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_getextattr, entry, a->a_vp, a);
		rc = vop->vop_getextattr(a);
		SDT_PROBE3(vfs, vop, vop_getextattr, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETEXTATTR OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETEXTATTR OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETEXTATTR Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETEXTATTR Error (vp)");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_GETEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "attrnamespace:0x%jX", a->a_attrnamespace, "name:0x%jX", a->a_name, "uio:0x%jX", a->a_uio);
	return (rc);
}

struct vnodeop_desc vop_getextattr_desc = {
	"vop_getextattr",
	0,
	__offsetof(struct vop_vector, vop_getextattr),
	(vop_bypass_t *)VOP_GETEXTATTR_AP,
	vop_getextattr_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_getextattr_args,a_cred),
	VOPARG_OFFSETOF(struct vop_getextattr_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_listextattr_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_listextattr_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_listextattr, entry, "struct vnode *", "struct vop_listextattr_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_listextattr, return, "struct vnode *", "struct vop_listextattr_args *", "int");


int
VOP_LISTEXTATTR_AP(struct vop_listextattr_args *a)
{

	return(VOP_LISTEXTATTR_APV(a->a_vp->v_op, a));
}

int
VOP_LISTEXTATTR_APV(struct vop_vector *vop, struct vop_listextattr_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_listextattr_desc, a->a_vp,
	    ("Wrong a_desc in vop_listextattr(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_listextattr(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_LISTEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "attrnamespace:0x%jX", a->a_attrnamespace, "uio:0x%jX", a->a_uio, "size:0x%jX", a->a_size);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_LISTEXTATTR Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_LISTEXTATTR Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_listextattr(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_listextattr, entry, a->a_vp, a);
		rc = vop->vop_listextattr(a);
		SDT_PROBE3(vfs, vop, vop_listextattr, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_LISTEXTATTR OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_LISTEXTATTR OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_LISTEXTATTR Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_LISTEXTATTR Error (vp)");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_LISTEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "attrnamespace:0x%jX", a->a_attrnamespace, "uio:0x%jX", a->a_uio, "size:0x%jX", a->a_size);
	return (rc);
}

struct vnodeop_desc vop_listextattr_desc = {
	"vop_listextattr",
	0,
	__offsetof(struct vop_vector, vop_listextattr),
	(vop_bypass_t *)VOP_LISTEXTATTR_AP,
	vop_listextattr_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_listextattr_args,a_cred),
	VOPARG_OFFSETOF(struct vop_listextattr_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_openextattr_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_openextattr_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_openextattr, entry, "struct vnode *", "struct vop_openextattr_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_openextattr, return, "struct vnode *", "struct vop_openextattr_args *", "int");


int
VOP_OPENEXTATTR_AP(struct vop_openextattr_args *a)
{

	return(VOP_OPENEXTATTR_APV(a->a_vp->v_op, a));
}

int
VOP_OPENEXTATTR_APV(struct vop_vector *vop, struct vop_openextattr_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_openextattr_desc, a->a_vp,
	    ("Wrong a_desc in vop_openextattr(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_openextattr(%p, %p)", a->a_vp, a));
	KTR_START3(KTR_VOP, "VOP", "VOP_OPENEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_OPENEXTATTR Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_OPENEXTATTR Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_openextattr(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_openextattr, entry, a->a_vp, a);
		rc = vop->vop_openextattr(a);
		SDT_PROBE3(vfs, vop, vop_openextattr, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_OPENEXTATTR OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_OPENEXTATTR OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_OPENEXTATTR Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_OPENEXTATTR Error (vp)");
	}
	KTR_STOP3(KTR_VOP, "VOP", "VOP_OPENEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	return (rc);
}

struct vnodeop_desc vop_openextattr_desc = {
	"vop_openextattr",
	0,
	__offsetof(struct vop_vector, vop_openextattr),
	(vop_bypass_t *)VOP_OPENEXTATTR_AP,
	vop_openextattr_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_openextattr_args,a_cred),
	VOPARG_OFFSETOF(struct vop_openextattr_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_deleteextattr_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_deleteextattr_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_deleteextattr, entry, "struct vnode *", "struct vop_deleteextattr_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_deleteextattr, return, "struct vnode *", "struct vop_deleteextattr_args *", "int");


int
VOP_DELETEEXTATTR_AP(struct vop_deleteextattr_args *a)
{

	return(VOP_DELETEEXTATTR_APV(a->a_vp->v_op, a));
}

int
VOP_DELETEEXTATTR_APV(struct vop_vector *vop, struct vop_deleteextattr_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_deleteextattr_desc, a->a_vp,
	    ("Wrong a_desc in vop_deleteextattr(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_deleteextattr(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_DELETEEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "attrnamespace:0x%jX", a->a_attrnamespace, "name:0x%jX", a->a_name, "cred:0x%jX", a->a_cred);
	vop_deleteextattr_pre(a);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_DELETEEXTATTR Entry (vp)");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_DELETEEXTATTR Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_deleteextattr(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_deleteextattr, entry, a->a_vp, a);
		rc = vop->vop_deleteextattr(a);
		SDT_PROBE3(vfs, vop, vop_deleteextattr, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_DELETEEXTATTR OK (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_DELETEEXTATTR OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_DELETEEXTATTR Error (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_DELETEEXTATTR Error (vp)");
	}
	vop_deleteextattr_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_DELETEEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "attrnamespace:0x%jX", a->a_attrnamespace, "name:0x%jX", a->a_name, "cred:0x%jX", a->a_cred);
	return (rc);
}

struct vnodeop_desc vop_deleteextattr_desc = {
	"vop_deleteextattr",
	0,
	__offsetof(struct vop_vector, vop_deleteextattr),
	(vop_bypass_t *)VOP_DELETEEXTATTR_AP,
	vop_deleteextattr_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_deleteextattr_args,a_cred),
	VOPARG_OFFSETOF(struct vop_deleteextattr_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_setextattr_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_setextattr_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_setextattr, entry, "struct vnode *", "struct vop_setextattr_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_setextattr, return, "struct vnode *", "struct vop_setextattr_args *", "int");


int
VOP_SETEXTATTR_AP(struct vop_setextattr_args *a)
{

	return(VOP_SETEXTATTR_APV(a->a_vp->v_op, a));
}

int
VOP_SETEXTATTR_APV(struct vop_vector *vop, struct vop_setextattr_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_setextattr_desc, a->a_vp,
	    ("Wrong a_desc in vop_setextattr(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_setextattr(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_SETEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "attrnamespace:0x%jX", a->a_attrnamespace, "name:0x%jX", a->a_name, "uio:0x%jX", a->a_uio);
	vop_setextattr_pre(a);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETEXTATTR Entry (vp)");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETEXTATTR Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_setextattr(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_setextattr, entry, a->a_vp, a);
		rc = vop->vop_setextattr(a);
		SDT_PROBE3(vfs, vop, vop_setextattr, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETEXTATTR OK (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETEXTATTR OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETEXTATTR Error (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETEXTATTR Error (vp)");
	}
	vop_setextattr_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_SETEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "attrnamespace:0x%jX", a->a_attrnamespace, "name:0x%jX", a->a_name, "uio:0x%jX", a->a_uio);
	return (rc);
}

struct vnodeop_desc vop_setextattr_desc = {
	"vop_setextattr",
	0,
	__offsetof(struct vop_vector, vop_setextattr),
	(vop_bypass_t *)VOP_SETEXTATTR_AP,
	vop_setextattr_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_setextattr_args,a_cred),
	VOPARG_OFFSETOF(struct vop_setextattr_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_setlabel_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_setlabel_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_setlabel, entry, "struct vnode *", "struct vop_setlabel_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_setlabel, return, "struct vnode *", "struct vop_setlabel_args *", "int");


int
VOP_SETLABEL_AP(struct vop_setlabel_args *a)
{

	return(VOP_SETLABEL_APV(a->a_vp->v_op, a));
}

int
VOP_SETLABEL_APV(struct vop_vector *vop, struct vop_setlabel_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_setlabel_desc, a->a_vp,
	    ("Wrong a_desc in vop_setlabel(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_setlabel(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_SETLABEL", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "label:0x%jX", a->a_label, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETLABEL Entry (vp)");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETLABEL Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_setlabel(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_setlabel, entry, a->a_vp, a);
		rc = vop->vop_setlabel(a);
		SDT_PROBE3(vfs, vop, vop_setlabel, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETLABEL OK (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETLABEL OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETLABEL Error (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETLABEL Error (vp)");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_SETLABEL", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "label:0x%jX", a->a_label, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	return (rc);
}

struct vnodeop_desc vop_setlabel_desc = {
	"vop_setlabel",
	0,
	__offsetof(struct vop_vector, vop_setlabel),
	(vop_bypass_t *)VOP_SETLABEL_AP,
	vop_setlabel_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_setlabel_args,a_cred),
	VOPARG_OFFSETOF(struct vop_setlabel_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_vptofh_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_vptofh_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_vptofh, entry, "struct vnode *", "struct vop_vptofh_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_vptofh, return, "struct vnode *", "struct vop_vptofh_args *", "int");


int
VOP_VPTOFH_AP(struct vop_vptofh_args *a)
{

	return(VOP_VPTOFH_APV(a->a_vp->v_op, a));
}

int
VOP_VPTOFH_APV(struct vop_vector *vop, struct vop_vptofh_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_vptofh_desc, a->a_vp,
	    ("Wrong a_desc in vop_vptofh(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_vptofh(%p, %p)", a->a_vp, a));
	KTR_START2(KTR_VOP, "VOP", "VOP_VPTOFH", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "fhp:0x%jX", a->a_fhp);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_VPTOFH Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_vptofh(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_vptofh, entry, a->a_vp, a);
		rc = vop->vop_vptofh(a);
		SDT_PROBE3(vfs, vop, vop_vptofh, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_VPTOFH OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_VPTOFH Error (vp)");
	}
	KTR_STOP2(KTR_VOP, "VOP", "VOP_VPTOFH", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "fhp:0x%jX", a->a_fhp);
	return (rc);
}

struct vnodeop_desc vop_vptofh_desc = {
	"vop_vptofh",
	0,
	__offsetof(struct vop_vector, vop_vptofh),
	(vop_bypass_t *)VOP_VPTOFH_AP,
	vop_vptofh_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_vptocnp_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_vptocnp_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_vptocnp, entry, "struct vnode *", "struct vop_vptocnp_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_vptocnp, return, "struct vnode *", "struct vop_vptocnp_args *", "int");


int
VOP_VPTOCNP_AP(struct vop_vptocnp_args *a)
{

	return(VOP_VPTOCNP_APV(a->a_vp->v_op, a));
}

int
VOP_VPTOCNP_APV(struct vop_vector *vop, struct vop_vptocnp_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_vptocnp_desc, a->a_vp,
	    ("Wrong a_desc in vop_vptocnp(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_vptocnp(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_VPTOCNP", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "vpp:0x%jX", a->a_vpp, "buf:0x%jX", a->a_buf, "buflen:0x%jX", a->a_buflen);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_VPTOCNP Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_VPTOCNP Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_vptocnp(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_vptocnp, entry, a->a_vp, a);
		rc = vop->vop_vptocnp(a);
		SDT_PROBE3(vfs, vop, vop_vptocnp, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_VPTOCNP OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_VPTOCNP OK (vp)");
		ASSERT_VI_UNLOCKED(*a->a_vpp, "VOP_VPTOCNP OK (vpp)");
		ASSERT_VOP_UNLOCKED(*a->a_vpp, "VOP_VPTOCNP OK (vpp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_VPTOCNP Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_VPTOCNP Error (vp)");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_VPTOCNP", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "vpp:0x%jX", a->a_vpp, "buf:0x%jX", a->a_buf, "buflen:0x%jX", a->a_buflen);
	return (rc);
}

struct vnodeop_desc vop_vptocnp_desc = {
	"vop_vptocnp",
	0,
	__offsetof(struct vop_vector, vop_vptocnp),
	(vop_bypass_t *)VOP_VPTOCNP_AP,
	vop_vptocnp_vp_offsets,
	VOPARG_OFFSETOF(struct vop_vptocnp_args,a_vpp),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_allocate_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_allocate_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_allocate, entry, "struct vnode *", "struct vop_allocate_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_allocate, return, "struct vnode *", "struct vop_allocate_args *", "int");


int
VOP_ALLOCATE_AP(struct vop_allocate_args *a)
{

	return(VOP_ALLOCATE_APV(a->a_vp->v_op, a));
}

int
VOP_ALLOCATE_APV(struct vop_vector *vop, struct vop_allocate_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_allocate_desc, a->a_vp,
	    ("Wrong a_desc in vop_allocate(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_allocate(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_ALLOCATE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "offset:0x%jX", a->a_offset, "len:0x%jX", a->a_len, "ioflag:0x%jX", a->a_ioflag);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ALLOCATE Entry (vp)");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_ALLOCATE Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_allocate(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_allocate, entry, a->a_vp, a);
		rc = vop->vop_allocate(a);
		SDT_PROBE3(vfs, vop, vop_allocate, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ALLOCATE OK (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_ALLOCATE OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ALLOCATE Error (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_ALLOCATE Error (vp)");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_ALLOCATE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "offset:0x%jX", a->a_offset, "len:0x%jX", a->a_len, "ioflag:0x%jX", a->a_ioflag);
	return (rc);
}

struct vnodeop_desc vop_allocate_desc = {
	"vop_allocate",
	0,
	__offsetof(struct vop_vector, vop_allocate),
	(vop_bypass_t *)VOP_ALLOCATE_AP,
	vop_allocate_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_allocate_args,a_cred),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_advise_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_advise_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_advise, entry, "struct vnode *", "struct vop_advise_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_advise, return, "struct vnode *", "struct vop_advise_args *", "int");


int
VOP_ADVISE_AP(struct vop_advise_args *a)
{

	return(VOP_ADVISE_APV(a->a_vp->v_op, a));
}

int
VOP_ADVISE_APV(struct vop_vector *vop, struct vop_advise_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_advise_desc, a->a_vp,
	    ("Wrong a_desc in vop_advise(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_advise(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_ADVISE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "start:0x%jX", a->a_start, "end:0x%jX", a->a_end, "advice:0x%jX", a->a_advice);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVISE Entry (vp)");
	ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVISE Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_advise(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_advise, entry, a->a_vp, a);
		rc = vop->vop_advise(a);
		SDT_PROBE3(vfs, vop, vop_advise, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVISE OK (vp)");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVISE OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVISE Error (vp)");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVISE Error (vp)");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_ADVISE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "start:0x%jX", a->a_start, "end:0x%jX", a->a_end, "advice:0x%jX", a->a_advice);
	return (rc);
}

struct vnodeop_desc vop_advise_desc = {
	"vop_advise",
	0,
	__offsetof(struct vop_vector, vop_advise),
	(vop_bypass_t *)VOP_ADVISE_AP,
	vop_advise_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_unp_bind_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_unp_bind_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_unp_bind, entry, "struct vnode *", "struct vop_unp_bind_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_unp_bind, return, "struct vnode *", "struct vop_unp_bind_args *", "int");


int
VOP_UNP_BIND_AP(struct vop_unp_bind_args *a)
{

	return(VOP_UNP_BIND_APV(a->a_vp->v_op, a));
}

int
VOP_UNP_BIND_APV(struct vop_vector *vop, struct vop_unp_bind_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_unp_bind_desc, a->a_vp,
	    ("Wrong a_desc in vop_unp_bind(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_unp_bind(%p, %p)", a->a_vp, a));
	KTR_START2(KTR_VOP, "VOP", "VOP_UNP_BIND", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "unpcb:0x%jX", a->a_unpcb);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_UNP_BIND Entry (vp)");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_UNP_BIND Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_unp_bind(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_unp_bind, entry, a->a_vp, a);
		rc = vop->vop_unp_bind(a);
		SDT_PROBE3(vfs, vop, vop_unp_bind, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_UNP_BIND OK (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_UNP_BIND OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_UNP_BIND Error (vp)");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_UNP_BIND Error (vp)");
	}
	KTR_STOP2(KTR_VOP, "VOP", "VOP_UNP_BIND", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "unpcb:0x%jX", a->a_unpcb);
	return (rc);
}

struct vnodeop_desc vop_unp_bind_desc = {
	"vop_unp_bind",
	0,
	__offsetof(struct vop_vector, vop_unp_bind),
	(vop_bypass_t *)VOP_UNP_BIND_AP,
	vop_unp_bind_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_unp_connect_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_unp_connect_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_unp_connect, entry, "struct vnode *", "struct vop_unp_connect_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_unp_connect, return, "struct vnode *", "struct vop_unp_connect_args *", "int");


int
VOP_UNP_CONNECT_AP(struct vop_unp_connect_args *a)
{

	return(VOP_UNP_CONNECT_APV(a->a_vp->v_op, a));
}

int
VOP_UNP_CONNECT_APV(struct vop_vector *vop, struct vop_unp_connect_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_unp_connect_desc, a->a_vp,
	    ("Wrong a_desc in vop_unp_connect(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_unp_connect(%p, %p)", a->a_vp, a));
	KTR_START2(KTR_VOP, "VOP", "VOP_UNP_CONNECT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "unpcb:0x%jX", a->a_unpcb);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_UNP_CONNECT Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_UNP_CONNECT Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_unp_connect(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_unp_connect, entry, a->a_vp, a);
		rc = vop->vop_unp_connect(a);
		SDT_PROBE3(vfs, vop, vop_unp_connect, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_UNP_CONNECT OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_UNP_CONNECT OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_UNP_CONNECT Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_UNP_CONNECT Error (vp)");
	}
	KTR_STOP2(KTR_VOP, "VOP", "VOP_UNP_CONNECT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "unpcb:0x%jX", a->a_unpcb);
	return (rc);
}

struct vnodeop_desc vop_unp_connect_desc = {
	"vop_unp_connect",
	0,
	__offsetof(struct vop_vector, vop_unp_connect),
	(vop_bypass_t *)VOP_UNP_CONNECT_AP,
	vop_unp_connect_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_unp_detach_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_unp_detach_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_unp_detach, entry, "struct vnode *", "struct vop_unp_detach_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_unp_detach, return, "struct vnode *", "struct vop_unp_detach_args *", "int");


int
VOP_UNP_DETACH_AP(struct vop_unp_detach_args *a)
{

	return(VOP_UNP_DETACH_APV(a->a_vp->v_op, a));
}

int
VOP_UNP_DETACH_APV(struct vop_vector *vop, struct vop_unp_detach_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_unp_detach_desc, a->a_vp,
	    ("Wrong a_desc in vop_unp_detach(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_unp_detach(%p, %p)", a->a_vp, a));
	KTR_START1(KTR_VOP, "VOP", "VOP_UNP_DETACH", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_UNP_DETACH Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_unp_detach(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_unp_detach, entry, a->a_vp, a);
		rc = vop->vop_unp_detach(a);
		SDT_PROBE3(vfs, vop, vop_unp_detach, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_UNP_DETACH OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_UNP_DETACH Error (vp)");
	}
	KTR_STOP1(KTR_VOP, "VOP", "VOP_UNP_DETACH", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	return (rc);
}

struct vnodeop_desc vop_unp_detach_desc = {
	"vop_unp_detach",
	0,
	__offsetof(struct vop_vector, vop_unp_detach),
	(vop_bypass_t *)VOP_UNP_DETACH_AP,
	vop_unp_detach_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_is_text_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_is_text_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_is_text, entry, "struct vnode *", "struct vop_is_text_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_is_text, return, "struct vnode *", "struct vop_is_text_args *", "int");


int
VOP_IS_TEXT_AP(struct vop_is_text_args *a)
{

	return(VOP_IS_TEXT_APV(a->a_vp->v_op, a));
}

int
VOP_IS_TEXT_APV(struct vop_vector *vop, struct vop_is_text_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_is_text_desc, a->a_vp,
	    ("Wrong a_desc in vop_is_text(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_is_text(%p, %p)", a->a_vp, a));
	KTR_START1(KTR_VOP, "VOP", "VOP_IS_TEXT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_IS_TEXT Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_IS_TEXT Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_is_text(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_is_text, entry, a->a_vp, a);
		rc = vop->vop_is_text(a);
		SDT_PROBE3(vfs, vop, vop_is_text, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_IS_TEXT OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_IS_TEXT OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_IS_TEXT Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_IS_TEXT Error (vp)");
	}
	KTR_STOP1(KTR_VOP, "VOP", "VOP_IS_TEXT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	return (rc);
}

struct vnodeop_desc vop_is_text_desc = {
	"vop_is_text",
	0,
	__offsetof(struct vop_vector, vop_is_text),
	(vop_bypass_t *)VOP_IS_TEXT_AP,
	vop_is_text_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_set_text_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_set_text_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_set_text, entry, "struct vnode *", "struct vop_set_text_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_set_text, return, "struct vnode *", "struct vop_set_text_args *", "int");


int
VOP_SET_TEXT_AP(struct vop_set_text_args *a)
{

	return(VOP_SET_TEXT_APV(a->a_vp->v_op, a));
}

int
VOP_SET_TEXT_APV(struct vop_vector *vop, struct vop_set_text_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_set_text_desc, a->a_vp,
	    ("Wrong a_desc in vop_set_text(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_set_text(%p, %p)", a->a_vp, a));
	KTR_START1(KTR_VOP, "VOP", "VOP_SET_TEXT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SET_TEXT Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_set_text(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_set_text, entry, a->a_vp, a);
		rc = vop->vop_set_text(a);
		SDT_PROBE3(vfs, vop, vop_set_text, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SET_TEXT OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SET_TEXT Error (vp)");
	}
	KTR_STOP1(KTR_VOP, "VOP", "VOP_SET_TEXT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	return (rc);
}

struct vnodeop_desc vop_set_text_desc = {
	"vop_set_text",
	0,
	__offsetof(struct vop_vector, vop_set_text),
	(vop_bypass_t *)VOP_SET_TEXT_AP,
	vop_set_text_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_unset_text_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_unset_text_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_unset_text, entry, "struct vnode *", "struct vop_unset_text_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_unset_text, return, "struct vnode *", "struct vop_unset_text_args *", "int");


int
VOP_UNSET_TEXT_AP(struct vop_unset_text_args *a)
{

	return(VOP_UNSET_TEXT_APV(a->a_vp->v_op, a));
}

int
VOP_UNSET_TEXT_APV(struct vop_vector *vop, struct vop_unset_text_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_unset_text_desc, a->a_vp,
	    ("Wrong a_desc in vop_unset_text(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_unset_text(%p, %p)", a->a_vp, a));
	KTR_START1(KTR_VOP, "VOP", "VOP_UNSET_TEXT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_unset_text(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_unset_text, entry, a->a_vp, a);
		rc = vop->vop_unset_text(a);
		SDT_PROBE3(vfs, vop, vop_unset_text, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
	} else {
	}
	KTR_STOP1(KTR_VOP, "VOP", "VOP_UNSET_TEXT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	return (rc);
}

struct vnodeop_desc vop_unset_text_desc = {
	"vop_unset_text",
	0,
	__offsetof(struct vop_vector, vop_unset_text),
	(vop_bypass_t *)VOP_UNSET_TEXT_AP,
	vop_unset_text_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_add_writecount_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_add_writecount_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_add_writecount, entry, "struct vnode *", "struct vop_add_writecount_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_add_writecount, return, "struct vnode *", "struct vop_add_writecount_args *", "int");


int
VOP_ADD_WRITECOUNT_AP(struct vop_add_writecount_args *a)
{

	return(VOP_ADD_WRITECOUNT_APV(a->a_vp->v_op, a));
}

int
VOP_ADD_WRITECOUNT_APV(struct vop_vector *vop, struct vop_add_writecount_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_add_writecount_desc, a->a_vp,
	    ("Wrong a_desc in vop_add_writecount(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_add_writecount(%p, %p)", a->a_vp, a));
	KTR_START2(KTR_VOP, "VOP", "VOP_ADD_WRITECOUNT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "inc:0x%jX", a->a_inc);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADD_WRITECOUNT Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_ADD_WRITECOUNT Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_add_writecount(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_add_writecount, entry, a->a_vp, a);
		rc = vop->vop_add_writecount(a);
		SDT_PROBE3(vfs, vop, vop_add_writecount, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADD_WRITECOUNT OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_ADD_WRITECOUNT OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADD_WRITECOUNT Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_ADD_WRITECOUNT Error (vp)");
	}
	KTR_STOP2(KTR_VOP, "VOP", "VOP_ADD_WRITECOUNT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "inc:0x%jX", a->a_inc);
	return (rc);
}

struct vnodeop_desc vop_add_writecount_desc = {
	"vop_add_writecount",
	0,
	__offsetof(struct vop_vector, vop_add_writecount),
	(vop_bypass_t *)VOP_ADD_WRITECOUNT_AP,
	vop_add_writecount_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_fdatasync_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_fdatasync_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_fdatasync, entry, "struct vnode *", "struct vop_fdatasync_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_fdatasync, return, "struct vnode *", "struct vop_fdatasync_args *", "int");


int
VOP_FDATASYNC_AP(struct vop_fdatasync_args *a)
{

	return(VOP_FDATASYNC_APV(a->a_vp->v_op, a));
}

int
VOP_FDATASYNC_APV(struct vop_vector *vop, struct vop_fdatasync_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_fdatasync_desc, a->a_vp,
	    ("Wrong a_desc in vop_fdatasync(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_fdatasync(%p, %p)", a->a_vp, a));
	KTR_START2(KTR_VOP, "VOP", "VOP_FDATASYNC", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "td:0x%jX", a->a_td);
	vop_fdatasync_debugpre(a);
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_fdatasync(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_fdatasync, entry, a->a_vp, a);
		rc = vop->vop_fdatasync(a);
		SDT_PROBE3(vfs, vop, vop_fdatasync, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
	} else {
	}
	vop_fdatasync_debugpost(a, rc);
	KTR_STOP2(KTR_VOP, "VOP", "VOP_FDATASYNC", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "td:0x%jX", a->a_td);
	return (rc);
}

struct vnodeop_desc vop_fdatasync_desc = {
	"vop_fdatasync",
	0,
	__offsetof(struct vop_vector, vop_fdatasync),
	(vop_bypass_t *)VOP_FDATASYNC_AP,
	vop_fdatasync_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_fdatasync_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_copy_file_range_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_copy_file_range_args,a_invp),
	VOPARG_OFFSETOF(struct vop_copy_file_range_args,a_outvp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_copy_file_range, entry, "struct vnode *", "struct vop_copy_file_range_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_copy_file_range, return, "struct vnode *", "struct vop_copy_file_range_args *", "int");


int
VOP_COPY_FILE_RANGE_AP(struct vop_copy_file_range_args *a)
{

	return(VOP_COPY_FILE_RANGE_APV(a->a_invp->v_op, a));
}

int
VOP_COPY_FILE_RANGE_APV(struct vop_vector *vop, struct vop_copy_file_range_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_copy_file_range_desc, a->a_invp,
	    ("Wrong a_desc in vop_copy_file_range(%p, %p)", a->a_invp, a));
	VNASSERT(vop != NULL, a->a_invp, ("No vop_copy_file_range(%p, %p)", a->a_invp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_COPY_FILE_RANGE", (uintptr_t)a,
	    "invp:0x%jX", (uintptr_t)a->a_invp, "inoffp:0x%jX", a->a_inoffp, "outvp:0x%jX", a->a_outvp, "outoffp:0x%jX", a->a_outoffp);
	ASSERT_VI_UNLOCKED(a->a_invp, "VOP_COPY_FILE_RANGE Entry (invp)");
	ASSERT_VOP_UNLOCKED(a->a_invp, "VOP_COPY_FILE_RANGE Entry (invp)");
	ASSERT_VI_UNLOCKED(a->a_outvp, "VOP_COPY_FILE_RANGE Entry (outvp)");
	ASSERT_VOP_UNLOCKED(a->a_outvp, "VOP_COPY_FILE_RANGE Entry (outvp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_copy_file_range(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_copy_file_range, entry, a->a_invp, a);
		rc = vop->vop_copy_file_range(a);
		SDT_PROBE3(vfs, vop, vop_copy_file_range, return, a->a_invp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_invp, "VOP_COPY_FILE_RANGE OK (invp)");
		ASSERT_VOP_UNLOCKED(a->a_invp, "VOP_COPY_FILE_RANGE OK (invp)");
		ASSERT_VI_UNLOCKED(a->a_outvp, "VOP_COPY_FILE_RANGE OK (outvp)");
		ASSERT_VOP_UNLOCKED(a->a_outvp, "VOP_COPY_FILE_RANGE OK (outvp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_invp, "VOP_COPY_FILE_RANGE Error (invp)");
		ASSERT_VOP_UNLOCKED(a->a_invp, "VOP_COPY_FILE_RANGE Error (invp)");
		ASSERT_VI_UNLOCKED(a->a_outvp, "VOP_COPY_FILE_RANGE Error (outvp)");
		ASSERT_VOP_UNLOCKED(a->a_outvp, "VOP_COPY_FILE_RANGE Error (outvp)");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_COPY_FILE_RANGE", (uintptr_t)a,
	    "invp:0x%jX", (uintptr_t)a->a_invp, "inoffp:0x%jX", a->a_inoffp, "outvp:0x%jX", a->a_outvp, "outoffp:0x%jX", a->a_outoffp);
	return (rc);
}

struct vnodeop_desc vop_copy_file_range_desc = {
	"vop_copy_file_range",
	0,
	__offsetof(struct vop_vector, vop_copy_file_range),
	(vop_bypass_t *)VOP_COPY_FILE_RANGE_AP,
	vop_copy_file_range_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_copy_file_range_args,a_incred),
	VOPARG_OFFSETOF(struct vop_copy_file_range_args,a_fsizetd),
	VDESC_NO_OFFSET,
};

static int vop_vput_pair_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_vput_pair_args,a_dvp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_vput_pair, entry, "struct vnode *", "struct vop_vput_pair_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_vput_pair, return, "struct vnode *", "struct vop_vput_pair_args *", "int");


int
VOP_VPUT_PAIR_AP(struct vop_vput_pair_args *a)
{

	return(VOP_VPUT_PAIR_APV(a->a_dvp->v_op, a));
}

int
VOP_VPUT_PAIR_APV(struct vop_vector *vop, struct vop_vput_pair_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_vput_pair_desc, a->a_dvp,
	    ("Wrong a_desc in vop_vput_pair(%p, %p)", a->a_dvp, a));
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_vput_pair(%p, %p)", a->a_dvp, a));
	KTR_START3(KTR_VOP, "VOP", "VOP_VPUT_PAIR", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "unlock_vp:0x%jX", a->a_unlock_vp);
	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_VPUT_PAIR Entry (dvp)");
	ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_VPUT_PAIR Entry (dvp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_vput_pair(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_vput_pair, entry, a->a_dvp, a);
		rc = vop->vop_vput_pair(a);
		SDT_PROBE3(vfs, vop, vop_vput_pair, return, a->a_dvp, a, rc);
	}
	if (rc == 0) {
	} else {
	}
	KTR_STOP3(KTR_VOP, "VOP", "VOP_VPUT_PAIR", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "unlock_vp:0x%jX", a->a_unlock_vp);
	return (rc);
}

struct vnodeop_desc vop_vput_pair_desc = {
	"vop_vput_pair",
	0,
	__offsetof(struct vop_vector, vop_vput_pair),
	(vop_bypass_t *)VOP_VPUT_PAIR_AP,
	vop_vput_pair_vp_offsets,
	VOPARG_OFFSETOF(struct vop_vput_pair_args,a_vpp),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_deallocate_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_deallocate_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_deallocate, entry, "struct vnode *", "struct vop_deallocate_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_deallocate, return, "struct vnode *", "struct vop_deallocate_args *", "int");


int
VOP_DEALLOCATE_AP(struct vop_deallocate_args *a)
{

	return(VOP_DEALLOCATE_APV(a->a_vp->v_op, a));
}

int
VOP_DEALLOCATE_APV(struct vop_vector *vop, struct vop_deallocate_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_deallocate_desc, a->a_vp,
	    ("Wrong a_desc in vop_deallocate(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_deallocate(%p, %p)", a->a_vp, a));
	KTR_START4(KTR_VOP, "VOP", "VOP_DEALLOCATE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "offset:0x%jX", a->a_offset, "len:0x%jX", a->a_len, "flags:0x%jX", a->a_flags);
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_DEALLOCATE Entry (vp)");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_DEALLOCATE Entry (vp)");
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_deallocate(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_deallocate, entry, a->a_vp, a);
		rc = vop->vop_deallocate(a);
		SDT_PROBE3(vfs, vop, vop_deallocate, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_DEALLOCATE OK (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_DEALLOCATE OK (vp)");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_DEALLOCATE Error (vp)");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_DEALLOCATE Error (vp)");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_DEALLOCATE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "offset:0x%jX", a->a_offset, "len:0x%jX", a->a_len, "flags:0x%jX", a->a_flags);
	return (rc);
}

struct vnodeop_desc vop_deallocate_desc = {
	"vop_deallocate",
	0,
	__offsetof(struct vop_vector, vop_deallocate),
	(vop_bypass_t *)VOP_DEALLOCATE_AP,
	vop_deallocate_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_deallocate_args,a_cred),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_spare1_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_spare1_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_spare1, entry, "struct vnode *", "struct vop_spare1_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_spare1, return, "struct vnode *", "struct vop_spare1_args *", "int");


int
VOP_SPARE1_AP(struct vop_spare1_args *a)
{

	return(VOP_SPARE1_APV(a->a_vp->v_op, a));
}

int
VOP_SPARE1_APV(struct vop_vector *vop, struct vop_spare1_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_spare1_desc, a->a_vp,
	    ("Wrong a_desc in vop_spare1(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_spare1(%p, %p)", a->a_vp, a));
	KTR_START1(KTR_VOP, "VOP", "VOP_SPARE1", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_spare1(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_spare1, entry, a->a_vp, a);
		rc = vop->vop_spare1(a);
		SDT_PROBE3(vfs, vop, vop_spare1, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
	} else {
	}
	KTR_STOP1(KTR_VOP, "VOP", "VOP_SPARE1", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	return (rc);
}

struct vnodeop_desc vop_spare1_desc = {
	"vop_spare1",
	0,
	__offsetof(struct vop_vector, vop_spare1),
	(vop_bypass_t *)VOP_SPARE1_AP,
	vop_spare1_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_spare2_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_spare2_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_spare2, entry, "struct vnode *", "struct vop_spare2_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_spare2, return, "struct vnode *", "struct vop_spare2_args *", "int");


int
VOP_SPARE2_AP(struct vop_spare2_args *a)
{

	return(VOP_SPARE2_APV(a->a_vp->v_op, a));
}

int
VOP_SPARE2_APV(struct vop_vector *vop, struct vop_spare2_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_spare2_desc, a->a_vp,
	    ("Wrong a_desc in vop_spare2(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_spare2(%p, %p)", a->a_vp, a));
	KTR_START1(KTR_VOP, "VOP", "VOP_SPARE2", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_spare2(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_spare2, entry, a->a_vp, a);
		rc = vop->vop_spare2(a);
		SDT_PROBE3(vfs, vop, vop_spare2, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
	} else {
	}
	KTR_STOP1(KTR_VOP, "VOP", "VOP_SPARE2", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	return (rc);
}

struct vnodeop_desc vop_spare2_desc = {
	"vop_spare2",
	0,
	__offsetof(struct vop_vector, vop_spare2),
	(vop_bypass_t *)VOP_SPARE2_AP,
	vop_spare2_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_spare3_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_spare3_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_spare3, entry, "struct vnode *", "struct vop_spare3_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_spare3, return, "struct vnode *", "struct vop_spare3_args *", "int");


int
VOP_SPARE3_AP(struct vop_spare3_args *a)
{

	return(VOP_SPARE3_APV(a->a_vp->v_op, a));
}

int
VOP_SPARE3_APV(struct vop_vector *vop, struct vop_spare3_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_spare3_desc, a->a_vp,
	    ("Wrong a_desc in vop_spare3(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_spare3(%p, %p)", a->a_vp, a));
	KTR_START1(KTR_VOP, "VOP", "VOP_SPARE3", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_spare3(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_spare3, entry, a->a_vp, a);
		rc = vop->vop_spare3(a);
		SDT_PROBE3(vfs, vop, vop_spare3, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
	} else {
	}
	KTR_STOP1(KTR_VOP, "VOP", "VOP_SPARE3", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	return (rc);
}

struct vnodeop_desc vop_spare3_desc = {
	"vop_spare3",
	0,
	__offsetof(struct vop_vector, vop_spare3),
	(vop_bypass_t *)VOP_SPARE3_AP,
	vop_spare3_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_spare4_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_spare4_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_spare4, entry, "struct vnode *", "struct vop_spare4_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_spare4, return, "struct vnode *", "struct vop_spare4_args *", "int");


int
VOP_SPARE4_AP(struct vop_spare4_args *a)
{

	return(VOP_SPARE4_APV(a->a_vp->v_op, a));
}

int
VOP_SPARE4_APV(struct vop_vector *vop, struct vop_spare4_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_spare4_desc, a->a_vp,
	    ("Wrong a_desc in vop_spare4(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_spare4(%p, %p)", a->a_vp, a));
	KTR_START1(KTR_VOP, "VOP", "VOP_SPARE4", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_spare4(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_spare4, entry, a->a_vp, a);
		rc = vop->vop_spare4(a);
		SDT_PROBE3(vfs, vop, vop_spare4, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
	} else {
	}
	KTR_STOP1(KTR_VOP, "VOP", "VOP_SPARE4", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	return (rc);
}

struct vnodeop_desc vop_spare4_desc = {
	"vop_spare4",
	0,
	__offsetof(struct vop_vector, vop_spare4),
	(vop_bypass_t *)VOP_SPARE4_AP,
	vop_spare4_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_spare5_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_spare5_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_spare5, entry, "struct vnode *", "struct vop_spare5_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_spare5, return, "struct vnode *", "struct vop_spare5_args *", "int");


int
VOP_SPARE5_AP(struct vop_spare5_args *a)
{

	return(VOP_SPARE5_APV(a->a_vp->v_op, a));
}

int
VOP_SPARE5_APV(struct vop_vector *vop, struct vop_spare5_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_spare5_desc, a->a_vp,
	    ("Wrong a_desc in vop_spare5(%p, %p)", a->a_vp, a));
	VNASSERT(vop != NULL, a->a_vp, ("No vop_spare5(%p, %p)", a->a_vp, a));
	KTR_START1(KTR_VOP, "VOP", "VOP_SPARE5", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	if (!SDT_PROBES_ENABLED()) {
		rc = vop->vop_spare5(a);
	} else {
		SDT_PROBE2(vfs, vop, vop_spare5, entry, a->a_vp, a);
		rc = vop->vop_spare5(a);
		SDT_PROBE3(vfs, vop, vop_spare5, return, a->a_vp, a, rc);
	}
	if (rc == 0) {
	} else {
	}
	KTR_STOP1(KTR_VOP, "VOP", "VOP_SPARE5", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	return (rc);
}

struct vnodeop_desc vop_spare5_desc = {
	"vop_spare5",
	0,
	__offsetof(struct vop_vector, vop_spare5),
	(vop_bypass_t *)VOP_SPARE5_AP,
	vop_spare5_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

void
vfs_vector_op_register(struct vop_vector *orig_vop)
{
	struct vop_vector *vop;

	if (orig_vop->registered)
		panic("%s: vop_vector %p already registered",
		    __func__, orig_vop);

	cache_vop_vector_register(orig_vop);

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_print == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_print = vop->vop_print;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_setlabel == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_setlabel = vop->vop_setlabel;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_read == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_read = vop->vop_read;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_is_text == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_is_text = vop->vop_is_text;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_accessx == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_accessx = vop->vop_accessx;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_vput_pair == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_vput_pair = vop->vop_vput_pair;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_getacl == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_getacl = vop->vop_getacl;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_getpages == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_getpages = vop->vop_getpages;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_reallocblks == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_reallocblks = vop->vop_reallocblks;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_access == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_access = vop->vop_access;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_islocked == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_islocked = vop->vop_islocked;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_setattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_setattr = vop->vop_setattr;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_whiteout == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_whiteout = vop->vop_whiteout;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_spare1 == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_spare1 = vop->vop_spare1;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_link == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_link = vop->vop_link;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_spare2 == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_spare2 = vop->vop_spare2;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_spare3 == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_spare3 = vop->vop_spare3;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_fplookup_symlink == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_fplookup_symlink = vop->vop_fplookup_symlink;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_spare4 == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_spare4 = vop->vop_spare4;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_spare5 == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_spare5 = vop->vop_spare5;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_deallocate == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_deallocate = vop->vop_deallocate;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_openextattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_openextattr = vop->vop_openextattr;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_vptocnp == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_vptocnp = vop->vop_vptocnp;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_lock1 == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_lock1 = vop->vop_lock1;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_remove == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_remove = vop->vop_remove;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_kqfilter == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_kqfilter = vop->vop_kqfilter;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_read_pgcache == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_read_pgcache = vop->vop_read_pgcache;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_reclaim == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_reclaim = vop->vop_reclaim;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_write == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_write = vop->vop_write;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_setextattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_setextattr = vop->vop_setextattr;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_setacl == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_setacl = vop->vop_setacl;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_unp_bind == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_unp_bind = vop->vop_unp_bind;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_symlink == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_symlink = vop->vop_symlink;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_mknod == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_mknod = vop->vop_mknod;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_need_inactive == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_need_inactive = vop->vop_need_inactive;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_mmapped == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_mmapped = vop->vop_mmapped;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_advlockasync == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_advlockasync = vop->vop_advlockasync;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_cachedlookup == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_cachedlookup = vop->vop_cachedlookup;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_rmdir == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_rmdir = vop->vop_rmdir;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_rename == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_rename = vop->vop_rename;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_stat == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_stat = vop->vop_stat;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_getattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_getattr = vop->vop_getattr;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_advlockpurge == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_advlockpurge = vop->vop_advlockpurge;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_pathconf == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_pathconf = vop->vop_pathconf;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_getextattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_getextattr = vop->vop_getextattr;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_closeextattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_closeextattr = vop->vop_closeextattr;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_poll == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_poll = vop->vop_poll;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_readlink == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_readlink = vop->vop_readlink;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_create == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_create = vop->vop_create;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_unset_text == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_unset_text = vop->vop_unset_text;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_strategy == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_strategy = vop->vop_strategy;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_getwritemount == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_getwritemount = vop->vop_getwritemount;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_unlock == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_unlock = vop->vop_unlock;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_open == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_open = vop->vop_open;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_fdatasync == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_fdatasync = vop->vop_fdatasync;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_unp_detach == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_unp_detach = vop->vop_unp_detach;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_readdir == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_readdir = vop->vop_readdir;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_set_text == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_set_text = vop->vop_set_text;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_aclcheck == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_aclcheck = vop->vop_aclcheck;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_fplookup_vexec == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_fplookup_vexec = vop->vop_fplookup_vexec;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_close == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_close = vop->vop_close;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_mkdir == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_mkdir = vop->vop_mkdir;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_advise == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_advise = vop->vop_advise;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_advlock == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_advlock = vop->vop_advlock;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_lookup == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_lookup = vop->vop_lookup;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_inactive == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_inactive = vop->vop_inactive;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_fsync == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_fsync = vop->vop_fsync;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_ioctl == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_ioctl = vop->vop_ioctl;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_getlowvnode == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_getlowvnode = vop->vop_getlowvnode;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_copy_file_range == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_copy_file_range = vop->vop_copy_file_range;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_add_writecount == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_add_writecount = vop->vop_add_writecount;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_revoke == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_revoke = vop->vop_revoke;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_vptofh == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_vptofh = vop->vop_vptofh;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_putpages == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_putpages = vop->vop_putpages;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_unp_connect == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_unp_connect = vop->vop_unp_connect;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_listextattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_listextattr = vop->vop_listextattr;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_bmap == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_bmap = vop->vop_bmap;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_allocate == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_allocate = vop->vop_allocate;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_deleteextattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_deleteextattr = vop->vop_deleteextattr;

	vop = orig_vop;
	while (vop != NULL && \
	    vop->vop_getpages_async == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_getpages_async = vop->vop_getpages_async;

	vop = orig_vop;
	while (vop != NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	if (vop != NULL)
		orig_vop->vop_bypass = vop->vop_bypass;

	if (orig_vop->vop_print == NULL)
		orig_vop->vop_print = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_setlabel == NULL)
		orig_vop->vop_setlabel = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_read == NULL)
		orig_vop->vop_read = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_is_text == NULL)
		orig_vop->vop_is_text = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_accessx == NULL)
		orig_vop->vop_accessx = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_vput_pair == NULL)
		orig_vop->vop_vput_pair = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_getacl == NULL)
		orig_vop->vop_getacl = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_getpages == NULL)
		orig_vop->vop_getpages = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_reallocblks == NULL)
		orig_vop->vop_reallocblks = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_access == NULL)
		orig_vop->vop_access = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_islocked == NULL)
		orig_vop->vop_islocked = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_setattr == NULL)
		orig_vop->vop_setattr = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_whiteout == NULL)
		orig_vop->vop_whiteout = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_spare1 == NULL)
		orig_vop->vop_spare1 = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_link == NULL)
		orig_vop->vop_link = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_spare2 == NULL)
		orig_vop->vop_spare2 = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_spare3 == NULL)
		orig_vop->vop_spare3 = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_fplookup_symlink == NULL)
		orig_vop->vop_fplookup_symlink = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_spare4 == NULL)
		orig_vop->vop_spare4 = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_spare5 == NULL)
		orig_vop->vop_spare5 = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_deallocate == NULL)
		orig_vop->vop_deallocate = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_openextattr == NULL)
		orig_vop->vop_openextattr = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_vptocnp == NULL)
		orig_vop->vop_vptocnp = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_lock1 == NULL)
		orig_vop->vop_lock1 = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_remove == NULL)
		orig_vop->vop_remove = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_kqfilter == NULL)
		orig_vop->vop_kqfilter = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_read_pgcache == NULL)
		orig_vop->vop_read_pgcache = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_reclaim == NULL)
		orig_vop->vop_reclaim = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_write == NULL)
		orig_vop->vop_write = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_setextattr == NULL)
		orig_vop->vop_setextattr = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_setacl == NULL)
		orig_vop->vop_setacl = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_unp_bind == NULL)
		orig_vop->vop_unp_bind = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_symlink == NULL)
		orig_vop->vop_symlink = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_mknod == NULL)
		orig_vop->vop_mknod = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_need_inactive == NULL)
		orig_vop->vop_need_inactive = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_mmapped == NULL)
		orig_vop->vop_mmapped = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_advlockasync == NULL)
		orig_vop->vop_advlockasync = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_cachedlookup == NULL)
		orig_vop->vop_cachedlookup = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_rmdir == NULL)
		orig_vop->vop_rmdir = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_rename == NULL)
		orig_vop->vop_rename = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_stat == NULL)
		orig_vop->vop_stat = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_getattr == NULL)
		orig_vop->vop_getattr = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_advlockpurge == NULL)
		orig_vop->vop_advlockpurge = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_pathconf == NULL)
		orig_vop->vop_pathconf = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_getextattr == NULL)
		orig_vop->vop_getextattr = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_closeextattr == NULL)
		orig_vop->vop_closeextattr = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_poll == NULL)
		orig_vop->vop_poll = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_readlink == NULL)
		orig_vop->vop_readlink = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_create == NULL)
		orig_vop->vop_create = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_unset_text == NULL)
		orig_vop->vop_unset_text = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_strategy == NULL)
		orig_vop->vop_strategy = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_getwritemount == NULL)
		orig_vop->vop_getwritemount = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_unlock == NULL)
		orig_vop->vop_unlock = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_open == NULL)
		orig_vop->vop_open = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_fdatasync == NULL)
		orig_vop->vop_fdatasync = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_unp_detach == NULL)
		orig_vop->vop_unp_detach = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_readdir == NULL)
		orig_vop->vop_readdir = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_set_text == NULL)
		orig_vop->vop_set_text = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_aclcheck == NULL)
		orig_vop->vop_aclcheck = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_fplookup_vexec == NULL)
		orig_vop->vop_fplookup_vexec = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_close == NULL)
		orig_vop->vop_close = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_mkdir == NULL)
		orig_vop->vop_mkdir = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_advise == NULL)
		orig_vop->vop_advise = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_advlock == NULL)
		orig_vop->vop_advlock = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_lookup == NULL)
		orig_vop->vop_lookup = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_inactive == NULL)
		orig_vop->vop_inactive = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_fsync == NULL)
		orig_vop->vop_fsync = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_ioctl == NULL)
		orig_vop->vop_ioctl = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_getlowvnode == NULL)
		orig_vop->vop_getlowvnode = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_copy_file_range == NULL)
		orig_vop->vop_copy_file_range = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_add_writecount == NULL)
		orig_vop->vop_add_writecount = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_revoke == NULL)
		orig_vop->vop_revoke = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_vptofh == NULL)
		orig_vop->vop_vptofh = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_putpages == NULL)
		orig_vop->vop_putpages = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_unp_connect == NULL)
		orig_vop->vop_unp_connect = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_listextattr == NULL)
		orig_vop->vop_listextattr = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_bmap == NULL)
		orig_vop->vop_bmap = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_allocate == NULL)
		orig_vop->vop_allocate = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_deleteextattr == NULL)
		orig_vop->vop_deleteextattr = (void *)orig_vop->vop_bypass;
	if (orig_vop->vop_getpages_async == NULL)
		orig_vop->vop_getpages_async = (void *)orig_vop->vop_bypass;

	orig_vop->registered = true;
}
