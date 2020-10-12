//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_devices_can_native_target.h"

// define these types here to make it shorter and improve code readability
typedef Library_nf_devices_can_native_nanoFramework_Devices_Can_CanSettings CanSettings;
typedef Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessage ManagedCanMessage;

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
    #if STM32_CAN_USE_CAN2
    if (canp == &CAND2)
    {
        palCan = &Can2_PAL;
        controllerId = 2;
    }
    #endif
    #if STM32_CAN_USE_CAN3
    if (canp == &CAND3)
    {
        palCan = &Can3_PAL;
        controllerId = 3;
    }
    #endif
  
    while (canReceiveTimeout(palCan->Driver, CAN_ANY_MAILBOX, &rxMsg, TIME_IMMEDIATE) == MSG_OK)
    {
        // got message

        // store message in the CAN buffer
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
#if STM32_CAN_USE_CAN3
    NF_PAL_CAN Can3_PAL;
#endif

HRESULT Library_nf_devices_can_native_nanoFramework_Devices_Can_CanController::WriteMessage___VOID__nanoFrameworkDevicesCanCanMessage( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        uint32_t id;
        CanMessageIdType msgIdType;
        CanMessageFrameType msgFrameType;
        CLR_RT_HeapBlock_Array* message;
        CANTxFrame txmsg;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        CLR_RT_HeapBlock* pMessage = stack.Arg1().Dereference();
        
        // get controller index
        uint8_t controllerIndex = (uint8_t)(pThis[ FIELD___controllerId ].NumericByRef().s4);

        // dereference message fields
        id = (uint32_t)(pMessage[ Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessage::FIELD___id ].NumericByRef().u4);
        msgIdType = (CanMessageIdType)(pMessage[ Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessage::FIELD___identifierType ].NumericByRefConst().u1);
        msgFrameType = (CanMessageFrameType)(pMessage[ Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessage::FIELD___frameType ].NumericByRefConst().u1);

        // get message to transmite
        message = pMessage[ Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessage::FIELD___message ].DereferenceArray();

        // message max length is 8 bytes
        if(message->m_numOfElements > 8) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);

        // compose the transmite packet to send
        if(message != NULL)
        {
            // copy message to structure
            memcpy(txmsg.data8, (uint8_t*)message->GetFirstElement(), message->m_numOfElements);
        }
        // message id according to it's type
        if(msgIdType == CanMessageIdType_SID)
        {
            txmsg.IDE = CAN_IDE_STD;
            txmsg.SID = id;
        }
        else
        {
            txmsg.IDE = CAN_IDE_EXT;
            txmsg.EID = id;
        }
        // remote transmission request
        txmsg.RTR = msgFrameType == CanMessageFrameType_Data ? CAN_RTR_DATA : CAN_RTR_REMOTE;
        // data length
        txmsg.DLC = message->m_numOfElements;

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
    #if STM32_CAN_USE_CAN3
            case 3:
                canTransmit(Can3_PAL.Driver, CAN_ANY_MAILBOX, &txmsg, TIME_MS2I(100));
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

      #if STM32_CAN_USE_CAN3
        case 3:
            palCan = &Can3_PAL;

            break;
      #endif

        default:
            // this CAN bus is not valid
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
    }

    // get next CAN frame from ring buffer
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
        CLR_RT_HeapBlock&  identifierTypeFieldRef = canMessage[ ManagedCanMessage::FIELD___identifierType ];
        CLR_RT_HeapBlock&  frameTypeFieldRef = canMessage[ ManagedCanMessage::FIELD___frameType ];

        // get message frame type
        uint8_t* fr = (uint8_t*)&frameTypeFieldRef.NumericByRef().u1;
        *fr = canFrame.RTR;

        // get message id type
        uint8_t* idType = (uint8_t*)&identifierTypeFieldRef.NumericByRef().u1;
        *idType = canFrame.IDE;

        // get message id
        CLR_RT_HeapBlock&  idFieldRef = canMessage[ ManagedCanMessage::FIELD___id ];
        uint32_t* id = (uint32_t*)&idFieldRef.NumericByRef().u4;

        if(canFrame.IDE == 0)
        {
            *id = canFrame.SID;
        }
        else
        {
            *id = canFrame.EID;
        }

        // get data if any
        if(canFrame.data8)
        {
            CLR_RT_HeapBlock& dataArrayField = canMessage[ ManagedCanMessage::FIELD___message ];
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

HRESULT Library_nf_devices_can_native_nanoFramework_Devices_Can_CanController::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    uint8_t controllerIndex;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // get controller index
    controllerIndex = (uint8_t)(pThis[ FIELD___controllerId ].NumericByRef().s4);

    // init the PAL struct for this CAN bus and assign the respective driver
    // all this occurs if not already done
    switch (controllerIndex)
    {
      #if STM32_CAN_USE_CAN1
        case 1:
            Can1_PAL.Driver = NULL;
            // stop CAN
            canStop(&CAND1);
            break;
      #endif

      #if STM32_CAN_USE_CAN2
        case 2:
            Can2_PAL.Driver = NULL;
            // stop CAN
            canStop(&CAND2);
            break;
      #endif

      #if STM32_CAN_USE_CAN3
        case 3:
            Can3_PAL.Driver = NULL;
            // stop CAN
            canStop(&CAND3);
            break;
      #endif

        default:
            // this CAN bus is not valid
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
    }

    NANOCLR_NOCLEANUP();
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
                break;
    #endif
    #if STM32_CAN_USE_CAN2
            case 2:
                Init_Can2();
                ConfigPins_CAN2();
                Can2_PAL.Driver = &CAND2;
                palCan = &Can2_PAL;
                break;
    #endif
    #if STM32_CAN_USE_CAN3
            case 3:
                Init_Can3();
                ConfigPins_CAN3();
                Can3_PAL.Driver = &CAND3;
                palCan = &Can3_PAL;
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
            break;
      #endif
      #if STM32_CAN_USE_CAN2
        case 2:
            palCan = &Can2_PAL;
            break;
      #endif
      #if STM32_CAN_USE_CAN3
        case 3:
            palCan = &Can3_PAL;
            break;
      #endif
        default:
            // this CAN bus is not valid
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
    }

    // need to start CAN again
    // it's safe to stop first
    canStop(palCan->Driver);

    if(callbacksRegistered)
    {
        // there is someone listening on the managed end
        palCan->Driver->rxfull_cb = RxMessage;
    }
    else
    {
        // no one listening, OK to remove
        palCan->Driver->rxfull_cb = NULL;
    }

    // start CAN
    canStart(palCan->Driver, &palCan->Configuration);


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
   #if STM32_CAN_USE_CAN3
       strcat(deviceSelectorString, "CAN3,");
   #endif

       // replace the last comma with a terminator
       deviceSelectorString[hal_strlen_s(deviceSelectorString) - 1] = '\0';

       // because the caller is expecting a result to be returned
       // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
       stack.SetResult_String(deviceSelectorString);
   }
   NANOCLR_NOCLEANUP_NOLABEL();
}
