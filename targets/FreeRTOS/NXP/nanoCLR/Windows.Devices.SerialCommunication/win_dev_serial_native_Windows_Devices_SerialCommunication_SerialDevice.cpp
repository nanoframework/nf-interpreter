//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "FreeRTOS.h"
#include <nanoHAL.h>
#include "win_dev_serial_native_target.h"



static void vRwEvent( void * pvParameters );

const size_t xStreamBufferSizeBytes = UART_RX_BUFER_SIZE;
const size_t xTriggerLevel = 10;

/////////////////////////////////////////////////////////
// UART PAL strucs delcared in win_dev_serial_native.h //
/////////////////////////////////////////////////////////

/* Static UART config/data structs. */
NF_PAL_UART Uart_PAL1;
NF_PAL_UART Uart_PAL2;
NF_PAL_UART Uart_PAL3;
NF_PAL_UART Uart_PAL4;
NF_PAL_UART Uart_PAL5;
NF_PAL_UART Uart_PAL6;
NF_PAL_UART Uart_PAL7;
NF_PAL_UART Uart_PAL8;

/* Array of pointers to above config UART structs. */
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

/* Interrupt handler invoked when a transmission buffer is completely transfered by DMA */ 
static void TxEnd(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status, void *data) 
{   
    (void) base;
    (void) handle;
    (void) status;

    uint8_t uartNum = *(uint8_t *) data;

    NATIVE_INTERRUPT_START     

    BaseType_t xHigherPriorityTaskWoken = pdTRUE;

    xTaskNotifyFromISR( Uart_PAL[uartNum]->xReadTaskToNotify, 0x01, eSetBits, &xHigherPriorityTaskWoken);
    
    /* At this point xTaskToNotify should not be NULL as a transmission was in progress. */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    
    NATIVE_INTERRUPT_END
}

/* Interrupt handler, reads one byte from UART and puts it in the xStreamBuffer. */
static void UART_Handle(LPUART_Type *base, uint8_t uartNum)
{
    uint32_t status = 0;
    char byte;
    BaseType_t xHigherPriorityTaskWoken;

    status = LPUART_GetStatusFlags(base);

        if (kLPUART_RxOverrunFlag & status)
        {
            /* Clear overrun flag, otherwise the RX does not work. */
            base->STAT = ((base->STAT & 0x3FE00000U) | LPUART_STAT_OR_MASK);
        }

        if (kLPUART_RxDataRegFullFlag & status)
        {
            if (!xStreamBufferIsFull( Uart_PAL[uartNum]->xReceiveBuffer ))
            {
                byte = LPUART_ReadByte(base);
                xHigherPriorityTaskWoken = pdFALSE; /* Initialised to pdFALSE. */
                xStreamBufferSendFromISR( Uart_PAL[uartNum]->xReceiveBuffer,
                                        &byte,
                                        sizeof(byte),
                                        &xHigherPriorityTaskWoken);
            }
        }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/*
    Override of the default MIMXRT1060 UART interrupt routines to simple UART_Handle function, which 
    reads 1 byte of input data to RTOS stream buffer, if theres buffer overflow it drops data and clears interrupts.
*/
extern "C"
{
    /* LPUART1 is currently used for debugging, disable it or will collide with debugger */
    /* void LPUART1_IRQHandler(void)
    {
        RX_Handle(LPUART1);
    }
    */
    void LPUART2_IRQHandler(void)
    {
        UART_Handle(LPUART2, 2);
    }
    void LPUART3_IRQHandler(void)
    {
        UART_Handle(LPUART3, 3);
    }
    void LPUART4_IRQHandler(void)
    {
        UART_Handle(LPUART4 ,4);
    }
    void LPUART5_IRQHandler(void)
    {
        UART_Handle(LPUART5, 5);
    }
    void LPUART6_IRQHandler(void)
    {
        UART_Handle(LPUART6, 6);
    }
    void LPUART7_IRQHandler(void)
    {
        UART_Handle(LPUART7, 7);
    }
    void LPUART8_IRQHandler(void)
    {
        UART_Handle(LPUART8, 8);
    }
}

/* Deinitialize serial port and allocated free memory */
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
        vTaskDelete( Uart_PAL[uartNum]->xReadTaskToNotify );
    }
    NANOCLR_NOCLEANUP();
}

/* Initialise a new Serial port, allocate buffer memory and create FreeRTOS idle tasks */
HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {   
       CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        status_t status = 0;
        uint8_t uartNum = 0;
        lpuart_config_t *config = NULL;
        BaseType_t xReturned = NULL;
        LPUART_Type *base = NULL;
        
        uartNum = pThis[ FIELD___portIndex ].NumericByRef().s4;
        Uart_PAL[uartNum]->uart_data.uartNum = uartNum;
        config = &Uart_PAL[uartNum]->uartCfg;
        base = lpuart_bases[uartNum];
        
        /* Quit if parameters or device is invalid or out of range */
        if (config == NULL || base == NULL || uartNum > 8) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);  

        /* Allocate memory for TX circular buffer */
        Uart_PAL[uartNum]->TxBuffer = (uint8_t *) platform_malloc(UART_TX_BUFER_SIZE * sizeof(uint8_t));
        if (Uart_PAL[uartNum]->TxBuffer == NULL) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);

        /* Initialize TX buffer */
        Uart_PAL[uartNum]->TxRingBuffer.Initialize(Uart_PAL[uartNum]->TxBuffer, UART_TX_BUFER_SIZE);
        Uart_PAL[uartNum]->TxOngoingCount = 0;
        Uart_PAL[uartNum]->WatchChar = 0;
        
        /* Allocate stream buffer for RX  */
        Uart_PAL[uartNum]->xReceiveBuffer = xStreamBufferCreate(xStreamBufferSizeBytes, xTriggerLevel);
        Uart_PAL[uartNum]->xRxSemaphore = xSemaphoreCreateMutex();
        
        /* Get default config structure for initializing given UART peripheral and enable TX, RX */
        LPUART_GetDefaultConfig(config);
        config->enableRx = true;
        config->enableTx = true;

        /* Initialize UART peripheral with default config */
        status = LPUART_Init(base, config, GetSrcFreq());   
        if (status != kStatus_Success) NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
               
        /* Enable RX interrupts */
        LPUART_EnableInterrupts(base, kLPUART_RxDataRegFullInterruptEnable | kLPUART_RxOverrunInterruptEnable);
        EnableIRQ((IRQn_Type) (19 + uartNum));
        NVIC_SetPriority((IRQn_Type) (19 + uartNum), 8U);
        
        /* Set lower priority of DMA UART interrupt for FreeRTOS interrupt task to work */
        NVIC_SetPriority(DMA0_DMA16_IRQn, 10U); 

        /* Initalize DMAMUX and setup channel for LPUART */
        /* TODO: Implement different mux than LPUART3 */
        DMAMUX_Init(DMAMUX);
        DMAMUX_SetSource(DMAMUX, LPUART_TX_DMA_CHANNEL, kDmaRequestMuxLPUART3Tx);
        DMAMUX_EnableChannel(DMAMUX, LPUART_TX_DMA_CHANNEL);        

        /* Initialize DMA with default config and setup callback handles */
        EDMA_GetDefaultConfig(&Uart_PAL[uartNum]->edmaCfg);
        EDMA_Init(DMA0, &Uart_PAL[uartNum]->edmaCfg);
        EDMA_CreateHandle(&Uart_PAL[uartNum]->g_lpuartTxEdmaHandle, DMA0, LPUART_TX_DMA_CHANNEL);
        
         /* FreeRTOS Task needs parameter data survive after this function finish, so write this parameter to Uart_PAL structure. */
        Uart_PAL[uartNum]->dma_num = uartNum;

        /* UART DMA TX finished handle */
         LPUART_TransferCreateHandleEDMA(base,
                                        &Uart_PAL[uartNum]->g_lpuartEdmaHandle, 
                                        TxEnd, 
                                        &Uart_PAL[uartNum]->dma_num, 
                                        &Uart_PAL[uartNum]->g_lpuartTxEdmaHandle, 
                                        NULL);

        /* Create idle task waiting for read/write. */
        xReturned = xTaskCreate(vRwEvent,       
                                "UART Read Event",         
                                configMINIMAL_STACK_SIZE + 10,    
                                (void *) Uart_PAL[uartNum],   
                                tskIDLE_PRIORITY + 5,
                                &Uart_PAL[uartNum]->xReadTaskToNotify );
        if (xReturned == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }
    NANOCLR_NOCLEANUP(); 
}

/* Set up serial port Configuration */
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

        /* TODO: Implement the rest of handshake */
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
         /* case SerialHandshake_RequestToSendXOnXOff: */
        }

        /* write config to UART peripheral */
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

/* Write data into buffer */
HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeWrite___VOID__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);      

        uint8_t * data;
        uint8_t uartNum = 0;
        size_t length = 0;
        
        if(pThis[ FIELD___disposed ].NumericByRef().u1 != 0) NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        
        /* Get UART device number */
        uartNum = pThis[ FIELD___portIndex ].NumericByRef().s4;

        /* Quit if parameters or device is invalid or out of range */
        if (uartNum > 8) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER); 

        /* dereference the data buffer from the argument */
        CLR_RT_HeapBlock_Array* dataBuffer = stack.Arg1().DereferenceArray();

        /* get a  the pointer to the array by using the first element of the array */
        data =dataBuffer->GetFirstElement();

        /* get the size of the buffer */
        length = dataBuffer->m_numOfElements;

        /* check if there is enough room in the buffer */
        if(Uart_PAL[uartNum]->TxRingBuffer.Capacity() - Uart_PAL[uartNum]->TxRingBuffer.Length() < length)
        {
            /* not enough room in the buffer */
             NANOCLR_SET_AND_LEAVE(CLR_E_BUFFER_TOO_SMALL);
        }

        /* push data to buffer */
        size_t bytesWritten = Uart_PAL[uartNum]->TxRingBuffer.Push(data, length);

        /* check if all requested bytes were written */
        if(bytesWritten != length)
        {
            /* not sure if this is the best exception to throw here... */
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        
        /* need to update the _unstoredBufferLength field in the SerialDeviceOutputStream */
        /* get pointer to outputStream field */
        CLR_RT_HeapBlock* outputStream = pThis[Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::FIELD___outputStream].Dereference();
        /* get pointer to _unstoredBufferLength field and udpate field value */
        outputStream[Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDeviceOutputStream::FIELD___unstoredBufferLength].NumericByRef().s4 = Uart_PAL[uartNum]->TxRingBuffer.Length();
    }
    NANOCLR_NOCLEANUP();
}

/* Store - Send buffer and wait */
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
        uint32_t timeout;

        /* get a pointer to the managed object instance and check that it's not NULL */
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        uartNum = pThis[ FIELD___portIndex ].NumericByRef().s4;

        if(pThis[ FIELD___disposed ].NumericByRef().u1 != 0) NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);

        /* Quit if parameters or device is invalid or out of range */
        if (uartNum > 8) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

        /* get value for _readtimeout field (pointer!) */
        writeTimeout = &pThis[ Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::FIELD___writeTimeout ];

        timeout = (uint32_t) pThis[Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::FIELD___writeTimeout].NumericByRef().u4;

        /* setup timeout */
        NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(*writeTimeout, timeoutTicks));

        /* push dummy length value onto the eval stack */
        /* this is going to be used to store how many bytes where buffered to Tx */
        if(stack.m_customState == 1)
        {
            stack.PushValueI4(0);
            /* bump custom state so the read value above is pushed only once */
            stack.m_customState = 2;
        }
 
        /* check if there is anything the buffer */
        if(Uart_PAL[uartNum]->TxRingBuffer.Length() > 0)
        {
            /* check if there is a TX operation ongoing */
            if(Uart_PAL[uartNum]->TxOngoingCount == 0)
            {
                /* OK to Tx */
                txOk = true;
            }
            else
            {
                /* need to wait for the ongoing operation to complete before starting a new one */
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
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, timeout, CLR_RT_ExecutionEngine::c_Event_SerialPortOut, eventResult));
            if(eventResult)
            {
                /* Event occurred, get from the eval stack how many bytes were buffered to Tx */
                /* Pop elements from ring buffer, just pop. */
                Uart_PAL[uartNum]->TxRingBuffer.Pop(Uart_PAL[uartNum]->TxOngoingCount);

                /* reset Tx ongoing count */
                Uart_PAL[uartNum]->TxOngoingCount = 0;
                
                /* Notify the task that the transmission is complete. */
                length = stack.m_evalStack[1].NumericByRef().s4;

                break;
            }
            else
            {
                asm("nop");
                /* NANOCLR_SET_AND_LEAVE( CLR_E_TIMEOUT ); */
            }
        }
        /* pop length/timeout heap block from stack */
        stack.PopValue();
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
        CLR_RT_HeapBlock* pThis = stack.This(); FAULT_ON_NULL(pThis);
        InputStreamOptions options = InputStreamOptions_None;

        bool eventResult = true;

        size_t read_count = 0;
        uint8_t uartNum = 0;        
        uart_data_t * rx_p = NULL;

        if(pThis[ FIELD___disposed ].NumericByRef().u1 != 0) NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);

        uartNum = pThis[ FIELD___portIndex ].NumericByRef().s4;
        rx_p    = &Uart_PAL[uartNum]->uart_data;
        
        /* Dereference the data buffer from the argument */
        dataBuffer = stack.Arg1().DereferenceArray();

        /* Get a the pointer to the array by using the first element of the array */
        rx_p->out_data = dataBuffer->GetFirstElement();

        /* Get the InputStreamOptions option */
        /* TODO: Implement transfer options. */  
        options = (InputStreamOptions)stack.Arg3().NumericByRef().s4;

        /* Get how many bytes are requested to read */
        read_count = stack.Arg2().NumericByRef().s4;

        /* Get the length of the data buffer */
        rx_p->bytes_to_read = dataBuffer->m_numOfElements;
        
        /* Zero out bytes received. */
        rx_p->bytes_received = 0;

        /* Set up timeout. */ 
        rx_p->timeout = (uint32_t) pThis[Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::FIELD___readTimeout].NumericByRef().u4;

        /* Calculate timeout to FreeRTOS ticks */
        rx_p->f_timeout = (rx_p->timeout/10000) - 1000;
        
        /* Set trigger level (number of bytes after xStreamBuffer function receiving data returns.) */
        xStreamBufferSetTriggerLevel(Uart_PAL[uartNum]->xReceiveBuffer,
                                    rx_p->bytes_to_read);

        /* We have enough bytes so retrive it directly. */  
        if (xStreamBufferBytesAvailable( Uart_PAL[uartNum]->xReceiveBuffer) >=  read_count)
        {   
            {           
                rx_p->bytes_received =  xStreamBufferReceive(Uart_PAL[uartNum]->xReceiveBuffer,
                                                            (void *) rx_p->out_data,
                                                            rx_p->bytes_to_read,
                                                            rx_p->f_timeout);
            }
        }
        
        /* Not enough bytes in buffer, fire task waiting for more data. */
        else 
        {                
            /* Notify task that we want to receive data. */
            xTaskNotify( Uart_PAL[uartNum]->xReadTaskToNotify, 0x02, eSetBits);

            /* Wait for event from task. */
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, rx_p->timeout, CLR_RT_ExecutionEngine::c_Event_SerialPortIn, eventResult));
        }

        /* Timeout! */
        if (!eventResult)        
        {
            asm("nop");
            /* NANOCLR_SET_AND_LEAVE( CLR_E_TIMEOUT ); */
        }

        /* Return how many bytes were read */
        stack.SetResult_U4(rx_p->bytes_received);

        (void) options;
    }    
    NANOCLR_NOCLEANUP();
}

/* TODO: Implement watch char. Currently this funtion does nothing. */
HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeSetWatchChar___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
	{
        /* get a pointer to the managed object instance and check that it's not NULL */
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        /* Choose the driver for this SerialDevice */
        uint8_t uartNum = (uint8_t)pThis[ FIELD___portIndex ].NumericByRef().s4;

        /* set watch char */
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
        read_count = xStreamBufferBytesAvailable(Uart_PAL[uartNum]->xReceiveBuffer);
        
        stack.SetResult_U4(read_count);
	}
	NANOCLR_NOCLEANUP();
}

/* Return available devices */
/* TODO: implement checking for available devices */
HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
   NANOCLR_HEADER();
   {
       /* declare the device selector string whose max size is "COM1,COM2,COM3,COM4,COM5,COM6,COM7,COM8" + terminator */
       char deviceSelectorString[ 40 ] = "COM1,COM2,COM3,COM4,COM5,COM6,COM7,COM8";

       /* because the caller is expecting a result to be returned */
       /* we need set a return result in the stack argument using the a ppropriate SetResult according to the variable type (a string here) */
       stack.SetResult_String(deviceSelectorString);
   }
   NANOCLR_NOCLEANUP_NOLABEL();
}

/* Task wait for notify from DMA interrupt or notify from read function.
   When notify comes it fires events to inform nanoFramework that read/write is finished */
static void vRwEvent(void * pvParameters)
{
    /* This is main UART struct, holding config, handlers, and the rest of variables.d */
    NF_PAL_UART * rx_Uart_PAL = (NF_PAL_UART *) pvParameters;
    /* UART data struct, holding variables needed for this task, also holding incoming data in xReceiveStream. */
    uart_data_t * rx_p = &rx_Uart_PAL->uart_data;
    
    while(1)
    {
        /* Task is in block state until receives event. */
        xTaskNotifyWait(0x00, 0xffffffff, &rx_Uart_PAL->ulNotifiedValue, portMAX_DELAY);

        /* Check for dma event, if theres is one unblock it. */
        if ((rx_Uart_PAL->ulNotifiedValue & 0x01) != 0)
        {
            /* Send event to nanoFramework that write is complete. */
            Events_Set(SYSTEM_EVENT_FLAG_COM_OUT);
        }
        
        /* Check is there event from task reading bytes. */
        if ((rx_Uart_PAL->ulNotifiedValue & 0x02) != 0)  
        {
            /* If there is read until trigger or timeout and send event to nanoFramework. */
            rx_p->bytes_received =  xStreamBufferReceive(rx_Uart_PAL->xReceiveBuffer,
                                                        (void *) rx_p->out_data,
                                                        rx_p->bytes_to_read,
                                                        rx_p->f_timeout);
            Events_Set(SYSTEM_EVENT_FLAG_COM_IN);    
        }
    }
    
    (void) pvParameters;    
    vTaskDelete(NULL);
}