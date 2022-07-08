//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_spi_native_target.h>

/////////////////////////////////////////////////////
// SPI PAL strucs declared in sys_dev_spi_native.h //
/////////////////////////////////////////////////////
#if GECKO_USE_SPI0 == TRUE
NF_PAL_SPI SPI0_PAL;
#endif
#if GECKO_USE_SPI1 == TRUE
NF_PAL_SPI SPI1_PAL;
#endif
#if GECKO_USE_SPI2 == TRUE
NF_PAL_SPI SPI2_PAL;
#endif
#if GECKO_USE_SPI3 == TRUE
NF_PAL_SPI SPI3_PAL;
#endif
#if GECKO_USE_SPI4 == TRUE
NF_PAL_SPI SPI4_PAL;
#endif
#if GECKO_USE_SPI5 == TRUE
NF_PAL_SPI SPI5_PAL;
#endif

// // Callback used when a async opertion completes
// static void SpiCallback(int BusIndex)
// {
//     (void)spip;

//     NATIVE_INTERRUPT_START

//     NF_PAL_SPI *palSpi = NULL;

//     // Find the NF_PAL_SPI * for driver
// #if STM32_SPI_USE_SPI2
//     if (spip == &SPID1)
//     {
//         palSpi = &SPI2_PAL;
//     }
// #endif
// #if STM32_SPI_USE_SPI3
//     if (spip == &SPID2)
//     {
//         palSpi = &SPI3_PAL;
//     }
// #endif
// #if STM32_SPI_USE_SPI4
//     if (spip == &SPID3)
//     {
//         palSpi = &SPI4_PAL;
//     }
// #endif
// #if STM32_SPI_USE_SPI5
//     if (spip == &SPID4)
//     {
//         palSpi = &SPI5_PAL;
//     }
// #endif
// #if STM32_SPI_USE_SPI6
//     if (spip == &SPID5)
//     {
//         palSpi = &SPI6_PAL;
//     }
// #endif
// #if STM32_SPI_USE_SPI6
//     if (spip == &SPID6)
//     {
//         palSpi = &SPI6_PAL;
//     }
// #endif

//     // check if there is any Rx operation due
//     if (palSpi->SequentialTxRx)
//     {
//         // yes there is!
//         // clear flag and...
//         palSpi->SequentialTxRx = false;

//         // ... start it
//         if (palSpi->busConfiguration)
//         {
//             // half duplex operation, clear output enable bit
//             palSpi->Driver->spi->CR1 &= ~SPI_CR1_BIDIOE;
//         }
//         spiStartReceive(palSpi->Driver, palSpi->ReadSize, palSpi->ReadBuffer);
//     }
//     else
//     {
//         // all done here!

//         if (palSpi->ReadSize > 0)
//         {
//             // because this was a Read transaction, need to copy from DMA buffer to managed buffer
//             int ReadSize = palSpi->ReadSize;

//             // Adjust read size for data width of 16bits
//             if (palSpi->BufferIs16bits)
//             {
//                 ReadSize *= 2;
//             }
//         }

//         // fire callback for SPI transaction complete
//         // only if callback set
//         if (palSpi->Callback)
//             palSpi->Callback(palSpi->BusIndex);
//     }

//     NATIVE_INTERRUPT_END
// };

// Return the NF_PAL structure for busIndex
// Return NULL is invalid bus
NF_PAL_SPI *GetNfPalfromBusIndex(uint8_t busIndex)
{
    NF_PAL_SPI *palSpi = NULL;

    // get the PAL struct for the SPI bus
    switch (busIndex)
    {

#if GECKO_USE_SPI0 == TRUE
        case 0:
            palSpi = &SPI0_PAL;
            break;
#endif

#if GECKO_USE_SPI1 == TRUE
        case 1:
            palSpi = &SPI1_PAL;
            break;
#endif

#if GECKO_USE_SPI2 == TRUE
        case 2:
            palSpi = &SPI2_PAL;
            break;
#endif

#if GECKO_USE_SPI3 == TRUE
        case 3:
            palSpi = &SPI3_PAL;
            break;
#endif

#if GECKO_USE_SPI4 == TRUE
        case 4:
            palSpi = &SPI4_PAL;
            break;
#endif

#if GECKO_USE_SPI5 == TRUE
        case 5:
            palSpi = &SPI5_PAL;
            break;
#endif

        default:
            // the requested SPI bus is not valid
            break;
    }

    return palSpi;
}

// Give a complete low-level SPI configuration from passed SPI_DEVICE_CONFIGURATION
void GetSPIConfig(SPI_DEVICE_CONFIGURATION &config, SPI_WRITE_READ_SETTINGS &wrc, SPIDRV_Init_t *spiDrivInit)
{
    (void)config;
    (void)wrc;

    // SPI mode
    // matches SPIDRV_ClockMode_t
    spiDrivInit->clockMode = config.Spi_Mode;

    // set data transfer length
    if (wrc.Bits16ReadWrite)
    {
        spiDrivInit->frameLength = 16;
    }
    else
    {
        spiDrivInit->frameLength = 8;
    }

    // baud rate
    spiDrivInit->bitRate = config.Clock_RateHz;

    // Sets the order of bytes transmission : MSB first or LSB first
    if (config.DataOrder16 == DataBitOrder_LSB)
    {
        spiDrivInit->bitOrder = spidrvBitOrderLsbFirst;
    }
    else
    {
        spiDrivInit->bitOrder = spidrvBitOrderMsbFirst;
    }

    // TODO handle half duplex
    // set bus configuration
    //     if (config.busConfiguration)
    //     }
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
HRESULT CPU_SPI_nWrite_nRead(
    uint32_t deviceHandle,
    SPI_DEVICE_CONFIGURATION &sdev,
    SPI_WRITE_READ_SETTINGS &wrc,
    uint8_t *writeBuffer,
    int32_t writeSize,
    uint8_t *readBuffer,
    int32_t readSize)
{
    NANOCLR_HEADER();
    {
        bool busConfigIsHalfDuplex;
        NF_PAL_SPI *palSpi = (NF_PAL_SPI *)deviceHandle;
        // If callback then use aync operation
        bool sync = (wrc.callback == 0);

        // Save width of transfer
        palSpi->BufferIs16bits = wrc.Bits16ReadWrite;

        // Callback sync / async
        palSpi->Callback = wrc.callback;

        if (writeBuffer != NULL)
        {
            palSpi->WriteSize = writeSize;
        }

        if (readBuffer != NULL)
        {
            palSpi->ReadSize = readSize;
        }

        // === Setup the operation and init buffers ===
        palSpi->BusIndex = sdev.Spi_Bus;

        // get the SPI configuration, depending on passed parameters and buffer element size
        GetSPIConfig(sdev, wrc, &(palSpi->Handle->initData));

        // set bus config flag
        busConfigIsHalfDuplex = palSpi->BusConfiguration == SpiBusConfiguration_HalfDuplex;

        // // Clear callback if sync
        // if (sync)
        // {
        //     palSpi->Configuration..data_cb = NULL;
        // }

        if (writeBuffer != NULL)
        {
            // set the pointer to the write buffer as BYTE
            palSpi->WriteBuffer = (uint8_t *)writeBuffer;
        }

        if (readBuffer != NULL)
        {
            // set DMA read buffer
            if (palSpi->ReadSize > 0)
            {
                palSpi->ReadBuffer = (uint8_t *)readBuffer;
            }
        }

        // // because the bus access is shared, acquire and select the appropriate bus
        // spiAcquireBus(palSpi->Driver);
        // spiStart(palSpi->Driver, &palSpi->Configuration);

        // // just to satisfy the driver ceremony, no actual implementation for STM32
        // spiSelect(palSpi->Driver);

        if (sync)
        {
            // Sync operation
            // perform SPI operation using driver's SYNC API
            if (palSpi->WriteSize != 0 && palSpi->ReadSize != 0)
            {
                // Transmit+Receive
                if (wrc.fullDuplex)
                {
                    // // Full duplex
                    // // Uses the largest buffer size as transfer size
                    // spiExchange(
                    //     palSpi->Driver,
                    //     palSpi->WriteSize > palSpi->ReadSize ? palSpi->WriteSize : palSpi->ReadSize,
                    //     palSpi->WriteBuffer,
                    //     palSpi->ReadBuffer);
                }
                else
                {
                    // // send operation
                    // if (busConfigIsHalfDuplex)
                    // {
                    //     // half duplex operation, set output enable
                    //     palSpi->Driver->spi->CR1 |= SPI_CR1_BIDIOE;
                    // }
                    // spiSend(palSpi->Driver, palSpi->WriteSize, palSpi->WriteBuffer);

                    // // receive operation
                    // if (busConfigIsHalfDuplex)
                    // {
                    //     // half duplex operation, set output enable
                    //     palSpi->Driver->spi->CR1 &= ~SPI_CR1_BIDIOE;
                    // }
                    // spiReceive(palSpi->Driver, palSpi->ReadSize, palSpi->ReadBuffer);
                }
            }
            else
            {
                // Transmit only or Receive only
                if (palSpi->ReadSize != 0)
                {
                    // // receive
                    // if (busConfigIsHalfDuplex)
                    // {
                    //     // half duplex operation, set output enable
                    //     palSpi->Driver->spi->CR1 &= ~SPI_CR1_BIDIOE;
                    // }

                    // spiReceive(palSpi->Driver, palSpi->ReadSize, palSpi->ReadBuffer);
                }
                else
                {
                    // send
                    if (busConfigIsHalfDuplex)
                    {
                        // half duplex operation, set output enable
                        // palSpi->Driver->spi->CR1 |= SPI_CR1_BIDIOE;
                    }

                    // spiSend(palSpi->Driver, palSpi->WriteSize, palSpi->WriteBuffer);
                }
            }

            // event occurred
            if (palSpi->ReadSize > 0)
            {
                // because this was a Read transaction, need to copy from DMA buffer to managed buffer
                int ReadSize = palSpi->ReadSize;

                // Adjust read size for data width of 16bits
                if (palSpi->BufferIs16bits)
                {
                    ReadSize *= 2;
                }
            }
        }
        else
        // Start an Asyncronous SPI transfer
        // perform SPI operation using driver's ASYNC API
        // Completed on calling Spi Callback
        {
            // this is a Async operation
            // perform SPI operation using driver's ASYNC API
            if (palSpi->WriteSize != 0 && palSpi->ReadSize != 0)
            {
                // Transmit+Receive
                if (wrc.fullDuplex)
                {
                    // Full duplex
                    // single operation, clear flag
                    palSpi->SequentialTxRx = false;

                    // // Uses the largest buffer size as transfer size
                    // spiStartExchange(
                    //     palSpi->Driver,
                    //     palSpi->WriteSize > palSpi->ReadSize ? palSpi->WriteSize : palSpi->ReadSize,
                    //     palSpi->WriteBuffer,
                    //     palSpi->ReadBuffer);
                }
                else
                {
                    // flag that an Rx is required after the Tx operation completes
                    palSpi->SequentialTxRx = true;

                    // start send operation
                    if (busConfigIsHalfDuplex)
                    {
                        // // half duplex operation, set output enable
                        // palSpi->Driver->spi->CR1 |= SPI_CR1_BIDIOE;
                    }

                    // spiStartSend(palSpi->Driver, palSpi->WriteSize, palSpi->WriteBuffer);
                    // receive operation will be started in the callback after the above completes
                }
            }
            else
            {
                // Transmit only or Receive only
                if (palSpi->ReadSize != 0)
                {
                    // single operation, clear flag
                    palSpi->SequentialTxRx = false;

                    // // start receive
                    // spiStartReceive(palSpi->Driver, palSpi->ReadSize, palSpi->ReadBuffer);
                }
                else
                {
                    // single operation, clear flag
                    palSpi->SequentialTxRx = false;

                    // // start send
                    // spiStartSend(palSpi->Driver, palSpi->WriteSize, palSpi->WriteBuffer);
                }
            }

            // Inform caller async operation started
            NANOCLR_SET_AND_LEAVE(CLR_E_BUSY);
        }
    }

    NANOCLR_NOCLEANUP();
}

bool CPU_SPI_Initialize(uint8_t busIndex, const SPI_DEVICE_CONFIGURATION &busConfiguration)
{
    (void)busConfiguration;

    SPIDRV_Init_t spiInit = {
        NULL,
#if defined(_USART_ROUTELOC0_MASK)
        0,
        0,
        0,
        0,
#elif defined(_GPIO_USART_ROUTEEN_MASK)
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
#else
        0,
#endif
        SL_SPIDRV_EXP_BITRATE,
        0,
        0,
        spidrvMaster,
        0,
        spidrvClockMode0,
        0,
        spidrvSlaveStartImmediate
    };

    // set CS control upfront
    if (busConfiguration.DeviceChipSelect >= 0)
    {
        spiInit.csControl = spidrvCsControlAuto;
    }
    else
    {
        spiInit.csControl = spidrvCsControlApplication;
    }

    // init the PAL struct for this SPI bus and assign the respective driver
    // all this occurs if not already done
    // why do we need this? because several SPIDevice objects can be created associated to the same bus
    switch (busIndex)
    {
        //////////////////////////////////
        // Gecko USART index is 0 based //
        //////////////////////////////////

#if GECKO_USE_SPI0 == TRUE
        case 0:
            if (SPI0_PAL.Handle == NULL)
            {
                SPI0_PAL.Configuration = (SPIDRV_Init_t *)platform_malloc(sizeof(SPIDRV_Init_t));

                if (SPI1_PAL.Configuration == NULL)
                {
                    return false;
                }

                // copy init struct
                memcpy(SPI0_PAL.Configuration, &spiInit, sizeof(SPIDRV_Init_t));

                ConfigPins_SPI0(busConfiguration);
            }
            break;
#endif

#if GECKO_USE_SPI1 == TRUE
        case 1:
            if (SPI1_PAL.Handle == NULL)
            {
                // allocate memory for the SPIDRV_HandleData
                SPI1_PAL.Handle = (SPIDRV_Handle_t)platform_malloc(sizeof(SPIDRV_HandleData_t));
                // sanity check allocation
                if (SPI1_PAL.Handle == NULL)
                {
                    return false;
                }

                // allocate memory for the SPIDRV_Init_t
                SPIDRV_Init_t *initSpinConfig = (SPIDRV_Init_t *)platform_malloc(sizeof(SPIDRV_Init_t));
                // sanity check allocation
                if (initSpinConfig == NULL)
                {
                    return false;
                }

                // copy init struct
                memcpy(initSpinConfig, &spiInit, sizeof(SPIDRV_Init_t));

                ConfigPins_SPI1(busConfiguration, *initSpinConfig);

                SPIDRV_Init(SPI1_PAL.Handle, initSpinConfig);
            }
            break;
#endif

#if GECKO_USE_SPI2 == TRUE
        case 2:
            if (SPI2_PAL.Handle == NULL)
            {
                SPI2_PAL.Configuration = (SPIDRV_Init_t *)platform_malloc(sizeof(SPIDRV_Init_t));

                if (SPI2_PAL.Configuration == NULL)
                {
                    return false;
                }

                // copy init struct
                memcpy(SPI2_PAL.Configuration, &spiInit, sizeof(SPIDRV_Init_t));

                ConfigPins_SPI2(busConfiguration);
            }
            break;
#endif

#if GECKO_USE_SPI3 == TRUE
        case 3:
            if (SPI3_PAL.Handle == NULL)
            {
                SPI3_PAL.Configuration = (SPIDRV_Init_t *)platform_malloc(sizeof(SPIDRV_Init_t));

                if (SPI3_PAL.Configuration == NULL)
                {
                    return false;
                }

                // copy init struct
                memcpy(SPI3_PAL.Configuration, &spiInit, sizeof(SPIDRV_Init_t));

                ConfigPins_SPI3(busConfiguration);
            }
            break;
#endif

#if GECKO_USE_SPI4 == TRUE
        case 4:
            if (SPI4_PAL.Handle == NULL)
            {
                SPI4_PAL.Configuration = (SPIDRV_Init_t *)platform_malloc(sizeof(SPIDRV_Init_t));

                if (SPI4_PAL.Configuration == NULL)
                {
                    return false;
                }

                // copy init struct
                memcpy(SPI4_PAL.Configuration, &spiInit, sizeof(SPIDRV_Init_t));

                ConfigPins_SPI4(busConfiguration);
            }
            break;
#endif

#if GECKO_USE_SPI5 == TRUE
        case 5:
            if (SPI5_PAL.Handle == NULL)
            {
                SPI5_PAL.Configuration = (SPIDRV_Init_t *)platform_malloc(sizeof(SPIDRV_Init_t));

                if (SPI5_PAL.Configuration == NULL)
                {
                    return false;
                }

                // copy init struct
                memcpy(SPI5_PAL.Configuration, &spiInit, sizeof(SPIDRV_Init_t));

                ConfigPins_SPI5(busConfiguration);
            }
            break;
#endif

        default:
            // this SPI bus is not valid
            return false;
    }

    return true;
}

bool CPU_SPI_Uninitialize(uint8_t busIndex)
{
    // get the PAL struct for the SPI bus
    switch (busIndex)
    {
        //////////////////////////////////
        // Gecko USART index is 0 based //
        //////////////////////////////////

#if GECKO_USE_SPI0 == TRUE
        case 0:
            // spiStop(&SPID0);
            SPI0_PAL.Configuration = NULL;
            // spiReleaseBus(&SPID1);
            break;
#endif

#if GECKO_USE_SPI1 == TRUE
        case 1:

            SPIDRV_DeInit(SPI1_PAL.Handle);

            // free memory
            platform_free(&SPI1_PAL.Handle->initData);
            platform_free(SPI1_PAL.Handle);

            // clears configuration
            SPI1_PAL.Handle = NULL;

            break;
#endif

#if GECKO_USE_SPI2 == TRUE
        case 2:
            // spiStop(&SPID3);
            SPI2_PAL.Configuration = NULL;
            // spiReleaseBus(&SPID3);
            break;
#endif

#if GECKO_USE_SPI3 == TRUE
        case 3:
            // spiStop(&SPID3);
            SPI3_PAL.Configuration = NULL;
            // spiReleaseBus(&SPID4);
            break;
#endif

#if GECKO_USE_SPI4 == TRUE
        case 4:
            // spiStop(&SPID5);
            SPI4_PAL.Configuration = NULL;
            // spiReleaseBus(&SPID5);
            break;
#endif

#if GECKO_USE_SPI5 == TRUE
        case 5:
            // free memory
            platform_free(SPI5_PAL.Configuration);

            // clears configuration
            SPI5_PAL.Configuration = NULL;
            break;
#endif

        default:
            // the requested SPI bus is not valid
            return false;
    }

    return true;
}

// return Map of available SPI ports
uint32_t CPU_SPI_PortsMap()
{
    uint32_t map = 0;

    //////////////////////////////////
    // Gecko USART index is 0 based //
    //////////////////////////////////

#if GECKO_USE_SPI0 == TRUE
    map |= 0x01;
#endif

#if GECKO_USE_SPI1 == TRUE
    map |= 0x02;
#endif

#if GECKO_USE_SPI2 == TRUE
    map |= 0x04;
#endif

#if GECKO_USE_SPI3 == TRUE
    map |= 0x08;
#endif

#if GECKO_USE_SPI4 == TRUE
    map |= 0x10;
#endif

#if GECKO_USE_SPI5 == TRUE
    map |= 0x20;
#endif

    return map;
}

// Add a device to SPi Bus (Optional)
// Returns a device handle.  Returns 0 if error
HRESULT CPU_SPI_Add_Device(const SPI_DEVICE_CONFIGURATION &spiDeviceConfig, uint32_t &handle)
{
    // check supported bus configuration: all valid except simplex
    if (spiDeviceConfig.BusConfiguration == SpiBusConfiguration_Simplex)
    {
        return CLR_E_NOT_SUPPORTED;
    }

    handle = (uint32_t)GetNfPalfromBusIndex(spiDeviceConfig.Spi_Bus);

    return S_OK;
}

// Return pins used for SPI bus
void CPU_SPI_GetPins(uint32_t spi_bus, GPIO_PIN &clk, GPIO_PIN &miso, GPIO_PIN &mosi)
{
    (void)spi_bus;

    clk = (GPIO_PIN)-1;
    miso = (GPIO_PIN)-1;
    mosi = (GPIO_PIN)-1;
}

// Minimum and Maximum clock frequency available based on bus and configured pins
HRESULT CPU_SPI_MinClockFrequency(uint32_t spiBus, int32_t *frequency)
{
    if (spiBus - 1 >= NUM_SPI_BUSES)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    // Max prescaler value = 256
    // SPI3 or SPI4 are on APB1, so divide max frequency by four.
    *frequency = (spiBus == 2 or spiBus == 3) ? SystemCoreClock >>= 9 : SystemCoreClock >> 8;

    return S_OK;
}

HRESULT CPU_SPI_MaxClockFrequency(uint32_t spiBus, int32_t *frequency)
{
    if (spiBus - 1 >= NUM_SPI_BUSES)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    // According to STM : "At a minimum, the clock frequency should be twice the required communication frequency."
    // So maximum useable frequency is CoreClock / 2.
    // SPI3 or SPI4 are on APB1, so divide max frequency by four.
    *frequency = (spiBus == 2 or spiBus == 3) ? SystemCoreClock >>= 2 : SystemCoreClock >> 1;

    return S_OK;
}

// Maximum number of SPI devices that can be opened on a bus
uint32_t CPU_SPI_ChipSelectLineCount(uint32_t spi_bus)
{
    (void)spi_bus;
    return 10;
}
