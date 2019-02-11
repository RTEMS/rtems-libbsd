/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/ofw/ofw_bus_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _ofw_bus_if_h_
#define _ofw_bus_if_h_


struct ofw_bus_devinfo {
	phandle_t	obd_node;
	char		*obd_compat;
	char		*obd_model;
	char		*obd_name;
	char		*obd_type;
	char		*obd_status;
};

/** @brief Unique descriptor for the OFW_BUS_GET_DEVINFO() method */
extern struct kobjop_desc ofw_bus_get_devinfo_desc;
/** @brief A function implementing the OFW_BUS_GET_DEVINFO() method */
typedef const struct ofw_bus_devinfo * ofw_bus_get_devinfo_t(device_t bus,
                                                             device_t dev);

static __inline const struct ofw_bus_devinfo * OFW_BUS_GET_DEVINFO(device_t bus,
                                                                   device_t dev)
{
	kobjop_t _m;
	const struct ofw_bus_devinfo * rc;
	KOBJOPLOOKUP(((kobj_t)bus)->ops,ofw_bus_get_devinfo);
	rc = ((ofw_bus_get_devinfo_t *) _m)(bus, dev);
	return (rc);
}

/** @brief Unique descriptor for the OFW_BUS_GET_COMPAT() method */
extern struct kobjop_desc ofw_bus_get_compat_desc;
/** @brief A function implementing the OFW_BUS_GET_COMPAT() method */
typedef const char * ofw_bus_get_compat_t(device_t bus, device_t dev);

static __inline const char * OFW_BUS_GET_COMPAT(device_t bus, device_t dev)
{
	kobjop_t _m;
	const char * rc;
	KOBJOPLOOKUP(((kobj_t)bus)->ops,ofw_bus_get_compat);
	rc = ((ofw_bus_get_compat_t *) _m)(bus, dev);
	return (rc);
}

/** @brief Unique descriptor for the OFW_BUS_GET_MODEL() method */
extern struct kobjop_desc ofw_bus_get_model_desc;
/** @brief A function implementing the OFW_BUS_GET_MODEL() method */
typedef const char * ofw_bus_get_model_t(device_t bus, device_t dev);

static __inline const char * OFW_BUS_GET_MODEL(device_t bus, device_t dev)
{
	kobjop_t _m;
	const char * rc;
	KOBJOPLOOKUP(((kobj_t)bus)->ops,ofw_bus_get_model);
	rc = ((ofw_bus_get_model_t *) _m)(bus, dev);
	return (rc);
}

/** @brief Unique descriptor for the OFW_BUS_GET_NAME() method */
extern struct kobjop_desc ofw_bus_get_name_desc;
/** @brief A function implementing the OFW_BUS_GET_NAME() method */
typedef const char * ofw_bus_get_name_t(device_t bus, device_t dev);

static __inline const char * OFW_BUS_GET_NAME(device_t bus, device_t dev)
{
	kobjop_t _m;
	const char * rc;
	KOBJOPLOOKUP(((kobj_t)bus)->ops,ofw_bus_get_name);
	rc = ((ofw_bus_get_name_t *) _m)(bus, dev);
	return (rc);
}

/** @brief Unique descriptor for the OFW_BUS_GET_NODE() method */
extern struct kobjop_desc ofw_bus_get_node_desc;
/** @brief A function implementing the OFW_BUS_GET_NODE() method */
typedef phandle_t ofw_bus_get_node_t(device_t bus, device_t dev);

static __inline phandle_t OFW_BUS_GET_NODE(device_t bus, device_t dev)
{
	kobjop_t _m;
	phandle_t rc;
	KOBJOPLOOKUP(((kobj_t)bus)->ops,ofw_bus_get_node);
	rc = ((ofw_bus_get_node_t *) _m)(bus, dev);
	return (rc);
}

/** @brief Unique descriptor for the OFW_BUS_GET_TYPE() method */
extern struct kobjop_desc ofw_bus_get_type_desc;
/** @brief A function implementing the OFW_BUS_GET_TYPE() method */
typedef const char * ofw_bus_get_type_t(device_t bus, device_t dev);

static __inline const char * OFW_BUS_GET_TYPE(device_t bus, device_t dev)
{
	kobjop_t _m;
	const char * rc;
	KOBJOPLOOKUP(((kobj_t)bus)->ops,ofw_bus_get_type);
	rc = ((ofw_bus_get_type_t *) _m)(bus, dev);
	return (rc);
}

/** @brief Unique descriptor for the OFW_BUS_MAP_INTR() method */
extern struct kobjop_desc ofw_bus_map_intr_desc;
/** @brief A function implementing the OFW_BUS_MAP_INTR() method */
typedef int ofw_bus_map_intr_t(device_t bus, device_t dev, phandle_t iparent,
                               int icells, pcell_t *interrupt);

static __inline int OFW_BUS_MAP_INTR(device_t bus, device_t dev,
                                     phandle_t iparent, int icells,
                                     pcell_t *interrupt)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)bus)->ops,ofw_bus_map_intr);
	rc = ((ofw_bus_map_intr_t *) _m)(bus, dev, iparent, icells, interrupt);
	return (rc);
}

#endif /* _ofw_bus_if_h_ */
