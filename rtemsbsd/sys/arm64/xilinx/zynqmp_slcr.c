#include <machine/rtems-bsd-kernel-space.h>

/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2021 Kinsey Moore
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
 *
 * $FreeBSD$
 */

/*
 * Zynq Ultrascale+ MPSoC SLCR driver.  Provides hook for CGEM clocks.
 *
 * Reference: Zynq Ultrascale+ MPSoC Technical Reference Manual.
 * (v2.2) December 4, 2020.  Xilinx doc UG1085.
 */

#include <sys/cdefs.h>

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

#include <arm64/xilinx/zynqmp_slcr.h>

struct zynqmp_slcr_softc {
	device_t	dev;
	struct mtx	sc_mtx;
	struct resource	*mem_res;
};

static struct zynqmp_slcr_softc *zynqmp_slcr_softc_p;

#define RD4(sc, off) 		(bus_read_4((sc)->mem_res, (off)))
#define WR4(sc, off, val) 	(bus_write_4((sc)->mem_res, (off), (val)))

SYSCTL_NODE(_hw, OID_AUTO, zynqmp, CTLFLAG_RD, 0, "Xilinx Zynq Ultrascale+ MPSoC");

#if defined(LIBBSP_AARCH64_XILINX_ZYNQMP_BSP_H)
/* Override cgem_set_refclk() in gigabit ethernet driver
 * (sys/dev/cadence/if_cgem.c).  This function is called to
 * request a change in the gem's reference clock speed.
 */
int
cgem_set_ref_clk(int unit, int frequency)
{
	struct zynqmp_slcr_softc *sc = zynqmp_slcr_softc_p;
	int div0, div1;
	uint64_t clk_ctrl, pll_ctrl;
	uint32_t clk_ctrl_val, pll_ctrl_val, pll_freq, pll_bypass;

	if (!sc)
		return (-1);

	switch (unit) {
		case 0:
			clk_ctrl = ZYNQMP_SLCR_GEM0_CLK_CTRL;
			break;
		case 1:
			clk_ctrl = ZYNQMP_SLCR_GEM1_CLK_CTRL;
			break;
		case 2:
			clk_ctrl = ZYNQMP_SLCR_GEM2_CLK_CTRL;
			break;
		case 3:
			clk_ctrl = ZYNQMP_SLCR_GEM3_CLK_CTRL;
			break;
		default:
			return (-1);
	}

	clk_ctrl_val = RD4(sc, clk_ctrl);
	switch (clk_ctrl_val & ZYNQMP_SLCR_GEM_CLK_CTRL_SRCSEL_MASK) {
		case 0:
			pll_ctrl = ZYNQMP_SLCR_IO_PLL_CTRL;
			break;
		case 2:
			pll_ctrl = ZYNQMP_SLCR_R_PLL_CTRL;
			break;
		case 3:
			pll_ctrl = ZYNQMP_SLCR_D_PLL_CTRL;
			break;
		default:
			return (-1);
	}

	/* Get PLL frequency */
	pll_ctrl_val = RD4(sc, pll_ctrl);
	pll_bypass = pll_ctrl_val & ZYNQMP_SLCR_PLL_CTRL_BYPASS;
	if ((pll_ctrl_val & ZYNQMP_SLCR_PLL_CTRL_RESET) && !pll_bypass) {
		return 0;
	}

	pll_freq = ZYNQMP_DEFAULT_PS_CLK_FREQUENCY;
	if (!pll_bypass) {
		pll_freq *= (pll_ctrl_val & ZYNQMP_SLCR_PLL_CTRL_FBDIV_MASK) >> ZYNQMP_SLCR_PLL_CTRL_FBDIV_SHIFT;
	}

	/* Divide by 2 if necessary */
	pll_freq >>= !!(pll_ctrl_val & ZYNQMP_SLCR_PLL_CTRL_DIV2);

	/* Find suitable divisor pairs.  Round result to nearest khz
	 * to test for match.
	 */
	for (div1 = 1; div1 <= ZYNQMP_SLCR_GEM_CLK_CTRL_DIVISOR1_MAX; div1++) {
		div0 = (pll_freq + div1 * frequency / 2) /
			div1 / frequency;
		if (div0 > 0 && div0 <= ZYNQMP_SLCR_GEM_CLK_CTRL_DIVISOR0_MAX &&
		    ((pll_freq / div0 / div1) + 500) / 1000 ==
		    (frequency + 500) / 1000)
			break;
	}

	if (div1 > ZYNQMP_SLCR_GEM_CLK_CTRL_DIVISOR1_MAX)
		return (-1);

	/* Modify GEM reference clock. */
	clk_ctrl_val &= ~ZYNQMP_SLCR_GEM_CLK_CTRL_DIVISOR1_MASK;
	clk_ctrl_val &= ~ZYNQMP_SLCR_GEM_CLK_CTRL_DIVISOR0_MASK;
	clk_ctrl_val |= div1 << ZYNQMP_SLCR_GEM_CLK_CTRL_DIVISOR1_SHIFT;
	clk_ctrl_val |= div0 << ZYNQMP_SLCR_GEM_CLK_CTRL_DIVISOR0_SHIFT;
	WR4(sc, clk_ctrl, clk_ctrl_val);

	return (0);
}
#endif

static int
zynqmp_slcr_probe(device_t dev)
{

	device_set_desc(dev, "Zynq Ultrascale+ MPSoC SLCR block");
	return (0);
}

static int
zynqmp_slcr_attach(device_t dev)
{
	struct zynqmp_slcr_softc *sc = device_get_softc(dev);
	int rid;

	/* Allow only one attach. */
	if (zynqmp_slcr_softc_p != NULL)
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
	zynqmp_slcr_softc_p = sc;

	return (0);
}

static int
zynqmp_slcr_detach(device_t dev)
{
	struct zynqmp_slcr_softc *sc = device_get_softc(dev);

	bus_generic_detach(dev);

	/* Release memory resource. */
	if (sc->mem_res != NULL)
		bus_release_resource(dev, SYS_RES_MEMORY,
			     rman_get_rid(sc->mem_res), sc->mem_res);

	zynqmp_slcr_softc_p = NULL;

	return (0);
}

static device_method_t zynqmp_slcr_methods[] = {
	/* device_if */
	DEVMETHOD(device_probe, 	zynqmp_slcr_probe),
	DEVMETHOD(device_attach, 	zynqmp_slcr_attach),
	DEVMETHOD(device_detach, 	zynqmp_slcr_detach),

	DEVMETHOD_END
};

static driver_t zynqmp_slcr_driver = {
	"zynqmp_slcr",
	zynqmp_slcr_methods,
	sizeof(struct zynqmp_slcr_softc),
};

DRIVER_MODULE(zynqmp_slcr, nexus, zynqmp_slcr_driver, 0, 0);
MODULE_VERSION(zynqmp_slcr, 1);
