//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NANOHAL_RTOS_H
#define NANOHAL_RTOS_H

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * Function to call RTOS API that implements yield/sleep or equivalent allowing the RTOS to run threads other than
     * the CLR. Implemented as weak so that it can be overridden by a platform specific implementation if required.
     */
    void RtosYield();

#ifdef __cplusplus
}
#endif

#endif // NANOHAL_RTOS_H
