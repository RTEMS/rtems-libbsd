/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/dwc/if_dwc_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _if_dwc_if_h_
#define _if_dwc_if_h_



/** @brief Unique descriptor for the IF_DWC_INIT() method */
extern struct kobjop_desc if_dwc_init_desc;
/** @brief A function implementing the IF_DWC_INIT() method */
typedef int if_dwc_init_t(device_t dev);

static __inline int IF_DWC_INIT(device_t dev)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,if_dwc_init);
	rc = ((if_dwc_init_t *) _m)(dev);
	return (rc);
}

/** @brief Unique descriptor for the IF_DWC_MAC_TYPE() method */
extern struct kobjop_desc if_dwc_mac_type_desc;
/** @brief A function implementing the IF_DWC_MAC_TYPE() method */
typedef int if_dwc_mac_type_t(device_t dev);

static __inline int IF_DWC_MAC_TYPE(device_t dev)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,if_dwc_mac_type);
	rc = ((if_dwc_mac_type_t *) _m)(dev);
	return (rc);
}

/** @brief Unique descriptor for the IF_DWC_MII_CLK() method */
extern struct kobjop_desc if_dwc_mii_clk_desc;
/** @brief A function implementing the IF_DWC_MII_CLK() method */
typedef int if_dwc_mii_clk_t(device_t dev);

static __inline int IF_DWC_MII_CLK(device_t dev)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,if_dwc_mii_clk);
	rc = ((if_dwc_mii_clk_t *) _m)(dev);
	return (rc);
}

/** @brief Unique descriptor for the IF_DWC_SET_SPEED() method */
extern struct kobjop_desc if_dwc_set_speed_desc;
/** @brief A function implementing the IF_DWC_SET_SPEED() method */
typedef int if_dwc_set_speed_t(device_t dev, int speed);

static __inline int IF_DWC_SET_SPEED(device_t dev, int speed)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,if_dwc_set_speed);
	rc = ((if_dwc_set_speed_t *) _m)(dev, speed);
	return (rc);
}

#endif /* _if_dwc_if_h_ */
