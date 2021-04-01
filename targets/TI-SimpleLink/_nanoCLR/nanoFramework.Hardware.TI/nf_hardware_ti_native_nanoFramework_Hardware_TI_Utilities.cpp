//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_hardware_ti_native.h"

HRESULT Library_nf_hardware_ti_native_nanoFramework_Hardware_TI_Utilities::NativeGetIeeeAddress___STATIC__SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t *ieeeAddress;

    // create pointer to return object
    CLR_RT_HeapBlock &top = stack.PushValueAndClear();

    // dereference the byte array from the static field
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, 8, g_CLR_RT_WellKnownTypes.m_UInt8));

    ieeeAddress = top.DereferenceArray()->GetFirstElement();
    FAULT_ON_NULL(ieeeAddress);

    // OK to ignore return result because the only possible error is thrown on null pointer
    EasyLink_getIeeeAddr(ieeeAddress);

    NANOCLR_NOCLEANUP();
}
