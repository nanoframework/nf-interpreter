//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_hardware_stm32_native.h"

HRESULT Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Utilities::NativeGetDeviceUniqueId___STATIC__VOID__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // dereference the data buffer from the argument
        CLR_RT_HeapBlock_Array* buffer = stack.Arg0().DereferenceArray();

        // get a the pointer to the byffer by using the first element of the array
        uint8_t* data = buffer->GetFirstElement();

        // 96 bit unique device ID => 12 bytes
        // memory copy from the address pointed by UID_BASE define (from STM32 HAL)
        memcpy(data, ((uint8_t *)UID_BASE), 12);

    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Utilities::NativeGetDeviceId___STATIC__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // this ID is provided by debug MCU register
        uint32_t deviceId = READ_BIT(DBGMCU->IDCODE, DBGMCU_IDCODE_DEV_ID);

        stack.SetResult_U4(deviceId);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Utilities::NativeGetDeviceRevisionId___STATIC__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    { 
        // this ID is provided by debug MCU register
        // need to right shift 16 bits because it shows as 0xFFFF0000
        uint32_t deviceRevisionId = (READ_BIT(DBGMCU->IDCODE, DBGMCU_IDCODE_REV_ID) >> DBGMCU_IDCODE_REV_ID_Pos);

        stack.SetResult_U4(deviceRevisionId);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
