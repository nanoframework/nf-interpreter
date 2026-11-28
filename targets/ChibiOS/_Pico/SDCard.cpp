//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"

#include <hal.h>

#include <nanoHAL.h>
#include <target_storage_config.h>
#include <sys_dev_spi_native_target.h>

NF_PAL_SPI *GetNfPalfromBusIndex(uint8_t busIndex);

extern "C"
{
    MMCDriver SD_CARD_DRIVER;
}

static uint8_t sdCardBuffer[MMC_BUFFER_SIZE];
static SPIConfig sdCardLowSpeedConfig;
static SPIConfig sdCardHighSpeedConfig;
static MMCConfig sdCardConfig;
static uint32_t sdCardSpiHandle;
static int32_t sdCardBus;
static int32_t sdCardChipSelectPin;
static GPIO_PIN sdCardDetectPin = GPIO_PIN_NONE;
static bool sdCardMounted;

typedef Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard SDCardNative;

static void CardDetectInterrupt(GPIO_PIN pin, bool pinState, void *argument)
{
    (void)pin;
    (void)argument;
    PostManagedEvent(EVENT_STORAGE, StorageEventType_CardDetectChanged, pinState, 0);
}

static HRESULT CheckCard(CLR_RT_HeapBlock *pThis)
{
    if (pThis[SDCardNative::FIELD___disposed].NumericByRef().u1 != 0)
    {
        return CLR_E_OBJECT_DISPOSED;
    }

    if (sdCardSpiHandle == 0 || pThis[SDCardNative::FIELD___spiBus].NumericByRef().s4 != sdCardBus ||
        pThis[SDCardNative::FIELD___chipSelectPin].NumericByRef().s4 != sdCardChipSelectPin)
    {
        return CLR_E_INVALID_OPERATION;
    }

    return S_OK;
}

extern "C" void SdCardWaitSpi(void)
{
    // A pending managed async transfer may still own the shared bus on this CLR thread.
    nanoSPI_Wait_Busy(sdCardSpiHandle);
}

static HRESULT ReleaseCard()
{
    if (sdCardSpiHandle == 0)
    {
        return S_OK;
    }

    if (sdCardMounted)
    {
        FS_UnmountVolume(INDEX0_DRIVE_LETTER);
        sdCardMounted = false;
    }

    SPIDriver *driver = sdCardConfig.spip;
    SdCardWaitSpi();
    spiAcquireBus(driver);
    bool failed = mmcDisconnect(&SD_CARD_DRIVER);
    mmcStop(&SD_CARD_DRIVER);
    spiReleaseBus(driver);

    HRESULT hr = nanoSPI_CloseDevice(sdCardSpiHandle);
    sdCardSpiHandle = 0;

    if (sdCardDetectPin != GPIO_PIN_NONE)
    {
        CPU_GPIO_DisablePin(sdCardDetectPin, PinMode_Input, 0);
        CPU_GPIO_ReservePin(sdCardDetectPin, false);
        sdCardDetectPin = GPIO_PIN_NONE;
    }

    return FAILED(hr) ? hr : (failed ? CLR_E_IO : S_OK);
}

extern "C" void SdCardUninitialize(void)
{
    HRESULT hr = ReleaseCard();
    if (FAILED(hr))
    {
        debug_printf("SD card cleanup failed: 0x%08x\r\n", (unsigned int)hr);
    }
}

HRESULT Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::InitNative___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    SPI_DEVICE_CONFIGURATION config = {};
    GPIO_PIN clk, miso, mosi;
    int32_t bus;
    int32_t chipSelect;
    int32_t detectPin;
    bool useDetect;

    FAULT_ON_NULL(pThis);
    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // A failed constructor must not let its finalizer release another card's resources.
    pThis[FIELD___disposed].NumericByRef().u1 = 1;

    if (pThis[FIELD___sdCardType].NumericByRef().s4 != SDCard_SDInterfaceType_Spi)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }

    bus = pThis[FIELD___spiBus].NumericByRef().s4;
    chipSelect = pThis[FIELD___chipSelectPin].NumericByRef().s4;
    detectPin = pThis[FIELD___cardDetectPin].NumericByRef().s4;
    useDetect = pThis[FIELD___enableCardDetectPin].NumericByRef().s4 != 0;

    if (pThis[FIELD___slotIndex].NumericByRef().s4 != 0 || bus < 1 || bus > NUM_SPI_BUSES ||
        !(CPU_SPI_PortsMap() & (1U << (bus - 1))) || chipSelect < 0 || chipSelect >= 30 ||
        (useDetect && (detectPin < 0 || detectPin >= 30 || detectPin == chipSelect)))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (sdCardSpiHandle != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

#if defined(CONFIG_TARGET_HAS_WIFI)
    if ((chipSelect >= 23 && chipSelect <= 25) || chipSelect == 29 ||
        (useDetect && ((detectPin >= 23 && detectPin <= 25) || detectPin == 29)))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_PIN_UNAVAILABLE);
    }
#endif

    CPU_SPI_GetPins(bus - 1, clk, miso, mosi);
    if ((GPIO_PIN)chipSelect == clk || (GPIO_PIN)chipSelect == miso || (GPIO_PIN)chipSelect == mosi ||
        (useDetect && ((GPIO_PIN)detectPin == clk || (GPIO_PIN)detectPin == miso || (GPIO_PIN)detectPin == mosi)))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (CPU_GPIO_PinIsBusy(chipSelect) || (useDetect && CPU_GPIO_PinIsBusy(detectPin)))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_PIN_UNAVAILABLE);
    }

    if (GetNfPalfromBusIndex(bus - 1)->Driver == NULL &&
        (CPU_GPIO_PinIsBusy(clk) || CPU_GPIO_PinIsBusy(miso) || CPU_GPIO_PinIsBusy(mosi)))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_PIN_UNAVAILABLE);
    }

    config.Spi_Bus = bus - 1;
    config.BusMode = SpiBusMode_master;
    config.DeviceChipSelect = chipSelect;
    config.Clock_RateHz = 400000;
    config.Spi_Mode = SpiMode_Mode0;
    config.BusConfiguration = SpiBusConfiguration_FullDuplex;
    NANOCLR_CHECK_HRESULT(nanoSPI_OpenDevice(config, sdCardSpiHandle));

    sdCardBus = bus;
    sdCardChipSelectPin = chipSelect;
    sdCardMounted = false;
    sdCardLowSpeedConfig = {};
    sdCardLowSpeedConfig.ssline = PAL_LINE(IOPORT1, chipSelect);
    // Maximum divider keeps initialization below 400 kHz on both Pico families.
    sdCardLowSpeedConfig.SSPCR0 = 7U | (255U << 8);
    sdCardLowSpeedConfig.SSPCPSR = 2U;
    sdCardHighSpeedConfig = sdCardLowSpeedConfig;
    sdCardHighSpeedConfig.SSPCR0 = 7U;
    sdCardHighSpeedConfig.SSPCPSR = ((RP_PERI_CLK + 19999999U) / 20000000U + 1U) & ~1U;
    sdCardConfig.lscfg = &sdCardLowSpeedConfig;
    sdCardConfig.hscfg = &sdCardHighSpeedConfig;
    sdCardConfig.spip = NULL;
#if RP_SPI_USE_SPI0
    if (bus == 1)
    {
        sdCardConfig.spip = &SPID0;
    }
#endif
#if RP_SPI_USE_SPI1
    if (bus == 2)
    {
        sdCardConfig.spip = &SPID1;
    }
#endif

    mmcObjectInit(&SD_CARD_DRIVER, sdCardBuffer);
    mmcStart(&SD_CARD_DRIVER, &sdCardConfig);

    if (useDetect)
    {
        if (!CPU_GPIO_ReservePin(detectPin, true))
        {
            SdCardUninitialize();
            NANOCLR_SET_AND_LEAVE(CLR_E_PIN_UNAVAILABLE);
        }
        sdCardDetectPin = detectPin;

        bool detectedState = pThis[FIELD___cardDetectedState].NumericByRef().s4 != 0;
        if (!CPU_GPIO_EnableInputPin(
                detectPin,
                200,
                CardDetectInterrupt,
                NULL,
                GPIO_INT_EDGE_BOTH,
                detectedState ? PinMode_InputPullDown : PinMode_InputPullUp))
        {
            SdCardUninitialize();
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        bool pinState = CPU_GPIO_GetPinState(detectPin) == GpioPinValue_High;
        if (pinState == detectedState)
        {
            CardDetectInterrupt(detectPin, pinState, NULL);
        }
    }

    pThis[FIELD___disposed].NumericByRef().u1 = 0;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::NativeDispose___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);
    NANOCLR_CHECK_HRESULT(CheckCard(pThis));
    pThis[FIELD___mounted].NumericByRef().s4 = 0;
    hr = ReleaseCard();
    pThis[FIELD___disposed].NumericByRef().u1 = 1;
    NANOCLR_CHECK_HRESULT(hr);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::MountNative___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    bool failed;
    FAULT_ON_NULL(pThis);
    NANOCLR_CHECK_HRESULT(CheckCard(pThis));

    if (sdCardMounted)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    SdCardWaitSpi();
    spiAcquireBus(sdCardConfig.spip);
    failed = mmcConnect(&SD_CARD_DRIVER);
    spiReleaseBus(sdCardConfig.spip);
    if (failed)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
    }

    if (!FS_MountVolume(INDEX0_DRIVE_LETTER, 0, "FATFS"))
    {
        spiAcquireBus(sdCardConfig.spip);
        failed = mmcDisconnect(&SD_CARD_DRIVER);
        spiReleaseBus(sdCardConfig.spip);
        NANOCLR_SET_AND_LEAVE(failed ? CLR_E_IO : CLR_E_VOLUME_NOT_FOUND);
    }

    sdCardMounted = true;
    pThis[FIELD___mounted].NumericByRef().s4 = 1;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::UnmountNative___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    bool failed;
    FAULT_ON_NULL(pThis);
    NANOCLR_CHECK_HRESULT(CheckCard(pThis));
    if (!sdCardMounted)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    FS_UnmountVolume(INDEX0_DRIVE_LETTER);
    sdCardMounted = false;
    pThis[FIELD___mounted].NumericByRef().s4 = 0;
    SdCardWaitSpi();
    spiAcquireBus(sdCardConfig.spip);
    failed = mmcDisconnect(&SD_CARD_DRIVER);
    spiReleaseBus(sdCardConfig.spip);
    if (failed)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_IO);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::PollCardDetectNative___BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);
    NANOCLR_CHECK_HRESULT(CheckCard(pThis));
    if (sdCardDetectPin == GPIO_PIN_NONE)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    stack.SetResult_Boolean(
        (CPU_GPIO_GetPinState(sdCardDetectPin) == GpioPinValue_High) ==
        (pThis[FIELD___cardDetectedState].NumericByRef().s4 != 0));

    NANOCLR_NOCLEANUP();
}
