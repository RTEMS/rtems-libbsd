#include <bsp.h>

#ifdef LIBBSP_POWERPC_QORIQ_BSP_H
#if QORIQ_CHIP_IS_T_VARIANT(QORIQ_CHIP_VARIANT)
#include <bsp/qoriq.h>
#endif
#else
#define QORIQ_CHIP_IS_T_VARIANT(x) 0
#endif

#ifdef QORIQ_IS_HYPERVISOR_GUEST
#define SDK_DPAA_COMPAT_STATIC
#else
#define SDK_DPAA_COMPAT_STATIC static
#endif

#undef __ppc_generic
#define __ppc_generic

#define KBUILD_MODNAME "dpaa"
