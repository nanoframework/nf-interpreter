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


#ifndef _WIN_DEV_I2C_NATIVE_H_
#define _WIN_DEV_I2C_NATIVE_H_

struct Library_win_dev_i2c_native_Windows_Devices_I2c_I2cConnectionSettings
{
    static const int FIELD___slaveAddress = 1;
    static const int FIELD___busSpeed = 2;
    static const int FIELD___sharingMode = 3;


    //--//

};

struct Library_win_dev_i2c_native_Windows_Devices_I2c_I2cController
{
    static const int FIELD_STATIC___syncLock = 0;
    static const int FIELD_STATIC__s_instance = 1;
    static const int FIELD_STATIC__s_deviceCollection = 2;
    static const int FIELD_STATIC__s_busIdCollection = 3;


    //--//

};

struct Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___deviceId = 2;
    static const int FIELD___connectionSettings = 3;
    static const int FIELD___disposed = 4;

    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(DisposeNative___VOID);
    NANOCLR_NATIVE_DECLARE(NativeTransmit___WindowsDevicesI2cI2cTransferResult__SZARRAY_U1__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(GetDeviceSelector___STATIC__STRING);

    //--//

};

struct Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult
{
    static const int FIELD___bytesTransferred = 1;
    static const int FIELD___status = 2;


    //--//

};

#endif  //_WIN_DEV_I2C_NATIVE_H_
