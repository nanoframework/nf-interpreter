//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_system_collections.h"

typedef Library_nf_system_collections_System_Collections_Hashtable HashtableType;
typedef Library_nf_system_collections_System_Collections_Bucket BucketType;

HRESULT Library_nf_system_collections_System_Collections_Hashtable__HashtableEnumerator::MoveNext___BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int32_t bucket;

    CLR_RT_HeapBlock *hastable;
    CLR_RT_HeapBlock *pThis;
    CLR_RT_HeapBlock *keyv;
    CLR_RT_HeapBlock *currentBucket;
    CLR_RT_HeapBlock *bucketElement;
    CLR_RT_HeapBlock_Array *buckets;

    pThis = (CLR_RT_HeapBlock *)stack.This();
    FAULT_ON_NULL(pThis);

    hastable = pThis[FIELD___hashtable].Dereference();

    if (pThis[FIELD___version].NumericByRef().s4 != hastable[HashtableType::FIELD___version].NumericByRef().s4)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION)
    }

    buckets = hastable[HashtableType::FIELD___buckets].DereferenceArray();

    bucket = pThis[FIELD___bucket].NumericByRef().s4;

    while (bucket > 0)
    {
        bucket--;

        bucketElement = ((CLR_RT_HeapBlock *)buckets->GetElement(bucket));
        currentBucket = bucketElement->Dereference();

        if (currentBucket != NULL)
        {
            keyv = currentBucket[BucketType::FIELD___key].Dereference();

            if (keyv != NULL)
            {
                pThis[FIELD___currentKey].SetObjectReference(keyv);
                pThis[FIELD___currentValue].SetObjectReference(currentBucket[BucketType::FIELD___value].Dereference());
                pThis[FIELD___current].NumericByRef().u1 = true;

                pThis[FIELD___bucket].NumericByRef().s4 = bucket;

                stack.SetResult_Boolean(true);

                NANOCLR_SET_AND_LEAVE(S_OK);
            }
        }
    }

    pThis[FIELD___bucket].NumericByRef().s4 = bucket;
    pThis[FIELD___current].NumericByRef().u1 = false;

    stack.SetResult_Boolean(false);

    NANOCLR_NOCLEANUP();
}
