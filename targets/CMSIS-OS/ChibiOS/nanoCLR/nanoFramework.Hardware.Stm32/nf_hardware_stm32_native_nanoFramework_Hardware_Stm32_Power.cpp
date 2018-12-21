//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_hardware_stm32_native.h"
#include <nanoCLR_Hardware.h>

HRESULT Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Power::EnterStandbyMode___STATIC__VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    (void)stack;

    // set flags to stop debug...
    CLR_EE_DBG_SET( RebootPending );
    // ... reboot CLR so execution ends gracefully ...
    CLR_EE_REBOOT_CLR;
    // ... and set power level to OFF
    g_CLR_HW_Hardware.m_powerLevel = PowerLevel__Off;

    NANOCLR_NOCLEANUP_NOLABEL();
}
