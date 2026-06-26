//-----------------------------------------------------------------------------
//
//                   ** WARNING! ** 
//    This file was generated automatically by a tool.
//    Re-running the tool will overwrite this file.
//    You should copy this file to a custom location
//    before adding any customization in the copy to
//    prevent loss of your changes when the tool is
//    re-run.
//
//-----------------------------------------------------------------------------

#ifndef NANOFRAMEWORK_HARDWARE_RP2040_NANOFRAMEWORK_HARDWARE_RP2040_NANOFRAMEWORK_HARDWARE_RP2040_PIO_PIOBLOCK_H
#define NANOFRAMEWORK_HARDWARE_RP2040_NANOFRAMEWORK_HARDWARE_RP2040_NANOFRAMEWORK_HARDWARE_RP2040_PIO_PIOBLOCK_H

namespace nanoFramework_Hardware_Rp2040
{
    namespace nanoFramework_Hardware_Rp2040
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

#endif // NANOFRAMEWORK_HARDWARE_RP2040_NANOFRAMEWORK_HARDWARE_RP2040_NANOFRAMEWORK_HARDWARE_RP2040_PIO_PIOBLOCK_H
