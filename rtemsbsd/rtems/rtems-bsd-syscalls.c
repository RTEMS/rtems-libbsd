/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009, 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
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

#include <machine/rtems-bsd-config.h>

#include <rtems/bsd/sys/types.h>
#include <rtems/bsd/sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <rtems/bsd/sys/lock.h>
#include <sys/mutex.h>
#include <sys/malloc.h>
#include <sys/proc.h>
#include <sys/fcntl.h>
#include <sys/protosw.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/uio.h>
#include <machine/pcpu.h>
#include <net/vnet.h>

#include <rtems/libio_.h>
#include <rtems/libio.h>
#include <rtems/seterr.h>

static const rtems_filesystem_file_handlers_r socket_handlers;
extern int killinfo( pid_t pid, int sig, const union sigval *value );

/*
 * Convert an RTEMS file descriptor to a BSD socket pointer.
 */

struct socket *rtems_bsdnet_fdToSocket(
  int fd
)
{
  rtems_libio_t *iop;

  /* same as rtems_libio_check_fd(_fd) but different return */
  if ((uint32_t)fd >= rtems_libio_number_iops) {
    errno = EBADF;
    return NULL;
  }
  iop = &rtems_libio_iops[fd];

  /* same as rtems_libio_check_is_open(iop) but different return */
  if ((iop->flags & LIBIO_FLAGS_OPEN) == 0) {
    errno = EBADF;
    return NULL;
  }

  if (iop->data1 == NULL)
    errno = EBADF;
  return iop->data1;
}

/*
 * Create an RTEMS file descriptor for a socket
 */
static int rtems_bsdnet_makeFdForSocket (void *so)
{
  rtems_libio_t *iop;
  int fd;

  iop = rtems_libio_allocate();
  if (iop == 0)
      rtems_set_errno_and_return_minus_one( ENFILE );

  fd = iop - rtems_libio_iops;
  iop->flags |= LIBIO_FLAGS_WRITE | LIBIO_FLAGS_READ;
  iop->data0 = fd;
  iop->data1 = so;
  iop->pathinfo.handlers = &socket_handlers;
  iop->pathinfo.mt_entry = &rtems_filesystem_null_mt_entry;
  rtems_filesystem_location_add_to_mt_entry(&iop->pathinfo);
  return fd;
}

/*
 * The following code is based on FreeBSD uipc_syscalls.c
 */

int
sockargs(mp, buf, buflen, type)
	struct mbuf **mp;
	caddr_t buf;
	int buflen, type;
{
	struct sockaddr *sa;
	struct mbuf *m;
	int error;

	if ((u_int)buflen > MLEN) {
#ifdef COMPAT_OLDSOCK
		if (type == MT_SONAME && (u_int)buflen <= 112)
			buflen = MLEN;		/* unix domain compat. hack */
		else
#endif
			if ((u_int)buflen > MCLBYTES)
				return (EINVAL);
	}
	m = m_get(M_WAIT, type);
	if ((u_int)buflen > MLEN)
		MCLGET(m, M_WAIT);
	m->m_len = buflen;
	error = copyin(buf, mtod(m, caddr_t), (u_int)buflen);
	if (error)
		(void) m_free(m);
	else {
		*mp = m;
		if (type == MT_SONAME) {
			sa = mtod(m, struct sockaddr *);

#if defined(COMPAT_OLDSOCK) && BYTE_ORDER != BIG_ENDIAN
			if (sa->sa_family == 0 && sa->sa_len < AF_MAX)
				sa->sa_family = sa->sa_len;
#endif
			sa->sa_len = buflen;
		}
	}
	return (error);
}

int
getsockaddr(namp, uaddr, len)
	struct sockaddr **namp;
	caddr_t uaddr;
	size_t len;
{
	struct sockaddr *sa;
	int error;

	if (len > SOCK_MAXADDRLEN)
		return (ENAMETOOLONG);
	if (len < offsetof(struct sockaddr, sa_data[0]))
		return (EINVAL);
	sa = malloc(len, M_SONAME, M_WAITOK);
	error = copyin(uaddr, sa, len);
	if (error) {
		free(sa, M_SONAME);
	} else {
#if defined(COMPAT_OLDSOCK) && BYTE_ORDER != BIG_ENDIAN
		if (sa->sa_family == 0 && sa->sa_len < AF_MAX)
			sa->sa_family = sa->sa_len;
#endif
		sa->sa_len = len;
		*namp = sa;
	}
	return (error);
}

/*
 *********************************************************************
 *                       BSD-style entry points                      *
 *********************************************************************
 */
int
socket (int domain, int type, int protocol)
{
	struct thread *td;
	struct socket *so;
	int fd, error;

	td = curthread;
        if (!td) { 
          printf("Current thread NULL\n");
          exit(0);
        }
#ifdef MAC
	error = mac_socket_check_create(td->td_ucred, domain, type, protocol);
	if (error == 0 )
	{
#endif
		/* An extra reference on `fp' has been held for us by falloc(). */
		error = socreate(domain, &so, type, protocol, td->td_ucred, td);
		if (error == 0) {
			fd = rtems_bsdnet_makeFdForSocket (so);
			if (fd < 0)
			{
				soclose (so);
				error = EBADF;
			}
		}
#ifdef MAC
	}
#endif
	if( error == 0 )
	{
		return fd;
	}
	errno = error;
	return -1;
}

int
kern_bind(td, fd, sa)
	struct thread *td;
	int fd;
	struct sockaddr *sa;
{
	struct socket *so;
	int error;

	if ((so = rtems_bsdnet_fdToSocket (fd)) == NULL) {
		error = EBADF;
		return (error);
	}
#ifdef KTRACE
	if (KTRPOINT(td, KTR_STRUCT))
		ktrsockaddr(sa);
#endif
#ifdef MAC
	error = mac_socket_check_bind(td->td_ucred, so, sa);
	if (error == 0)
#endif
		error = sobind(so, sa, td);
	return (error);
}

int
bind (int s, struct sockaddr *name, int namelen)
{
	struct thread *td;
	struct sockaddr *sa;
	int error;

	error = getsockaddr(&sa, name, namelen);
	if( error == 0 )
	{
		td = curthread;
		error = kern_bind(td, s, sa);
		free(sa, M_SONAME);
	}
	if( error == 0 )
	{
		return error;
	}
	errno = error;
	return -1;
}

int
kern_connect(td, fd, sa)
	struct thread *td;
	int fd;
	struct sockaddr *sa;
{
	struct socket *so;
	int error;
	int interrupted = 0;

	if ((so = rtems_bsdnet_fdToSocket (fd)) == NULL) {
		error = EBADF;
		return (error);
	}

	if (so->so_state & SS_ISCONNECTING) {
		error = EALREADY;
		goto done1;
	}
#ifdef KTRACE
	if (KTRPOINT(td, KTR_STRUCT))
		ktrsockaddr(sa);
#endif
#ifdef MAC
	error = mac_socket_check_connect(td->td_ucred, so, sa);
	if (error)
		goto bad;
#endif
	error = soconnect(so, sa, td);
	if (error)
		goto bad;
	if ((so->so_state & SS_NBIO) && (so->so_state & SS_ISCONNECTING)) {
		error = EINPROGRESS;
		goto done1;
	}
	SOCK_LOCK(so);
	while ((so->so_state & SS_ISCONNECTING) && so->so_error == 0) {
		error = msleep(&so->so_timeo, SOCK_MTX(so), PSOCK | PCATCH,
		    "connec", 0);
		if (error) {
			if (error == EINTR || error == ERESTART)
				interrupted = 1;
			break;
		}
	}
	if (error == 0) {
		error = so->so_error;
		so->so_error = 0;
	}
	SOCK_UNLOCK(so);
bad:
	if (!interrupted)
		so->so_state &= ~SS_ISCONNECTING;
	if (error == ERESTART)
		error = EINTR;
done1:
	return (error);
}

int
connect (int s, struct sockaddr *name, int namelen)
{
	int error;
	struct sockaddr *sa;
	struct thread *td;

	error = getsockaddr(&sa, name, namelen);
	if (error == 0)
	{
		td = curthread;
		error = kern_connect(td, s, sa);
		free(sa, M_SONAME);
	}
	if( error == 0 )
	{
		return error;
	}
	errno = error;
	return -1;
}

int
listen (int s, int backlog)
{
	struct thread *td;
	struct socket *so;
	int error = 0;

	if ((so = rtems_bsdnet_fdToSocket (s)) == NULL) {
		error = EBADF;
	}
	if( error == 0 )
	{
		td = curthread;
#ifdef MAC
		error = mac_socket_check_listen(td->td_ucred, so);
		if (error == 0) {
#endif
			CURVNET_SET(so->so_vnet);
			error = solisten(so, backlog, td);
			CURVNET_RESTORE();
#ifdef MAC
		}
#endif
	}
	if( error == 0 )
	{
		return error;
	}
	errno = error;
	return -1;
}

int
kern_accept(struct thread *td, int s, struct sockaddr **name, socklen_t *namelen)
{
	struct sockaddr *sa = NULL;
	int error;
	struct socket *head, *so;
	int fd;
	u_int fflag;
	pid_t pgid;
	int tmp;

	if (name) {
		*name = NULL;
		if (*namelen < 0)
			return (EINVAL);
	}

	if ((head = rtems_bsdnet_fdToSocket (s)) == NULL) {
		error = EBADF;
		return (error);
	}
	if ((head->so_options & SO_ACCEPTCONN) == 0) {
		error = EINVAL;
		goto done;
	}
#ifdef MAC
	error = mac_socket_check_accept(td->td_ucred, head);
	if (error != 0)
		goto done;
#endif
	ACCEPT_LOCK();
	if ((head->so_state & SS_NBIO) && TAILQ_EMPTY(&head->so_comp)) {
		ACCEPT_UNLOCK();
		error = EWOULDBLOCK;
		goto noconnection;
	}
	while (TAILQ_EMPTY(&head->so_comp) && head->so_error == 0) {
		if (head->so_rcv.sb_state & SBS_CANTRCVMORE) {
			head->so_error = ECONNABORTED;
			break;
		}
		error = msleep(&head->so_timeo, &accept_mtx, PSOCK | PCATCH,
		    "accept", 0);
		if (error) {
			ACCEPT_UNLOCK();
			goto noconnection;
		}
	}
	if (head->so_error) {
		error = head->so_error;
		head->so_error = 0;
		ACCEPT_UNLOCK();
		goto noconnection;
	}
	so = TAILQ_FIRST(&head->so_comp);
	KASSERT(!(so->so_qstate & SQ_INCOMP), ("accept1: so SQ_INCOMP"));
	KASSERT(so->so_qstate & SQ_COMP, ("accept1: so not SQ_COMP"));

	/*
	 * Before changing the flags on the socket, we have to bump the
	 * reference count.  Otherwise, if the protocol calls sofree(),
	 * the socket will be released due to a zero refcount.
	 */
	SOCK_LOCK(so);			/* soref() and so_state update */
	soref(so);			/* file descriptor reference */

	TAILQ_REMOVE(&head->so_comp, so, so_list);
	head->so_qlen--;

	fd = rtems_bsdnet_makeFdForSocket (so);
	if (fd < 0) {
		TAILQ_INSERT_HEAD(&head->so_comp, so, so_list);
		head->so_qlen++;
    wakeup(head);
		error = EBADF;
		return (error);
	}

	so->so_state |= (head->so_state & SS_NBIO);
	so->so_qstate &= ~SQ_COMP;
	so->so_head = NULL;

	SOCK_UNLOCK(so);
	ACCEPT_UNLOCK();

	td->td_retval[0] = fd;

	sa = 0;
	CURVNET_SET(so->so_vnet);
	error = soaccept(so, &sa);
	CURVNET_RESTORE();
	if (error) {
		/*
		 * return a namelen of zero for older code which might
		 * ignore the return value from accept.
		 */
		if (name)
			*namelen = 0;
		goto noconnection;
	}
	if (sa == NULL) {
		if (name)
			*namelen = 0;
		goto done;
	}
	if (name) {
		/* check sa_len before it is destroyed */
		if (*namelen > sa->sa_len)
			*namelen = sa->sa_len;
#ifdef KTRACE
		if (KTRPOINT(td, KTR_STRUCT))
			ktrsockaddr(sa);
#endif
		*name = sa;
		sa = NULL;
	}
noconnection:
	if (sa)
		free(sa, M_SONAME);

done:
	return (error);
}

static int
accept1(td, s, _name, _namelen, compat)
	struct thread *td;
	int s;
	struct sockaddr *_name;
	int *_namelen;
	int compat;
{
	struct sockaddr *name;
	socklen_t namelen;
	int error;

	if (_name == NULL)
		return (kern_accept(td, s, NULL, NULL));

	error = copyin(_namelen, &namelen, sizeof (namelen));
	if (error)
		return (error);

	error = kern_accept(td, s, &name, &namelen);

	/*
	 * return a namelen of zero for older code which might
	 * ignore the return value from accept.
	 */
	if (error) {
		(void) copyout(&namelen,
		    _namelen, sizeof(*_namelen));
		return (error);
	}

	if (error == 0 && name != NULL) {
#ifdef COMPAT_OLDSOCK
		if (compat)
			((struct osockaddr *)name)->sa_family =
			    name->sa_family;
#endif
		error = copyout(name, _name, namelen);
	}
	if (error == 0)
		error = copyout(&namelen, _namelen,
		    sizeof(namelen));
	free(name, M_SONAME);
	return (error);
}

int
accept (int s, struct sockaddr *name, int *namelen)
{
	struct thread *td;
	int error;

	td = curthread;
	error = accept1(td, s, name, namelen, 0);
	if( error == 0 )
	{
		return td->td_retval[0];
	}
	errno = error;
	return -1;
}

/*
 *  Shutdown routine
 */

int
shutdown (int s, int how)
{
  struct socket *so;
	int error = 0;

	if ((so = rtems_bsdnet_fdToSocket (s)) == NULL) {
		error = EBADF;
	}
	if( error == 0 )
	{
		error = soshutdown(so, how);
	}
	if( error == 0 )
	{
		return error;
	}
	errno = error;
	return -1;
}

int
kern_sendit(td, s, mp, flags, control, segflg)
	struct thread *td;
	int s;
	struct msghdr *mp;
	int flags;
	struct mbuf *control;
	enum uio_seg segflg;
{
	struct uio auio;
	struct iovec *iov;
	struct socket *so;
	int i;
	int len, error;
#ifdef KTRACE
	struct uio *ktruio = NULL;
#endif

	if ((so = rtems_bsdnet_fdToSocket (s)) == NULL) {
		error = EBADF;
		return (error);
	}

#ifdef MAC
	if (mp->msg_name != NULL) {
		error = mac_socket_check_connect(td->td_ucred, so,
		    mp->msg_name);
		if (error)
			goto bad;
	}
	error = mac_socket_check_send(td->td_ucred, so);
	if (error)
		goto bad;
#endif

	auio.uio_iov = mp->msg_iov;
	auio.uio_iovcnt = mp->msg_iovlen;
	auio.uio_segflg = segflg;
	auio.uio_rw = UIO_WRITE;
	auio.uio_td = td;
	auio.uio_offset = 0;			/* XXX */
	auio.uio_resid = 0;
	iov = mp->msg_iov;
	for (i = 0; i < mp->msg_iovlen; i++, iov++) {
		if ((auio.uio_resid += iov->iov_len) < 0) {
			error = EINVAL;
			goto bad;
		}
	}
#ifdef KTRACE
	if (KTRPOINT(td, KTR_GENIO))
		ktruio = cloneuio(&auio);
#endif
	len = auio.uio_resid;
	error = sosend(so, mp->msg_name, &auio, 0, control, flags, td);
	if (error) {
		if (auio.uio_resid != len && (error == ERESTART ||
		    error == EINTR || error == EWOULDBLOCK))
			error = 0;
		/* Generation of SIGPIPE can be controlled per socket */
		if (error == EPIPE && !(so->so_options & SO_NOSIGPIPE) &&
		    !(flags & MSG_NOSIGNAL)) {
			killinfo(td->td_proc->p_pid, SIGPIPE, NULL);
		}
	}
	if (error == 0)
		td->td_retval[0] = len - auio.uio_resid;
#ifdef KTRACE
	if (ktruio != NULL) {
		ktruio->uio_resid = td->td_retval[0];
		ktrgenio(s, UIO_WRITE, ktruio, error);
	}
#endif
bad:
	return (error);
}

static int
sendit(td, s, mp, flags)
	struct thread *td;
	int s;
	struct msghdr *mp;
	int flags;
{
	struct mbuf *control;
	struct sockaddr *to;
	int error;

	if (mp->msg_name != NULL) {
		error = getsockaddr(&to, mp->msg_name, mp->msg_namelen);
		if (error) {
			to = NULL;
			goto bad;
		}
		mp->msg_name = to;
	} else {
		to = NULL;
	}

	if (mp->msg_control) {
		if (mp->msg_controllen < sizeof(struct cmsghdr)
#ifdef COMPAT_OLDSOCK
		    && mp->msg_flags != MSG_COMPAT
#endif
		) {
			error = EINVAL;
			goto bad;
		}
		error = sockargs(&control, mp->msg_control,
		    mp->msg_controllen, MT_CONTROL);
		if (error)
			goto bad;
#ifdef COMPAT_OLDSOCK
		if (mp->msg_flags == MSG_COMPAT) {
			struct cmsghdr *cm;

			M_PREPEND(control, sizeof(*cm), M_WAIT);
			cm = mtod(control, struct cmsghdr *);
			cm->cmsg_len = control->m_len;
			cm->cmsg_level = SOL_SOCKET;
			cm->cmsg_type = SCM_RIGHTS;
		}
#endif
	} else {
		control = NULL;
	}

	error = kern_sendit(td, s, mp, flags, control, UIO_USERSPACE);

bad:
	if (to)
		free(to, M_SONAME);
	return (error);
}

/*
 * All `transmit' operations end up calling this routine.
 */
ssize_t
sendmsg (int s, const struct msghdr *mp, int flags)
{
	struct thread *td;
	struct msghdr msg;
	struct iovec *iov;
	int error;

	td = curthread;
	error = copyin(mp, &msg, sizeof (msg));
	if (error)
		return (error);
	error = copyiniov(msg.msg_iov, msg.msg_iovlen, &iov, EMSGSIZE);
	if (error)
		return (error);
	msg.msg_iov = iov;
#ifdef COMPAT_OLDSOCK
	msg.msg_flags = 0;
#endif
	error = sendit(td, s, &msg, flags);
	free(iov, M_IOV);
	if( error == 0 )
	{
		return td->td_retval[0];
	}
	errno = error;
	return -1;
}

/*
 * Send a message to a host
 */
ssize_t
sendto (int s, const void *buf, size_t len, int flags, const struct sockaddr *to, int tolen)
{
	struct thread *td;
	struct msghdr msg;
	struct iovec aiov;
	int error;

	td = curthread;
	msg.msg_name = to;
	msg.msg_namelen = tolen;
	msg.msg_iov = &aiov;
	msg.msg_iovlen = 1;
	msg.msg_control = 0;
#ifdef COMPAT_OLDSOCK
	msg.msg_flags = 0;
#endif
	aiov.iov_base = buf;
	aiov.iov_len = len;
	error = sendit(td, s, &msg, flags);
	if( error == 0 )
	{
		return td->td_retval[0];
	}
	errno = error;
	return -1;
}

ssize_t
send( int s, const void *msg, size_t len, int flags )
{
  return (sendto(s, msg, len, flags, NULL, 0));
}

int
kern_recvit(td, s, mp, fromseg, controlp)
	struct thread *td;
	int s;
	struct msghdr *mp;
	enum uio_seg fromseg;
	struct mbuf **controlp;
{
	struct uio auio;
	struct iovec *iov;
	int i;
	socklen_t len;
	int error;
	struct mbuf *m, *control = 0;
	caddr_t ctlbuf;
	struct socket *so;
	struct sockaddr *fromsa = 0;
#ifdef KTRACE
	struct uio *ktruio = NULL;
#endif

	if(controlp != NULL)
		*controlp = 0;

	if ((so = rtems_bsdnet_fdToSocket (s)) == NULL) {
		error = EBADF;
		return (error);
	}

#ifdef MAC
	error = mac_socket_check_receive(td->td_ucred, so);
	if (error) {
		return (error);
	}
#endif

	auio.uio_iov = mp->msg_iov;
	auio.uio_iovcnt = mp->msg_iovlen;
	auio.uio_segflg = UIO_USERSPACE;
	auio.uio_rw = UIO_READ;
	auio.uio_td = td;
	auio.uio_offset = 0;			/* XXX */
	auio.uio_resid = 0;
	iov = mp->msg_iov;
	for (i = 0; i < mp->msg_iovlen; i++, iov++) {
		if ((auio.uio_resid += iov->iov_len) < 0) {
			return (EINVAL);
		}
	}
#ifdef KTRACE
	if (KTRPOINT(td, KTR_GENIO))
		ktruio = cloneuio(&auio);
#endif
	len = auio.uio_resid;
	CURVNET_SET(so->so_vnet);
	error = soreceive(so, &fromsa, &auio, (struct mbuf **)0,
	    (mp->msg_control || controlp) ? &control : (struct mbuf **)0,
	    &mp->msg_flags);
	CURVNET_RESTORE();
	if (error) {
		if (auio.uio_resid != (int)len && (error == ERESTART ||
		    error == EINTR || error == EWOULDBLOCK))
			error = 0;
	}
#ifdef KTRACE
	if (ktruio != NULL) {
		ktruio->uio_resid = (int)len - auio.uio_resid;
		ktrgenio(s, UIO_READ, ktruio, error);
	}
#endif
	if (error)
		goto out;
	td->td_retval[0] = (int)len - auio.uio_resid;
	if (mp->msg_name) {
		len = mp->msg_namelen;
		if (len <= 0 || fromsa == 0)
			len = 0;
		else {
			/* save sa_len before it is destroyed by MSG_COMPAT */
			len = MIN(len, fromsa->sa_len);
#ifdef COMPAT_OLDSOCK
			if (mp->msg_flags & MSG_COMPAT)
				((struct osockaddr *)fromsa)->sa_family =
				    fromsa->sa_family;
#endif
			if (fromseg == UIO_USERSPACE) {
				error = copyout(fromsa, mp->msg_name,
				    (unsigned)len);
				if (error)
					goto out;
			} else
				bcopy(fromsa, mp->msg_name, len);
		}
		mp->msg_namelen = len;
	}
	if (mp->msg_control && controlp == NULL) {
#ifdef COMPAT_OLDSOCK
		/*
		 * We assume that old recvmsg calls won't receive access
		 * rights and other control info, esp. as control info
		 * is always optional and those options didn't exist in 4.3.
		 * If we receive rights, trim the cmsghdr; anything else
		 * is tossed.
		 */
		if (control && mp->msg_flags & MSG_COMPAT) {
			if (mtod(control, struct cmsghdr *)->cmsg_level !=
			    SOL_SOCKET ||
			    mtod(control, struct cmsghdr *)->cmsg_type !=
			    SCM_RIGHTS) {
				mp->msg_controllen = 0;
				goto out;
			}
			control->m_len -= sizeof (struct cmsghdr);
			control->m_data += sizeof (struct cmsghdr);
		}
#endif
		len = mp->msg_controllen;
		m = control;
		mp->msg_controllen = 0;
		ctlbuf = mp->msg_control;

		while (m && len > 0) {
			unsigned int tocopy;

			if (len >= m->m_len)
				tocopy = m->m_len;
			else {
				mp->msg_flags |= MSG_CTRUNC;
				tocopy = len;
			}

			if ((error = copyout(mtod(m, caddr_t),
					ctlbuf, tocopy)) != 0)
				goto out;

			ctlbuf += tocopy;
			len -= tocopy;
			m = m->m_next;
		}
		mp->msg_controllen = ctlbuf - (caddr_t)mp->msg_control;
	}
out:
#ifdef KTRACE
	if (fromsa && KTRPOINT(td, KTR_STRUCT))
		ktrsockaddr(fromsa);
#endif
	if (fromsa)
		free(fromsa, M_SONAME);

	if (error == 0 && controlp != NULL)
		*controlp = control;
	else  if (control)
		m_freem(control);

	return (error);
}

static int
recvit(td, s, mp, namelenp)
	struct thread *td;
	int s;
	struct msghdr *mp;
	void *namelenp;
{
	int error;

	error = kern_recvit(td, s, mp, UIO_USERSPACE, NULL);
	if (error)
		return (error);
	if (namelenp) {
		error = copyout(&mp->msg_namelen, namelenp, sizeof (socklen_t));
#ifdef COMPAT_OLDSOCK
		if (mp->msg_flags & MSG_COMPAT)
			error = 0;	/* old recvfrom didn't check */
#endif
	}
	return (error);
}

/*
 * All `receive' operations end up calling this routine.
 */
ssize_t
recvmsg (int s, struct msghdr *mp, int flags)
{
	struct thread *td;
	struct msghdr msg;
	struct iovec *uiov, *iov;
	int error;

	td = curthread;
	error = copyin(mp, &msg, sizeof (msg));
	if (error == 0 )
	{
		error = copyiniov(msg.msg_iov, msg.msg_iovlen, &iov, EMSGSIZE);
		if (error == 0)
		{
			msg.msg_flags = flags;
	#ifdef COMPAT_OLDSOCK
			msg.msg_flags &= ~MSG_COMPAT;
	#endif
			uiov = msg.msg_iov;
			msg.msg_iov = iov;
			error = recvit(td, s, &msg, NULL);
			if (error == 0) {
				msg.msg_iov = uiov;
				error = copyout(&msg, mp, sizeof(msg));
			}
			free(iov, M_IOV);
		}
	}
	if( error == 0 )
	{
		return td->td_retval[0];
	}
	errno = error;
	return -1;
}

/*
 * Receive a message from a host
 */
ssize_t
recvfrom (int s, void *buf, size_t len, int flags, const struct sockaddr *from, socklen_t *fromlenaddr)
{
	struct thread *td;
	struct msghdr msg;
	struct iovec aiov;
	int error;

	td = curthread;
	if (fromlenaddr) {
		error = copyin(fromlenaddr,
		    &msg.msg_namelen, sizeof (msg.msg_namelen));
		if (error)
			goto done2;
	} else {
		msg.msg_namelen = 0;
	}
	msg.msg_name = from;
	msg.msg_iov = &aiov;
	msg.msg_iovlen = 1;
	aiov.iov_base = buf;
	aiov.iov_len = len;
	msg.msg_control = 0;
	msg.msg_flags = flags;
	error = recvit(td, s, &msg, fromlenaddr);
done2:
	if( error == 0 )
	{
		return td->td_retval[0];
	}
	errno = error;
	return -1;
}

ssize_t
recv( int s, void *buf, size_t len, int flags )
{
  return (recvfrom(s, buf, len, flags, NULL, 0));
}

int
kern_setsockopt(td, s, level, name, val, valseg, valsize)
	struct thread *td;
	int s;
	int level;
	int name;
	void *val;
	enum uio_seg valseg;
	socklen_t valsize;
{
	int error;
	struct socket *so;
	struct sockopt sopt;

	if (val == NULL && valsize != 0)
		return (EFAULT);
	if ((int)valsize < 0)
		return (EINVAL);

	sopt.sopt_dir = SOPT_SET;
	sopt.sopt_level = level;
	sopt.sopt_name = name;
	sopt.sopt_val = val;
	sopt.sopt_valsize = valsize;
	switch (valseg) {
	case UIO_USERSPACE:
		sopt.sopt_td = td;
		break;
	case UIO_SYSSPACE:
		sopt.sopt_td = NULL;
		break;
	default:
		panic("kern_setsockopt called with bad valseg");
	}

	if ((so = rtems_bsdnet_fdToSocket (s)) == NULL) {
		error = EBADF;
		return error;
	}
	CURVNET_SET(so->so_vnet);
	error = sosetopt(so, &sopt);
	CURVNET_RESTORE();
	return(error);
}

int
setsockopt (int s, int level, int name, const void *val, socklen_t valsize)
{
	struct thread *td;
	int error;

	td = curthread;
	error = kern_setsockopt(td, s, level, name, val, UIO_USERSPACE, valsize);
	if( error == 0 )
	{
		return error;
	}
	errno = error;
	return -1;
}

int
kern_getsockopt(td, s, level, name, val, valseg, valsize)
	struct thread *td;
	int s;
	int level;
	int name;
	void *val;
	enum uio_seg valseg;
	socklen_t *valsize;
{
	int error;
	struct  socket *so;
	struct	sockopt sopt;

	if (val == NULL)
		*valsize = 0;
	if ((int)*valsize < 0)
		return (EINVAL);

	sopt.sopt_dir = SOPT_GET;
	sopt.sopt_level = level;
	sopt.sopt_name = name;
	sopt.sopt_val = val;
	sopt.sopt_valsize = (size_t)*valsize; /* checked non-negative above */
	switch (valseg) {
	case UIO_USERSPACE:
		sopt.sopt_td = td;
		break;
	case UIO_SYSSPACE:
		sopt.sopt_td = NULL;
		break;
	default:
		panic("kern_getsockopt called with bad valseg");
	}

	if ((so = rtems_bsdnet_fdToSocket (s)) == NULL) {
		error = EBADF;
		return error;
	}
	CURVNET_SET(so->so_vnet);
	error = sogetopt(so, &sopt);
	CURVNET_RESTORE();
	*valsize = sopt.sopt_valsize;
	return (error);
}

int
getsockopt (int s, int level, int name, void *val, socklen_t *avalsize)
{
	struct thread *td;
	socklen_t valsize;
	int	error = 0;

	td = curthread;
	if (val) {
		error = copyin(avalsize, &valsize, sizeof (valsize));
	}

	if( error == 0 )
	{
		error = kern_getsockopt(td, s, level, name, val, UIO_USERSPACE, &valsize);

		if (error == 0)
			error = copyout(&valsize, avalsize, sizeof (valsize));
	}
	if( error == 0 )
	{
		return error;
	}
	errno = error;
	return -1;
}

int
kern_getpeername(struct thread *td, int fd, struct sockaddr **sa,
    socklen_t *alen)
{
	struct socket *so;
	socklen_t len;
	int error;

	if (*alen < 0)
		return (EINVAL);

	if ((so = rtems_bsdnet_fdToSocket (fd)) == NULL) {
		error = EBADF;
		return error;
	}
	if ((so->so_state & (SS_ISCONNECTED|SS_ISCONFIRMING)) == 0) {
		error = ENOTCONN;
		goto done;
	}
	*sa = NULL;
	CURVNET_SET(so->so_vnet);
	error = (*so->so_proto->pr_usrreqs->pru_peeraddr)(so, sa);
	CURVNET_RESTORE();
	if (error)
		goto bad;
	if (*sa == NULL)
		len = 0;
	else
		len = MIN(*alen, (*sa)->sa_len);
	*alen = len;
#ifdef KTRACE
	if (KTRPOINT(td, KTR_STRUCT))
		ktrsockaddr(*sa);
#endif
bad:
	if (error && *sa) {
		free(*sa, M_SONAME);
		*sa = NULL;
	}
done:
	return (error);
}

static int
getpeername1(td, fdes, asa, alen, compat)
	struct thread *td;
	int	fdes;
	struct sockaddr * asa;
	socklen_t * alen;
	int compat;
{
	struct sockaddr *sa;
	socklen_t len;
	int error;

	error = copyin(alen, &len, sizeof (len));
	if (error)
		return (error);

	error = kern_getpeername(td, fdes, &sa, &len);
	if (error)
		return (error);

	if (len != 0) {
#ifdef COMPAT_OLDSOCK
		if (compat)
			((struct osockaddr *)sa)->sa_family = sa->sa_family;
#endif
		error = copyout(sa, asa, (u_int)len);
	}
	free(sa, M_SONAME);
	if (error == 0)
		error = copyout(&len, alen, sizeof(len));
	return (error);
}

int
getpeername (int s, struct sockaddr *name, socklen_t *namelen)
{
	struct thread *td;
	int error;

	td = curthread;
	error = getpeername1(td, s, name, namelen, 0);
	if( error == 0 )
	{
		return error;
	}
	errno = error;
	return -1;
}

int
kern_getsockname(struct thread *td, int fd, struct sockaddr **sa,
    socklen_t *alen)
{
	struct socket *so;
	socklen_t len;
	int error;

	if (*alen < 0)
		return (EINVAL);

	if ((so = rtems_bsdnet_fdToSocket (fd)) == NULL) {
		error = EBADF;
		return error;
	}
	*sa = NULL;
	CURVNET_SET(so->so_vnet);
	error = (*so->so_proto->pr_usrreqs->pru_sockaddr)(so, sa);
	CURVNET_RESTORE();
	if (error)
		goto bad;
	if (*sa == NULL)
		len = 0;
	else
		len = MIN(*alen, (*sa)->sa_len);
	*alen = len;
#ifdef KTRACE
	if (KTRPOINT(td, KTR_STRUCT))
		ktrsockaddr(*sa);
#endif
bad:
	if (error && *sa) {
		free(*sa, M_SONAME);
		*sa = NULL;
	}
	return (error);
}

static int
getsockname1(td, fdes, asa, alen, compat)
	struct thread *td;
	int	fdes;
	struct sockaddr *  asa;
	socklen_t * alen;
	int compat;
{
	struct sockaddr *sa;
	socklen_t len;
	int error;

	error = copyin(alen, &len, sizeof(len));
	if (error)
		return (error);

	error = kern_getsockname(td, fdes, &sa, &len);
	if (error)
		return (error);

	if (len != 0) {
#ifdef COMPAT_OLDSOCK
		if (compat)
			((struct osockaddr *)sa)->sa_family = sa->sa_family;
#endif
		error = copyout(sa, asa, (u_int)len);
	}
	free(sa, M_SONAME);
	if (error == 0)
		error = copyout(&len, alen, sizeof(len));
	return (error);
}

int
getsockname (int s, struct sockaddr *name, socklen_t *namelen)
{
	struct thread *td;
	int error;

	td = curthread;
	error = getsockname1(td, s, name, namelen, 0);
	if( error == 0 )
	{
		return error;
	}
	errno = error;
	return -1;
}

/*
 ************************************************************************
 *                      RTEMS I/O HANDLER ROUTINES                      *
 ************************************************************************
 */
static int
rtems_bsdnet_close (rtems_libio_t *iop)
{
	struct socket *so;
	int error;

	if ((so = iop->data1) == NULL) {
		errno = EBADF;
		return -1;
	}
	error = soclose (so);
	if (error) {
		errno = error;
		return -1;
	}
	return 0;
}

static ssize_t
rtems_bsdnet_read (rtems_libio_t *iop, void *buffer, size_t count)
{
	return recv (iop->data0, buffer, count, 0);
}

static ssize_t
rtems_bsdnet_write (rtems_libio_t *iop, const void *buffer, size_t count)
{
	return send (iop->data0, buffer, count, 0);
}

static int
so_ioctl (rtems_libio_t *iop, struct socket *so, uint32_t   command, void *buffer)
{
	switch (command) {
	case FIONBIO:
		SOCK_LOCK(so);
		if (*(int *)buffer) {
			iop->flags |= O_NONBLOCK;
			so->so_state |= SS_NBIO;
		}
		else {
			iop->flags &= ~O_NONBLOCK;
			so->so_state &= ~SS_NBIO;
		}
		SOCK_UNLOCK(so);
		return 0;

	case FIONREAD:
		*(int *)buffer = so->so_rcv.sb_cc;
		return 0;
	}

	if (IOCGROUP(command) == 'i')
		return ifioctl (so, command, buffer, NULL);
	if (IOCGROUP(command) == 'r')
		return rtioctl (command, buffer, NULL);
	return (*so->so_proto->pr_usrreqs->pru_control)(so, command, buffer, 0, curthread);
}

static int
rtems_bsdnet_ioctl (rtems_libio_t *iop, uint32_t   command, void *buffer)
{
	struct socket *so;
	int error;

	if ((so = iop->data1) == NULL) {
		errno = EBADF;
		return -1;
	}
	error = so_ioctl (iop, so, command, buffer);
	if (error) {
		errno = error;
		return -1;
	}
	return 0;
}

static int
rtems_bsdnet_fcntl (int cmd, rtems_libio_t *iop)
{
	struct socket *so;

	if (cmd == F_SETFL) {
		if ((so = iop->data1) == NULL) {
			return EBADF;
		}
		SOCK_LOCK(so);
		if (iop->flags & LIBIO_FLAGS_NO_DELAY)
			so->so_state |= SS_NBIO;
		else
			so->so_state &= ~SS_NBIO;
		SOCK_UNLOCK(so);
	}
  return 0;
}

static int
rtems_bsdnet_fstat (rtems_filesystem_location_info_t *loc, struct stat *sp)
{
	sp->st_mode = S_IFSOCK;
	return 0;
}

static const rtems_filesystem_file_handlers_r socket_handlers = {
	rtems_filesystem_default_open,		/* open */
	rtems_bsdnet_close,			/* close */
	rtems_bsdnet_read,			/* read */
	rtems_bsdnet_write,			/* write */
	rtems_bsdnet_ioctl,			/* ioctl */
	rtems_filesystem_default_lseek,		/* lseek */
	rtems_bsdnet_fstat,			/* fstat */
	rtems_filesystem_default_fchmod,	/* fchmod */
	rtems_filesystem_default_ftruncate,	/* ftruncate */
	rtems_filesystem_default_fsync_or_fdatasync,		/* fsync */
	rtems_filesystem_default_fsync_or_fdatasync,	/* fdatasync */
	rtems_bsdnet_fcntl,			/* fcntl */
	rtems_filesystem_default_rmnod		/* rmnod */
};
