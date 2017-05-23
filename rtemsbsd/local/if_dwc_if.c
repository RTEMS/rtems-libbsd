#include <machine/rtems-bsd-kernel-space.h>

/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/dwc/if_dwc_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <rtems/bsd/local/if_dwc_if.h>

#include <dev/dwc/if_dwc.h>

static int
if_dwc_default_init(device_t dev)
{
	return (0);
}

static int
if_dwc_default_mac_type(device_t dev)
{
	return (DWC_GMAC);
}

static int
if_dwc_default_mii_clk(device_t dev)
{
	return (GMAC_MII_CLK_25_35M_DIV16);
}

struct kobjop_desc if_dwc_init_desc = {
	0, { &if_dwc_init_desc, (kobjop_t)if_dwc_default_init }
};

struct kobjop_desc if_dwc_mac_type_desc = {
	0, { &if_dwc_mac_type_desc, (kobjop_t)if_dwc_default_mac_type }
};

struct kobjop_desc if_dwc_mii_clk_desc = {
	0, { &if_dwc_mii_clk_desc, (kobjop_t)if_dwc_default_mii_clk }
};

