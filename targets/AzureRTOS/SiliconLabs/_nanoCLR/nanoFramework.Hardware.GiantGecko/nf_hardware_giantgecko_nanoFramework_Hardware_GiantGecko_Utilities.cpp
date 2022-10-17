//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_hardware_giantgecko.h"
#include <em_device.h>

HRESULT Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_Utilities::
    NativeGetDeviceUniqueId___STATIC__VOID__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // device Unique ID is 64 bits long
    uint8_t deviceUID[8];

    // high 32 bits
    uint32_t rawId = DEVINFO->UNIQUEH;

    deviceUID[3] = rawId;
    rawId >>= 8;
    deviceUID[2] = rawId;
    rawId >>= 8;
    deviceUID[1] = rawId;
    rawId >>= 8;
    deviceUID[0] = rawId;

    // low 32 bits
    rawId = DEVINFO->UNIQUEL;

    deviceUID[7] = rawId;
    rawId >>= 8;
    deviceUID[6] = rawId;
    rawId >>= 8;
    deviceUID[5] = rawId;
    rawId >>= 8;
    deviceUID[4] = rawId;

    // dereference the data buffer from the argument
    CLR_RT_HeapBlock_Array *buffer = stack.Arg0().DereferenceArray();

    // get a the pointer to the byffer by using the first element of the array
    uint8_t *data = buffer->GetFirstElement();

    // 64 bit unique device ID => 16 bytes
    // memory copy from the address pointed by deviceUID
    memcpy(data, deviceUID, 8);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_Utilities::
    NativeGetProductionRevision___STATIC__U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // read PROD_REV from 31:24 bits
    stack.SetResult_U1(DEVINFO->PART >> 24);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_Utilities::NativeGetDeviceFamily___STATIC__U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // read DEVICE_FAMILY from 23:16 bits
    stack.SetResult_U1(DEVINFO->PART >> 16);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_Utilities::NativeGetDeviceNumber___STATIC__U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // read DEVICE_NUMBER from 15:0 bits
    stack.SetResult_U4((uint16_t)DEVINFO->PART);

    NANOCLR_NOCLEANUP_NOLABEL();
}
