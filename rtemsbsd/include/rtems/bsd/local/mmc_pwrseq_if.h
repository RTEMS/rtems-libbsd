/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/mmc/mmc_pwrseq_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _mmc_pwrseq_if_h_
#define _mmc_pwrseq_if_h_

/** @brief Unique descriptor for the MMC_PWRSEQ_SET_POWER() method */
extern struct kobjop_desc mmc_pwrseq_set_power_desc;
/** @brief A function implementing the MMC_PWRSEQ_SET_POWER() method */
typedef int mmc_pwrseq_set_power_t(device_t bus, bool power_on);

static __inline int MMC_PWRSEQ_SET_POWER(device_t bus, bool power_on)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)bus)->ops,mmc_pwrseq_set_power);
	rc = ((mmc_pwrseq_set_power_t *) _m)(bus, power_on);
	return (rc);
}

#endif /* _mmc_pwrseq_if_h_ */
