//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "win_storage_native_target.h"
#include <target_windows_storage_config.h>
#include <nanoHAL_Windows_Storage.h>
#include <target_platform.h>
#include <Esp32_DeviceMapping.h>

HRESULT Library_win_storage_native_Windows_Storage_Devices_SDCard::MountMMCNative___STATIC__VOID__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if (HAL_USE_SDC == TRUE)
    bool bit1Mode = (bool)stack.Arg0().NumericByRef().u1;
    if (!Storage_MountMMC(bit1Mode, 0))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
    }
#else
    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_storage_native_Windows_Storage_Devices_SDCard::MountSpiNative___STATIC__VOID__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if (HAL_USE_SDC == TRUE)

    SPI_DEVICE_CONFIGURATION spiConfig;

    // internally SPI bus ID is zero based
    uint32_t busIndex = stack.Arg0().NumericByRef().s4 - 1;

    // get GPIO pin for Chip select
    uint32_t csPin = stack.Arg1().NumericByRef().s4;

    // set SPI bus configuration
    spiConfig.BusConfiguration = SpiBusConfiguration_FullDuplex;

    // Try to initialised SPI bus in case it's not open, mount requires bus to be already initialised
    // Ignore errors as it may already been opened by managed code if trying to share bus
    CPU_SPI_Initialize(busIndex, spiConfig);

    if (!Storage_MountSpi(busIndex, csPin, 0))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
    }
#else
    (void)stack;
    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_storage_native_Windows_Storage_Devices_SDCard::UnmountNative___STATIC__VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    (void)stack;

#if (HAL_USE_SDC == TRUE)

    // Unmount SPI device
    if (!Storage_UnMountSDCard(0))
    {
        // SDcard not mounted
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

#else
    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
#endif

    NANOCLR_NOCLEANUP();
}
