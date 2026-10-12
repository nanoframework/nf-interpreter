#include "nf_device_can_native.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    Library_nf_device_can_native_nanoFramework_Device_Can_CanController::DisposeNative___VOID,
    Library_nf_device_can_native_nanoFramework_Device_Can_CanController::GetDeviceSelector___STATIC__STRING,
    Library_nf_device_can_native_nanoFramework_Device_Can_CanController::GetMessage___nanoFrameworkDeviceCanCanMessage,
    Library_nf_device_can_native_nanoFramework_Device_Can_CanController::NativeInit___VOID,
    Library_nf_device_can_native_nanoFramework_Device_Can_CanController::NativeUpdateCallbacks___VOID,
    Library_nf_device_can_native_nanoFramework_Device_Can_CanController::WriteMessage___VOID__nanoFrameworkDeviceCanCanMessage,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Device_Can =
{
    "nanoFramework.Device.Can",
    0x55484877,
    method_lookup,
    ARRAYSIZE(method_lookup)
};

// clang-format on
