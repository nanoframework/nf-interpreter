//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <stdio.h>
#include <alloca.h>>
#include <em_chip.h>
#include <em_cmu.h>
#include <em_gpcrc.h>

// strong implementation of this function specific Gecko targets
uint32_t SUPPORT_ComputeCRC(const void *rgBlock, const uint32_t nLength, const uint32_t crc)
{
    // Prepare GPCRC_DATA for inputs
    GPCRC_Start(GPCRC);



    uint32_t myCrc = crcCompute(rgBlock, nLength, crc);

    crcReleaseModule();

    return myCrc;
};

void InitGpCrc(void)
{
    // Enable clocks required
    CMU_ClockEnable(cmuClock_GPCRC, true);

    // Declare init structs
    GPCRC_Init_TypeDef init = GPCRC_INIT_DEFAULT;

    // Starting value in GPCRC_DATA
    init.initValue = 0x04C11DB7;
    // Reset GPCRC_DATA to 0xFFFF_FFFF after every read
    init.autoInit = true;
    // Reverse all bits of the incoming message
    init.reverseBits = true;

    // Initialize GPCRC
    GPCRC_Init(GPCRC, &init);
}
