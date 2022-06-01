//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#define SNTP_SERVER0_DEFAULT_ADDRESS "0.pool.ntp.org"
#define SNTP_SERVER1_DEFAULT_ADDRESS "1.pool.ntp.org"

// update delay (default 1 hour)
// (value in milliseconds)
// can't use this for ESP32 build because IDF defines it's own
#ifndef PLATFORM_ESP32
#define SNTP_UPDATE_DELAY 3600000
#endif

// better have a startup delay because we can have DHCP enabled (default 15 seconds)
// value in milliseconds
#define SNTP_STARTUP_DELAY 15 * 1000

// retry timeout (15 minutes)
// value in milliseconds
#define SNTP_RETRY_TIMEOUT 900000
