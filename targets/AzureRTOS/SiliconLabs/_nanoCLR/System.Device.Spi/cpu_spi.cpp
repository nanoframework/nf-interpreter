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

#if defined(EUSART_PRESENT)
#error "Only USART type is supported. Drivar can't handle EUSART."
#endif

static USART_TypeDef *GetUsartFromHandle(struct Gecko_SpiDriver *handle)
{
    return handle->Usart;
}

// Callback used when a async transfer operation completes
static void SpiTransferCompleteCallback(struct Gecko_SpiDriver *driver, Ecode_t transferStatus, int itemsTransferred)
{
    (void)transferStatus;
    (void)itemsTransferred;

    NATIVE_INTERRUPT_START

    NF_PAL_SPI *palSpi = NULL;

    // Find the NF_PAL_SPI* for driver
#if GECKO_USE_SPI0 == TRUE
    if (GetUsartFromHandle(driver) == USART0)
    {
        palSpi = &SPI0_PAL;
    }
#endif

#if GECKO_USE_SPI1 == TRUE
    if (GetUsartFromHandle(driver) == USART1)
    {
        palSpi = &SPI1_PAL;
    }
#endif

#if GECKO_USE_SPI2 == TRUE
    if (GetUsartFromHandle(driver) == USART2)
    {
        palSpi = &SPI2_PAL;
    }
#endif

#if GECKO_USE_SPI3 == TRUE
    if (GetUsartFromHandle(driver) == USART3)
    {
        palSpi = &SPI3_PAL;
    }
#endif

#if GECKO_USE_SPI4 == TRUE
    if (GetUsartFromHandle(driver) == USART4)
    {
        palSpi = &SPI4_PAL;
    }
#endif

#if GECKO_USE_SPI5 == TRUE
    if (GetUsartFromHandle(driver) == USART5)
    {
        palSpi = &SPI5_PAL;
    }
#endif

    // check if there is any Rx operation due
    if (palSpi->SequentialTxRx)
    {
        // yes there is!
        // clear flag and...
        palSpi->SequentialTxRx = false;

        // ... start it
        // TODO
        // if (palSpi->busConfiguration)
        // {
        //     // half duplex operation, clear output enable bit
        //     palSpi->Driver->spi->CR1 &= ~SPI_CR1_BIDIOE;
        // }
        SpiReceive(palSpi->Driver, palSpi->ReadBuffer, palSpi->ReadSize, SpiTransferCompleteCallback);
    }
    else
    {
        // all done here!

        // if CS is to be controlled by the driver, set the GPIO
        if (palSpi->ChipSelect >= 0)
        {
            // de-assert pin based on CS active level
            CPU_GPIO_TogglePinState(palSpi->ChipSelect);
        }

        // fire callback for SPI transaction complete
        // only if callback set
        if (palSpi->Callback)
        {
            palSpi->Callback(palSpi->BusIndex);
        }
    }

    NATIVE_INTERRUPT_END
};

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
void GetSPIConfig(const SPI_DEVICE_CONFIGURATION &config, USART_InitSync_TypeDef *uartInit)
{
    // SPI mode (matches SPIDRV_ClockMode_t)
    uartInit->baudrate = config.Clock_RateHz;

    switch (config.Spi_Mode)
    {
        case SpiMode_Mode0:
            uartInit->clockMode = usartClockMode0;
            break;

        case SpiMode_Mode1:
            uartInit->clockMode = usartClockMode1;
            break;

        case SpiMode_Mode2:
            uartInit->clockMode = usartClockMode2;
            break;

        case SpiMode_Mode3:
            uartInit->clockMode = usartClockMode3;
            break;

        default:
            ASSERT(false);
            break;
    }

    // Sets the order of bytes transmission : MSB first or LSB first
    uartInit->msbf = config.DataOrder16 == DataBitOrder_MSB;
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

        palSpi->Driver->DataIs16bits = wrc.Bits16ReadWrite;

        // set bus config flag
        busConfigIsHalfDuplex = (palSpi->BusConfiguration == SpiBusConfiguration_HalfDuplex);

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

        if (!SpiDmaStart(palSpi->Driver))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_TOO_MANY_OPEN_HANDLES);
        }

        // if CS is to be controlled by the driver, set the GPIO
        if (palSpi->ChipSelect >= 0)
        {
            // assert pin based on CS active level
            CPU_GPIO_SetPinState(GPIO_PORT(palSpi->ChipSelect), (GpioPinValue)sdev.ChipSelectActive);
        }

        if (sync)
        {
            // Sync operation
            // perform SPI operation using driver's SYNC API
            if (palSpi->WriteSize != 0 && palSpi->ReadSize != 0)
            {
                // Transmit+Receive
                if (wrc.fullDuplex)
                {
                    // Full duplex
                    // Uses the largest buffer size as transfer size
                    SpiTransferBlocking(
                        palSpi->Driver,
                        palSpi->WriteBuffer,
                        palSpi->ReadBuffer,
                        palSpi->WriteSize > palSpi->ReadSize ? palSpi->WriteSize : palSpi->ReadSize);
                }
                else
                {
                    // send operation
                    // TODO
                    // if (busConfigIsHalfDuplex)
                    // {
                    //     // half duplex operation, set output enable
                    //     palSpi->Driver->spi->CR1 |= SPI_CR1_BIDIOE;
                    // }
                    SpiTransmitBlocking(palSpi->Driver, palSpi->WriteBuffer, palSpi->WriteSize);

                    // receive operation
                    // TODO
                    // if (busConfigIsHalfDuplex)
                    // {
                    //     // half duplex operation, set output enable
                    //     palSpi->Driver->spi->CR1 &= ~SPI_CR1_BIDIOE;
                    // }
                    SpiReceiveBlocking(palSpi->Driver, palSpi->ReadBuffer, palSpi->ReadSize);
                }
            }
            else
            {
                // Transmit only or Receive only
                if (palSpi->ReadSize != 0)
                {
                    // receive
                    // TODO
                    // if (busConfigIsHalfDuplex)
                    // {
                    //     // half duplex operation, set output enable
                    //     palSpi->Driver->spi->CR1 &= ~SPI_CR1_BIDIOE;
                    // }
                    SpiReceiveBlocking(palSpi->Driver, palSpi->ReadBuffer, palSpi->ReadSize);
                }
                else
                {
                    // send
                    // TODO
                    if (busConfigIsHalfDuplex)
                    {
                        // half duplex operation, set output enable
                        // palSpi->Driver->spi->CR1 |= SPI_CR1_BIDIOE;
                    }
                    SpiTransmitBlocking(palSpi->Driver, palSpi->WriteBuffer, palSpi->WriteSize);
                }
            }

            // if CS is to be controlled by the driver, set the GPIO
            if (palSpi->ChipSelect >= 0)
            {
                // de-assert pin based on CS active level
                CPU_GPIO_TogglePinState(palSpi->ChipSelect);
            }
        }
        else
        {
            // Start an Asyncronous SPI transfer
            // perform SPI operation using driver's ASYNC API
            // Completed on calling SPI Callback

            // if CS is to be controlled by the driver, set the GPIO
            if (palSpi->ChipSelect >= 0)
            {
                // assert pin based on CS active level
                CPU_GPIO_SetPinState(palSpi->ChipSelect, (GpioPinValue)sdev.ChipSelectActive);
            }

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

                    // Uses the largest buffer size as transfer size
                    SpiTransfer(
                        palSpi->Driver,
                        palSpi->WriteBuffer,
                        palSpi->ReadBuffer,
                        palSpi->WriteSize > palSpi->ReadSize ? palSpi->WriteSize : palSpi->ReadSize,
                        SpiTransferCompleteCallback);
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

                    // receive operation will be started in the callback after the above completes
                    SpiTransmit(palSpi->Driver, palSpi->WriteBuffer, palSpi->WriteSize, SpiTransferCompleteCallback);
                }
            }
            else
            {
                // Transmit only or Receive only
                if (palSpi->ReadSize != 0)
                {
                    // single operation, clear flag
                    palSpi->SequentialTxRx = false;

                    // start receive
                    SpiReceive(palSpi->Driver, palSpi->ReadBuffer, palSpi->ReadSize, SpiTransferCompleteCallback);
                }
                else
                {
                    // single operation, clear flag
                    palSpi->SequentialTxRx = false;

                    // start send
                    SpiTransmit(palSpi->Driver, palSpi->WriteBuffer, palSpi->WriteSize, SpiTransferCompleteCallback);
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
    NF_PAL_SPI *palSpi = NULL;
    USART_TypeDef *usart = NULL;
    void (*configPinsHandler)(const struct SPI_DEVICE_CONFIGURATION &) = NULL;

    // create USART init struct with the defaults:
    // ports and pins with "invalid" values
    // always MASTER mode
    // we'll be controlling CS programmatically
    USART_InitSync_TypeDef configInit = {
        usartDisable,
        0,
        1000000,
        usartDatabits8,
        true,
        false,
        usartClockMode0,
        false,
        0,
        false,
        false,
        false,
#if (_SILICON_LABS_32B_SERIES > 0)
        0,
        0
#endif
    };

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
            palSpi = &SPI0_PAL;
            usart = USART0;
            configPinsHandler = &ConfigPins_SPI0;
            break;
#endif

#if GECKO_USE_SPI1 == TRUE
        case 1:
            palSpi = &SPI1_PAL;
            usart = USART1;
            configPinsHandler = &ConfigPins_SPI1;
            break;
#endif

#if GECKO_USE_SPI2 == TRUE
        case 2:
            palSpi = &SPI2_PAL;
            usart = USART2;
            configPinsHandler = &ConfigPins_SPI2;

            break;
#endif

#if GECKO_USE_SPI3 == TRUE
        case 3:
            palSpi = &SPI3_PAL;
            usart = USART3;
            configPinsHandler = &ConfigPins_SPI3;
            break;
#endif

#if GECKO_USE_SPI4 == TRUE
        case 4:
            palSpi = &SPI4_PAL;
            usart = USART4;
            configPinsHandler = &ConfigPins_SPI4;
            break;
#endif

#if GECKO_USE_SPI5 == TRUE
        case 5:
            palSpi = &SPI5_PAL;
            usart = USART5;
            configPinsHandler = &ConfigPins_SPI5;
            break;
#endif

        default:
            // this SPI bus is not valid
            return false;
    }

    if (palSpi->Driver == NULL)
    {

        // allocate memory for the USART_InitSync_TypeDef
        palSpi->Driver = (Gecko_SpiDriver *)platform_malloc(sizeof(Gecko_SpiDriver));

        // sanity check allocation
        if (palSpi->Driver == NULL)
        {
            return false;
        }

        // allocate memory for the USART_InitSync_TypeDef
        palSpi->Driver->Configuration = (USART_InitSync_TypeDef *)platform_malloc(sizeof(USART_InitSync_TypeDef));

        // sanity check allocation
        if (palSpi->Driver->Configuration == NULL)
        {
            platform_free(palSpi->Driver);

            return false;
        }

        // copy init struct
        memcpy(palSpi->Driver->Configuration, &configInit, sizeof(USART_InitSync_TypeDef));

        // set USART
        palSpi->Driver->Usart = usart;

        // get the SPI configuration
        GetSPIConfig(busConfiguration, palSpi->Driver->Configuration);

        SpiDriverInit(palSpi->Driver);

        palSpi->ChipSelect = busConfiguration.DeviceChipSelect;

        // call handler to configure pins
        configPinsHandler(busConfiguration);

        // init DMA driver (don't bother check return value as if it's already started it won't fail)v
        DMADRV_Init();
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
            USART_Reset(UART0);

            // free memory
            platform_free(SPI0_PAL.Driver->Configuration);
            platform_free(SPI0_PAL.Driver);

            SPI0_PAL.Driver = NULL;

            break;
#endif

#if GECKO_USE_SPI1 == TRUE
        case 1:
            USART_Reset(UART1);

            // free memory
            platform_free(SPI1_PAL.Driver->Configuration);
            platform_free(SPI1_PAL.Driver);

            SPI1_PAL.Driver = NULL;

            break;
#endif

#if GECKO_USE_SPI2 == TRUE
        case 2:
            USART_Reset(UART2);

            // free memory
            platform_free(SPI2_PAL.Driver->Configuration);
            platform_free(SPI2_PAL.Driver);

            SPI2_PAL.Driver = NULL;

            break;
#endif

#if GECKO_USE_SPI3 == TRUE
        case 3:
            USART_Reset(UART3);

            // free memory
            platform_free(SPI3_PAL.Driver->Configuration);
            platform_free(SPI3_PAL.Driver);

            SPI3_PAL.Driver = NULL;

            break;
#endif

#if GECKO_USE_SPI4 == TRUE
        case 4:
            USART_Reset(UART4);

            // free memory
            platform_free(SPI4_PAL.Driver->Configuration);
            platform_free(SPI4_PAL.Driver);

            SPI4_PAL.Driver = NULL;

            break;
#endif

#if GECKO_USE_SPI5 == TRUE
        case 5:
            USART_Reset(UART5);

            // free memory
            platform_free(SPI5_PAL.Driver->Configuration);
            platform_free(SPI5_PAL.Driver);

            SPI5_PAL.Driver = NULL;

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
