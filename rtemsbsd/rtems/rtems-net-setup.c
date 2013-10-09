/*
 * XXX copyright and license.
 * XXX integrate more setup from old code in rtems_glue.c
 * XXX integrate using old configuration structure as it makes sense
 */

#include <freebsd/bsd.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <rtems/rtems_bsdnet.h>

static const struct sockaddr_in address_template = {
  sizeof(address_template),
  AF_INET,
  0,
  { INADDR_ANY },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};

/*
 * Manipulate routing tables
 */
int rtems_bsdnet_rtrequest(
  int              req,
  struct sockaddr *dst,
  struct sockaddr *gateway,
  struct sockaddr *netmask,
  int              flags,
  struct rtentry **net_nrt
)
{
  int error;

  // rtems_bsdnet_semaphore_obtain();
  error = rtrequest(req, dst, gateway, netmask, flags, net_nrt);
  // rtems_bsdnet_semaphore_release();
  if (error) {
    errno = error;
    return -1;
  }
  return 0;
}

static void
rtems_bsdnet_initialize_sockaddr_in(struct sockaddr_in *addr)
{
  memcpy(addr, &address_template, sizeof(*addr));
}

/*
 * Interface Configuration.
 */
int rtems_bsdnet_ifconfig(const char *ifname, uint32_t cmd, void *param)
{
  int s, r = 0;
  struct ifreq ifreq;

  /*
   * Configure interfaces
   */
  s = socket(AF_INET, SOCK_DGRAM, 0);
  if (s < 0)
    return -1;

  strncpy(ifreq.ifr_name, ifname, IFNAMSIZ);

  switch (cmd) {
    case SIOCSIFADDR:
    case SIOCSIFNETMASK:
      memcpy(&ifreq.ifr_addr, param, sizeof(struct sockaddr));
      r = ioctl(s, cmd, &ifreq);
      break;

    case OSIOCGIFADDR:
    case SIOCGIFADDR:
    case OSIOCGIFNETMASK:
    case SIOCGIFNETMASK:
      if ((r = ioctl(s, cmd, &ifreq)) < 0)
        break;
      memcpy(param, &ifreq.ifr_addr, sizeof(struct sockaddr));
      break;

    case SIOCGIFFLAGS:
    case SIOCSIFFLAGS:
      if ((r = ioctl(s, SIOCGIFFLAGS, &ifreq)) < 0)
        break;
      if (cmd == SIOCGIFFLAGS) {
        *((short*) param) = ifreq.ifr_flags;
        break;
      }
      ifreq.ifr_flags |= *((short*) param);
      if ( (*((short*) param) & IFF_UP ) == 0 ) {
          /* set the interface down */
          ifreq.ifr_flags &= ~(IFF_UP);
      }
      r = ioctl(s, SIOCSIFFLAGS, &ifreq);
      break;

    case SIOCSIFDSTADDR:
      memcpy(&ifreq.ifr_dstaddr, param, sizeof(struct sockaddr));
      r = ioctl(s, cmd, &ifreq);
      break;

    case OSIOCGIFDSTADDR:
    case SIOCGIFDSTADDR:
      if ((r = ioctl(s, cmd, &ifreq)) < 0)
        break;
      memcpy(param, &ifreq.ifr_dstaddr, sizeof(struct sockaddr));
      break;

    case SIOCSIFBRDADDR:
      memcpy(&ifreq.ifr_broadaddr, param, sizeof(struct sockaddr));
      r = ioctl(s, cmd, &ifreq);
      break;

    case OSIOCGIFBRDADDR:
    case SIOCGIFBRDADDR:
      if ((r = ioctl(s, cmd, &ifreq)) < 0)
        break;
      memcpy(param, &ifreq.ifr_broadaddr, sizeof(struct sockaddr));
      break;

    case SIOCSIFMETRIC:
      ifreq.ifr_metric = *((int*) param);
      r = ioctl(s, cmd, &ifreq);
      break;

    case SIOCGIFMETRIC:
      if ((r = ioctl(s, cmd, &ifreq)) < 0)
        break;
      *((int*) param) = ifreq.ifr_metric;
      break;

    case SIOCSIFMTU:
      ifreq.ifr_mtu = *((int*) param);
      r = ioctl(s, cmd, &ifreq);
      break;

    case SIOCGIFMTU:
      if ((r = ioctl(s, cmd, &ifreq)) < 0)
        break;
      *((int*) param) = ifreq.ifr_mtu;
      break;

    case SIOCSIFPHYS:
      ifreq.ifr_phys = *((int*) param);
      r = ioctl(s, cmd, &ifreq);
      break;

    case SIOCGIFPHYS:
      if ((r = ioctl(s, cmd, &ifreq)) < 0)
        break;
      *((int*) param) = ifreq.ifr_phys;
      break;

    case SIOCSIFMEDIA:
      ifreq.ifr_media = *((int*) param);
      r = ioctl(s, cmd, &ifreq);
      break;

    case SIOCGIFMEDIA:
      /* 'param' passes the phy index they want to
       * look at...
       */
      ifreq.ifr_media = *((int*) param);
      if ((r = ioctl(s, cmd, &ifreq)) < 0)
        break;
      *((int*) param) = ifreq.ifr_media;
      break;

    case SIOCAIFADDR:
    case SIOCDIFADDR:
      r = ioctl(s, cmd, (struct ifreq *) param);
      break;

    default:
      errno = EOPNOTSUPP;
      r = -1;
      break;
  }

  close(s);
  return r;
}



static bool rtems_bsdnet_setup_interface(
  const char *name,
  const char *ip_address,
  const char *ip_netmask
)
{
  struct sockaddr_in address;
  struct sockaddr_in netmask;
  short flags;

  /*
   * Bring interface up
   */
  flags = IFF_UP;
  if (rtems_bsdnet_ifconfig(name, SIOCSIFFLAGS, &flags) < 0) {
    printf("Can't bring %s up: %s\n", name, strerror(errno));
    return false;
  }

  /*
   * Set interface netmask
   */
  rtems_bsdnet_initialize_sockaddr_in(&netmask);
  netmask.sin_addr.s_addr = inet_addr(ip_netmask);
  if (rtems_bsdnet_ifconfig(name, SIOCSIFNETMASK, &netmask) < 0) {
    printf("Can't set %s netmask: %s\n", name, strerror(errno));
    return false;
  }

  /*
   * Set interface address
   */
  rtems_bsdnet_initialize_sockaddr_in(&address);
  address.sin_addr.s_addr = inet_addr(ip_address);
  if (rtems_bsdnet_ifconfig(name, SIOCSIFADDR, &address) < 0) {
    printf("Can't set %s address: %s\n", name, strerror(errno));
    return false;
  }

  /*
   * Set interface broadcast address if the interface has the
   * broadcast flag set.
   */
  if (rtems_bsdnet_ifconfig(name, SIOCGIFFLAGS, &flags) < 0) {
    printf("Can't read %s flags: %s\n", name, strerror(errno));
    return false;
  }

  if (flags & IFF_BROADCAST) {
    struct sockaddr_in broadcast;

    rtems_bsdnet_initialize_sockaddr_in(&broadcast);
    broadcast.sin_addr.s_addr =
        address.sin_addr.s_addr | ~netmask.sin_addr.s_addr;
    if (rtems_bsdnet_ifconfig(name, SIOCSIFBRDADDR, &broadcast) < 0) {
      struct in_addr  in_addr;
      char      buf[20];
      in_addr.s_addr = broadcast.sin_addr.s_addr;
      if (!inet_ntop(AF_INET, &in_addr, buf, sizeof(buf)))
          strcpy(buf,"?.?.?.?");
      printf("Can't set %s broadcast address %s: %s\n",
        name, buf, strerror(errno));
    }
  }

  return true;
}



void rtems_initialize_interfaces(void)
{
  bool                          any_if_configured;
  struct rtems_bsdnet_ifconfig *ifp;

  any_if_configured = false;

  any_if_configured |= rtems_bsdnet_setup_interface(
    "lo0",
    "127.0.0.1",
    "255.0.0.0"
  );


  for (ifp = rtems_bsdnet_config.ifconfig ; ifp ; ifp = ifp->next) {
    if (ifp->ip_address == NULL)
      continue;

    any_if_configured |= rtems_bsdnet_setup_interface(
      ifp->name,
      ifp->ip_address,
      ifp->ip_netmask
    );
  }
  /*
   * Set default route
   */
  if (rtems_bsdnet_config.gateway && any_if_configured) {
    struct sockaddr_in address;
    struct sockaddr_in netmask;
    struct sockaddr_in gateway;

    rtems_bsdnet_initialize_sockaddr_in(&address);
    rtems_bsdnet_initialize_sockaddr_in(&netmask);
    rtems_bsdnet_initialize_sockaddr_in(&gateway);

    gateway.sin_addr.s_addr = inet_addr (rtems_bsdnet_config.gateway);

    if (rtems_bsdnet_rtrequest (
        RTM_ADD,
        (struct sockaddr *)&address,
        (struct sockaddr *)&gateway,
        (struct sockaddr *)&netmask,
        (RTF_UP | RTF_GATEWAY | RTF_STATIC), NULL) < 0) {
      printf ("Can't set default route: %s\n", strerror (errno));
      return -1;
    }
  }
  return 0;

}
