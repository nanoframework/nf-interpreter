//-----------------------------------------------------------------------------
//
//                   ** WARNING! **
//    This file was generated automatically by a tool.
//    Re-running the tool will overwrite this file.
//    You should copy this file to a custom location
//    before adding any customization in the copy to
//    prevent loss of your changes when the tool is
//    re-run.
//
//-----------------------------------------------------------------------------

#ifndef _SYS_IO_SER_NATIVE_H_
#define _SYS_IO_SER_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>
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
    static const int FIELD_STATIC__s_instance = 1;
    static const int FIELD_STATIC__s_deviceCollection = 2;

    //--//
};

struct Library_sys_io_ser_native_System_IO_Ports_SerialPort
{
    static const int FIELD_STATIC__s_eventListener = 3;

    static const int FIELD___writeTimeout = 1;
    static const int FIELD___readTimeout = 2;
    static const int FIELD___receivedBytesThreshold = 3;
    static const int FIELD___baudRate = 4;
    static const int FIELD___opened = 5;
    static const int FIELD___disposed = 6;
    static const int FIELD___handshake = 7;
    static const int FIELD___stopBits = 8;
    static const int FIELD___dataBits = 9;
    static const int FIELD___parity = 10;
    static const int FIELD___mode = 11;
    static const int FIELD___deviceId = 12;
    static const int FIELD___portIndex = 13;
    static const int FIELD___watchChar = 14;
    static const int FIELD___callbacksDataReceivedEvent = 15;
    static const int FIELD___stream = 16;
    static const int FIELD___syncLock = 17;
    static const int FIELD___newLine = 18;
    static const int FIELD___hasBeenOpened = 19;

    NANOCLR_NATIVE_DECLARE(get_BytesToRead___I4);
    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID);
    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeConfig___VOID);
    NANOCLR_NATIVE_DECLARE(NativeWrite___VOID__SZARRAY_U1__I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeStore___U4);
    NANOCLR_NATIVE_DECLARE(NativeRead___U4__SZARRAY_U1__I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeSetWatchChar___VOID);
    NANOCLR_NATIVE_DECLARE(GetDeviceSelector___STATIC__STRING);

    //--//
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

#endif //_SYS_IO_SER_NATIVE_H_
