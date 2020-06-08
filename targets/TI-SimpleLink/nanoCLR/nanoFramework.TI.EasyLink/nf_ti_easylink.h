//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NF_TI_EASYLINK_H_
#define _NF_TI_EASYLINK_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoHAL.h>

// TI-RTOS Header files
#include <ti/drivers/rf/RF.h>

// Board Header files
#include <Board.h>

// EasyLink API Header files
#include <easylink/EasyLink.h>

struct Library_nf_ti_easylink_nanoFramework_TI_EasyLink_TransmitPacket
{
    static const int FIELD___address = 1;
    static const int FIELD___payload = 2;

    //--//

};

struct Library_nf_ti_easylink_nanoFramework_TI_EasyLink_ReceivedPacket
{
    static const int FIELD___address = 1;
    static const int FIELD___payload = 2;
    static const int FIELD___rssi = 3;
    static const int FIELD___absoluteTime = 4;
    static const int FIELD___rxTimeout = 5;

    //--//

};

struct Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___disposed = 2;
    static const int FIELD___initialized = 3;
    static const int FIELD___phyType = 4;
    static const int FIELD___addressFilter = 5;

    NANOCLR_NATIVE_DECLARE(get_AbsoluteTime___U4);
    NANOCLR_NATIVE_DECLARE(get_Rssi___I1);
    NANOCLR_NATIVE_DECLARE(get_Frequency___U4);
    NANOCLR_NATIVE_DECLARE(get_RfPower___I1);
    NANOCLR_NATIVE_DECLARE(get_IeeeAddress___SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(GetConfiguration___U4__nanoFrameworkTIEasyLinkControlOption);
    NANOCLR_NATIVE_DECLARE(DisposeNative___VOID);
    NANOCLR_NATIVE_DECLARE(UpdateRxAddressFilterNative___VOID);
    NANOCLR_NATIVE_DECLARE(InitNative___U1);
    NANOCLR_NATIVE_DECLARE(ReceiveNative___U1__BYREF_nanoFrameworkTIEasyLinkReceivedPacket__I4);
    NANOCLR_NATIVE_DECLARE(SetConfigurationNative___U1__nanoFrameworkTIEasyLinkControlOption__U4);
    NANOCLR_NATIVE_DECLARE(SetFrequencyNative___U1__U4);
    NANOCLR_NATIVE_DECLARE(SetRfPowerNative___U1__I1);
    NANOCLR_NATIVE_DECLARE(TransmitNative___U1__nanoFrameworkTIEasyLinkTransmitPacket__I4__I4);

    //--//

    static HRESULT UpdateRxAddressFilter( CLR_RT_StackFrame& stack );

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_TI_EasyLink;

#endif  //_NF_TI_EASYLINK_H_
