//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef NANOCLR_CHECKS_H
#define NANOCLR_CHECKS_H

#include <nanoCLR_Runtime.h>

// clang-format off

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

struct CLR_RT_DUMP
{
#undef DECL_POSTFIX
#if defined(NANOCLR_TRACE_ERRORS)
#define DECL_POSTFIX
#else
#define DECL_POSTFIX {}
#endif

     static void TYPE      (const CLR_RT_TypeDef_Index&       cls                                             ) DECL_POSTFIX;
     static void TYPE      (const CLR_RT_ReflectionDef_Index& reflex                                          ) DECL_POSTFIX;
     static void METHOD    (const CLR_RT_MethodDef_Index&     method, const CLR_RT_TypeSpec_Index *genericType) DECL_POSTFIX;
     static void FIELD     (const CLR_RT_FieldDef_Index&      field                                           ) DECL_POSTFIX;
     static void OBJECT    (      CLR_RT_HeapBlock*           ptr   , const char* text                        ) DECL_POSTFIX;
     static void METHODREF (const CLR_RT_MethodRef_Index&     method                                          ) DECL_POSTFIX;
     static void METHODSPEC(const CLR_RT_MethodSpec_Index&    method                                          ) DECL_POSTFIX;

    //--//

#undef DECL_POSTFIX
#if defined(NANOCLR_TRACE_EXCEPTIONS)
#define DECL_POSTFIX
#else
#define DECL_POSTFIX {}
#endif
     static void EXCEPTION             ( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock& ref ) DECL_POSTFIX;
     static void POST_PROCESS_EXCEPTION( CLR_RT_HeapBlock& ref                           ) DECL_POSTFIX;

     static const char* GETERRORMESSAGE( HRESULT hrError );
#if defined(VIRTUAL_DEVICE)
	 static const char* GETERRORDETAIL ();
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct CLR_Checks
{
    static HRESULT VerifyStackOK( const CLR_RT_StackFrame& stack, const CLR_RT_HeapBlock* top, const int num );

    static HRESULT VerifyObject                ( CLR_RT_HeapBlock& top );

    static HRESULT VerifyArrayReference        ( CLR_RT_HeapBlock& ref );

    static HRESULT VerifyUnknownInstruction    ( CLR_OPCODE op         );
    static HRESULT VerifyUnsupportedInstruction( CLR_OPCODE op         );
};

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

// clang-format on

#endif // NANOCLR_CHECKS_H
