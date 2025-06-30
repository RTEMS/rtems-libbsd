/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   xdma_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _xdma_if_h_
#define _xdma_if_h_

/** @brief Unique descriptor for the XDMA_CHANNEL_REQUEST() method */
extern struct kobjop_desc xdma_channel_request_desc;
/** @brief A function implementing the XDMA_CHANNEL_REQUEST() method */
typedef int xdma_channel_request_t(device_t dev, struct xdma_channel *xchan,
                                   struct xdma_request *req);

static __inline int XDMA_CHANNEL_REQUEST(device_t dev,
                                         struct xdma_channel *xchan,
                                         struct xdma_request *req)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,xdma_channel_request);
	rc = ((xdma_channel_request_t *) _m)(dev, xchan, req);
	return (rc);
}

/** @brief Unique descriptor for the XDMA_CHANNEL_PREP_SG() method */
extern struct kobjop_desc xdma_channel_prep_sg_desc;
/** @brief A function implementing the XDMA_CHANNEL_PREP_SG() method */
typedef int xdma_channel_prep_sg_t(device_t dev, struct xdma_channel *xchan);

static __inline int XDMA_CHANNEL_PREP_SG(device_t dev,
                                         struct xdma_channel *xchan)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,xdma_channel_prep_sg);
	rc = ((xdma_channel_prep_sg_t *) _m)(dev, xchan);
	return (rc);
}

/** @brief Unique descriptor for the XDMA_CHANNEL_CAPACITY() method */
extern struct kobjop_desc xdma_channel_capacity_desc;
/** @brief A function implementing the XDMA_CHANNEL_CAPACITY() method */
typedef int xdma_channel_capacity_t(device_t dev, struct xdma_channel *xchan,
                                    uint32_t *capacity);

static __inline int XDMA_CHANNEL_CAPACITY(device_t dev,
                                          struct xdma_channel *xchan,
                                          uint32_t *capacity)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,xdma_channel_capacity);
	rc = ((xdma_channel_capacity_t *) _m)(dev, xchan, capacity);
	return (rc);
}

/** @brief Unique descriptor for the XDMA_CHANNEL_SUBMIT_SG() method */
extern struct kobjop_desc xdma_channel_submit_sg_desc;
/** @brief A function implementing the XDMA_CHANNEL_SUBMIT_SG() method */
typedef int xdma_channel_submit_sg_t(device_t dev, struct xdma_channel *xchan,
                                     struct xdma_sglist *sg, uint32_t sg_n);

static __inline int XDMA_CHANNEL_SUBMIT_SG(device_t dev,
                                           struct xdma_channel *xchan,
                                           struct xdma_sglist *sg,
                                           uint32_t sg_n)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,xdma_channel_submit_sg);
	rc = ((xdma_channel_submit_sg_t *) _m)(dev, xchan, sg, sg_n);
	return (rc);
}

/** @brief Unique descriptor for the XDMA_OFW_MD_DATA() method */
extern struct kobjop_desc xdma_ofw_md_data_desc;
/** @brief A function implementing the XDMA_OFW_MD_DATA() method */
typedef int xdma_ofw_md_data_t(device_t dev, pcell_t *cells, int ncells,
                               void **data);

static __inline int XDMA_OFW_MD_DATA(device_t dev, pcell_t *cells, int ncells,
                                     void **data)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,xdma_ofw_md_data);
	rc = ((xdma_ofw_md_data_t *) _m)(dev, cells, ncells, data);
	return (rc);
}

/** @brief Unique descriptor for the XDMA_CHANNEL_ALLOC() method */
extern struct kobjop_desc xdma_channel_alloc_desc;
/** @brief A function implementing the XDMA_CHANNEL_ALLOC() method */
typedef int xdma_channel_alloc_t(device_t dev, struct xdma_channel *xchan);

static __inline int XDMA_CHANNEL_ALLOC(device_t dev, struct xdma_channel *xchan)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,xdma_channel_alloc);
	rc = ((xdma_channel_alloc_t *) _m)(dev, xchan);
	return (rc);
}

/** @brief Unique descriptor for the XDMA_CHANNEL_FREE() method */
extern struct kobjop_desc xdma_channel_free_desc;
/** @brief A function implementing the XDMA_CHANNEL_FREE() method */
typedef int xdma_channel_free_t(device_t dev, struct xdma_channel *xchan);

static __inline int XDMA_CHANNEL_FREE(device_t dev, struct xdma_channel *xchan)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,xdma_channel_free);
	rc = ((xdma_channel_free_t *) _m)(dev, xchan);
	return (rc);
}

/** @brief Unique descriptor for the XDMA_CHANNEL_CONTROL() method */
extern struct kobjop_desc xdma_channel_control_desc;
/** @brief A function implementing the XDMA_CHANNEL_CONTROL() method */
typedef int xdma_channel_control_t(device_t dev, struct xdma_channel *xchan,
                                   int cmd);

static __inline int XDMA_CHANNEL_CONTROL(device_t dev,
                                         struct xdma_channel *xchan, int cmd)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,xdma_channel_control);
	rc = ((xdma_channel_control_t *) _m)(dev, xchan, cmd);
	return (rc);
}

void pmap_kremove_device(vm_offset_t, vm_size_t);
void pmap_kenter_device(vm_offset_t, vm_size_t, vm_paddr_t);
#define vtophys(x) (x)

#endif /* _xdma_if_h_ */
