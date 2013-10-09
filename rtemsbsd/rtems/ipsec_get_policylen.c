
#include <rtems/bsd/sys/types.h>
#include <rtems/bsd/sys/param.h>

#include <netipsec/ipsec.h>

#include <net/pfkeyv2.h>

#include "ipsec_strerror.h"

int
ipsec_get_policylen(policy)
	caddr_t policy;
{
	return policy ? PFKEY_EXTLEN(policy) : -1;
}
