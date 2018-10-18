//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_esp32_native.h"

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Logging::NativeSetLogLevel___STATIC__VOID__STRING__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        const char * szTag = stack.Arg0().RecoverString();
        esp_log_level_t logLevel = (esp_log_level_t)stack.Arg1().NumericByRef().s4;
 
        esp_log_level_set(szTag, logLevel);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

