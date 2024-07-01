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

// void cardDetect_interrupt(GPIO_PIN Pin, bool pinState, void *pArg)
// {
//     (void)Pin;

//     postManagedStorageEvent(pinState, (uint32_t)pArg);
// }

// // Reserve all MMC pins
// // CMD, Data0, Data1, Data2, Data3
// int8_t pins4bit[] = {15, 2, 4, 12, 13};
// // CMD, Data0
// int8_t pins1bit[] = {15, 2};

// void GetMMCPins(bool _1bit, int *count, int8_t **pPins)
// {
//     if (_1bit)
//     {
//         *pPins = pins1bit;
//         *count = sizeof(pins1bit);
//     }
//     else
//     {
//         *pPins = pins4bit;
//         *count = sizeof(pins4bit);
//     }
// }

// void UnReservePins(int count, int8_t *pPins)
// {
//     for (int index = count - 1; index >= 0; index--)
//     {
//         CPU_GPIO_ReservePin((GPIO_PIN)pPins[index], false);
//     }
// }

// bool TryReservePins(int count, int8_t *pPins)
// {
//     int index = 0;
//     bool failed = false;

//     for (index = 0; index < count; index++)
//     {
//         if (CPU_GPIO_ReservePin((int8_t)pPins[index], true) == false)
//         {
//             failed = true;
//             break;
//         }
//     }

//     // If we failed then UnReserve pins already reserved
//     if (failed)
//     {
//         UnReservePins(index - 1, pPins);
//         return false;
//     }

//     return true;
// }

HRESULT Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::InitNative___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::NativeDispose___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::MountNative___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::UnmountNative___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::PollCardDetectNative___BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}
