//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NANOFRAMEWORK_HARDWARE_RPI_NANOFRAMEWORK_HARDWARE_RPI_NANOFRAMEWORK_HARDWARE_RPI_PIO_PIOBLOCK_H
#define NANOFRAMEWORK_HARDWARE_RPI_NANOFRAMEWORK_HARDWARE_RPI_NANOFRAMEWORK_HARDWARE_RPI_PIO_PIOBLOCK_H

namespace nf_hardware_rpi_native
{
    namespace nanoFramework_Hardware_Rpi
    {
        struct PioBlock
        {
            // Helper Functions to access fields of managed object
            // Declaration of stubs. These functions are implemented by Interop code developers

            static signed int NativeAddProgram( signed int param0, CLR_RT_TypedArray_UINT16 param1, signed int param2, signed int param3, HRESULT &hr );

            static void NativeRemoveProgram( signed int param0, signed int param1, signed int param2, HRESULT &hr );

            static signed int NativeClaimUnusedSm( signed int param0, bool param1, HRESULT &hr );

            static void NativeInitGpio( signed int param0, signed int param1, HRESULT &hr );

            static void NativeForceIrq( signed int param0, signed int param1, HRESULT &hr );

            static void NativeClearIrq( signed int param0, signed int param1, HRESULT &hr );

        };
    }
}

#endif // NANOFRAMEWORK_HARDWARE_RPI_NANOFRAMEWORK_HARDWARE_RPI_NANOFRAMEWORK_HARDWARE_RPI_PIO_PIOBLOCK_H
