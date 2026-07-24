//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// MCUboot logging backend, shared by the standalone bootloader (nanoMcubooter.elf)
// and the nanoCLR-side porting library (nf_mcuboot_port).
//
// bootutil_public.c/bootutil_misc.c are compiled into both binaries, so whenever
// NF_MCUBOOT_LOG_LEVEL is above OFF their MCUBOOT_LOG_* macros need
// nf_mcuboot_log_emit() to be linkable from both. Kept free of ChibiOS/HAL
// includes (unlike mcuboot_hal_stubs.c) so it can be built into nf_mcuboot_port,
// which does not have the board HAL include paths.

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

// Weak default log output: no-op.
// Boards that want MCUboot log output override this in their BSP file
// (e.g. route messages via chprintf to a UART or USB-CDC channel).
__attribute__((weak)) void nf_mcuboot_log_write(const char *msg)
{
    (void)msg;
}

void nf_mcuboot_log_emit(const char *prefix, const char *fmt, ...)
{
    char buffer[128];
    char *out = buffer;
    const char *in = prefix;
    size_t left;
    int len;
    va_list args;

    while (*in != '\0')
    {
        *out++ = *in++;
    }

    left = sizeof(buffer) - (size_t)(out - buffer);
    va_start(args, fmt);
    len = vsnprintf(out, left, fmt, args);
    va_end(args);

    if (len > 0)
    {
        if ((size_t)len >= (left - 2U))
        {
            out = &buffer[sizeof(buffer) - 3U];
        }
        else
        {
            out += len;
        }
    }

    *out++ = '\r';
    *out++ = '\n';
    *out = '\0';

    nf_mcuboot_log_write(buffer);
}
