/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/extres/regulator/regdev_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _regdev_if_h_
#define _regdev_if_h_


int regdev_default_ofw_map(device_t , phandle_t, int, pcell_t *, intptr_t *);

/** @brief Unique descriptor for the REGDEV_MAP() method */
extern struct kobjop_desc regdev_map_desc;
/** @brief A function implementing the REGDEV_MAP() method */
typedef int regdev_map_t(device_t provider_dev, phandle_t xref, int ncells,
                         pcell_t *cells, intptr_t *id);

static __inline int REGDEV_MAP(device_t provider_dev, phandle_t xref,
                               int ncells, pcell_t *cells, intptr_t *id)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)provider_dev)->ops,regdev_map);
	rc = ((regdev_map_t *) _m)(provider_dev, xref, ncells, cells, id);
	return (rc);
}

#endif /* _regdev_if_h_ */
