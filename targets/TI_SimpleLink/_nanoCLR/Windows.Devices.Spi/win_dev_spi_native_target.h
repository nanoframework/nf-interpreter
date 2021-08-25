//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef WIN_DEV_SPI_NATIVE_TARGET_H
#define WIN_DEV_SPI_NATIVE_TARGET_H

#include <win_dev_spi_native.h>
#include "Board.h"
#include <ti/drivers/SPI.h>

// struct representing the SPI 
struct NF_PAL_SPI
{
    SPI_Handle          masterSpi;
    SPI_Params          spiParams;
    SPI_Transaction*    transactions;
    uint8_t             transactionCount;
    SPI_OP_STATUS       status;
    SPI_Callback        callback;
};

///////////////////////////////////////////
// declaration of the the SPI PAL strucs //
///////////////////////////////////////////
extern NF_PAL_SPI SPI1_PAL;

#endif //WIN_DEV_SPI_NATIVE_TARGET_H
