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
//
// Exception: the SWO/ITM log output in nf_mcuboot_log_write() needs CMSIS core
// registers (ITM, TPI, CoreDebug, DBGMCU), STM32_SYSCLK and PAL, so that HAL
// dependent path is guarded by NF_MCUBOOT_BOOTLOADER and only compiled for the
// standalone bootloader (nanoMcubooter.elf). The nf_mcuboot_port build, which
// has no HAL include paths, takes the HAL-free no-op branch below.

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#if defined(NF_MCUBOOT_BOOTLOADER)

#include <hal.h>
#include <stdbool.h>

// number of attempts to write to the ITM port before quitting
// bounded spin so a disconnected SWO viewer can't hang the boot
#define ITM_WRITE_ATTEMPTS 10000

// SWO/ITM (ARM Serial Wire Output) log output for the standalone bootloader.
// Boards may still override this in their BSP file.
__attribute__((weak)) void nf_mcuboot_log_write(const char *msg)
{
    static bool swoInitialized = false;

    if (!swoInitialized)
    {
        // set SWO pin (PB3) to alternate mode (0 == the status after RESET)
        // in case it's being set to a different function in board config
        palSetPadMode(GPIOB, 0x03, PAL_MODE_ALTERNATE(0));

        // enable trace in core debug register
        CoreDebug->DEMCR = CoreDebug_DEMCR_TRCENA_Msk;

        // enable trace pins with async trace
        DBGMCU->CR |= DBGMCU_CR_TRACE_IOEN | ~DBGMCU_CR_TRACE_MODE;

        // default speed for ST-Link is 2000kHz
        uint32_t swoSpeed = 2000000;
        uint32_t swoPrescaler = (STM32_SYSCLK / swoSpeed) + 1;

        // Write the TPIU Current Port Size Register (default is 0x1 for a 1-bit port size)
        TPI->CSPSR = 1;
        // Write TPIU Formatter and Flush Control Register
        TPI->FFCR = 0x100;
        // Write the TPIU Select Pin Protocol to select async NRZ mode (UART like)
        TPI->SPPR = 2;

        // Write clock prescaler
        TPI->ACPR = swoPrescaler;

        // unlock Write Access to the ITM registers
        ITM->LAR = 0xC5ACCE55;

        // Trace bus ID for TPIU, enable events, enable sync packets, time stamp
        // enable and trace main enable
        ITM->TCR = ITM_TCR_TraceBusID_Msk | ITM_TCR_SWOENA_Msk | ITM_TCR_SYNCENA_Msk | ITM_TCR_ITMENA_Msk;

        // enable stimulus port 0 (the only port we use)
        ITM->TER |= 1;
        ITM->TPR = ITM_TPR_PRIVMASK_Msk;

        swoInitialized = true;
    }

    while (*msg != '\0')
    {
        uint32_t retryCounter = ITM_WRITE_ATTEMPTS;
        bool okToTx = (ITM->PORT[0U].u32 == 1UL);

        // wait (with timeout) until ITM port TX buffer is available
        while (!okToTx && (retryCounter > 0))
        {
            // do... nothing
            __NOP();

            // decrease retry counter
            retryCounter--;

            // check again
            okToTx = (ITM->PORT[0U].u32 == 1UL);
        }

        if (okToTx)
        {
            ITM->PORT[0U].u8 = (uint8_t)*msg;
        }

        msg++;
    }
}

#else

// Weak default log output: no-op.
// Boards that want MCUboot log output override this in their BSP file
// (e.g. route messages via chprintf to a UART or USB-CDC channel).
__attribute__((weak)) void nf_mcuboot_log_write(const char *msg)
{
    (void)msg;
}

#endif // NF_MCUBOOT_BOOTLOADER

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
