//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_SNTP_OPTS_H
#define TARGET_SNTP_OPTS_H

#define SNTP_SERVER0_DEFAULT_ADDRESS "0.pool.ntp.org"
#define SNTP_SERVER1_DEFAULT_ADDRESS "1.pool.ntp.org"

// update delay (default 1 hour)
// (value in seconds)
// Must wait at least 15 sec to retry NTP server (RFC 4330)
#define SNTP_UPDATE_DELAY (60 * 60)

// better have a startup delay because we can have DHCP enabled (default 2 seconds)
// value in seconds
#define SNTP_STARTUP_DELAY (2)

// retry timeout (15 minutes)
// value in seconds
#define SNTP_RETRY_TIMEOUT (15 * 60)

#endif // TARGET_SNTP_OPTS_H
