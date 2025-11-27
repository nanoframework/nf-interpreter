//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "CorLib.h"

typedef Library_corlib_native_System_Runtime_CompilerServices_RuntimeHelpers RuntimeHelpers;

HRESULT Library_corlib_native_System_ReadOnlySpan_1::_ctor___VOID__VOIDptr__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int32_t length;
    bool isRefContainsRefs = false;
    uintptr_t objectRawPointer;

    CLR_RT_HeapBlock_Array *destinationArray;
    CLR_RT_HeapBlock *thisSpan = stack.This();

    // grab caller to get the generic type
    CLR_RT_MethodDef_Instance &caller = stack.MethodCall();

    if (caller.genericType == nullptr || !NANOCLR_INDEX_IS_VALID(*caller.genericType))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    CLR_RT_TypeSpec_Instance typeSpec;
    if (!typeSpec.InitializeFromIndex(*caller.genericType))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    CLR_RT_SignatureParser parser;
    parser.Initialize_TypeSpec(typeSpec);

    CLR_RT_SignatureParser::Element element;

    if (FAILED(parser.Advance(element)))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    // The first element should be the generic type instantiation
    if (element.DataType != DATATYPE_GENERICINST)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    // Advance to get the generic argument (T)
    if (FAILED(parser.Advance(element)))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    if (element.DataType == DATATYPE_VALUETYPE)
    {
        // For value types we need to advance again
        if (FAILED(parser.Advance(element)))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }
    }

    // check if T is a reference type or contains references
    NANOCLR_CHECK_HRESULT(
        RuntimeHelpers::CheckReferenceOrContainsReferences(
            element.Class,
            element.DataType,
            &parser,
            isRefContainsRefs));

    if (isRefContainsRefs)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // validate length parameter
    length = stack.Arg2().NumericByRefConst().s4;

    if (length < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    // get the pointer to the array
    // validate data type as being an unmanaged pointer
    if (stack.Arg1().DataType() != DATATYPE_PTR)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    objectRawPointer = (uintptr_t)stack.Arg1().Dereference();

    {
        CLR_RT_HeapBlock &refArray = thisSpan[FIELD___array];
        CLR_RT_HeapBlock_Array::CreateInstance(refArray, length, element.Class);

        destinationArray = thisSpan[FIELD___array].DereferenceArray();
        CLR_UINT32 elementSize = destinationArray->m_sizeOfElement;
        CLR_UINT8 *elementPtr = destinationArray->GetFirstElement();

        // copy data from the raw pointer to the newly created array
        memcpy(elementPtr, (void *)objectRawPointer, elementSize * length);
    }

    // set length
    thisSpan[FIELD___length].NumericByRef().s4 = length;

    NANOCLR_NOCLEANUP();
}

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
