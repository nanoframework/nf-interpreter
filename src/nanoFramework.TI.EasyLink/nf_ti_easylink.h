//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NF_TI_EASYLINK_H_
#define _NF_TI_EASYLINK_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoHAL.h>

struct Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___phyType = 2;
    static const int FIELD___addressFilter = 3;
    static const int FIELD___disposedValue = 4;

    NANOCLR_NATIVE_DECLARE(get_AbsoluteTime___U4);
    NANOCLR_NATIVE_DECLARE(get_Rssi___I1);
    NANOCLR_NATIVE_DECLARE(get_Frequency___U4);
    NANOCLR_NATIVE_DECLARE(set_Frequency___VOID__U4);
    NANOCLR_NATIVE_DECLARE(get_RfPower___I1);
    NANOCLR_NATIVE_DECLARE(set_RfPower___VOID__I1);
    NANOCLR_NATIVE_DECLARE(get_IeeeAddress___SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(SetConfiguration___nanoFrameworkTIEasyLinkStatus__nanoFrameworkTIEasyLinkControlOption__U4);
    NANOCLR_NATIVE_DECLARE(GetConfiguration___U4__nanoFrameworkTIEasyLinkControlOption);
    NANOCLR_NATIVE_DECLARE(InitNative___STATIC__VOID);
    NANOCLR_NATIVE_DECLARE(DisposeNative___STATIC__VOID);
    NANOCLR_NATIVE_DECLARE(EnableRxAddressFilterNative___STATIC__VOID);
    NANOCLR_NATIVE_DECLARE(TransmitNative___STATIC__nanoFrameworkTIEasyLinkStatus__nanoFrameworkTIEasyLinkTransmitPacket__mscorlibSystemTimeSpan__I4);
    NANOCLR_NATIVE_DECLARE(ReceiveNative___STATIC__nanoFrameworkTIEasyLinkStatus__BYREF_nanoFrameworkTIEasyLinkReceivedPacket__mscorlibSystemTimeSpan);

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

struct Library_nf_ti_easylink_nanoFramework_TI_EasyLink_TransmitPacket
{
    static const int FIELD___address = 1;
    static const int FIELD___payload = 2;


    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_TI_EasyLink;

#endif  //_NF_TI_EASYLINK_H_
