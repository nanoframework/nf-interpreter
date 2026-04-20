//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// MCUboot logging stubs for nanoFramework bare-metal builds.
//
// MCUBOOT_HAVE_LOGGING is intentionally NOT defined for the bare-metal STM32
// bootloader.  This file exists so that MCUboot v2.3.0 headers (sha.h etc.)
// that unconditionally include <mcuboot_config/mcuboot_logging.h> can resolve
// the include.  All macros are no-ops.
//

#ifndef __MCUBOOT_LOGGING_H__
#define __MCUBOOT_LOGGING_H__

#define MCUBOOT_LOG_MODULE_DECLARE(module)
#define MCUBOOT_LOG_MODULE_REGISTER(module)

#define MCUBOOT_LOG_ERR(...)
#define MCUBOOT_LOG_WRN(...)
#define MCUBOOT_LOG_INF(...)
#define MCUBOOT_LOG_DBG(...)
#define MCUBOOT_LOG_SIM(...)

#endif // __MCUBOOT_LOGGING_H__
