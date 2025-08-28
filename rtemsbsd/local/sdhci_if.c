#include <machine/rtems-bsd-kernel-space.h>
/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/sdhci/sdhci_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/taskqueue.h>
#include <machine/bus.h>
#include <dev/mmc/bridge.h>
#include <dev/sdhci/sdhci.h>

static int
null_set_clock(device_t brdev __unused,
    struct sdhci_slot *slot __unused,
    int clock)
{
	return (clock);
}

static void
null_set_uhs_timing(device_t brdev __unused,
    struct sdhci_slot *slot __unused)
{

}

#include <rtems/bsd/local/sdhci_if.h>

struct kobjop_desc sdhci_read_1_desc = {
	0, { &sdhci_read_1_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc sdhci_read_2_desc = {
	0, { &sdhci_read_2_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc sdhci_read_4_desc = {
	0, { &sdhci_read_4_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc sdhci_read_multi_4_desc = {
	0, { &sdhci_read_multi_4_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc sdhci_write_1_desc = {
	0, { &sdhci_write_1_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc sdhci_write_2_desc = {
	0, { &sdhci_write_2_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc sdhci_write_4_desc = {
	0, { &sdhci_write_4_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc sdhci_write_multi_4_desc = {
	0, { &sdhci_write_multi_4_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc sdhci_platform_will_handle_desc = {
	0, { &sdhci_platform_will_handle_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc sdhci_platform_start_transfer_desc = {
	0, { &sdhci_platform_start_transfer_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc sdhci_platform_finish_transfer_desc = {
	0, { &sdhci_platform_finish_transfer_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc sdhci_min_freq_desc = {
	0, { &sdhci_min_freq_desc, (kobjop_t)sdhci_generic_min_freq }
};

struct kobjop_desc sdhci_get_card_present_desc = {
	0, { &sdhci_get_card_present_desc, (kobjop_t)sdhci_generic_get_card_present }
};

struct kobjop_desc sdhci_set_uhs_timing_desc = {
	0, { &sdhci_set_uhs_timing_desc, (kobjop_t)null_set_uhs_timing }
};

struct kobjop_desc sdhci_set_clock_desc = {
	0, { &sdhci_set_clock_desc, (kobjop_t)null_set_clock }
};

struct kobjop_desc sdhci_reset_desc = {
	0, { &sdhci_reset_desc, (kobjop_t)sdhci_generic_reset }
};

