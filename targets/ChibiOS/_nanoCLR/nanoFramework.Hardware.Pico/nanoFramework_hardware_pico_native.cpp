//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//





#include "nanoFramework_hardware_pico_native.h"



// clang-format off



static const CLR_RT_MethodHandler method_lookup[] =

{


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::NativeAddProgram___STATIC__I4__I4__SZARRAY_U2__I4__I4,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::NativeRemoveProgram___STATIC__VOID__I4__I4__I4,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::NativeClaimUnusedSm___STATIC__I4__I4__BOOLEAN,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::NativeInitGpio___STATIC__VOID__I4__I4,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::NativeForceIrq___STATIC__VOID__I4__I4,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock::NativeClearIrq___STATIC__VOID__I4__I4,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::NativeRead___STATIC__I4__I4__I4__SZARRAY_U4__I4__I4__I4,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::NativeWrite___STATIC__I4__I4__I4__SZARRAY_U4__I4__I4__I4,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::NativeInit___STATIC__VOID__I4__I4__I4__SZARRAY_U4,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::NativeSetEnabled___STATIC__VOID__I4__I4__BOOLEAN,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::NativePutBlocking___STATIC__VOID__I4__I4__U4,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::NativeGetBlocking___STATIC__U4__I4__I4,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::NativeTxFull___STATIC__BOOLEAN__I4__I4,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::NativeRxEmpty___STATIC__BOOLEAN__I4__I4,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::NativeUnclaim___STATIC__VOID__I4__I4,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::NativeSetConsecutivePinDirs___STATIC__VOID__I4__I4__I4__I4__BOOLEAN,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::NativeClearFifos___STATIC__VOID__I4__I4,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::NativeDrainTxFifo___STATIC__VOID__I4__I4,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::NativeRestart___STATIC__VOID__I4__I4,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::NativeClkDivRestart___STATIC__VOID__I4__I4,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::NativeExec___STATIC__VOID__I4__I4__U2,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::NativeTxLevel___STATIC__U4__I4__I4,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::NativeRxLevel___STATIC__U4__I4__I4,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::NativeGetPc___STATIC__U4__I4__I4,


    Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine::NativeSetClockDivisor___STATIC__VOID__I4__I4__I4__I4,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


    NULL,


};



const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_Pico =

{

    "nanoFramework.Hardware.Pico",

    0x52CD0E56,

    method_lookup,

    { 1, 0, 0, 0 }

};



// clang-format on


