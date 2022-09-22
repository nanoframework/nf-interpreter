//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
//
// CPU_SPI_xxxx
//
// Read / Write physical SPI bus
//

#include <targetHAL.h>
#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>

#include <sys_dev_spi_native.h>
#include "Board.h"
#include <ti/drivers/SPI.h>

// struct representing the SPI
struct NF_PAL_SPI
{
    SPI_Handle masterSpi;
    SPI_Params spiParams;
    SPI_Transaction *transactions;
    uint8_t transactionCount;
    SPI_OP_STATUS status;
    SPI_Callback callback;
};

/////////////////////////////////////////////////////
// SPI PAL strucs declared in win_dev_spi_native.h //
/////////////////////////////////////////////////////
NF_PAL_SPI SPI1_PAL;

void SpiCallback(SPI_Handle handle, SPI_Transaction *transaction)
{
    NATIVE_INTERRUPT_START

    // are there any other transactions pending?
    if (SPI1_PAL.transactionCount)
    {
        // start transfer of next transaction
        SPI_transfer(SPI1_PAL.masterSpi, &SPI1_PAL.transactions[SPI1_PAL.transactionCount--]);
    }
    else
    {
        // no more transactions
        // fire callback for SPI transaction complete
        SPI1_PAL.status = SPI_OP_COMPLETE;
        SPI1_PAL.callback(0);
    }

    NATIVE_INTERRUPT_END
}

// Give a complete low-level SPI configuration from SPI_DEVICE_CONFIGURATION
void GetSPIConfig(const SPI_DEVICE_CONFIGURATION &spiDeviceConfig, SPI_WRITE_READ_SETTINGS &wrc)
{
    SPI_Params spiParams;

    // Open SPI as slave (default)
    SPI_Params_init(&spiParams);
    spiParams.frameFormat = (SPI_FrameFormat)spiDeviceConfig.Spi_Mode;

    // default to slave
    // spiParams.mode = spiDeviceConfig.BusMode == SpiBusMode_master ? SPI_MASTER : SPI_SLAVE;
    spiParams.mode = SPI_SLAVE;

    spiParams.transferCallbackFxn = SpiCallback;
    spiParams.dataSize = wrc.Bits16ReadWrite ? 16 : 8;
    spiParams.bitRate = spiDeviceConfig.Clock_RateHz;

    spiParams.transferMode = SPI_MODE_CALLBACK;

    SPI1_PAL.masterSpi = SPI_open(Board_SPI_SLAVE, &spiParams);

    SPI1_PAL.status = SPI_OP_READY;
    SPI1_PAL.callback = wrc.callback;
}

bool CPU_SPI_Initialize(uint8_t bus, SPI_DEVICE_CONFIGURATION &spiDeviceConfig)
{
    (void)bus;
    (void)spiDeviceConfig;

    return true;
}

bool CPU_SPI_Uninitialize(uint8_t bus)
{
    (void)bus;

    SPI_close(SPI1_PAL.masterSpi);
    return true;
}

// Performs a read/write operation on 8-bit word data.
//
// Parameters
//  deviceHandle
//      Device handle from add_device
//  sdev
//		reference to SPI_DEVICE_CONFIGURATION
//  wrc
//		reference to SPI_WRITE_READ_SETTINGS
//  writeData
//      A pointer to the buffer from which the data is to be written to the device.
//  writeSize
//      The number of elements(8 or 16) to be written.
//  readData
//      A pointer to the buffer into which the data is to be read from the device.
//  readSize
//      The number of elements(8 or 16) to be read.
//
// return S_OK=Successful, Async started=CLR_BUSY, Error=CLR_E_OUT_OF_MEMORY, CLR_E_INVALID_PARAMETER, CLR_E_FAIL
//

// TODO The SPI driver in TI-RTOS doesn't handle the Chip select
// TODO This needs to be added to this driver
HRESULT CPU_SPI_nWrite_nRead(
    uint32_t deviceHandle,
    SPI_DEVICE_CONFIGURATION &sdev,
    SPI_WRITE_READ_SETTINGS &wrc,
    uint8_t *writeData,
    int32_t writeSize,
    uint8_t *readData,
    int32_t readSize)
{
    (void)deviceHandle;

    NANOCLR_HEADER();
    {
        unsigned char *readDataBuffer = NULL;

        GetSPIConfig(sdev, wrc);

        if (wrc.fullDuplex)
        {
            // we'll be doing this on a single transaction
            SPI1_PAL.transactions = (SPI_Transaction *)platform_malloc(sizeof(SPI_Transaction));
            SPI1_PAL.transactionCount = 1;
        }
        else
        {
            // sequential io we need two transactions
            SPI1_PAL.transactions = (SPI_Transaction *)platform_malloc(sizeof(SPI_Transaction) * 2);
            SPI1_PAL.transactionCount = 2;
        }

        // check allocation
        if (SPI1_PAL.transactions == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        if (writeSize)
        {
            // get the size of the buffer by reading the number of elements in the CLR_RT_HeapBlock_Array
            SPI1_PAL.transactions[0].count = writeSize;

            // buffer data width is 8bit or 16bits
            // get the pointer to the write buffer which is ptrs to UINT16 (2 bytes) or UINT8_t data
            SPI1_PAL.transactions[0].txBuf = writeData;
        }

        if (wrc.fullDuplex)
        {
            if (readSize)
            {
                // buffer data  16bit or 8bit data
                SPI1_PAL.transactions[0].rxBuf = readData;
            }
        }
        else
        {
            // on a sequential transmition we TX first and then RX as 2nd transaction
            if (readSize)
            {
                // get the size of the buffer by reading the number of elements in the CLR_RT_HeapBlock_Array
                SPI1_PAL.transactions[1].count = readSize;
                SPI1_PAL.transactions[1].rxBuf = readData;
            }
        }

        // start transfer of 1st transaction
        SPI_transfer(SPI1_PAL.masterSpi, &SPI1_PAL.transactions[0]);

        // take one from pending transcations counter
        SPI1_PAL.transactionCount--;

        SPI1_PAL.status = SPI_OP_RUNNING;

        // Inform caller async operation started
        NANOCLR_SET_AND_LEAVE(CLR_E_BUSY);
    }

    NANOCLR_NOCLEANUP();
}

// Performs a read/write operation on 16-bit word data.
HRESULT CPU_SPI_nWrite16_nRead16(
    uint32_t deviceHandle,
    SPI_DEVICE_CONFIGURATION &sdev,
    SPI_WRITE_READ_SETTINGS &swrs,
    uint16_t *writePtr,
    int32_t writeSize,
    uint16_t *readPtr,
    int32_t readSize)
{
    swrs.Bits16ReadWrite = true;
    return CPU_SPI_nWrite_nRead(deviceHandle, sdev, swrs, (uint8_t *)writePtr, writeSize, (uint8_t *)readPtr, readSize);
}

// Return status of current SPI operation
// Used to find status of an Async SPI call ( Not supported ), just return complete
SPI_OP_STATUS CPU_SPI_OP_STATUS(uint8_t busIndex, uint32_t deviceHandle)
{
    (void)busIndex;
    (void)deviceHandle;

    return SPI1_PAL.status;
}

// Return map of available SPI buses as a bit map
uint32_t CPU_SPI_PortsMap()
{
    // SPI1(0x01)
    return 0x01;
}

// Return SPI minimum clock frequency
HRESULT CPU_SPI_MinClockFrequency(uint32_t spiBus, int32_t *frequency)
{
    // bus index is 0 based, here it's 1 based
    if (spiBus >= NUM_SPI_BUSES)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    // TODO check what is minimum ( min clock that can be configured on chip, master only)
    *frequency = 10000000 / 256;

    return S_OK;
}

// Return SPI maximum clock frequency
HRESULT CPU_SPI_MaxClockFrequency(uint32_t spiBus, int32_t *frequency)
{
    // bus index is 0 based, here it's 1 based
    if (spiBus >= NUM_SPI_BUSES)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    *frequency = 40000000;

    return S_OK;
}

//
// Return the number of chip select lines available on the bus.
// TODO this still needs to be sorted as no CS handling
uint32_t CPU_SPI_ChipSelectLineCount(uint32_t busIndex)
{
    (void)busIndex;

    return MAX_SPI_DEVICES;
}
