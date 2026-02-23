#include <machine/rtems-bsd-kernel-space.h>
/*
 * Copyright (c) 2026 Aaron Nyholm <aaron.nyholm@unfoldedeffective.com>.
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

/* Driver for Northwest Logic AXI-PCIe bridge */

#include <rtems/bsd/local/opt_platform.h>

#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/sysctl.h>
#include <sys/kernel.h>
#include <sys/rman.h>
#include <sys/module.h>
#include <sys/bus.h>
#include <sys/endian.h>
#include <sys/mutex.h>
#include <sys/proc.h>

#include <machine/intr.h>
#include <machine/bus.h>

#include <dev/ofw/openfirm.h>
#include <dev/ofw/ofw_bus.h>
#include <dev/ofw/ofw_bus_subr.h>

#include <dev/pci/pcireg.h>
#include <dev/pci/pcivar.h>
#include <dev/pci/pci_host_generic.h>
#include <dev/pci/pci_host_generic_fdt.h>
#include <dev/pci/pcib_private.h>

#include "xlnx_nwl_pcib.h"

#include <rtems/bsd/local/ofw_bus_if.h>
#include <rtems/bsd/local/msi_if.h>
#include <rtems/bsd/local/pcib_if.h>
#include <rtems/bsd/local/pic_if.h>

struct xlnx_nwl_pcib_softc {
    struct generic_pcie_fdt_softc base;
    struct mtx msi_mtx;
    struct resource* breg;
    struct resource* pcireg;
    struct resource* cfg;
    struct resource* msi_intr[2];
    struct resource* misc_intr;
    struct xlnx_nwl_pcib_irqsrc* isrcs;
    device_t dev;
    void* intr_cookie[3];
};

struct xlnx_nwl_pcib_irqsrc {
    struct intr_irqsrc isrc;
    u_int irq;
    bool allocated;
};

static struct ofw_compat_data compat_data[] = {
    { "xlnx,nwl-pcie-2.11", 1 },
    { NULL, 0 },
};

static void xlnx_nwl_pcib_msi_mask(device_t dev, struct intr_irqsrc *isrc, bool mask);
static uint32_t xlnx_nwl_pcib_read_config(device_t dev, u_int bus, u_int slot,
    u_int func, u_int reg, int bytes);
static void xlnx_nwl_pcib_write_config(device_t dev, u_int bus, u_int slot,
    u_int func, u_int reg, uint32_t val, int bytes);

static int xlnx_nwl_pcib_fdt_probe(device_t dev) {
    if (!ofw_bus_status_okay(dev))
        return (ENXIO);

    if (ofw_bus_search_compatible(dev, compat_data)->ocd_data == 0)
        return (ENXIO);

    device_set_desc(dev, "Xilinx NWL PCIe Controller");

    return (BUS_PROBE_DEFAULT);
}

static int xlnx_nwl_pcib_locate_bridge_window(device_t dev,
    struct xlnx_nwl_pcib_softc* sc) {
    struct generic_pcie_fdt_softc* fdt_sc;
    struct generic_pcie_core_softc* core_sc;
    uint16_t reg;
    pci_addr_t base;
    pci_addr_t limit;

    fdt_sc = &sc->base;
    core_sc = &fdt_sc->base;

    base = core_sc->ranges[0].pci_base;
    reg = (uint16_t)((base >> 16) & 0xFFFF);
    xlnx_nwl_pcib_write_config(dev, 0, 0, 0, PCIR_MEMBASE_1, reg, 2);

    limit = base + core_sc->ranges[0].size - 1;
    reg = (uint16_t)((limit >> 16) & 0xFFFF);
    xlnx_nwl_pcib_write_config(dev, 0, 0, 0, PCIR_MEMLIMIT_1, reg, 2);

    base = core_sc->ranges[1].pci_base;
    reg = (uint16_t)((base >> 16) & 0xFFFF);
    xlnx_nwl_pcib_write_config(dev, 0, 0, 0, PCIR_PMBASEL_1, reg, 2);
    xlnx_nwl_pcib_write_config(dev, 0, 0, 0, PCIR_PMBASEH_1, (base >> 32), 4);

    limit = base + core_sc->ranges[1].size - 1;
    reg = (uint16_t)((limit >> 16) & 0xFFFF);
    xlnx_nwl_pcib_write_config(dev, 0, 0, 0, PCIR_PMLIMITL_1, reg, 2);
    xlnx_nwl_pcib_write_config(dev, 0, 0, 0, PCIR_PMLIMITH_1, (limit >> 32), 4);
}

static int xlnx_nwl_pcib_init(device_t dev, struct xlnx_nwl_pcib_softc* sc) {
    rman_res_t breg_base;
    rman_res_t ecam_base;
    rman_res_t ecam_size;
    uint32_t reg;
    uint32_t shift;

    /* Map the bridge register aperture. */
    breg_base = rman_get_start(sc->breg);
    bus_write_4(sc->breg, XLNX_NWLP_E_BREG_BASE_HI, (breg_base >> 32));
    bus_write_4(sc->breg, XLNX_NWLP_E_BREG_BASE_LO, (breg_base >>  0));

    bus_write_4(sc->breg, XLNX_NWLP_E_BREG_CONTROL,
        XLNX_NWLP_E_BREG_CTRL_ENA & (~XLNX_NWLP_E_BREG_CTRL_ENA_FORCE));

    /* Map the ECAM space. */
    ecam_base = rman_get_start(sc->cfg);
    bus_write_4(sc->breg, XLNX_NWLP_E_ECAM_BASE_HI, (ecam_base >> 32));
    bus_write_4(sc->breg, XLNX_NWLP_E_ECAM_BASE_LO, (ecam_base >>  0));

    ecam_size = rman_get_size(sc->cfg);
    for (shift = 0; (1U << shift) <= ecam_size; shift++);
    reg = bus_read_4(sc->breg, XLNX_NWLP_E_ECAM_CAPS);
    shift = shift -
        ((reg & XLNX_NWLP_E_ECAM_CAPS_SIZE_OFF_MASK)
        >> XLNX_NWLP_E_ECAM_CAPS_SIZE_OFF_SHIFT);
    if (shift > ((reg & XLNX_NWLP_E_ECAM_CAPS_SIZE_MAX_MASK) >>
            XLNX_NWLP_E_ECAM_CAPS_SIZE_MAX_SHIFT)) {
        device_printf(dev, "ECAM space too large\n");
        return ENXIO;
    }
    shift = ((shift << XLNX_NWLP_E_ECAM_CTRL_SIZE_SHIFT)
        & XLNX_NWLP_E_ECAM_CTRL_SIZE_MASK) | XLNX_NWLP_E_ECAM_CTRL_ENA;
    bus_write_4(sc->breg, XLNX_NWLP_E_ECAM_CONTROL, shift);

    /* Disable DMA register access from Endpoint */
    bus_write_4(sc->breg, XLNX_NWLP_CCFG_PCIE_RX0,
        XLNX_NWLP_CCFG_PCIE_RX0_DMA_BAR_NONE
        | XLNX_NWLP_CCFG_PCIE_RX0_PCIE_NO_BREG
        | XLNX_NWLP_CCFG_PCIE_RX0_PCIE_NO_DMA);

    /* Enable ingress subtractive decode */
    bus_write_4(sc->breg, XLNX_NWLP_I_ISUB_CONTROL,
        XLNX_NWLP_I_ISUB_CTRL_ENA);

    /* Disable, clear and enable interrupts */
    bus_write_4(sc->breg, XLNX_NWLP_MSGF_MISC_MASK, 0x00000000);
    bus_write_4(sc->breg, XLNX_NWLP_MSGF_MISC_STATUS, 0xFFFFFFFF);
    bus_write_4(sc->breg, XLNX_NWLP_MSGF_MISC_MASK, 
        XLNX_NWLP_MISC_RX_MSG_AVAIL
        | XLNX_NWLP_MISC_RX_MSG_OVER
        | XLNX_NWLP_MISC_UC_WRITE
        | XLNX_NWLP_MISC_SLAVE
        | XLNX_NWLP_MISC_MASTER
        | XLNX_NWLP_MISC_I_ADDR_TRANS
        | XLNX_NWLP_MISC_E_ADDR_TRANS
        | XLNX_NWLP_MISC_LINK_DOWN
    );

    return 0;
}

static int xlnx_nwl_pcib_register_msi(struct xlnx_nwl_pcib_softc* sc) {
    const char* name;
    int error;
    int irq;

    sc->isrcs = malloc(sizeof(*sc->isrcs) * XLNX_NWLP_MSI_MAX,
        M_DEVBUF, M_WAITOK | M_ZERO);

    name = device_get_nameunit(sc->dev);

    for (irq = 0; irq < XLNX_NWLP_MSI_MAX; irq++) {
        sc->isrcs[irq].irq = irq;
        error = intr_isrc_register(&sc->isrcs[irq].isrc, sc->dev, 0, "%s,%u",
            name, irq);
        if (error != 0) {
            device_printf(sc->dev, "failed to register MSI interrupt: %d\n", error);
            return error;
        }
    }

    error = intr_msi_register(sc->dev,
        OF_xref_from_node(ofw_bus_get_node(sc->dev)));
    if (error != 0) {
        device_printf(sc->dev, "failed to register MSI: %d\n", error);
        return error;
    }

    /* Setup ingress MSI aperture. */
    bus_write_4(sc->breg, XLNX_NWLP_I_MSII_BASE_HI, 0x00000000);
    bus_write_4(sc->breg, XLNX_NWLP_I_MSII_BASE_LO, XLNX_NWLP_MSI_ADDR);

    /* Enable MSI */
    bus_write_4(sc->breg, XLNX_NWLP_I_MSII_CONTROL,
        XLNX_NWLP_I_MSII_CTRL_ENA | XLNX_NWLP_I_MSII_CTRL_STS_ENA);

    bus_write_4(sc->breg, XLNX_NWLP_MSGF_MSI_MASK_HI, 0x00000000);
    bus_write_4(sc->breg, XLNX_NWLP_MSGF_MSI_STATUS_HI, 0xFFFFFFFF);

    return 0;
}

static void xlnx_nwl_pcib_msi_intr(void* arg, int msireg) {
    struct xlnx_nwl_pcib_softc* sc;
    struct xlnx_nwl_pcib_irqsrc* irq_i;
    struct trapframe* tf;
    uint32_t reg;
    int i;
    int irq;

    sc = arg;
#ifndef __rtems__
    tf = curthread->td_intr_frame;
#else /* __rtems__ */
    tf = NULL;
#endif

    do {
        reg = bus_read_4(sc->breg, msireg);

        for (i = 0; i < 32; ++i) {
            if (reg & (1U << i)) {
                bus_write_4(sc->breg, msireg, (1U << i));

                irq = i;
                if (msireg == XLNX_NWLP_MSGF_MSI_STATUS_HI) {
                    irq += 32;
                }

                irq_i = &sc->isrcs[irq];
                if (intr_isrc_dispatch(&irq_i->isrc, tf) != 0) {
                    xlnx_nwl_pcib_msi_mask(sc->dev, &irq_i->isrc, true);
                    device_printf(sc->dev, "disabled irq %u: stray\n", irq);
                }
            }
        }
    } while (reg != 0);
}

static int xlnx_nwl_pcib_msi0_intr(void* arg) {
    xlnx_nwl_pcib_msi_intr(arg, XLNX_NWLP_MSGF_MSI_STATUS_LO);
    return FILTER_HANDLED;
}

static int xlnx_nwl_pcib_msi1_intr(void* arg) {
    xlnx_nwl_pcib_msi_intr(arg, XLNX_NWLP_MSGF_MSI_STATUS_HI);
    return FILTER_HANDLED;
}

static void xlnx_nwl_pcib_msgs_read(struct xlnx_nwl_pcib_softc* sc) {
    uint32_t i;
    uint32_t count;
    uint32_t reg;

    count = bus_read_4(sc->breg, XLNX_NWLP_MSGF_RX_FIFO_LEVEL)
        & XLNX_NWLP_MSGF_RX_FIFO_LEVEL_MASK;

    for (i = 0; i < count; ++i) {
        device_printf(sc->dev, "---- MSG received ----");
        reg = bus_read_4(sc->breg, XLNX_NWLP_MSGF_RX_FIFO_TYPE);
        device_printf(sc->dev, "MSG type: 0x%08x\n", reg);
        reg = bus_read_4(sc->breg, XLNX_NWLP_MSGF_RX_FIFO_MSG);
        device_printf(sc->dev, "MSG msg: 0x%08x\n", reg);
        reg = bus_read_4(sc->breg, XLNX_NWLP_MSGF_RX_FIFO_ADDR_LO);
        device_printf(sc->dev, "MSG address low: 0x%08x\n", reg);
        reg = bus_read_4(sc->breg, XLNX_NWLP_MSGF_RX_FIFO_ADDR_HI);
        device_printf(sc->dev, "MSG address high: 0x%08x\n", reg);
        reg = bus_read_4(sc->breg, XLNX_NWLP_MSGF_RX_FIFO_DATA);
        device_printf(sc->dev, "MSG data: 0x%08x\n", reg);
        bus_write_4(sc->breg, XLNX_NWLP_MSGF_RX_FIFO_POP,
            XLNX_NWLP_MSGF_RX_FIFO_POP_MASK);
    }
}

static int xlnx_nwl_pcib_misc_intr(void* arg) {
    struct xlnx_nwl_pcib_softc* sc;
    uint32_t reg;

    sc = arg;

    reg = bus_read_4(sc->breg, XLNX_NWLP_MSGF_MISC_STATUS);

    if (reg & XLNX_NWLP_MISC_RX_MSG_AVAIL) {
        xlnx_nwl_pcib_msgs_read(sc);
        bus_write_4(sc->breg, XLNX_NWLP_MSGF_MISC_STATUS,
            XLNX_NWLP_MISC_RX_MSG_AVAIL);
    }

    if (reg & XLNX_NWLP_MISC_RX_MSG_OVER) {
        device_printf(sc->dev, "MSG overflow\n");
        bus_write_4(sc->breg, XLNX_NWLP_MSGF_MISC_STATUS,
            XLNX_NWLP_MISC_RX_MSG_OVER);
    }

    if (reg & XLNX_NWLP_MISC_UC_WRITE) {
        device_printf(sc->dev, "Uncorrectable write error\n");
        bus_write_4(sc->breg, XLNX_NWLP_MSGF_MISC_STATUS,
            XLNX_NWLP_MISC_UC_WRITE);
    }

    if (reg & XLNX_NWLP_MISC_SLAVE) {
        reg = bus_read_4(sc->breg, XLNX_NWLP_MSGF_MISC_SLAVE_ID);
        device_printf(sc->dev, "AXI Slave error: 0x%x\n", reg);
        bus_write_4(sc->breg, XLNX_NWLP_MSGF_MISC_STATUS,
            XLNX_NWLP_MISC_SLAVE);
    }

    if (reg & XLNX_NWLP_MISC_MASTER) {
        reg = bus_read_4(sc->breg, XLNX_NWLP_MSGF_MISC_MASTER_ID);
        device_printf(sc->dev, "AXI Master error: 0x%x\n", reg);
        bus_write_4(sc->breg, XLNX_NWLP_MSGF_MISC_STATUS,
            XLNX_NWLP_MISC_MASTER);
    }

    if (reg & XLNX_NWLP_MISC_I_ADDR_TRANS) {
        reg = bus_read_4(sc->breg, XLNX_NWLP_MSGF_MISC_INGRESS_ID);
        device_printf(sc->dev, "Ingress address translation error at: 0x%x\n",
            reg);
        bus_write_4(sc->breg, XLNX_NWLP_MSGF_MISC_STATUS,
            XLNX_NWLP_MISC_I_ADDR_TRANS);
    }

    if (reg & XLNX_NWLP_MISC_E_ADDR_TRANS) {
        reg = bus_read_4(sc->breg, XLNX_NWLP_MSGF_MISC_EGRESS_ID);
        device_printf(sc->dev, "Egress address translation error at: 0x%x\n",
            reg);
        bus_write_4(sc->breg, XLNX_NWLP_MSGF_MISC_STATUS,
            XLNX_NWLP_MISC_E_ADDR_TRANS);
    }

    if (reg & XLNX_NWLP_MISC_LINK_DOWN) {
        device_printf(sc->dev, "PCIe Link down\n");
        bus_write_4(sc->breg, XLNX_NWLP_MSGF_MISC_STATUS,
            XLNX_NWLP_MISC_E_ADDR_TRANS);
    }

    return FILTER_HANDLED;
}

static int xlnx_nwl_pcib_fdt_attach(device_t dev) {
    struct xlnx_nwl_pcib_softc* sc;
    struct generic_pcie_fdt_softc* fdt_sc;
    struct generic_pcie_core_softc* core_sc;
    int error;
    int rid;
    phandle_t node;

    node = ofw_bus_get_node(dev);
    sc = device_get_softc(dev);
    sc->dev = dev;
    fdt_sc = &sc->base;
    core_sc = &fdt_sc->base;

    core_sc->quirks |= PCIE_CUSTOM_CONFIG_SPACE_QUIRK;

    mtx_init(&sc->msi_mtx, "xlnx_nwl_pcib: msi_mtx", NULL, MTX_DEF);

    /* Attach BREG */
    error = ofw_bus_find_string_index(node, "reg-names", "breg", &rid);
    if (error != 0) {
        device_printf(dev, "could not get breg memory: %d\n", error);
        return (error);
    }
    sc->breg = bus_alloc_resource_any(dev, SYS_RES_MEMORY, &rid,
        RF_ACTIVE);
    if (sc->breg == NULL) {
        device_printf(dev, "could not allocate breg memory.\n");
        return (ENXIO);
    }

    /* Attach PCI attributes */
    error = ofw_bus_find_string_index(node, "reg-names", "pcireg", &rid);
    if (error != 0) {
        device_printf(dev, "could not get pcireg memory: %d\n", error);
        return (error);
    }
    sc->pcireg = bus_alloc_resource_any(dev, SYS_RES_MEMORY, &rid,
        RF_ACTIVE);
    if (sc->pcireg == NULL) {
        device_printf(dev, "could not allocate pcireg memory.\n");
        return (ENXIO);
    }

    /* Attach PCI configuration space */
    error = ofw_bus_find_string_index(node, "reg-names", "cfg", &rid);
    if (error != 0) {
        device_printf(dev, "could not get cfg memory: %d\n", error);
        return (error);
    }
    sc->cfg = bus_alloc_resource_any(dev, SYS_RES_MEMORY, &rid,
        RF_ACTIVE);
    if (sc->cfg == NULL) {
        device_printf(dev, "could not allocate cfg memory.\n");
        return (ENXIO);
    }
    core_sc->res = sc->cfg;

    /* MSI0 interrupt handler */
    error = ofw_bus_find_string_index(node, "interrupt-names", "msi0", &rid);
    if (error != 0) {
        device_printf(dev, "could not get MSI0 interrupt: %d\n", error);
        return (error);
    }
    sc->msi_intr[0] = bus_alloc_resource_any(dev, SYS_RES_IRQ, &rid,
        RF_ACTIVE);
    if (sc->msi_intr[0] == NULL) {
        device_printf(dev, "could not allocate MSI0 interrupt.\n");
        return (ENXIO);
    }
    error = bus_setup_intr(dev, sc->msi_intr[0], INTR_TYPE_MISC | INTR_MPSAFE,
        xlnx_nwl_pcib_msi0_intr, NULL, sc, &sc->intr_cookie[0]);
    if (error != 0) {
        device_printf(dev, "could not setup MSI0 interrupt handler: %d\n", error);
        return (error);
    }
    bus_describe_intr(dev, sc->msi_intr[0], sc->intr_cookie[0], "msi0");

    /* MSI1 interrupt handler */
    error = ofw_bus_find_string_index(node, "interrupt-names", "msi1", &rid);
    if (error != 0) {
        device_printf(dev, "could not get MSI1 interrupt: %d\n", error);
        return (error);
    }
    sc->msi_intr[1] = bus_alloc_resource_any(dev, SYS_RES_IRQ, &rid,
        RF_ACTIVE);
    if (sc->msi_intr[1] == NULL) {
        device_printf(dev, "could not allocate MSI1 interrupt.\n");
        return (ENXIO);
    }
    error = bus_setup_intr(dev, sc->msi_intr[1], INTR_TYPE_MISC | INTR_MPSAFE,
        xlnx_nwl_pcib_msi1_intr, NULL, sc, &sc->intr_cookie[1]);
    if (error != 0) {
        device_printf(dev, "could not setup MSI1 interrupt handler: %d\n", error);
        return (error);
    }
    bus_describe_intr(dev, sc->msi_intr[1], sc->intr_cookie[1], "msi1");

    /* MISC interrupt handler */
    error = ofw_bus_find_string_index(node, "interrupt-names", "misc", &rid);
    if (error != 0) {
        device_printf(dev, "could not get MISC interrupt: %d\n", error);
        return (error);
    }
    sc->misc_intr = bus_alloc_resource_any(dev, SYS_RES_IRQ, &rid,
        RF_ACTIVE);
    if (sc->misc_intr == NULL) {
        device_printf(dev, "could not allocate MISC interrupt.\n");
        return (ENXIO);
    }
    error = bus_setup_intr(dev, sc->misc_intr, INTR_TYPE_MISC | INTR_MPSAFE,
        xlnx_nwl_pcib_misc_intr, NULL, sc, &sc->intr_cookie[2]);
    if (error != 0) {
        device_printf(dev, "could not setup MISC interrupt handler: %d\n", error);
        return (error);
    }
    bus_describe_intr(dev, sc->misc_intr, sc->intr_cookie[2], "misc");

    error = xlnx_nwl_pcib_init(dev, sc);
    if (error != 0) {
        return (error);
    }

    error = xlnx_nwl_pcib_register_msi(sc);
    if (error != 0) {
        return (error);
    }

    error = pci_host_generic_setup_fdt(dev);
    if (error != 0) {
        return (error);
    }

    xlnx_nwl_pcib_locate_bridge_window(dev, sc);

    device_add_child(dev, "pci", -1);
    return bus_generic_attach(dev);
}

static bool xlnx_nwl_pcib_link_up(struct xlnx_nwl_pcib_softc* sc) {
    uint32_t val;

    val = (bus_read_4(sc->pcireg, XLNX_NWLP_PCIE_STATUS)
        & XLNX_NWLP_PCIE_STS_LINK_UP);

    return (val != 0);
}

static bool xlnx_nwl_pcib_phy_ready(struct xlnx_nwl_pcib_softc* sc) {
    uint32_t val;

    val = (bus_read_4(sc->pcireg, XLNX_NWLP_PCIE_STATUS)
        & XLNX_NWLP_PCIE_STS_PHY_RDY);

    return (val != 0);
}

static bool xlnx_nwl_pcib_valid_request(struct xlnx_nwl_pcib_softc* sc,
    u_int bus, u_int slot, u_int func, u_int reg) {
    struct generic_pcie_fdt_softc* fdt_sc;
    struct generic_pcie_core_softc* core_sc;

    fdt_sc = &sc->base;
    core_sc = &fdt_sc->base;

    if ((bus < core_sc->bus_start) || (bus > core_sc->bus_end)) {
        return false;
    }
    if ((slot > PCI_SLOTMAX) || (func > PCI_FUNCMAX) || (reg > PCIE_REGMAX)) {
        return false;
    }
    if (bus == 0 && (slot != 0 || func != 0)) {
        return false;
    }

    if (!xlnx_nwl_pcib_link_up(sc) || !xlnx_nwl_pcib_phy_ready(sc)) {
        return false;
    }

    return true;
}

static uint32_t xlnx_nwl_pcib_read_config(device_t dev, u_int bus, u_int slot,
    u_int func, u_int reg, int bytes) {
    struct xlnx_nwl_pcib_softc* sc;
    struct generic_pcie_fdt_softc* fdt_sc;
    struct generic_pcie_core_softc* core_sc;
    uint64_t offset;
    uint32_t val;

    sc = device_get_softc(dev);
    fdt_sc = &sc->base;
    core_sc = &fdt_sc->base;

    if (!xlnx_nwl_pcib_valid_request(sc, bus, slot, func, reg)) {
        return (~0U);
    }

    offset = PCIE_ADDR_OFFSET(bus - core_sc->bus_start, slot, func, reg);

    switch (bytes) {
    case 1:
        val = bus_read_1(core_sc->res, offset);
        break;
    case 2:
        val = le16toh(bus_read_2(core_sc->res, offset));
        break;
    case 4:
        val = le32toh(bus_read_4(core_sc->res, offset));
        break;
    default:
        break;
    }

    return val;
}

static void xlnx_nwl_pcib_write_config(device_t dev, u_int bus, u_int slot,
    u_int func, u_int reg, uint32_t val, int bytes) {
    struct xlnx_nwl_pcib_softc* sc;
    struct generic_pcie_fdt_softc* fdt_sc;
    struct generic_pcie_core_softc* core_sc;
    uint64_t offset;

    sc = device_get_softc(dev);
    fdt_sc = &sc->base;
    core_sc = &fdt_sc->base;

    if (!xlnx_nwl_pcib_valid_request(sc, bus, slot, func, reg)) {
        return;
    }

    offset = PCIE_ADDR_OFFSET(bus - core_sc->bus_start, slot, func, reg);

    switch (bytes) {
    case 1:
        bus_write_1(core_sc->res, offset, val);
        break;
    case 2:
        bus_write_2(core_sc->res, offset, htole16(val));
        break;
    case 4:
        bus_write_4(core_sc->res, offset, htole32(val));
        break;
    default:
        break;
    }
}

static int xlnx_nwl_pcib_msi_alloc_msi(device_t dev, device_t child, int count,
    int maxcount, device_t* pic, struct intr_irqsrc **srcs) {
    struct xlnx_nwl_pcib_softc* sc;
    int first_irq;
    int i;
    bool found;

    sc = device_get_softc(dev);

    mtx_lock(&sc->msi_mtx);

    found = false;

    for (first_irq = 0; (first_irq + count) < XLNX_NWLP_MSI_MAX; ) {
        found = true;
        /* Check for continuous region of free MSIs. */
        for (i = first_irq; i < first_irq + count; ++i) {
            if (sc->isrcs[i].allocated) {
                found = false;
                break;
            }
        }
        if (found) {
            break;
        } else {
            first_irq = i + 1;
        }
    }

    if (!found) {
        mtx_unlock(&sc->msi_mtx);
        return ENXIO;
    }

    for (i = 0; i < count; ++i) {
        sc->isrcs[first_irq + i].allocated = true;
        srcs[i] = (struct intr_irqsrc*)&(sc->isrcs[first_irq + i]);
    }

    mtx_unlock(&sc->msi_mtx);

    *pic = device_get_parent(dev);

    return 0;
}

static int xlnx_nwl_pcib_msi_release_msi(device_t dev, device_t child,
    int count, struct intr_irqsrc **isrc) {
    struct xlnx_nwl_pcib_softc* sc;
    struct xlnx_nwl_pcib_irqsrc* irq_i;
    int i;

    sc = device_get_softc(dev);

    mtx_lock(&sc->msi_mtx);

    for (i = 0; i < count; i++) {
        irq_i = (struct xlnx_nwl_pcib_irqsrc*)isrc[i];
        irq_i->allocated = false;
    }

    mtx_unlock(&sc->msi_mtx);
    return 0;
}

static int xlnx_nwl_pcib_msi_map_msi(device_t dev, device_t child,
    struct intr_irqsrc *isrc, uint64_t* addr, uint32_t* data) {
    struct xlnx_nwl_pcib_softc* sc;
    struct xlnx_nwl_pcib_irqsrc* irq_i;

    sc = device_get_softc(dev);
    irq_i = (struct xlnx_nwl_pcib_irqsrc*)isrc;

    *addr = XLNX_NWLP_MSI_ADDR;
    *data = irq_i->irq;

    return 0;
}

static void xlnx_nwl_pcib_msi_mask(device_t dev, struct intr_irqsrc *isrc,
    bool mask) {
    struct xlnx_nwl_pcib_softc* sc;
    struct xlnx_nwl_pcib_irqsrc* irq_i;
    uint32_t msi_reg;
    uint32_t reg;
    uint32_t irq;

    sc = device_get_softc(dev);
    irq_i = (struct xlnx_nwl_pcib_irqsrc*)isrc;
    irq = irq_i->irq;

    msi_reg = XLNX_NWLP_MSGF_MSI_MASK_LO;
    if (irq >= 32) {
        msi_reg = XLNX_NWLP_MSGF_MSI_MASK_HI;
        irq = irq - 32;
    }

    reg = bus_read_4(sc->breg, msi_reg);
    if (mask) {
        reg &= ~(1 << irq);
    } else {
        reg |= (1 << irq);
    }
    bus_write_4(sc->breg, msi_reg, reg);
}

static void xlnx_nwl_pcib_msi_disable_intr(device_t dev,
    struct intr_irqsrc *isrc) {
    xlnx_nwl_pcib_msi_mask(dev, isrc, true);
}

static void xlnx_nwl_pcib_msi_enable_intr(device_t dev,
    struct intr_irqsrc *isrc) {
    xlnx_nwl_pcib_msi_mask(dev, isrc, false);
}

static int xlnx_nwl_pcib_msi_setup_intr(device_t dev, struct intr_irqsrc *isrc,
    struct resource* res, struct intr_map_data* data) {
    return 0;
}

static int xlnx_nwl_pcib_msi_teardown_intr(device_t dev, struct intr_irqsrc *isrc,
    struct resource* res, struct intr_map_data* data) {
    return 0;
}

static void xlnx_nwl_pcib_msi_post_filter(device_t dev,
    struct intr_irqsrc *isrc) {
}

static void xlnx_nwl_pcib_msi_post_ithread(device_t dev,
    struct intr_irqsrc *isrc) {
    xlnx_nwl_pcib_msi_mask(dev, isrc, false);
}

static void xlnx_nwl_pcib_msi_pre_ithread(device_t dev,
    struct intr_irqsrc *isrc) {
    xlnx_nwl_pcib_msi_mask(dev, isrc, true);
}

static device_method_t xlnx_nwl_pcib_fdt_methods[] = {
    /* Device interface */
    DEVMETHOD(device_probe,       xlnx_nwl_pcib_fdt_probe),
    DEVMETHOD(device_attach,      xlnx_nwl_pcib_fdt_attach),

    /* pcib interface */
    DEVMETHOD(pcib_read_config,   xlnx_nwl_pcib_read_config),
    DEVMETHOD(pcib_write_config,  xlnx_nwl_pcib_write_config),

    /* MSI interface */
    DEVMETHOD(msi_alloc_msi,      xlnx_nwl_pcib_msi_alloc_msi),
    DEVMETHOD(msi_release_msi,    xlnx_nwl_pcib_msi_release_msi),
    DEVMETHOD(msi_map_msi,        xlnx_nwl_pcib_msi_map_msi),

    /* Interrupt controller interface */
    DEVMETHOD(pic_disable_intr,   xlnx_nwl_pcib_msi_disable_intr),
    DEVMETHOD(pic_enable_intr,    xlnx_nwl_pcib_msi_enable_intr),
    DEVMETHOD(pic_setup_intr,     xlnx_nwl_pcib_msi_setup_intr),
    DEVMETHOD(pic_teardown_intr,  xlnx_nwl_pcib_msi_teardown_intr),
    DEVMETHOD(pic_post_filter,    xlnx_nwl_pcib_msi_post_filter),
    DEVMETHOD(pic_post_ithread,   xlnx_nwl_pcib_msi_post_ithread),
    DEVMETHOD(pic_pre_ithread,    xlnx_nwl_pcib_msi_pre_ithread),

    /* End */
    DEVMETHOD_END
};

DEFINE_CLASS_1(pcib, xlnx_nwl_pcib_fdt_driver, xlnx_nwl_pcib_fdt_methods,
    sizeof(struct xlnx_nwl_pcib_softc), generic_pcie_fdt_driver);

DRIVER_MODULE(xlnx_nwl_pcib, simplebus, xlnx_nwl_pcib_fdt_driver, 0, 0);
DRIVER_MODULE(xlnx_nwl_pcib, ofwbus, xlnx_nwl_pcib_fdt_driver, 0, 0);
