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

#ifndef NANOFRAMEWORK_HARDWARE_RPI_NANOFRAMEWORK_HARDWARE_RPI_NANOFRAMEWORK_HARDWARE_RPI_PIO_PIOSTATEMACHINE_H
#define NANOFRAMEWORK_HARDWARE_RPI_NANOFRAMEWORK_HARDWARE_RPI_NANOFRAMEWORK_HARDWARE_RPI_PIO_PIOSTATEMACHINE_H

namespace nf_hardware_rpi_native
{
    namespace nanoFramework_Hardware_Rpi
    {
        struct PioStateMachine
        {
            // Helper Functions to access fields of managed object
            // Declaration of stubs. These functions are implemented by Interop code developers

            static void NativeInit( signed int param0, signed int param1, signed int param2, CLR_RT_TypedArray_UINT32 param3, HRESULT &hr );

            static void NativeSetEnabled( signed int param0, signed int param1, bool param2, HRESULT &hr );

            static void NativePutBlocking( signed int param0, signed int param1, unsigned int param2, HRESULT &hr );

            static unsigned int NativeGetBlocking( signed int param0, signed int param1, HRESULT &hr );

            static bool NativeTxFull( signed int param0, signed int param1, HRESULT &hr );

            static bool NativeRxEmpty( signed int param0, signed int param1, HRESULT &hr );

            static void NativeUnclaim( signed int param0, signed int param1, HRESULT &hr );

            static void NativeSetConsecutivePinDirs( signed int param0, signed int param1, signed int param2, signed int param3, bool param4, HRESULT &hr );

            static void NativeClearFifos( signed int param0, signed int param1, HRESULT &hr );

            static void NativeDrainTxFifo( signed int param0, signed int param1, HRESULT &hr );

            static void NativeRestart( signed int param0, signed int param1, HRESULT &hr );

            static void NativeClkDivRestart( signed int param0, signed int param1, HRESULT &hr );

            static void NativeExec( signed int param0, signed int param1, uint16_t param2, HRESULT &hr );

            static unsigned int NativeTxLevel( signed int param0, signed int param1, HRESULT &hr );

            static unsigned int NativeRxLevel( signed int param0, signed int param1, HRESULT &hr );

            static unsigned int NativeGetPc( signed int param0, signed int param1, HRESULT &hr );

            static void NativeSetClockDivisor( signed int param0, signed int param1, signed int param2, signed int param3, HRESULT &hr );

        };
    }
}

#endif // NANOFRAMEWORK_HARDWARE_RPI_NANOFRAMEWORK_HARDWARE_RPI_NANOFRAMEWORK_HARDWARE_RPI_PIO_PIOSTATEMACHINE_H
