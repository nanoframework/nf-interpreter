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

// define these types here to make it shorter and improve code readability
typedef Library_nf_devices_can_native_nanoFramework_Devices_Can_CanSettings CanSettings;
typedef Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessage ManagedCanMessage;

// CAN receiver thread
// static THD_FUNCTION(CanReceiverThread, arg)
// {
//     NF_PAL_CAN* palCan = (NF_PAL_CAN*)arg;
//     event_listener_t eventListener;
//     CANRxFrame rxMsg;
//     int controllerId;

//   #if STM32_CAN_USE_CAN1
//         if(palCan->Driver == &CAND1)
//         {
//            controllerId = 1;
//         }
//   #endif
//   #if STM32_CAN_USE_CAN2
//         if(palCan->Driver == &CAND2)
//         {
//            controllerId = 2;
//         }
//   #endif

//     // set event listener for CAN RX
//     chEvtRegister(&palCan->Driver->rxfull_event, &eventListener, 0);
    
//     // wrap working code inside this call to process abort request with chThdTerminate()
//     while (!chThdShouldTerminateX())
//     {
//         if (chEvtWaitAnyTimeout(ALL_EVENTS, TIME_MS2I(100)) == 0)
//         {
//             continue;
//         }

        
//         while (canReceiveTimeout(palCan->Driver, CAN_ANY_MAILBOX, &rxMsg, TIME_IMMEDIATE) == MSG_OK)
//         {
//             // got message
            
//             // store message into the CAN buffer
//             // don't care about the success of the operation, if it's full we are droping the message anyway
//             palCan->MsgRingBuffer.Push(rxMsg);

//             // fire event for CAN message received
//             PostManagedEvent( EVENT_CAN, 0, controllerId, CanEvent_MessageReceived );
//        }
//     }

//     // unregister CAN RX event
//     chEvtUnregister(&palCan->Driver->rxfull_event, &eventListener);

//     // need to set thread exit code before leaving
//     chThdExit(MSG_OK);
// }

static void RxMessage(CANDriver *canp, uint32_t flags) 
{
    NATIVE_INTERRUPT_START

    NF_PAL_CAN* palCan;
    uint8_t controllerId = 0;
    CANRxFrame rxMsg;

    (void)flags;

    #if STM32_CAN_USE_CAN1
    if (canp == &CAND1)
    {
        palCan = &Can1_PAL;
        controllerId = 1;
    }
    #endif
    #if STM32_CAN_USE_CAN1
    if (canp == &CAND2)
    {
        palCan = &Can2_PAL;
        controllerId = 2;
    }
    #endif
  
    while (canReceiveTimeout(palCan->Driver, CAN_ANY_MAILBOX, &rxMsg, TIME_IMMEDIATE) == MSG_OK)
    {
        // got message
        
        // store message into the CAN buffer
        // don't care about the success of the operation, if it's full we are droping the message anyway
        palCan->MsgRingBuffer.Push(rxMsg);

        // fire event for CAN message received
        PostManagedEvent( EVENT_CAN, 0, controllerId, CanEvent_MessageReceived );
    }

    NATIVE_INTERRUPT_END
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

HRESULT Library_nf_devices_can_native_nanoFramework_Devices_Can_CanController::WriteMessage___VOID__nanoFrameworkDevicesCanCanMessage( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        uint32_t id;
        bool isSID, isDataFrame;
        CLR_RT_HeapBlock_Array* message;

        CANTxFrame txmsg;
        uint8_t msgLength = 0;
        uint8_t dataSize = 0;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        CLR_RT_HeapBlock* pMessage = stack.Arg1().Dereference();
        
        // get controller index
        uint8_t controllerIndex = (uint8_t)(pThis[ FIELD___controllerId ].NumericByRef().s4);

        // dereference message fields
        id = (uint32_t)(pMessage[ Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessage::FIELD___id ].NumericByRef().u4);
        isSID = (bool)(pMessage[ Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessage::FIELD___isSID ].NumericByRefConst().u1 != 0);
        isDataFrame = (bool)(pMessage[ Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessage::FIELD___isDataFrame ].NumericByRefConst().u1 != 0);

        // need to handle message data differently because it's one of the 4 possible types (byte, short, int, long)
        message = pMessage[ Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessage::FIELD___byteMessage ].DereferenceArray();

        if(message != NULL)
        {
            // BYTE -> 1 byte
            dataSize = 1;

            // get message lenght
            msgLength = dataSize * message->m_numOfElements;
            memcpy(txmsg.data8, (uint8_t*)message->GetFirstElement(), msgLength);
        }
        if(message == NULL)
        {
            message = pMessage[ Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessage::FIELD___uShortMessage ].DereferenceArray();
            // INT16 -> 2 bytes
            dataSize = 2;

            // get message lenght
            msgLength = dataSize * message->m_numOfElements;
            memcpy(txmsg.data16, (uint16_t*)message->GetFirstElement(), msgLength);
        }
        if(message == NULL)
        {
            message = pMessage[ Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessage::FIELD___uIntMessage ].DereferenceArray();
            // INT32 -> 4 bytes
            dataSize = 4;

            // get message lenght
            msgLength = dataSize * message->m_numOfElements;
            memcpy(txmsg.data32, (uint32_t*)message->GetFirstElement(), msgLength);
        }
        if(message == NULL)
        {
            message = pMessage[ Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessage::FIELD___uLongMessage ].DereferenceArray();
            // LONG -> 8 bytes
            dataSize = 8;

            // get message lenght
            msgLength = dataSize * message->m_numOfElements;
            memcpy(txmsg.data64, (uint64_t*)message->GetFirstElement(), msgLength);
        }
        
        // COMEMTS COMENTS + COMENTS
        FAULT_ON_NULL(message);

        
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

        switch (controllerIndex)
        {
    #if STM32_CAN_USE_CAN1
            case 1:
                canTransmit(Can1_PAL.Driver, CAN_ANY_MAILBOX, &txmsg, TIME_MS2I(100));
                break;
    #endif
    #if STM32_CAN_USE_CAN2
            case 2:
                canTransmit(Can2_PAL.Driver, CAN_ANY_MAILBOX, &txmsg, TIME_MS2I(100));
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

    CLR_RT_TypeDef_Index    canMessageTypeDef;
    CLR_RT_HeapBlock*       canMessage = NULL;
    // CLR_RT_HeapBlock*       hbObj = NULL;
    
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

    // create return value
    // stack.PushValue();

    messageCount = palCan->MsgRingBuffer.Pop(&canFrame, 1);

    // find <CanMessage> type, don't bother checking the result as the type exists for sure
    g_CLR_RT_TypeSystem.FindTypeDef( "CanMessage", "nanoFramework.Devices.Can", canMessageTypeDef );

    // create an instance of <StorageFolder>
    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(stack.PushValue(), canMessageTypeDef));

    canMessage = stack.TopValue().Dereference();

    if(messageCount == 1)
    {
        // we have a message
        
        // get pointer to each managed field and set appropriate value 
        CLR_RT_HeapBlock&  isSIDFieldRef = canMessage[ ManagedCanMessage::FIELD___isSID ];
        // CLR_INT64* pRes = (CLR_INT64*)&fieldRef.NumericByRef().u1;

        uint8_t* p = (uint8_t*)&isSIDFieldRef.NumericByRef().u1;
        *p = canFrame.IDE;

        CLR_RT_HeapBlock&  idFieldRef = canMessage[ ManagedCanMessage::FIELD___id ];
        uint32_t* p32 = (uint32_t*)&idFieldRef.NumericByRef().u4;

        if(canFrame.IDE == 0)
        {
            *p32 = canFrame.SID;
        }
        else
        {
            *p32 = canFrame.EID;
        }

        if(canFrame.data8)
        {
            CLR_RT_HeapBlock& dataArrayField = canMessage[ ManagedCanMessage::FIELD___byteMessage ];
            // create an array of <bytes>
            NANOCLR_CHECK_HRESULT( CLR_RT_HeapBlock_Array::CreateInstance( dataArrayField, 8, g_CLR_RT_WellKnownTypes.m_UInt8 ) );

            // get a pointer to the first object in the array
            CLR_UINT8* dataBuffer = (CLR_UINT8*)(dataArrayField.DereferenceArray()->GetFirstElement());
            memcpy(dataBuffer, &canFrame.data8[0], 8);
        }
    }
    else
    {
        // no more messages, return null
        stack.SetResult_Object(NULL);
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
        NF_PAL_CAN* palCan = NULL;

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
                Init_Can1();
                ConfigPins_CAN1();
                Can1_PAL.Driver = &CAND1;
                palCan = &Can1_PAL;
                palCan->Driver->rxfull_cb = RxMessage;
                break;
    #endif
    #if STM32_CAN_USE_CAN2
            case 2:
                Init_Can2();
                ConfigPins_CAN2();
                Can2_PAL.Driver = &CAND2;
                palCan = &Can2_PAL;
                palCan->Driver->rxfull_cb = RxMessage;
                break;
    #endif
            default:
                // this CAN bus is not valid
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }

        // get config
        palCan->Configuration = {
            CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
                (uint32_t)(
                CAN_BTR_SJW((uint8_t)pConfig[ CanSettings::FIELD___syncJumpWidth ].NumericByRef().u1) | 
                CAN_BTR_TS2((uint8_t)pConfig[ CanSettings::FIELD___phaseSegment2 ].NumericByRef().u1) |
                CAN_BTR_TS1((uint8_t)pConfig[ CanSettings::FIELD___phaseSegment1 ].NumericByRef().u1) | 
                CAN_BTR_BRP((uint8_t)pConfig[ CanSettings::FIELD___baudRatePrescaler ].NumericByRef().u1)
                )
            };

            // CAN_BTR_LBKM | 
        // start CAN
        canStart(palCan->Driver, &palCan->Configuration);
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
                // palCan->ReceiverThread = chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(256),
                //                             "CAN1RT", NORMALPRIO, CanReceiverThread, palCan);
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
                // palCan->ReceiverThread = chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(256),
                //                             "CAN2RT", NORMALPRIO, CanReceiverThread, palCan);
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
