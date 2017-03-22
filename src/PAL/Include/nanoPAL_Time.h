//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOPAL_TIME_H_
#define _NANOPAL_TIME_H_ 1

#include <nanoWeak.h>
#include <netmf_errors.h>
#include <targetPAL_Time.h>

#ifdef __cplusplus
extern "C" {
#endif

/// <summary>
/// Initializes PAL Time drivers, must be called before any of the Time_* PAL
/// methods could be used.
/// </summary>
__nfweak HRESULT    Time_Initialize  (                     );

#ifdef __cplusplus
}
#endif

#endif //_NANOPAL_TIME_H_
