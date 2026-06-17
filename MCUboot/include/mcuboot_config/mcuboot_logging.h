//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// MCUboot logging for nanoFramework MCUboot builds.
//
// Log level numeric values (must match the MCUBOOT_LOG_LEVEL definition in
// mcuboot_config.h):
//   0 = OFF, 1 = ERROR, 2 = WARNING, 3 = INFO, 4 = DEBUG
//
// Output backend: nf_mcuboot_log_write(const char *msg).
// The weak default (in targets/ChibiOS/_mcuboot/mcuboot_hal_stubs.c) is a
// no-op.  Boards that want log output implement it in their MCUboot BSP file,
// e.g. to route messages via chprintf to a UART or USB-CDC channel.
//

#ifndef __MCUBOOT_LOGGING_H__
#define __MCUBOOT_LOGGING_H__

#define MCUBOOT_LOG_LEVEL_OFF     0
#define MCUBOOT_LOG_LEVEL_ERROR   1
#define MCUBOOT_LOG_LEVEL_WARNING 2
#define MCUBOOT_LOG_LEVEL_INFO    3
#define MCUBOOT_LOG_LEVEL_DEBUG   4

#ifndef MCUBOOT_LOG_LEVEL
#define MCUBOOT_LOG_LEVEL MCUBOOT_LOG_LEVEL_OFF
#endif

// Board-supplied output function; weak default in mcuboot_hal_stubs.c is a no-op.
extern void nf_mcuboot_log_write(const char *msg);

// Format and emit one log line via nf_mcuboot_log_write().
extern void nf_mcuboot_log_emit(const char *prefix, const char *fmt, ...);

#define _NF_LOG(_prefix, _fmt, ...) nf_mcuboot_log_emit(_prefix, _fmt, ##__VA_ARGS__)

#if MCUBOOT_LOG_LEVEL >= MCUBOOT_LOG_LEVEL_ERROR
#define MCUBOOT_LOG_ERR(_fmt, ...) _NF_LOG("[ERR] ", _fmt, ##__VA_ARGS__)
#else
#define MCUBOOT_LOG_ERR(_fmt, ...)
#endif

#if MCUBOOT_LOG_LEVEL >= MCUBOOT_LOG_LEVEL_WARNING
#define MCUBOOT_LOG_WRN(_fmt, ...) _NF_LOG("[WRN] ", _fmt, ##__VA_ARGS__)
#else
#define MCUBOOT_LOG_WRN(_fmt, ...)
#endif

#if MCUBOOT_LOG_LEVEL >= MCUBOOT_LOG_LEVEL_INFO
#define MCUBOOT_LOG_INF(_fmt, ...) _NF_LOG("[INF] ", _fmt, ##__VA_ARGS__)
#else
#define MCUBOOT_LOG_INF(_fmt, ...)
#endif

#if MCUBOOT_LOG_LEVEL >= MCUBOOT_LOG_LEVEL_DEBUG
#define MCUBOOT_LOG_DBG(_fmt, ...) _NF_LOG("[DBG] ", _fmt, ##__VA_ARGS__)
#else
#define MCUBOOT_LOG_DBG(_fmt, ...)
#endif

#define MCUBOOT_LOG_SIM(_fmt, ...)

#define MCUBOOT_LOG_MODULE_DECLARE(module)
#define MCUBOOT_LOG_MODULE_REGISTER(module)

#endif // __MCUBOOT_LOGGING_H__
