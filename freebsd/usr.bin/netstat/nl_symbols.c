#include <machine/rtems-bsd-user-space.h>

#ifdef __rtems__
#include "rtems-bsd-netstat-namespace.h"
#endif /* __rtems__ */

#ifdef __rtems__
#include <machine/rtems-bsd-program.h>
#endif /* __rtems__ */
#include <sys/param.h>
#include <nlist.h>
#ifdef __rtems__
#include "rtems-bsd-netstat-nl_symbols-data.h"
#endif /* __rtems__ */
struct nlist nl[] = {
	{ .n_name = "_ahstat" },
	{ .n_name = "_arpstat" },
	{ .n_name = "_carpstats" },
	{ .n_name = "_espstat" },
	{ .n_name = "_icmp6stat" },
	{ .n_name = "_icmpstat" },
	{ .n_name = "_igmpstat" },
	{ .n_name = "_ip6stat" },
	{ .n_name = "_ipcompstat" },
	{ .n_name = "_ipsec4stat" },
	{ .n_name = "_ipsec6stat" },
	{ .n_name = "_ipstat" },
	{ .n_name = "_mf6ctable" },
	{ .n_name = "_mfchashtbl" },
	{ .n_name = "_mfctablesize" },
	{ .n_name = "_mif6table" },
	{ .n_name = "_mrt6stat" },
	{ .n_name = "_mrtstat" },
	{ .n_name = "_netisr_bindthreads" },
	{ .n_name = "_netisr_defaultqlimit" },
	{ .n_name = "_netisr_dispatch_policy" },
	{ .n_name = "_netisr_maxprot" },
	{ .n_name = "_netisr_maxqlimit" },
	{ .n_name = "_netisr_maxthreads" },
	{ .n_name = "_netisr_proto" },
	{ .n_name = "_ngsocklist" },
	{ .n_name = "_nws" },
	{ .n_name = "_nws_array" },
	{ .n_name = "_nws_count" },
	{ .n_name = "_pfkeystat" },
	{ .n_name = "_pfsyncstats" },
	{ .n_name = "_pflowstats" },
	{ .n_name = "_pim6stat" },
	{ .n_name = "_pimstat" },
	{ .n_name = "_rip6stat" },
	{ .n_name = "_rtree" },
	{ .n_name = "_rtstat" },
	{ .n_name = "_sctpstat" },
	{ .n_name = "_sfstat" },
	{ .n_name = "_tcpstat" },
	{ .n_name = "_tcps_states" },
	{ .n_name = "_udpstat" },
	{ .n_name = "_unp_count" },
	{ .n_name = "_unp_dhead" },
	{ .n_name = "_unp_gencnt" },
	{ .n_name = "_unp_shead" },
	{ .n_name = "_unp_sphead" },
	{ .n_name = "_viftable" },
	{ .n_name = NULL },
};
