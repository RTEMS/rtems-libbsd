/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/powerpc/powerpc/pic_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _pic_if_h_
#define _pic_if_h_

/** @brief Unique descriptor for the PIC_BIND() method */
extern struct kobjop_desc pic_bind_desc;
/** @brief A function implementing the PIC_BIND() method */
typedef void pic_bind_t(device_t dev, u_int irq, cpuset_t cpumask, void **priv);

static __inline void PIC_BIND(device_t dev, u_int irq, cpuset_t cpumask,
                              void **priv)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_bind);
	((pic_bind_t *) _m)(dev, irq, cpumask, priv);
}

/** @brief Unique descriptor for the PIC_TRANSLATE_CODE() method */
extern struct kobjop_desc pic_translate_code_desc;
/** @brief A function implementing the PIC_TRANSLATE_CODE() method */
typedef void pic_translate_code_t(device_t dev, u_int irq, int code,
                                  enum intr_trigger *trig,
                                  enum intr_polarity *pol);

static __inline void PIC_TRANSLATE_CODE(device_t dev, u_int irq, int code,
                                        enum intr_trigger *trig,
                                        enum intr_polarity *pol)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_translate_code);
	((pic_translate_code_t *) _m)(dev, irq, code, trig, pol);
}

/** @brief Unique descriptor for the PIC_CONFIG() method */
extern struct kobjop_desc pic_config_desc;
/** @brief A function implementing the PIC_CONFIG() method */
typedef void pic_config_t(device_t dev, u_int irq, enum intr_trigger trig,
                          enum intr_polarity pol);

static __inline void PIC_CONFIG(device_t dev, u_int irq, enum intr_trigger trig,
                                enum intr_polarity pol)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_config);
	((pic_config_t *) _m)(dev, irq, trig, pol);
}

/** @brief Unique descriptor for the PIC_DISPATCH() method */
extern struct kobjop_desc pic_dispatch_desc;
/** @brief A function implementing the PIC_DISPATCH() method */
typedef void pic_dispatch_t(device_t dev, struct trapframe *tf);

static __inline void PIC_DISPATCH(device_t dev, struct trapframe *tf)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_dispatch);
	((pic_dispatch_t *) _m)(dev, tf);
}

/** @brief Unique descriptor for the PIC_ENABLE() method */
extern struct kobjop_desc pic_enable_desc;
/** @brief A function implementing the PIC_ENABLE() method */
typedef void pic_enable_t(device_t dev, u_int irq, u_int vector, void **priv);

static __inline void PIC_ENABLE(device_t dev, u_int irq, u_int vector,
                                void **priv)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_enable);
	((pic_enable_t *) _m)(dev, irq, vector, priv);
}

/** @brief Unique descriptor for the PIC_EOI() method */
extern struct kobjop_desc pic_eoi_desc;
/** @brief A function implementing the PIC_EOI() method */
typedef void pic_eoi_t(device_t dev, u_int irq, void *priv);

static __inline void PIC_EOI(device_t dev, u_int irq, void *priv)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_eoi);
	((pic_eoi_t *) _m)(dev, irq, priv);
}

/** @brief Unique descriptor for the PIC_IPI() method */
extern struct kobjop_desc pic_ipi_desc;
/** @brief A function implementing the PIC_IPI() method */
typedef void pic_ipi_t(device_t dev, u_int cpu);

static __inline void PIC_IPI(device_t dev, u_int cpu)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_ipi);
	((pic_ipi_t *) _m)(dev, cpu);
}

/** @brief Unique descriptor for the PIC_MASK() method */
extern struct kobjop_desc pic_mask_desc;
/** @brief A function implementing the PIC_MASK() method */
typedef void pic_mask_t(device_t dev, u_int irq, void *priv);

static __inline void PIC_MASK(device_t dev, u_int irq, void *priv)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_mask);
	((pic_mask_t *) _m)(dev, irq, priv);
}

/** @brief Unique descriptor for the PIC_UNMASK() method */
extern struct kobjop_desc pic_unmask_desc;
/** @brief A function implementing the PIC_UNMASK() method */
typedef void pic_unmask_t(device_t dev, u_int irq, void *priv);

static __inline void PIC_UNMASK(device_t dev, u_int irq, void *priv)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pic_unmask);
	((pic_unmask_t *) _m)(dev, irq, priv);
}

#endif /* _pic_if_h_ */
