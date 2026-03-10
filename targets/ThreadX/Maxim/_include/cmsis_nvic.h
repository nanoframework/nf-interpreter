//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _CMSIS_NVIC_H_
#define _CMSIS_NVIC_H_

extern void (*ramVectorTable[MXC_IRQ_COUNT])(void);

#define NVIC_NUM_VECTORS        (MXC_IRQ_COUNT)
#define NVIC_RAM_VECTOR_ADDRESS (ramVectorTable)    // Vectors positioned at start of RAM

#endif // _CMSIS_NVIC_H_
