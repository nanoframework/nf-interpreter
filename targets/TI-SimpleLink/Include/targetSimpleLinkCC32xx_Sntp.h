//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Texas Instruments Incorporated.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_SNTP_H_
#define _TARGET_SNTP_H_ 1

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <target_sntp_opts.h>
#include <target_platform.h>

// default SNTP server address
#ifndef SNTP_SERVER_DEFAULT_ADDRESS
#define SNTP_SERVER_DEFAULT_ADDRESS "0.pool.ntp.org"
#endif

// Must wait at least 15 sec to retry NTP server (RFC 4330)
#ifndef SNTP_UPDATE_DELAY
#define SNTP_UPDATE_DELAY       (15)
#endif

// better have a startup delay because we can have DHCP enabled (default 30 seconds)
// value in seconds
#ifndef SNTP_STARTUP_DELAY
#define SNTP_STARTUP_DELAY      (30)
#endif

// retry timeout (15 minutes)
// value in seconds
#ifndef SNTP_RETRY_TIMEOUT
#define SNTP_RETRY_TIMEOUT      (15 * 60)
#endif

#define NTP_SERVERS 1
#define NTP_SERVER_PORT 123

//  Time to wait for reply from server (seconds)
#define NTP_REPLY_WAIT_TIME 5

#define TIME_BASEDIFF        ((((uint32_t)70 * 365 + 17) * 24 * 3600))
#define TIME_NTP_TO_LOCAL(t) ((t) - TIME_BASEDIFF)

// SNTP Error Codes
// Failed to resolve Host address
#define SNTP_EGETHOSTBYNAMEFAIL     (-100)

// Input arguments are invalid
#define SNTP_EINVALIDARGS           (-101)

// Failed to create a socket
#define SNTP_ESOCKCREATEFAIL        (-102)

// The input socket address is not of AF_INET (IPv4) or AF_INET6
//         (IPv6) family type
#define SNTP_EINVALIDFAMILY         (-103)

// Failed to set receive timeout on socket
#define SNTP_ESOCKOPTFAIL           (-104)

// Failed to connect to the NTP server
#define SNTP_ECONNECTFAIL           (-105)

// Failed to send a time request to the NTP server
#define SNTP_ESENDFAIL              (-106)

// Failed to recieve the new time from the NTP server
#define SNTP_ERECVFAIL              (-107)

// NTP Server requests to reduce the update rate (RFC 5905 kiss code RATE)

#define SNTP_ERATEBACKOFF           (-108)

// NTP Server invalid or server requests to end all communications (RFC
// 5905 kiss code DENY or RSTR)
#define SNTP_EFATALNORETRY          (-109)

// Server response with stratum == 0, with no kiss code
#define SNTP_EINVALIDRESP           (-110)

// KOD error code: rate exceeded, server requesting NTP client to back off
#define SNTP_KOD_RATE_STR "RATE"
#define SNTP_KOD_RATE_CODE 3

// KOD error code: access denied, server requests client to end all comm
#define SNTP_KOD_DENY_STR "DENY"
#define SNTP_KOD_DENY_CODE 2

// KOD error code: access denied, server requests client to end all comm
#define SNTP_KOD_RSTR_STR "RSTR"
#define SNTP_KOD_RSTR_CODE 1

// Size of KOD error codes
#define SNTP_KOD_ERROR_CODE_SIZE 4

// Use NTP version 4
#define SNTP_VERSION 4

// Flag value for unsync'ed leap indicator field, signifying server error
#define SNTP_NOSYNC 3

// NTP mode defined in RFC 4330
#define SNTP_MODE_CLIENT 3

// SNTP Header (as specified in RFC 4330)
typedef struct _SNTP_Header_t_
{
    //
    //  'flags' stores three values:
    //
    //    - 2 bit Leap Indicator (LI)
    //    - 3 bit Version Number (VN)
    //    - 3 bit Mode.
    
    uint8_t flags;
    uint8_t stratum;
    uint8_t poll;
    int8_t  precision;
    int32_t  rootDelay;
    uint32_t  rootDispersion;
    uint32_t  referenceID;

    // NTP time stamps
    uint32_t referenceTS[2];
    uint32_t originateTS[2];
    uint32_t receiveTS[2];
    uint32_t transmitTS[2];
} SNTP_Header_t;

struct sntp_server
{
  char* name;
};

#ifdef __cplusplus
extern "C" {
#endif

void sntp_init(void);
void sntp_stop(void);
bool sntp_enabled(void);
char* sntp_getservername(int index);
void sntp_setservername(int index, char* server);

#ifdef __cplusplus
}
#endif

#endif //_TARGET_SNTP_H_
