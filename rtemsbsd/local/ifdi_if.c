#include <machine/rtems-bsd-kernel-space.h>

/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/net/ifdi_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/socket.h>
#include <machine/bus.h>
#include <sys/bus.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_var.h>
#include <net/if_media.h>
#include <net/iflib.h>
#include <net/if_private.h>
#include <rtems/bsd/local/ifdi_if.h>



	static void
	null_void_op(if_ctx_t _ctx __unused)
	{
	}

	static void
	null_timer_op(if_ctx_t _ctx __unused, uint16_t _qsidx __unused)
	{
	}

	static int
	null_int_op(if_ctx_t _ctx __unused)
	{
		return (0);
	}

	static int
	null_queue_intr_enable(if_ctx_t _ctx __unused, uint16_t _qid __unused)
	{
		return (ENOTSUP);
	}

	static void
	null_led_func(if_ctx_t _ctx __unused, int _onoff __unused)
	{
	}

	static void
	null_vlan_register_op(if_ctx_t _ctx __unused, uint16_t vtag __unused)
	{
	}

	static int
	null_q_setup(if_ctx_t _ctx __unused, uint32_t _qid __unused)
	{
		return (0);
	}

	static int
	null_i2c_req(if_ctx_t _sctx __unused, struct ifi2creq *_i2c __unused)
	{
		return (ENOTSUP);
	}

	static int
	null_sysctl_int_delay(if_ctx_t _sctx __unused, if_int_delay_info_t _iidi __unused)
	{
		return (0);
	}

	static int
	null_iov_init(if_ctx_t _ctx __unused, uint16_t num_vfs __unused, const nvlist_t *params __unused)
	{
		return (ENOTSUP);
	}

	static int
	null_vf_add(if_ctx_t _ctx __unused, uint16_t num_vfs __unused, const nvlist_t *params __unused)
	{
		return (ENOTSUP);
	}

	static int
	null_priv_ioctl(if_ctx_t _ctx __unused, u_long command, caddr_t *data __unused)
	{
		return (ENOTSUP);
	}

	static bool
	null_needs_restart(if_ctx_t _ctx __unused, enum iflib_restart_event _event __unused)
	{
		return (false);
	}

struct kobjop_desc ifdi_attach_pre_desc = {
	0, { &ifdi_attach_pre_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ifdi_attach_post_desc = {
	0, { &ifdi_attach_post_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ifdi_reinit_pre_desc = {
	0, { &ifdi_reinit_pre_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ifdi_reinit_post_desc = {
	0, { &ifdi_reinit_post_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ifdi_detach_desc = {
	0, { &ifdi_detach_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ifdi_suspend_desc = {
	0, { &ifdi_suspend_desc, (kobjop_t)null_int_op }
};

struct kobjop_desc ifdi_shutdown_desc = {
	0, { &ifdi_shutdown_desc, (kobjop_t)null_int_op }
};

struct kobjop_desc ifdi_resume_desc = {
	0, { &ifdi_resume_desc, (kobjop_t)null_int_op }
};

struct kobjop_desc ifdi_tx_queues_alloc_desc = {
	0, { &ifdi_tx_queues_alloc_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ifdi_rx_queues_alloc_desc = {
	0, { &ifdi_rx_queues_alloc_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ifdi_queues_free_desc = {
	0, { &ifdi_queues_free_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ifdi_init_desc = {
	0, { &ifdi_init_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ifdi_stop_desc = {
	0, { &ifdi_stop_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ifdi_msix_intr_assign_desc = {
	0, { &ifdi_msix_intr_assign_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ifdi_intr_enable_desc = {
	0, { &ifdi_intr_enable_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ifdi_intr_disable_desc = {
	0, { &ifdi_intr_disable_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ifdi_rx_queue_intr_enable_desc = {
	0, { &ifdi_rx_queue_intr_enable_desc, (kobjop_t)null_queue_intr_enable }
};

struct kobjop_desc ifdi_tx_queue_intr_enable_desc = {
	0, { &ifdi_tx_queue_intr_enable_desc, (kobjop_t)null_queue_intr_enable }
};

struct kobjop_desc ifdi_link_intr_enable_desc = {
	0, { &ifdi_link_intr_enable_desc, (kobjop_t)null_void_op }
};

struct kobjop_desc ifdi_admin_completion_handle_desc = {
	0, { &ifdi_admin_completion_handle_desc, (kobjop_t)null_void_op }
};

struct kobjop_desc ifdi_multi_set_desc = {
	0, { &ifdi_multi_set_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ifdi_mtu_set_desc = {
	0, { &ifdi_mtu_set_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ifdi_media_set_desc = {
	0, { &ifdi_media_set_desc, (kobjop_t)null_void_op }
};

struct kobjop_desc ifdi_promisc_set_desc = {
	0, { &ifdi_promisc_set_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ifdi_crcstrip_set_desc = {
	0, { &ifdi_crcstrip_set_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ifdi_vflr_handle_desc = {
	0, { &ifdi_vflr_handle_desc, (kobjop_t)null_void_op }
};

struct kobjop_desc ifdi_iov_init_desc = {
	0, { &ifdi_iov_init_desc, (kobjop_t)null_iov_init }
};

struct kobjop_desc ifdi_iov_uninit_desc = {
	0, { &ifdi_iov_uninit_desc, (kobjop_t)null_void_op }
};

struct kobjop_desc ifdi_iov_vf_add_desc = {
	0, { &ifdi_iov_vf_add_desc, (kobjop_t)null_vf_add }
};

struct kobjop_desc ifdi_update_admin_status_desc = {
	0, { &ifdi_update_admin_status_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ifdi_media_status_desc = {
	0, { &ifdi_media_status_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ifdi_media_change_desc = {
	0, { &ifdi_media_change_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ifdi_get_counter_desc = {
	0, { &ifdi_get_counter_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ifdi_priv_ioctl_desc = {
	0, { &ifdi_priv_ioctl_desc, (kobjop_t)null_priv_ioctl }
};

struct kobjop_desc ifdi_i2c_req_desc = {
	0, { &ifdi_i2c_req_desc, (kobjop_t)null_i2c_req }
};

struct kobjop_desc ifdi_txq_setup_desc = {
	0, { &ifdi_txq_setup_desc, (kobjop_t)null_q_setup }
};

struct kobjop_desc ifdi_rxq_setup_desc = {
	0, { &ifdi_rxq_setup_desc, (kobjop_t)null_q_setup }
};

struct kobjop_desc ifdi_timer_desc = {
	0, { &ifdi_timer_desc, (kobjop_t)null_timer_op }
};

struct kobjop_desc ifdi_watchdog_reset_desc = {
	0, { &ifdi_watchdog_reset_desc, (kobjop_t)null_void_op }
};

struct kobjop_desc ifdi_led_func_desc = {
	0, { &ifdi_led_func_desc, (kobjop_t)null_led_func }
};

struct kobjop_desc ifdi_vlan_register_desc = {
	0, { &ifdi_vlan_register_desc, (kobjop_t)null_vlan_register_op }
};

struct kobjop_desc ifdi_vlan_unregister_desc = {
	0, { &ifdi_vlan_unregister_desc, (kobjop_t)null_vlan_register_op }
};

struct kobjop_desc ifdi_sysctl_int_delay_desc = {
	0, { &ifdi_sysctl_int_delay_desc, (kobjop_t)null_sysctl_int_delay }
};

struct kobjop_desc ifdi_debug_desc = {
	0, { &ifdi_debug_desc, (kobjop_t)null_void_op }
};

struct kobjop_desc ifdi_needs_restart_desc = {
	0, { &ifdi_needs_restart_desc, (kobjop_t)null_needs_restart }
};

