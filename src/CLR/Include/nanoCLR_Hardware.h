//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOCLR_HARDWARE_H_
#define _NANOCLR_HARDWARE_H_

#include <nanoCLR_Types.h>
#include <nanoCLR_Runtime.h>
//#include <nanoCLR_Graphics.h>

//--//
























#if defined(_WIN32)
#include "nanoCLR_Win32.h"
#endif

//--//

struct CLR_HW_Hardware
{
    static const CLR_UINT32 c_Default_PowerLevel   = PowerLevel__Sleep; 
    static const CLR_UINT32 c_Default_WakeupEvents = SYSTEM_EVENT_FLAG_COM_IN            |
                                                     SYSTEM_EVENT_FLAG_COM_OUT           |
                                                     SYSTEM_EVENT_FLAG_STORAGE_IO        |
                                                     SYSTEM_EVENT_FLAG_SYSTEM_TIMER      |
                                                     SYSTEM_EVENT_FLAG_SPI_MASTER        |
                                                     SYSTEM_EVENT_FLAG_I2C_MASTER        |
                                                     SYSTEM_EVENT_HW_INTERRUPT           |
                                                     SYSTEM_EVENT_FLAG_SOCKET            |
                                                     SYSTEM_EVENT_FLAG_DEBUGGER_ACTIVITY |
                                                     SYSTEM_EVENT_FLAG_MESSAGING_ACTIVITY|
                                                     SYSTEM_EVENT_FLAG_ONEWIRE_MASTER;

    //--//

    struct HalInterruptRecord
    {
        CLR_INT64              m_time;
        CLR_RT_HeapBlock_Node* m_context;

        CLR_UINT32             m_data1;
        CLR_UINT32             m_data2;
        CLR_UINT32             m_data3;
    
        void AssignFrom( const HalInterruptRecord &recFrom )
        {
            m_time      = recFrom.m_time;
            m_context   = recFrom.m_context;
            m_data1     = recFrom.m_data1;
            m_data2     = recFrom.m_data2;
        }
    };

    struct InterruptData
    {
        Hal_Queue_UnknownSize<HalInterruptRecord> m_HalQueue;
        CLR_RT_DblLinkedList                      m_applicationQueue;
        CLR_UINT32                                m_queuedInterrupts;
    };

    //--//

    bool          m_fInitialized;

    InterruptData m_interruptData;

    CLR_UINT32    m_DebuggerEventsMask;

    PowerLevel_type    m_powerLevel;
    CLR_UINT32    m_wakeupEvents;
    
    //--//

     static HRESULT CreateInstance();

     HRESULT Hardware_Initialize();

     static HRESULT DeleteInstance();

     void Hardware_Cleanup();

     void PrepareForGC   ();
     void ProcessActivity();

     HRESULT ManagedHardware_Initialize();
     HRESULT ProcessInterrupts();
     HRESULT SpawnDispatcher();
     HRESULT TransferAllInterruptsToApplicationQueue();
};

extern CLR_HW_Hardware g_CLR_HW_Hardware;

//--//


////////////////////////////////////////////////////////////////////////////////////////
// keep under control the size of the HalInterruptRecord, since we will use externally
// defined arrays to handle those data structures in the interrupt dispatching

CT_ASSERT(sizeof(CLR_HW_Hardware::HalInterruptRecord) == 24)

////////////////////////////////////////////////////////////////////////////////////////

//--//

#endif // _NANOCLR_HARDWARE_H_

