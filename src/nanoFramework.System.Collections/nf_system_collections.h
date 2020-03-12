//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _NF_SYSTEM_COLLECTIONS_H_
#define _NF_SYSTEM_COLLECTIONS_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>

struct Library_nf_system_collections_System_Collections_Hashtable
{
    static const int FIELD___buckets = 1;
    static const int FIELD___numberOfBuckets = 2;
    static const int FIELD___count = 3;
    static const int FIELD___loadFactor = 4;
    static const int FIELD___maxLoadFactor = 5;
    static const int FIELD___growthFactor = 6;


    //--//

};

struct Library_nf_system_collections_System_Collections_Hashtable__Entry
{
    static const int FIELD__key = 1;
    static const int FIELD__value = 2;
    static const int FIELD__next = 3;


    //--//

};

struct Library_nf_system_collections_System_Collections_Hashtable__HashtableEnumerator
{
    static const int FIELD__ht = 1;
    static const int FIELD__temp = 2;
    static const int FIELD__index = 3;
    static const int FIELD__returnType = 4;


    //--//

};

struct Library_nf_system_collections_System_Collections_Hashtable__KeyCollection
{
    static const int FIELD__ht = 1;


    //--//

};

struct Library_nf_system_collections_System_Collections_DictionaryEntry
{
    static const int FIELD__Key = 1;
    static const int FIELD__Value = 2;


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

struct Library_nf_system_collections_System_Collections_Hashtable__ValueCollection
{
    static const int FIELD__ht = 1;


    //--//

};



extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_System_Collections;

#endif  //_NF_SYSTEM_COLLECTIONS_H_
