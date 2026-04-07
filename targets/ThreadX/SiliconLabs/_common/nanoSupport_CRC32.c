//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <stdio.h>
#include <alloca.h>
#include <em_chip.h>
#include <em_cmu.h>
#include <em_gpcrc.h>

// strong implementation of this function specific Gecko targets
uint32_t SUPPORT_ComputeCRC(const void *rgBlock, const uint32_t nLength, const uint32_t crc)
{
    const uint8_t *ptr = (const uint8_t *)rgBlock;

    // anything to do here?
    if (nLength == 0)
    {
        return crc;
    }

    // Declare init structs
    GPCRC_Init_TypeDef init = GPCRC_INIT_DEFAULT;

    // reverse all bits of the incoming message
    init.reverseBits = true;
    // all buffers are treated as byte buffers
    init.enableByteMode = true;

    // Initialize GPCRC
    GPCRC_Init(GPCRC, &init);

    if (crc == 0)
    {
        // this is NOT continuing a previous CRC calculation

        // set intial value
        GPCRC_InitValueSet(GPCRC, crc);

        // Prepare GPCRC_DATA for inputs
        GPCRC_Start(GPCRC);
    }

    for (uint32_t i = 0; i < nLength; i++)
    {
        GPCRC_InputU8(GPCRC, *ptr++);
    }

    return GPCRC_DataReadBitReversed(GPCRC);
};

void InitGpCrc(void)
{
    // Enable clocks required
    CMU_ClockEnable(cmuClock_GPCRC, true);

    // Declare init structs
    GPCRC_Init_TypeDef init = GPCRC_INIT_DEFAULT;

    // reverse all bits of the incoming message
    init.reverseBits = true;
    // all buffers are treated as byte buffers
    init.enableByteMode = true;

    // Initialize GPCRC
    GPCRC_Init(GPCRC, &init);
}
