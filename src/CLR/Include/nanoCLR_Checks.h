//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOCLR_CHECKS_H_
#define _NANOCLR_CHECKS_H_

#include <nanoCLR_Runtime.h>

struct CLR_RT_DUMP
{
#undef DECL_POSTFIX
#if defined(NANOCLR_TRACE_ERRORS)
#define DECL_POSTFIX
#else
#define DECL_POSTFIX {}
#endif

    __nfweak static void TYPE  ( const CLR_RT_TypeDef_Index&       cls                 ) DECL_POSTFIX;
    __nfweak static void TYPE  ( const CLR_RT_ReflectionDef_Index& reflex              ) DECL_POSTFIX;
    __nfweak static void METHOD( const CLR_RT_MethodDef_Index&     method              ) DECL_POSTFIX;
    __nfweak static void FIELD ( const CLR_RT_FieldDef_Index&      field               ) DECL_POSTFIX;
    __nfweak static void OBJECT(       CLR_RT_HeapBlock*           ptr   , const char* text ) DECL_POSTFIX;

    //--//

#undef DECL_POSTFIX
#if defined(NANOCLR_TRACE_EXCEPTIONS)
#define DECL_POSTFIX
#else
#define DECL_POSTFIX {}
#endif
    __nfweak static void EXCEPTION             ( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock& ref ) DECL_POSTFIX;
    __nfweak static void POST_PROCESS_EXCEPTION( CLR_RT_HeapBlock& ref                           ) DECL_POSTFIX;

    __nfweak static const char* GETERRORMESSAGE( HRESULT hrError );
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct CLR_Checks
{
    static HRESULT VerifyStackOK( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock* top, int num ) { return S_OK; }

    static HRESULT VerifyObject                ( CLR_RT_HeapBlock& top );

    static HRESULT VerifyArrayReference        ( CLR_RT_HeapBlock& ref );

    static HRESULT VerifyUnknownInstruction    ( CLR_OPCODE op         );
    static HRESULT VerifyUnsupportedInstruction( CLR_OPCODE op         );
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _NANOCLR_CHECKS_H_

