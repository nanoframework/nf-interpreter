//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NF_WIRELESS_H_
#define _NF_WIRELESS_H_

#include <nanoHAL.h>
#include <nf_netxduo.h>

#ifdef __cplusplus
 extern "C" {
#endif

int NF_Wireless_Open(HAL_Configuration_NetworkInterface *config);
bool NF_Wireless_Close();

#ifdef __cplusplus
}
#endif

#endif // _NF_WIRELESS_H_
