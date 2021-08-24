//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NANOPAL_TIME_H
#define NANOPAL_TIME_H

#include <nanoWeak.h>
#include <targetPAL_Time.h>

#ifdef __cplusplus
extern "C" {
#endif

/// <summary>
/// Initializes PAL Time drivers, must be called before any of the Time_* PAL
/// methods could be used.
/// </summary>
HRESULT    Time_Initialize    (                            );
HRESULT    Time_Uninitialize  (                            );
void       Time_SetCompare    ( uint64_t compareValueTicks );

#ifdef __cplusplus
}
#endif

#endif //NANOPAL_TIME_H
