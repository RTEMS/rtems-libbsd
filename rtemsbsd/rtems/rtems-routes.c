/*
 * Copyright (c) 2016 Chris Johns <chrisj@rtems.org>.  All rights reserved.
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

/*
 * Useful functions to access the routing tables. Based on unpv13e from
 * Stevens.
 */

#include <stdbool.h>
#include <errno.h>

#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>
#include <sys/socket.h>

/*
 * Round up 'a' to next multiple of 'size', which must be a power of 2
 */
#define RR_ROUNDUP(a, size) (((a) & ((size)-1)) ? (1 + ((a) | ((size)-1))) : (a))

/*
 * Step to next socket address structure;
 * if sa_len is 0, assume it is sizeof(u_long).
 */
#define NEXT_SA(ap)							\
  ap = (struct sockaddr*) ((caddr_t) ap +				\
			   (ap->sa_len ? RR_ROUNDUP(ap->sa_len, sizeof (u_long)) : sizeof(u_long)))

static void
get_rtaddrs(int addrs, struct sockaddr* sa, struct sockaddr** rti_info)
{
  int i;

  for (i = 0; i < RTAX_MAX; i++) {
    if ((addrs & (1 << i)) != 0) {
      rti_info[i] = sa;
      NEXT_SA(sa);
    } else
      rti_info[i] = NULL;
  }
}

/*
 * Get a route.
 */
int rtems_get_route(const struct sockaddr_in* sin, struct sockaddr** rti_info)
{
  int                 s;
  char*               buf;
  const size_t        buflen = sizeof(struct rt_msghdr) + 512;
  pid_t               pid;
  struct rt_msghdr*   rtm;
  struct sockaddr_in* r_sin;
  struct sockaddr*    sa;
  const int           seq = (int) 0x28290817;
  ssize_t             r;

  buf = calloc(1, buflen);
  if (buf == NULL) {
    errno = ENOMEM;
    return -1;
  }

  s = socket(AF_ROUTE, SOCK_RAW, AF_UNSPEC);
  if (s < 0)
    return -1;

  rtm = (struct rt_msghdr *) buf;
  rtm->rtm_msglen = sizeof(struct rt_msghdr) + sizeof(struct sockaddr_in);
  rtm->rtm_version = RTM_VERSION;
  rtm->rtm_type = RTM_GET;
  rtm->rtm_addrs = RTA_DST;
  rtm->rtm_pid = pid = getpid();
  rtm->rtm_seq = seq;

  r_sin = (struct sockaddr_in *) (rtm + 1);
  *r_sin = *sin;

  r = write(s, rtm, rtm->rtm_msglen);
  if (r < 0) {
    int en = errno;
    close(s);
    free(buf);
    errno = en;
    return -1;
  }

  while (true) {
    r = read(s, rtm, buflen);
    if (r < 0) {
      int en = errno;
      close(s);
      free(buf);
      errno = en;
      return -1;
    }

    /*
     * The kernel sends all routing message to all routing sockets. We need to
     * filter them for the one for us.
     */
    if (rtm->rtm_type == RTM_GET &&
	rtm->rtm_seq == seq &&
	rtm->rtm_pid == pid)
      break;
  }

  close(s);

  rtm = (struct rt_msghdr*) buf;
  sa = (struct sockaddr*) (rtm + 1);

  get_rtaddrs(rtm->rtm_addrs, sa, rti_info);

  free(buf);

  return 0;
}
