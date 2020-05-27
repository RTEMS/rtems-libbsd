#include <rtems/bsd/modules.h>
#include <bsp.h>

#define USB_HAVE_CONDVAR 1

#define USB_HAVE_UGEN 1

#define USB_HAVE_BUSDMA 1

#if !defined(LIBBSP_ARM_STM32H7_BSP_H)
#define USB_NEED_BUSDMA_COHERENT_ALLOC 1
#endif

#define USB_HAVE_COMPAT_LINUX 0

#define USB_HAVE_USER_IO 0

#if defined(RTEMS_BSD_MODULE_NETINET) || defined(RTEMS_BSD_MODULE_NETINET6)
#define USB_HAVE_MBUF 1
#endif

#undef USB_VERBOSE

#undef USB_DEBUG

#define USB_HAVE_TT_SUPPORT 1

#define USB_HAVE_POWERD 1

#define USB_HAVE_PER_BUS_PROCESS 1
