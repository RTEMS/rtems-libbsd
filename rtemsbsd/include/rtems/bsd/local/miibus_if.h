/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   dev/mii/miibus_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _miibus_if_h_
#define _miibus_if_h_

/** @brief Unique descriptor for the MIIBUS_READREG() method */
extern struct kobjop_desc miibus_readreg_desc;
/** @brief A function implementing the MIIBUS_READREG() method */
typedef int miibus_readreg_t(device_t dev, int phy, int reg);

static __inline int MIIBUS_READREG(device_t dev, int phy, int reg)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,miibus_readreg);
	rc = ((miibus_readreg_t *) _m)(dev, phy, reg);
	return (rc);
}

/** @brief Unique descriptor for the MIIBUS_WRITEREG() method */
extern struct kobjop_desc miibus_writereg_desc;
/** @brief A function implementing the MIIBUS_WRITEREG() method */
typedef int miibus_writereg_t(device_t dev, int phy, int reg, int val);

static __inline int MIIBUS_WRITEREG(device_t dev, int phy, int reg, int val)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,miibus_writereg);
	rc = ((miibus_writereg_t *) _m)(dev, phy, reg, val);
	return (rc);
}

/** @brief Unique descriptor for the MIIBUS_STATCHG() method */
extern struct kobjop_desc miibus_statchg_desc;
/** @brief A function implementing the MIIBUS_STATCHG() method */
typedef void miibus_statchg_t(device_t dev);

static __inline void MIIBUS_STATCHG(device_t dev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,miibus_statchg);
	((miibus_statchg_t *) _m)(dev);
}

/** @brief Unique descriptor for the MIIBUS_LINKCHG() method */
extern struct kobjop_desc miibus_linkchg_desc;
/** @brief A function implementing the MIIBUS_LINKCHG() method */
typedef void miibus_linkchg_t(device_t dev);

static __inline void MIIBUS_LINKCHG(device_t dev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,miibus_linkchg);
	((miibus_linkchg_t *) _m)(dev);
}

/** @brief Unique descriptor for the MIIBUS_MEDIAINIT() method */
extern struct kobjop_desc miibus_mediainit_desc;
/** @brief A function implementing the MIIBUS_MEDIAINIT() method */
typedef void miibus_mediainit_t(device_t dev);

static __inline void MIIBUS_MEDIAINIT(device_t dev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,miibus_mediainit);
	((miibus_mediainit_t *) _m)(dev);
}

#endif /* _miibus_if_h_ */
