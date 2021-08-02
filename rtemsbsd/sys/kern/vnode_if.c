#include <machine/rtems-bsd-kernel-space.h>

/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from $FreeBSD$
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
	(vop_bypass_t *)vop_panic,
	NULL,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_islocked_vp_offsets[] =  {
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
	while(vop != NULL && \
	    vop->vop_islocked == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_islocked(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_islocked, entry, a->a_vp, a);

	KTR_START1(KTR_VOP, "VOP", "VOP_ISLOCKED", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_islocked != NULL)
		rc = vop->vop_islocked(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_islocked, return, a->a_vp, a, rc);

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
	while(vop != NULL && \
	    vop->vop_lookup == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_lookup(%p, %p)", a->a_dvp, a));
	SDT_PROBE2(vfs, vop, vop_lookup, entry, a->a_dvp, a);

	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_LOOKUP");
	ASSERT_VOP_LOCKED(a->a_dvp, "VOP_LOOKUP");
	KTR_START3(KTR_VOP, "VOP", "VOP_LOOKUP", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp);
	VFS_PROLOGUE(a->a_dvp->v_mount);
	if (vop->vop_lookup != NULL)
		rc = vop->vop_lookup(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_dvp->v_mount);
	SDT_PROBE3(vfs, vop, vop_lookup, return, a->a_dvp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_LOOKUP");
		ASSERT_VOP_LOCKED(a->a_dvp, "VOP_LOOKUP");
		ASSERT_VI_UNLOCKED(*a->a_vpp, "VOP_LOOKUP");
		ASSERT_VOP_LOCKED(*a->a_vpp, "VOP_LOOKUP");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_LOOKUP");
		ASSERT_VOP_LOCKED(a->a_dvp, "VOP_LOOKUP");
	}
	KTR_STOP3(KTR_VOP, "VOP", "VOP_LOOKUP", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp);
	return (rc);
}

struct vnodeop_desc vop_lookup_desc = {
	"vop_lookup",
	0,
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
	while(vop != NULL && \
	    vop->vop_cachedlookup == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_cachedlookup(%p, %p)", a->a_dvp, a));
	SDT_PROBE2(vfs, vop, vop_cachedlookup, entry, a->a_dvp, a);

	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_CACHEDLOOKUP");
	ASSERT_VOP_LOCKED(a->a_dvp, "VOP_CACHEDLOOKUP");
	KTR_START3(KTR_VOP, "VOP", "VOP_CACHEDLOOKUP", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp);
	VFS_PROLOGUE(a->a_dvp->v_mount);
	if (vop->vop_cachedlookup != NULL)
		rc = vop->vop_cachedlookup(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_dvp->v_mount);
	SDT_PROBE3(vfs, vop, vop_cachedlookup, return, a->a_dvp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_CACHEDLOOKUP");
		ASSERT_VOP_LOCKED(a->a_dvp, "VOP_CACHEDLOOKUP");
		ASSERT_VI_UNLOCKED(*a->a_vpp, "VOP_CACHEDLOOKUP");
		ASSERT_VOP_LOCKED(*a->a_vpp, "VOP_CACHEDLOOKUP");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_CACHEDLOOKUP");
		ASSERT_VOP_LOCKED(a->a_dvp, "VOP_CACHEDLOOKUP");
	}
	KTR_STOP3(KTR_VOP, "VOP", "VOP_CACHEDLOOKUP", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp);
	return (rc);
}

struct vnodeop_desc vop_cachedlookup_desc = {
	"vop_cachedlookup",
	0,
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
	while(vop != NULL && \
	    vop->vop_create == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_create(%p, %p)", a->a_dvp, a));
	SDT_PROBE2(vfs, vop, vop_create, entry, a->a_dvp, a);

	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_CREATE");
	ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_CREATE");
	KTR_START4(KTR_VOP, "VOP", "VOP_CREATE", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp, "vap:0x%jX", a->a_vap);
	VFS_PROLOGUE(a->a_dvp->v_mount);
	if (vop->vop_create != NULL)
		rc = vop->vop_create(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_dvp->v_mount);
	SDT_PROBE3(vfs, vop, vop_create, return, a->a_dvp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_CREATE");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_CREATE");
		ASSERT_VI_UNLOCKED(*a->a_vpp, "VOP_CREATE");
		ASSERT_VOP_LOCKED(*a->a_vpp, "VOP_CREATE");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_CREATE");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_CREATE");
	}
	vop_create_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_CREATE", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp, "vap:0x%jX", a->a_vap);
	return (rc);
}

struct vnodeop_desc vop_create_desc = {
	"vop_create",
	0,
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
	while(vop != NULL && \
	    vop->vop_whiteout == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_whiteout(%p, %p)", a->a_dvp, a));
	SDT_PROBE2(vfs, vop, vop_whiteout, entry, a->a_dvp, a);

	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_WHITEOUT");
	ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_WHITEOUT");
	KTR_START3(KTR_VOP, "VOP", "VOP_WHITEOUT", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "cnp:0x%jX", a->a_cnp, "flags:0x%jX", a->a_flags);
	VFS_PROLOGUE(a->a_dvp->v_mount);
	if (vop->vop_whiteout != NULL)
		rc = vop->vop_whiteout(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_dvp->v_mount);
	SDT_PROBE3(vfs, vop, vop_whiteout, return, a->a_dvp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_WHITEOUT");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_WHITEOUT");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_WHITEOUT");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_WHITEOUT");
	}
	KTR_STOP3(KTR_VOP, "VOP", "VOP_WHITEOUT", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "cnp:0x%jX", a->a_cnp, "flags:0x%jX", a->a_flags);
	return (rc);
}

struct vnodeop_desc vop_whiteout_desc = {
	"vop_whiteout",
	0,
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
	while(vop != NULL && \
	    vop->vop_mknod == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_mknod(%p, %p)", a->a_dvp, a));
	SDT_PROBE2(vfs, vop, vop_mknod, entry, a->a_dvp, a);

	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_MKNOD");
	ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_MKNOD");
	KTR_START4(KTR_VOP, "VOP", "VOP_MKNOD", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp, "vap:0x%jX", a->a_vap);
	VFS_PROLOGUE(a->a_dvp->v_mount);
	if (vop->vop_mknod != NULL)
		rc = vop->vop_mknod(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_dvp->v_mount);
	SDT_PROBE3(vfs, vop, vop_mknod, return, a->a_dvp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_MKNOD");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_MKNOD");
		ASSERT_VI_UNLOCKED(*a->a_vpp, "VOP_MKNOD");
		ASSERT_VOP_LOCKED(*a->a_vpp, "VOP_MKNOD");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_MKNOD");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_MKNOD");
	}
	vop_mknod_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_MKNOD", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp, "vap:0x%jX", a->a_vap);
	return (rc);
}

struct vnodeop_desc vop_mknod_desc = {
	"vop_mknod",
	0,
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
	while(vop != NULL && \
	    vop->vop_open == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_open(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_open, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_OPEN");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_OPEN");
	KTR_START4(KTR_VOP, "VOP", "VOP_OPEN", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "mode:0x%jX", a->a_mode, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_open != NULL)
		rc = vop->vop_open(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_open, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_OPEN");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_OPEN");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_OPEN");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_OPEN");
	}
	vop_open_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_OPEN", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "mode:0x%jX", a->a_mode, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	return (rc);
}

struct vnodeop_desc vop_open_desc = {
	"vop_open",
	0,
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
	while(vop != NULL && \
	    vop->vop_close == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_close(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_close, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_CLOSE");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_CLOSE");
	KTR_START4(KTR_VOP, "VOP", "VOP_CLOSE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "fflag:0x%jX", a->a_fflag, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_close != NULL)
		rc = vop->vop_close(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_close, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_CLOSE");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_CLOSE");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_CLOSE");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_CLOSE");
	}
	vop_close_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_CLOSE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "fflag:0x%jX", a->a_fflag, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	return (rc);
}

struct vnodeop_desc vop_close_desc = {
	"vop_close",
	0,
	(vop_bypass_t *)VOP_CLOSE_AP,
	vop_close_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_close_args,a_cred),
	VOPARG_OFFSETOF(struct vop_close_args,a_td),
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
	while(vop != NULL && \
	    vop->vop_access == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_access(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_access, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACCESS");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_ACCESS");
	KTR_START4(KTR_VOP, "VOP", "VOP_ACCESS", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "accmode:0x%jX", a->a_accmode, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_access != NULL)
		rc = vop->vop_access(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_access, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACCESS");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_ACCESS");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACCESS");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_ACCESS");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_ACCESS", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "accmode:0x%jX", a->a_accmode, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	return (rc);
}

struct vnodeop_desc vop_access_desc = {
	"vop_access",
	0,
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
	while(vop != NULL && \
	    vop->vop_accessx == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_accessx(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_accessx, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACCESSX");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_ACCESSX");
	KTR_START4(KTR_VOP, "VOP", "VOP_ACCESSX", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "accmode:0x%jX", a->a_accmode, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_accessx != NULL)
		rc = vop->vop_accessx(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_accessx, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACCESSX");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_ACCESSX");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACCESSX");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_ACCESSX");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_ACCESSX", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "accmode:0x%jX", a->a_accmode, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	return (rc);
}

struct vnodeop_desc vop_accessx_desc = {
	"vop_accessx",
	0,
	(vop_bypass_t *)VOP_ACCESSX_AP,
	vop_accessx_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_accessx_args,a_cred),
	VOPARG_OFFSETOF(struct vop_accessx_args,a_td),
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
	while(vop != NULL && \
	    vop->vop_getattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_getattr(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_getattr, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETATTR");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETATTR");
	KTR_START3(KTR_VOP, "VOP", "VOP_GETATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "vap:0x%jX", a->a_vap, "cred:0x%jX", a->a_cred);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_getattr != NULL)
		rc = vop->vop_getattr(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_getattr, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETATTR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETATTR");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETATTR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETATTR");
	}
	KTR_STOP3(KTR_VOP, "VOP", "VOP_GETATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "vap:0x%jX", a->a_vap, "cred:0x%jX", a->a_cred);
	return (rc);
}

struct vnodeop_desc vop_getattr_desc = {
	"vop_getattr",
	0,
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
	while(vop != NULL && \
	    vop->vop_setattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_setattr(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_setattr, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETATTR");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETATTR");
	KTR_START3(KTR_VOP, "VOP", "VOP_SETATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "vap:0x%jX", a->a_vap, "cred:0x%jX", a->a_cred);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_setattr != NULL)
		rc = vop->vop_setattr(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_setattr, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETATTR");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETATTR");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETATTR");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETATTR");
	}
	vop_setattr_post(a, rc);
	KTR_STOP3(KTR_VOP, "VOP", "VOP_SETATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "vap:0x%jX", a->a_vap, "cred:0x%jX", a->a_cred);
	return (rc);
}

struct vnodeop_desc vop_setattr_desc = {
	"vop_setattr",
	0,
	(vop_bypass_t *)VOP_SETATTR_AP,
	vop_setattr_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_setattr_args,a_cred),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_markatime_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_markatime_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_markatime, entry, "struct vnode *", "struct vop_markatime_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_markatime, return, "struct vnode *", "struct vop_markatime_args *", "int");


int
VOP_MARKATIME_AP(struct vop_markatime_args *a)
{

	return(VOP_MARKATIME_APV(a->a_vp->v_op, a));
}

int
VOP_MARKATIME_APV(struct vop_vector *vop, struct vop_markatime_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_markatime_desc, a->a_vp,
	    ("Wrong a_desc in vop_markatime(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_markatime == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_markatime(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_markatime, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_MARKATIME");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_MARKATIME");
	KTR_START1(KTR_VOP, "VOP", "VOP_MARKATIME", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_markatime != NULL)
		rc = vop->vop_markatime(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_markatime, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_MARKATIME");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_MARKATIME");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_MARKATIME");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_MARKATIME");
	}
	KTR_STOP1(KTR_VOP, "VOP", "VOP_MARKATIME", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	return (rc);
}

struct vnodeop_desc vop_markatime_desc = {
	"vop_markatime",
	0,
	(vop_bypass_t *)VOP_MARKATIME_AP,
	vop_markatime_vp_offsets,
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
	while(vop != NULL && \
	    vop->vop_read == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_read(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_read, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READ");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_READ");
	KTR_START4(KTR_VOP, "VOP", "VOP_READ", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "uio:0x%jX", a->a_uio, "ioflag:0x%jX", a->a_ioflag, "cred:0x%jX", a->a_cred);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_read != NULL)
		rc = vop->vop_read(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_read, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READ");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_READ");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READ");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_READ");
	}
	vop_read_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_READ", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "uio:0x%jX", a->a_uio, "ioflag:0x%jX", a->a_ioflag, "cred:0x%jX", a->a_cred);
	return (rc);
}

struct vnodeop_desc vop_read_desc = {
	"vop_read",
	0,
	(vop_bypass_t *)VOP_READ_AP,
	vop_read_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_read_args,a_cred),
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
	while(vop != NULL && \
	    vop->vop_write == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_write(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_write, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_WRITE");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_WRITE");
	KTR_START4(KTR_VOP, "VOP", "VOP_WRITE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "uio:0x%jX", a->a_uio, "ioflag:0x%jX", a->a_ioflag, "cred:0x%jX", a->a_cred);
	VOP_WRITE_PRE(a);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_write != NULL)
		rc = vop->vop_write(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_write, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_WRITE");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_WRITE");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_WRITE");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_WRITE");
	}
	VOP_WRITE_POST(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_WRITE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "uio:0x%jX", a->a_uio, "ioflag:0x%jX", a->a_ioflag, "cred:0x%jX", a->a_cred);
	return (rc);
}

struct vnodeop_desc vop_write_desc = {
	"vop_write",
	0,
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
	while(vop != NULL && \
	    vop->vop_ioctl == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_ioctl(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_ioctl, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_IOCTL");
	ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_IOCTL");
	KTR_START4(KTR_VOP, "VOP", "VOP_IOCTL", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "command:0x%jX", a->a_command, "data:0x%jX", a->a_data, "fflag:0x%jX", a->a_fflag);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_ioctl != NULL)
		rc = vop->vop_ioctl(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_ioctl, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_IOCTL");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_IOCTL");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_IOCTL");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_IOCTL");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_IOCTL", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "command:0x%jX", a->a_command, "data:0x%jX", a->a_data, "fflag:0x%jX", a->a_fflag);
	return (rc);
}

struct vnodeop_desc vop_ioctl_desc = {
	"vop_ioctl",
	0,
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
	while(vop != NULL && \
	    vop->vop_poll == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_poll(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_poll, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_POLL");
	ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_POLL");
	KTR_START4(KTR_VOP, "VOP", "VOP_POLL", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "events:0x%jX", a->a_events, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_poll != NULL)
		rc = vop->vop_poll(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_poll, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_POLL");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_POLL");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_POLL");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_POLL");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_POLL", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "events:0x%jX", a->a_events, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	return (rc);
}

struct vnodeop_desc vop_poll_desc = {
	"vop_poll",
	0,
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
	while(vop != NULL && \
	    vop->vop_kqfilter == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_kqfilter(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_kqfilter, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_KQFILTER");
	ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_KQFILTER");
	KTR_START2(KTR_VOP, "VOP", "VOP_KQFILTER", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "kn:0x%jX", a->a_kn);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_kqfilter != NULL)
		rc = vop->vop_kqfilter(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_kqfilter, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_KQFILTER");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_KQFILTER");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_KQFILTER");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_KQFILTER");
	}
	KTR_STOP2(KTR_VOP, "VOP", "VOP_KQFILTER", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "kn:0x%jX", a->a_kn);
	return (rc);
}

struct vnodeop_desc vop_kqfilter_desc = {
	"vop_kqfilter",
	0,
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
	while(vop != NULL && \
	    vop->vop_revoke == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_revoke(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_revoke, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REVOKE");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_REVOKE");
	KTR_START2(KTR_VOP, "VOP", "VOP_REVOKE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "flags:0x%jX", a->a_flags);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_revoke != NULL)
		rc = vop->vop_revoke(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_revoke, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REVOKE");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_REVOKE");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REVOKE");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_REVOKE");
	}
	KTR_STOP2(KTR_VOP, "VOP", "VOP_REVOKE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "flags:0x%jX", a->a_flags);
	return (rc);
}

struct vnodeop_desc vop_revoke_desc = {
	"vop_revoke",
	0,
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
	while(vop != NULL && \
	    vop->vop_fsync == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_fsync(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_fsync, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_FSYNC");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_FSYNC");
	KTR_START3(KTR_VOP, "VOP", "VOP_FSYNC", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "waitfor:0x%jX", a->a_waitfor, "td:0x%jX", a->a_td);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_fsync != NULL)
		rc = vop->vop_fsync(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_fsync, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_FSYNC");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_FSYNC");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_FSYNC");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_FSYNC");
	}
	KTR_STOP3(KTR_VOP, "VOP", "VOP_FSYNC", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "waitfor:0x%jX", a->a_waitfor, "td:0x%jX", a->a_td);
	return (rc);
}

struct vnodeop_desc vop_fsync_desc = {
	"vop_fsync",
	0,
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
	while(vop != NULL && \
	    vop->vop_remove == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_remove(%p, %p)", a->a_dvp, a));
	SDT_PROBE2(vfs, vop, vop_remove, entry, a->a_dvp, a);

	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_REMOVE");
	ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_REMOVE");
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REMOVE");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_REMOVE");
	KTR_START3(KTR_VOP, "VOP", "VOP_REMOVE", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vp:0x%jX", a->a_vp, "cnp:0x%jX", a->a_cnp);
	VFS_PROLOGUE(a->a_dvp->v_mount);
	if (vop->vop_remove != NULL)
		rc = vop->vop_remove(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_dvp->v_mount);
	SDT_PROBE3(vfs, vop, vop_remove, return, a->a_dvp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_REMOVE");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_REMOVE");
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REMOVE");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_REMOVE");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_REMOVE");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_REMOVE");
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REMOVE");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_REMOVE");
	}
	vop_remove_post(a, rc);
	KTR_STOP3(KTR_VOP, "VOP", "VOP_REMOVE", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vp:0x%jX", a->a_vp, "cnp:0x%jX", a->a_cnp);
	return (rc);
}

struct vnodeop_desc vop_remove_desc = {
	"vop_remove",
	0,
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
	while(vop != NULL && \
	    vop->vop_link == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_tdvp, ("No vop_link(%p, %p)", a->a_tdvp, a));
	SDT_PROBE2(vfs, vop, vop_link, entry, a->a_tdvp, a);

	ASSERT_VI_UNLOCKED(a->a_tdvp, "VOP_LINK");
	ASSERT_VOP_ELOCKED(a->a_tdvp, "VOP_LINK");
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_LINK");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_LINK");
	KTR_START3(KTR_VOP, "VOP", "VOP_LINK", (uintptr_t)a,
	    "tdvp:0x%jX", (uintptr_t)a->a_tdvp, "vp:0x%jX", a->a_vp, "cnp:0x%jX", a->a_cnp);
	VFS_PROLOGUE(a->a_tdvp->v_mount);
	if (vop->vop_link != NULL)
		rc = vop->vop_link(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_tdvp->v_mount);
	SDT_PROBE3(vfs, vop, vop_link, return, a->a_tdvp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_tdvp, "VOP_LINK");
		ASSERT_VOP_ELOCKED(a->a_tdvp, "VOP_LINK");
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_LINK");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_LINK");
	} else {
		ASSERT_VI_UNLOCKED(a->a_tdvp, "VOP_LINK");
		ASSERT_VOP_ELOCKED(a->a_tdvp, "VOP_LINK");
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_LINK");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_LINK");
	}
	vop_link_post(a, rc);
	KTR_STOP3(KTR_VOP, "VOP", "VOP_LINK", (uintptr_t)a,
	    "tdvp:0x%jX", (uintptr_t)a->a_tdvp, "vp:0x%jX", a->a_vp, "cnp:0x%jX", a->a_cnp);
	return (rc);
}

struct vnodeop_desc vop_link_desc = {
	"vop_link",
	0,
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
	while(vop != NULL && \
	    vop->vop_rename == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_fdvp, ("No vop_rename(%p, %p)", a->a_fdvp, a));
	SDT_PROBE2(vfs, vop, vop_rename, entry, a->a_fdvp, a);

	KTR_START4(KTR_VOP, "VOP", "VOP_RENAME", (uintptr_t)a,
	    "fdvp:0x%jX", (uintptr_t)a->a_fdvp, "fvp:0x%jX", a->a_fvp, "fcnp:0x%jX", a->a_fcnp, "tdvp:0x%jX", a->a_tdvp);
	vop_rename_pre(a);
	VFS_PROLOGUE(a->a_fdvp->v_mount);
	if (vop->vop_rename != NULL)
		rc = vop->vop_rename(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_fdvp->v_mount);
	SDT_PROBE3(vfs, vop, vop_rename, return, a->a_fdvp, a, rc);

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
	while(vop != NULL && \
	    vop->vop_mkdir == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_mkdir(%p, %p)", a->a_dvp, a));
	SDT_PROBE2(vfs, vop, vop_mkdir, entry, a->a_dvp, a);

	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_MKDIR");
	ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_MKDIR");
	KTR_START4(KTR_VOP, "VOP", "VOP_MKDIR", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp, "vap:0x%jX", a->a_vap);
	VFS_PROLOGUE(a->a_dvp->v_mount);
	if (vop->vop_mkdir != NULL)
		rc = vop->vop_mkdir(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_dvp->v_mount);
	SDT_PROBE3(vfs, vop, vop_mkdir, return, a->a_dvp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_MKDIR");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_MKDIR");
		ASSERT_VI_UNLOCKED(*a->a_vpp, "VOP_MKDIR");
		ASSERT_VOP_ELOCKED(*a->a_vpp, "VOP_MKDIR");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_MKDIR");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_MKDIR");
	}
	vop_mkdir_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_MKDIR", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp, "vap:0x%jX", a->a_vap);
	return (rc);
}

struct vnodeop_desc vop_mkdir_desc = {
	"vop_mkdir",
	0,
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
	while(vop != NULL && \
	    vop->vop_rmdir == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_rmdir(%p, %p)", a->a_dvp, a));
	SDT_PROBE2(vfs, vop, vop_rmdir, entry, a->a_dvp, a);

	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_RMDIR");
	ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_RMDIR");
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_RMDIR");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_RMDIR");
	KTR_START3(KTR_VOP, "VOP", "VOP_RMDIR", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vp:0x%jX", a->a_vp, "cnp:0x%jX", a->a_cnp);
	VFS_PROLOGUE(a->a_dvp->v_mount);
	if (vop->vop_rmdir != NULL)
		rc = vop->vop_rmdir(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_dvp->v_mount);
	SDT_PROBE3(vfs, vop, vop_rmdir, return, a->a_dvp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_RMDIR");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_RMDIR");
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_RMDIR");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_RMDIR");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_RMDIR");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_RMDIR");
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_RMDIR");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_RMDIR");
	}
	vop_rmdir_post(a, rc);
	KTR_STOP3(KTR_VOP, "VOP", "VOP_RMDIR", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vp:0x%jX", a->a_vp, "cnp:0x%jX", a->a_cnp);
	return (rc);
}

struct vnodeop_desc vop_rmdir_desc = {
	"vop_rmdir",
	0,
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
	while(vop != NULL && \
	    vop->vop_symlink == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_symlink(%p, %p)", a->a_dvp, a));
	SDT_PROBE2(vfs, vop, vop_symlink, entry, a->a_dvp, a);

	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_SYMLINK");
	ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_SYMLINK");
	KTR_START4(KTR_VOP, "VOP", "VOP_SYMLINK", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp, "vap:0x%jX", a->a_vap);
	VFS_PROLOGUE(a->a_dvp->v_mount);
	if (vop->vop_symlink != NULL)
		rc = vop->vop_symlink(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_dvp->v_mount);
	SDT_PROBE3(vfs, vop, vop_symlink, return, a->a_dvp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_SYMLINK");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_SYMLINK");
		ASSERT_VI_UNLOCKED(*a->a_vpp, "VOP_SYMLINK");
		ASSERT_VOP_ELOCKED(*a->a_vpp, "VOP_SYMLINK");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_SYMLINK");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_SYMLINK");
	}
	vop_symlink_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_SYMLINK", (uintptr_t)a,
	    "dvp:0x%jX", (uintptr_t)a->a_dvp, "vpp:0x%jX", a->a_vpp, "cnp:0x%jX", a->a_cnp, "vap:0x%jX", a->a_vap);
	return (rc);
}

struct vnodeop_desc vop_symlink_desc = {
	"vop_symlink",
	0,
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
	while(vop != NULL && \
	    vop->vop_readdir == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_readdir(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_readdir, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READDIR");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_READDIR");
	KTR_START4(KTR_VOP, "VOP", "VOP_READDIR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "uio:0x%jX", a->a_uio, "cred:0x%jX", a->a_cred, "eofflag:0x%jX", a->a_eofflag);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_readdir != NULL)
		rc = vop->vop_readdir(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_readdir, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READDIR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_READDIR");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READDIR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_READDIR");
	}
	vop_readdir_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_READDIR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "uio:0x%jX", a->a_uio, "cred:0x%jX", a->a_cred, "eofflag:0x%jX", a->a_eofflag);
	return (rc);
}

struct vnodeop_desc vop_readdir_desc = {
	"vop_readdir",
	0,
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
	while(vop != NULL && \
	    vop->vop_readlink == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_readlink(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_readlink, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READLINK");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_READLINK");
	KTR_START3(KTR_VOP, "VOP", "VOP_READLINK", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "uio:0x%jX", a->a_uio, "cred:0x%jX", a->a_cred);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_readlink != NULL)
		rc = vop->vop_readlink(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_readlink, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READLINK");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_READLINK");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READLINK");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_READLINK");
	}
	KTR_STOP3(KTR_VOP, "VOP", "VOP_READLINK", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "uio:0x%jX", a->a_uio, "cred:0x%jX", a->a_cred);
	return (rc);
}

struct vnodeop_desc vop_readlink_desc = {
	"vop_readlink",
	0,
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
	while(vop != NULL && \
	    vop->vop_inactive == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_inactive(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_inactive, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_INACTIVE");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_INACTIVE");
	KTR_START2(KTR_VOP, "VOP", "VOP_INACTIVE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "td:0x%jX", a->a_td);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_inactive != NULL)
		rc = vop->vop_inactive(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_inactive, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_INACTIVE");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_INACTIVE");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_INACTIVE");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_INACTIVE");
	}
	KTR_STOP2(KTR_VOP, "VOP", "VOP_INACTIVE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "td:0x%jX", a->a_td);
	return (rc);
}

struct vnodeop_desc vop_inactive_desc = {
	"vop_inactive",
	0,
	(vop_bypass_t *)VOP_INACTIVE_AP,
	vop_inactive_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_inactive_args,a_td),
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
	while(vop != NULL && \
	    vop->vop_reclaim == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_reclaim(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_reclaim, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_RECLAIM");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_RECLAIM");
	KTR_START2(KTR_VOP, "VOP", "VOP_RECLAIM", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "td:0x%jX", a->a_td);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_reclaim != NULL)
		rc = vop->vop_reclaim(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_reclaim, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_RECLAIM");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_RECLAIM");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_RECLAIM");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_RECLAIM");
	}
	vop_reclaim_post(a, rc);
	KTR_STOP2(KTR_VOP, "VOP", "VOP_RECLAIM", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "td:0x%jX", a->a_td);
	return (rc);
}

struct vnodeop_desc vop_reclaim_desc = {
	"vop_reclaim",
	0,
	(vop_bypass_t *)VOP_RECLAIM_AP,
	vop_reclaim_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_reclaim_args,a_td),
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
	while(vop != NULL && \
	    vop->vop_lock1 == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_lock1(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_lock1, entry, a->a_vp, a);

	KTR_START4(KTR_VOP, "VOP", "VOP_LOCK1", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "flags:0x%jX", a->a_flags, "file:0x%jX", a->a_file, "line:0x%jX", a->a_line);
	vop_lock_pre(a);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_lock1 != NULL)
		rc = vop->vop_lock1(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_lock1, return, a->a_vp, a, rc);

	if (rc == 0) {
	} else {
	}
	vop_lock_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_LOCK1", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "flags:0x%jX", a->a_flags, "file:0x%jX", a->a_file, "line:0x%jX", a->a_line);
	return (rc);
}

struct vnodeop_desc vop_lock1_desc = {
	"vop_lock1",
	0,
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
	while(vop != NULL && \
	    vop->vop_unlock == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_unlock(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_unlock, entry, a->a_vp, a);

	KTR_START2(KTR_VOP, "VOP", "VOP_UNLOCK", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "flags:0x%jX", a->a_flags);
	vop_unlock_pre(a);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_unlock != NULL)
		rc = vop->vop_unlock(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_unlock, return, a->a_vp, a, rc);

	if (rc == 0) {
	} else {
	}
	vop_unlock_post(a, rc);
	KTR_STOP2(KTR_VOP, "VOP", "VOP_UNLOCK", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "flags:0x%jX", a->a_flags);
	return (rc);
}

struct vnodeop_desc vop_unlock_desc = {
	"vop_unlock",
	0,
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
	while(vop != NULL && \
	    vop->vop_bmap == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_bmap(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_bmap, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_BMAP");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_BMAP");
	KTR_START4(KTR_VOP, "VOP", "VOP_BMAP", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "bn:0x%jX", a->a_bn, "bop:0x%jX", a->a_bop, "bnp:0x%jX", a->a_bnp);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_bmap != NULL)
		rc = vop->vop_bmap(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_bmap, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_BMAP");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_BMAP");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_BMAP");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_BMAP");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_BMAP", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "bn:0x%jX", a->a_bn, "bop:0x%jX", a->a_bop, "bnp:0x%jX", a->a_bnp);
	return (rc);
}

struct vnodeop_desc vop_bmap_desc = {
	"vop_bmap",
	0,
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
	while(vop != NULL && \
	    vop->vop_strategy == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_strategy(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_strategy, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_STRATEGY");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_STRATEGY");
	KTR_START2(KTR_VOP, "VOP", "VOP_STRATEGY", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "bp:0x%jX", a->a_bp);
	vop_strategy_pre(a);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_strategy != NULL)
		rc = vop->vop_strategy(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_strategy, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_STRATEGY");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_STRATEGY");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_STRATEGY");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_STRATEGY");
	}
	KTR_STOP2(KTR_VOP, "VOP", "VOP_STRATEGY", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "bp:0x%jX", a->a_bp);
	return (rc);
}

struct vnodeop_desc vop_strategy_desc = {
	"vop_strategy",
	0,
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
	while(vop != NULL && \
	    vop->vop_getwritemount == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_getwritemount(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_getwritemount, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETWRITEMOUNT");
	KTR_START2(KTR_VOP, "VOP", "VOP_GETWRITEMOUNT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "mpp:0x%jX", a->a_mpp);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_getwritemount != NULL)
		rc = vop->vop_getwritemount(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_getwritemount, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETWRITEMOUNT");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETWRITEMOUNT");
	}
	KTR_STOP2(KTR_VOP, "VOP", "VOP_GETWRITEMOUNT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "mpp:0x%jX", a->a_mpp);
	return (rc);
}

struct vnodeop_desc vop_getwritemount_desc = {
	"vop_getwritemount",
	0,
	(vop_bypass_t *)VOP_GETWRITEMOUNT_AP,
	vop_getwritemount_vp_offsets,
	VDESC_NO_OFFSET,
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
	while(vop != NULL && \
	    vop->vop_print == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_print(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_print, entry, a->a_vp, a);

	KTR_START1(KTR_VOP, "VOP", "VOP_PRINT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_print != NULL)
		rc = vop->vop_print(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_print, return, a->a_vp, a, rc);

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
	while(vop != NULL && \
	    vop->vop_pathconf == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_pathconf(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_pathconf, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_PATHCONF");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_PATHCONF");
	KTR_START3(KTR_VOP, "VOP", "VOP_PATHCONF", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "name:0x%jX", a->a_name, "retval:0x%jX", a->a_retval);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_pathconf != NULL)
		rc = vop->vop_pathconf(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_pathconf, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_PATHCONF");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_PATHCONF");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_PATHCONF");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_PATHCONF");
	}
	KTR_STOP3(KTR_VOP, "VOP", "VOP_PATHCONF", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "name:0x%jX", a->a_name, "retval:0x%jX", a->a_retval);
	return (rc);
}

struct vnodeop_desc vop_pathconf_desc = {
	"vop_pathconf",
	0,
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
	while(vop != NULL && \
	    vop->vop_advlock == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_advlock(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_advlock, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCK");
	ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVLOCK");
	KTR_START4(KTR_VOP, "VOP", "VOP_ADVLOCK", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "id:0x%jX", a->a_id, "op:0x%jX", a->a_op, "fl:0x%jX", a->a_fl);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_advlock != NULL)
		rc = vop->vop_advlock(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_advlock, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCK");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVLOCK");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCK");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVLOCK");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_ADVLOCK", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "id:0x%jX", a->a_id, "op:0x%jX", a->a_op, "fl:0x%jX", a->a_fl);
	return (rc);
}

struct vnodeop_desc vop_advlock_desc = {
	"vop_advlock",
	0,
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
	while(vop != NULL && \
	    vop->vop_advlockasync == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_advlockasync(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_advlockasync, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCKASYNC");
	ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVLOCKASYNC");
	KTR_START4(KTR_VOP, "VOP", "VOP_ADVLOCKASYNC", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "id:0x%jX", a->a_id, "op:0x%jX", a->a_op, "fl:0x%jX", a->a_fl);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_advlockasync != NULL)
		rc = vop->vop_advlockasync(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_advlockasync, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCKASYNC");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVLOCKASYNC");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCKASYNC");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVLOCKASYNC");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_ADVLOCKASYNC", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "id:0x%jX", a->a_id, "op:0x%jX", a->a_op, "fl:0x%jX", a->a_fl);
	return (rc);
}

struct vnodeop_desc vop_advlockasync_desc = {
	"vop_advlockasync",
	0,
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
	while(vop != NULL && \
	    vop->vop_advlockpurge == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_advlockpurge(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_advlockpurge, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCKPURGE");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_ADVLOCKPURGE");
	KTR_START1(KTR_VOP, "VOP", "VOP_ADVLOCKPURGE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_advlockpurge != NULL)
		rc = vop->vop_advlockpurge(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_advlockpurge, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCKPURGE");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_ADVLOCKPURGE");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCKPURGE");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_ADVLOCKPURGE");
	}
	KTR_STOP1(KTR_VOP, "VOP", "VOP_ADVLOCKPURGE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	return (rc);
}

struct vnodeop_desc vop_advlockpurge_desc = {
	"vop_advlockpurge",
	0,
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
	while(vop != NULL && \
	    vop->vop_reallocblks == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_reallocblks(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_reallocblks, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REALLOCBLKS");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_REALLOCBLKS");
	KTR_START2(KTR_VOP, "VOP", "VOP_REALLOCBLKS", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "buflist:0x%jX", a->a_buflist);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_reallocblks != NULL)
		rc = vop->vop_reallocblks(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_reallocblks, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REALLOCBLKS");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_REALLOCBLKS");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REALLOCBLKS");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_REALLOCBLKS");
	}
	KTR_STOP2(KTR_VOP, "VOP", "VOP_REALLOCBLKS", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "buflist:0x%jX", a->a_buflist);
	return (rc);
}

struct vnodeop_desc vop_reallocblks_desc = {
	"vop_reallocblks",
	0,
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
	while(vop != NULL && \
	    vop->vop_getpages == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_getpages(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_getpages, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETPAGES");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETPAGES");
	KTR_START4(KTR_VOP, "VOP", "VOP_GETPAGES", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "m:0x%jX", a->a_m, "count:0x%jX", a->a_count, "rbehind:0x%jX", a->a_rbehind);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_getpages != NULL)
		rc = vop->vop_getpages(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_getpages, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETPAGES");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETPAGES");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETPAGES");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETPAGES");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_GETPAGES", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "m:0x%jX", a->a_m, "count:0x%jX", a->a_count, "rbehind:0x%jX", a->a_rbehind);
	return (rc);
}

struct vnodeop_desc vop_getpages_desc = {
	"vop_getpages",
	0,
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
	while(vop != NULL && \
	    vop->vop_getpages_async == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_getpages_async(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_getpages_async, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETPAGES_ASYNC");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETPAGES_ASYNC");
	KTR_START4(KTR_VOP, "VOP", "VOP_GETPAGES_ASYNC", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "m:0x%jX", a->a_m, "count:0x%jX", a->a_count, "rbehind:0x%jX", a->a_rbehind);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_getpages_async != NULL)
		rc = vop->vop_getpages_async(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_getpages_async, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETPAGES_ASYNC");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETPAGES_ASYNC");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETPAGES_ASYNC");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETPAGES_ASYNC");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_GETPAGES_ASYNC", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "m:0x%jX", a->a_m, "count:0x%jX", a->a_count, "rbehind:0x%jX", a->a_rbehind);
	return (rc);
}

struct vnodeop_desc vop_getpages_async_desc = {
	"vop_getpages_async",
	0,
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
	while(vop != NULL && \
	    vop->vop_putpages == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_putpages(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_putpages, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_PUTPAGES");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_PUTPAGES");
	KTR_START4(KTR_VOP, "VOP", "VOP_PUTPAGES", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "m:0x%jX", a->a_m, "count:0x%jX", a->a_count, "sync:0x%jX", a->a_sync);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_putpages != NULL)
		rc = vop->vop_putpages(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_putpages, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_PUTPAGES");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_PUTPAGES");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_PUTPAGES");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_PUTPAGES");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_PUTPAGES", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "m:0x%jX", a->a_m, "count:0x%jX", a->a_count, "sync:0x%jX", a->a_sync);
	return (rc);
}

struct vnodeop_desc vop_putpages_desc = {
	"vop_putpages",
	0,
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
	while(vop != NULL && \
	    vop->vop_getacl == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_getacl(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_getacl, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETACL");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETACL");
	KTR_START4(KTR_VOP, "VOP", "VOP_GETACL", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "type:0x%jX", a->a_type, "aclp:0x%jX", a->a_aclp, "cred:0x%jX", a->a_cred);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_getacl != NULL)
		rc = vop->vop_getacl(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_getacl, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETACL");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETACL");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETACL");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETACL");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_GETACL", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "type:0x%jX", a->a_type, "aclp:0x%jX", a->a_aclp, "cred:0x%jX", a->a_cred);
	return (rc);
}

struct vnodeop_desc vop_getacl_desc = {
	"vop_getacl",
	0,
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
	while(vop != NULL && \
	    vop->vop_setacl == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_setacl(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_setacl, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETACL");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETACL");
	KTR_START4(KTR_VOP, "VOP", "VOP_SETACL", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "type:0x%jX", a->a_type, "aclp:0x%jX", a->a_aclp, "cred:0x%jX", a->a_cred);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_setacl != NULL)
		rc = vop->vop_setacl(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_setacl, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETACL");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETACL");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETACL");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETACL");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_SETACL", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "type:0x%jX", a->a_type, "aclp:0x%jX", a->a_aclp, "cred:0x%jX", a->a_cred);
	return (rc);
}

struct vnodeop_desc vop_setacl_desc = {
	"vop_setacl",
	0,
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
	while(vop != NULL && \
	    vop->vop_aclcheck == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_aclcheck(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_aclcheck, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACLCHECK");
	KTR_START4(KTR_VOP, "VOP", "VOP_ACLCHECK", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "type:0x%jX", a->a_type, "aclp:0x%jX", a->a_aclp, "cred:0x%jX", a->a_cred);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_aclcheck != NULL)
		rc = vop->vop_aclcheck(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_aclcheck, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACLCHECK");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACLCHECK");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_ACLCHECK", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "type:0x%jX", a->a_type, "aclp:0x%jX", a->a_aclp, "cred:0x%jX", a->a_cred);
	return (rc);
}

struct vnodeop_desc vop_aclcheck_desc = {
	"vop_aclcheck",
	0,
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
	while(vop != NULL && \
	    vop->vop_closeextattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_closeextattr(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_closeextattr, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_CLOSEEXTATTR");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_CLOSEEXTATTR");
	KTR_START4(KTR_VOP, "VOP", "VOP_CLOSEEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "commit:0x%jX", a->a_commit, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_closeextattr != NULL)
		rc = vop->vop_closeextattr(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_closeextattr, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_CLOSEEXTATTR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_CLOSEEXTATTR");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_CLOSEEXTATTR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_CLOSEEXTATTR");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_CLOSEEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "commit:0x%jX", a->a_commit, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	return (rc);
}

struct vnodeop_desc vop_closeextattr_desc = {
	"vop_closeextattr",
	0,
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
	while(vop != NULL && \
	    vop->vop_getextattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_getextattr(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_getextattr, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETEXTATTR");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETEXTATTR");
	KTR_START4(KTR_VOP, "VOP", "VOP_GETEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "attrnamespace:0x%jX", a->a_attrnamespace, "name:0x%jX", a->a_name, "uio:0x%jX", a->a_uio);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_getextattr != NULL)
		rc = vop->vop_getextattr(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_getextattr, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETEXTATTR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETEXTATTR");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETEXTATTR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETEXTATTR");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_GETEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "attrnamespace:0x%jX", a->a_attrnamespace, "name:0x%jX", a->a_name, "uio:0x%jX", a->a_uio);
	return (rc);
}

struct vnodeop_desc vop_getextattr_desc = {
	"vop_getextattr",
	0,
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
	while(vop != NULL && \
	    vop->vop_listextattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_listextattr(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_listextattr, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_LISTEXTATTR");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_LISTEXTATTR");
	KTR_START4(KTR_VOP, "VOP", "VOP_LISTEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "attrnamespace:0x%jX", a->a_attrnamespace, "uio:0x%jX", a->a_uio, "size:0x%jX", a->a_size);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_listextattr != NULL)
		rc = vop->vop_listextattr(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_listextattr, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_LISTEXTATTR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_LISTEXTATTR");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_LISTEXTATTR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_LISTEXTATTR");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_LISTEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "attrnamespace:0x%jX", a->a_attrnamespace, "uio:0x%jX", a->a_uio, "size:0x%jX", a->a_size);
	return (rc);
}

struct vnodeop_desc vop_listextattr_desc = {
	"vop_listextattr",
	0,
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
	while(vop != NULL && \
	    vop->vop_openextattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_openextattr(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_openextattr, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_OPENEXTATTR");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_OPENEXTATTR");
	KTR_START3(KTR_VOP, "VOP", "VOP_OPENEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_openextattr != NULL)
		rc = vop->vop_openextattr(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_openextattr, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_OPENEXTATTR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_OPENEXTATTR");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_OPENEXTATTR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_OPENEXTATTR");
	}
	KTR_STOP3(KTR_VOP, "VOP", "VOP_OPENEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	return (rc);
}

struct vnodeop_desc vop_openextattr_desc = {
	"vop_openextattr",
	0,
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
	while(vop != NULL && \
	    vop->vop_deleteextattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_deleteextattr(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_deleteextattr, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_DELETEEXTATTR");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_DELETEEXTATTR");
	KTR_START4(KTR_VOP, "VOP", "VOP_DELETEEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "attrnamespace:0x%jX", a->a_attrnamespace, "name:0x%jX", a->a_name, "cred:0x%jX", a->a_cred);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_deleteextattr != NULL)
		rc = vop->vop_deleteextattr(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_deleteextattr, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_DELETEEXTATTR");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_DELETEEXTATTR");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_DELETEEXTATTR");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_DELETEEXTATTR");
	}
	vop_deleteextattr_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_DELETEEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "attrnamespace:0x%jX", a->a_attrnamespace, "name:0x%jX", a->a_name, "cred:0x%jX", a->a_cred);
	return (rc);
}

struct vnodeop_desc vop_deleteextattr_desc = {
	"vop_deleteextattr",
	0,
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
	while(vop != NULL && \
	    vop->vop_setextattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_setextattr(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_setextattr, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETEXTATTR");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETEXTATTR");
	KTR_START4(KTR_VOP, "VOP", "VOP_SETEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "attrnamespace:0x%jX", a->a_attrnamespace, "name:0x%jX", a->a_name, "uio:0x%jX", a->a_uio);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_setextattr != NULL)
		rc = vop->vop_setextattr(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_setextattr, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETEXTATTR");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETEXTATTR");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETEXTATTR");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETEXTATTR");
	}
	vop_setextattr_post(a, rc);
	KTR_STOP4(KTR_VOP, "VOP", "VOP_SETEXTATTR", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "attrnamespace:0x%jX", a->a_attrnamespace, "name:0x%jX", a->a_name, "uio:0x%jX", a->a_uio);
	return (rc);
}

struct vnodeop_desc vop_setextattr_desc = {
	"vop_setextattr",
	0,
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
	while(vop != NULL && \
	    vop->vop_setlabel == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_setlabel(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_setlabel, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETLABEL");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETLABEL");
	KTR_START4(KTR_VOP, "VOP", "VOP_SETLABEL", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "label:0x%jX", a->a_label, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_setlabel != NULL)
		rc = vop->vop_setlabel(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_setlabel, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETLABEL");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETLABEL");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETLABEL");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETLABEL");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_SETLABEL", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "label:0x%jX", a->a_label, "cred:0x%jX", a->a_cred, "td:0x%jX", a->a_td);
	return (rc);
}

struct vnodeop_desc vop_setlabel_desc = {
	"vop_setlabel",
	0,
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
	while(vop != NULL && \
	    vop->vop_vptofh == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_vptofh(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_vptofh, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_VPTOFH");
	KTR_START2(KTR_VOP, "VOP", "VOP_VPTOFH", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "fhp:0x%jX", a->a_fhp);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_vptofh != NULL)
		rc = vop->vop_vptofh(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_vptofh, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_VPTOFH");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_VPTOFH");
	}
	KTR_STOP2(KTR_VOP, "VOP", "VOP_VPTOFH", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "fhp:0x%jX", a->a_fhp);
	return (rc);
}

struct vnodeop_desc vop_vptofh_desc = {
	"vop_vptofh",
	0,
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
	while(vop != NULL && \
	    vop->vop_vptocnp == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_vptocnp(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_vptocnp, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_VPTOCNP");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_VPTOCNP");
	KTR_START4(KTR_VOP, "VOP", "VOP_VPTOCNP", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "vpp:0x%jX", a->a_vpp, "cred:0x%jX", a->a_cred, "buf:0x%jX", a->a_buf);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_vptocnp != NULL)
		rc = vop->vop_vptocnp(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_vptocnp, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_VPTOCNP");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_VPTOCNP");
		ASSERT_VI_UNLOCKED(*a->a_vpp, "VOP_VPTOCNP");
		ASSERT_VOP_UNLOCKED(*a->a_vpp, "VOP_VPTOCNP");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_VPTOCNP");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_VPTOCNP");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_VPTOCNP", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "vpp:0x%jX", a->a_vpp, "cred:0x%jX", a->a_cred, "buf:0x%jX", a->a_buf);
	return (rc);
}

struct vnodeop_desc vop_vptocnp_desc = {
	"vop_vptocnp",
	0,
	(vop_bypass_t *)VOP_VPTOCNP_AP,
	vop_vptocnp_vp_offsets,
	VOPARG_OFFSETOF(struct vop_vptocnp_args,a_vpp),
	VOPARG_OFFSETOF(struct vop_vptocnp_args,a_cred),
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
	while(vop != NULL && \
	    vop->vop_allocate == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_allocate(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_allocate, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ALLOCATE");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_ALLOCATE");
	KTR_START3(KTR_VOP, "VOP", "VOP_ALLOCATE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "offset:0x%jX", a->a_offset, "len:0x%jX", a->a_len);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_allocate != NULL)
		rc = vop->vop_allocate(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_allocate, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ALLOCATE");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_ALLOCATE");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ALLOCATE");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_ALLOCATE");
	}
	KTR_STOP3(KTR_VOP, "VOP", "VOP_ALLOCATE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "offset:0x%jX", a->a_offset, "len:0x%jX", a->a_len);
	return (rc);
}

struct vnodeop_desc vop_allocate_desc = {
	"vop_allocate",
	0,
	(vop_bypass_t *)VOP_ALLOCATE_AP,
	vop_allocate_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
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
	while(vop != NULL && \
	    vop->vop_advise == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_advise(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_advise, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVISE");
	ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVISE");
	KTR_START4(KTR_VOP, "VOP", "VOP_ADVISE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "start:0x%jX", a->a_start, "end:0x%jX", a->a_end, "advice:0x%jX", a->a_advice);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_advise != NULL)
		rc = vop->vop_advise(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_advise, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVISE");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVISE");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVISE");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVISE");
	}
	KTR_STOP4(KTR_VOP, "VOP", "VOP_ADVISE", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "start:0x%jX", a->a_start, "end:0x%jX", a->a_end, "advice:0x%jX", a->a_advice);
	return (rc);
}

struct vnodeop_desc vop_advise_desc = {
	"vop_advise",
	0,
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
	while(vop != NULL && \
	    vop->vop_unp_bind == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_unp_bind(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_unp_bind, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_UNP_BIND");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_UNP_BIND");
	KTR_START2(KTR_VOP, "VOP", "VOP_UNP_BIND", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "unpcb:0x%jX", a->a_unpcb);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_unp_bind != NULL)
		rc = vop->vop_unp_bind(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_unp_bind, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_UNP_BIND");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_UNP_BIND");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_UNP_BIND");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_UNP_BIND");
	}
	KTR_STOP2(KTR_VOP, "VOP", "VOP_UNP_BIND", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "unpcb:0x%jX", a->a_unpcb);
	return (rc);
}

struct vnodeop_desc vop_unp_bind_desc = {
	"vop_unp_bind",
	0,
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
	while(vop != NULL && \
	    vop->vop_unp_connect == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_unp_connect(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_unp_connect, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_UNP_CONNECT");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_UNP_CONNECT");
	KTR_START2(KTR_VOP, "VOP", "VOP_UNP_CONNECT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "unpcb:0x%jX", a->a_unpcb);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_unp_connect != NULL)
		rc = vop->vop_unp_connect(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_unp_connect, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_UNP_CONNECT");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_UNP_CONNECT");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_UNP_CONNECT");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_UNP_CONNECT");
	}
	KTR_STOP2(KTR_VOP, "VOP", "VOP_UNP_CONNECT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "unpcb:0x%jX", a->a_unpcb);
	return (rc);
}

struct vnodeop_desc vop_unp_connect_desc = {
	"vop_unp_connect",
	0,
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
	while(vop != NULL && \
	    vop->vop_unp_detach == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_unp_detach(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_unp_detach, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_UNP_DETACH");
	KTR_START1(KTR_VOP, "VOP", "VOP_UNP_DETACH", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_unp_detach != NULL)
		rc = vop->vop_unp_detach(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_unp_detach, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_UNP_DETACH");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_UNP_DETACH");
	}
	KTR_STOP1(KTR_VOP, "VOP", "VOP_UNP_DETACH", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	return (rc);
}

struct vnodeop_desc vop_unp_detach_desc = {
	"vop_unp_detach",
	0,
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
	while(vop != NULL && \
	    vop->vop_is_text == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_is_text(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_is_text, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_IS_TEXT");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_IS_TEXT");
	KTR_START1(KTR_VOP, "VOP", "VOP_IS_TEXT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_is_text != NULL)
		rc = vop->vop_is_text(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_is_text, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_IS_TEXT");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_IS_TEXT");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_IS_TEXT");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_IS_TEXT");
	}
	KTR_STOP1(KTR_VOP, "VOP", "VOP_IS_TEXT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	return (rc);
}

struct vnodeop_desc vop_is_text_desc = {
	"vop_is_text",
	0,
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
	while(vop != NULL && \
	    vop->vop_set_text == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_set_text(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_set_text, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SET_TEXT");
	KTR_START1(KTR_VOP, "VOP", "VOP_SET_TEXT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_set_text != NULL)
		rc = vop->vop_set_text(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_set_text, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SET_TEXT");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SET_TEXT");
	}
	KTR_STOP1(KTR_VOP, "VOP", "VOP_SET_TEXT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	return (rc);
}

struct vnodeop_desc vop_set_text_desc = {
	"vop_set_text",
	0,
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
	while(vop != NULL && \
	    vop->vop_unset_text == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_unset_text(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_unset_text, entry, a->a_vp, a);

	KTR_START1(KTR_VOP, "VOP", "VOP_UNSET_TEXT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_unset_text != NULL)
		rc = vop->vop_unset_text(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_unset_text, return, a->a_vp, a, rc);

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
	(vop_bypass_t *)VOP_UNSET_TEXT_AP,
	vop_unset_text_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_get_writecount_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_get_writecount_args,a_vp),
	VDESC_NO_OFFSET
};


SDT_PROBE_DEFINE2(vfs, vop, vop_get_writecount, entry, "struct vnode *", "struct vop_get_writecount_args *");

SDT_PROBE_DEFINE3(vfs, vop, vop_get_writecount, return, "struct vnode *", "struct vop_get_writecount_args *", "int");


int
VOP_GET_WRITECOUNT_AP(struct vop_get_writecount_args *a)
{

	return(VOP_GET_WRITECOUNT_APV(a->a_vp->v_op, a));
}

int
VOP_GET_WRITECOUNT_APV(struct vop_vector *vop, struct vop_get_writecount_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_get_writecount_desc, a->a_vp,
	    ("Wrong a_desc in vop_get_writecount(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_get_writecount == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_get_writecount(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_get_writecount, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GET_WRITECOUNT");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_GET_WRITECOUNT");
	KTR_START2(KTR_VOP, "VOP", "VOP_GET_WRITECOUNT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "writecount:0x%jX", a->a_writecount);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_get_writecount != NULL)
		rc = vop->vop_get_writecount(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_get_writecount, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GET_WRITECOUNT");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GET_WRITECOUNT");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GET_WRITECOUNT");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GET_WRITECOUNT");
	}
	KTR_STOP2(KTR_VOP, "VOP", "VOP_GET_WRITECOUNT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "writecount:0x%jX", a->a_writecount);
	return (rc);
}

struct vnodeop_desc vop_get_writecount_desc = {
	"vop_get_writecount",
	0,
	(vop_bypass_t *)VOP_GET_WRITECOUNT_AP,
	vop_get_writecount_vp_offsets,
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
	while(vop != NULL && \
	    vop->vop_add_writecount == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_add_writecount(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_add_writecount, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADD_WRITECOUNT");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_ADD_WRITECOUNT");
	KTR_START2(KTR_VOP, "VOP", "VOP_ADD_WRITECOUNT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "inc:0x%jX", a->a_inc);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_add_writecount != NULL)
		rc = vop->vop_add_writecount(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_add_writecount, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADD_WRITECOUNT");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_ADD_WRITECOUNT");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADD_WRITECOUNT");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_ADD_WRITECOUNT");
	}
	KTR_STOP2(KTR_VOP, "VOP", "VOP_ADD_WRITECOUNT", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "inc:0x%jX", a->a_inc);
	return (rc);
}

struct vnodeop_desc vop_add_writecount_desc = {
	"vop_add_writecount",
	0,
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
	while(vop != NULL && \
	    vop->vop_fdatasync == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_fdatasync(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_fdatasync, entry, a->a_vp, a);

	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_FDATASYNC");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_FDATASYNC");
	KTR_START2(KTR_VOP, "VOP", "VOP_FDATASYNC", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "td:0x%jX", a->a_td);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_fdatasync != NULL)
		rc = vop->vop_fdatasync(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_fdatasync, return, a->a_vp, a, rc);

	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_FDATASYNC");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_FDATASYNC");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_FDATASYNC");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_FDATASYNC");
	}
	KTR_STOP2(KTR_VOP, "VOP", "VOP_FDATASYNC", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp, "td:0x%jX", a->a_td);
	return (rc);
}

struct vnodeop_desc vop_fdatasync_desc = {
	"vop_fdatasync",
	0,
	(vop_bypass_t *)VOP_FDATASYNC_AP,
	vop_fdatasync_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_fdatasync_args,a_td),
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
	while(vop != NULL && \
	    vop->vop_spare1 == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_spare1(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_spare1, entry, a->a_vp, a);

	KTR_START1(KTR_VOP, "VOP", "VOP_SPARE1", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_spare1 != NULL)
		rc = vop->vop_spare1(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_spare1, return, a->a_vp, a, rc);

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
	while(vop != NULL && \
	    vop->vop_spare2 == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_spare2(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_spare2, entry, a->a_vp, a);

	KTR_START1(KTR_VOP, "VOP", "VOP_SPARE2", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_spare2 != NULL)
		rc = vop->vop_spare2(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_spare2, return, a->a_vp, a, rc);

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
	while(vop != NULL && \
	    vop->vop_spare3 == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_spare3(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_spare3, entry, a->a_vp, a);

	KTR_START1(KTR_VOP, "VOP", "VOP_SPARE3", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_spare3 != NULL)
		rc = vop->vop_spare3(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_spare3, return, a->a_vp, a, rc);

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
	while(vop != NULL && \
	    vop->vop_spare4 == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_spare4(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_spare4, entry, a->a_vp, a);

	KTR_START1(KTR_VOP, "VOP", "VOP_SPARE4", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_spare4 != NULL)
		rc = vop->vop_spare4(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_spare4, return, a->a_vp, a, rc);

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
	while(vop != NULL && \
	    vop->vop_spare5 == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_spare5(%p, %p)", a->a_vp, a));
	SDT_PROBE2(vfs, vop, vop_spare5, entry, a->a_vp, a);

	KTR_START1(KTR_VOP, "VOP", "VOP_SPARE5", (uintptr_t)a,
	    "vp:0x%jX", (uintptr_t)a->a_vp);
	VFS_PROLOGUE(a->a_vp->v_mount);
	if (vop->vop_spare5 != NULL)
		rc = vop->vop_spare5(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	VFS_EPILOGUE(a->a_vp->v_mount);
	SDT_PROBE3(vfs, vop, vop_spare5, return, a->a_vp, a, rc);

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
	(vop_bypass_t *)VOP_SPARE5_AP,
	vop_spare5_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};
