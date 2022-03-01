//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_IO_SER_NATIVE_H
#define SYS_IO_SER_NATIVE_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
// #include <corlib_native.h>
#include <nanoCLR_Checks.h>

typedef enum __nfpack Handshake
{
    Handshake_None = 0,
    Handshake_XOnXOff = 1,
    Handshake_RequestToSend = 2,
    Handshake_RequestToSendXOnXOff = 3,
} Handshake;

typedef enum __nfpack Parity
{
    Parity_None = 0,
    Parity_Odd = 1,
    Parity_Even = 2,
} Parity;

typedef enum __nfpack SerialData
{
    SerialData_Chars = 1,
    SerialData_WatchChar = 2,
} SerialData;

typedef enum __nfpack SerialMode
{
    SerialMode_Normal = 0,
    SerialMode_RS485 = 1,
} SerialMode;

typedef enum __nfpack StopBits
{
    StopBits_One = 1,
    StopBits_Two = 2,
    StopBits_OnePointFive = 3,
} StopBits;

struct Library_sys_io_ser_native_System_IO_Ports_SerialDataEvent
{
    static const int FIELD__SerialDeviceIndex = 3;
    static const int FIELD__Event = 4;

    //--//
};

struct Library_sys_io_ser_native_System_IO_Ports_SerialDataReceivedEventArgs
{
    static const int FIELD___data = 1;

    //--//
};

struct Library_sys_io_ser_native_System_IO_Ports_SerialDeviceController
{
    static const int FIELD_STATIC___syncLock = 0;
    static const int FIELD_STATIC__s_deviceCollection = 1;

    //--//
};

struct Library_sys_io_ser_native_System_IO_Ports_SerialPort
{
    static const int FIELD_STATIC__s_eventListener = 2;

    static const int FIELD___disposed = 1;
    static const int FIELD___syncLock = 2;
    static const int FIELD___opened = 3;
    static const int FIELD___writeTimeout = 4;
    static const int FIELD___readTimeout = 5;
    static const int FIELD___receivedBytesThreshold = 6;
    static const int FIELD___baudRate = 7;
    static const int FIELD___handshake = 8;
    static const int FIELD___stopBits = 9;
    static const int FIELD___dataBits = 10;
    static const int FIELD___parity = 11;
    static const int FIELD___mode = 12;
    static const int FIELD___deviceId = 13;
    static const int FIELD___portIndex = 14;
    static const int FIELD___watchChar = 15;
    static const int FIELD___callbacksDataReceivedEvent = 16;
    static const int FIELD___stream = 17;
    static const int FIELD___newLine = 18;
    static const int FIELD___bufferSize = 19;

    NANOCLR_NATIVE_DECLARE(get_BytesToRead___I4);
    NANOCLR_NATIVE_DECLARE(get_InvertSignalLevels___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(set_InvertSignalLevels___VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(Read___I4__SZARRAY_U1__I4__I4);
    NANOCLR_NATIVE_DECLARE(ReadExisting___STRING);
    NANOCLR_NATIVE_DECLARE(ReadLine___STRING);
    NANOCLR_NATIVE_DECLARE(Write___VOID__SZARRAY_U1__I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID);
    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeConfig___VOID);
    NANOCLR_NATIVE_DECLARE(NativeSetWatchChar___VOID);
    NANOCLR_NATIVE_DECLARE(NativeWriteString___VOID__STRING__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(GetDeviceSelector___STATIC__STRING);

    //--//

    static HRESULT SetupWriteLine(CLR_RT_StackFrame &stack, char **buffer, uint32_t *length, bool *isNewAllocation);
    static bool GetLineFromRxBuffer(
        CLR_RT_HeapBlock *serialDevice,
        HAL_RingBuffer<uint8_t> *ringBuffer,
        uint8_t *&line);
};

struct Library_sys_io_ser_native_System_IO_Ports_SerialDeviceEventListener
{
    static const int FIELD___serialDevicesMap = 1;

    //--//
};

struct Library_sys_io_ser_native_System_IO_Ports_SerialStream
{
    static const int FIELD___serial = 1;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_IO_Ports;

#endif // SYS_IO_SER_NATIVE_H
