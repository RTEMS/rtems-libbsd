#include <paths.h>

#define	_PATH_DEVPCI	"/dev/pci"
#ifndef __rtems__
#define	_PATH_PCIVDB	"/usr/share/misc/pci_vendors"
#define	_PATH_LPCIVDB	_PATH_LOCALBASE "/share/pciids/pci.ids"
#endif /* __rtems__ */
