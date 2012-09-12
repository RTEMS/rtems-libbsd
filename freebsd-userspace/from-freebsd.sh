#! /bin/bash
#
#  Temporary hack of a script to get FreeBSD user space header
#  files and network libc source code into this tree.
#
#  NOTE: libc and include are NOT in the FreeBSD checkout used for the
#        USB and TCP/IP stacks. This has to be addressed.
#
#  This probably could be integrated into the main Python script.
#  But so far, there are no real transformations needed and the
#  script just performs two tasks:
#
#  + copies source files from FreeBSD libc and include files into this tree
#  + generates "wrapper" header files for the public network .h files
#
src=/home/joel/newbsd/git/freebsd-8.2
dest=/home/joel/newbsd/git/rtems-libbsd/freebsd-userspace

progname=${0##*/}        # fast basename hack for ksh, bash

USAGE=\
"usage: $progname [ -opts ]
	-v	   -- verbose
	-f DIR	   -- FreeBSD directory
	-r DIR	   -- rtems-libbsd top directory
"

check_status()
{
  if [ $1 -ne 0 ] ; then
    shift 
    echo "$USAGE" >&2
    echo >&2
    echo "FAILED: " "$*" >&2
    exit 1
  fi
}

usage()
{
  echo "$USAGE" >&2
  exit 1
}

#
# process the options
#
# defaults for getopt vars
#

src=NOTSET
dest=NOTSET
verbose="no"

while getopts "vf:r:" OPT
do
 case "$OPT" in
   v) verbose="yes";;
   f) src="$OPTARG";;
   r) dest="$OPTARG";;
   *) usage;;
  esac
done

# Check arguments specified
test ${src} != "NOTSET" 
check_status $? "FreeBSD source directory not specified"

test ${dest} != "NOTSET" 
check_status $? "rtems-libbsd source directory not specified"

# Check that they are directories
test -d ${src}
check_status $? "FreeBSD source directory is not a directory"

test -d ${dest}
check_status $? "rtems-libbsd source directory is not a directory"

# Check that they appear to be the right directories
test -d ${src}/include
check_status $? "FreeBSD source directory does not have include directory"

test -d ${src}/lib/libc
check_status $? "FreeBSD source directory does not have libc directory"

test -d ${dest}/freebsd-userspace
check_status $? "rtems-libbsd source directory does not have freebsd-userspace directory"

dest=${dest}/freebsd-userspace

# source files to simply copy
while read f
do
  d=`dirname $f`
  test -d ${dest}/${d} || mkdir -p ${dest}/${d}
  test -r ${src}/${f}
  check_status $? "${src}/${f} is not present"

  test ${verbose} = "yes" && echo "Simple copy $f"
  cp ${src}/${f} ${dest}/${d}
done <<EOF
include/db.h
include/err.h
include/ifaddrs.h
include/netconfig.h
include/nsswitch.h
include/resolv.h
include/sysexit.h
include/res_update.h
include/rpc/auth.h
include/rpc/auth_unix.h
include/rpc/auth_des.h
include/rpc/clnt.h
include/rpc/clnt_soc.h
include/rpc/clnt_stat.h
include/rpc/pmap_clnt.h
include/rpc/pmap_prot.h
include/rpc/rpcent.h
include/rpc/rpc_msg.h
include/rpc/rpcb_clnt.h
include/rpc/rpcb_prot.x
include/rpc/svc.h
include/rpc/svc_auth.h
include/rpc/svc_soc.h
include/rpc/xdr.h
include/arpa/ftp.h
include/arpa/inet.h
include/arpa/nameser.h
sys/net/ethernet.h
sys/rpc/types.h
sys/sys/_null.h
sys/sys/un.h
lib/libc/include/libc_private.h
lib/libc/include/namespace.h
lib/libc/include/nss_tls.h
lib/libc/include/reentrant.h
lib/libc/include/resolv_mt.h
lib/libc/include/un-namespace.h
lib/libc/inet/inet_addr.c
lib/libc/inet/inet_cidr_ntop.c
lib/libc/inet/inet_cidr_pton.c
lib/libc/inet/inet_lnaof.c
lib/libc/inet/inet_makeaddr.c
lib/libc/inet/inet_neta.c
lib/libc/inet/inet_net_ntop.c
lib/libc/inet/inet_netof.c
lib/libc/inet/inet_net_pton.c
lib/libc/inet/inet_network.c
lib/libc/inet/inet_ntoa.c
lib/libc/inet/inet_ntop.c
lib/libc/inet/inet_pton.c
lib/libc/inet/nsap_addr.c
lib/libc/nameser/ns_samedomain.c
lib/libc/net/netdb_private.h
lib/libc/net/res_config.h
EOF
# processed by hand
# include/arpa/nameser_compat.h

# disable BIND_4_COMPAT since it trips a weird endian issue in nameser_compat.h
#sed -e 's/#define BIND_4_COMPAT/\/* #define BIND_4_COMPAT *\//' \
#  >${dest}/include/arpa/nameser.h
#  <${src}/include/arpa/nameser.h \

# fix include so it sees FreeBSD endian definitions
sed -e 's/<machine\/endian.h>/<freebsd\/machine\/endian.h>/' \
  <${src}/include/arpa/nameser_compat.h \
  >${dest}/include/arpa/nameser_compat.h


# source files to prepend "include of local/port_before.h"
while read f
do
  d=`dirname $f`
  test -d ${dest}/${d} || mkdir -p ${dest}/${d}
  test -r ${src}/${f}
  check_status $? "${src}/${f} is not present"

  test ${verbose} = "yes" && echo "Copy with prepend $f"
   
  ( echo "#include \"port_before.h\""; echo ; cat ${src}/${f} ) >${dest}/${f}
done <<EOF
lib/libc/net/base64.c
lib/libc/net/ether_addr.c
lib/libc/net/getaddrinfo.c
lib/libc/net/gethostbydns.c
lib/libc/net/gethostbyht.c
lib/libc/net/gethostbynis.c
lib/libc/net/gethostnamadr.c
lib/libc/net/getifaddrs.c
lib/libc/net/getifmaddrs.c
lib/libc/net/getnameinfo.c
lib/libc/net/getnetbydns.c
lib/libc/net/getnetbyht.c
lib/libc/net/getnetbynis.c
lib/libc/net/getnetnamadr.c
lib/libc/net/getproto.c
lib/libc/net/getprotoent.c
lib/libc/net/getprotoname.c
lib/libc/net/getservent.c
lib/libc/gen/gethostname.c
lib/libc/nameser/ns_name.c
lib/libc/nameser/ns_netint.c
lib/libc/nameser/ns_parse.c
lib/libc/nameser/ns_print.c
lib/libc/nameser/ns_ttl.c
lib/libc/net/if_indextoname.c
lib/libc/net/if_nameindex.c
lib/libc/net/linkaddr.c
lib/libc/net/map_v4v6.c
lib/libc/net/name6.c
lib/libc/net/rcmd.c
lib/libc/net/recv.c
lib/libc/net/send.c
lib/libc/resolv/res_private.h
lib/libc/resolv/herror.c
lib/libc/resolv/res_comp.c
lib/libc/resolv/res_data.c
lib/libc/resolv/res_debug.c
lib/libc/resolv/res_init.c
lib/libc/resolv/res_mkquery.c
lib/libc/resolv/res_mkupdate.c
lib/libc/resolv/res_query.c
lib/libc/resolv/res_send.c
lib/libc/resolv/res_update.c
lib/libc/string/strsep.c
EOF

# This file includes a private "dprintf" which conflicts with stdio.h
sed -e 's/dprintf/DPRINTF/g' <${dest}/lib/libc/net/gethostbydns.c  >XXX
mv XXX ${dest}/lib/libc/net/gethostbydns.c


# files to "include with freebsd"
while read f
do
  d=`dirname $f`
  test -d include/${d}/${d} || mkdir -p include/${d}/${d}

  test ${verbose} = "yes" && echo "Generate wrapper $f"
  ( echo "#include <freebsd/bsd.h>" ; 
    echo "#include <freebsd/${f}>" ) > include/${f}
done <<EOF
poll.h
net/ethernet.h
net/if.h
net/if_arp.h
net/if_dl.h
net/if_types.h
net/in.h
net/route.h
netatalk/at.h
netinet/if_ether.h
netinet/in.h
netinet/icmp6.h
netinet/igmp.h
netinet/in_systm.h
netinet/ip.h
netinet/ip_icmp.h
netinet/tcp.h
netinet/udp.h
sys/socket.h
sys/socketvar.h
sys/sysctl.h
sys/mman.h
EOF


# empty files
while read f
do
  test ${verbose} = "yes" && echo "Generate empty file $f"
  echo "/* empty file */" > ${f}
done <<EOF
local/pthread_np.h
local/sys/_pthreadtypes.h
EOF


# Fix syslog.h issues...
echo "#include <sys/syslog.h>" > local/syslog.h
# Should be able to copy this except for printflike issue
# sys/sys/syslog.h

# netdb.h has an issue with __socklen_t which needs to be run down
# for now, a manually edited copy is in rtems/include
# include/rpc/rpc.h has issue with internal methods changing from 
#   old implementation to current day. We need to update our RPC
