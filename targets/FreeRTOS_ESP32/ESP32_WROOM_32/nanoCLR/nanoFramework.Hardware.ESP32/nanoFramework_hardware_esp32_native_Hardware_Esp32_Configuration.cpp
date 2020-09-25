//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_esp32_native.h"

#include <Esp32_os.h>
#include "Esp32_DeviceMapping.h"

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Configuration::NativeSetPinFunction___STATIC__VOID__I4__I4( CLR_RT_StackFrame& stack )
{
	NANOCLR_HEADER();
	{
		CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

		uint32_t pin = (int32_t)stack.Arg0().NumericByRef().s4;
		int32_t function = (int32_t)stack.Arg1().NumericByRef().s4;

		Esp32_SetMappedDevicePins(pin, function);
	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Configuration::NativeGetPinFunction___STATIC__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
	{
		CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);
		int32_t function = (int32_t)stack.Arg0().NumericByRef().s4;

		// Return current pin used for function
		int32_t pin = (int32_t)Esp32_GetMappedDevicePinsWithFunction(function);
		stack.SetResult_I4(pin);
	}
    NANOCLR_NOCLEANUP();
}
