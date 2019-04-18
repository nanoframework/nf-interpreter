//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_storage_native.h"
#include <target_windows_storage_config.h>
#include <nanoHAL_Windows_Storage.h>
#include <target_platform.h>

#include "Esp32_DeviceMapping.h"

// Defines for Windows.Storage.c
extern "C"
{
    bool Storage_InitSDCardSPI(char * vfsName, int maxFiles, int pin_Miso, int pin_Mosi, int pin_Clk, int pin_Cs);
    bool Storage_InitSDCardMMC(char * vfsName, int maxFiles, bool bit1Mode);
    bool Storage_UnMountSDCard();
}

HRESULT Library_win_storage_native_Windows_Storage_Devices_SDCard::MountMMCNative___STATIC__VOID__BOOLEAN(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();

#if defined(HAL_USE_SDC)
    bool bit1Mode = stack.Arg0().NumericByRef().s4;
	char     mountPoint[] = INDEX0_DRIVE_LETTER;

	// Change fatfs drive letter to mount point  D: -> /D
	mountPoint[1] = mountPoint[0];
	mountPoint[0] = '/';

    // Try mounting
    if (!Storage_InitSDCardMMC(mountPoint, SDC_MAX_OPEN_FILES, bit1Mode) )
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
    }
#else
    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_storage_native_Windows_Storage_Devices_SDCard::MountSpiNative___STATIC__VOID__I4__I4(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();

#if defined(HAL_USE_SDC)

    int      spiBus;
    uint32_t CSPin;
    char     mountPoint[] = INDEX0_DRIVE_LETTER;

	// Change fatfs drive letter to mount point  D: -> /D
	mountPoint[1] = mountPoint[0]; 
	mountPoint[0] = '/';

    // Get passed SPi bus number
    spiBus = stack.Arg0().NumericByRef().s4;

    // get Gpio pin for Chip select
    CSPin = stack.Arg1().NumericByRef().s4;

    // Get current gpio pins used by SPI device 
    int mosiPin =  Esp32_GetMappedDevicePins(DEV_TYPE_SPI, spiBus, 0);
    int misoPin =  Esp32_GetMappedDevicePins(DEV_TYPE_SPI, spiBus, 1);
    int clockPin = Esp32_GetMappedDevicePins(DEV_TYPE_SPI, spiBus, 2);

    // Try mounting
    if ( !Storage_InitSDCardSPI(mountPoint, SDC_MAX_OPEN_FILES, misoPin, mosiPin, clockPin, CSPin) )
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
    }
#else
	(void)stack;
    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
#endif

    NANOCLR_NOCLEANUP();
}


HRESULT Library_win_storage_native_Windows_Storage_Devices_SDCard::UnmountNative___STATIC__VOID(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();
    (void)stack;

#if defined(HAL_USE_SDC)

    // Unmount SPI device
    if (!Storage_UnMountSDCard())
    {
        // SDcard not mounted
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }
    
#else	
    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
#endif

    NANOCLR_NOCLEANUP();
}
