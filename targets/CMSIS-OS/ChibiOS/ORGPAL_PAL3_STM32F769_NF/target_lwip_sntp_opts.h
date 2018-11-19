//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#define SNTP_SERVER_DEFAULT_ADDRESS "0.pool.ntp.org"

// update delay (default 1 hour)
// (value in milliseconds)
#define SNTP_UPDATE_DELAY       3600000

// better have a startup delay because we can have DHCP enabled (default 30 seconds)
// value in milliseconds
#define SNTP_STARTUP_DELAY      30000

// retry timeout (15 minutes)
// value in milliseconds
#define SNTP_RETRY_TIMEOUT      900000
