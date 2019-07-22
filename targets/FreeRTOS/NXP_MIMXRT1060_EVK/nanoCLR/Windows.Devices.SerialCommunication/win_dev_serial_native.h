//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIN_DEV_SERIAL_NATIVE_H_
#define _WIN_DEV_SERIAL_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>
#include <corlib_native.h>

#include "fsl_lpuart.h"
#include "fsl_lpuart_edma.h"
#include "fsl_dmamux.h"

#include "stream_buffer.h"

#define UART_TX_BUFER_SIZE 256
#define UART_RX_BUFER_SIZE 256

struct Library_win_dev_serial_native_Windows_Devices_SerialCommunication_ErrorReceivedEventArgs
{
    static const int FIELD___error = 1;


    //--//

};

struct Library_win_dev_serial_native_Windows_Devices_SerialCommunication_PinChangedEventArgs
{
    static const int FIELD___pinChange = 1;


    //--//

};

struct Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDataEvent
{
    static const int FIELD__SerialDeviceIndex = 3;
    static const int FIELD__Event = 4;


    //--//

};

struct Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDataReceivedEventArgs
{
    static const int FIELD___data = 1;


    //--//

};

struct Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice
{
    static const int FIELD_STATIC__s_eventListener = 0;

    static const int FIELD___disposed = 1;
    static const int FIELD___syncLock = 2;
    static const int FIELD___opened = 3;
    static const int FIELD___deviceId = 4;
    static const int FIELD___portIndex = 5;
    static const int FIELD___outputStream = 6;
    static const int FIELD___inputStream = 7;
    static const int FIELD___readTimeout = 8;
    static const int FIELD___writeTimeout = 9;
    static const int FIELD___baudRate = 10;
    static const int FIELD___dataBits = 11;
    static const int FIELD___handshake = 12;
    static const int FIELD___parity = 13;
    static const int FIELD___stopBits = 14;
    static const int FIELD___bytesReceived = 15;
    static const int FIELD___watchChar = 16;
    static const int FIELD___callbacksDataReceivedEvent = 17;

    NANOCLR_NATIVE_DECLARE(get_BytesToRead___U4);
    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID);
    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeConfig___VOID);
    NANOCLR_NATIVE_DECLARE(NativeWrite___VOID__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(NativeStore___U4);
    NANOCLR_NATIVE_DECLARE(NativeRead___U4__SZARRAY_U1__I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeSetWatchChar___VOID);
    NANOCLR_NATIVE_DECLARE(GetDeviceSelector___STATIC__STRING);

    //--//

};

struct Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDeviceController
{
    static const int FIELD_STATIC___syncLock = 1;
    static const int FIELD_STATIC__s_instance = 2;
    static const int FIELD_STATIC__s_deviceCollection = 3;


    //--//

};

struct Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDeviceEventListener
{
    static const int FIELD___serialDevicesMap = 1;


    //--//

};

struct Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDeviceInputStream
{
    static const int FIELD___serialDevice = 1;


    //--//

};

struct Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDeviceOutputStream
{
    static const int FIELD___serialDevice = 1;
    static const int FIELD___unstoredBufferLength = 2;


    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_SerialCommunication;

typedef struct {
    size_t bytes_received;
    size_t bytes_to_read;
    int64_t*  timeoutTicks;
    uint8_t data[UART_RX_BUFER_SIZE];
} uart_data_t;

struct NF_PAL_UART
{
    uint8_t dma_num;

    lpuart_config_t uartCfg;
    edma_config_t edmaCfg;
    
    lpuart_edma_handle_t g_lpuartEdmaHandle;
    edma_handle_t g_lpuartTxEdmaHandle;

    HAL_RingBuffer<uint8_t> TxRingBuffer;
    uint8_t* TxBuffer;
    uint16_t TxOngoingCount;

    uint8_t WatchChar;
    TaskHandle_t xWriteTaskToNotify = NULL;    
    TaskHandle_t xReadTaskToNotify = NULL;

    StreamBufferHandle_t xReceiveBuffer;
    uart_data_t uart_data;
};

////////////////////////////////////////////
// declaration of the the UART PAL strucs //
////////////////////////////////////////////

// TODO This is implementation of one UART devices, need to add the rest of structures or find better way to keep uarts config

extern LPUART_Type *const lpuart_bases[];

extern NF_PAL_UART Uart_PAL1;
extern NF_PAL_UART Uart_PAL2;
extern NF_PAL_UART Uart_PAL3;
extern NF_PAL_UART Uart_PAL4;
extern NF_PAL_UART Uart_PAL5;
extern NF_PAL_UART Uart_PAL6;
extern NF_PAL_UART Uart_PAL7;
extern NF_PAL_UART Uart_PAL8;

extern NF_PAL_UART* Uart_PAL[];

#endif  //_WIN_DEV_SERIAL_NATIVE_H_
