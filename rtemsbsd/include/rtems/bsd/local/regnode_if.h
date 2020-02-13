/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/extres/regulator/regnode_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _regnode_if_h_
#define _regnode_if_h_


struct regnode;

/** @brief Unique descriptor for the REGNODE_INIT() method */
extern struct kobjop_desc regnode_init_desc;
/** @brief A function implementing the REGNODE_INIT() method */
typedef int regnode_init_t(struct regnode *regnode);

static __inline int REGNODE_INIT(struct regnode *regnode)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)regnode)->ops,regnode_init);
	rc = ((regnode_init_t *) _m)(regnode);
	return (rc);
}

/** @brief Unique descriptor for the REGNODE_ENABLE() method */
extern struct kobjop_desc regnode_enable_desc;
/** @brief A function implementing the REGNODE_ENABLE() method */
typedef int regnode_enable_t(struct regnode *regnode, bool enable, int *udelay);

static __inline int REGNODE_ENABLE(struct regnode *regnode, bool enable,
                                   int *udelay)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)regnode)->ops,regnode_enable);
	rc = ((regnode_enable_t *) _m)(regnode, enable, udelay);
	return (rc);
}

/** @brief Unique descriptor for the REGNODE_STATUS() method */
extern struct kobjop_desc regnode_status_desc;
/** @brief A function implementing the REGNODE_STATUS() method */
typedef int regnode_status_t(struct regnode *regnode, int *status);

static __inline int REGNODE_STATUS(struct regnode *regnode, int *status)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)regnode)->ops,regnode_status);
	rc = ((regnode_status_t *) _m)(regnode, status);
	return (rc);
}

/** @brief Unique descriptor for the REGNODE_SET_VOLTAGE() method */
extern struct kobjop_desc regnode_set_voltage_desc;
/** @brief A function implementing the REGNODE_SET_VOLTAGE() method */
typedef int regnode_set_voltage_t(struct regnode *regnode, int min_uvolt,
                                  int max_uvolt, int *udelay);

static __inline int REGNODE_SET_VOLTAGE(struct regnode *regnode, int min_uvolt,
                                        int max_uvolt, int *udelay)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)regnode)->ops,regnode_set_voltage);
	rc = ((regnode_set_voltage_t *) _m)(regnode, min_uvolt, max_uvolt, udelay);
	return (rc);
}

/** @brief Unique descriptor for the REGNODE_GET_VOLTAGE() method */
extern struct kobjop_desc regnode_get_voltage_desc;
/** @brief A function implementing the REGNODE_GET_VOLTAGE() method */
typedef int regnode_get_voltage_t(struct regnode *regnode, int *uvolt);

static __inline int REGNODE_GET_VOLTAGE(struct regnode *regnode, int *uvolt)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)regnode)->ops,regnode_get_voltage);
	rc = ((regnode_get_voltage_t *) _m)(regnode, uvolt);
	return (rc);
}

/** @brief Unique descriptor for the REGNODE_STOP() method */
extern struct kobjop_desc regnode_stop_desc;
/** @brief A function implementing the REGNODE_STOP() method */
typedef int regnode_stop_t(struct regnode *regnode, int *udelay);

static __inline int REGNODE_STOP(struct regnode *regnode, int *udelay)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)regnode)->ops,regnode_stop);
	rc = ((regnode_stop_t *) _m)(regnode, udelay);
	return (rc);
}

#endif /* _regnode_if_h_ */
