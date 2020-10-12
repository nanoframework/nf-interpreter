//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


static const CLR_UINT16 c_WhiteSpaces[] =
{
    0x0009,
    0x000A,
    0x000B,
    0x000C,
    0x000D,
    0x0020,
    0x00A0,
    0x2000,
    0x2001,
    0x2002,
    0x2003,
    0x2004,
    0x2005,
    0x2006,
    0x2007,
    0x2008,
    0x2009,
    0x200A,
    0x200B,
    0x3000,
    0xFEFF,
};

//--//

HRESULT Library_corlib_native_System_String::CompareTo___I4__OBJECT( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(Library_corlib_native_System_String::CompareTo___I4__STRING( stack ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::get_Chars___CHAR__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    const char*               szText;
    CLR_RT_UnicodeHelper uh;
    CLR_UINT16           buf[ 3 ];
    int                  len;
    int                  index;

    szText = stack.Arg0().RecoverString(); FAULT_ON_NULL(szText);

    uh.SetInputUTF8( szText );

    len   = uh.CountNumberOfCharacters()  ; if(len   < 0                ) NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    index = stack.Arg1().NumericByRef().s4; if(index < 0 || index >= len) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);

    uh.m_outputUTF16      = buf;
    uh.m_outputUTF16_size = MAXSTRLEN(buf);

    //
    // First move to the character, then read it.
    //
    uh.ConvertFromUTF8( index, true  );
    uh.ConvertFromUTF8( 1    , false );

    stack.SetResult( buf[ 0 ], DATATYPE_CHAR );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::ToCharArray___SZARRAY_CHAR( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(ToCharArray( stack, 0, -1 ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::ToCharArray___SZARRAY_CHAR__I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(ToCharArray( stack, stack.Arg1().NumericByRef().s4, stack.Arg2().NumericByRef().s4 ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::get_Length___I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    const char* szText = stack.Arg0().RecoverString(); FAULT_ON_NULL(szText);

    CLR_RT_UnicodeHelper uh; uh.SetInputUTF8( szText );

    stack.SetResult_I4( uh.CountNumberOfCharacters() );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::Split___SZARRAY_STRING__SZARRAY_CHAR( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(Split( stack, stack.Arg1(), 0x7FFFFFFF )); /// Sending INT_MAX instead of -1. 

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::Split___SZARRAY_STRING__SZARRAY_CHAR__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(Split( stack, stack.Arg1(), stack.Arg2().NumericByRef().s4 ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::Substring___STRING__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(Substring( stack, stack.Arg1().NumericByRef().s4, -1 ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::Substring___STRING__I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_INT32 length = stack.Arg2().NumericByRef().s4;

    if(length < 0) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);

    NANOCLR_SET_AND_LEAVE(Substring( stack, stack.Arg1().NumericByRef().s4, length ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::Trim___STRING__SZARRAY_CHAR( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(Trim( stack, stack.Arg1().DereferenceArray(), true, true ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::TrimStart___STRING__SZARRAY_CHAR( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(Trim( stack, stack.Arg1().DereferenceArray(), true, false ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::TrimEnd___STRING__SZARRAY_CHAR( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(Trim( stack, stack.Arg1().DereferenceArray(), false, true ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::_ctor___VOID__SZARRAY_CHAR__I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(FromCharArray( stack, stack.Arg2().NumericByRef().s4, stack.Arg3().NumericByRef().s4 ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::_ctor___VOID__SZARRAY_CHAR( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(FromCharArray( stack, 0, -1 ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::_ctor___VOID__CHAR__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_UINT16 ch  = stack.Arg1().NumericByRef().u2;
    int        len = stack.Arg2().NumericByRef().s4; if(len < 0) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);

    {
        CLR_RT_HeapBlock     tmp; tmp.SetObjectReference( NULL );
        CLR_RT_ProtectFromGC gc( tmp );

        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( tmp, len, g_CLR_RT_WellKnownTypes.m_Char ));

        {
            CLR_RT_HeapBlock_Array* tmpArray = tmp.DereferenceArray();
            CLR_UINT16*             pTmp     = (CLR_UINT16*)tmpArray->GetFirstElement();
            CLR_UINT16*             p;

            p = pTmp; while(len--) *p++ = ch;

            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( stack.Arg0(), pTmp, tmpArray->m_numOfElements ));
        }
    }

    NANOCLR_NOCLEANUP();
}


HRESULT Library_corlib_native_System_String::CompareTo___I4__STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock& pThis = stack.Arg0(); // String references are special, they don't point to an object, they are the object. So use stack.Arg0() instead of stack.This()
    CLR_RT_HeapBlock& pArg  = stack.Arg1();

    stack.SetResult_I4( CLR_RT_HeapBlock::Compare_Unsigned_Values( pThis, pArg ) );

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_corlib_native_System_String::IndexOf___I4__CHAR( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(IndexOf( stack, c_IndexOf__SingleChar ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::IndexOf___I4__CHAR__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(IndexOf( stack, c_IndexOf__SingleChar | c_IndexOf__StartIndex ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::IndexOf___I4__CHAR__I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(IndexOf( stack, c_IndexOf__SingleChar | c_IndexOf__StartIndex | c_IndexOf__Count ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::IndexOfAny___I4__SZARRAY_CHAR( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(IndexOf( stack, c_IndexOf__MultipleChars ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::IndexOfAny___I4__SZARRAY_CHAR__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(IndexOf( stack, c_IndexOf__MultipleChars | c_IndexOf__StartIndex ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::IndexOfAny___I4__SZARRAY_CHAR__I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(IndexOf( stack, c_IndexOf__MultipleChars | c_IndexOf__StartIndex | c_IndexOf__Count ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::IndexOf___I4__STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(IndexOf( stack, c_IndexOf__String ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::IndexOf___I4__STRING__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(IndexOf( stack, c_IndexOf__String | c_IndexOf__StartIndex ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::IndexOf___I4__STRING__I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(IndexOf( stack, c_IndexOf__String | c_IndexOf__StartIndex | c_IndexOf__Count ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::LastIndexOf___I4__CHAR( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(IndexOf( stack, c_IndexOf__SingleChar | c_IndexOf__Last ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::LastIndexOf___I4__CHAR__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(IndexOf( stack, c_IndexOf__SingleChar | c_IndexOf__Last | c_IndexOf__StartIndex ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::LastIndexOf___I4__CHAR__I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(IndexOf( stack, c_IndexOf__SingleChar | c_IndexOf__Last | c_IndexOf__StartIndex | c_IndexOf__Count ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::LastIndexOfAny___I4__SZARRAY_CHAR( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(IndexOf( stack, c_IndexOf__MultipleChars | c_IndexOf__Last ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::LastIndexOfAny___I4__SZARRAY_CHAR__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(IndexOf( stack, c_IndexOf__MultipleChars | c_IndexOf__Last | c_IndexOf__StartIndex ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::LastIndexOfAny___I4__SZARRAY_CHAR__I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(IndexOf( stack, c_IndexOf__MultipleChars | c_IndexOf__Last | c_IndexOf__StartIndex | c_IndexOf__Count ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::LastIndexOf___I4__STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(IndexOf( stack, c_IndexOf__String | c_IndexOf__Last ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::LastIndexOf___I4__STRING__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(IndexOf( stack, c_IndexOf__String | c_IndexOf__Last | c_IndexOf__StartIndex ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::LastIndexOf___I4__STRING__I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(IndexOf( stack, c_IndexOf__String | c_IndexOf__Last | c_IndexOf__StartIndex | c_IndexOf__Count ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::ToLower___STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(ChangeCase( stack, false ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::ToUpper___STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(ChangeCase( stack, true ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::Trim___STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(Trim( stack, NULL, true, true ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::Equals___STATIC__BOOLEAN__STRING__STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    stack.SetResult_Boolean( CLR_RT_HeapBlock::Compare_Unsigned_Values( stack.Arg0(), stack.Arg1() ) == 0 );

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_corlib_native_System_String::op_Equality___STATIC__BOOLEAN__STRING__STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(Library_corlib_native_System_String::Equals___STATIC__BOOLEAN__STRING__STRING( stack ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::op_Inequality___STATIC__BOOLEAN__STRING__STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_CHECK_HRESULT(Library_corlib_native_System_String::Equals___STATIC__BOOLEAN__STRING__STRING( stack ));

    stack.NegateResult();

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::Compare___STATIC__I4__STRING__STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    stack.SetResult_I4( CLR_RT_HeapBlock::Compare_Unsigned_Values( stack.Arg0(), stack.Arg1() ) );

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_corlib_native_System_String::Concat___STATIC__STRING__STRING__STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(Concat( stack, &stack.Arg0(), 2 ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::Concat___STATIC__STRING__STRING__STRING__STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(Concat( stack, &stack.Arg0(), 3 ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::Concat___STATIC__STRING__STRING__STRING__STRING__STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(Concat( stack, &stack.Arg0(), 4 ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::Concat___STATIC__STRING__SZARRAY_STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array* pArray = stack.Arg0().DereferenceArray(); FAULT_ON_NULL(pArray);

    NANOCLR_SET_AND_LEAVE(Concat( stack, (CLR_RT_HeapBlock*)pArray->GetFirstElement(), pArray->m_numOfElements ));

    NANOCLR_NOCLEANUP();
}

//--//

HRESULT Library_corlib_native_System_String::FromCharArray( CLR_RT_StackFrame& stack, int startIndex, int length )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array* array;
    CLR_UINT32              len;

    array = stack.Arg1().DereferenceArray();
    if (!array || array->m_numOfElements == 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_RT_HeapBlock_String::CreateInstance(stack.Arg0(), ""));
    }
    else
    {
        len = array->m_numOfElements;

        if (length == -1) 
            length = len - startIndex;

        if (CLR_RT_HeapBlock_Array::CheckRange(startIndex, length, len) == false) 
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);

        NANOCLR_SET_AND_LEAVE(CLR_RT_HeapBlock_String::CreateInstance(stack.Arg0(), (CLR_UINT16*)array->GetElement(startIndex), length));
    }

    NANOCLR_NOCLEANUP();
}


HRESULT Library_corlib_native_System_String::ToCharArray( CLR_RT_StackFrame& stack, int startIndex, int length )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array* array;

    NANOCLR_SET_AND_LEAVE(ConvertToCharArray( stack, stack.PushValueAndClear(), array, startIndex, length ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::IndexOf( CLR_RT_StackFrame& stack, int mode )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    const char*               szText;
    int                  startIndex;
    int                  count;
    int                  pos;
    const char*               pString;
    const CLR_UINT16*    pChars;
    int                  iChars = 0;
    CLR_RT_UnicodeHelper inputIterator;
    int                  inputLen;
    int                  searchLen = 1;
    
    szText  = stack.Arg0().RecoverString(); if(!szText) szText = "";
    pos     = -1;
    pString = NULL;
    pChars  = NULL;
    
    if(mode & c_IndexOf__SingleChar)
    {
        pChars = (CLR_UINT16*)&stack.Arg1().NumericByRefConst().u2;
        iChars = 1;
    }
    else if(mode & c_IndexOf__MultipleChars)
    {
        CLR_RT_HeapBlock_Array* array = stack.Arg1().DereferenceArray(); FAULT_ON_NULL(array);

        pChars = (const CLR_UINT16*)array->GetFirstElement();
        iChars =                    array->m_numOfElements;
    }
    else if(mode & c_IndexOf__String)
    {
        pString = stack.Arg1().RecoverString(); FAULT_ON_NULL(pString);
        // how long is the search string?
        inputIterator.SetInputUTF8( pString );
        searchLen = inputIterator.CountNumberOfCharacters();
    }

    // calculate input string length
    inputIterator.SetInputUTF8( szText );
    inputLen = inputIterator.CountNumberOfCharacters();

    // calculate start index
    if(mode & c_IndexOf__StartIndex)
    {
        startIndex = stack.Arg2().NumericByRefConst().s4;
    }
    else
    {
        // for mode LastIndex... we are searching backwards toward the start of the string
        if(mode & c_IndexOf__Last)
        {
            startIndex = inputLen - 1;
        }
        else
        {
            startIndex = 0;
        }
    }

    // check the start index
    if(startIndex < 0 || startIndex > inputLen) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);

    // for mode LastIndex... with string we move the start index back by the search string length -1
    if((mode & c_IndexOf__String_Last) == c_IndexOf__String_Last)
    {
        startIndex -= searchLen - 1;
        // check the start index; if not in range skip the search
        if(startIndex < 0 || startIndex > inputLen) goto Exit;
    }
 
    // calculate the iteration count
    if(mode & c_IndexOf__Count)
    {
        // count form parameter
        count = stack.Arg3().NumericByRefConst().s4;
    }
    else
    {
        // for mode LastIndex... we are searching from start index backwards toward the start of the string
        if(mode & c_IndexOf__Last)
        {
            // backward until the start of string
            // one more time than the startIndex because we should iterate until zero
            count = startIndex + 1;
        }
        else
        {
            // forward until the end of string
            count = inputLen - startIndex;
        }
    }

    // for mode with string we reduce the count by the search string length -1
    // if we search foreward
    if((mode & c_IndexOf__String_Last) == c_IndexOf__String)
    {
        count -= searchLen - 1;
    }
    
    // check the count
    if(mode & c_IndexOf__Last)
    {
        // check for backward mode; no exception; just exit
        if(count > startIndex + 1) goto Exit;
    }
    else
    {
        // check for forward mode
        if(startIndex + count > inputLen) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }
        
    // First move to the character, then read it.
    if(inputIterator.ConvertFromUTF8( startIndex, true ))
    {
        // string mode?
        if(pString)
        {
            // iterate thru all positions
            while(count-- > 0)
            {
                CLR_RT_UnicodeHelper inputString; inputString.SetInputUTF8( (const char*)inputIterator.m_inputUTF8 );
                CLR_RT_UnicodeHelper searchString; searchString.SetInputUTF8(         pString        );
                bool finished = false;
                
                while(true)
                {
                    CLR_UINT16 bufInput[ 3 ];
                    CLR_UINT16 bufSearch[ 3 ];

                    inputString.m_outputUTF16      = bufInput;
                    inputString.m_outputUTF16_size = MAXSTRLEN(bufInput);

                    searchString.m_outputUTF16      = bufSearch;
                    searchString.m_outputUTF16_size = MAXSTRLEN(bufSearch);

                    // read next char from search string; if no more chars to read (false)
                    // then we are done and found the search string in the input string
                    if(searchString.ConvertFromUTF8( 1, false ) == false)
                    {
                        pos = startIndex;
                        finished = true;
                        break;
                    }

                    // read the next char from the input string; if no more chars to read (false)
                    // we didn't found the search string in the input string; we abort the search now
                    if(inputString.ConvertFromUTF8( 1, false ) == false)
                    {
                        finished = true;
                        break;
                    }

                    // does the char from input not match the char from the search string
                    if(bufInput[ 0 ] != bufSearch[ 0 ])
                    {
                        // next iteration round but not finished
                        break;
                    }
                }

                // finished (with or without a found) then break
                if (finished)
                {
                    break;
                }
                
                // reading forward or backward
                if(mode & c_IndexOf__Last)
                {
                    startIndex--;
                    // move one chars backward
                    if(inputIterator.MoveBackwardInUTF8( szText, 1 ) == false)
                    {
                        break;
                    }
                }
                else
                {
                    startIndex++;
                    // move to the next char
                    if(inputIterator.ConvertFromUTF8( 1, true ) == false)
                    {
                        break;
                    }
                }
            }
        }

        // char mode?
        if(pChars)
        {
            // iterate thru all positions
            while(count-- > 0)
            {
                CLR_UINT16 buf[ 3 ];

                inputIterator.m_outputUTF16      = buf;
                inputIterator.m_outputUTF16_size = MAXSTRLEN(buf);

                // read the next char from the input string; if no more chars to read (false)
                // we didn't found the search chars in the input string
                if(inputIterator.ConvertFromUTF8( 1, false ) == false)
                {
                    break;
                }

                // test each search char if it's a match
                for(int i=0; i<iChars; i++)
                {
                    // match?
                    if(buf[ 0 ] == pChars[ i ])
                    {
                        // position found!
                        pos = startIndex;
                        break;
                    }
                }

                // found? => break
                if (pos != -1)
                {
                    break;
                }
                
                // for mode LastIndex... we are searching from start index backwards toward the start of the string
                if(mode & c_IndexOf__Last)
                {
                    // in backward mode
                    startIndex--;
                    // move two chars backward, because the current char is already read
                    if(inputIterator.MoveBackwardInUTF8( szText, 2 ) == false)
                    {
                        break;
                    }
                }
                else
                {
                    // forward mode; simple advance the start index
                    startIndex++;
                }
            }
        }
    }

Exit:
    stack.SetResult_I4( pos );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::ChangeCase( CLR_RT_StackFrame& stack, bool fToUpper )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock        refTmp; refTmp.SetObjectReference( NULL );
    CLR_RT_ProtectFromGC    gc( refTmp );
    CLR_RT_HeapBlock_Array* arrayTmp;
    CLR_UINT16*             ptr;

    NANOCLR_CHECK_HRESULT(ConvertToCharArray( stack, refTmp, arrayTmp, 0, -1 ));

    ptr = (CLR_UINT16*)arrayTmp->GetFirstElement();

    for(CLR_UINT32 i=0; i<arrayTmp->m_numOfElements; i++)
    {
        CLR_UINT16 c = *ptr;

        if(fToUpper)
        {
            if(c >= 'a' && c <= 'z') c += 'A' - 'a';
        }
        else
        {
            if(c >= 'A' && c <= 'Z') c -= 'A' - 'a';
        }

        *ptr++ = c;
    }

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( stack.PushValue(), (CLR_UINT16*)arrayTmp->GetFirstElement(), arrayTmp->m_numOfElements ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::Substring( CLR_RT_StackFrame& stack, int startIndex, int length )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock        refTmp; refTmp.SetObjectReference( NULL );
    CLR_RT_ProtectFromGC    gc( refTmp );
    CLR_RT_HeapBlock_Array* arrayTmp;

    NANOCLR_CHECK_HRESULT(ConvertToCharArray( stack, refTmp, arrayTmp, 0, -1 ));

    if(startIndex < 0 || startIndex > (int)arrayTmp->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);

    if(length == -1)
    {
        length = arrayTmp->m_numOfElements - startIndex;
    }
    else
    {
        if(length < 0 || (startIndex + length) > (int)arrayTmp->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }
    
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( stack.PushValue(), (CLR_UINT16*)arrayTmp->GetElement( startIndex ), length ));

    NANOCLR_NOCLEANUP();
}


HRESULT Library_corlib_native_System_String::Trim( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock_Array* arrayTrimChars, bool fStart, bool fEnd )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock        refTmp; refTmp.SetObjectReference( NULL );
    CLR_RT_ProtectFromGC    gc( refTmp );
    CLR_RT_HeapBlock_Array* arrayTmp;
    CLR_UINT16*             pSrcStart;
    CLR_UINT16*             pSrcEnd;

    NANOCLR_CHECK_HRESULT(ConvertToCharArray( stack, refTmp, arrayTmp, 0, -1 ));

    pSrcStart = (CLR_UINT16*)arrayTmp->GetFirstElement();
    pSrcEnd   = &pSrcStart[ arrayTmp->m_numOfElements ];

    const CLR_UINT16* pTrim;
    CLR_UINT32        iTrim;

    if(arrayTrimChars && arrayTrimChars->m_numOfElements)
    {
        pTrim = (const CLR_UINT16*)arrayTrimChars->GetFirstElement();
        iTrim =                    arrayTrimChars->m_numOfElements;
    }
    else
    {
       pTrim = c_WhiteSpaces;
       iTrim = ARRAYSIZE(c_WhiteSpaces);
    }

    //--//

    if(fStart)
    {
        while(pSrcStart < pSrcEnd)
        {
            const CLR_UINT16* p = pTrim;
            CLR_UINT32        i;

            for(i=0; i<iTrim; i++, p++)
            {
                if(*p == pSrcStart[ 0 ]) break;
            }

            if(i == iTrim) break;

            pSrcStart++;
        }
    }

    if(fEnd)
    {
        while(pSrcStart < pSrcEnd)
        {
            const CLR_UINT16* p = pTrim;
            CLR_UINT32        i;

            for(i=0; i<iTrim; i++, p++)
            {
                if(*p == pSrcEnd[ -1 ]) break;
            }

            if(i == iTrim) break;

            pSrcEnd--;
        }
    }


    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( stack.PushValue(), pSrcStart, (CLR_UINT32)(pSrcEnd - pSrcStart) ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::Split( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock& chars, int maxStrings )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array* arraySrc;
    CLR_RT_HeapBlock_Array* arrayChars;
    CLR_RT_HeapBlock_Array* arrayDst;
    const CLR_UINT16* pChars;
    CLR_UINT32 cChars;
    
    if (maxStrings < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE)
    }
    else if (maxStrings == 0)
    {
        CLR_RT_HeapBlock& refTarget = stack.PushValue();

        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( refTarget, 0, g_CLR_RT_WellKnownTypes.m_String ));

        arrayDst = refTarget.DereferenceArray();
    }
    else
    {
        arrayChars = chars.DereferenceArray(); 
        
        if(arrayChars != NULL && arrayChars->m_numOfElements > 0)
        {
            pChars = (CLR_UINT16*)arrayChars->GetFirstElement();
            cChars = arrayChars->m_numOfElements;
        }
        else
        {
            pChars = &c_WhiteSpaces[ 0 ];
            cChars = ARRAYSIZE(c_WhiteSpaces);
        }
        
        arrayDst   = NULL;

        {
            CLR_RT_HeapBlock     refSrc; refSrc.SetObjectReference( NULL );
            CLR_RT_ProtectFromGC gc( refSrc );

            NANOCLR_CHECK_HRESULT(ConvertToCharArray( stack, refSrc, arraySrc, 0, -1 ));

            for(int pass=0; pass<2; pass++)
            {
                CLR_UINT16* pSrcStart   = (CLR_UINT16*)arraySrc->GetFirstElement();
                CLR_UINT16* pSrc        = pSrcStart;
                int         count       = 0;
                bool        fFound      = false;
                bool fContinueSearching = true;

                for(CLR_UINT32 iSrc=0; iSrc <= arraySrc->m_numOfElements; iSrc++, pSrc++)
                {
                    if(iSrc == arraySrc->m_numOfElements)
                    {
                        fFound = true;
                    }
                    else if ((count + 1) >= maxStrings)
                    {
                        fContinueSearching = false;
                    }
                    else if (fContinueSearching)
                    {
                        const CLR_UINT16* pCharsT = pChars;

                        for(CLR_UINT32 iChars=0; iChars<cChars; iChars++)
                        {
                            if(pSrc[ 0 ] == *pCharsT++)
                            {
                                fFound = true;
                                break;
                            }
                        }
                    }

                    if(fFound)
                    {
                        if(pass == 1)
                        {
                            CLR_RT_HeapBlock* str = (CLR_RT_HeapBlock*)arrayDst->GetElement( count );

                            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( *str, pSrcStart, (CLR_UINT32)(pSrc - pSrcStart) ));

                            pSrcStart = pSrc + 1;
                        }

                        count++;
                        fFound = false;
                    }
                }

                if(pass == 0)
                {
                    CLR_RT_HeapBlock& refTarget = stack.PushValue();

                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( refTarget, count, g_CLR_RT_WellKnownTypes.m_String ));

                    arrayDst = refTarget.DereferenceArray();
                }
            }
        }
    }

    NANOCLR_NOCLEANUP();
}

//--//

HRESULT Library_corlib_native_System_String::Concat( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock* array, int num )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();
 
    CLR_RT_HeapBlock*       ptrSrc;
    const char*                  szTextSrc;
    char*                   szTextDst = NULL;
    CLR_UINT32              totLen;
    CLR_UINT32              len;
 
    totLen = 0;
 
    for(int i=0; i<2; i++)
    {
        ptrSrc = array;
 
        for(int iStr=0; iStr<num; iStr++)
        {
            _ASSERTE(ptrSrc->DataType() == DATATYPE_OBJECT);
            _ASSERTE(FIMPLIES(ptrSrc->Dereference(), ptrSrc->Dereference()->DataType() == DATATYPE_STRING));
 
            szTextSrc = ptrSrc->RecoverString();
            if(szTextSrc)
            {
                len = (CLR_UINT32)hal_strlen_s( szTextSrc );
 
                if(i==0)
                {
                    totLen += len;
                }
                else
                {                    
                    memcpy( szTextDst, szTextSrc, len );
 
                    szTextDst += len;
                }                
            }
            
            ptrSrc++;
        }
 
        if(i==0)
        {
            //push return value
            CLR_RT_HeapBlock&        blkResult = stack.PushValue();
            CLR_RT_HeapBlock_String* str       = CLR_RT_HeapBlock_String::CreateInstance( blkResult, totLen ); CHECK_ALLOCATION(str);
            
            szTextDst           = (char*)str->StringText();
            szTextDst[ totLen ] = 0;
        }
    }
 
    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::ConvertToCharArray( const char* szText, CLR_RT_HeapBlock& ref, CLR_RT_HeapBlock_Array*& array, int startIndex, int length )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_UnicodeHelper uh;
    int                  totLength;

    FAULT_ON_NULL(szText);

    uh.SetInputUTF8( szText );

    totLength = uh.CountNumberOfCharacters(); if(totLength < 0) NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);

    if(length == -1) length = totLength;

    if(CLR_RT_HeapBlock_Array::CheckRange( startIndex, length, totLength ) == false) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( ref, length, g_CLR_RT_WellKnownTypes.m_Char ));

    array = ref.DereferenceArray();

    uh.m_outputUTF16      = (CLR_UINT16*)array->GetFirstElement();
    uh.m_outputUTF16_size =              array->m_numOfElements;

    //
    // First move to the character, then read it.
    //
    uh.ConvertFromUTF8( startIndex            , true  );
    uh.ConvertFromUTF8( array->m_numOfElements, false );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_String::ConvertToCharArray( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock& ref, CLR_RT_HeapBlock_Array*& array, int startIndex, int length )
{
    NATIVE_PROFILE_CLR_CORE();
    return ConvertToCharArray( stack.Arg0().RecoverString(), ref, array, startIndex, length );
}
