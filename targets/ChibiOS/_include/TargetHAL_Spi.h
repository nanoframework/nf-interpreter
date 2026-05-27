//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_HAL_SPI_H
#define TARGET_HAL_SPI_H

// # of buses
#if defined(RP_GPIO_NUM_LINES)
// RP2040/RP2350: 2 SPI controllers (SPI0, SPI1)
#define NUM_SPI_BUSES 2
#else
// STM32: up to 5 SPI buses
#define NUM_SPI_BUSES 5
#endif

// Maximum number of devices per SPI bus
#define MAX_SPI_DEVICES 5

#endif //TARGET_HAL_SPI_H
