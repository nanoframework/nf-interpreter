//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Implements the Mynewt-compatible crc16_ccitt() function
//
// Algorithm: CRC-16/CCITT
//   Polynomial : 0x1021
//   Initial CRC: 0x0000 (caller-provided via initial_crc)
//   Input/Output: not reflected
//   Final XOR  : none
//
// This matches the CRC used by MCUboot's NLIP framing in boot_serial.c
// to frame and verify serial recovery protocol frames.

#include "crc/crc16.h"

uint16_t crc16_ccitt(uint16_t initial_crc, const void *buf, int len)
{
    const uint8_t *p = (const uint8_t *)buf;
    uint16_t crc = initial_crc;

    while (len-- > 0)
    {
        crc ^= (uint16_t)((uint16_t)(*p++) << 8);
        for (int i = 0; i < 8; i++)
        {
            if (crc & 0x8000U)
            {
                crc = (uint16_t)((crc << 1) ^ 0x1021U);
            }
            else
            {
                crc = (uint16_t)(crc << 1);
            }
        }
    }

    return crc;
}
