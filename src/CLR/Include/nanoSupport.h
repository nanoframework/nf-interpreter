//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _SUPPORT_NANOSUPPORT_H_
#define _SUPPORT_NANOSUPPORT_H_

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

uint32_t SUPPORT_ComputeCRC(const void* rgBlock, const uint32_t nLength, const uint32_t crc);

#if defined(__cplusplus)
}
#endif

#endif // _SUPPORT_NANOSUPPORT_H_
