/*
 *  FTP Server Daemon
 *
 *  Submitted by: Jake Janovetz <janovetz@tempest.ece.uiuc.edu>
 *
 *  Changed by:   Sergei Organov <osv@javad.ru> (OSV)
 *                Arnout Vandecappelle <arnout@mind.be> (AV)
 *                Sebastien Bourdeauducq <sebastien@milkymist.org> (MM)
 */

/*************************************************************************
 *                                 ftpd-init.c
 *************************************************************************
 * Description:
 *
 *    This file contains the legacy daemon initialisation.
 *
 *************************************************************************/

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/ftpd.h>

/* Configuration table */
extern struct rtems_ftpd_configuration rtems_ftpd_configuration;

/*
 * rtems_initialize_ftpd
 *
 * Call the FTPD start function.
 */
int
rtems_initialize_ftpd(void)
{
  rtems_ftpd_configuration.verbose = true;
  return rtems_ftpd_start(&rtems_ftpd_configuration);
}
