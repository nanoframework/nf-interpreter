//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//



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




#ifndef NANOFRAMEWORK_HARDWARE_PICO_NATIVE_H

#define NANOFRAMEWORK_HARDWARE_PICO_NATIVE_H



#include <nanoCLR_Interop.h>

#include <nanoCLR_Runtime.h>

#include <nanoPackStruct.h>


#include <corlib_native.h>





typedef enum __nfpack DestinationOperand

{


    DestinationOperand_Pins = 0,


    DestinationOperand_RegisterX = 1,


    DestinationOperand_RegisterY = 2,


    DestinationOperand_DiscardsData = 3,


    DestinationOperand_PinDirs = 4,


    DestinationOperand_Pc = 5,


    DestinationOperand_Status = 5,


    DestinationOperand_InputShiftRegister = 6,


    DestinationOperand_OutputShiftRegister = 7,


    DestinationOperand_Executes = 7,



} DestinationOperand;



typedef enum __nfpack PioCondition

{


    PioCondition_Always = 0,


    PioCondition_XZero = 1,


    PioCondition_XPostDec = 2,


    PioCondition_YZero = 3,


    PioCondition_YPostDec = 4,


    PioCondition_XNotEqualY = 5,


    PioCondition_Pin = 6,


    PioCondition_OsrNotEmpty = 7,



} PioCondition;



typedef enum __nfpack PioFifoJoin

{


    PioFifoJoin_None = 0,


    PioFifoJoin_Tx = 1,


    PioFifoJoin_Rx = 2,


    PioFifoJoin_TxGet = 4,


    PioFifoJoin_TxPut = 8,


    PioFifoJoin_PutGet = 12,



} PioFifoJoin;



typedef enum __nfpack PioMovOp

{


    PioMovOp_None = 0,


    PioMovOp_Invert = 1,


    PioMovOp_BitReverse = 2,



} PioMovOp;



typedef enum __nfpack PioMovStatusSel

{


    PioMovStatusSel_TxLevel = 0,


    PioMovStatusSel_RxLevel = 1,



} PioMovStatusSel;



typedef enum __nfpack PioVersion

{


    PioVersion_Rp2040 = 0,


    PioVersion_Rp2350 = 1,



} PioVersion;



typedef enum __nfpack PioWaitSource

{


    PioWaitSource_Gpio = 0,


    PioWaitSource_Pin = 1,


    PioWaitSource_Irq = 2,



} PioWaitSource;



typedef enum __nfpack ShiftDirection

{


    ShiftDirection_Left = 0,


    ShiftDirection_Right = 1,



} ShiftDirection;



typedef enum __nfpack SourceOperand

{


    SourceOperand_Pins = 0,


    SourceOperand_RegisterX = 1,


    SourceOperand_RegisterY = 2,


    SourceOperand_Null = 3,


    SourceOperand_Status = 5,


    SourceOperand_InputShiftRegister = 6,


    SourceOperand_OutputShiftRegister = 7,



} SourceOperand;





struct Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_Pio

{




    static const int FIELD_STATIC___blocks = 0;



    static const int FIELD_STATIC___lock = 1;










    //--//

};



struct Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioAssemblerOptions

{







    // renamed backing field '<Version>k__BackingField'


    static const int FIELD__Version = 1;



    // renamed backing field '<SideSetCount>k__BackingField'


    static const int FIELD__SideSetCount = 2;



    // renamed backing field '<SideSetOption>k__BackingField'


    static const int FIELD__SideSetOption = 3;



    // renamed backing field '<SideSetPinDirs>k__BackingField'


    static const int FIELD__SideSetPinDirs = 4;







    //--//

};



struct Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioInstructionRef

{







    static const int FIELD___owner = 1;



    static const int FIELD___index = 2;







    //--//

};



struct Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioLabel

{







    // renamed backing field '<Id>k__BackingField'


    static const int FIELD__Id = 1;



    // renamed backing field '<Owner>k__BackingField'


    static const int FIELD__Owner = 2;



    // renamed backing field '<Address>k__BackingField'


    static const int FIELD__Address = 3;







    //--//

};



struct Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioAssembler

{







    static const int FIELD___baseBits = 1;



    static const int FIELD___delay = 2;



    static const int FIELD___sideValue = 3;



    static const int FIELD___sideUsed = 4;



    static const int FIELD___jmpLabel = 5;



    static const int FIELD___count = 6;



    static const int FIELD___labelCount = 7;



    static const int FIELD___version = 8;



    static const int FIELD___sideSetCount = 9;



    static const int FIELD___sideSetOpt = 10;



    static const int FIELD___sideSetPinDirs = 11;



    static const int FIELD___wrapTarget = 12;



    static const int FIELD___wrap = 13;



    static const int FIELD___origin = 14;



    static const int FIELD___outShiftDir = 15;



    static const int FIELD___autoPull = 16;



    static const int FIELD___pullThreshold = 17;



    static const int FIELD___inShiftDir = 18;



    static const int FIELD___autoPush = 19;



    static const int FIELD___pushThreshold = 20;







    //--//

};



struct Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioBlock

{







    static const int FIELD___index = 1;



    static const int FIELD___irqDispatcher = 2;



    static const int FIELD___interruptCallbacks = 3;



    static const int FIELD___irqLock = 4;





    NANOCLR_NATIVE_DECLARE(NativeAddProgram___STATIC__I4__I4__SZARRAY_U2__I4__I4);


    NANOCLR_NATIVE_DECLARE(NativeRemoveProgram___STATIC__VOID__I4__I4__I4);


    NANOCLR_NATIVE_DECLARE(NativeClaimUnusedSm___STATIC__I4__I4__BOOLEAN);


    NANOCLR_NATIVE_DECLARE(NativeInitGpio___STATIC__VOID__I4__I4);


    NANOCLR_NATIVE_DECLARE(NativeForceIrq___STATIC__VOID__I4__I4);


    NANOCLR_NATIVE_DECLARE(NativeClearIrq___STATIC__VOID__I4__I4);




    //--//

};



struct Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioProgramOptions

{







    // renamed backing field '<Version>k__BackingField'


    static const int FIELD__Version = 1;



    // renamed backing field '<Origin>k__BackingField'


    static const int FIELD__Origin = 2;



    // renamed backing field '<SideSetCount>k__BackingField'


    static const int FIELD__SideSetCount = 3;



    // renamed backing field '<SideSetOptional>k__BackingField'


    static const int FIELD__SideSetOptional = 4;



    // renamed backing field '<SideSetPinDirs>k__BackingField'


    static const int FIELD__SideSetPinDirs = 5;



    // renamed backing field '<OutShiftDir>k__BackingField'


    static const int FIELD__OutShiftDir = 6;



    // renamed backing field '<AutoPull>k__BackingField'


    static const int FIELD__AutoPull = 7;



    // renamed backing field '<PullThreshold>k__BackingField'


    static const int FIELD__PullThreshold = 8;



    // renamed backing field '<InShiftDir>k__BackingField'


    static const int FIELD__InShiftDir = 9;



    // renamed backing field '<AutoPush>k__BackingField'


    static const int FIELD__AutoPush = 10;



    // renamed backing field '<PushThreshold>k__BackingField'


    static const int FIELD__PushThreshold = 11;







    //--//

};



struct Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioProgram

{







    // renamed backing field '<Instructions>k__BackingField'


    static const int FIELD__Instructions = 1;



    // renamed backing field '<Wrap>k__BackingField'


    static const int FIELD__Wrap = 2;



    // renamed backing field '<WrapTarget>k__BackingField'


    static const int FIELD__WrapTarget = 3;



    // renamed backing field '<Origin>k__BackingField'


    static const int FIELD__Origin = 4;



    // renamed backing field '<SideSetCount>k__BackingField'


    static const int FIELD__SideSetCount = 5;



    // renamed backing field '<SideSetOptional>k__BackingField'


    static const int FIELD__SideSetOptional = 6;



    // renamed backing field '<SideSetPinDirs>k__BackingField'


    static const int FIELD__SideSetPinDirs = 7;



    // renamed backing field '<OutShiftDir>k__BackingField'


    static const int FIELD__OutShiftDir = 8;



    // renamed backing field '<AutoPull>k__BackingField'


    static const int FIELD__AutoPull = 9;



    // renamed backing field '<PullThreshold>k__BackingField'


    static const int FIELD__PullThreshold = 10;



    // renamed backing field '<InShiftDir>k__BackingField'


    static const int FIELD__InShiftDir = 11;



    // renamed backing field '<AutoPush>k__BackingField'


    static const int FIELD__AutoPush = 12;



    // renamed backing field '<PushThreshold>k__BackingField'


    static const int FIELD__PushThreshold = 13;



    // renamed backing field '<Version>k__BackingField'


    static const int FIELD__Version = 14;







    //--//

};



struct Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachine

{







    static const int FIELD___block = 1;



    static const int FIELD___sm = 2;



    static const int FIELD___owned = 3;



    static const int FIELD___disposed = 4;



    static const int FIELD___enabled = 5;





    NANOCLR_NATIVE_DECLARE(NativeRead___STATIC__I4__I4__I4__SZARRAY_U4__I4__I4__I4);


    NANOCLR_NATIVE_DECLARE(NativeWrite___STATIC__I4__I4__I4__SZARRAY_U4__I4__I4__I4);


    NANOCLR_NATIVE_DECLARE(NativeInit___STATIC__VOID__I4__I4__I4__SZARRAY_U4);


    NANOCLR_NATIVE_DECLARE(NativeSetEnabled___STATIC__VOID__I4__I4__BOOLEAN);


    NANOCLR_NATIVE_DECLARE(NativePutBlocking___STATIC__VOID__I4__I4__U4);


    NANOCLR_NATIVE_DECLARE(NativeGetBlocking___STATIC__U4__I4__I4);


    NANOCLR_NATIVE_DECLARE(NativeTxFull___STATIC__BOOLEAN__I4__I4);


    NANOCLR_NATIVE_DECLARE(NativeRxEmpty___STATIC__BOOLEAN__I4__I4);


    NANOCLR_NATIVE_DECLARE(NativeUnclaim___STATIC__VOID__I4__I4);


    NANOCLR_NATIVE_DECLARE(NativeSetConsecutivePinDirs___STATIC__VOID__I4__I4__I4__I4__BOOLEAN);


    NANOCLR_NATIVE_DECLARE(NativeClearFifos___STATIC__VOID__I4__I4);


    NANOCLR_NATIVE_DECLARE(NativeDrainTxFifo___STATIC__VOID__I4__I4);


    NANOCLR_NATIVE_DECLARE(NativeRestart___STATIC__VOID__I4__I4);


    NANOCLR_NATIVE_DECLARE(NativeClkDivRestart___STATIC__VOID__I4__I4);


    NANOCLR_NATIVE_DECLARE(NativeExec___STATIC__VOID__I4__I4__U2);


    NANOCLR_NATIVE_DECLARE(NativeTxLevel___STATIC__U4__I4__I4);


    NANOCLR_NATIVE_DECLARE(NativeRxLevel___STATIC__U4__I4__I4);


    NANOCLR_NATIVE_DECLARE(NativeGetPc___STATIC__U4__I4__I4);


    NANOCLR_NATIVE_DECLARE(NativeSetClockDivisor___STATIC__VOID__I4__I4__I4__I4);




    //--//

};



struct Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioStateMachineConfig

{







    static const int FIELD___outBase = 1;



    static const int FIELD___outCount = 2;



    static const int FIELD___setBase = 3;



    static const int FIELD___setCount = 4;



    static const int FIELD___sideSetBase = 5;



    static const int FIELD___inBase = 6;



    static const int FIELD___inBaseSet = 7;



    static const int FIELD___jmpPin = 8;



    static const int FIELD___jmpPinSet = 9;



    static const int FIELD___sideSetCount = 10;



    static const int FIELD___sideSetOpt = 11;



    static const int FIELD___sideSetPinDirs = 12;



    static const int FIELD___outShiftDir = 13;



    static const int FIELD___autoPull = 14;



    static const int FIELD___pullThreshold = 15;



    static const int FIELD___inShiftDir = 16;



    static const int FIELD___autoPush = 17;



    static const int FIELD___pushThreshold = 18;



    static const int FIELD___wrapTarget = 19;



    static const int FIELD___wrap = 20;



    static const int FIELD___clkDivInt = 21;



    static const int FIELD___clkDivFrac = 22;



    static const int FIELD___fifoJoin = 23;



    static const int FIELD___gpioBase = 24;



    static const int FIELD___movStatusSel = 25;



    static const int FIELD___movStatusN = 26;



    static const int FIELD___outSticky = 27;



    static const int FIELD___inlineOutEn = 28;



    static const int FIELD___outEnSel = 29;







    //--//

};



extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_Pico;


#endif // NANOFRAMEWORK_HARDWARE_PICO_NATIVE_H

