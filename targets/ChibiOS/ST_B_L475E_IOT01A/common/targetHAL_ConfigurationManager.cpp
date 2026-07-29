//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_v2.h>
#include <nanoWeak.h>

// Strong implementation replacing ChibiOS 'weak' InitialiseNetworkDefaultConfig.
// This target doesn't have a network interface enabled, so there is no default
// network configuration to provide (no MAC address available).
bool InitialiseNetworkDefaultConfig(HAL_Configuration_NetworkInterface *pconfig, uint32_t configurationIndex)
{
    (void)pconfig;
    (void)configurationIndex;

    return false;
}
