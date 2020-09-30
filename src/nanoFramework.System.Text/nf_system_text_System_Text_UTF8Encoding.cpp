//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nf_system_text.h"


HRESULT Library_nf_system_text_System_Text_UTF8Encoding::GetBytes___SZARRAY_U1__STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    size_t                  cBytes;
    CLR_RT_HeapBlock_Array* arr;
    const char*                   str;
    CLR_RT_HeapBlock&       ret = stack.PushValueAndClear();

    str    = stack.Arg1().RecoverString(); FAULT_ON_NULL(str);
    cBytes = hal_strlen_s(str);    

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( ret, (CLR_UINT32)cBytes, g_CLR_RT_WellKnownTypes.m_UInt8 ));

    arr = ret.DereferenceArray();

    memcpy( arr->GetFirstElement(), str, cBytes );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_text_System_Text_UTF8Encoding::GetBytes___I4__STRING__I4__I4__SZARRAY_U1__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    const char*             str         = stack.Arg1().RecoverString();
    CLR_INT32               strIdx      = stack.Arg2().NumericByRef().s4;
    CLR_INT32               strCnt      = stack.Arg3().NumericByRef().s4;
    CLR_RT_HeapBlock_Array* pArrayBytes = stack.Arg4().DereferenceArray();
    CLR_INT32               byteIdx     = stack.Arg5().NumericByRef().s4;

    const CLR_UINT8*        i;
    const CLR_UINT8*        j;
    CLR_RT_UnicodeHelper    uh;
    int                     strLength;

    FAULT_ON_NULL(str);
    FAULT_ON_NULL(pArrayBytes);

    uh.SetInputUTF8(str);
    strLength = uh.CountNumberOfCharacters();
    if (strLength < 0) 
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);   

    if ((strIdx + strCnt) > (CLR_INT32)strLength) 
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);

    uh.ConvertFromUTF8(strIdx, true);
    i = uh.m_inputUTF8;
    uh.ConvertFromUTF8(strCnt, true);
    j = uh.m_inputUTF8;

    if ((byteIdx + j - i) > (CLR_INT32)pArrayBytes->m_numOfElements) 
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);

    memcpy(pArrayBytes->GetElement(byteIdx), i, j - i);

    stack.SetResult_I4(j - i);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_text_System_Text_UTF8Encoding::GetChars___SZARRAY_CHAR__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_CHECK_HRESULT(Helper__GetChars( stack, false ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_text_System_Text_UTF8Encoding::GetChars___SZARRAY_CHAR__SZARRAY_U1__I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_CHECK_HRESULT(Helper__GetChars( stack, true ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_text_System_Text_UTF8Encoding::Helper__GetChars(CLR_RT_StackFrame& stack, bool fIndexed)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    const char* szText;
    CLR_RT_HeapBlock ref; ref.SetObjectReference( NULL );
    CLR_RT_ProtectFromGC gc( ref );
    CLR_RT_HeapBlock_Array* pArrayBytes = stack.Arg1().DereferenceArray();
    CLR_INT32               byteIdx     = fIndexed ? stack.Arg2().NumericByRef().s4 : 0;
    CLR_INT32               byteCnt     = fIndexed ? stack.Arg3().NumericByRef().s4 : pArrayBytes->m_numOfElements;
    CLR_RT_HeapBlock_Array* pArrayBytesCopy;
    CLR_RT_HeapBlock_Array* arrTmp;
    int cBytesCopy;

    FAULT_ON_NULL(pArrayBytes);

    _ASSERTE(pArrayBytes->m_typeOfElement == DATATYPE_U1);

    if((byteIdx + byteCnt) > (CLR_INT32)pArrayBytes->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);

    cBytesCopy = byteCnt+1;
        
    /* Copy the array to a temporary buffer to create a zero-terminated string */
    NANOCLR_CHECK_HRESULT( CLR_RT_HeapBlock_Array::CreateInstance( ref, cBytesCopy, g_CLR_RT_WellKnownTypes.m_UInt8 ));
    
    pArrayBytesCopy = ref.DereferenceArray();
    szText = (const char*)pArrayBytesCopy->GetFirstElement(); 

    hal_strncpy_s( (char*) szText, cBytesCopy, (const char*)pArrayBytes->GetElement(byteIdx), byteCnt );    

    NANOCLR_CHECK_HRESULT(Library_corlib_native_System_String::ConvertToCharArray( szText, stack.PushValueAndClear(), arrTmp, 0, -1 ));

    NANOCLR_NOCLEANUP();
}
