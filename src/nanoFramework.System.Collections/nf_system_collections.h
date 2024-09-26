//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef NF_SYSTEM_COLLECTIONS_H
#define NF_SYSTEM_COLLECTIONS_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Runtime__HeapBlock.h>

struct Library_nf_system_collections_System_Collections_Bucket
{
    static const int FIELD___key = 1;
    static const int FIELD___value = 2;
    static const int FIELD___hash = 3;

    //--//
};

struct Library_nf_system_collections_System_Collections_Hashtable
{
    static const int FIELD_STATIC___syncLock = 0;

    static const int FIELD___buckets = 1;
    static const int FIELD___loadsize = 2;
    static const int FIELD___loadFactor = 3;
    static const int FIELD___count = 4;
    static const int FIELD___version = 5;

    NANOCLR_NATIVE_DECLARE(Clear___VOID);
    NANOCLR_NATIVE_DECLARE(InsertNative___VOID__OBJECT__OBJECT__BOOLEAN__I4);
    NANOCLR_NATIVE_DECLARE(GetNative___OBJECT__OBJECT__I4);
    NANOCLR_NATIVE_DECLARE(ContainsNative___BOOLEAN__OBJECT__I4);
    NANOCLR_NATIVE_DECLARE(RemoveNative___VOID__OBJECT__I4);
    NANOCLR_NATIVE_DECLARE(GetPrimeNative___STATIC__I4__I4);

    //--//

    static HRESULT Expand(CLR_RT_StackFrame &stack);
    static uint32_t InitHash(const int32_t keyHashCode, int32_t hashsize, uint32_t *seed, uint32_t *incr);
};

struct Library_nf_system_collections_System_Collections_Hashtable__HashtableEnumerator
{
    static const int FIELD___hashtable = 1;
    static const int FIELD___bucket = 2;
    static const int FIELD___version = 3;
    static const int FIELD___current = 4;
    static const int FIELD___getObjectRetType = 5;
    static const int FIELD___currentKey = 6;
    static const int FIELD___currentValue = 7;

    NANOCLR_NATIVE_DECLARE(MoveNext___BOOLEAN);

    //--//
};

struct Library_nf_system_collections_System_Collections_Hashtable__KeyCollection
{
    static const int FIELD___hashtable = 1;

    //--//
};

struct Library_nf_system_collections_System_Collections_Hashtable__ValueCollection
{
    static const int FIELD___hashtable = 1;

    //--//
};

struct Library_nf_system_collections_System_Collections_Queue
{
    static const int FIELD___array = 1;
    static const int FIELD___head = 2;
    static const int FIELD___tail = 3;
    static const int FIELD___size = 4;

    NANOCLR_NATIVE_DECLARE(Clear___VOID);
    NANOCLR_NATIVE_DECLARE(CopyTo___VOID__SystemArray__I4);
    NANOCLR_NATIVE_DECLARE(Enqueue___VOID__OBJECT);
    NANOCLR_NATIVE_DECLARE(Dequeue___OBJECT);
    NANOCLR_NATIVE_DECLARE(Peek___OBJECT);

    //--//
};

struct Library_nf_system_collections_System_Collections_Stack
{
    static const int FIELD___array = 1;
    static const int FIELD___size = 2;

    NANOCLR_NATIVE_DECLARE(Clear___VOID);
    NANOCLR_NATIVE_DECLARE(Peek___OBJECT);
    NANOCLR_NATIVE_DECLARE(Pop___OBJECT);
    NANOCLR_NATIVE_DECLARE(Push___VOID__OBJECT);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_System_Collections;

#endif // NF_SYSTEM_COLLECTIONS_H
