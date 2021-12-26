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

#ifndef _NANOFRAMEWORK_HARDWARE_ESP32_ESPNOW_NATIVE_H_
#define _NANOFRAMEWORK_HARDWARE_ESP32_ESPNOW_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

#include <esp_wifi.h>
#include <esp_now.h>

struct Library_nanoFramework_hardware_esp32_espnow_native_nanoFramework_Hardware_Esp32_EspNow_DataReceivedEventArgs
{
    static const int FIELD__PeerMac = 1;
    static const int FIELD__Data = 2;
    static const int FIELD__DataLen = 3;

    //--//

};

struct Library_nanoFramework_hardware_esp32_espnow_native_nanoFramework_Hardware_Esp32_EspNow_DataSentEventArgs
{
    static const int FIELD__PeerMac = 1;
    static const int FIELD__Status = 2;

    //--//

};

struct Library_nanoFramework_hardware_esp32_espnow_native_nanoFramework_Hardware_Esp32_EspNow_EspNowController
{
    static const int FIELD__DataSent = 1;
    static const int FIELD__DataReceived = 2;
    static const int FIELD__isDisposed = 3;

    NANOCLR_NATIVE_DECLARE(NativeEspNowInit___I4);
    NANOCLR_NATIVE_DECLARE(NativeEspNowDeinit___I4);
    NANOCLR_NATIVE_DECLARE(NativeEspNowRegisterRecvCb___I4__nanoFrameworkHardwareEsp32EspNowEspNowControllerRegisterRecvDelegate);
    NANOCLR_NATIVE_DECLARE(NativeEspNowUnregisterRecvCb___I4);
    NANOCLR_NATIVE_DECLARE(NativeEspNowRegisterSendCb___I4__nanoFrameworkHardwareEsp32EspNowEspNowControllerRegisterSendDelegate);
    NANOCLR_NATIVE_DECLARE(NativeEspNowUnregisterSendCb___I4);
    NANOCLR_NATIVE_DECLARE(NativeEspNowSend___I4__SZARRAY_U1__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(NativeEspNowAddPeer___I4__SZARRAY_U1__U1);

    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_Esp32_EspNow;

#endif  //_NANOFRAMEWORK_HARDWARE_ESP32_ESPNOW_NATIVE_H_
