/* -*- Mode: C; tab-width: 4 -*-
 *
 * Copyright (c) 2016 Apple Inc. All rights reserved.
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
 */

#import "Metrics.h"

#if TARGET_OS_EMBEDDED
#import <CoreUtils/SoftLinking.h>
#import <WirelessDiagnostics/AWDDNSDomainStats.h>
#import <WirelessDiagnostics/AWDMDNSResponderDNSStatistics.h>
#import <WirelessDiagnostics/AWDMetricIds_MDNSResponder.h>
#import <WirelessDiagnostics/WirelessDiagnostics.h>

#import "DNSCommon.h"
#import "mDNSMacOSX.h"
#import "DebugServices.h"

//===========================================================================================================================
//  External Frameworks
//===========================================================================================================================

SOFT_LINK_FRAMEWORK(PrivateFrameworks, WirelessDiagnostics)

SOFT_LINK_CLASS(WirelessDiagnostics, AWDServerConnection)
#define AWDServerConnectionSoft                 getAWDServerConnectionClass()

SOFT_LINK_CLASS(WirelessDiagnostics, AWDMDNSResponderDNSStatistics)
#define AWDMDNSResponderDNSStatisticsSoft       getAWDMDNSResponderDNSStatisticsClass()

SOFT_LINK_CLASS(WirelessDiagnostics, AWDDNSDomainStats)
#define AWDDNSDomainStatsSoft                   getAWDDNSDomainStatsClass()

//===========================================================================================================================
//  Macros
//===========================================================================================================================

#define countof(X)      (sizeof(X) / sizeof(X[0]))

//===========================================================================================================================
//  Constants
//===========================================================================================================================

#define kUDNSStatsMaxQuerySendCount     10

// Important: Do not update this list without getting privacy approval. See <rdar://problem/24155761>.

static const char * const       kUDNSStatsDomains[] =
{
    ".",
    "apple.com.",
    "icloud.com.",
    "me.com.",
    "google.com.",
    "facebook.com.",
    "youtube.com.",
    "baidu.com.",
    "amazon.com.",
    "yahoo.com.",
    "wikipedia.org."
};

static const mDNSu32        kResponseLatencyMsLimits[] =
{
        1,     2,     3,     4,     5,
       10,    20,    30,    40,    50,    60,    70,    80,    90,
      100,   110,   120,   130,   140,   150,   160,   170,   180,   190,
      200,   250,   300,   350,   400,   450,   500,   550,   600,   650,   700,   750,   800,   850,   900,   950,
     1000,  1500,  2000,  2500,  3000,  3500,  4000,  4500,
     5000,  6000,  7000,  8000,  9000,
    10000
};

//===========================================================================================================================
//  Data structures
//===========================================================================================================================

typedef struct
{
    uint32_t    answeredQuerySendCountBins[kUDNSStatsMaxQuerySendCount + 1];
    uint32_t    unansweredQuerySendCountBins[kUDNSStatsMaxQuerySendCount + 1];
    uint32_t    responseLatencyBins[countof(kResponseLatencyMsLimits) + 1];

}   DNSStats;

typedef struct DNSDomainStats * DNSDomainStatsRef;
struct DNSDomainStats
{
    DNSDomainStatsRef   next;
    domainname          domain;
    int                 domainLabelCount;
    char *              domainStr;
    DNSStats            stats;
    DNSStats            statsCellular;
};

//===========================================================================================================================
//  Globals
//===========================================================================================================================

extern mDNS     mDNSStorage;

static DNSDomainStatsRef            gDomainStatsList        = NULL;
static AWDServerConnection *        gAWDServerConnection    = nil;

//===========================================================================================================================
//  Local Prototypes
//===========================================================================================================================

mDNSlocal mStatus   DNSDomainStatsCreate(const char *inDomain, DNSDomainStatsRef *outStats);
mDNSlocal void      DNSDomainStatsFree(DNSDomainStatsRef inStats);
mDNSlocal void      DNSDomainStatsFreeList(DNSDomainStatsRef inList);

mDNSlocal mStatus   CreateDomainStatsList(DNSDomainStatsRef *outList);
mDNSlocal void      UpdateDNSStats(DNSStats *inStats, mDNSBool inAnswered, mDNSu32 inQuerySendCount, mDNSu32 inLatencyMs);
mDNSlocal mStatus   SubmitAWDMetric(void);
mDNSlocal mStatus   CreateAWDDNSDomainStats(DNSStats *inStats, const char *inDomain, mDNSBool inIsForCellular, AWDDNSDomainStats **outAWDStats);
mDNSlocal void      LogDNSStats(const DNSStats *inStats);

//===========================================================================================================================
//  MetricsInit
//===========================================================================================================================

mStatus MetricsInit(void)
{
    mStatus     err;

    err = CreateDomainStatsList(&gDomainStatsList);
    require_noerr_quiet(err, exit);

    @autoreleasepool
    {
        gAWDServerConnection = [[AWDServerConnectionSoft alloc]
            initWithComponentId:        AWDComponentId_MDNSResponder
            andBlockOnConfiguration:    NO];

        if (gAWDServerConnection)
        {
            [gAWDServerConnection
                registerQueriableMetricCallback: ^(UInt32 metricId)
                {
                    mStatus localErr;

                    (void) metricId;

                    localErr = SubmitAWDMetric();
                    if (localErr) LogMsg("SubmitAWDMetric failed with error %d", localErr);
                }
                forIdentifier: (UInt32)AWDMetricId_MDNSResponder_DNSStatistics];
        }
    }
exit:
    return (err);
}

//===========================================================================================================================
//  MetricsUpdateUDNSStats
//===========================================================================================================================

mDNSexport void MetricsUpdateUDNSStats(const domainname *inQueryName, mDNSBool inAnswered, mDNSu32 inSendCount, mDNSu32 inLatencyMs, mDNSBool inForCellular)
{
    DNSStats *              stats;
    DNSDomainStatsRef       domainStats;
    int                     queryLabelCount;
    mDNSBool                isQueryInDomain;

    queryLabelCount = CountLabels(inQueryName);

    for (domainStats = gDomainStatsList; domainStats; domainStats = domainStats->next)
    {
        isQueryInDomain = mDNSfalse;
        if (strcmp(domainStats->domainStr, ".") == 0)
        {
            // All queries are in the root domain.
            isQueryInDomain = mDNStrue;
        }
        else
        {
            int                     skipCount;
            const domainname *      queryParentDomain;

            skipCount = queryLabelCount - domainStats->domainLabelCount;
            if (skipCount >= 0)
            {
                queryParentDomain = SkipLeadingLabels(inQueryName, skipCount);
                isQueryInDomain = SameDomainName(queryParentDomain, &domainStats->domain);
            }
        }

        if (isQueryInDomain)
        {
            stats = inForCellular ? &domainStats->statsCellular : &domainStats->stats;
            UpdateDNSStats(stats, inAnswered, inSendCount, inLatencyMs);
        }
    }
}

//===========================================================================================================================
//  CreateDomainStatsList
//===========================================================================================================================

mDNSlocal mStatus CreateDomainStatsList(DNSDomainStatsRef *outList)
{
    mStatus                 err;
    size_t                  i;
    DNSDomainStatsRef       domainStats;
    DNSDomainStatsRef *     p;
    DNSDomainStatsRef       list = NULL;

    p = &list;
    for (i = 0; i < countof(kUDNSStatsDomains); ++i)
    {
        err = DNSDomainStatsCreate(kUDNSStatsDomains[i], &domainStats);
        require_noerr_quiet(err, exit);

        *p = domainStats;
        p = &domainStats->next;
    }

    *outList = list;
    list = NULL;

exit:
    DNSDomainStatsFreeList(list);
    return (err);
}

//===========================================================================================================================
//  UpdateDNSStats
//===========================================================================================================================

mDNSlocal void UpdateDNSStats(DNSStats *inStats, mDNSBool inAnswered, mDNSu32 inQuerySendCount, mDNSu32 inLatencyMs)
{
    size_t      i;

    if (inAnswered)
    {
        i = (inQuerySendCount <= kUDNSStatsMaxQuerySendCount) ? inQuerySendCount : kUDNSStatsMaxQuerySendCount;
        inStats->answeredQuerySendCountBins[i]++;

        if (inQuerySendCount > 0)
        {
            for (i = 0; (i < countof(kResponseLatencyMsLimits)) && (inLatencyMs >= kResponseLatencyMsLimits[i]); ++i) {}
            inStats->responseLatencyBins[i]++;
        }
    }
    else if (inQuerySendCount > 0)
    {
        i = (inQuerySendCount <= kUDNSStatsMaxQuerySendCount) ? inQuerySendCount : kUDNSStatsMaxQuerySendCount;
        inStats->unansweredQuerySendCountBins[i]++;
    }
}

//===========================================================================================================================
//  SubmitAWDMetric
//===========================================================================================================================

mDNSlocal mStatus SubmitAWDMetric(void)
{
    mStatus                             err;
    BOOL                                success;
    DNSDomainStatsRef                   domainStats;
    DNSDomainStatsRef                   newDomainStatsList;
    DNSDomainStatsRef                   domainStatsList = NULL;
    AWDMetricContainer *                container       = nil;
    AWDMDNSResponderDNSStatistics *     metric          = nil;
    AWDDNSDomainStats *                 awdDomainStats  = nil;

    err = CreateDomainStatsList(&newDomainStatsList);
    require_noerr_quiet(err, exit);

    domainStatsList = gDomainStatsList;

    KQueueLock(&mDNSStorage);
    gDomainStatsList = newDomainStatsList;
    KQueueUnlock(&mDNSStorage, "SubmitAWDMetric");

    container = [gAWDServerConnection newMetricContainerWithIdentifier:AWDMetricId_MDNSResponder_DNSStatistics];
    require_action_quiet(container, exit, err = mStatus_UnknownErr);

    metric = [[AWDMDNSResponderDNSStatisticsSoft alloc] init];
    require_action_quiet(metric, exit, err = mStatus_UnknownErr);

    for (domainStats = domainStatsList; domainStats; domainStats = domainStats->next)
    {
        err = CreateAWDDNSDomainStats(&domainStats->stats, domainStats->domainStr, mDNSfalse, &awdDomainStats);
        require_noerr_quiet(err, exit);

        [metric addStats:awdDomainStats];
        [awdDomainStats release];
        awdDomainStats = nil;

        err = CreateAWDDNSDomainStats(&domainStats->statsCellular, domainStats->domainStr, mDNStrue, &awdDomainStats);
        require_noerr_quiet(err, exit);

        [metric addStats:awdDomainStats];
        [awdDomainStats release];
        awdDomainStats = nil;
    }

    container.metric = metric;
    success = [gAWDServerConnection submitMetric:container];
    LogMsg("SubmitAWDMetric: metric submission %s.", success ? "succeeded" : "failed" );
    err = success ? mStatus_NoError : mStatus_UnknownErr;

exit:
    [awdDomainStats release];
    [metric release];
    [container release];
    DNSDomainStatsFreeList(domainStatsList);
    return (err);
}

//===========================================================================================================================
//  DNSDomainStatsCreate
//===========================================================================================================================

mDNSlocal mStatus DNSDomainStatsCreate(const char *inDomain, DNSDomainStatsRef *outStats)
{
    mStatus                 err;
    DNSDomainStatsRef       obj;
    mDNSu8 *                ptr;

    obj = (DNSDomainStatsRef) calloc(1, sizeof(*obj));
    require_action_quiet(obj, exit, err = mStatus_NoMemoryErr);

    obj->domainStr = strdup(inDomain);
    require_action_quiet(obj, exit, err = mStatus_NoMemoryErr);

    // Initialize domainname for non-root domains.

    if (strcmp(obj->domainStr, ".") != 0)
    {
        ptr = MakeDomainNameFromDNSNameString(&obj->domain, obj->domainStr);
        require_action_quiet(ptr, exit, err = mStatus_Invalid);
        obj->domainLabelCount = CountLabels(&obj->domain);
    }

    *outStats = obj;
    obj = NULL;
    err = mStatus_NoError;

exit:
    if (obj) DNSDomainStatsFree(obj);
    return (err);
}

//===========================================================================================================================
//  DNSDomainStatsFree
//===========================================================================================================================

mDNSlocal void DNSDomainStatsFree(DNSDomainStatsRef inStats)
{
    if (inStats->domainStr) free(inStats->domainStr);
    free(inStats);
}

//===========================================================================================================================
//  DNSDomainStatsFreeList
//===========================================================================================================================

mDNSlocal void DNSDomainStatsFreeList(DNSDomainStatsRef inList)
{
    DNSDomainStatsRef       stats;

    while ((stats = inList) != NULL)
    {
        inList = stats->next;
        DNSDomainStatsFree(stats);
    }
}

//===========================================================================================================================
//  CreateAWDDNSDomainStats
//===========================================================================================================================

mDNSlocal mStatus CreateAWDDNSDomainStats(DNSStats *inStats, const char *inDomain, mDNSBool inIsCellType, AWDDNSDomainStats **outAWDStats)
{
    mStatus                 err;
    AWDDNSDomainStats *     awdStats    = nil;
    NSString *              domain      = nil;

    awdStats = [[AWDDNSDomainStatsSoft alloc] init];
    require_action_quiet(awdStats, exit, err = mStatus_UnknownErr);

    domain = [[NSString alloc] initWithUTF8String:inDomain];
    require_action_quiet(domain, exit, err = mStatus_UnknownErr);

    awdStats.domain = domain;
    awdStats.networkType = inIsCellType ? AWDDNSDomainStats_NetworkType_Cellular : AWDDNSDomainStats_NetworkType_NonCellular;

    [awdStats
        setAnsweredQuerySendCounts: inStats->answeredQuerySendCountBins
        count:                      (NSUInteger)countof(inStats->answeredQuerySendCountBins)];

    [awdStats
        setUnansweredQuerySendCounts:   inStats->unansweredQuerySendCountBins
        count:                          (NSUInteger)countof(inStats->unansweredQuerySendCountBins)];

    [awdStats
        setResponseLatencyMs:   inStats->responseLatencyBins
        count:                  (NSUInteger)countof(inStats->responseLatencyBins)];

    *outAWDStats = awdStats;
    awdStats = nil;
    err = mStatus_NoError;

exit:
    [domain release];
    [awdStats release];
    return (err);
}

//===========================================================================================================================
//  LogDNSStats
//===========================================================================================================================

#define Percent(N, D)       ((N) * 100) / (D), (((N) * 10000) / (D)) % 100
#define PercentFmt          "%3u.%02u"
#define LogStat(LABEL, COUNT, ACCUMULATOR, TOTAL) \
    LogMsgNoIdent("%s %5u " PercentFmt " " PercentFmt, (LABEL), (COUNT), Percent(COUNT, TOTAL), Percent(ACCUMULATOR, TOTAL))

mDNSlocal void LogDNSStats(const DNSStats *inStats)
{
    uint32_t        total;
    uint32_t        totalUnanswered;
    size_t          i;
    char            label[16];

    totalUnanswered = 0;
    for (i = 0; i < countof(inStats->unansweredQuerySendCountBins); ++i)
    {
        totalUnanswered += inStats->unansweredQuerySendCountBins[i];
    }

    total = 0;
    for (i = 0; i <= countof(inStats->answeredQuerySendCountBins); ++i)
    {
        total += inStats->answeredQuerySendCountBins[i];
    }

    LogMsgNoIdent("Answered questions   %5u", total);
    LogMsgNoIdent("Unanswered questions %5u", totalUnanswered);
    LogMsgNoIdent("+++ Number of queries sent +++");
    if (total > 0)
    {
        uint32_t        accumulator = 0;

        for (i = 0; i < countof(inStats->answeredQuerySendCountBins); ++i)
        {
            uint32_t            count;
            const char *        suffix;

            count = inStats->answeredQuerySendCountBins[i];
            accumulator += count;
            suffix = (i < (countof(inStats->answeredQuerySendCountBins) - 1)) ? " " : "+";
            snprintf(label, sizeof(label), "%2d%s", (int)i, suffix);
            LogStat(label, count, accumulator, total);
        }
    }
    else
    {
        LogMsgNoIdent("No data.");
    }

    total = 0;
    for (i = 0; i < countof(inStats->responseLatencyBins); ++i)
    {
        total += inStats->responseLatencyBins[i];
    }

    LogMsgNoIdent("+++++++ Response times +++++++");
    if (total > 0)
    {
        uint32_t        accumulator = 0;

        for (i = 0; i < countof(inStats->responseLatencyBins); ++i)
        {
            uint32_t        count;

            count = inStats->responseLatencyBins[i];
            accumulator += count;
            if (i < countof(kResponseLatencyMsLimits))
            {
                snprintf(label, sizeof(label), "< %5u ms", kResponseLatencyMsLimits[i]);
            }
            else
            {
                snprintf(label, sizeof(label), "<     ∞ ms");
            }
            LogStat(label, count, accumulator, total);
            if (accumulator == total) break;
        }
    }
    else
    {
        LogMsgNoIdent("No data.");
    }
}

//===========================================================================================================================
//  LogMetrics
//===========================================================================================================================

mDNSexport void LogMetrics(void)
{
    DNSDomainStatsRef       domainStats;

    LogMsgNoIdent("---- DNS stats by domain -----");

    for (domainStats = gDomainStatsList; domainStats; domainStats = domainStats->next)
    {
        LogMsgNoIdent("Domain: %s (non-cellular)", domainStats->domainStr);
        LogDNSStats(&domainStats->stats);
        LogMsgNoIdent("Domain: %s (cellular)", domainStats->domainStr);
        LogDNSStats(&domainStats->statsCellular);
    }
}
#endif // TARGET_OS_EMBEDDED
