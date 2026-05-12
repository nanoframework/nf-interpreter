//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Declares the Mynewt-compatible crc16_ccitt() function

#ifndef CRC_CRC16_H
#define CRC_CRC16_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // CRC-16/CCITT (polynomial 0x1021). Returns the CRC of buf[0..len-1],
    // seeded with initial_crc. To compute a CRC over multiple segments,
    // pass the result of the previous call as initial_crc.
    uint16_t crc16_ccitt(uint16_t initial_crc, const void *buf, int len);

// Initial CRC seed value used by MCUboot's NLIP framing in boot_serial.c.
// CRC-16/CCITT starts with an all-zeros register.
#define CRC16_INITIAL_CRC 0U

#ifdef __cplusplus
}
#endif

#endif // CRC_CRC16_H
