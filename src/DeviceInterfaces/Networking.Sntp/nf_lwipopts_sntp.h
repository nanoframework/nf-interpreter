//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// lwIP options related with SNTP

#ifndef __NF_LWIPOPTS_SNTP_H__
#define __NF_LWIPOPTS_SNTP_H__

#include <time.h>
#include <nanoWeak.h>

extern void SetSystemTimeFromUnixEpoch(uint32_t seconds);

// SNTP servers can be IP or full address and are resolved at 
#define SNTP_SERVER_DNS         1

// use two servers
#define SNTP_MAX_SERVERS        2

// set update delay (in milliseconds)
#define SNTP_UPDATE_DELAY       3600000

// need a startup delay because we can have DHCP enabled
#define SNTP_STARTUP_DELAY      30000

// retry timeout (15 minutes)
#define SNTP_RETRY_TIMEOUT      900000

// call to set the system time with seconds (Unix Epoch time)
#define SNTP_SET_SYSTEM_TIME    SetSystemTimeFromUnixEpoch   

// need to declare this here to keep compiler happy 
__nfweak char* ctime (const time_t * timer);

#endif // __NF_LWIPOPTS_SNTP_H__
