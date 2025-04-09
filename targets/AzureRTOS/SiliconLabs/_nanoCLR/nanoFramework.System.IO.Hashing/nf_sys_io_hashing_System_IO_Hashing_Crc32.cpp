//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nf_sys_io_hashing.h>
// #include <stdio.h>
// #include <alloca.h>
#include <em_chip.h>
#include <em_cmu.h>
#include <em_gpcrc.h>

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

    if (bufferSize == 0)
    {
        hash = 0xFFFFFFFF;
    }
    else
    {
        // Declare init structs
        GPCRC_Init_TypeDef init = GPCRC_INIT_DEFAULT;
        init.initValue = crc32;
        // reverse all bits of the incoming message
        init.reverseBits = false;
        // all buffers are treated as byte buffers
        init.enableByteMode = true;

        // Initialize GPCRC
        GPCRC_Init(GPCRC, &init);

        // Prepare GPCRC_DATA for inputs
        GPCRC_Start(GPCRC);

        for (int32_t i = 0; i < bufferSize; i++)
        {
            GPCRC_InputU8(GPCRC, *bufferData++);
        }

        hash = GPCRC_DataRead(GPCRC);
    }

    stack.SetResult_U4(hash);

    NANOCLR_NOCLEANUP();
}
