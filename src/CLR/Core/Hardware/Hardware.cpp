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
        m_MessagingEventsMask = 0;

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
        m_DebuggerEventsMask |= ExtractEventFromTransport( HalSystemConfig.DebuggerPort );
#endif

        m_MessagingEventsMask |= ExtractEventFromTransport( HalSystemConfig.MessagingPort );

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

    unsigned int events    = ::Events_Get( m_wakeupEvents );    
    unsigned int eventsCLR = 0;

    if(events & m_MessagingEventsMask)
    {
        //msg.ProcessCommands();
    }

    if(events & m_DebuggerEventsMask)
    {
        //dbg.ProcessCommands();

#if defined(PLATFORM_ARM)
        if(CLR_EE_DBG_IS(RebootPending))
        {
#if !defined(BUILD_RTM)
            CLR_Debug::Printf( "Rebooting...\r\n" );
#endif

            if(!CLR_EE_REBOOT_IS(ClrOnly))
            {
                CLR_RT_ExecutionEngine::Reboot( true );
            }
        }
#endif
    }

    if( events & (SYSTEM_EVENT_FLAG_COM_IN | SYSTEM_EVENT_FLAG_COM_OUT) )
    {
        eventsCLR |= CLR_RT_ExecutionEngine::c_Event_SerialPort;
    }

    // UNDONE: FIXME: if(events & SYSTEM_EVENT_I2C_XACTION)
    //{
    //    eventsCLR |= CLR_RT_ExecutionEngine::c_Event_I2C;
    //}

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

    if(events & SYSTEM_EVENT_FLAG_IO)
    {
        eventsCLR |= CLR_RT_ExecutionEngine::c_Event_IO;
    }

    // UNDONE: FIXME: if(events & SYSTEM_EVENT_FLAG_CHARGER_CHANGE)
    //{
    //    static unsigned int lastStatus;
    //    unsigned int        status;

    //    if(::Charger_Status( status ))
    //    {
    //        status &= CHARGER_STATUS_ON_AC_POWER;

    //        if(lastStatus != status)
    //        {
    //            lastStatus = status;

    //            eventsCLR |= CLR_RT_ExecutionEngine::c_Event_Battery;
    //        }
    //    }
    //}

    if(eventsCLR)
    {
        g_CLR_RT_ExecutionEngine.SignalEvents( eventsCLR );
    }
}

//--//

// UNDONE: FIXME: void CLR_HW_Hardware::Screen_Flush( CLR_GFX_Bitmap& bitmap, CLR_UINT16 x, CLR_UINT16 y, CLR_UINT16 width, CLR_UINT16 height )
//{
//    NATIVE_PROFILE_CLR_HARDWARE();
//    CLR_INT32 widthMax  = LCD_SCREEN_WIDTH;
//    CLR_INT32 heightMax = LCD_SCREEN_HEIGHT;
//
//    if((CLR_UINT32)(x + width)  > bitmap.m_bm.m_width ) width  = bitmap.m_bm.m_width  - x;
//    if((CLR_UINT32)(y + height) > bitmap.m_bm.m_height) height = bitmap.m_bm.m_height - y;    
//
//    if(bitmap.m_bm.m_width                 != widthMax                              ) return;
//    if(bitmap.m_bm.m_height                != heightMax                             ) return;
//    if(bitmap.m_bm.m_bitsPerPixel          != CLR_GFX_BitmapDescription::c_NativeBpp) return;
//    if(bitmap.m_palBitmap.transparentColor != PAL_GFX_Bitmap::c_InvalidColor        ) return;
//
//    LCD_BitBltEx( x, y, width, height, bitmap.m_palBitmap.data );
//}

//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//

