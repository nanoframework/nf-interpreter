//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"
#include "Esp32_DeviceMapping.h"

void postManagedStorageEvent(bool pinState, uint32_t slotIndex)
{
    PostManagedEvent(EVENT_STORAGE, StorageEventType_CardDetectChanged, pinState, slotIndex);
}

void cardDetect_interrupt(GPIO_PIN Pin, bool pinState, void *pArg)
{
    (void)Pin;

    // Decode pArg
    int slotIndex = (uint32_t)0xff && pArg;

    postManagedStorageEvent(pinState, slotIndex);
}

#if SOC_SDMMC_HOST_SUPPORTED

void GetMMCPins(int slotIndex, bool _1bit, int *count, int8_t **pPins)
{
    *pPins = &Esp32_SDMMC_DevicePinMap[slotIndex][Esp32SdmmcPin_Clock];
    *count = _1bit ? 3 : 6;
}

#endif

void UnReservePins(int count, int8_t *pPins)
{
    for (int index = count - 1; index >= 0; index--)
    {
        CPU_GPIO_ReservePin((GPIO_PIN)pPins[index], false);
    }
}

bool TryReservePins(int count, int8_t *pPins)
{
    int index = 0;
    bool failed = false;

    for (index = 0; index < count; index++)
    {
        if (CPU_GPIO_ReservePin((int8_t)pPins[index], true) == false)
        {
            failed = true;
            break;
        }
    }

    // If we failed then UnReserve pins already reserved
    if (failed)
    {
        UnReservePins(index - 1, pPins);
        return false;
    }

    return true;
}

HRESULT Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::InitNative___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    SDCard_SDInterfaceType cardType;
    int slotIndex;
    bool useCardDetect = false;
    bool cardDetectedState = false;
    int cardDetectPin;
    bool cdPinState;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    slotIndex = (pThis[FIELD___slotIndex].NumericByRef().u4);
    useCardDetect = (bool)(pThis[FIELD___enableCardDetectPin].NumericByRef().s4);
    cardDetectedState = (bool)(pThis[FIELD___cardDetectedState].NumericByRef().s4);
    cardDetectPin = (int)(pThis[FIELD___cardDetectPin].NumericByRef().s4);

    cardType = (SDCard_SDInterfaceType)pThis[FIELD___sdCardType].NumericByRef().s4;
    switch (cardType)
    {
        case SDCard_SDInterfaceType::SDCard_SDInterfaceType_Mmc:
        {
#if SOC_SDMMC_HOST_SUPPORTED

            bool bit1Mode = pThis[FIELD___dataWidth].NumericByRef().s4 == SDCard_SDDataWidth::SDCard_SDDataWidth__1_bit;

            int count;
            int8_t *pPins;

            GetMMCPins(slotIndex, bit1Mode, &count, &pPins);

            if (!TryReservePins(count, pPins))
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_PIN_UNAVAILABLE);
            }
#else
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif
            break;
        }

        case SDCard_SDInterfaceType::SDCard_SDInterfaceType_Spi:
            // TODO reserve pins ?
            break;

        // No predefined system SD card or other
        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
    }

    if (useCardDetect)
    {
        if (!CPU_GPIO_ReservePin(cardDetectPin, true))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_PIN_UNAVAILABLE);
        }

        // Set interrupt for change on Card Detect GPIO pin
        PinMode pullups = cardDetectedState ? PinMode::PinMode_InputPullDown : PinMode::PinMode_InputPullUp;

        if (!CPU_GPIO_EnableInputPin(
                cardDetectPin,
                200,
                cardDetect_interrupt,
                (void *)slotIndex,
                GPIO_INT_EDGE::GPIO_INT_EDGE_BOTH,
                pullups))
        {
            CPU_GPIO_ReservePin(cardDetectPin, false);
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // Check if we need to fire an event on start
        cdPinState = CPU_GPIO_GetPinState(cardDetectPin);
        if (cdPinState == cardDetectedState)
        {
            // Fire event for initial card detect state if card inserted
            // This is so mount can be done solely in event handler
            postManagedStorageEvent(cdPinState, slotIndex);
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::NativeDispose___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    SDCard_SDInterfaceType cardType;
    int cardDetectPin;
#if SOC_SDMMC_HOST_SUPPORTED
    int slotIndex;
#endif

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

#if SOC_SDMMC_HOST_SUPPORTED
    slotIndex = (pThis[FIELD___slotIndex].NumericByRef().u4);
#endif

    if ((bool)(pThis[FIELD___enableCardDetectPin].NumericByRef().s4))
    {
        // Stop card detect interupt
        cardDetectPin = (int)(pThis[FIELD___cardDetectPin].NumericByRef().s4);

        CPU_GPIO_DisablePin(cardDetectPin, PinMode::PinMode_Input, 0);
    }

    cardType = (SDCard_SDInterfaceType)pThis[FIELD___sdCardType].NumericByRef().s4;
    switch (cardType)
    {
        // Unreserve MMC pins. I suppose they could be used for something else
        case SDCard_SDInterfaceType::SDCard_SDInterfaceType_Mmc:
        {
#if SOC_SDMMC_HOST_SUPPORTED
            bool bit1Mode = pThis[FIELD___dataWidth].NumericByRef().s4 == SDCard_SDDataWidth::SDCard_SDDataWidth__1_bit;

            int count;
            int8_t *pPins;

            GetMMCPins(slotIndex, bit1Mode, &count, &pPins);

            UnReservePins(count, pPins);
#else
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif
            break;
        }

        default:
            break;
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::MountNative___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if (HAL_USE_SDC == TRUE)

    SDCard_SDInterfaceType cardType;
    int slotIndex;
    char mountPoint[] = INDEX0_DRIVE_LETTER;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    slotIndex = (pThis[FIELD___slotIndex].NumericByRef().u4);

    pThis[FIELD___mounted].NumericByRef().s4 = 0;

    // Get Card type so we know what parameters to use
    cardType = (SDCard_SDInterfaceType)pThis[FIELD___sdCardType].NumericByRef().s4;
    switch (cardType)
    {
        case SDCard_SDInterfaceType::SDCard_SDInterfaceType_Mmc:
        {
#if SOC_SDMMC_HOST_SUPPORTED
            if (slotIndex < 0 || slotIndex > (CONFIG_SOC_SDMMC_NUM_SLOTS - 1))
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            }

            bool bit1Mode = pThis[FIELD___dataWidth].NumericByRef().s4 == SDCard_SDDataWidth::SDCard_SDDataWidth__1_bit;

            if (!Storage_MountMMC(bit1Mode, slotIndex))
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
            }
#else
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif
            break;
        }

        case SDCard_SDInterfaceType::SDCard_SDInterfaceType_Spi:
        {
            int busIndex;
            int chipSelectPin;
            SPI_DEVICE_CONFIGURATION spiConfig;

            // Allow drive index 0-3 for SPI
            if (slotIndex < 0 || slotIndex >= 4)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            }

            // internally SPI bus ID is zero based
            busIndex = (int)(pThis[FIELD___spiBus].NumericByRef().s4 - 1);
            chipSelectPin = (int)(pThis[FIELD___chipSelectPin].NumericByRef().s4);

            // set SPI bus configuration
            spiConfig.BusConfiguration = SpiBusConfiguration_FullDuplex;

            // Try to initialised SPI bus in case it's not open, mount requires bus to be already initialised
            // Ignore errors as it may already been opened by managed code if trying to share bus
            CPU_SPI_Initialize(busIndex, spiConfig);

            // Try mount twice
            if (!Storage_MountSpi(busIndex, chipSelectPin, slotIndex))
            {
                if (!Storage_MountSpi(busIndex, chipSelectPin, slotIndex))
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
                }
            }
        }
        break;

        // No predefined system SD card or other
        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
    } // switch

    // add volume to the file system
    mountPoint[0] += slotIndex;
    FS_MountVolume(mountPoint, 0, "FATFS");

    pThis[FIELD___mounted].NumericByRef().s4 = 1;

#else

    (void)stack;
    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::UnmountNative___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if (HAL_USE_SDC == TRUE)

    SDCard_SDInterfaceType cardType;
    int slotIndex;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    slotIndex = (pThis[FIELD___slotIndex].NumericByRef().u4);

    cardType = (SDCard_SDInterfaceType)pThis[FIELD___sdCardType].NumericByRef().s4;
    switch (cardType)
    {
        case SDCard_SDInterfaceType_Mmc:
        case SDCard_SDInterfaceType_Spi:

            // Unmount SPI device
            if (!Storage_UnMountSDCard(slotIndex))
            {
                // SDcard not mounted
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
            }
            break;

        // Any other type unsupported
        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
            break;
    }

    FS_UnmountVolume(INDEX0_DRIVE_LETTER);

    pThis[FIELD___mounted].NumericByRef().s4 = 0;

#else

    (void)stack;
    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::PollCardDetectNative___BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if (HAL_USE_SDC == TRUE)

    int cardDetectPin;
    GpioPinValue detectedState;
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    if (!pThis[FIELD___enableCardDetectPin].NumericByRef().s4)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // Poll Card Detect pin
    cardDetectPin = (int)pThis[FIELD___cardDetectPin].NumericByRef().s4;
    detectedState = (bool)pThis[FIELD___cardDetectedState].NumericByRef().s4 ? GpioPinValue_High : GpioPinValue_Low;
    stack.SetResult_Boolean(CPU_GPIO_GetPinState(cardDetectPin) == detectedState);

#else

    (void)stack;
    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

#endif

    NANOCLR_NOCLEANUP();
}
