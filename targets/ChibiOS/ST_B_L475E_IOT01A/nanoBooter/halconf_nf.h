//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef HALCONF_NF_H
#define HALCONF_NF_H

// enables STM32 Flash driver (internal flash used for deployment/config block storage)
#if !defined(HAL_NF_USE_STM32_FLASH)
#define HAL_NF_USE_STM32_FLASH TRUE
#endif

#endif // HALCONF_NF_H
