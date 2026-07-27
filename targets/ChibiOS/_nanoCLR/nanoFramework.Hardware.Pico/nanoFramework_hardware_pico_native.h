//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

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

typedef enum __nfpack PioInterruptFlags
{
    PioInterruptFlags_None = 0,
    PioInterruptFlags_Sm0 = 1,
    PioInterruptFlags_Sm1 = 2,
    PioInterruptFlags_Sm2 = 4,
    PioInterruptFlags_Sm3 = 8,
} PioInterruptFlags;

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

typedef enum __nfpack PioStateMachineIndex
{
    PioStateMachineIndex_Sm0 = 0,
    PioStateMachineIndex_Sm1 = 1,
    PioStateMachineIndex_Sm2 = 2,
    PioStateMachineIndex_Sm3 = 3,
    PioStateMachineIndex_Any = 4,
} PioStateMachineIndex;

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

    NANOCLR_NATIVE_DECLARE(get_MinIndex___STATIC__I4);
    NANOCLR_NATIVE_DECLARE(get_BlockCount___STATIC__I4);
    NANOCLR_NATIVE_DECLARE(get_MaxPin___STATIC__I4);

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
    static const int FIELD_STATIC__s_eventListener = 2;

    static const int FIELD___index = 1;
    static const int FIELD___interruptCallbacks = 2;
    static const int FIELD___irqLock = 3;

    NANOCLR_NATIVE_DECLARE(AddProgram___U4__nanoFrameworkHardwarePicoPioPioProgram);
    NANOCLR_NATIVE_DECLARE(RemoveProgram___VOID__nanoFrameworkHardwarePicoPioPioProgram__U4);
    NANOCLR_NATIVE_DECLARE(InitGpio___VOID__I4);
    NANOCLR_NATIVE_DECLARE(ForceIrq___VOID__I4);
    NANOCLR_NATIVE_DECLARE(ClearIrq___VOID__I4);
    NANOCLR_NATIVE_DECLARE(NativeClaimSm___I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeSetIrqEnabled___VOID__BOOLEAN);

    //--//
};

struct Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioEvent
{
    static const int FIELD__BlockIndex = 3;
    static const int FIELD__Flags = 4;

    //--//
};

struct Library_nanoFramework_hardware_pico_native_nanoFramework_Hardware_Pico_Pio_PioEventListener
{
    static const int FIELD___pioMap = 1;
    static const int FIELD___syncRoot = 2;

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

    NANOCLR_NATIVE_DECLARE(get_Enabled___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(set_Enabled___VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(get_IsTxFull___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(get_IsRxEmpty___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(get_TxLevel___U4);
    NANOCLR_NATIVE_DECLARE(get_RxLevel___U4);
    NANOCLR_NATIVE_DECLARE(get_ProgramCounter___U4);
    NANOCLR_NATIVE_DECLARE(set_ClockDivisor___VOID__R4);
    NANOCLR_NATIVE_DECLARE(Put___VOID__U4);
    NANOCLR_NATIVE_DECLARE(Get___U4);
    NANOCLR_NATIVE_DECLARE(TryPut___BOOLEAN__U4);
    NANOCLR_NATIVE_DECLARE(TryGet___BOOLEAN__BYREF_U4);
    NANOCLR_NATIVE_DECLARE(ClearFifos___VOID);
    NANOCLR_NATIVE_DECLARE(DrainTxFifo___VOID);
    NANOCLR_NATIVE_DECLARE(Restart___VOID);
    NANOCLR_NATIVE_DECLARE(ClockDivRestart___VOID);
    NANOCLR_NATIVE_DECLARE(Exec___VOID__U2);
    NANOCLR_NATIVE_DECLARE(SetConsecutivePinDirs___VOID__I4__I4__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(Read___I4__SZARRAY_U4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(Write___I4__SZARRAY_U4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeInit___VOID__I4__SZARRAY_U4);
    NANOCLR_NATIVE_DECLARE(NativeUnclaim___VOID);

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
