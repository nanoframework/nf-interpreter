//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "FreeRTOS.h"
#include <nanoHAL.h>
#include "win_dev_serial_native_target.h"

#define LPUART_TX_DMA_CHANNEL 0U

static void Write_Event( void * pvParameters );
static void Read_Event( void * pvParameters );

LPUART_Type *const lpuart_bases[] = LPUART_BASE_PTRS;

const size_t xStreamBufferSizeBytes = UART_RX_BUFER_SIZE, xTriggerLevel = 10;

/////////////////////////////////////////////////////////
// UART PAL strucs delcared in win_dev_serial_native.h //
/////////////////////////////////////////////////////////

NF_PAL_UART Uart_PAL1;
NF_PAL_UART Uart_PAL2;
NF_PAL_UART Uart_PAL3;
NF_PAL_UART Uart_PAL4;
NF_PAL_UART Uart_PAL5;
NF_PAL_UART Uart_PAL6;
NF_PAL_UART Uart_PAL7;
NF_PAL_UART Uart_PAL8;

NF_PAL_UART* Uart_PAL[] {
    NULL,
    &Uart_PAL1,
    &Uart_PAL2,
    &Uart_PAL3,
    &Uart_PAL4,
    &Uart_PAL5,
    &Uart_PAL6,
    &Uart_PAL7,
    &Uart_PAL8
};

// 
// Interrupt handler invoked when a transmission buffer is completely transfered by DMA
// 
static void TxEnd(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status, void *data) 
{   
    (void) base;
    (void) handle;
    (void) status;

    uint8_t uartNum = *(uint8_t *) data;

    NATIVE_INTERRUPT_START     

    // pop elements from ring buffer, just pop
    Uart_PAL[uartNum]->TxRingBuffer.Pop(Uart_PAL[uartNum]->TxOngoingCount);

    // reset Tx ongoing count
    Uart_PAL[uartNum]->TxOngoingCount = 0;

    // At this point xTaskToNotify should not be NULL as a transmission was in progress.
    configASSERT( Uart_PAL[uartNum]->xWriteTaskToNotify != NULL );

    // Notify the task that the transmission is complete.
    vTaskNotifyGiveFromISR( Uart_PAL[uartNum]->xWriteTaskToNotify, NULL );

     // At this point xTaskToNotify should not be NULL as a transmission was in progress.
    configASSERT( Uart_PAL[uartNum]->xReadTaskToNotify != NULL );

    // Notify the task that the transmission is complete.
    vTaskNotifyGiveFromISR( Uart_PAL[uartNum]->xReadTaskToNotify, NULL );
    
    NATIVE_INTERRUPT_END
}

static void RX_Handle(LPUART_Type *base)
{
    NATIVE_INTERRUPT_START

    uint8_t * uartNum = (uint8_t *) base;
    char byte = LPUART_ReadByte(base);

    if (!xStreamBufferIsFull( Uart_PAL[*uartNum]->xReceiveBuffer ))
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE; /* Initialised to pdFALSE. */
        xStreamBufferSendFromISR( Uart_PAL[*uartNum]->xReceiveBuffer,
                                &byte,
                                sizeof(byte),
                                &xHigherPriorityTaskWoken );
    }   
         
    NATIVE_INTERRUPT_END
}

/*
    Override of the default MIMXRT1060 UART interrupt routines to simple RX_Handle function, which 
    reads 1 byte of input data to RTOS stream buffer.
*/
extern "C"
{
/* LPUART1 is used for debugging, disable it or will collide with debugger */
/* void LPUART1_IRQHandler(void)
{
    RX_Handle(LPUART1);
}
*/
void LPUART2_IRQHandler(void)
{
    RX_Handle(LPUART2);
}
void LPUART3_IRQHandler(void)
{
    RX_Handle(LPUART3);
}
void LPUART4_IRQHandler(void)
{
    RX_Handle(LPUART4);
}
 void LPUART5_IRQHandler(void)
{
    RX_Handle(LPUART5);
}
void LPUART6_IRQHandler(void)
{
    RX_Handle(LPUART6);
}
void LPUART7_IRQHandler(void)
{
    RX_Handle(LPUART7);
}
void LPUART8_IRQHandler(void)
{
    RX_Handle(LPUART8);
}
}

//
//  Deinitialize serial port and allocated free memory
//
HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeDispose___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        uint8_t uartNum = 0;
        LPUART_Type *base = NULL;

        uartNum = pThis[ FIELD___portIndex ].NumericByRef().s4;
        base = lpuart_bases[uartNum];

        /* Quit if parameters or device is invalid or out of range */
        if (base == NULL || uartNum > 8) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER); 

        /* Free ring buffers memory */
        free(Uart_PAL[uartNum]->TxBuffer);

        /* Deinitialize device and delete FreeRTOS idle tasks */
        LPUART_Deinit(base);   
        vTaskDelete( Uart_PAL[uartNum]->xWriteTaskToNotify );
        vTaskDelete( Uart_PAL[uartNum]->xReadTaskToNotify );
    }
    NANOCLR_NOCLEANUP();
}

//
//  Initialise a new Serial port, allocate buffer memory and create FreeRTOS idle tasks
//
HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {   
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        status_t status = 0;
        uint8_t uartNum = 0;
        lpuart_config_t *config = NULL;
        LPUART_Type *base = NULL;
        BaseType_t xReturned = NULL;
        
        uartNum = pThis[ FIELD___portIndex ].NumericByRef().s4;
        base = lpuart_bases[uartNum];        

        /* Quit if parameters or device is invalid or out of range */
        if (base == NULL || uartNum > 8) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);  

        /* Allocate memory for TX and RX circular buffers */
        Uart_PAL[uartNum]->TxBuffer = (uint8_t *) platform_malloc(UART_TX_BUFER_SIZE * sizeof(uint8_t));
        if (Uart_PAL[uartNum]->TxBuffer == NULL) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);

        /* Initialize buffers  */
        Uart_PAL[uartNum]->TxRingBuffer.Initialize(Uart_PAL[uartNum]->TxBuffer, UART_TX_BUFER_SIZE);
        Uart_PAL[uartNum]->TxOngoingCount = 0;
        Uart_PAL[uartNum]->WatchChar = 0;
    
        /* Get default config structure for initializing given UART peripheral and enable TX, RX */
        LPUART_GetDefaultConfig(config);
        config->enableRx = true;
        config->enableTx = true;

        /* Set lower priority of DMA UART interrupt for FreeRTOS interrupt task to work */
        NVIC_SetPriority(DMA0_DMA16_IRQn, 10U);

        /* Initialize UART peripheral with default config */
        status = LPUART_Init(base, config, GetSrcFreq());   
        if (status != kStatus_Success) NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        
        /* Enable RX interrupts */
        LPUART_EnableInterrupts(base, kLPUART_RxDataRegFullInterruptEnable | kLPUART_RxOverrunInterruptEnable);
        EnableIRQ((IRQn_Type) (19 + uartNum));
        NVIC_SetPriority((IRQn_Type) (19 + uartNum), 8U);
        
        /* Initalize DMAMUX and setup channel for LPUART */
        /* TODO: Implement different mux than LPUART3 */
        DMAMUX_Init(DMAMUX);
        DMAMUX_SetSource(DMAMUX, LPUART_TX_DMA_CHANNEL, kDmaRequestMuxLPUART3Tx);
        DMAMUX_EnableChannel(DMAMUX, LPUART_TX_DMA_CHANNEL);        
        // DMAMUX_SetSource(DMAMUX, LPUART_RX_DMA_CHANNEL, kDmaRequestMuxLPUART3Rx);
        // DMAMUX_EnableChannel(DMAMUX, LPUART_RX_DMA_CHANNEL);

        /* Initialize DMA with default config and setup callback handles */
        /* TODO: Implement different DMA channels */
        EDMA_GetDefaultConfig(&Uart_PAL[uartNum]->edmaCfg);
        EDMA_Init(DMA0, &Uart_PAL[uartNum]->edmaCfg);
        EDMA_CreateHandle(&Uart_PAL[uartNum]->g_lpuartTxEdmaHandle, DMA0, LPUART_TX_DMA_CHANNEL);
        // EDMA_CreateHandle(&Uart_PAL[uartNum]->g_lpuartRxEdmaHandle, DMA0, LPUART_RX_DMA_CHANNEL);      

        /* FreeRTOS Task needs parameter data survive after this function finish, so write this parameter to Uart_PAL structure. */
        Uart_PAL[uartNum]->dma_num = uartNum;

        /* UART DMA TX finished handle */
         LPUART_TransferCreateHandleEDMA(base,
                                        &Uart_PAL[uartNum]->g_lpuartEdmaHandle, 
                                        TxEnd, 
                                        &Uart_PAL[uartNum]->dma_num, 
                                        &Uart_PAL[uartNum]->g_lpuartTxEdmaHandle, 
                                        NULL);

        /* Create idle task for setting write finished event */
        xReturned = xTaskCreate(Write_Event,       
                                "UART Write Event",          
                                configMINIMAL_STACK_SIZE + 10,
                                (void *) &Uart_PAL[uartNum]->dma_num,
                                tskIDLE_PRIORITY + 3,
                                &Uart_PAL[uartNum]->xWriteTaskToNotify );
        if (xReturned == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);    
        /* Allocate stream buffer for RX  */
        Uart_PAL[uartNum]->xReceiveBuffer = xStreamBufferCreate(xStreamBufferSizeBytes, xTriggerLevel);
        if( Uart_PAL[uartNum]->xReceiveBuffer == NULL ) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY); 
    }
    NANOCLR_NOCLEANUP(); 
}

//
//  Set up serial port Configuration
//
HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeConfig___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {        
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        status_t status = 0;
        uint8_t uartNum = 0;
        lpuart_config_t *config = NULL;
        LPUART_Type *base = NULL;        

        uartNum = pThis[ FIELD___portIndex ].NumericByRef().s4;
        config = &Uart_PAL[uartNum]->uartCfg;
        base = lpuart_bases[uartNum];

        if (config == NULL || base == NULL || uartNum > 8) NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);

        config->baudRate_Bps = (uint32_t)pThis[ FIELD___baudRate ].NumericByRef().s4;

        switch( pThis[ FIELD___dataBits ].NumericByRef().s4 )
        {
            default:
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER); break;
            case 7:
                config->dataBitsCount = kLPUART_SevenDataBits; break;
            case 8:
                config->dataBitsCount = kLPUART_EightDataBits; break;
        }
        
        switch ( pThis[ FIELD___parity ].NumericByRef().s4 )
        {
            default:
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER); break;
            case SerialParity_None:
                config->parityMode = kLPUART_ParityDisabled; break;
            case SerialParity_Even:
                config->parityMode = kLPUART_ParityEven; break;
            case SerialParity_Odd:
                config->parityMode = kLPUART_ParityOdd; break;
        }

        // TODO: Implement the rest of handshake
        switch ( pThis[ FIELD___handshake ].NumericByRef().s4 )
        {
            default:
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER); break;
            case SerialHandshake_None:
                config->enableRxRTS = false;
                config->enableTxCTS = false;
                break;
            case SerialHandshake_RequestToSend:
                config->enableRxRTS = true;
                config->enableTxCTS = false;
                break;
            //case SerialHandshake_RequestToSendXOnXOff:
        }

        // write config to UART peripheral
        status = LPUART_Init(base, config, GetSrcFreq());     
        if (status != kStatus_Success) NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        pThis = NULL;

        /* Reenable interrupts after setting configuration */
        /* Disable transmitter */
        base->CTRL &= ~(1U << 19);
        /* Disable receiver */
        base->CTRL &= ~(1U << 18);
        /* Enable receiver interrupt */        
        base->CTRL |= 1U << 21; 
        /* Enable overrun interrupt */
        // base->CTRL |= 1U << 27; 
        /* Renable receiver and transmitter */
        base->CTRL |= 1U << 19;  
        base->CTRL |=  1U << 18;     
    }
    NANOCLR_NOCLEANUP();
}

//
//  Write data into buffer
//
HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeWrite___VOID__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);      

        uint8_t* data;
        unsigned int length = 0;

        uint8_t uartNum = 0;
        
        if(pThis[ FIELD___disposed ].NumericByRef().u1 != 0) NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        
        /* Get UART device number */
        uartNum = pThis[ FIELD___portIndex ].NumericByRef().s4;

        /* Quit if parameters or device is invalid or out of range */
        if (uartNum > 8) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER); 

        // dereference the data buffer from the argument
        CLR_RT_HeapBlock_Array* dataBuffer = stack.Arg1().DereferenceArray();

        // get a  the pointer to the array by using the first element of the array
        data =dataBuffer->GetFirstElement();

        // get the size of the buffer
        length = dataBuffer->m_numOfElements;

         // check if there is enough room in the buffer
        if(Uart_PAL[uartNum]->TxRingBuffer.Capacity() - Uart_PAL[uartNum]->TxRingBuffer.Length() < length)
        {
            // not enough room in the buffer
             NANOCLR_SET_AND_LEAVE(CLR_E_BUFFER_TOO_SMALL);
        }

        // push data to buffer
        size_t bytesWritten = Uart_PAL[uartNum]->TxRingBuffer.Push(data, length);

        // check if all requested bytes were written
        if(bytesWritten != length)
        {
            // not sure if this is the best exception to throw here...
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        
        // need to update the _unstoredBufferLength field in the SerialDeviceOutputStream
        // get pointer to outputStream field
        CLR_RT_HeapBlock* outputStream = pThis[Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::FIELD___outputStream].Dereference();
        // get pointer to _unstoredBufferLength field and udpate field value
        outputStream[Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDeviceOutputStream::FIELD___unstoredBufferLength].NumericByRef().s4 = Uart_PAL[uartNum]->TxRingBuffer.Length();

        // null pointers and vars
        pThis = NULL;   
    }
    NANOCLR_NOCLEANUP();
}

//
// Store - Send buffer and wait
//
HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeStore___U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {        
        size_t length = 0;

        CLR_RT_HeapBlock* writeTimeout;
        int64_t*  timeoutTicks;
        bool eventResult = true;
        bool txOk = false;    

        uint8_t uartNum = NULL;
        lpuart_transfer_t xfer;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        uartNum = pThis[ FIELD___portIndex ].NumericByRef().s4;

        if(pThis[ FIELD___disposed ].NumericByRef().u1 != 0) NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);

        /* Quit if parameters or device is invalid or out of range */
        if (uartNum > 8) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

        // get value for _readtimeout field (pointer!)
        writeTimeout = &pThis[ Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::FIELD___writeTimeout ];

        // setup timeout
        NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(*writeTimeout, timeoutTicks));

        // push dummy length value onto the eval stack
        // this is going to be used to store how many bytes where buffered to Tx
        if(stack.m_customState == 1)
        {
            stack.PushValueI4(0);
            // bump custom state so the read value above is pushed only once
            stack.m_customState = 2;
        }

        // check if there is anything the buffer
        if(Uart_PAL[uartNum]->TxRingBuffer.Length() > 0)
        {
            // check if there is a TX operation ongoing
            if(Uart_PAL[uartNum]->TxOngoingCount == 0)
            {
                // OK to Tx
                txOk = true;
            }
            else
            {
                // need to wait for the ongoing operation to complete before starting a new one
            }
        }

        if(txOk)
        {
            /* Optimize buffer for sequential reading */
            Uart_PAL[uartNum]->TxRingBuffer.OptimizeSequence();

            /* Get data length available in the buffer */
            length = Uart_PAL[uartNum]->TxRingBuffer.Length();

            /* Push to the stack how many bytes bytes where buffered for Tx */
            stack.m_evalStack[1].NumericByRef().s4 = length;
            
            /* Set TX ongoing count */
            Uart_PAL[uartNum]->TxOngoingCount = length;

            /* Set up DMA transfer structure */
            xfer.data = (uint8_t*) Uart_PAL[uartNum]->TxRingBuffer.Reader();
            xfer.dataSize = length;
            
            /* Initiate DMA transfer to UART peripheral with xfer data structure */
            LPUART_SendEDMA(lpuart_bases[uartNum], &Uart_PAL[uartNum]->g_lpuartEdmaHandle, &xfer);
        }        

        while(eventResult)
        {
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, CLR_RT_ExecutionEngine::c_Event_SerialPortOut, eventResult));
            if(eventResult)
            {
                /* Event occurred, get from the eval stack how many bytes were buffered to Tx */
                length = stack.m_evalStack[1].NumericByRef().s4;

                // done here
                break;
            }
            else
            {
                NANOCLR_SET_AND_LEAVE( CLR_E_TIMEOUT );
            }
        }
        // pop length heap block from stack
        stack.PopValue();

        // pop timeout heap block from stack
        stack.PopValue();

        stack.SetResult_U4(length);

        NANOCLR_NOCLEANUP();
    }
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeRead___U4__SZARRAY_U1__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {        
        CLR_RT_HeapBlock_Array* dataBuffer = NULL;
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        CLR_RT_HeapBlock* readTimeout;
        bool eventResult = true;

        uint8_t* data = NULL;
        size_t read_count = 0;
        uint8_t uartNum = 0;
        
        BaseType_t xReturned = NULL;
        uart_data_t * rx_p = NULL;

        if(pThis[ FIELD___disposed ].NumericByRef().u1 != 0) NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);

        uartNum = pThis[ FIELD___portIndex ].NumericByRef().s4;
        
        /* Dereference the data buffer from the argument */
        dataBuffer = stack.Arg1().DereferenceArray();

        /* Get a the pointer to the array by using the first element of the array */
        data = dataBuffer->GetFirstElement();

        InputStreamOptions options = InputStreamOptions_None;

        /* Get how many bytes are requested to read */
        read_count = stack.Arg2().NumericByRef().s4;

        /* Get the length of the data buffer */
        Uart_PAL[uartNum]->uart_data.bytes_to_read = dataBuffer->m_numOfElements;

        /* Get the InputStreamOptions option */
        options = (InputStreamOptions)stack.Arg3().NumericByRef().s4;

        rx_p = &Uart_PAL[uartNum]->uart_data;
        
        xStreamBufferSetTriggerLevel( Uart_PAL[uartNum]->xReceiveBuffer,
                                    rx_p->bytes_to_read);

        /* Set up timeout */ 
        readTimeout = &pThis[ Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::FIELD___readTimeout ];
        NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks( *readTimeout, Uart_PAL[uartNum]->uart_data.timeoutTicks ));

         /* Task receiving data to xReadStream buffor */
        xReturned = xTaskCreate(
                    Read_Event,       
                    "UART Read Event",         
                    configMINIMAL_STACK_SIZE + 10,    
                    (void *) &Uart_PAL[uartNum]->uart_data,   
                    tskIDLE_PRIORITY + 3,
                    &Uart_PAL[uartNum]->xReadTaskToNotify );
        if (xReturned == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);

        /* Wait for event from task */
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *Uart_PAL[uartNum]->uart_data.timeoutTicks, CLR_RT_ExecutionEngine::c_Event_SerialPortIn, eventResult));
       
        if (eventResult) /* Event! */
        {
            if (Uart_PAL[uartNum]->uart_data.bytes_received == read_count) 
            {
                /* We received corrent amount of data, copy it to data (upper layer) */
                memcpy(data, Uart_PAL[uartNum]->uart_data.data, Uart_PAL[uartNum]->uart_data.bytes_received);           
            }

            else 
            {
                /* Not enough data */
            }
        }
        else
        {
            /* Timeout */
        }

        /* Return how many bytes were read */
        stack.SetResult_U4(Uart_PAL[uartNum]->uart_data.bytes_received);
    (void) options;
    }    
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeSetWatchChar___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
	{
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Choose the driver for this SerialDevice
        uint8_t uartNum = (uint8_t)pThis[ FIELD___portIndex ].NumericByRef().s4;

        // set watch char
        Uart_PAL[uartNum]                                                                                                                                                               ->WatchChar = (uint8_t)pThis[ FIELD___watchChar ].NumericByRef().u1;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::get_BytesToRead___U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();	
    {
       
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
       
        uint8_t uartNum = 0;
        size_t read_count = 0;
        
        if(pThis[ FIELD___disposed ].NumericByRef().u1 != 0) NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);

        uartNum = pThis[ FIELD___portIndex ].NumericByRef().s4;

        /* Check how many bytes are avaliable in buffer */
        read_count = xStreamBufferBytesAvailable( Uart_PAL[uartNum]->xReceiveBuffer );
        
        stack.SetResult_U4(read_count);
	}
	NANOCLR_NOCLEANUP();
}

//
// Return available devices
// TODO: implement checking for available devices
HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
   NANOCLR_HEADER();
   {
       /* declare the device selector string whose max size is "COM1,COM2,COM3,COM4,COM5,COM6,COM7,COM8" + terminator */
       char deviceSelectorString[ 40 ] = "COM1,COM2,COM3,COM4,COM5,COM6,COM7,COM8";

       /* because the caller is expecting a result to be returned */
       /* we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here) */
       stack.SetResult_String(deviceSelectorString);
   }
   NANOCLR_NOCLEANUP_NOLABEL();
}


/* Idle task, waiting for interrupt from peripheral device. When interrupt comes it fires SYSTEM_EVENT_FLAG_COM_OUT. */
static void Write_Event( void * pvParameters )
{
    uint8_t uartNum = *(uint8_t *) pvParameters;
    (void) uartNum;

    while(1) 
    {
        // non-blocking wait allowing other threads to run while we wait for the Tx operation to complete
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
        Events_Set( SYSTEM_EVENT_FLAG_COM_OUT );
    }
    vTaskDelete( NULL );
}

/* Idle task, waiting for interrupt from peripheral device. When interrupt comes it fires SYSTEM_EVENT_FLAG_COM_IN. */
static void Read_Event( void * pvParameters )
{
    uart_data_t * uart_data_p = (uart_data_t *) pvParameters;
    
    uart_data_p->bytes_received = xStreamBufferReceive( Uart_PAL[3]->xReceiveBuffer,
                                ( void * ) uart_data_p->data,
                                uart_data_p->bytes_to_read,
                                *Uart_PAL[3]->uart_data.timeoutTicks );
    
    Events_Set( SYSTEM_EVENT_FLAG_COM_IN );    
    vTaskDelete( NULL );
}