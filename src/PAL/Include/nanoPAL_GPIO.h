//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOPAL_GPIO_H_
#define _NANOPAL_GPIO_H_ 1

#include <nanoWeak.h>

#ifdef __cplusplus
extern "C" {
#endif

/// <summary>
/// Performs initialization of GPIO pins and associated drivers.
/// </summary>
void    GPIO_Initialize();
void    GPIO_Uninitialize();

#ifdef __cplusplus
}
#endif

#endif //_NANOPAL_GPIO_H_
