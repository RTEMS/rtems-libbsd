/* -*- Mode: C; tab-width: 4 -*-
 *
 * Copyright (c) 2002-2015 Apple Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This code is completely 100% portable C. It does not depend on any external header files
 * from outside the mDNS project -- all the types it expects to find are defined right here.
 *
 * The previous point is very important: This file does not depend on any external
 * header files. It should compile on *any* platform that has a C compiler, without
 * making *any* assumptions about availability of so-called "standard" C functions,
 * routines, or types (which may or may not be present on any given platform).
 */

/*
 * Apple removed the mDNS_StartResolveService() and mDNS_StopResolveService()
 * functions in v765.1.2.  The reason for this is unknown.
 */

#include "DNSCommon.h"                  // Defines general DNS utility routines
#include "uDNS.h"                       // Defines entry points into unicast-specific routines

mDNSlocal mDNSBool MachineHasActiveIPv6(mDNS *const m)
{
    NetworkInterfaceInfo *intf;
    for (intf = m->HostInterfaces; intf; intf = intf->next)
        if (intf->ip.type == mDNSAddrType_IPv6) return(mDNStrue);
    return(mDNSfalse);
}

mDNSlocal void FoundServiceInfoSRV(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, QC_result AddRecord)
{
    ServiceInfoQuery *query = (ServiceInfoQuery *)question->QuestionContext;
    mDNSBool PortChanged = !mDNSSameIPPort(query->info->port, answer->rdata->u.srv.port);
    if (!AddRecord) return;
    if (answer->rrtype != kDNSType_SRV) return;

    query->info->port = answer->rdata->u.srv.port;

    // If this is our first answer, then set the GotSRV flag and start the address query
    if (!query->GotSRV)
    {
        query->GotSRV             = mDNStrue;
        query->qAv4.InterfaceID   = answer->InterfaceID;
        AssignDomainName(&query->qAv4.qname, &answer->rdata->u.srv.target);
        query->qAv6.InterfaceID   = answer->InterfaceID;
        AssignDomainName(&query->qAv6.qname, &answer->rdata->u.srv.target);
        mDNS_StartQuery(m, &query->qAv4);
        // Only do the AAAA query if this machine actually has IPv6 active
        if (MachineHasActiveIPv6(m)) mDNS_StartQuery(m, &query->qAv6);
    }
    // If this is not our first answer, only re-issue the address query if the target host name has changed
    else if ((query->qAv4.InterfaceID != query->qSRV.InterfaceID && query->qAv4.InterfaceID != answer->InterfaceID) ||
             !SameDomainName(&query->qAv4.qname, &answer->rdata->u.srv.target))
    {
        mDNS_StopQuery(m, &query->qAv4);
        if (query->qAv6.ThisQInterval >= 0) mDNS_StopQuery(m, &query->qAv6);
        if (SameDomainName(&query->qAv4.qname, &answer->rdata->u.srv.target) && !PortChanged)
        {
            // If we get here, it means:
            // 1. This is not our first SRV answer
            // 2. The interface ID is different, but the target host and port are the same
            // This implies that we're seeing the exact same SRV record on more than one interface, so we should
            // make our address queries at least as broad as the original SRV query so that we catch all the answers.
            query->qAv4.InterfaceID = query->qSRV.InterfaceID;  // Will be mDNSInterface_Any, or a specific interface
            query->qAv6.InterfaceID = query->qSRV.InterfaceID;
        }
        else
        {
            query->qAv4.InterfaceID   = answer->InterfaceID;
            AssignDomainName(&query->qAv4.qname, &answer->rdata->u.srv.target);
            query->qAv6.InterfaceID   = answer->InterfaceID;
            AssignDomainName(&query->qAv6.qname, &answer->rdata->u.srv.target);
        }
        debugf("FoundServiceInfoSRV: Restarting address queries for %##s (%s)", query->qAv4.qname.c, DNSTypeName(query->qAv4.qtype));
        mDNS_StartQuery(m, &query->qAv4);
        // Only do the AAAA query if this machine actually has IPv6 active
        if (MachineHasActiveIPv6(m)) mDNS_StartQuery(m, &query->qAv6);
    }
    else if (query->ServiceInfoQueryCallback && query->GotADD && query->GotTXT && PortChanged)
    {
        if (++query->Answers >= 100)
            debugf("**** WARNING **** Have given %lu answers for %##s (SRV) %##s %u",
                   query->Answers, query->qSRV.qname.c, answer->rdata->u.srv.target.c,
                   mDNSVal16(answer->rdata->u.srv.port));
        query->ServiceInfoQueryCallback(m, query);
    }
    // CAUTION: MUST NOT do anything more with query after calling query->Callback(), because the client's
    // callback function is allowed to do anything, including deleting this query and freeing its memory.
}

mDNSlocal void FoundServiceInfoTXT(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, QC_result AddRecord)
{
    ServiceInfoQuery *query = (ServiceInfoQuery *)question->QuestionContext;
    if (!AddRecord) return;
    if (answer->rrtype != kDNSType_TXT) return;
    if (answer->rdlength > sizeof(query->info->TXTinfo)) return;

    query->GotTXT       = mDNStrue;
    query->info->TXTlen = answer->rdlength;
    query->info->TXTinfo[0] = 0;        // In case answer->rdlength is zero
    mDNSPlatformMemCopy(query->info->TXTinfo, answer->rdata->u.txt.c, answer->rdlength);

    verbosedebugf("FoundServiceInfoTXT: %##s GotADD=%d", query->info->name.c, query->GotADD);

    // CAUTION: MUST NOT do anything more with query after calling query->Callback(), because the client's
    // callback function is allowed to do anything, including deleting this query and freeing its memory.
    if (query->ServiceInfoQueryCallback && query->GotADD)
    {
        if (++query->Answers >= 100)
            debugf("**** WARNING **** have given %lu answers for %##s (TXT) %#s...",
                   query->Answers, query->qSRV.qname.c, answer->rdata->u.txt.c);
        query->ServiceInfoQueryCallback(m, query);
    }
}

mDNSlocal void FoundServiceInfo(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, QC_result AddRecord)
{
    ServiceInfoQuery *query = (ServiceInfoQuery *)question->QuestionContext;
    //LogInfo("FoundServiceInfo %d %s", AddRecord, RRDisplayString(m, answer));
    if (!AddRecord) return;

    if (answer->rrtype == kDNSType_A)
    {
        query->info->ip.type = mDNSAddrType_IPv4;
        query->info->ip.ip.v4 = answer->rdata->u.ipv4;
    }
    else if (answer->rrtype == kDNSType_AAAA)
    {
        query->info->ip.type = mDNSAddrType_IPv6;
        query->info->ip.ip.v6 = answer->rdata->u.ipv6;
    }
    else
    {
        debugf("FoundServiceInfo: answer %##s type %d (%s) unexpected", answer->name->c, answer->rrtype, DNSTypeName(answer->rrtype));
        return;
    }

    query->GotADD = mDNStrue;
    query->info->InterfaceID = answer->InterfaceID;

    verbosedebugf("FoundServiceInfo v%ld: %##s GotTXT=%d", query->info->ip.type, query->info->name.c, query->GotTXT);

    // CAUTION: MUST NOT do anything more with query after calling query->Callback(), because the client's
    // callback function is allowed to do anything, including deleting this query and freeing its memory.
    if (query->ServiceInfoQueryCallback && query->GotTXT)
    {
        if (++query->Answers >= 100)
            debugf(answer->rrtype == kDNSType_A ?
                   "**** WARNING **** have given %lu answers for %##s (A) %.4a" :
                   "**** WARNING **** have given %lu answers for %##s (AAAA) %.16a",
                   query->Answers, query->qSRV.qname.c, &answer->rdata->u.data);
        query->ServiceInfoQueryCallback(m, query);
    }
}

// On entry, the client must have set the name and InterfaceID fields of the ServiceInfo structure
// If the query is not interface-specific, then InterfaceID may be zero
// Each time the Callback is invoked, the remainder of the fields will have been filled in
// In addition, InterfaceID will be updated to give the interface identifier corresponding to that response
mDNSexport mStatus mDNS_StartResolveService(mDNS *const m,
                                            ServiceInfoQuery *query, ServiceInfo *info, mDNSServiceInfoQueryCallback *Callback, void *Context)
{
    mStatus status;
    mDNS_Lock(m);

    query->qSRV.ThisQInterval       = -1;       // So that mDNS_StopResolveService() knows whether to cancel this question
    query->qSRV.InterfaceID         = info->InterfaceID;
    query->qSRV.flags               = 0;
    query->qSRV.Target              = zeroAddr;
    AssignDomainName(&query->qSRV.qname, &info->name);
    query->qSRV.qtype               = kDNSType_SRV;
    query->qSRV.qclass              = kDNSClass_IN;
    query->qSRV.LongLived           = mDNSfalse;
    query->qSRV.ExpectUnique        = mDNStrue;
    query->qSRV.ForceMCast          = mDNSfalse;
    query->qSRV.ReturnIntermed      = mDNSfalse;
    query->qSRV.SuppressUnusable    = mDNSfalse;
    query->qSRV.SearchListIndex     = 0;
    query->qSRV.AppendSearchDomains = 0;
    query->qSRV.RetryWithSearchDomains = mDNSfalse;
    query->qSRV.TimeoutQuestion     = 0;
    query->qSRV.WakeOnResolve       = 0;
    query->qSRV.UseBackgroundTrafficClass = mDNSfalse;
    query->qSRV.ValidationRequired  = 0;
    query->qSRV.ValidatingResponse  = 0;
    query->qSRV.ProxyQuestion       = 0;
    query->qSRV.qnameOrig           = mDNSNULL;
    query->qSRV.AnonInfo            = mDNSNULL;
    query->qSRV.QuestionCallback    = FoundServiceInfoSRV;
    query->qSRV.QuestionContext     = query;

    query->qTXT.ThisQInterval       = -1;       // So that mDNS_StopResolveService() knows whether to cancel this question
    query->qTXT.InterfaceID         = info->InterfaceID;
    query->qTXT.flags               = 0;
    query->qTXT.Target              = zeroAddr;
    AssignDomainName(&query->qTXT.qname, &info->name);
    query->qTXT.qtype               = kDNSType_TXT;
    query->qTXT.qclass              = kDNSClass_IN;
    query->qTXT.LongLived           = mDNSfalse;
    query->qTXT.ExpectUnique        = mDNStrue;
    query->qTXT.ForceMCast          = mDNSfalse;
    query->qTXT.ReturnIntermed      = mDNSfalse;
    query->qTXT.SuppressUnusable    = mDNSfalse;
    query->qTXT.SearchListIndex     = 0;
    query->qTXT.AppendSearchDomains = 0;
    query->qTXT.RetryWithSearchDomains = mDNSfalse;
    query->qTXT.TimeoutQuestion     = 0;
    query->qTXT.WakeOnResolve       = 0;
    query->qTXT.UseBackgroundTrafficClass = mDNSfalse;
    query->qTXT.ValidationRequired  = 0;
    query->qTXT.ValidatingResponse  = 0;
    query->qTXT.ProxyQuestion       = 0;
    query->qTXT.qnameOrig           = mDNSNULL;
    query->qTXT.AnonInfo            = mDNSNULL;
    query->qTXT.QuestionCallback    = FoundServiceInfoTXT;
    query->qTXT.QuestionContext     = query;

    query->qAv4.ThisQInterval       = -1;       // So that mDNS_StopResolveService() knows whether to cancel this question
    query->qAv4.InterfaceID         = info->InterfaceID;
    query->qAv4.flags               = 0;
    query->qAv4.Target              = zeroAddr;
    query->qAv4.qname.c[0]          = 0;
    query->qAv4.qtype               = kDNSType_A;
    query->qAv4.qclass              = kDNSClass_IN;
    query->qAv4.LongLived           = mDNSfalse;
    query->qAv4.ExpectUnique        = mDNStrue;
    query->qAv4.ForceMCast          = mDNSfalse;
    query->qAv4.ReturnIntermed      = mDNSfalse;
    query->qAv4.SuppressUnusable    = mDNSfalse;
    query->qAv4.SearchListIndex     = 0;
    query->qAv4.AppendSearchDomains = 0;
    query->qAv4.RetryWithSearchDomains = mDNSfalse;
    query->qAv4.TimeoutQuestion     = 0;
    query->qAv4.WakeOnResolve       = 0;
    query->qAv4.UseBackgroundTrafficClass = mDNSfalse;
    query->qAv4.ValidationRequired  = 0;
    query->qAv4.ValidatingResponse  = 0;
    query->qAv4.ProxyQuestion       = 0;
    query->qAv4.qnameOrig           = mDNSNULL;
    query->qAv4.AnonInfo            = mDNSNULL;
    query->qAv4.QuestionCallback    = FoundServiceInfo;
    query->qAv4.QuestionContext     = query;

    query->qAv6.ThisQInterval       = -1;       // So that mDNS_StopResolveService() knows whether to cancel this question
    query->qAv6.InterfaceID         = info->InterfaceID;
    query->qAv6.flags               = 0;
    query->qAv6.Target              = zeroAddr;
    query->qAv6.qname.c[0]          = 0;
    query->qAv6.qtype               = kDNSType_AAAA;
    query->qAv6.qclass              = kDNSClass_IN;
    query->qAv6.LongLived           = mDNSfalse;
    query->qAv6.ExpectUnique        = mDNStrue;
    query->qAv6.ForceMCast          = mDNSfalse;
    query->qAv6.ReturnIntermed      = mDNSfalse;
    query->qAv6.SuppressUnusable    = mDNSfalse;
    query->qAv6.SearchListIndex     = 0;
    query->qAv6.AppendSearchDomains = 0;
    query->qAv6.RetryWithSearchDomains = mDNSfalse;
    query->qAv6.TimeoutQuestion     = 0;
    query->qAv6.UseBackgroundTrafficClass = mDNSfalse;
    query->qAv6.ValidationRequired  = 0;
    query->qAv6.ValidatingResponse  = 0;
    query->qAv6.ProxyQuestion       = 0;
    query->qAv6.qnameOrig           = mDNSNULL;
    query->qAv6.AnonInfo            = mDNSNULL;
    query->qAv6.QuestionCallback    = FoundServiceInfo;
    query->qAv6.QuestionContext     = query;

    query->GotSRV                   = mDNSfalse;
    query->GotTXT                   = mDNSfalse;
    query->GotADD                   = mDNSfalse;
    query->Answers                  = 0;

    query->info                     = info;
    query->ServiceInfoQueryCallback = Callback;
    query->ServiceInfoQueryContext  = Context;

//	info->name      = Must already be set up by client
//	info->interface = Must already be set up by client
    info->ip        = zeroAddr;
    info->port      = zeroIPPort;
    info->TXTlen    = 0;

    // We use mDNS_StartQuery_internal here because we're already holding the lock
    status = mDNS_StartQuery_internal(m, &query->qSRV);
    if (status == mStatus_NoError) status = mDNS_StartQuery_internal(m, &query->qTXT);
    if (status != mStatus_NoError) mDNS_StopResolveService(m, query);

    mDNS_Unlock(m);
    return(status);
}

mDNSexport void    mDNS_StopResolveService (mDNS *const m, ServiceInfoQuery *q)
{
    mDNS_Lock(m);
    // We use mDNS_StopQuery_internal here because we're already holding the lock
    if (q->qSRV.ThisQInterval >= 0) mDNS_StopQuery_internal(m, &q->qSRV);
    if (q->qTXT.ThisQInterval >= 0) mDNS_StopQuery_internal(m, &q->qTXT);
    if (q->qAv4.ThisQInterval >= 0) mDNS_StopQuery_internal(m, &q->qAv4);
    if (q->qAv6.ThisQInterval >= 0) mDNS_StopQuery_internal(m, &q->qAv6);
    mDNS_Unlock(m);
}
