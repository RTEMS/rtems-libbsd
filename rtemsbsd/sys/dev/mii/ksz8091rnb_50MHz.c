#include <machine/rtems-bsd-kernel-space.h>

/*-
 * Copyright (c) 1998, 1999 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe of the Numerical Aerospace Simulation Facility,
 * NASA Ames Research Center, and by Frank van der Linden.
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
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*-
 * Copyright (c) 1997 Manuel Bouyer.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

/*
 * A PHY driver for the KSZ8091RNB in a 50MHz configuration. This PHY needs an
 * extra bit set in one of it's config registers for the 50MHz mode. If you have
 * that hardware configuration, you can link in this driver.
 *
 * This driver is based on the ksz8091rnb.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/module.h>
#include <sys/bus.h>

#include <net/if.h>
#include <net/if_media.h>

#include <dev/mii/mii.h>
#include <dev/mii/miivar.h>
#include <rtems/bsd/local/miidevs.h>

#include <rtems/bsd/local/miibus_if.h>

#define MII_KSZ8091RNB_PHY_CTRL2 0x1F
#define MII_KSZ8091RNB_PHY_CTRL2_REFCLK_SELECT 0x0080

static int ksz8091rnb_probe(device_t);
static int ksz8091rnb_attach(device_t);

static device_method_t ksz8091rnb_methods[] = {
	/* device interface */
	DEVMETHOD(device_probe,		ksz8091rnb_probe),
	DEVMETHOD(device_attach,	ksz8091rnb_attach),
	DEVMETHOD(device_detach,	mii_phy_detach),
	DEVMETHOD(device_shutdown,	bus_generic_shutdown),
	DEVMETHOD_END
};

static driver_t ksz8091rnb_driver = {
	"ksz8091rnb",
	ksz8091rnb_methods,
	sizeof(struct mii_softc)
};

DRIVER_MODULE(ksz8091rnb, miibus, ksz8091rnb_driver, 0, 0);

static const struct mii_phydesc ksz8091rnbphys[] = {
	{
		.mpd_oui=MII_OUI_MICREL,
		.mpd_model=0x16,
		.mpd_name="Microchip KSZ8091RNB with 50MHz clock"
	},
	MII_PHY_END
};

static int	ksz8091rnb_service(struct mii_softc *, struct mii_data *, int);
static void	ksz8091rnb_phy_reset(struct mii_softc *);

static const struct mii_phy_funcs ksz8091rnb_funcs = {
	ksz8091rnb_service,
	ukphy_status,
	ksz8091rnb_phy_reset
};

static int
ksz8091rnb_probe(device_t dev)
{

	return (mii_phy_dev_probe(dev, ksz8091rnbphys, BUS_PROBE_DEFAULT));
}

static int
ksz8091rnb_attach(device_t dev)
{
	struct mii_softc *sc;

	sc = device_get_softc(dev);

	mii_phy_dev_attach(dev, MIIF_NOMANPAUSE | MIIF_NOISOLATE,
	    &ksz8091rnb_funcs, 1);
	mii_phy_setmedia(sc);

	return (0);
}

static void
ksz8091rnb_phy_reset(struct mii_softc *sc)
{
	uint16_t ctrl2;

	mii_phy_reset(sc);

	/* Set 50MHz */
	ctrl2 = PHY_READ(sc, MII_KSZ8091RNB_PHY_CTRL2);
	ctrl2 |= MII_KSZ8091RNB_PHY_CTRL2_REFCLK_SELECT;
	PHY_WRITE(sc, MII_KSZ8091RNB_PHY_CTRL2, ctrl2);
}

static int
ksz8091rnb_service(struct mii_softc *sc, struct mii_data *mii, int cmd)
{

	switch (cmd) {
	case MII_POLLSTAT:
		break;

	case MII_MEDIACHG:
		mii_phy_setmedia(sc);
		break;

	case MII_TICK:
		if (mii_phy_tick(sc) == EJUSTRETURN)
			return (0);
		break;
	}

	/* Update the media status. */
	PHY_STATUS(sc);

	/* Callback if something changed. */
	mii_phy_update(sc, cmd);
	return (0);
}
