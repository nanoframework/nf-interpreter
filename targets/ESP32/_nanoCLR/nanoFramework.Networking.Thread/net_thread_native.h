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

#ifndef NET_THREAD_NATIVE_H
#define NET_THREAD_NATIVE_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

typedef enum __nfpack ThreadState
{
    ThreadState_Idle = 0,
    ThreadState_EndDevice = 1,
    ThreadState_Router = 2,
    ThreadState_Leader = 3,
} ThreadState;

struct Library_net_thread_native_nanoFramework_Networking_Thread_Dataset
{
    static const int FIELD__networkName = 1;
    static const int FIELD__networkKey = 2;
    static const int FIELD__extendedPanId = 3;
    static const int FIELD__pskc = 4;
    static const int FIELD__channelMask = 5;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Networking_Thread;

#endif // NET_THREAD_NATIVE_H
