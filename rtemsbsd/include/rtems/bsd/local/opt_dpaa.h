#include <bsp.h>
#ifdef LIBBSP_POWERPC_QORIQ_BSP_H
#if QORIQ_CHIP_IS_T_VARIANT(QORIQ_CHIP_VARIANT)
#include <bsp/qoriq.h>
#endif
#else
#define QORIQ_CHIP_IS_T_VARIANT(x) 0
#endif
#undef __ppc_generic
#define __ppc_generic
#define KBUILD_MODNAME "dpaa"
#define CONFIG_FSL_BMAN
#define CONFIG_FSL_BMAN_PORTAL
#define CONFIG_FSL_BMAN_TEST_API
#define CONFIG_FSL_BMAN_TEST_THRESH
#define CONFIG_FSL_QMAN
#define CONFIG_FSL_QMAN_CONFIG
#define CONFIG_FSL_QMAN_PORTAL
#define CONFIG_FSL_QMAN_TEST_API
#define CONFIG_FSL_QMAN_TEST_STASH
