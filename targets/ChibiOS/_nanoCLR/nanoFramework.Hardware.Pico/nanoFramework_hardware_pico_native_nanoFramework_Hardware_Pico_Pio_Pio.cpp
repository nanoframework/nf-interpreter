//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_pico_native.h"
#include "nanoFramework_hardware_pico_native_target.h"
#include <hal.h>

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_Pio::get_MinIndex___STATIC__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    stack.SetResult_I4(PIO_MIN_BLOCK);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_Pio::get_BlockCount___STATIC__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    stack.SetResult_I4(PIO_MAX_BLOCK + 1);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_Pio::get_MaxPin___STATIC__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    stack.SetResult_I4(PIO_MAX_PIN);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_Pio::get_SystemClock___STATIC__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    stack.SetResult_I4(static_cast<CLR_INT32>(RP_CORE_CLK));

    NANOCLR_NOCLEANUP_NOLABEL();
}
