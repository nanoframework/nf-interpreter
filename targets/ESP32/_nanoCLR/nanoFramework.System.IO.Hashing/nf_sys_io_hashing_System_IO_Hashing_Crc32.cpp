//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nf_sys_io_hashing.h>
#include <esp32_idf.h>

typedef Library_corlib_native_System_SpanByte SpanByte;

HRESULT Library_nf_sys_io_hashing_System_IO_Hashing_Crc32::ComputeHash___STATIC__U4__U4__SystemSpanByte(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *bufferSpanByte;
    CLR_RT_HeapBlock_Array *buffer;
    uint8_t *bufferData = NULL;
    int16_t bufferSize = 0;
    int16_t bufferOffset = 0;
    uint32_t crc32 = 0;
    uint32_t hash = 0;

    // get a pointer to the managed object instance and check that it's not NULL
    bufferSpanByte = stack.Arg1().Dereference();
    FAULT_ON_NULL_ARG(bufferSpanByte);

    // get initial CRC32 value
    crc32 = stack.Arg0().NumericByRef().u4;

    // get buffer
    buffer = bufferSpanByte[SpanByte::FIELD___array].DereferenceArray();

    // Get the write offset
    bufferOffset = bufferSpanByte[SpanByte::FIELD___start].NumericByRef().s4;

    // use the span length as write size, only the elements defined by the span must be written
    bufferSize = bufferSpanByte[SpanByte::FIELD___length].NumericByRef().s4;
    bufferData = (unsigned char *)buffer->GetElement(bufferOffset);

#ifdef ESP_ROM_HAS_CRC_LE

    // this series has ROM support for CRC32

    if (bufferSize == 0)
    {
        hash = 0xFFFFFFFF;
    }
    else
    {
        hash = ~esp_rom_crc32_le(~crc32, bufferData, bufferSize);
    }

#else

    // this series does not have ROM support for CRC32
    // for the time being this is not supported
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

#endif

    stack.SetResult_U4(hash);

    NANOCLR_NOCLEANUP();
}
