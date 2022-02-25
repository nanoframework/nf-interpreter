//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_dev_onewire_target.h"
#include <target_nf_dev_onewire_config.h>
#include <Esp32_DeviceMapping.h>

// struct for working threads
static OneWireFindStruct FindStruct;
static bool OneWireOperationResult;
static TaskHandle_t WaitingTask;
static uint8_t LastDiscrepancy;
static uint8_t LastFamilyDiscrepancy;
static uint8_t LastDevice;
static uint8_t SerialNum[8];

// Driver state.
static oneWireState DriverState = ONEWIRE_UNINIT;

HRESULT oneWireInit()
{
    DriverState = ONEWIRE_STOP;

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .use_ref_tick = false,
    };

    // get GPIO pins configured for UART assigned to 1-Wire
    // need to subtract one to get the correct index of UART in mapped device pins
    int txPin = Esp32_GetMappedDevicePins(DEV_TYPE_SERIAL, NF_ONEWIRE_ESP32_UART_NUM - 1, Esp32SerialPin_Tx);
    int rxPin = Esp32_GetMappedDevicePins(DEV_TYPE_SERIAL, NF_ONEWIRE_ESP32_UART_NUM - 1, Esp32SerialPin_Rx);

    // check if TX, RX pins have been previously set
    if (txPin == UART_PIN_NO_CHANGE || rxPin == UART_PIN_NO_CHANGE)
    {
        return CLR_E_PIN_UNAVAILABLE;
    }

    // configure GPIO and UART
    if (gpio_set_direction(gpio_num_t(txPin), GPIO_MODE_OUTPUT_OD) != ESP_OK)
    {
        return CLR_E_INVALID_OPERATION;
    }

    if (uart_param_config(NF_ONEWIRE_ESP32_UART_NUM, &uart_config) != ESP_OK)
    {
        return CLR_E_INVALID_OPERATION;
    }

    if (uart_set_pin(NF_ONEWIRE_ESP32_UART_NUM, txPin, rxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) != ESP_OK)
    {
        return CLR_E_INVALID_OPERATION;
    }

    if (uart_driver_install(NF_ONEWIRE_ESP32_UART_NUM, UART_FIFO_LEN * 2, 0, 0, NULL, ESP_INTR_FLAG_IRAM) != ESP_OK)
    {
        return CLR_E_INVALID_OPERATION;
    }

    DriverState = ONEWIRE_READY;

    return S_OK;
}

void oneWireStop()
{
    // stop UART
    uart_driver_delete(NF_ONEWIRE_ESP32_UART_NUM);

    // driver is stopped
    DriverState = ONEWIRE_STOP;
}

uint8_t oneWireTouchReset(void)
{
    char reset = 0xF0;
    uint8_t presence;

    // flush DMA buffer to ensure cache coherency
    uart_flush(NF_ONEWIRE_ESP32_UART_NUM);
    // set UART baud rate to 9600bps (required to send the RESET condition to the 1-Wire bus)
    uart_set_baudrate(NF_ONEWIRE_ESP32_UART_NUM, 9600);

    uart_write_bytes(NF_ONEWIRE_ESP32_UART_NUM, (const char *)&reset, 1);
    uart_read_bytes(NF_ONEWIRE_ESP32_UART_NUM, &presence, 1, 20 / portTICK_RATE_MS);

    // set UART baud rate to 115200bps (normal comm is performed at this baud rate)
    uart_set_baudrate(NF_ONEWIRE_ESP32_UART_NUM, 115200);

    // check for presence pulse
    return (presence != reset);
}

bool oneWireTouchBit(bool sendbit)
{
    // need to send 1-Wire write 1 or 0 according to sendbit
    char write = sendbit ? IWIRE_WR1 : IWIRE_WR0;
    uint8_t reply;

    // flush DMA buffer to ensure cache coherency
    uart_flush(NF_ONEWIRE_ESP32_UART_NUM);

    uart_write_bytes(NF_ONEWIRE_ESP32_UART_NUM, (const char *)&write, 1);
    uart_read_bytes(NF_ONEWIRE_ESP32_UART_NUM, &reply, 1, 20 / portTICK_RATE_MS);

    // interpret 1-Wire reply
    return (reply == IWIRE_RD);
}

uint8_t oneWireTouchByte(uint8_t sendbyte)
{
    uint8_t send_mask = 0x01, result = 0;
    uint8_t i = 0;
    char writeBuffer[8];
    uint8_t readBuffer[8];

    // send byte
    while (send_mask)
    {
        writeBuffer[i] = (sendbyte & send_mask) ? IWIRE_WR1 : IWIRE_WR0;
        i++;
        // rotates the position mask transmit bit
        send_mask <<= 1;
    };

    // flush DMA buffer to ensure cache coherency
    uart_flush(NF_ONEWIRE_ESP32_UART_NUM);

    uart_write_bytes(NF_ONEWIRE_ESP32_UART_NUM, (const char *)writeBuffer, 8);
    uart_read_bytes(NF_ONEWIRE_ESP32_UART_NUM, readBuffer, 8, 20 / portTICK_RATE_MS);

    // reset send mask to interpret the reply
    send_mask = 0x01;

    for (i = 0; i < 8; i++)
    {
        if (readBuffer[i] == IWIRE_RD)
        {
            result |= send_mask;
        }

        send_mask <<= 1;
    }

    return result;
}

void oneWireAquire()
{
}

void oneWireRelease()
{
}

// compute CRC8 using running algorith (slower but saves FLASH)
uint8_t doCrc8(uint8_t oldCrc, uint8_t x)
{
    uint8_t crc = oldCrc;

    for (uint8_t i = 8; i; i--)
    {
        uint8_t mix = (crc ^ x) & 0x01;
        crc >>= 1;
        if (mix)
            crc ^= 0x8C;
        x >>= 1;
    }

    return crc;
}

/*******************************************************************************
** NAME: oneWireSerialNum **************************************************
********************************************************************************

DESCRIPTION:
     // The 'oneWireSerialNum' function either reads or sets the SerialNum buffer
     // that is used in the search functions 'owFirst' and 'owNext'.
     // This function contains two parameters, 'serialnum_buf' is a pointer
     // to a buffer provided by the caller.  'serialnum_buf' should point to
     // an array of 8 unsigned chars.  The second parameter is a flag called
     // 'do_read' that is TRUE (1) if the operation is to read and FALSE
     // (0) if the operation is to set the internal SerialNum buffer from
     // the data in the provided buffer.
     //
     // 'serialnum_buf' - buffer to that contains the serial number to set
     //                   when do_read = FALSE (0) and buffer to get the serial
     //                   number when do_read = TRUE (1).
     // 'do_read'       - flag to indicate reading (1) or setting (0) the current
     //                   serial number.
     //

USAGE EXAMPLES:

AUTHOR: jassimoes

COMMENTS:

*******************************************************************************/
void oneWireSerialNum(uint8_t *serialnum_buf, uint8_t do_read)
{
    uint8_t i;

    //-------------------------------------------------//
    // read the internal buffer and place in 'serialnum_buf'
    if (do_read)
    {
        for (i = 0; i < 8; i++)
        {
            serialnum_buf[i] = SerialNum[i];
        }
    }
    // set the internal buffer from the data in 'serialnum_buf'
    else
    {
        for (i = 0; i < 8; i++)
        {
            SerialNum[i] = serialnum_buf[i];
        }
    }
}

/*******************************************************************************
** NAME: oneWireFindNext *******************************************************
********************************************************************************/
// The 'oneWireFindNext' function does a general search.
// This function continues from the previos search state. The search state
// can be reset by using the 'oneWireFindFirst' function.
// This function contains one parameter 'alarmOnly'.
// When 'alarmOnly' is TRUE (1) the find alarm command
// 0xEC is sent instead of the normal search command 0xF0.
// Using the find alarm command 0xEC will limit the search to only
// 1-Wire devices that are in an 'alarm' state.
//
// 'doReset'   - TRUE (1) perform reset before search, FALSE (0) do not
//                perform reset before search.
// 'alarmOnly' - TRUE (1) the find alarm command 0xEC is
//                sent instead of the normal search command 0xF0
//
// Returns:   TRUE (1) : when a 1-Wire device was found and it's
//                       Serial Number placed in the global SerialNum
//            FALSE (0): when no new device was found.  Either the
//                       last search was the last device or there
//                       are no devices on the 1-Wire Net.
bool oneWireFindNext(bool doReset, bool alarmOnly)
{
    uint8_t romBitIndex = 1;
    uint8_t romByteIndex = 0;
    uint8_t bitMask = 1;
    uint8_t attempt = 0;
    uint8_t discrepMarker = 0;
    bool outBit = 0;
    bool result = FALSE;
    uint8_t lastcrc8 = 0;

    // if the last call was the last one
    if (LastDevice)
    {
        // reset the search
        LastDiscrepancy = 0;
        LastDevice = FALSE;
        LastFamilyDiscrepancy = 0;

        return FALSE;
    }

    // check if reset bus was requested
    if (doReset)
    {
        // reset the 1-Wire bus
        // if there is no presence pulse there is nothing to do here, return FALSE
        if (!oneWireTouchReset())
        {
            // reset the search
            LastDiscrepancy = 0;
            LastFamilyDiscrepancy = 0;
            return FALSE;
        }
    }

    // send search command
    if (alarmOnly)
    {
        // conditional search command (devices in alarm condition)
        oneWireTouchByte(COND_SEARCH_ROM);
    }
    else
    {
        // ROM search command
        oneWireTouchByte(SEARCH_ROM);
    }

    do
    {
        attempt = 0;

        if (oneWireTouchBit(TRUE) == 1)
        {
            attempt = 2;
        }

        if (oneWireTouchBit(TRUE) == 1)
        {
            attempt |= 1;
        }

        if (attempt == 3)
        {
            // no devices present, done here
            break;
        }
        else
        {
            if (attempt > 0)
            {
                // all connected devices reply with 0 or 1
                // write bit to perform search
                outBit = attempt >> 1;
            }
            else
            {
                if (romBitIndex < LastDiscrepancy)
                {
                    outBit = ((SerialNum[romByteIndex] & bitMask) > 0);
                }
                else
                {
                    // set to 1 if it's the same, otherwise 0
                    outBit = (romBitIndex == LastDiscrepancy);
                }

                // on 0 record position on bitMask
                if (outBit == 0)
                {
                    discrepMarker = romBitIndex;
                }
            }

            // isolate bit in ROM[n] with bitMask
            if (outBit == 1)
            {
                SerialNum[romByteIndex] |= bitMask;
            }
            else
            {
                SerialNum[romByteIndex] &= ~bitMask;
            }

            // ROM search write
            oneWireTouchBit(outBit);

            romBitIndex++;

            bitMask = bitMask << 1;

            // if the mask has reached 0 then go for a new ROM
            if (bitMask == 0)
            {
                // reset mask and perform CRC8
                lastcrc8 = doCrc8(lastcrc8, SerialNum[romByteIndex]);

                romByteIndex++;
                bitMask++;
            }
        }

    } while (romByteIndex < 8); // loop until we have all ROM bytes

    if ((romBitIndex < 65) || (lastcrc8 != 0))
    {
        // search was unsuccessful reset the last discrepancy
        LastDiscrepancy = 0;
    }
    else
    {
        // search was successful: set last discrepancy, device and result
        LastDiscrepancy = discrepMarker;
        LastDevice = (LastDiscrepancy == 0);

        // search isn't completed there are more devices present in the bus
        result = TRUE;
    }

    return result;
}

/*******************************************************************************
** NAME: oneWireFindFirst ******************************************************
********************************************************************************/
// The 'oneWireFindFirst' finds the first device on the 1-Wire Net.
// This function contains one parameter 'alarmOnly'.  When
// 'alarmOnly' is TRUE (1) the find alarm command 0xEC is
// sent instead of the normal search command 0xF0.
// Using the find alarm command 0xEC will limit the search to only
// 1-Wire devices that are in an 'alarm' state.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'doReset'   - TRUE (1) perform reset before search, FALSE (0) do not
//                perform reset before search.
// 'alarmOnly' - TRUE (1) the find alarm command 0xEC is
//                sent instead of the normal search command 0xF0
//
// Returns:   TRUE (1) : when a 1-Wire device was found and it's
//                       Serial Number placed in the global SerialNum
//            FALSE (0): There are no devices on the 1-Wire Net.
bool oneWireFindFirst(bool doReset, bool alarmOnly)
{
    // reset the search state
    LastDiscrepancy = 0;
    LastDevice = FALSE;
    LastFamilyDiscrepancy = 0;

    // clear serial number buffer for new search
    memset(SerialNum, 0, 8);

    // Call Next and return it's return value;
    return oneWireFindNext(doReset, alarmOnly);
}

// OneWire Find First/Next working thread
static void OneWireFindWorkingThread(void *pvParameters)
{
    OneWireFindStruct *findStruct = (OneWireFindStruct *)pvParameters;

    OneWireOperationResult = findStruct->FindFirst ? oneWireFindFirst(findStruct->DoReset, findStruct->AlarmOnly)
                                                   : oneWireFindNext(findStruct->DoReset, findStruct->AlarmOnly);

    // fire event for 1-Wire operation completed
    xTaskNotifyGive(WaitingTask);
    vTaskDelete(NULL);
}

HRESULT FindOneDevice(CLR_RT_StackFrame &stack, bool findFirst)
{
    uint8_t *serialNumberPointer;
    CLR_RT_HeapBlock hbTimeout;
    CLR_INT64 *timeout;
    TaskHandle_t task;
    HRESULT result;

    // set an infinite timeout to wait forever for the operation to complete
    // this value has to be in ticks to be properly loaded by SetupTimeoutFromTicks() below
    hbTimeout.SetInteger((CLR_INT64)-1);
    result = stack.SetupTimeoutFromTicks(hbTimeout, timeout);

    if (result != S_OK)
    {
        return result;
    }

    // this is going to be used to check for the right event in case of simultaneous 1-Wire operations
    if (stack.m_customState == 1)
    {
        FindStruct.DoReset = stack.Arg1().NumericByRefConst().u1 != 0;
        FindStruct.AlarmOnly = stack.Arg2().NumericByRefConst().u1 != 0;
        FindStruct.FindFirst = findFirst;

        // because the 1-Wire bus is shared, acquire the module
        oneWireAquire();

        // spawn working thread to perform the 1-Wire operations
        WaitingTask = xTaskGetCurrentTaskHandle();
        xTaskCreate(OneWireFindWorkingThread, "OWWT", 2048, &FindStruct, 12, &task);

        // bump custom state
        stack.m_customState = 2;
    }

    // wait for 1-Wire operation complete
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    oneWireRelease();

    // get the result from the working thread execution
    if (OneWireOperationResult)
    {
        // update serialNumber field

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        if (pThis == NULL)
        {
            return CLR_E_NULL_REFERENCE;
        }

        // get a pointer to the serial number field in the OneWireController instance
        CLR_RT_HeapBlock_Array *serialNumberField =
            pThis[Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::FIELD___serialNumber]
                .DereferenceArray();

        _ASSERTE(serialNumberField->m_numOfElements == 8);

        // get a pointer to the first element of the byte array
        serialNumberPointer = (uint8_t *)serialNumberField->GetFirstElement();

        oneWireSerialNum(serialNumberPointer, TRUE);
    }

    // pop timeout heap block from stack
    stack.PopValue();

    // set result
    stack.SetResult_Boolean(OneWireOperationResult);

    return S_OK;
}

HRESULT Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::TouchReset___BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    stack.SetResult_Boolean(oneWireTouchReset());

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::TouchBit___BOOLEAN__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    stack.SetResult_Boolean(oneWireTouchBit(stack.Arg1().NumericByRefConst().u1 != 0));

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::TouchByte___U1__U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    stack.SetResult_U1(oneWireTouchByte((uint8_t)stack.Arg1().NumericByRefConst().u1));

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::WriteByte___U1__U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t sendbyte;

    sendbyte = (uint8_t)stack.Arg1().NumericByRefConst().u1;
    stack.SetResult_U1(oneWireTouchByte(sendbyte) == sendbyte ? TRUE : FALSE);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::ReadByte___U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    stack.SetResult_U1(oneWireTouchByte(0xFF));

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::FindFirstDevice___BOOLEAN__BOOLEAN__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_CHECK_HRESULT(FindOneDevice(stack, true));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::FindNextDevice___BOOLEAN__BOOLEAN__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    NANOCLR_CHECK_HRESULT(FindOneDevice(stack, false));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::NativeDispose___VOID(
    CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    oneWireStop();

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_CHECK_HRESULT(oneWireInit());

    NANOCLR_NOCLEANUP();
}
