#include <machine/rtems-bsd-kernel-space.h>

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

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <sys/bus.h>
#include <sys/cpuset.h>
#include <machine/frame.h>
#include <rtems/bsd/local/pic_if.h>


static pic_translate_code_t pic_translate_code_default;

static void pic_translate_code_default(device_t dev, u_int irq,
    int code, enum intr_trigger *trig, enum intr_polarity *pol)
{
	*trig = INTR_TRIGGER_CONFORM;
	*pol = INTR_POLARITY_CONFORM;
}

struct kobjop_desc pic_bind_desc = {
	0, { &pic_bind_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pic_translate_code_desc = {
	0, { &pic_translate_code_desc, (kobjop_t)pic_translate_code_default }
};

struct kobjop_desc pic_config_desc = {
	0, { &pic_config_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pic_dispatch_desc = {
	0, { &pic_dispatch_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pic_enable_desc = {
	0, { &pic_enable_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pic_eoi_desc = {
	0, { &pic_eoi_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pic_ipi_desc = {
	0, { &pic_ipi_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pic_mask_desc = {
	0, { &pic_mask_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pic_unmask_desc = {
	0, { &pic_unmask_desc, (kobjop_t)kobj_error_method }
};

