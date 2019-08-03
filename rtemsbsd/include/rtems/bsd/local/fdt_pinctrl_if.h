/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/fdt/fdt_pinctrl_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _fdt_pinctrl_if_h_
#define _fdt_pinctrl_if_h_


#include <sys/tslog.h>

/** @brief Unique descriptor for the FDT_PINCTRL_CONFIGURE() method */
extern struct kobjop_desc fdt_pinctrl_configure_desc;
/** @brief A function implementing the FDT_PINCTRL_CONFIGURE() method */
typedef int fdt_pinctrl_configure_t(device_t pinctrl, phandle_t cfgxref);

static __inline int FDT_PINCTRL_CONFIGURE(device_t pinctrl, phandle_t cfgxref)
{
	kobjop_t _m;
	int rc;

TSENTER2(device_get_name(pinctrl));

	KOBJOPLOOKUP(((kobj_t)pinctrl)->ops,fdt_pinctrl_configure);
	rc = ((fdt_pinctrl_configure_t *) _m)(pinctrl, cfgxref);

TSEXIT2(device_get_name(pinctrl));

	return (rc);
}

#endif /* _fdt_pinctrl_if_h_ */
