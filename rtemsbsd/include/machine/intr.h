#ifdef INTRNG
#include <sys/intr.h>
#endif /* INTRNG */

#ifndef	NIRQ
#include <bsp/irq.h>
#define	NIRQ			BSP_INTERRUPT_VECTOR_COUNT
#endif
