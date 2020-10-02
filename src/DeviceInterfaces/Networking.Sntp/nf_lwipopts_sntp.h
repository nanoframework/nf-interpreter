//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// lwIP options related with SNTP

#ifndef __NF_LWIPOPTS_SNTP_H__
#define __NF_LWIPOPTS_SNTP_H__

#include <time.h>
#include <nanoWeak.h>
#include <target_lwip_sntp_opts.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void SetSystemTimeFromUnixEpoch(uint32_t seconds);

#ifdef __cplusplus
}
#endif

// SNTP servers can be IP or full address and are resolved at 
#define SNTP_SERVER_DNS         1

// use two servers
#define SNTP_MAX_SERVERS        2

// call to set the system time with seconds (Unix Epoch time)
#define SNTP_SET_SYSTEM_TIME    SetSystemTimeFromUnixEpoch   

// need to declare this here to keep compiler happy 
__nfweak char* ctime (const time_t * timer);

#endif // __NF_LWIPOPTS_SNTP_H__
