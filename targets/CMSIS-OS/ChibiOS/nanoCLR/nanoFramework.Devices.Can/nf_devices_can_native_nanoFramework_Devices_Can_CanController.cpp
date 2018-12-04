//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_devices_can_native.h"


////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH nanoFramework.Devices.Can.CanEvent (in managed code) !!! //
////////////////////////////////////////////////////////////////////////////////////

enum CanEvent
{
    CanEvent_MessageReceived = 0,
    CanEvent_ErrorOccurred,
};

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

// CAN receiver thread
static THD_FUNCTION(CanReceiverThread, arg)
{
    NF_PAL_CAN* palCan = (NF_PAL_CAN*)arg;
    event_listener_t eventListener;
    CANRxFrame rxMsg;
    int controllerId;

  #if STM32_CAN_USE_CAN1
        if(palCan->Driver == &CAND1)
        {
           controllerId = 1;
        }
  #endif
  #if STM32_CAN_USE_CAN2
        if(palCan->Driver == &CAND2)
        {
           controllerId = 2;
        }
  #endif

    // set event listener for CAN RX
    chEvtRegister(&palCan->Driver->rxfull_event, &eventListener, 0);
    
    // wrap working code inside this call to process abort request with chThdTerminate()
    while (!chThdShouldTerminateX())
    {
        if (chEvtWaitAnyTimeout(ALL_EVENTS, TIME_MS2I(100)) == 0)
        {
            continue;
        }

        while (canReceiveTimeout(palCan->Driver, CAN_ANY_MAILBOX, &rxMsg, TIME_IMMEDIATE) == MSG_OK)
        {
            // got message
            
            // store message into the CAN buffer
            // don't care about the success of the operation, if it's full we are droping the message anyway
            palCan->MsgRingBuffer.Push(rxMsg);

            // fire event for CAN message received
            PostManagedEvent( EVENT_CAN, 0, controllerId, CanEvent_MessageReceived );
        }
    }

    // unregister CAN RX event
    chEvtUnregister(&palCan->Driver->rxfull_event, &eventListener);

    // need to set thread exit code before leaving
    chThdExit(MSG_OK);
}


/////////////////////////////////////////////////////////
// CAN PAL strucs delcared in nf_devices_can_native.h  //
/////////////////////////////////////////////////////////
#if STM32_CAN_USE_CAN1
    NF_PAL_CAN Can1_PAL;
#endif
#if STM32_CAN_USE_CAN2
    NF_PAL_CAN Can2_PAL;
#endif

// define this type here to make it shorter and improve code readability
typedef Library_nf_devices_can_native_nanoFramework_Devices_Can_CanSettings CanSettings;

HRESULT Library_nf_devices_can_native_nanoFramework_Devices_Can_CanController::WriteMessage___VOID__nanoFrameworkDevicesCanCanMessage( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // NF_PAL_CAN* palCan = NULL;
        uint32_t id;
        bool isSID, isDataFrame;
        CLR_RT_HeapBlock_Array* message;
        CLR_UINT8* msgArray;
        CANTxFrame txmsg;
        uint8_t msgLength = 0;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        CLR_RT_HeapBlock* pMessage = stack.Arg0().Dereference();
        
        // get controller index
        uint8_t controllerIndex = (uint8_t)(pThis[ FIELD___controllerId ].NumericByRef().s4);

        // dereference tx parameters
        id = (uint32_t)(pMessage[ Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessage::FIELD___id ].NumericByRef().u4);
        isSID = (bool)(pMessage[ Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessage::FIELD___isSID ].NumericByRefConst().u1 != 0);
        isDataFrame = (bool)(pMessage[ Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessage::FIELD___isDataFrame ].NumericByRefConst().u1 != 0);
        message = pMessage[ Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessage::FIELD___byteMessage ].DereferenceArray();
        msgLength = 8;
        if(message == NULL)
        {
            message = pMessage[ Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessage::FIELD___uShortMessage ].DereferenceArray();
            msgLength = 4;
        }
        if(message == NULL)
        {
            message = pMessage[ Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessage::FIELD___uIntMessage ].DereferenceArray();
            msgLength = 2;
        }
        if(message == NULL)
        {
            message = pMessage[ Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessage::FIELD___uLongMessage ].DereferenceArray();
            msgLength = 1;
        }
        FAULT_ON_NULL(message);
        msgArray = message->GetFirstElement();

        if(isSID)
        {
            txmsg.IDE = CAN_IDE_STD;
            txmsg.SID = id;
        }
        else
        {
            txmsg.IDE = CAN_IDE_EXT;
            txmsg.EID = id;
        }
        txmsg.RTR = isDataFrame ? CAN_RTR_DATA : CAN_RTR_REMOTE;
        txmsg.DLC = msgLength;
        memcpy(&msgArray[0], &txmsg.data8[0], msgLength);

        switch (controllerIndex)
        {
    #if STM32_CAN_USE_CAN1
            case 1:
                canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, TIME_MS2I(100));
                break;
    #endif
    #if STM32_CAN_USE_CAN2
            case 2:
                canTransmit(&CAND2, CAN_ANY_MAILBOX, &txmsg, TIME_MS2I(100));
                break;
    #endif
            default:
                // this CAN bus is not valid
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_devices_can_native_nanoFramework_Devices_Can_CanController::GetMessage___nanoFrameworkDevicesCanCanMessage( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CANRxFrame canFrame;
    NF_PAL_CAN* palCan;
    uint8_t controllerIndex;
    size_t messageCount = 0;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // get controller index
    controllerIndex = (uint8_t)(pThis[ FIELD___controllerId ].NumericByRef().s4);

    // get the PAL struct for this CAN bus
    switch (controllerIndex)
    {

      #if STM32_CAN_USE_CAN1
        case 1:
            palCan = &Can1_PAL;

            break;
      #endif

      #if STM32_CAN_USE_CAN2
        case 2:
            palCan = &Can2_PAL;
            break;
      #endif

        default:
            // this CAN bus is not valid
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
    }

    messageCount = palCan->MsgRingBuffer.Pop(&canFrame, 1);

    if(messageCount == 1)
    {
        // we have a message
    }

    NANOCLR_NOCLEANUP();
}

/*
HRESULT Library_nf_devices_can_native_nanoFramework_Devices_Can_CanController::WriteMessage___VOID__U4__BOOLEAN__BOOLEAN__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // NF_PAL_CAN* palCan = NULL;
        uint32_t id;
        bool isSID, isDataFrame;
        CLR_RT_HeapBlock_Array* message;
        CLR_UINT8* msgArray;
        CANTxFrame txmsg;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
        // get controller index
        uint8_t controllerIndex = (uint8_t)(pThis[ FIELD___controllerId ].NumericByRef().s4);

    //     switch (controllerIndex)
    //     {
    // #if STM32_CAN_USE_CAN1
    //         case 1:
    //             palCan = &Can1_PAL;
    //             break;
    // #endif
    // #if STM32_CAN_USE_CAN2
    //         case 2:
    //             palCan = &Can2_PAL;
    //             break;
    // #endif
    //         default:
    //             // this CAN bus is not valid
    //             NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    //             break;
    //     }

        // dereference tx parameters
        id = stack.Arg0().NumericByRef().u4;
        isSID = stack.Arg1().NumericByRefConst().u1 != 0;
        isDataFrame = stack.Arg2().NumericByRefConst().u1 != 0;
        message = stack.Arg3().DereferenceArray();
        msgArray = message->GetFirstElement();

        if(isSID)
        {
            txmsg.IDE = CAN_IDE_STD;
            txmsg.SID = id;
        }
        else
        {
            txmsg.IDE = CAN_IDE_EXT;
            txmsg.EID = id;
        }
        txmsg.RTR = isDataFrame ? CAN_RTR_DATA : CAN_RTR_REMOTE;
        txmsg.DLC = 8;
        memcpy(&msgArray[0], &txmsg.data8[0], 8);
        // txmsg.data8[0] = (uint8_t)message->GetElement(0);
        // txmsg.data8[1] = (uint8_t)message->GetElement(1);
        // txmsg.data8[2] = (uint8_t)message->GetElement(2);
        // txmsg.data8[3] = (uint8_t)message->GetElement(3);
        // txmsg.data8[4] = (uint8_t)message->GetElement(4);
        // txmsg.data8[5] = (uint8_t)message->GetElement(5);
        // txmsg.data8[6] = (uint8_t)message->GetElement(6);
        // txmsg.data8[7] = (uint8_t)message->GetElement(7);

        switch (controllerIndex)
        {
    #if STM32_CAN_USE_CAN1
            case 1:
                canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, TIME_MS2I(100));
                break;
    #endif
    #if STM32_CAN_USE_CAN2
            case 2:
                canTransmit(&CAND2, CAN_ANY_MAILBOX, &txmsg, TIME_MS2I(100));
                break;
    #endif
            default:
                // this CAN bus is not valid
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_devices_can_native_nanoFramework_Devices_Can_CanController::WriteMessage___VOID__U4__BOOLEAN__BOOLEAN__SZARRAY_U2( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // NF_PAL_CAN* palCan = NULL;
        uint32_t id;
        bool isSID, isDataFrame;
        CLR_RT_HeapBlock_Array* message;
        CLR_UINT8* msgArray;
        CANTxFrame txmsg;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
        // get controller index
        uint8_t controllerIndex = (uint8_t)(pThis[ FIELD___controllerId ].NumericByRef().s4);

    //     switch (controllerIndex)
    //     {
    // #if STM32_CAN_USE_CAN1
    //         case 1:
    //             palCan = &Can1_PAL;
    //             break;
    // #endif
    // #if STM32_CAN_USE_CAN2
    //         case 2:
    //             palCan = &Can2_PAL;
    //             break;
    // #endif
    //         default:
    //             // this CAN bus is not valid
    //             NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    //             break;
    //     }

        // dereference tx parameters
        id = stack.Arg0().NumericByRef().u4;
        isSID = stack.Arg1().NumericByRefConst().u1 != 0;
        isDataFrame = stack.Arg2().NumericByRefConst().u1 != 0;
        message = stack.Arg3().DereferenceArray();
        msgArray = message->GetFirstElement();

        if(isSID)
        {
            txmsg.IDE = CAN_IDE_STD;
            txmsg.SID = id;
        }
        else
        {
            txmsg.IDE = CAN_IDE_EXT;
            txmsg.EID = id;
        }
        txmsg.RTR = isDataFrame ? CAN_RTR_DATA : CAN_RTR_REMOTE;
        txmsg.DLC = 4;
        memcpy(&msgArray[0], &txmsg.data16[0], 4);
        // txmsg.data16[0] = (uint16_t)message->GetElementUInt16(0);
        // txmsg.data16[1] = (uint16_t)message->GetElementUInt16(1);
        // txmsg.data16[2] = (uint16_t)message->GetElementUInt16(2);
        // txmsg.data16[3] = (uint16_t)message->GetElementUInt16(3);

        switch (controllerIndex)
        {
    #if STM32_CAN_USE_CAN1
            case 1:
                canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, TIME_MS2I(100));
                break;
    #endif
    #if STM32_CAN_USE_CAN2
            case 2:
                canTransmit(&CAND2, CAN_ANY_MAILBOX, &txmsg, TIME_MS2I(100));
                break;
    #endif
            default:
                // this CAN bus is not valid
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_devices_can_native_nanoFramework_Devices_Can_CanController::WriteMessage___VOID__U4__BOOLEAN__BOOLEAN__SZARRAY_U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // NF_PAL_CAN* palCan = NULL;
        uint32_t id;
        bool isSID, isDataFrame;
        CLR_RT_HeapBlock_Array* message;
        CLR_UINT8* msgArray;
        CANTxFrame txmsg;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
        // get controller index
        uint8_t controllerIndex = (uint8_t)(pThis[ FIELD___controllerId ].NumericByRef().s4);

    //     switch (controllerIndex)
    //     {
    // #if STM32_CAN_USE_CAN1
    //         case 1:
    //             palCan = &Can1_PAL;
    //             break;
    // #endif
    // #if STM32_CAN_USE_CAN2
    //         case 2:
    //             palCan = &Can2_PAL;
    //             break;
    // #endif
    //         default:
    //             // this CAN bus is not valid
    //             NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    //             break;
    //     }

        // dereference tx parameters
        id = stack.Arg0().NumericByRef().u4;
        isSID = stack.Arg1().NumericByRefConst().u1 != 0;
        isDataFrame = stack.Arg2().NumericByRefConst().u1 != 0;
        message = stack.Arg3().DereferenceArray();
        msgArray = message->GetFirstElement();

        if(isSID)
        {
            txmsg.IDE = CAN_IDE_STD;
            txmsg.SID = id;
        }
        else
        {
            txmsg.IDE = CAN_IDE_EXT;
            txmsg.EID = id;
        }
        txmsg.RTR = isDataFrame ? CAN_RTR_DATA : CAN_RTR_REMOTE;
        txmsg.DLC = 2;
        memcpy(&msgArray[0], &txmsg.data32[0], 2);
        // txmsg.data32[0] = message->GetAtomicDataUsedBytes();
        // txmsg.data32[1] = message->GetAtomicDataUsedBytes();

        switch (controllerIndex)
        {
    #if STM32_CAN_USE_CAN1
            case 1:
                canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, TIME_MS2I(100));
                break;
    #endif
    #if STM32_CAN_USE_CAN2
            case 2:
                canTransmit(&CAND2, CAN_ANY_MAILBOX, &txmsg, TIME_MS2I(100));
                break;
    #endif
            default:
                // this CAN bus is not valid
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_devices_can_native_nanoFramework_Devices_Can_CanController::WriteMessage___VOID__U4__BOOLEAN__BOOLEAN__U8( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // NF_PAL_CAN* palCan = NULL;
        uint32_t id;
        bool isSID, isDataFrame;
        uint64_t message;
        CANTxFrame txmsg;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
        // get controller index
        uint8_t controllerIndex = (uint8_t)(pThis[ FIELD___controllerId ].NumericByRef().s4);

    //     switch (controllerIndex)
    //     {
    // #if STM32_CAN_USE_CAN1
    //         case 1:
    //             palCan = &Can1_PAL;
    //             break;
    // #endif
    // #if STM32_CAN_USE_CAN2
    //         case 2:
    //             palCan = &Can2_PAL;
    //             break;
    // #endif
    //         default:
    //             // this CAN bus is not valid
    //             NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    //             break;
    //     }

        // dereference tx parameters
        id = stack.Arg0().NumericByRef().u4;
        isSID = stack.Arg1().NumericByRefConst().u1 != 0;
        isDataFrame = stack.Arg2().NumericByRefConst().u1 != 0;
        message = stack.Arg0().NumericByRef().s8;

        if(isSID)
        {
            txmsg.IDE = CAN_IDE_STD;
            txmsg.SID = id;
        }
        else
        {
            txmsg.IDE = CAN_IDE_EXT;
            txmsg.EID = id;
        }
        txmsg.RTR = isDataFrame ? CAN_RTR_DATA : CAN_RTR_REMOTE;
        txmsg.DLC = 1;
        txmsg.data64[0] = message;

        switch (controllerIndex)
        {
    #if STM32_CAN_USE_CAN1
            case 1:
                canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, TIME_MS2I(100));
                break;
    #endif
    #if STM32_CAN_USE_CAN2
            case 2:
                canTransmit(&CAND2, CAN_ANY_MAILBOX, &txmsg, TIME_MS2I(100));
                break;
    #endif
            default:
                // this CAN bus is not valid
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }
    }

    NANOCLR_NOCLEANUP();
}
*/
HRESULT Library_nf_devices_can_native_nanoFramework_Devices_Can_CanController::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    (void)stack;

    NANOCLR_HEADER();
    {

    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_devices_can_native_nanoFramework_Devices_Can_CanController::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
        // get a pointer to the managed spi connectionSettings object instance
        CLR_RT_HeapBlock* pConfig = pThis[ FIELD___settings ].Dereference();
        
        // get controller index
        uint8_t controllerIndex = (uint8_t)(pThis[ FIELD___controllerId ].NumericByRef().s4);

        // init the PAL struct for this CAN bus and assign the respective driver
        // all this occurs if not already done
        switch (controllerIndex)
        {
    #if STM32_CAN_USE_CAN1
            case 1:
                Can1_PAL.Driver = &CAND1;
                // palCan = &Can1_PAL;

                // build CAN config
                Can1_PAL.Configuration = {
                    CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
                    CAN_BTR_LBKM | 
                    CAN_BTR_SJW(pConfig[ CanSettings::FIELD___syncJumpWidth ].NumericByRef().u4) | 
                    CAN_BTR_TS2(pConfig[ CanSettings::FIELD___phaseSegment2 ].NumericByRef().u4) |
                    CAN_BTR_TS1(pConfig[ CanSettings::FIELD___phaseSegment1 ].NumericByRef().u4) | 
                    CAN_BTR_BRP(pConfig[ CanSettings::FIELD___baudRatePrescaler ].NumericByRef().u4)
                    };

                canStart(&CAND1, &Can1_PAL.Configuration);
                break;
    #endif
    #if STM32_CAN_USE_CAN2
            case 2:
                Can2_PAL.Driver = &CAND2;
                // palCan = &Can2_PAL;

                // build CAN config
                Can2_PAL.Configuration = {
                    CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
                    CAN_BTR_LBKM | 
                    CAN_BTR_SJW(pConfig[ CanSettings::FIELD___syncJumpWidth ].NumericByRef().u4) | 
                    CAN_BTR_TS2(pConfig[ CanSettings::FIELD___phaseSegment2 ].NumericByRef().u4) |
                    CAN_BTR_TS1(pConfig[ CanSettings::FIELD___phaseSegment1 ].NumericByRef().u4) | 
                    CAN_BTR_BRP(pConfig[ CanSettings::FIELD___baudRatePrescaler ].NumericByRef().u4)
                    };

                canStart(&CAND2, &Can2_PAL.Configuration);
                break;
    #endif
            default:
                // this CAN bus is not valid
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_devices_can_native_nanoFramework_Devices_Can_CanController::NativeUpdateCallbacks___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NF_PAL_CAN* palCan = NULL;
    bool callbacksRegistered;
    uint8_t controllerIndex;
    msg_t dummy;

    CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

    // get controller index
    controllerIndex = (uint8_t)(pThis[ FIELD___controllerId ].NumericByRef().s4);

    // flag to determine if there are any callbacks registered in managed code
    callbacksRegistered = (pThis[ FIELD___callbacks ].Dereference() != NULL);

    switch (controllerIndex)
    {
      #if STM32_CAN_USE_CAN1
        case 1:
            palCan = &Can1_PAL;
        
            if(callbacksRegistered)
            {
                // there is someone listening on the managed end

                // create rx thread
                palCan->ReceiverThread = chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(256),
                                            "CAN1RT", NORMALPRIO, CanReceiverThread, (void *)&palCan);
            }
            else
            {
                // no one listening, OK to remove

                // send terminating flag to thread
                chThdTerminate(palCan->ReceiverThread);

                // ChibiOS requirement: need to call chThdWait for working thread in order to have it's memory released to the heap, otherwise it won't be returned
                dummy = chThdWait(palCan->ReceiverThread);
                (void)dummy;

                // null thread pointer
                palCan->ReceiverThread = NULL;
            }

            break;
      #endif
      #if STM32_CAN_USE_CAN2
        case 2:
            palCan = &Can2_PAL;
        
            if(callbacksRegistered)
            {
                // there is someone listening on the managed end

                // create rx thread
                palCan->ReceiverThread = chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(256),
                                            "CAN2RT", NORMALPRIO, CanReceiverThread, (void *)&palCan);
            }
            else
            {
                // no one listening, OK to remove

                // send terminating flag to thread
                chThdTerminate(palCan->ReceiverThread);

                // ChibiOS requirement: need to call chThdWait for working thread in order to have it's memory released to the heap, otherwise it won't be returned
                dummy = chThdWait(palCan->ReceiverThread);
                (void)dummy;

                // null thread pointer
                palCan->ReceiverThread = NULL;
            }

            break;
      #endif
        default:
            // this CAN bus is not valid
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_devices_can_native_nanoFramework_Devices_Can_CanController::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
   NANOCLR_HEADER();
   {
       // declare the device selector string whose max size is "SPI1,SPI2,SPI3,SPI4,SPI5,SPI6," + terminator and init with the terminator
       char deviceSelectorString[ 30 + 1] = { 0 };

   #if STM32_CAN_USE_CAN1
       strcat(deviceSelectorString, "CAN1,");
   #endif
   #if STM32_CAN_USE_CAN2
       strcat(deviceSelectorString, "CAN2,");
   #endif

       // replace the last comma with a terminator
       deviceSelectorString[hal_strlen_s(deviceSelectorString) - 1] = '\0';

       // because the caller is expecting a result to be returned
       // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
       stack.SetResult_String(deviceSelectorString);
   }
   NANOCLR_NOCLEANUP_NOLABEL();
}
