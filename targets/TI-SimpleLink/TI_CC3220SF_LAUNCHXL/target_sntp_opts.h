//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_SNTP_OPTS_H_
#define _TARGET_SNTP_OPTS_H_ 1

#define SNTP_SERVER_DEFAULT_ADDRESS "0.pool.ntp.org"

// update delay (default 1 hour)
// (value in seconds)
// Must wait at least 15 sec to retry NTP server (RFC 4330)
#define SNTP_UPDATE_DELAY       (60 * 60)

// no startup delay for SNTP
// value in seconds
#define SNTP_STARTUP_DELAY      (0)

// retry timeout
// value in seconds
#define SNTP_RETRY_TIMEOUT      (10)

#endif // _TARGET_SNTP_OPTS_H_
