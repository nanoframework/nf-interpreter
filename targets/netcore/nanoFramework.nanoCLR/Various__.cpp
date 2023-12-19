//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"
#include <target_common.h>
#include "platform_selector.h"

#include <lwip/init.h>

bool Network_Initialize()
{
    lwip_init();

    return true;
}

uint64_t CPU_MillisecondsToTicks(uint64_t ticks)
{
    return ticks * 10 * 1000;
}
