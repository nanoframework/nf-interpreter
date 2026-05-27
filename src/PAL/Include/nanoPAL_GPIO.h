//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NANOPAL_GPIO_H
#define NANOPAL_GPIO_H

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

#endif // NANOPAL_GPIO_H
