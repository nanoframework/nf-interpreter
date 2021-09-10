//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"

extern "C"
{
    bool Storage_MountMMC( bool bit1Mode);
    bool Storage_MountSpi(int spiBus, uint32_t CSPin);
    bool Storage_InitSDCardSPI(char * vfsName, int maxFiles, int pin_Miso, int pin_Mosi, int pin_Clk, int pin_Cs);
    bool Storage_InitSDCardMMC(char * vfsName, int maxFiles, bool bit1Mode);
    bool Storage_UnMountSDCard();
}

HRESULT Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::MountMMCNative___STATIC__VOID__nanoFrameworkSystemIOFileSystemSDCardSDDataWidth( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

#if defined(HAL_USE_SDC)
    bool bit1Mode = stack.Arg0().NumericByRef().s4;
    if (!Storage_MountMMC(bit1Mode))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
    }
#else
    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::MountSpiNative___STATIC__VOID__I4__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

#if defined(HAL_USE_SDC)
    // Get passed SPi bus number 1 or 2
    int spiBus = stack.Arg0().NumericByRef().s4;

    // get Gpio pin for Chip select
    uint32_t CSPin = stack.Arg1().NumericByRef().s4;

    // Get current Gpio pins used by SPI device 
    spiBus--;  // Spi devnumber 0 & 1

    if (!Storage_MountSpi(spiBus, CSPin) )
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);    
    }
#else
	(void)stack;
    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
#endif    

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::UnmountNative___STATIC__VOID( CLR_RT_StackFrame &stack )
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
