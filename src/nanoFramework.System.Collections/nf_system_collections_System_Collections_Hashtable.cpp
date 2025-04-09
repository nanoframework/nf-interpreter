//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_system_collections.h"

const int32_t HashPrime = 101;
const int32_t MaxPrimeArrayLength = 12143;

static int32_t s_primes[] = {3,    7,    11,   17,   23,   29,   37,   47,   59,   71,    89,   107,  131,
                             163,  197,  239,  293,  353,  431,  521,  631,  761,  919,   1103, 1327, 1597,
                             1931, 2333, 2801, 3371, 4049, 4861, 5839, 7013, 8419, 10103, 12143};

const size_t lenghtOfPrimes = ARRAYSIZE(s_primes);

typedef Library_nf_system_collections_System_Collections_Bucket BucketType;

HRESULT Library_nf_system_collections_System_Collections_Hashtable::Clear___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int32_t bucketNumber;
    int32_t bucketsLength;
    CLR_RT_HeapBlock *bucket;
    CLR_RT_HeapBlock *bucketElement;
    CLR_RT_HeapBlock_Array *buckets;

    CLR_RT_HeapBlock *pThis;

    pThis = (CLR_RT_HeapBlock *)stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___count].NumericByRef().s4 == 0)
    {
        NANOCLR_SET_AND_LEAVE(S_OK)
    }

    buckets = pThis[FIELD___buckets].DereferenceArray();
    bucketsLength = buckets->m_numOfElements;

    for (bucketNumber = 0; bucketNumber < bucketsLength; bucketNumber++)
    {
        bucketElement = ((CLR_RT_HeapBlock *)buckets->GetElement(bucketNumber));
        bucket = bucketElement->Dereference();

        // sanity check: can only clear buckets that already have objects
        if (bucket != NULL)
        {
            bucket[BucketType::FIELD___hash].NumericByRef().u4 = 0;
            bucket[BucketType::FIELD___value].SetObjectReference(NULL);
            bucket[BucketType::FIELD___key].SetObjectReference(NULL);
        }
    }

    // reset count field
    pThis[FIELD___count].NumericByRef().s4 = 0;

    // update version
    pThis[FIELD___version].NumericByRef().s4++;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_collections_System_Collections_Hashtable::ContainsNative___BOOLEAN__OBJECT__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int32_t entry = 0;
    int32_t bucketNumber;
    uint32_t hashcode;
    int32_t keyHashCode;
    uint32_t seed;
    uint32_t incr;
    int32_t bucketsLength;
    CLR_RT_HeapBlock *key;
    CLR_RT_HeapBlock *bucket;
    CLR_RT_HeapBlock *bucketElement;
    CLR_RT_HeapBlock_Array *buckets;

    CLR_RT_HeapBlock *pThis;

    pThis = (CLR_RT_HeapBlock *)stack.This();
    FAULT_ON_NULL(pThis);

    key = stack.Arg1().Dereference();
    FAULT_ON_NULL_ARG(key);

    keyHashCode = stack.Arg2().NumericByRef().s4;

    buckets = pThis[FIELD___buckets].DereferenceArray();
    bucketsLength = buckets->m_numOfElements;

    hashcode = InitHash(keyHashCode, bucketsLength, &seed, &incr);

    bucketNumber = (int)(seed % (uint32_t)bucketsLength);

    do
    {
        bucketElement = ((CLR_RT_HeapBlock *)buckets->GetElement(bucketNumber));
        bucket = bucketElement->Dereference();

        if (bucket != NULL && bucket[BucketType::FIELD___key].Dereference() != NULL &&
            (bucket[BucketType::FIELD___hash].NumericByRef().u4 == hashcode) &&
            (CLR_RT_HeapBlock::ObjectsEqual(*bucket[BucketType::FIELD___key].Dereference(), *key, true)))
        {
            // found
            stack.SetResult_Boolean(true);

            NANOCLR_SET_AND_LEAVE(S_OK)
        }

        bucketNumber = (int32_t)((bucketNumber + incr) % (uint32_t)bucketsLength);

    } while (++entry < bucketsLength);

    // not found
    stack.SetResult_Boolean(false);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_collections_System_Collections_Hashtable::RemoveNative___VOID__OBJECT__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int32_t entry = 0;
    int32_t bucketNumber;
    uint32_t hashcode;
    int32_t keyHashCode;
    uint32_t seed;
    uint32_t incr;
    int32_t bucketsLength;
    CLR_RT_HeapBlock *key;
    CLR_RT_HeapBlock *bucket;
    CLR_RT_HeapBlock *bucketElement;
    CLR_RT_HeapBlock_Array *buckets;

    CLR_RT_HeapBlock *pThis;

    pThis = (CLR_RT_HeapBlock *)stack.This();
    FAULT_ON_NULL(pThis);

    key = stack.Arg1().Dereference();
    FAULT_ON_NULL_ARG(key);

    keyHashCode = stack.Arg2().NumericByRef().s4;

    buckets = pThis[FIELD___buckets].DereferenceArray();
    bucketsLength = buckets->m_numOfElements;

    hashcode = InitHash(keyHashCode, bucketsLength, &seed, &incr);

    bucketNumber = (int)(seed % (uint32_t)bucketsLength);

    do
    {
        bucketElement = ((CLR_RT_HeapBlock *)buckets->GetElement(bucketNumber));
        bucket = bucketElement->Dereference();

        if (bucket != NULL && bucket[BucketType::FIELD___key].Dereference() != NULL &&
            (bucket[BucketType::FIELD___hash].NumericByRef().u4 == hashcode) &&
            (CLR_RT_HeapBlock::ObjectsEqual(*bucket[BucketType::FIELD___key].Dereference(), *key, true)))
        {
            // Clear hash field, then key, then value

            bucket[BucketType::FIELD___hash].NumericByRef().u4 = 0;
            bucket[BucketType::FIELD___value].SetObjectReference(NULL);
            bucket[BucketType::FIELD___key].SetObjectReference(NULL);

            // subtract count field
            pThis[FIELD___count].NumericByRef().s4--;

            // update version
            pThis[FIELD___version].NumericByRef().s4++;

            NANOCLR_SET_AND_LEAVE(S_OK)
        }

        bucketNumber = (int32_t)((bucketNumber + incr) % (uint32_t)bucketsLength);

    } while (++entry < bucketsLength);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_collections_System_Collections_Hashtable::InsertNative___VOID__OBJECT__OBJECT__BOOLEAN__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    bool add;
    uint32_t entry = 0;
    int32_t bucketNumber;
    uint32_t hashcode;
    int32_t keyHashCode;
    uint32_t seed;
    uint32_t incr;
    uint32_t bucketsLength;
    CLR_RT_HeapBlock *key;
    CLR_RT_HeapBlock *newValue;
    CLR_RT_HeapBlock *bucket;
    CLR_RT_HeapBlock *bucketElement;
    CLR_RT_HeapBlock_Array *buckets;

    CLR_RT_HeapBlock *pThis;

    pThis = (CLR_RT_HeapBlock *)stack.This();
    FAULT_ON_NULL(pThis);

    key = stack.Arg1().Dereference();
    FAULT_ON_NULL_ARG(key);

    newValue = stack.Arg2().Dereference();

    add = (bool)stack.Arg3().NumericByRef().u1;

    keyHashCode = stack.Arg4().NumericByRef().s4;

    if (pThis[FIELD___count].NumericByRef().s4 >= pThis[FIELD___loadsize].NumericByRef().s4)
    {
        NANOCLR_CHECK_HRESULT(Expand(stack));
    }

    buckets = pThis[FIELD___buckets].DereferenceArray();
    bucketsLength = buckets->m_numOfElements;

    hashcode = InitHash(keyHashCode, bucketsLength, &seed, &incr);

    bucketNumber = (int)(seed % bucketsLength);

    do
    {
        bucketElement = ((CLR_RT_HeapBlock *)buckets->GetElement(bucketNumber));
        bucket = bucketElement->Dereference();

        // Insert the key/value pair into this bucket if this bucket is empty
        if (bucket == NULL || (bucket != NULL && bucket[BucketType::FIELD___key].Dereference() == NULL))
        {
            // We pretty much have to insert in this order.  Don't set hash
            // code until the value & key are set appropriately.

            CLR_RT_TypeDef_Index bucketTypeDef;
            CLR_RT_HeapBlock newBucket;

            if (bucket == NULL)
            {
                // find <Bucket> type, don't bother checking the result as it exists for sure
                g_CLR_RT_TypeSystem.FindTypeDef("Bucket", "System.Collections", bucketTypeDef);
                // create a new <Bucket>
                NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(newBucket, bucketTypeDef));
                bucketElement->LoadFromReference(newBucket);
                bucket = bucketElement->Dereference();
            }

            bucket[BucketType::FIELD___hash].NumericByRef().u4 = hashcode;
            bucket[BucketType::FIELD___value].SetObjectReference(newValue);
            bucket[BucketType::FIELD___key].SetObjectReference(key);

            // add count field
            pThis[FIELD___count].NumericByRef().s4++;

            // update version
            pThis[FIELD___version].NumericByRef().s4++;

            NANOCLR_SET_AND_LEAVE(S_OK)
        }

        if (bucket != NULL &&
            (CLR_RT_HeapBlock::ObjectsEqual(*bucket[BucketType::FIELD___key].Dereference(), *key, true)))
        {
            if (add)
            {
                // entry already exists
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER)
            }

            bucket[BucketType::FIELD___value].SetObjectReference(newValue);

            // update version
            pThis[FIELD___version].NumericByRef().s4++;

            NANOCLR_SET_AND_LEAVE(S_OK)
        }

        bucketNumber = (int32_t)((bucketNumber + incr) % bucketsLength);

    } while (++entry < bucketsLength);

    // got here, found no place for this
    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION)

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_collections_System_Collections_Hashtable::GetNative___OBJECT__OBJECT__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int32_t entry = 0;
    int32_t bucketNumber;
    uint32_t hashcode;
    int32_t keyHashCode;
    uint32_t seed;
    uint32_t incr;
    int32_t bucketsLength;
    CLR_RT_HeapBlock *key;
    CLR_RT_HeapBlock *bucket;
    CLR_RT_HeapBlock *bucketElement;
    CLR_RT_HeapBlock_Array *buckets;

    CLR_RT_HeapBlock *pThis;

    pThis = (CLR_RT_HeapBlock *)stack.This();
    FAULT_ON_NULL(pThis);

    key = stack.Arg1().Dereference();
    FAULT_ON_NULL_ARG(key);

    keyHashCode = stack.Arg2().NumericByRef().s4;

    buckets = pThis[FIELD___buckets].DereferenceArray();
    bucketsLength = buckets->m_numOfElements;

    hashcode = InitHash(keyHashCode, bucketsLength, &seed, &incr);

    bucketNumber = (int)(seed % (uint32_t)bucketsLength);

    do
    {
        bucketElement = ((CLR_RT_HeapBlock *)buckets->GetElement(bucketNumber));
        bucket = bucketElement->Dereference();

        if (bucket != NULL && (bucket[BucketType::FIELD___hash].NumericByRef().u4 == hashcode) &&
            (CLR_RT_HeapBlock::ObjectsEqual(*bucket[BucketType::FIELD___key].Dereference(), *key, true)))
        {
            stack.SetResult_Object(bucket[BucketType::FIELD___value].Dereference());
            NANOCLR_SET_AND_LEAVE(S_OK)
        }

        bucketNumber = (int32_t)((bucketNumber + incr) % (uint32_t)bucketsLength);
    } while (++entry < bucketsLength);

    // no key found
    stack.SetResult_Object(NULL);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_collections_System_Collections_Hashtable::GetPrimeNative___STATIC__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int32_t min;
    uint32_t i;

    min = stack.Arg0().NumericByRef().s4;

    if (min < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    for (i = 0; i < lenghtOfPrimes; i++)
    {
        if (s_primes[i] >= min)
        {
            SetResult_INT32(stack, s_primes[i]);
            break;
        }
    }

    if (i == lenghtOfPrimes)
    {
        // we are not supporting hastables bigger than this size
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_collections_System_Collections_Hashtable::Expand(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint32_t bucketsLength;
    uint32_t i;
    int32_t newSize;
    uint32_t seed;
    uint32_t incr;
    int32_t newBucketNumber;

    CLR_RT_HeapBlock *oldBucket;
    CLR_RT_HeapBlock *oldBucketElement;
    CLR_RT_HeapBlock *newBucketElement;
    CLR_RT_HeapBlock *bucket;
    CLR_RT_HeapBlock newBucketsHB;
    CLR_RT_HeapBlock_Array *buckets;
    CLR_RT_HeapBlock_Array *newBuckets;
    CLR_RT_HeapBlock *pThis;
    CLR_RT_TypeDef_Index bucketTypeDef;

    pThis = stack.This();

    buckets = pThis[FIELD___buckets].DereferenceArray();
    bucketsLength = buckets->m_numOfElements;

    newSize = 2 * bucketsLength;

    // Allow the hashtables to grow to maximum reasonable size before encountering capacity overflow.
    for (i = 0; i < lenghtOfPrimes; i++)
    {
        if (s_primes[i] >= newSize)
        {
            newSize = s_primes[i];
            break;
        }
    }

    // Don't replace any internal state until we've finished adding to the
    // new Bucket[].  This serves two purposes:
    //   1) Allow concurrent readers to see valid hashtable contents
    //      at all times
    //   2) Protect against an OutOfMemoryException while allocating this
    //      new Bucket[].

    // find <Bucket> type, don't bother checking the result as it exists for sure
    g_CLR_RT_TypeSystem.FindTypeDef("Bucket", "System.Collections", bucketTypeDef);

    // create a new array of <Bucket>
    CLR_RT_ProtectFromGC gc(newBucketsHB);

    if (i == lenghtOfPrimes)
    {
        // we are not supporting hastables bigger than this size
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED)
    }

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(newBucketsHB, newSize, bucketTypeDef))
    newBuckets = newBucketsHB.DereferenceArray();

    for (uint32_t bucketNumber = 0; bucketNumber < bucketsLength; bucketNumber++)
    {
        oldBucketElement = ((CLR_RT_HeapBlock *)buckets->GetElement(bucketNumber));
        oldBucket = oldBucketElement->Dereference();

        // sanity check: can only clear buckets that already have objects
        if (oldBucket != NULL && oldBucket[BucketType::FIELD___key].Dereference() != NULL)
        {
            seed = oldBucket[BucketType::FIELD___hash].NumericByRef().u4;
            incr = 1 + ((seed * HashPrime) % ((uint32_t)newSize - 1));

            newBucketNumber = (int)(seed % (uint32_t)newSize);

            while (true)
            {
                newBucketElement = ((CLR_RT_HeapBlock *)newBuckets->GetElement(newBucketNumber));
                bucket = newBucketElement->Dereference();

                if (bucket == NULL || bucket[BucketType::FIELD___key].Dereference() == NULL)
                {
                    // create a new <Bucket>
                    CLR_RT_HeapBlock newBucket;

                    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(newBucket, bucketTypeDef))

                    newBucketElement->LoadFromReference(newBucket);
                    bucket = newBucketElement->Dereference();

                    // copy over from old bucket
                    bucket[BucketType::FIELD___hash].NumericByRef().u4 =
                        oldBucket[BucketType::FIELD___hash].NumericByRef().u4;
                    bucket[BucketType::FIELD___value].SetObjectReference(
                        oldBucket[BucketType::FIELD___value].Dereference());
                    bucket[BucketType::FIELD___key].SetObjectReference(
                        oldBucket[BucketType::FIELD___key].Dereference());

                    break;
                }

                newBucketNumber = (int)((newBucketNumber + incr) % (uint32_t)newSize);
            }
        }
    }

    pThis[FIELD___buckets].SetObjectReference(newBucketsHB.DereferenceArray());
    pThis[FIELD___loadsize].NumericByRef().s4 = (CLR_INT32)(pThis[FIELD___loadFactor].NumericByRef().r4 * newSize);

    // update version
    pThis[FIELD___version].NumericByRef().s4++;

    NANOCLR_NOCLEANUP();
}

uint32_t Library_nf_system_collections_System_Collections_Hashtable::InitHash(
    const int32_t keyHashCode,
    int32_t hashsize,
    uint32_t *seed,
    uint32_t *incr)
{
    uint32_t hashcode = keyHashCode;

    *seed = hashcode;

    // compute and store incr value
    *incr = 1 + ((hashcode * HashPrime) % ((uint32_t)hashsize - 1));

    return hashcode;
}
