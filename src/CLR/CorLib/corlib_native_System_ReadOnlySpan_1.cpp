//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "CorLib.h"

HRESULT Library_corlib_native_System_ReadOnlySpan_1::NativeReadOnlySpanConstructor___VOID__SZARRAY_GENERICTYPE__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int32_t start;
    int32_t length;

    CLR_RT_TypeDescriptor descDst{};
    CLR_RT_TypeDef_Index sourceType{};
    CLR_RT_HeapBlock_Array *destinationArray;
    CLR_RT_HeapBlock_Array *sourceArray = stack.Arg1().DereferenceArray();
    CLR_RT_HeapBlock *thisSpan = stack.This();

    // get working values
    start = stack.Arg2().NumericByRefConst().s4;
    length = stack.Arg3().NumericByRefConst().s4;

    {
        // get type of the source array
        NANOCLR_CHECK_HRESULT(descDst.InitializeFromObject(*sourceArray));
        descDst.GetElementType(descDst);

        sourceType.data = descDst.m_handlerCls.data;

        CLR_RT_HeapBlock &refArray = thisSpan[FIELD___array];
        CLR_RT_HeapBlock_Array::CreateInstance(refArray, length, sourceType);

        // get pointer to the array
        destinationArray = thisSpan[FIELD___array].DereferenceArray();

        // protect from GC
        CLR_RT_ProtectFromGC gc1(*sourceArray);
        CLR_RT_ProtectFromGC gc2(refArray);

        // copy array
        CLR_RT_HeapBlock_Array::Copy(sourceArray, start, destinationArray, 0, length);

        // set length
        thisSpan[FIELD___length].NumericByRef().s4 = length;
    }

    NANOCLR_NOCLEANUP();
}
