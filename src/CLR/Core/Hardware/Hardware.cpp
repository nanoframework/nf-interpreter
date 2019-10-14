//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include <nanoCLR_Hardware.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CLR_HW_Hardware::CreateInstance()
{
    NATIVE_PROFILE_CLR_HARDWARE();

    NANOCLR_HEADER();

    NANOCLR_CLEAR(g_CLR_HW_Hardware);

    g_CLR_HW_Hardware.m_fInitialized = false;

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT CLR_HW_Hardware::Hardware_Initialize()
{
    NATIVE_PROFILE_CLR_HARDWARE();

    NANOCLR_HEADER();

    if(m_fInitialized == false)
    {
        Time_Initialize();

        m_interruptData.m_HalQueue.Initialize( (CLR_HW_Hardware::HalInterruptRecord*)&g_scratchInterruptDispatchingStorage, InterruptRecords() );

        m_interruptData.m_applicationQueue.DblLinkedList_Initialize ();

        m_interruptData.m_queuedInterrupts = 0;

        m_DebuggerEventsMask  = 0;

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
        m_DebuggerEventsMask |= ExtractEventFromTransport( HalSystemConfig.DebuggerPort );
#endif

        m_wakeupEvents = c_Default_WakeupEvents | m_DebuggerEventsMask;
        m_powerLevel   = PowerLevel__Active;
            
        m_fInitialized = true;
    }

    NANOCLR_NOCLEANUP_NOLABEL();
}

//--//

HRESULT CLR_HW_Hardware::DeleteInstance()
{
    NATIVE_PROFILE_CLR_HARDWARE();

    NANOCLR_HEADER();

    g_CLR_HW_Hardware.Hardware_Cleanup();

    NANOCLR_NOCLEANUP_NOLABEL();
}

void CLR_HW_Hardware::Hardware_Cleanup()
{
    NATIVE_PROFILE_CLR_HARDWARE();

    if(m_fInitialized == true)
    {
        m_fInitialized = false;
    }
}

//--//

void CLR_HW_Hardware::PrepareForGC()
{
    NATIVE_PROFILE_CLR_HARDWARE();
}

void CLR_HW_Hardware::ProcessActivity()
{
    NATIVE_PROFILE_CLR_HARDWARE();

    for(int i=0; i<10; i++)
    {
        if(!HAL_CONTINUATION::Dequeue_And_Execute()) break;
    }

	// UNDONE: FIXME:
    //if(!msg.IsDebuggerInitialized())
    //{
    //    msg.InitializeDebugger();
    //}
	// UNDONE: FIXME:msg.PurgeCache();
    
	// UNDONE: FIXME:dbg.PurgeCache();

    uint32_t events    = ::Events_Get( m_wakeupEvents );    
    uint32_t eventsCLR = 0;

    if(events & m_DebuggerEventsMask)
    {
        //dbg.ProcessCommands();

#if defined(PLATFORM_ARM) || defined(PLATFORM_ESP32)
        if(CLR_EE_DBG_IS(RebootPending))
        {
#if !defined(BUILD_RTM)
            CLR_Debug::Printf( "Rebooting...\r\n" );
#endif

            if(!CLR_EE_REBOOT_IS(ClrOnly))
            {
                CLR_RT_ExecutionEngine::Reboot( false );
            }
        }
#endif
    }

    if(events & SYSTEM_EVENT_FLAG_COM_IN)
    {
        eventsCLR |= CLR_RT_ExecutionEngine::c_Event_SerialPortIn;
    }

    if(events & SYSTEM_EVENT_FLAG_COM_OUT)
    {
        eventsCLR |= CLR_RT_ExecutionEngine::c_Event_SerialPortOut;
    }

    if((events & SYSTEM_EVENT_HW_INTERRUPT)
#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
        || (!CLR_EE_DBG_IS(Stopped) && !g_CLR_HW_Hardware.m_interruptData.m_applicationQueue.IsEmpty())
#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
        )
    {
        ProcessInterrupts();
    }

    if(events & SYSTEM_EVENT_FLAG_SOCKET)
    {
         eventsCLR |= CLR_RT_ExecutionEngine::c_Event_Socket;
    }

    if(events & SYSTEM_EVENT_FLAG_SPI_MASTER)
    {
        eventsCLR |= CLR_RT_ExecutionEngine::c_Event_SpiMaster;
    }

    if(events & SYSTEM_EVENT_FLAG_I2C_MASTER)
    {
        eventsCLR |= CLR_RT_ExecutionEngine::c_Event_I2cMaster;
    }

    if(events & SYSTEM_EVENT_FLAG_ONEWIRE_MASTER)
    {
        eventsCLR |= CLR_RT_ExecutionEngine::c_Event_OneWireMaster;
    }

    if(events & SYSTEM_EVENT_FLAG_STORAGE_IO)
    {
        eventsCLR |= CLR_RT_ExecutionEngine::c_Event_StorageIo;
    }

    if(eventsCLR)
    {
        g_CLR_RT_ExecutionEngine.SignalEvents( eventsCLR );
    }
}

//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//

