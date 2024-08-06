#include <machine/rtems-bsd-kernel-space.h>

/*-
 * Copyright (c) 2008 Benno Rice
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
 *
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/bus.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/socket.h>
#include <sys/systm.h>
#include <sys/taskqueue.h>

#include <machine/bus.h>
#include <machine/resource.h>

#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/if_media.h>

#include <dev/smc/if_smcvar.h>

#include <dev/mii/mii.h>
#include <dev/mii/miivar.h>

#include <rtems/bsd/local/miibus_if.h>

static int
nexus_smc_probe(device_t dev)
{
	struct	smc_softc *sc;

	sc = device_get_softc(dev);
	sc->smc_usemem = 1;

	return (smc_probe(dev));
}

static device_method_t nexus_smc_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe,		nexus_smc_probe),
	DEVMETHOD(device_attach,	smc_attach),
	DEVMETHOD(device_detach,	smc_detach),

	/* MII interface */
	DEVMETHOD(miibus_readreg,	smc_miibus_readreg),
	DEVMETHOD(miibus_writereg,	smc_miibus_writereg),
	DEVMETHOD(miibus_statchg,	smc_miibus_statchg),

	{ 0, 0 }
};

static driver_t nexus_smc_driver = {
	"smc",
	nexus_smc_methods,
	sizeof(struct smc_softc),
};

DRIVER_MODULE(smc, nexus, nexus_smc_driver, 0, 0);
DRIVER_MODULE(miibus, smc, miibus_driver, 0, 0);
MODULE_DEPEND(smc, ether, 1, 1, 1);
MODULE_DEPEND(smc, miibus, 1, 1, 1);
