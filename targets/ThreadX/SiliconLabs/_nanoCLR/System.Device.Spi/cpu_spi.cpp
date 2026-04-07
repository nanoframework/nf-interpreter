//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_spi_native_target.h>

/////////////////////////////////////////////////////
// SPI PAL structs declared in sys_dev_spi_native.h //
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
#error "Only USART type is supported. Driver can't handle EUSART."
#endif

extern void GetIoLine(int16_t pinNumber, GPIO_Port_TypeDef *port, uint32_t *portPin);

static USART_TypeDef *GetUsartFromHandle(NF_SpiDriver_Handle_t handle)
{
    return handle->peripheral.usartPort;
}

static void CompleteTransfer(NF_SpiDriver_Handle_t handle)
{
    (void)handle;
    // SpiRelease(driver);
}

// Callback used when a async transfer operation completes
void SpiTransferCompleteCallback(NF_SpiDriver_Handle_t handle, Ecode_t transferStatus, int itemsTransferred)
{
    (void)transferStatus;
    (void)itemsTransferred;

    NATIVE_INTERRUPT_START

    NF_PAL_SPI *palSpi = NULL;

    // Find the NF_PAL_SPI* for handle
#if GECKO_USE_SPI0 == TRUE
    if (GetUsartFromHandle(handle) == USART0)
    {
        palSpi = &SPI0_PAL;
    }
#endif

#if GECKO_USE_SPI1 == TRUE
    if (GetUsartFromHandle(handle) == USART1)
    {
        palSpi = &SPI1_PAL;
    }
#endif

#if GECKO_USE_SPI2 == TRUE
    if (GetUsartFromHandle(handle) == USART2)
    {
        palSpi = &SPI2_PAL;
    }
#endif

#if GECKO_USE_SPI3 == TRUE
    if (GetUsartFromHandle(handle) == USART3)
    {
        palSpi = &SPI3_PAL;
    }
#endif

#if GECKO_USE_SPI4 == TRUE
    if (GetUsartFromHandle(handle) == USART4)
    {
        palSpi = &SPI4_PAL;
    }
#endif

#if GECKO_USE_SPI5 == TRUE
    if (GetUsartFromHandle(handle) == USART5)
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
        //     palSpi->Handle->spi->CR1 &= ~SPI_CR1_BIDIOE;
        // }
        NF_SpiDriver_MReceive(palSpi->Handle, palSpi->ReadBuffer, palSpi->ReadSize, SpiTransferCompleteCallback);
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
void GetSpiConfig(const SPI_DEVICE_CONFIGURATION &config, NF_SpiDriver_Init_t &initSpiData)
{
    // SPI mode (matches SPIDRV_ClockMode_t)
    initSpiData.bitRate = config.Clock_RateHz;

    switch (config.Spi_Mode)
    {
        case SpiMode_Mode0:
            initSpiData.clockMode = spidrvClockMode0;
            break;

        case SpiMode_Mode1:
            initSpiData.clockMode = spidrvClockMode1;
            break;

        case SpiMode_Mode2:
            initSpiData.clockMode = spidrvClockMode2;
            break;

        case SpiMode_Mode3:
            initSpiData.clockMode = spidrvClockMode3;
            break;

        default:
            ASSERT(false);
            break;
    }

#if defined(SPI_CS_CONTROL)
    initSpiData.csControl = SPI_CS_CONTROL;
#else
    initSpiData.csControl = spidrvCsControlApplication;
#endif
    initSpiData.dummyTxValue = 0;
    // Sets the order of bytes transmission : MSB first or LSB first
    initSpiData.bitOrder = config.DataOrder16 == DataBitOrder_MSB ? spidrvBitOrderMsbFirst : spidrvBitOrderLsbFirst;
    initSpiData.frameLength = config.DataIs16bits ? 16 : 8;
    initSpiData.isHalfDuplex = config.BusConfiguration == SpiBusConfiguration_HalfDuplex ? true : false;
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

        // adjust the bus index to match the PAL struct
        NF_SpiDriver_SetFramelength(palSpi->Handle, wrc.Bits16ReadWrite ? 16 : 8);

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

        // if CS is to be controlled by the driver, set the GPIO
        if (palSpi->ChipSelect >= 0)
        {
            // assert pin based on CS active level
            CPU_GPIO_SetPinState(palSpi->ChipSelect, (GpioPinValue)sdev.ChipSelectActiveState);
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
                    NF_SpiDriver_MTransferB(
                        palSpi->Handle,
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
                    //     palSpi->Handle->spi->CR1 |= SPI_CR1_BIDIOE;
                    // }
                    NF_SpiDriver_MTransmitB(palSpi->Handle, palSpi->WriteBuffer, palSpi->WriteSize);

                    // receive operation
                    // TODO
                    // if (busConfigIsHalfDuplex)
                    // {
                    //     // half duplex operation, set output enable
                    //     palSpi->Handle->spi->CR1 &= ~SPI_CR1_BIDIOE;
                    // }
                    NF_SpiDriver_MReceiveB(palSpi->Handle, palSpi->ReadBuffer, palSpi->ReadSize);
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
                    //     palSpi->Handle->spi->CR1 &= ~SPI_CR1_BIDIOE;
                    // }
                    NF_SpiDriver_MReceiveB(palSpi->Handle, palSpi->ReadBuffer, palSpi->ReadSize);
                }
                else
                {
                    // send
                    // TODO
                    if (busConfigIsHalfDuplex)
                    {
                        // half duplex operation, set output enable
                        // palSpi->Handle->spi->CR1 |= SPI_CR1_BIDIOE;
                    }
                    NF_SpiDriver_MTransmitB(palSpi->Handle, palSpi->WriteBuffer, palSpi->WriteSize);
                }
            }

            CompleteTransfer(palSpi->Handle);

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
                CPU_GPIO_SetPinState(palSpi->ChipSelect, (GpioPinValue)sdev.ChipSelectActiveState);
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
                    NF_SpiDriver_MTransfer(
                        palSpi->Handle,
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
                        // palSpi->Handle->spi->CR1 |= SPI_CR1_BIDIOE;
                    }

                    // receive operation will be started in the callback after the above completes
                    NF_SpiDriver_MTransmit(
                        palSpi->Handle,
                        palSpi->WriteBuffer,
                        palSpi->WriteSize,
                        SpiTransferCompleteCallback);
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
                    NF_SpiDriver_MReceive(
                        palSpi->Handle,
                        palSpi->ReadBuffer,
                        palSpi->ReadSize,
                        SpiTransferCompleteCallback);
                }
                else
                {
                    // single operation, clear flag
                    palSpi->SequentialTxRx = false;

                    // start send
                    NF_SpiDriver_MTransmit(
                        palSpi->Handle,
                        palSpi->WriteBuffer,
                        palSpi->WriteSize,
                        SpiTransferCompleteCallback);
                }
            }

            // Inform caller async operation started
            NANOCLR_SET_AND_LEAVE(CLR_E_BUSY);
        }
    }

    NANOCLR_NOCLEANUP();
}

// this is exposing the extended call that allow for re-configuration of SPI
bool CPU_SPI_Initialize_Extended(uint8_t busIndex, const SPI_DEVICE_CONFIGURATION &busConfiguration, bool reconfigure)
{
#if !defined(BUILD_RTM)
    Ecode_t configResult;
#endif

    GPIO_Port_TypeDef port;
    uint32_t portPin;
    NF_PAL_SPI *palSpi = NULL;
    void (*initSpiConfig)(NF_SpiDriver_Init_t &, bool) = NULL;

    // init the PAL struct for this SPI bus and assign the respective driver
    // all this occurs if not already done
    // why do we need this? because several SPIDevice objects can be created associated to the same bus
    switch (busIndex)
    {
#if GECKO_USE_SPI0 == TRUE
        case 0:
            palSpi = &SPI0_PAL;
            initSpiConfig = &InitSpiConfig0;
            break;
#endif

#if GECKO_USE_SPI1 == TRUE
        case 1:
            palSpi = &SPI1_PAL;
            initSpiConfig = &InitSpiConfig1;
            break;
#endif

#if GECKO_USE_SPI2 == TRUE
        case 2:
            palSpi = &SPI2_PAL;
            initSpiConfig = &InitSpiConfig2;

            break;
#endif

#if GECKO_USE_SPI3 == TRUE
        case 3:
            palSpi = &SPI3_PAL;
            initSpiConfig = &InitSpiConfig3;
            break;
#endif

#if GECKO_USE_SPI4 == TRUE
        case 4:
            palSpi = &SPI4_PAL;
            initSpiConfig = &InitSpiConfig4;
            break;
#endif

#if GECKO_USE_SPI5 == TRUE
        case 5:
            palSpi = &SPI5_PAL;
            initSpiConfig = &InitSpiConfig5;
            break;
#endif

        default:
            // this SPI bus is not valid
            return false;
    }

    if (palSpi->Handle == NULL)
    {
        // allocate memory for the USART_InitSync_TypeDef
        palSpi->Handle = (NF_SpiDriver_Handle_t)platform_malloc(sizeof(NF_SpiDriver_HandleData_t));

        // sanity check allocation
        if (palSpi->Handle == NULL)
        {
            return false;
        }

        memset(palSpi->Handle, 0, sizeof(NF_SpiDriver_HandleData_t));

        // allocate memory for the NF_SpiDriver_Init_t
        palSpi->InitSpiData = (NF_SpiDriver_Init_t *)platform_malloc(sizeof(NF_SpiDriver_Init_t));

        // sanity check allocation
        if (palSpi->InitSpiData == NULL)
        {
            platform_free(palSpi->Handle);

            return false;
        }

        memset(palSpi->InitSpiData, 0, sizeof(NF_SpiDriver_Init_t));

        // call handler to configure pins
        initSpiConfig(*palSpi->InitSpiData, busConfiguration.BusConfiguration == SpiBusConfiguration_HalfDuplex);

    jump_to_init:

        // get the SPI configuration
        GetSpiConfig(busConfiguration, *palSpi->InitSpiData);

#if !defined(BUILD_RTM)
        configResult =
#endif
            NF_SpiDriver_Init(palSpi->Handle, palSpi->InitSpiData);
        _ASSERTE(configResult == ECODE_OK);

        palSpi->ChipSelect = busConfiguration.DeviceChipSelect;

        // setup GPIO for CS
        if (busConfiguration.DeviceChipSelect >= 0)
        {
            GetIoLine(busConfiguration.DeviceChipSelect, &port, &portPin);
            GPIO_PinModeSet(port, portPin, gpioModePushPull, busConfiguration.ChipSelectActiveState ? 0 : 1);
        }
    }
    else
    {
        // there's already a handle, check if we need to re-configure the SPI bus
        if (reconfigure)
        {
            // deinitalize the SPI bus
            NF_SpiDriver_DeInit(palSpi->Handle);

            // jump straight to init
            goto jump_to_init;
        }
    }

    return true;
}

// this is exposing the "standard" call
bool CPU_SPI_Initialize(uint8_t busIndex, const SPI_DEVICE_CONFIGURATION &busConfiguration)
{
    return CPU_SPI_Initialize_Extended(busIndex, busConfiguration, false);
}

bool CPU_SPI_Uninitialize(uint8_t busIndex)
{
    NF_PAL_SPI *palSpi = NULL;
    GPIO_Port_TypeDef port;
    uint32_t portPin;

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
            return false;
    }

    // reset GPIO for CS
    if (palSpi->ChipSelect >= 0)
    {
        GetIoLine(palSpi->ChipSelect, &port, &portPin);
        GPIO_PinModeSet(port, portPin, gpioModeInputPull, 0);
    }

    NF_SpiDriver_DeInit(palSpi->Handle);

    // free memory
    platform_free(palSpi->Handle->initData);
    platform_free(palSpi->Handle);

    palSpi->Handle = NULL;

    return true;
}

// return Map of available SPI ports
uint32_t CPU_SPI_PortsMap()
{
    uint32_t map = 0;

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
