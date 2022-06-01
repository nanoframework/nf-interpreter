#include "nf_device_can_native.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_device_can_native_nanoFramework_Device_Can_CanController::WriteMessage___VOID__nanoFrameworkDeviceCanCanMessage,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_device_can_native_nanoFramework_Device_Can_CanController::GetMessage___nanoFrameworkDeviceCanCanMessage,
    Library_nf_device_can_native_nanoFramework_Device_Can_CanController::DisposeNative___VOID,
    Library_nf_device_can_native_nanoFramework_Device_Can_CanController::NativeInit___VOID,
    Library_nf_device_can_native_nanoFramework_Device_Can_CanController::NativeUpdateCallbacks___VOID,
    NULL,
    NULL,
    Library_nf_device_can_native_nanoFramework_Device_Can_CanController::GetDeviceSelector___STATIC__STRING,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Device_Can =
{
    "nanoFramework.Device.Can",
    0x5B569036,
    method_lookup,
    { 100, 0, 5, 1 }
};

// clang-format on
