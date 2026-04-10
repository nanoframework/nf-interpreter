//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// RP2040 has no RTC so time is bogus until SNTP syncs.
// Send the first NTP request immediately (no startup delay).
#undef SNTP_STARTUP_DELAY
#define SNTP_STARTUP_DELAY 0
