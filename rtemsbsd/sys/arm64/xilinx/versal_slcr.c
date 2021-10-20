#include <machine/rtems-bsd-kernel-space.h>

/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2021 Chris Johns <chrisj@rtems.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Versal ACAP SLCR driver.  Provides hook for CGEM clocks.
 *
 * Reference:
 *   https://www.xilinx.com/html_docs/registers/am012/mod___crl.html
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <bsp.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <sys/conf.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <rtems/bsd/sys/resource.h>
#include <sys/sysctl.h>
#include <sys/rman.h>

#include <machine/bus.h>
#include <machine/resource.h>
#include <machine/stdarg.h>

#include <arm64/xilinx/versal_slcr.h>

struct versal_slcr_softc {
	device_t	dev;
	struct mtx	sc_mtx;
	struct resource	*mem_res;
};

static struct versal_slcr_softc *versal_slcr_softc_p;

#define RD4(sc, off) 		(bus_read_4((sc)->mem_res, (off)))
#define WR4(sc, off, val) 	(bus_write_4((sc)->mem_res, (off), (val)))

SYSCTL_NODE(_hw, OID_AUTO, versal, CTLFLAG_RD, 0, "Xilinx Versal ACAP SLCR");

#if defined(LIBBSP_AARCH64_XILINX_VERSAL_BSP_H)
/* Override cgem_set_refclk() in gigabit ethernet driver
 * (sys/dev/cadence/if_cgem.c).  This function is called to
 * request a change in the gem's reference clock speed.
 */
int
cgem_set_ref_clk(int unit, int frequency)
{
	struct versal_slcr_softc *sc = versal_slcr_softc_p;
	int div, last_error = 0;
	uint64_t clk_ctrl, pll_ctrl;
	uint32_t clk_ctrl_val, pll_ctrl_val, pll_freq, pll_reset, pll_bypass;

	if (!sc)
		return (-1);

	switch (unit) {
		case 0:
			clk_ctrl = VERSAL_SLCR_GEM0_CLK_CTRL;
			break;
		case 1:
			clk_ctrl = VERSAL_SLCR_GEM1_CLK_CTRL;
			break;
		default:
			return (-1);
	}

	clk_ctrl_val = RD4(sc, clk_ctrl);
	switch (clk_ctrl_val & VERSAL_SLCR_GEM_CLK_CTRL_SRCSEL_MASK) {
		case VERSAL_SLCR_GEM_CLK_CTRL_SRCSEL_P_PLL:
			pll_ctrl = VERSAL_SLCR_P_PLL_CTRL;
			break;
		case VERSAL_SLCR_GEM_CLK_CTRL_SRCSEL_R_PLL:
			pll_ctrl = VERSAL_SLCR_R_PLL_CTRL;
			break;
		case VERSAL_SLCR_GEM_CLK_CTRL_SRCSEL_N_PLL:
			pll_ctrl = VERSAL_SLCR_N_PLL_CTRL;
			break;
		default:
			return (-1);
	}

	/* Get PLL frequency */
	pll_ctrl_val = RD4(sc, pll_ctrl);
	pll_reset = (pll_ctrl_val & VERSAL_SLCR_PLL_CTRL_RESET) != 0;
	pll_bypass = (pll_ctrl_val & VERSAL_SLCR_PLL_CTRL_BYPASS) != 0;
	if (pll_reset && !pll_bypass) {
		return 0;
	}

	pll_freq = VERSAL_DEFAULT_PS_CLK_FREQUENCY;
	if (!pll_bypass) {
		pll_freq *= (pll_ctrl_val & VERSAL_SLCR_PLL_CTRL_FBDIV_MASK) >> VERSAL_SLCR_PLL_CTRL_FBDIV_SHIFT;
	}

	/* Apply divider */
  pll_freq >>= (pll_ctrl_val & VERSAL_SLCR_PLL_CTRL_DIV_MASK) >> VERSAL_SLCR_PLL_CTRL_DIV_SHIFT;

	/* Find suitable divisor. Linear search, not the fastest method but hey.
	 */
	for (div = 1; div <= VERSAL_SLCR_GEM_CLK_CTRL_DIVISOR_MAX; div++) {
    int div_freq = pll_freq / div;
		int error = abs(frequency - div_freq);
		if (error >= last_error && last_error != 0) {
      div--;
			break;
		}
		last_error = error;
	}

	if (div > VERSAL_SLCR_GEM_CLK_CTRL_DIVISOR_MAX)
		return (-1);

	/* Modify GEM reference clock. */
	clk_ctrl_val &= ~VERSAL_SLCR_GEM_CLK_CTRL_DIVISOR_MASK;
	clk_ctrl_val |= div << VERSAL_SLCR_GEM_CLK_CTRL_DIVISOR_SHIFT;
	WR4(sc, clk_ctrl, clk_ctrl_val);

	return (0);
}
#endif

static int
versal_slcr_probe(device_t dev)
{
	device_set_desc(dev, "Versal ACAP SLCR block");
	return (0);
}

static int
versal_slcr_attach(device_t dev)
{
	struct versal_slcr_softc *sc = device_get_softc(dev);
	int rid;

	/* Allow only one attach. */
	if (versal_slcr_softc_p != NULL)
		return (ENXIO);

	sc->dev = dev;

	/* Get memory resource. */
	rid = 0;
	sc->mem_res = bus_alloc_resource_any(dev, SYS_RES_MEMORY, &rid,
					     RF_ACTIVE);
	if (sc->mem_res == NULL) {
		device_printf(dev, "could not allocate memory resources.\n");
		return (ENOMEM);
	}

	/* For use with CGEM clock setting */
	versal_slcr_softc_p = sc;

	return (0);
}

static int
versal_slcr_detach(device_t dev)
{
	struct versal_slcr_softc *sc = device_get_softc(dev);

	bus_generic_detach(dev);

	/* Release memory resource. */
	if (sc->mem_res != NULL)
		bus_release_resource(dev, SYS_RES_MEMORY,
			     rman_get_rid(sc->mem_res), sc->mem_res);

	versal_slcr_softc_p = NULL;

	return (0);
}

static device_method_t versal_slcr_methods[] = {
	/* device_if */
	DEVMETHOD(device_probe, 	versal_slcr_probe),
	DEVMETHOD(device_attach, 	versal_slcr_attach),
	DEVMETHOD(device_detach, 	versal_slcr_detach),

	DEVMETHOD_END
};

static driver_t versal_slcr_driver = {
	"versal_slcr",
	versal_slcr_methods,
	sizeof(struct versal_slcr_softc),
};
static devclass_t versal_slcr_devclass;

DRIVER_MODULE(versal_slcr, nexus, versal_slcr_driver, versal_slcr_devclass, 0, 0);
MODULE_VERSION(versal_slcr, 1);
