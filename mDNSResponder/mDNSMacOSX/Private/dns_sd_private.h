/* -*- Mode: C; tab-width: 4 -*-
 *
 * Copyright (c) 2015 Apple Inc. All rights reserved.
 */

#ifndef _DNS_SD_PRIVATE_H
#define _DNS_SD_PRIVATE_H

/* DNSServiceCreateDelegateConnection()
 *
 * Parameters:
 *
 * sdRef:           A pointer to an uninitialized DNSServiceRef. Deallocating
 *                  the reference (via DNSServiceRefDeallocate()) severs the
 *                  connection and deregisters all records registered on this connection.
 *
 * pid :            Process ID of the delegate
 *
 * uuid:            UUID of the delegate
 *
 *                  Note that only one of the two arguments (pid or uuid) can be specified. If pid
 *                  is zero, uuid will be assumed to be a valid value; otherwise pid will be used.
 *
 * return value:    Returns kDNSServiceErr_NoError on success, otherwise returns
 *                  an error code indicating the specific failure that occurred (in which
 *                  case the DNSServiceRef is not initialized). kDNSServiceErr_NotAuth is
 *                  returned to indicate that the calling process does not have entitlements
 *                  to use this API.
 */
DNSServiceErrorType DNSSD_API DNSServiceCreateDelegateConnection(DNSServiceRef *sdRef, int32_t pid, uuid_t uuid);

#endif
