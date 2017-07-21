//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#ifndef _WINDOWS_DEVICES_SPI_NATIVE_WINDOWS_DEVICES_SPI_SPIBUSINFO_H_
#define _WINDOWS_DEVICES_SPI_NATIVE_WINDOWS_DEVICES_SPI_SPIBUSINFO_H_

namespace Windows
{
    namespace Devices
    {
        namespace Spi
        {
            struct SpiBusInfo
            {
                // Helper Functions to access fields of managed object
                // TODO not sure why this showing up here               
                //static signed int& Get_<ChipSelectLineCount>k__BackingField( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_INT32( pMngObj, Library_windows_devices_spi_native_Windows_Devices_Spi_SpiBusInfo::FIELD__<ChipSelectLineCount>k__BackingField ); }

                // Declaration of stubs. These functions are implemented by Interop code developers
                static signed int get_MaxClockFrequency( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
                static signed int get_MinClockFrequency( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
            };
        }
    }
}
#endif  //_WINDOWS_DEVICES_SPI_NATIVE_WINDOWS_DEVICES_SPI_SPIBUSINFO_H_
