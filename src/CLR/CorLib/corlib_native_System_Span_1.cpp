//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "CorLib.h"

HRESULT Library_corlib_native_System_Span_1::CopyTo___VOID__SystemSpan_1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array *sourceArray;
    CLR_RT_HeapBlock_Array *destinationArray;
    CLR_RT_HeapBlock *sourceSpan = stack.This();
    CLR_RT_HeapBlock *destinationSpan = stack.Arg1().Dereference();

    // check lengths
    if (sourceSpan[FIELD___length].NumericByRefConst().u4 > destinationSpan[FIELD___length].NumericByRefConst().u4)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // get pointers to the arrays
    sourceArray = sourceSpan[FIELD___array].DereferenceArray();
    destinationArray = destinationSpan[FIELD___array].DereferenceArray();

    {
        // prevent GC from moving the arrays while we copy the data
        CLR_RT_ProtectFromGC gc1(*sourceArray);
        CLR_RT_ProtectFromGC gc2(*destinationArray);

        // copy the data
        memcpy(
            &destinationArray[sourceSpan[FIELD___start].NumericByRefConst().u4],
            &sourceArray[sourceSpan[FIELD___start].NumericByRefConst().u4],
            sourceSpan[FIELD___length].NumericByRefConst().u4 *
                c_CLR_RT_DataTypeLookup[sourceArray->DataType()].m_sizeInBytes);
    }

    NANOCLR_NOCLEANUP();
}
