//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Using https://www.nongnu.org/lwip/2_1_x/group__sntp__opts.html as a reference

#define SNTP_SERVER0_DEFAULT_ADDRESS "0.pool.ntp.org"
#define SNTP_SERVER1_DEFAULT_ADDRESS "1.pool.ntp.org"

// can't use this for ESP32 build because IDF defines it's own
#ifndef PLATFORM_ESP32
// update delay (default 1 hour)
// (value in milliseconds)
#define SNTP_UPDATE_DELAY 3600000
#endif

// #define SNTP_CHECK_RESPONSE   0
// #define SNTP_COMP_ROUNDTRIP 0
// #define SNTP_PORT   LWIP_IANA_PORT_SNTP

// the maximum time to wait for a response from the server
// (value in milliseconds)
#define SNTP_RECV_TIMEOUT 2000 // it should be 15000, FIXME: probably caused by a change in IP Address!

// startup delay (default 5 seconds)
// (value in milliseconds)
// According to the RFC, this shall be a random delay between 1 and 5 minutes to prevent load peaks. This can be defined
// to a random generation function, which must return the delay in milliseconds as u32_t. Turned off by default.
#define SNTP_STARTUP_DELAY 5000 // FIXME: we ignore the RFC! and set to 2 seconds
// But we also could add some randomness!
// if we were, we need to change SNTP_STARTUP_DELAY to 1
// #define SNTP_STARTUP_DELAY_FUNC (LWIP_RAND() % 5000)

// Default retry timeout (15 seconds) if the response received is invalid. This is doubled with each retry until
// SNTP_RETRY_TIMEOUT_MAX is reached. (value in milliseconds)
#define SNTP_RETRY_TIMEOUT SNTP_RECV_TIMEOUT

// maximum retry timeout (15 minutes) before giving up.
// (value in milliseconds)
#define SNTP_RETRY_TIMEOUT_MAX (SNTP_RETRY_TIMEOUT * 10)
