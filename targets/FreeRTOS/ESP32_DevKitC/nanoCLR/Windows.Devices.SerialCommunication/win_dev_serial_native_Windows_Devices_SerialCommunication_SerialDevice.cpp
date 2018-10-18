//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


//#include <hal.h>
// #include <cmsis_os.h>
// #include <LaunchCLR.h>
// #include <string.h>
#include <nanoHAL.h>
#include "win_dev_serial_native.h"


// buffers size
// tx buffer size: 256 bytes
#define UART_TX_BUFER_SIZE  256
// rx buffer size: 256 bytes
#define UART_RX_BUFER_SIZE  256

// in UWP the COM ports are named COM1, COM2, COM3. But ESP32 uses internally UART0, UART1, UART2. This maps the port index 1, 2 or 3 to the uart number 0, 1 or 2
#define PORT_INDEX_TO_UART_NUM(portIndex)	((portIndex) - 1)
// in UWP the COM ports are named COM1, COM2, COM3. But ESP32 uses internally UART0, UART1, UART2. This maps the uart number 0, 1 or 2 to the port index 1, 2 or 3
#define UART_NUM_TO_PORT_INDEX(uart_num)	((uart_num) + 1)

/////////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.SerialCommunication.SerialHandshake (in managed code) !!! //
/////////////////////////////////////////////////////////////////////////////////////////////////////

enum SerialHandshake
{
    SerialHandshake_None = 0,
    SerialHandshake_RequestToSend,
    SerialHandshake_RequestToSendXOnXOff,
    SerialHandshake_XOnXOff
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.SerialCommunication.SerialParity (in managed code) !!! //
//////////////////////////////////////////////////////////////////////////////////////////////////

enum SerialParity
{
    SerialParity_None = 0,
    SerialParity_Even,
    SerialParity_Mark,
    SerialParity_Odd,
    SerialParity_Space
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.SerialCommunication.SerialStopBitCount (in managed code) !!! //
////////////////////////////////////////////////////////////////////////////////////////////////////////

enum SerialStopBitCount
{
    SerialStopBitCount_One = 0,
    SerialStopBitCount_OnePointFive,
    SerialStopBitCount_Two
};

/////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH  Windows.Storage.Streams.InputStreamOptions (in managed code) !!! //
/////////////////////////////////////////////////////////////////////////////////////////////

enum InputStreamOptions
{
    InputStreamOptions_None = 0,
    InputStreamOptions_Partial,
    InputStreamOptions_ReadAhead
};

////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.SerialCommunication.SerialData (in managed code) !!! //
////////////////////////////////////////////////////////////////////////////////////////////////

enum SerialData
{
    SerialData_Chars = 0,
    SerialData_WatchChar,
};

static const char* TAG = "SerialDevice";

static char Esp_Serial_Initialised_Flag[UART_NUM_MAX] = {0, 0, 0};
static QueueHandle_t Uart_Event_Queue[UART_NUM_MAX];
static bool Uart_Post_SerialData_Chars_Event[UART_NUM_MAX] = {true, true, true};
CLR_RT_HeapBlock* Serial_Device_Instance[UART_NUM_MAX];
										  
void Esp32_Serial_UnitializeAll()
{
    for (int uart_num = 0; uart_num < UART_NUM_MAX; uart_num++) 
    {
        if (Esp_Serial_Initialised_Flag[uart_num])
        {
            // Delete uart driver and send the exit signal to the UART event handling queue
            uart_driver_delete((uart_port_t)uart_num);
			xQueueSend(Uart_Event_Queue[uart_num], (void*)UART_EVENT_MAX, (portTickType)0);
			Uart_Post_SerialData_Chars_Event[uart_num] = false;
			Serial_Device_Instance[uart_num] = NULL;
            Esp_Serial_Initialised_Flag[uart_num] = 0;
        }
    }
}

static void uart_event_task(void *pvParameters)
{
    int uart_num = (int)pvParameters;
    uart_event_t event;
    bool run = true;
	
	while(run)
	{
        // Waiting for UART event.
        if(xQueueReceive(Uart_Event_Queue[uart_num], (void*)&event, (portTickType)portMAX_DELAY)) {
            switch(event.type) {
                // Data received
                case UART_DATA:
					// only if no one is reading the chars already and only if an event handler is attached
					if (Uart_Post_SerialData_Chars_Event[uart_num] && Serial_Device_Instance[uart_num][Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::FIELD___callbacksDataReceivedEvent].Dereference() != NULL)
					{
						// post a managed event with the port index and the chars event code; uart_num + 1 because UART0 maps to COM1
						PostManagedEvent(EVENT_SERIAL, 0, UART_NUM_TO_PORT_INDEX(uart_num), SerialData_Chars);
						// supress the event for the following chars; prevents spamming with the SerialData_Chars event
						// the flag will be reset if NativeRead will be called
						Uart_Post_SerialData_Chars_Event[uart_num] = false;
					}
                    break;
				// Pattern detection used for the WatchChar
                case UART_PATTERN_DET:
					// only if an event handler is attached
					if (Serial_Device_Instance[uart_num][Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::FIELD___callbacksDataReceivedEvent].Dereference() != NULL)
					{
						// post a managed event with the port index and the watch char event code; uart_num + 1 because UART0 maps to COM1
						PostManagedEvent(EVENT_SERIAL, 0, UART_NUM_TO_PORT_INDEX(uart_num), SerialData_WatchChar);
					}
					break;
				// signal to end the task (UART_EVENT_MAX used)
				case UART_EVENT_MAX:
					run = false;
					break;
                // Others
                default:
                    break;
            }
        }
    }
	vQueueDelete(Uart_Event_Queue[uart_num]);
    vTaskDelete(NULL);
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeDispose___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
		// get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

		// Get Uart number for serial device
        uart_port_t uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[ FIELD___portIndex ].NumericByRef().s4);
		// uninstall the driver and send the exit signal to the UART event handling queue
        uart_driver_delete(uart_num);
		xQueueSend(Uart_Event_Queue[uart_num], (void*)UART_EVENT_MAX, (portTickType)portMAX_DELAY);
		Uart_Post_SerialData_Chars_Event[uart_num] = false;
		Serial_Device_Instance[uart_num] = NULL;
        Esp_Serial_Initialised_Flag[uart_num] = 0;
   }
    NANOCLR_NOCLEANUP();
}

//
//  Initialise a new Serial port
//
HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Get Uart number for serial device
        uart_port_t uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[ FIELD___portIndex ].NumericByRef().s4);
        if ( uart_num > UART_NUM_2 || uart_num < 0 )
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // call the configure and abort if not OK
        HRESULT res = NativeConfig___VOID(stack);
        if (res != S_OK)
        {
            NANOCLR_SET_AND_LEAVE(res);
        }

        // Install driver
        esp_err_t esp_err = uart_driver_install(uart_num, 
                                                UART_RX_BUFER_SIZE, 			// rx_buffer_size, 
                                                UART_TX_BUFER_SIZE, 			// tx_buffer_size, not buffered
                                                20,                  			// queue_size
                                                &Uart_Event_Queue[uart_num],    // QueueHandle_t *uart_queue ( none for now )
                                                0                   			// intr_alloc_flags
                                                );
        if ( esp_err != ESP_OK )
        {
            ESP_LOGE( TAG, "Failed to install uart driver");
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }   
        
		// receive the SerialData_Chars events and store the SerialDevice instance
		Uart_Post_SerialData_Chars_Event[uart_num] = true;
		Serial_Device_Instance[uart_num] = pThis;
		
		// Create a task to handle UART event from ISR
		char task_name[16];
		sprintf(task_name, "uart%d_events", uart_num);
		xTaskCreate(uart_event_task, task_name, 2048, (void*)uart_num, 12, NULL);
	
        // Ensure driver gets unitialized during soft reboot
        HAL_AddSoftRebootHandler(Esp32_Serial_UnitializeAll);
        Esp_Serial_Initialised_Flag[uart_num] = 1;
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
        uart_config_t uart_config;
    
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Get Uart number for serial device
        uart_port_t uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[ FIELD___portIndex ].NumericByRef().s4);
        if ( uart_num > UART_NUM_2 || uart_num < 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // setup configuration
        // data bits / baud rate
        uart_config.baud_rate  = (uart_word_length_t)pThis[ FIELD___baudRate ].NumericByRef().s4;      //baudrate

        switch( pThis[ FIELD___dataBits ].NumericByRef().s4 )      //data bit mode
        {
            case 5:
                uart_config.data_bits = UART_DATA_5_BITS; break;
            case 6:
                uart_config.data_bits = UART_DATA_6_BITS; break;
            case 7:
                uart_config.data_bits = UART_DATA_7_BITS; break;
            case 8:
                uart_config.data_bits = UART_DATA_8_BITS; break;
            default:
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
 
        // parity 
        switch ((SerialParity)pThis[ FIELD___parity ].NumericByRef().s4)
        {
            default:
            case SerialParity_None :
                uart_config.parity = UART_PARITY_DISABLE ;                       
                break;
            case SerialParity_Even :
                uart_config.parity = UART_PARITY_EVEN ;                       
                break;
            case SerialParity_Odd :
                uart_config.parity = UART_PARITY_ODD ;                       
                break;
        }
 
        // stop bits 
        switch ((SerialStopBitCount)pThis[ FIELD___stopBits ].NumericByRef().s4)
        {
            default:
            case SerialStopBitCount_One :
                uart_config.stop_bits = UART_STOP_BITS_1;                       
                break;
            case SerialStopBitCount_OnePointFive :
                uart_config.stop_bits = UART_STOP_BITS_1_5;
                break;
            case SerialStopBitCount_Two :
                uart_config.stop_bits = UART_STOP_BITS_2;
                break;
        }

       bool EnableXonXoff = false;
       switch ((SerialHandshake)pThis[ FIELD___handshake ].NumericByRef().s4)
        {
           default:
            case SerialHandshake_None :
                uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;                      
                break;
           case SerialHandshake_RequestToSend :
                uart_config.flow_ctrl = UART_HW_FLOWCTRL_RTS ;                      
                break;
         
         case SerialHandshake_RequestToSendXOnXOff :
                uart_config.flow_ctrl = UART_HW_FLOWCTRL_RTS ;     
                EnableXonXoff = true;                 
                break;

         case SerialHandshake_XOnXOff :
                EnableXonXoff = true;                 
                break;
        }

       uart_param_config(uart_num, &uart_config);

       if ( EnableXonXoff )
            uart_set_sw_flow_ctrl(uart_num, true, 20, 40);


       // By default set the direct GPIO pins
       int txPin = UART_NUM_0_TXD_DIRECT_GPIO_NUM;      // 1
       int rxPin = UART_NUM_0_RXD_DIRECT_GPIO_NUM;      // 3
       int rtsPin = UART_NUM_0_CTS_DIRECT_GPIO_NUM;     // 19
       int ctsPin = UART_NUM_0_RTS_DIRECT_GPIO_NUM;     // 22

       switch(uart_num)
       {
           case UART_NUM_1:
                txPin = UART_NUM_1_TXD_DIRECT_GPIO_NUM;  // 10
                rxPin = UART_NUM_1_RXD_DIRECT_GPIO_NUM;  // 9
                rtsPin = UART_NUM_1_CTS_DIRECT_GPIO_NUM; // 6
                ctsPin = UART_NUM_1_RTS_DIRECT_GPIO_NUM; // 11
                break;

           case UART_NUM_2:
                //txPin = UART_NUM_2_TXD_DIRECT_GPIO_NUM;  // 17
                txPin = 33;
                //rxPin = UART_NUM_2_RXD_DIRECT_GPIO_NUM;  // 16  
                rxPin = 32;
                rtsPin = UART_NUM_2_CTS_DIRECT_GPIO_NUM; // 8
                ctsPin = UART_NUM_2_RTS_DIRECT_GPIO_NUM; // 7
                break;

            default:
                break;
       }

       // Don't use RTS/CTS if no hardware handshake enabled
       if ( uart_config.flow_ctrl == UART_HW_FLOWCTRL_DISABLE )
       {
           rtsPin = UART_PIN_NO_CHANGE;
           ctsPin = UART_PIN_NO_CHANGE;
       }

       uart_set_pin(uart_num, txPin, rxPin, rtsPin, ctsPin);

        // null pointers and vars
        pThis = NULL;
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
        uint8_t* data;
        size_t length = 0;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        if(pThis[ FIELD___disposed ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        // dereference the data buffer from the argument
        CLR_RT_HeapBlock_Array* dataBuffer = stack.Arg1().DereferenceArray();

        // get a the pointer to the array by using the first element of the array
        data = dataBuffer->GetFirstElement();

        // get the size of the buffer
        length = (size_t)dataBuffer->m_numOfElements;

        // Get Uart number for serial device
        uart_port_t uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[ FIELD___portIndex ].NumericByRef().s4);
 

        // Write data to ring bufferand start sending
        size_t bytesWritten = uart_write_bytes( uart_num, (const char*)data, length);

        // check if all requested bytes were written
        if(bytesWritten != length)
        {
            // not sure if this is the best exception to throw here...
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        
        // // need to update the _unstoredBufferLength field in the SerialDeviceOutputStream
        // // get pointer to outputStream field
        // CLR_RT_HeapBlock* outputStream = pThis[Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::FIELD___outputStream].Dereference();
        
        // // get pointer to _unstoredBufferLength field and udpate field value
        // outputStream[Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDeviceOutputStream::FIELD___unstoredBufferLength].NumericByRef().s4 = palUart->TxRingBuffer.Length();

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

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        if(pThis[ FIELD___disposed ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        // Get Uart number for serial device
        uart_port_t uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[ FIELD___portIndex ].NumericByRef().s4);

        // get value for writetimeout
        // the way to access this it's somewhat convoluted but it is what it is
        // get pointer to the field
        CLR_RT_HeapBlock* timeout = &pThis[ Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::FIELD___writeTimeout ];

        // now get a pointer to the actual value
        int64_t* timeoutTicks = Library_corlib_native_System_TimeSpan::GetValuePtr( *timeout );

        // now get the value in ticks and convert it to milliseconds
        int64_t timeoutMillisecondsValue = *timeoutTicks / TIME_CONVERSION__TICKUNITS;

        // Wait for max. writetimeout timespan for data to be sent
        esp_err_t esp_err = uart_wait_tx_done( uart_num,  (TickType_t) timeoutMillisecondsValue / portTICK_PERIOD_MS);
        if (esp_err == ESP_ERR_TIMEOUT)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
        else if (esp_err != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // return how many bytes were send to the UART
        stack.SetResult_U4(length);

        // null pointers and vars
        pThis = NULL;
    }
    NANOCLR_NOCLEANUP();
}


HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeRead___U4__SZARRAY_U1__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        uint8_t* data;
        size_t dataLength = 0;

        size_t count = 0;
        int    bytesRead = 0;
        size_t toRead = 0;

        InputStreamOptions options = InputStreamOptions_None;

        CLR_RT_HeapBlock* timeout;
        int64_t*  timeoutTicks;
        int64_t  timeoutMillisecondsValue;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        if(pThis[ FIELD___disposed ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        // dereference the data buffer from the argument
        CLR_RT_HeapBlock_Array* dataBuffer = stack.Arg1().DereferenceArray();

        // get a the pointer to the array by using the first element of the array
        data = dataBuffer->GetFirstElement();

        // get the length of the data buffer
        dataLength =  dataBuffer->m_numOfElements;

        // get how many bytes are requested to read
        count = stack.Arg2().NumericByRef().s4;

        // get the InputStreamOptions option
        options = (InputStreamOptions)stack.Arg3().NumericByRef().s4;

       // Get Uart number for serial device
        uart_port_t uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[ FIELD___portIndex ].NumericByRef().s4);

        // Bytes requested to read
        toRead = count;
        
        // is the read ahead option enabled?
        if(options == InputStreamOptions_ReadAhead)
        {
            // yes
            // check how many bytes we can store in the buffer argument
            size_t bufferedLength = 0;
            uart_get_buffered_data_len(uart_num, &bufferedLength);

            if(dataLength < bufferedLength )
            {
                // read as many bytes has the buffer can hold
                toRead = dataLength;
            }
            else
            {
                // read everything that's available in the ring buffer
                toRead = bufferedLength;
            }
        }

        // get value for readtimeout
        // the way to access this it's somewhat convoluted but it is what it is
        // get pointer to the field
        timeout = &pThis[ Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::FIELD___readTimeout ];

        // now get a pointer to the actual value
        timeoutTicks = Library_corlib_native_System_TimeSpan::GetValuePtr( *timeout );

        // now get the value in ticks and convert it to miliseconds
        timeoutMillisecondsValue = *timeoutTicks / TIME_CONVERSION__TICKUNITS;

		// suppress the SerialData_Chars event during the reading of the chars; prevents spamming with the SerialData_Chars event
		Uart_Post_SerialData_Chars_Event[uart_num] = false;
        bytesRead = uart_read_bytes(uart_num, data, toRead, timeoutMillisecondsValue / portTICK_RATE_MS);
		Uart_Post_SerialData_Chars_Event[uart_num] = true;
		
        if ( bytesRead < 0 )
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
 
        // return how many bytes were read from the Rx buffer and/or UART
        stack.SetResult_U4(bytesRead);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeSetWatchChar___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
	{
		// get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        if(pThis[ FIELD___disposed ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }
		
		// Get Uart number for serial device
        uart_port_t uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[ FIELD___portIndex ].NumericByRef().s4);
		
		// Get the watch char
        uint8_t watchChar = (uint8_t)pThis[ FIELD___watchChar ].NumericByRef().u1;
		
		// Enable pattern detection for the serial device
		uart_enable_pattern_det_intr(uart_num, watchChar, 1, 10000, 10, 10);
	}
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::get_BytesToRead___U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
	{
		// get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        if(pThis[ FIELD___disposed ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }
		
		// Get Uart number for serial device
        uart_port_t uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[ FIELD___portIndex ].NumericByRef().s4);
		
		// check how many bytes are in the buffer
        size_t bufferedLength = 0;
        uart_get_buffered_data_len(uart_num, &bufferedLength);
		
		// return how many bytes can be read from the Rx buffer
        stack.SetResult_U4(bufferedLength);
	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
   NANOCLR_HEADER();
   {
       // declare the device selector string whose max size is "COM1,COM2,COM3" + terminator 
       char deviceSelectorString[ 15 ] = "COM1,COM2,COM3";

       // because the caller is expecting a result to be returned
       // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
       stack.SetResult_String(deviceSelectorString);
   }
   NANOCLR_NOCLEANUP_NOLABEL();
}