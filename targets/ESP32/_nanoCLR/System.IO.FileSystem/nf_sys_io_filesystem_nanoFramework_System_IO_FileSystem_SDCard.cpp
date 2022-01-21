//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"

StorageEventType GetStorageEventType(bool pinState)
{
    // Low means card inserted
    return pinState ? StorageEventType_RemovableDeviceRemoval : StorageEventType_RemovableDeviceInsertion;
}

void postManagedStorageEvent(bool pinState, uint32_t driveIndex)
{
    PostManagedEvent(EVENT_STORAGE, 0, GetStorageEventType(pinState), driveIndex);
}

void cardDetect_interrupt(GPIO_PIN Pin, bool pinState, void *pArg)
{
    (void)Pin;

    postManagedStorageEvent(pinState, (uint32_t)pArg);
}

// Reserve all MMC pins
// CMD, Data0, Data1, Data2, Data3
int8_t pins4bit[] = {15, 2, 4, 12, 13};
// CMD, Data0
int8_t pins1bit[] = {15, 2};

void GetMMCPins(bool _1bit, int *count, int8_t **pPins)
{
    if (_1bit)
    {
        *pPins = pins1bit;
        *count = sizeof(pins1bit);
    }
    else
    {
        *pPins = pins4bit;
        *count = sizeof(pins4bit);
    }
}

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
    bool useCardDetect = false;
    int cardDetectPin;
    bool cardDetectpinState;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    useCardDetect = (bool)(pThis[FIELD___enableCardDetectPin].NumericByRef().s4);
    cardDetectPin = (int)(pThis[FIELD___cardDetectPin].NumericByRef().s4);

    cardType = (SDCard_SDInterfaceType)pThis[FIELD___sdCardType].NumericByRef().s4;
    switch (cardType)
    {
        case SDCard_SDInterfaceType::SDCard_SDInterfaceType_Mmc:
        {
            bool bit1Mode = (bool)(pThis[FIELD___dataWidth].NumericByRef().s4);

            int count;
            int8_t *pPins;

            GetMMCPins(bit1Mode, &count, &pPins);

            if (!TryReservePins(count, pPins))
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_PIN_UNAVAILABLE);
            }
        }
        break;

        case SDCard_SDInterfaceType::SDCard_SDInterfaceType_Spi:
            // TODO reserve pins ?
            //  or init SPI
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
        if (!CPU_GPIO_EnableInputPin(
                cardDetectPin,
                100,
                cardDetect_interrupt,
                0,
                GPIO_INT_EDGE::GPIO_INT_EDGE_BOTH,
                GpioPinDriveMode::GpioPinDriveMode_InputPullUp))
        {
            CPU_GPIO_ReservePin(cardDetectPin, false);
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        cardDetectpinState = CPU_GPIO_GetPinState(cardDetectPin);
        if (GetStorageEventType(cardDetectpinState) == StorageEventType_RemovableDeviceInsertion)
        {
            // Fire event for initial card detect state if card inserted
            // This is so mount can be done solely in event handler
            postManagedStorageEvent(cardDetectpinState, 0);
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

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if ((bool)(pThis[FIELD___enableCardDetectPin].NumericByRef().s4))
    {
        // Stop card detect interupt
        cardDetectPin = (int)(pThis[FIELD___cardDetectPin].NumericByRef().s4);

        CPU_GPIO_DisablePin(cardDetectPin, GpioPinDriveMode::GpioPinDriveMode_Input, 0);
    }

    cardType = (SDCard_SDInterfaceType)pThis[FIELD___sdCardType].NumericByRef().s4;
    switch (cardType)
    {
        // Unreserve MMC pins. I suppose they could be used for something else
        case SDCard_SDInterfaceType::SDCard_SDInterfaceType_Mmc:
        {
            bool bit1Mode = (bool)(pThis[FIELD___dataWidth].NumericByRef().s4);
            int count;
            int8_t *pPins;

            GetMMCPins(bit1Mode, &count, &pPins);

            UnReservePins(count, pPins);
        }
        break;

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

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    pThis[FIELD___mounted].NumericByRef().s4 = 0;

    // Get Card type so we know what parameters to use
    cardType = (SDCard_SDInterfaceType)pThis[FIELD___sdCardType].NumericByRef().s4;
    switch (cardType)
    {
        case SDCard_SDInterfaceType::SDCard_SDInterfaceType_Mmc:
        {
            bool bit1Mode = (bool)(pThis[FIELD___dataWidth].NumericByRef().s4);

            if (!Storage_MountMMC(bit1Mode, 0))
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
            }
        }
        break;

        case SDCard_SDInterfaceType::SDCard_SDInterfaceType_Spi:
        {
            int spiBus;
            int chipSelectPin;

            spiBus = (int)(pThis[FIELD___spiBus].NumericByRef().s4);
            chipSelectPin = (int)(pThis[FIELD___chipSelectPin].NumericByRef().s4);

            // Get current Gpio pins used by SPI device
            spiBus--; // Spi devnumber 0 & 1

            // Try to initialised SPI bus in case it's not open, mount requires bus to be already initialised
            // Ignore errors as it may already been opened by managed code if trying to share bus
            CPU_SPI_Initialize(spiBus);

            // Try mount twice
            if (!Storage_MountSpi(spiBus, chipSelectPin, 0))
            {
                if (!Storage_MountSpi(spiBus, chipSelectPin, 0))
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

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    cardType = (SDCard_SDInterfaceType)pThis[FIELD___sdCardType].NumericByRef().s4;
    switch (cardType)
    {
        case SDCard_SDInterfaceType_Mmc:
        case SDCard_SDInterfaceType_Spi:

            // Unmount SPI device
            if (!Storage_UnMountSDCard())
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
    stack.SetResult_Boolean(CPU_GPIO_GetPinState(cardDetectPin) == GpioPinValue_Low);

#else

    (void)stack;
    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

#endif

    NANOCLR_NOCLEANUP();
}
