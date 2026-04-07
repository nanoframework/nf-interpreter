//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <targetHAL.h>
#include <tx_api.h>

void RtosYield()
{
    // OK to call relinquish here despite it will only execute threads with same or higher priority
    tx_thread_relinquish();
}
