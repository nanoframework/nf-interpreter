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

// startup delay (default 2 seconds)
// (value in milliseconds)
// Remarks: because we could have DHCP enabled
//   we allow time for the IP to be established
//   but also need to take into account that NetworkHelper is likely to timeout
#define SNTP_STARTUP_DELAY 2000

// retry timeout (15 minutes)
// (value in milliseconds)
#define SNTP_RETRY_TIMEOUT 900000
