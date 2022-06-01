//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <esp32_idf.h>

#ifndef ESP_ROM_HAS_CRC_BE
extern const uint32_t c_CRCTable[256];
const uint32_t *myCRCTable = c_CRCTable;
#endif

// strong implementation of this function specific to the ESP32
uint32_t SUPPORT_ComputeCRC(const void *rgBlock, const uint32_t nLength, const uint32_t crc)
{
    uint8_t const *ptr = (const unsigned char *)rgBlock;
    int32_t lenght = nLength;
    uint32_t newCrc = crc;

#ifdef ESP_ROM_HAS_CRC_BE

    // this series has ROM support for CRC32

    // IDF CRC32 polinomial required this tweak
    if (crc == 0)
    {
        newCrc = 0xFFFFFFFF;
    }

    newCrc = ~esp_rom_crc32_be(newCrc, ptr, lenght);

#else

    // this series does not have ROM support for CRC32
    // use our implementation and CRC table
    while (lenght-- > 0)
    {
        newCrc = myCRCTable[((newCrc >> 24) ^ (*ptr++)) & 0xFF] ^ (newCrc << 8);
    }

#endif

    return newCrc;
}
