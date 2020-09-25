//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CLR_Checks::VerifyObject( CLR_RT_HeapBlock& top )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    switch(top.DataType())
    {
    case DATATYPE_OBJECT:
    case DATATYPE_BYREF:
        if(top.Dereference() != NULL) NANOCLR_SET_AND_LEAVE(S_OK);
        break;

    case DATATYPE_ARRAY_BYREF:
        if(top.DereferenceArray() != NULL) NANOCLR_SET_AND_LEAVE(S_OK);
        break;

    default:
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    NANOCLR_SET_AND_LEAVE(CLR_E_NULL_REFERENCE);

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_Checks::VerifyArrayReference( CLR_RT_HeapBlock& ref )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array* array;

    if(ref.DataType() != DATATYPE_OBJECT)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    array = ref.DereferenceArray();
    if(array == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NULL_REFERENCE);
    }

    if(array->DataType() != DATATYPE_SZARRAY)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_Checks::VerifyUnknownInstruction( CLR_OPCODE op )
{
    (void)op;

    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(CLR_E_UNKNOWN_INSTRUCTION);

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_Checks::VerifyUnsupportedInstruction( CLR_OPCODE op )
{
    (void)op;

    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(CLR_E_UNSUPPORTED_INSTRUCTION);

    NANOCLR_NOCLEANUP();
}
