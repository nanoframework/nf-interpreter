//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIN_DEV_SERIAL_NATIVE_H_
#define _WIN_DEV_SERIAL_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

typedef enum __nfpack SerialData
{
    SerialData_Chars = 0,
    SerialData_WatchChar = 1,
} SerialData;

typedef enum __nfpack SerialHandshake
{
    SerialHandshake_None = 0,
    SerialHandshake_RequestToSend = 1,
    SerialHandshake_RequestToSendXOnXOff = 2,
    SerialHandshake_XOnXOff = 3,
} SerialHandshake;

typedef enum __nfpack SerialParity
{
    SerialParity_None = 0,
    SerialParity_Even = 1,
    SerialParity_Mark = 2,
    SerialParity_Odd = 3,
    SerialParity_Space = 4,
} SerialParity;

typedef enum __nfpack SerialStopBitCount
{
    SerialStopBitCount_One = 0,
    SerialStopBitCount_OnePointFive = 1,
    SerialStopBitCount_Two = 2,
} SerialStopBitCount;

/////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH  Windows.Storage.Streams.InputStreamOptions (in managed code) !!! //
/////////////////////////////////////////////////////////////////////////////////////////////

enum InputStreamOptions
{
    InputStreamOptions_None = 0,
    InputStreamOptions_Partial,
    InputStreamOptions_ReadAhead
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

#endif  //_WIN_DEV_SERIAL_NATIVE_H_
