//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <targetHAL.h>
#include <ti/sysbios/knl/Task.h>

void RtosYield()
{
    Task_yield();
}
