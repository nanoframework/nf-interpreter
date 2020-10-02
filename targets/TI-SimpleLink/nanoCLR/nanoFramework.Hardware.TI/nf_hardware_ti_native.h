//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NF_HARDWARE_TI_NATIVE_H_
#define _NF_HARDWARE_TI_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

#include <ti_drivers_config.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <EasyLink.h>

// clang-format off
#include DeviceFamily_constructPath(inc/hw_prcm.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
// clang-format on

typedef enum __nfpack Power_PinPullUpDown
{
    Power_PinPullUpDown_None = 2147483648,
    Power_PinPullUpDown_PullUp = 2147491840,
    Power_PinPullUpDown_PullDown = 2147500032,
} Power_PinPullUpDown;

typedef enum __nfpack Power_PinWakeupEdge
{
    Power_PinWakeupEdge_None = 0,
    Power_PinWakeupEdge_PositiveEdge = 402653184,
    Power_PinWakeupEdge_NegativeEdge = 268435456,
} Power_PinWakeupEdge;

struct Library_nf_hardware_ti_native_nanoFramework_Hardware_TI_Power
{
    NANOCLR_NATIVE_DECLARE(
        ConfigureWakeupFromGpioPin___STATIC__VOID__SZARRAY_nanoFrameworkHardwareTIPowerPinWakeupConfig);
    NANOCLR_NATIVE_DECLARE(get_SourceOfReset___STATIC__nanoFrameworkHardwareTIPowerResetSource);
    NANOCLR_NATIVE_DECLARE(NativeEnterShutdownMode___STATIC__VOID);
    NANOCLR_NATIVE_DECLARE(NativeEnterStandbyMode___STATIC__VOID__SystemTimeSpan);

    //--//
};

struct Library_nf_hardware_ti_native_nanoFramework_Hardware_TI_Power__PinWakeupConfig
{
    static const int FIELD___pin = 1;
    static const int FIELD___wakeupConfig = 2;
    static const int FIELD___pinPullUpDown = 3;

    //--//
};

struct Library_nf_hardware_ti_native_nanoFramework_Hardware_TI_Utilities
{
    static const int FIELD_STATIC___deviceIeeeAddress = 0;

    NANOCLR_NATIVE_DECLARE(NativeGetIeeeAddress___STATIC__SZARRAY_U1);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_TI;

#endif //_NF_HARDWARE_TI_NATIVE_H_
