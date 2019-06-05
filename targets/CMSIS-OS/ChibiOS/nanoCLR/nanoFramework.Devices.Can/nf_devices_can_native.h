//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#ifndef _NF_DEVICES_CAN_NATIVE_H_
#define _NF_DEVICES_CAN_NATIVE_H_

#include <hal.h>
#include "target_nf_devices_can_config.h"
#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>

struct Library_nf_devices_can_native_nanoFramework_Devices_Can_CanController
{
    static const int FIELD_STATIC__s_eventListener = 0;

    static const int FIELD___disposed = 1;
    static const int FIELD___callbacks = 2;
    static const int FIELD___syncLock = 3;
    static const int FIELD___controllerId = 4;
    static const int FIELD___settings = 5;
    static const int FIELD___message = 6;
    static const int FIELD__ControllerId = 7;

    NANOCLR_NATIVE_DECLARE(WriteMessage___VOID__nanoFrameworkDevicesCanCanMessage);
    NANOCLR_NATIVE_DECLARE(GetMessage___nanoFrameworkDevicesCanCanMessage);
    NANOCLR_NATIVE_DECLARE(DisposeNative___VOID);
    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeUpdateCallbacks___VOID);
    NANOCLR_NATIVE_DECLARE(GetDeviceSelector___STATIC__STRING);

    //--//

};

struct Library_nf_devices_can_native_nanoFramework_Devices_Can_CanControllerEventListener
{
    static const int FIELD___canControllersMap = 1;


    //--//

};

struct Library_nf_devices_can_native_nanoFramework_Devices_Can_CanControllerManager
{
    static const int FIELD_STATIC___syncLock = 1;
    static const int FIELD_STATIC__s_controllersCollection = 2;


    //--//

};

struct Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessage
{
    static const int FIELD___id = 1;
    static const int FIELD___identifierType = 2;
    static const int FIELD___frameType = 3;
    static const int FIELD___message = 4;


    //--//

};

struct Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessageEvent
{
    static const int FIELD__ControllerIndex = 3;
    static const int FIELD__Event = 4;


    //--//

};

struct Library_nf_devices_can_native_nanoFramework_Devices_Can_CanSettings
{
    static const int FIELD___baudRatePrescaler = 1;
    static const int FIELD___phaseSegment1 = 2;
    static const int FIELD___phaseSegment2 = 3;
    static const int FIELD___syncJumpWidth = 4;


    //--//

};



extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Devices_Can;

// struct representing the CAN 
struct NF_PAL_CAN
{
    CANDriver*  Driver;
    CANConfig   Configuration;
    thread_t*   ReceiverThread;

    HAL_RingBuffer<CANRxFrame> MsgRingBuffer;
    CANRxFrame* MsgBuffer;
};

///////////////////////////////////////////
// declaration of the CAN PAL strucs     //
///////////////////////////////////////////
#if STM32_CAN_USE_CAN1
    extern NF_PAL_CAN Can1_PAL;
#endif
#if STM32_CAN_USE_CAN2
    extern NF_PAL_CAN Can2_PAL;
#endif
#if STM32_CAN_USE_CAN3
    extern NF_PAL_CAN Can3_PAL;
#endif

// the following macro defines a function that configures the GPIO pins for a STM32 CAN
// it gets called in the can_lld_start function// this is required because the CAN peripherals can use multiple GPIO configuration combinations
#define CAN_CONFIG_PINS(num, gpio_port_tx, gpio_port_rx, tx_pin, rx_pin, alternate_function) void ConfigPins_CAN##num() { \
    palSetPadMode(gpio_port_tx, tx_pin, PAL_MODE_ALTERNATE(alternate_function) | PAL_STM32_OTYPE_PUSHPULL \
    | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_MODE_ALTERNATE); \
    palSetPadMode(gpio_port_rx, rx_pin, PAL_MODE_ALTERNATE(alternate_function) | PAL_STM32_OTYPE_PUSHPULL \
    | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_MODE_ALTERNATE); \
}

///////////////////////////////////////////////////////////////////////////////////////////
// when a CAN is defined the declarations below will have the real function/configuration 
// in the target folder @ target_nf_devices_can_config.cpp
///////////////////////////////////////////////////////////////////////////////////////////
void ConfigPins_CAN1();
void ConfigPins_CAN2();
void ConfigPins_CAN3();


/////////////////////////////////////
// CAN Msg buffers                 //
// these live in the target folder //
/////////////////////////////////////
extern CANRxFrame Can1_MsgBuffer[];
extern CANRxFrame Can2_MsgBuffer[];
extern CANRxFrame Can3_MsgBuffer[];

// 
#define CAN_INIT(num, buffer_size) void Init_Can##num() { \
    Can##num##_PAL.MsgBuffer = Can##num##_MsgBuffer; \
    Can##num##_PAL.MsgRingBuffer.Initialize( Can##num##_PAL.MsgBuffer, buffer_size); \
}

// when a CAN is defined the declarations below will have the real function/configuration 
// in the target folder @ target_nf_devices_can_config.cpp
void Init_Can1();
void Init_Can2();
void Init_Can3();

#endif  //_NF_DEVICES_CAN_NATIVE_H_
