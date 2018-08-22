/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/sdhci/sdhci_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _sdhci_if_h_
#define _sdhci_if_h_

/** @brief Unique descriptor for the SDHCI_READ_1() method */
extern struct kobjop_desc sdhci_read_1_desc;
/** @brief A function implementing the SDHCI_READ_1() method */
typedef uint8_t sdhci_read_1_t(device_t brdev, struct sdhci_slot *slot,
                               bus_size_t off);

static __inline uint8_t SDHCI_READ_1(device_t brdev, struct sdhci_slot *slot,
                                     bus_size_t off)
{
	kobjop_t _m;
	uint8_t rc;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,sdhci_read_1);
	rc = ((sdhci_read_1_t *) _m)(brdev, slot, off);
	return (rc);
}

/** @brief Unique descriptor for the SDHCI_READ_2() method */
extern struct kobjop_desc sdhci_read_2_desc;
/** @brief A function implementing the SDHCI_READ_2() method */
typedef uint16_t sdhci_read_2_t(device_t brdev, struct sdhci_slot *slot,
                                bus_size_t off);

static __inline uint16_t SDHCI_READ_2(device_t brdev, struct sdhci_slot *slot,
                                      bus_size_t off)
{
	kobjop_t _m;
	uint16_t rc;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,sdhci_read_2);
	rc = ((sdhci_read_2_t *) _m)(brdev, slot, off);
	return (rc);
}

/** @brief Unique descriptor for the SDHCI_READ_4() method */
extern struct kobjop_desc sdhci_read_4_desc;
/** @brief A function implementing the SDHCI_READ_4() method */
typedef uint32_t sdhci_read_4_t(device_t brdev, struct sdhci_slot *slot,
                                bus_size_t off);

static __inline uint32_t SDHCI_READ_4(device_t brdev, struct sdhci_slot *slot,
                                      bus_size_t off)
{
	kobjop_t _m;
	uint32_t rc;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,sdhci_read_4);
	rc = ((sdhci_read_4_t *) _m)(brdev, slot, off);
	return (rc);
}

/** @brief Unique descriptor for the SDHCI_READ_MULTI_4() method */
extern struct kobjop_desc sdhci_read_multi_4_desc;
/** @brief A function implementing the SDHCI_READ_MULTI_4() method */
typedef void sdhci_read_multi_4_t(device_t brdev, struct sdhci_slot *slot,
                                  bus_size_t off, uint32_t *data,
                                  bus_size_t count);

static __inline void SDHCI_READ_MULTI_4(device_t brdev, struct sdhci_slot *slot,
                                        bus_size_t off, uint32_t *data,
                                        bus_size_t count)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,sdhci_read_multi_4);
	((sdhci_read_multi_4_t *) _m)(brdev, slot, off, data, count);
}

/** @brief Unique descriptor for the SDHCI_WRITE_1() method */
extern struct kobjop_desc sdhci_write_1_desc;
/** @brief A function implementing the SDHCI_WRITE_1() method */
typedef void sdhci_write_1_t(device_t brdev, struct sdhci_slot *slot,
                             bus_size_t off, uint8_t val);

static __inline void SDHCI_WRITE_1(device_t brdev, struct sdhci_slot *slot,
                                   bus_size_t off, uint8_t val)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,sdhci_write_1);
	((sdhci_write_1_t *) _m)(brdev, slot, off, val);
}

/** @brief Unique descriptor for the SDHCI_WRITE_2() method */
extern struct kobjop_desc sdhci_write_2_desc;
/** @brief A function implementing the SDHCI_WRITE_2() method */
typedef void sdhci_write_2_t(device_t brdev, struct sdhci_slot *slot,
                             bus_size_t off, uint16_t val);

static __inline void SDHCI_WRITE_2(device_t brdev, struct sdhci_slot *slot,
                                   bus_size_t off, uint16_t val)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,sdhci_write_2);
	((sdhci_write_2_t *) _m)(brdev, slot, off, val);
}

/** @brief Unique descriptor for the SDHCI_WRITE_4() method */
extern struct kobjop_desc sdhci_write_4_desc;
/** @brief A function implementing the SDHCI_WRITE_4() method */
typedef void sdhci_write_4_t(device_t brdev, struct sdhci_slot *slot,
                             bus_size_t off, uint32_t val);

static __inline void SDHCI_WRITE_4(device_t brdev, struct sdhci_slot *slot,
                                   bus_size_t off, uint32_t val)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,sdhci_write_4);
	((sdhci_write_4_t *) _m)(brdev, slot, off, val);
}

/** @brief Unique descriptor for the SDHCI_WRITE_MULTI_4() method */
extern struct kobjop_desc sdhci_write_multi_4_desc;
/** @brief A function implementing the SDHCI_WRITE_MULTI_4() method */
typedef void sdhci_write_multi_4_t(device_t brdev, struct sdhci_slot *slot,
                                   bus_size_t off, uint32_t *data,
                                   bus_size_t count);

static __inline void SDHCI_WRITE_MULTI_4(device_t brdev,
                                         struct sdhci_slot *slot,
                                         bus_size_t off, uint32_t *data,
                                         bus_size_t count)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,sdhci_write_multi_4);
	((sdhci_write_multi_4_t *) _m)(brdev, slot, off, data, count);
}

/** @brief Unique descriptor for the SDHCI_PLATFORM_WILL_HANDLE() method */
extern struct kobjop_desc sdhci_platform_will_handle_desc;
/** @brief A function implementing the SDHCI_PLATFORM_WILL_HANDLE() method */
typedef int sdhci_platform_will_handle_t(device_t brdev,
                                         struct sdhci_slot *slot);

static __inline int SDHCI_PLATFORM_WILL_HANDLE(device_t brdev,
                                               struct sdhci_slot *slot)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,sdhci_platform_will_handle);
	rc = ((sdhci_platform_will_handle_t *) _m)(brdev, slot);
	return (rc);
}

/** @brief Unique descriptor for the SDHCI_PLATFORM_START_TRANSFER() method */
extern struct kobjop_desc sdhci_platform_start_transfer_desc;
/** @brief A function implementing the SDHCI_PLATFORM_START_TRANSFER() method */
typedef void sdhci_platform_start_transfer_t(device_t brdev,
                                             struct sdhci_slot *slot,
                                             uint32_t *intmask);

static __inline void SDHCI_PLATFORM_START_TRANSFER(device_t brdev,
                                                   struct sdhci_slot *slot,
                                                   uint32_t *intmask)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,sdhci_platform_start_transfer);
	((sdhci_platform_start_transfer_t *) _m)(brdev, slot, intmask);
}

/** @brief Unique descriptor for the SDHCI_PLATFORM_FINISH_TRANSFER() method */
extern struct kobjop_desc sdhci_platform_finish_transfer_desc;
/** @brief A function implementing the SDHCI_PLATFORM_FINISH_TRANSFER() method */
typedef void sdhci_platform_finish_transfer_t(device_t brdev,
                                              struct sdhci_slot *slot);

static __inline void SDHCI_PLATFORM_FINISH_TRANSFER(device_t brdev,
                                                    struct sdhci_slot *slot)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,sdhci_platform_finish_transfer);
	((sdhci_platform_finish_transfer_t *) _m)(brdev, slot);
}

/** @brief Unique descriptor for the SDHCI_MIN_FREQ() method */
extern struct kobjop_desc sdhci_min_freq_desc;
/** @brief A function implementing the SDHCI_MIN_FREQ() method */
typedef uint32_t sdhci_min_freq_t(device_t brdev, struct sdhci_slot *slot);

static __inline uint32_t SDHCI_MIN_FREQ(device_t brdev, struct sdhci_slot *slot)
{
	kobjop_t _m;
	uint32_t rc;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,sdhci_min_freq);
	rc = ((sdhci_min_freq_t *) _m)(brdev, slot);
	return (rc);
}

/** @brief Unique descriptor for the SDHCI_GET_CARD_PRESENT() method */
extern struct kobjop_desc sdhci_get_card_present_desc;
/** @brief A function implementing the SDHCI_GET_CARD_PRESENT() method */
typedef bool sdhci_get_card_present_t(device_t brdev, struct sdhci_slot *slot);

static __inline bool SDHCI_GET_CARD_PRESENT(device_t brdev,
                                            struct sdhci_slot *slot)
{
	kobjop_t _m;
	bool rc;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,sdhci_get_card_present);
	rc = ((sdhci_get_card_present_t *) _m)(brdev, slot);
	return (rc);
}

/** @brief Unique descriptor for the SDHCI_SET_UHS_TIMING() method */
extern struct kobjop_desc sdhci_set_uhs_timing_desc;
/** @brief A function implementing the SDHCI_SET_UHS_TIMING() method */
typedef void sdhci_set_uhs_timing_t(device_t brdev, struct sdhci_slot *slot);

static __inline void SDHCI_SET_UHS_TIMING(device_t brdev,
                                          struct sdhci_slot *slot)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,sdhci_set_uhs_timing);
	((sdhci_set_uhs_timing_t *) _m)(brdev, slot);
}

#endif /* _sdhci_if_h_ */
