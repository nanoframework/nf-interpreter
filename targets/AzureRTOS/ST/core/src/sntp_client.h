/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _SNTP_CLIENT_H
#define _SNTP_CLIENT_H

#include <tx_api.h>

ULONG sntp_time_get();
UINT sntp_time(ULONG* unix_time);

UINT sntp_sync_wait();
UINT sntp_start();
UINT sntp_stop();

#endif // _SNTP_CLIENT_H
