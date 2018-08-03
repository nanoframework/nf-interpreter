//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// lwIP options related with SNTP

#ifndef __NF_LWIPOPTS_SNTP_H__
#define __NF_LWIPOPTS_SNTP_H__

// SNTP servers can be IP or full address and are resolved at 
#define SNTP_SERVER_DNS         1

// use two servers
#define SNTP_MAX_SERVERS        2

// set update delay (in milliseconds)
#define SNTP_UPDATE_DELAY       3600000

// requiring time precision on the milliseconds
#define SNTP_SET_SYSTEM_TIME_US

// retry timeout (15 minutes)
#define SNTP_RETRY_TIMEOUT      900000

#define SNTP_SET_SYSTEM_TIME_US(sec, us)    

#endif // __NF_LWIPOPTS_SNTP_H__
