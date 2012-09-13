/*
 * RTEMS version of syslog initialization
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


struct in_addr rtems_bsdnet_log_host_address = {0};

int rtems_syslog_initialize(const char *log_host)
{
  struct in_addr *i;

  i = &rtems_bsdnet_log_host_address;
  
  if (log_host)
    i->s_addr = inet_addr(log_host);
  return 0;
}
