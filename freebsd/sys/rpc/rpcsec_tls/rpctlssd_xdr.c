#include <machine/rtems-bsd-kernel-space.h>
/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "rpctlssd.h"

bool_t
xdr_rpctlssd_connect_res(XDR *xdrs, rpctlssd_connect_res *objp)
{

	if (!xdr_uint32_t(xdrs, &objp->flags))
		return (FALSE);
	if (!xdr_uint64_t(xdrs, &objp->sec))
		return (FALSE);
	if (!xdr_uint64_t(xdrs, &objp->usec))
		return (FALSE);
	if (!xdr_uint64_t(xdrs, &objp->ssl))
		return (FALSE);
	if (!xdr_uint32_t(xdrs, &objp->uid))
		return (FALSE);
	if (!xdr_array(xdrs, (char **)&objp->gid.gid_val, (u_int *) &objp->gid.gid_len, ~0,
		sizeof (uint32_t), (xdrproc_t) xdr_uint32_t))
		return (FALSE);
	return (TRUE);
}

bool_t
xdr_rpctlssd_handlerecord_arg(XDR *xdrs, rpctlssd_handlerecord_arg *objp)
{

	if (!xdr_uint64_t(xdrs, &objp->sec))
		return (FALSE);
	if (!xdr_uint64_t(xdrs, &objp->usec))
		return (FALSE);
	if (!xdr_uint64_t(xdrs, &objp->ssl))
		return (FALSE);
	return (TRUE);
}

bool_t
xdr_rpctlssd_handlerecord_res(XDR *xdrs, rpctlssd_handlerecord_res *objp)
{

	if (!xdr_uint32_t(xdrs, &objp->reterr))
		return (FALSE);
	return (TRUE);
}

bool_t
xdr_rpctlssd_disconnect_arg(XDR *xdrs, rpctlssd_disconnect_arg *objp)
{

	if (!xdr_uint64_t(xdrs, &objp->sec))
		return (FALSE);
	if (!xdr_uint64_t(xdrs, &objp->usec))
		return (FALSE);
	if (!xdr_uint64_t(xdrs, &objp->ssl))
		return (FALSE);
	return (TRUE);
}

bool_t
xdr_rpctlssd_disconnect_res(XDR *xdrs, rpctlssd_disconnect_res *objp)
{

	if (!xdr_uint32_t(xdrs, &objp->reterr))
		return (FALSE);
	return (TRUE);
}
