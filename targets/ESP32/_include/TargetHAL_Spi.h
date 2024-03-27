//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_HAL_SPI_H
#define TARGET_HAL_SPI_H

#include "spi_pins.h"

// # of buses, first can't be used because it's used by SPI flash
#define NUM_SPI_BUSES SOC_SPI_PERIPH_NUM - 1

// Maximum number of devices per SPI bus
// this is valid for all series, except S2, C3 and H2 (which support up to 6 devices, depending on the chip)
#define MAX_SPI_DEVICES 3

#endif // TARGET_HAL_SPI_H
