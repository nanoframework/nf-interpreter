//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_esp32_rmt_native.h"

typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtSymbol RmtSymbol;

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtUtils::
    DecodeSymbolArrayToBytes___STATIC__SZARRAY_U1__SZARRAY_nanoFrameworkHardwareEsp32RmtRmtSymbol__I4__BOOLEAN__I4__I4__BOOLEAN(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array *symbolsArray;
    int threshold;
    bool msbFirst;
    int symbolOffset;
    int symbolLength;
    bool spaceIsHigh;

    int symbolCount;
    int decodeCount;
    int byteCount;

    CLR_RT_HeapBlock_Array *outArray;
    uint8_t *outBytes;

    CLR_RT_HeapBlock *elem;
    uint32_t duration0;
    uint32_t duration1;
    bool level0;
    bool level1;
    bool level0IsSpace;
    bool level1IsSpace;

    int i;
    int bitIndex;
    int byteIndex;
    int bitPos;
    int msbPos;

    // Read arguments
    symbolsArray = stack.Arg0().DereferenceArray();
    threshold = stack.Arg1().NumericByRef().s4;
    msbFirst = (stack.Arg2().NumericByRef().u1 != 0);
    symbolOffset = stack.Arg3().NumericByRef().s4;
    symbolLength = stack.Arg4().NumericByRef().s4;
    spaceIsHigh = (stack.Arg5().NumericByRef().u1 != 0);

    if (symbolsArray == nullptr)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NULL_REFERENCE);
    }

    symbolCount = symbolsArray->m_numOfElements;

    if (symbolOffset < 0 || symbolOffset >= symbolCount)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    // Determine number of symbols to decode
    if (symbolLength < 0)
    {
        decodeCount = symbolCount - symbolOffset;
    }
    else
    {
        if (symbolOffset + symbolLength > symbolCount)
        {
            decodeCount = symbolCount - symbolOffset;
        }
        else
        {
            decodeCount = symbolLength;
        }
    }

    byteCount = (decodeCount + 7) / 8;

    // Allocate return byte[] using PushValueAndClear()
    NANOCLR_CHECK_HRESULT(
        CLR_RT_HeapBlock_Array::CreateInstance(stack.PushValueAndClear(), byteCount, g_CLR_RT_WellKnownTypes.m_UInt8));

    outArray = stack.TopValue().DereferenceArray();
    outBytes = outArray->GetFirstElement();

    memset(outBytes, 0, byteCount);

    // Decode symbols → bits → bytes
    for (i = 0; i < decodeCount; i++)
    {
        // get element slot and unwrap boxed struct
        elem = ((CLR_RT_HeapBlock *)symbolsArray->GetElement(symbolOffset + i))->Dereference();

        // Read struct fields
        level0 = elem[RmtSymbol::FIELD___level0].NumericByRef().u1 != 0;
        level1 = elem[RmtSymbol::FIELD___level1].NumericByRef().u1 != 0;
        duration0 = elem[RmtSymbol::FIELD___duration0].NumericByRef().u4;
        duration1 = elem[RmtSymbol::FIELD___duration1].NumericByRef().u4;

        // Determine which duration is SPACE (bit-encoding)
        level0IsSpace = (level0 == spaceIsHigh);
        level1IsSpace = (level1 == spaceIsHigh);

        uint32_t d;

        if (level0IsSpace)
        {
            d = duration0;
        }
        else if (level1IsSpace)
        {
            d = duration1;
        }
        else
        {
            d = duration1; // fallback (matches ESP-IDF behavior)
        }

        // Convert duration to bit
        bitIndex = i;
        byteIndex = bitIndex / 8;
        bitPos = bitIndex % 8;

        if (msbFirst)
        {
            msbPos = 7 - bitPos;

            if (d >= (uint32_t)threshold)
            {
                outBytes[byteIndex] |= (1 << msbPos);
            }
        }
        else
        {
            if (d >= (uint32_t)threshold)
            {
                outBytes[byteIndex] |= (1 << bitPos);
            }
        }
    }

    NANOCLR_NOCLEANUP();
}
