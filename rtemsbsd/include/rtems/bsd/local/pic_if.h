/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/kern/pic_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _pic_if_h_
#define _pic_if_h_

/** @brief Unique descriptor for the PIC_ACTIVATE_INTR() method */
extern struct kobjop_desc pic_activate_intr_desc;
/** @brief A function implementing the PIC_ACTIVATE_INTR() method */
typedef int pic_activate_intr_t(device_t dev, struct intr_irqsrc *isrc,
                                struct resource *res,
                                struct intr_map_data *data);

static __inline int PIC_ACTIVATE_INTR(device_t dev, struct intr_irqsrc *isrc,
                                      struct resource *res,
                                      struct intr_map_data *data)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_activate_intr);
	rc = ((pic_activate_intr_t *) _m)(dev, isrc, res, data);
	return (rc);
}

/** @brief Unique descriptor for the PIC_BIND_INTR() method */
extern struct kobjop_desc pic_bind_intr_desc;
/** @brief A function implementing the PIC_BIND_INTR() method */
typedef int pic_bind_intr_t(device_t dev, struct intr_irqsrc *isrc);

static __inline int PIC_BIND_INTR(device_t dev, struct intr_irqsrc *isrc)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_bind_intr);
	rc = ((pic_bind_intr_t *) _m)(dev, isrc);
	return (rc);
}

/** @brief Unique descriptor for the PIC_DISABLE_INTR() method */
extern struct kobjop_desc pic_disable_intr_desc;
/** @brief A function implementing the PIC_DISABLE_INTR() method */
typedef void pic_disable_intr_t(device_t dev, struct intr_irqsrc *isrc);

static __inline void PIC_DISABLE_INTR(device_t dev, struct intr_irqsrc *isrc)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_disable_intr);
	((pic_disable_intr_t *) _m)(dev, isrc);
}

/** @brief Unique descriptor for the PIC_ENABLE_INTR() method */
extern struct kobjop_desc pic_enable_intr_desc;
/** @brief A function implementing the PIC_ENABLE_INTR() method */
typedef void pic_enable_intr_t(device_t dev, struct intr_irqsrc *isrc);

static __inline void PIC_ENABLE_INTR(device_t dev, struct intr_irqsrc *isrc)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_enable_intr);
	((pic_enable_intr_t *) _m)(dev, isrc);
}

/** @brief Unique descriptor for the PIC_MAP_INTR() method */
extern struct kobjop_desc pic_map_intr_desc;
/** @brief A function implementing the PIC_MAP_INTR() method */
typedef int pic_map_intr_t(device_t dev, struct intr_map_data *data,
                           struct intr_irqsrc **isrcp);

static __inline int PIC_MAP_INTR(device_t dev, struct intr_map_data *data,
                                 struct intr_irqsrc **isrcp)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_map_intr);
	rc = ((pic_map_intr_t *) _m)(dev, data, isrcp);
	return (rc);
}

/** @brief Unique descriptor for the PIC_DEACTIVATE_INTR() method */
extern struct kobjop_desc pic_deactivate_intr_desc;
/** @brief A function implementing the PIC_DEACTIVATE_INTR() method */
typedef int pic_deactivate_intr_t(device_t dev, struct intr_irqsrc *isrc,
                                  struct resource *res,
                                  struct intr_map_data *data);

static __inline int PIC_DEACTIVATE_INTR(device_t dev, struct intr_irqsrc *isrc,
                                        struct resource *res,
                                        struct intr_map_data *data)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_deactivate_intr);
	rc = ((pic_deactivate_intr_t *) _m)(dev, isrc, res, data);
	return (rc);
}

/** @brief Unique descriptor for the PIC_SETUP_INTR() method */
extern struct kobjop_desc pic_setup_intr_desc;
/** @brief A function implementing the PIC_SETUP_INTR() method */
typedef int pic_setup_intr_t(device_t dev, struct intr_irqsrc *isrc,
                             struct resource *res, struct intr_map_data *data);

static __inline int PIC_SETUP_INTR(device_t dev, struct intr_irqsrc *isrc,
                                   struct resource *res,
                                   struct intr_map_data *data)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_setup_intr);
	rc = ((pic_setup_intr_t *) _m)(dev, isrc, res, data);
	return (rc);
}

/** @brief Unique descriptor for the PIC_TEARDOWN_INTR() method */
extern struct kobjop_desc pic_teardown_intr_desc;
/** @brief A function implementing the PIC_TEARDOWN_INTR() method */
typedef int pic_teardown_intr_t(device_t dev, struct intr_irqsrc *isrc,
                                struct resource *res,
                                struct intr_map_data *data);

static __inline int PIC_TEARDOWN_INTR(device_t dev, struct intr_irqsrc *isrc,
                                      struct resource *res,
                                      struct intr_map_data *data)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_teardown_intr);
	rc = ((pic_teardown_intr_t *) _m)(dev, isrc, res, data);
	return (rc);
}

/** @brief Unique descriptor for the PIC_POST_FILTER() method */
extern struct kobjop_desc pic_post_filter_desc;
/** @brief A function implementing the PIC_POST_FILTER() method */
typedef void pic_post_filter_t(device_t dev, struct intr_irqsrc *isrc);

static __inline void PIC_POST_FILTER(device_t dev, struct intr_irqsrc *isrc)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_post_filter);
	((pic_post_filter_t *) _m)(dev, isrc);
}

/** @brief Unique descriptor for the PIC_POST_ITHREAD() method */
extern struct kobjop_desc pic_post_ithread_desc;
/** @brief A function implementing the PIC_POST_ITHREAD() method */
typedef void pic_post_ithread_t(device_t dev, struct intr_irqsrc *isrc);

static __inline void PIC_POST_ITHREAD(device_t dev, struct intr_irqsrc *isrc)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_post_ithread);
	((pic_post_ithread_t *) _m)(dev, isrc);
}

/** @brief Unique descriptor for the PIC_PRE_ITHREAD() method */
extern struct kobjop_desc pic_pre_ithread_desc;
/** @brief A function implementing the PIC_PRE_ITHREAD() method */
typedef void pic_pre_ithread_t(device_t dev, struct intr_irqsrc *isrc);

static __inline void PIC_PRE_ITHREAD(device_t dev, struct intr_irqsrc *isrc)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_pre_ithread);
	((pic_pre_ithread_t *) _m)(dev, isrc);
}

/** @brief Unique descriptor for the PIC_INIT_SECONDARY() method */
extern struct kobjop_desc pic_init_secondary_desc;
/** @brief A function implementing the PIC_INIT_SECONDARY() method */
typedef void pic_init_secondary_t(device_t dev);

static __inline void PIC_INIT_SECONDARY(device_t dev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_init_secondary);
	((pic_init_secondary_t *) _m)(dev);
}

/** @brief Unique descriptor for the PIC_IPI_SEND() method */
extern struct kobjop_desc pic_ipi_send_desc;
/** @brief A function implementing the PIC_IPI_SEND() method */
typedef void pic_ipi_send_t(device_t dev, struct intr_irqsrc *isrc,
                            cpuset_t cpus, u_int ipi);

static __inline void PIC_IPI_SEND(device_t dev, struct intr_irqsrc *isrc,
                                  cpuset_t cpus, u_int ipi)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_ipi_send);
	((pic_ipi_send_t *) _m)(dev, isrc, cpus, ipi);
}

/** @brief Unique descriptor for the PIC_IPI_SETUP() method */
extern struct kobjop_desc pic_ipi_setup_desc;
/** @brief A function implementing the PIC_IPI_SETUP() method */
typedef int pic_ipi_setup_t(device_t dev, u_int ipi,
                            struct intr_irqsrc **isrcp);

static __inline int PIC_IPI_SETUP(device_t dev, u_int ipi,
                                  struct intr_irqsrc **isrcp)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_ipi_setup);
	rc = ((pic_ipi_setup_t *) _m)(dev, ipi, isrcp);
	return (rc);
}

#endif /* _pic_if_h_ */
