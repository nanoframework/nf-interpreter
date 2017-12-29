//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIN_DEV_SERIAL_NATIVE_H_
#define _WIN_DEV_SERIAL_NATIVE_H_


#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>
#include <hal.h>


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

struct Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___opened = 2;
    static const int FIELD___deviceId = 3;
    static const int FIELD___portIndex = 4;
    static const int FIELD___outputStream = 5;
    static const int FIELD___inputStream = 6;
    static const int FIELD___readTimeout = 7;
    static const int FIELD___writeTimeout = 8;
    static const int FIELD___baudRate = 9;
    static const int FIELD___dataBits = 10;
    static const int FIELD___handshake = 11;
    static const int FIELD___parity = 12;
    static const int FIELD___stopBits = 13;
    static const int FIELD___bytesReceived = 14;
    static const int FIELD___disposedValue = 15;

    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID);
    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeWrite___VOID__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(GetDeviceSelector___STATIC__STRING);

    //--//

};

struct Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDeviceController
{
    static const int FIELD_STATIC__s_instance = 0;
    static const int FIELD_STATIC__s_deviceCollection = 1;


    //--//

};

struct Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDeviceOutputStream
{
    static const int FIELD___serialDevice = 1;


    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_SerialCommunication;

#endif  //_WIN_DEV_SERIAL_NATIVE_H_
