//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"

#include <nanoPAL.h>


HAL_DblLinkedList<HAL_CONTINUATION> g_HAL_Completion_List;

/***************************************************************************/

void HAL_COMPLETION::Execute()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();

#if defined(_DEBUG)
    this->Start_RTC_Ticks = 0;
#endif

    if(this->ExecuteInISR)
    {
        HAL_CONTINUATION* cont = this;

        cont->Execute();
    }
    else
    {
        this->Enqueue();
    }
}

//--//

void HAL_COMPLETION::InitializeList()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
    g_HAL_Completion_List.Initialize();
}

//--//

void HAL_COMPLETION::DequeueAndExec()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();

    NANOCLR_LOCKED_ACCESS_DECLARATION(HAL_COMPLETION*, ptr);
    NANOCLR_LOCKED_ACCESS_DECLARATION(HAL_COMPLETION*, ptrNext);

    NANOCLR_LOCKED_ACCESS_EXECUTE(ptr, (HAL_COMPLETION*)g_HAL_Completion_List.FirstNode());
    NANOCLR_LOCKED_ACCESS_EXECUTE(ptrNext, (HAL_COMPLETION*)(NANOCLR_LOCKED_ACCESS_GET(ptr))->Next());

    // waitforevents does not have an associated completion, therefore we need to verify
    // than their is a next completion and that the current one has expired.
    if(NANOCLR_LOCKED_ACCESS_GET(ptrNext))
    {
        Events_Set(SYSTEM_EVENT_FLAG_SYSTEM_TIMER);

        NANOCLR_LOCKED_ACCESS_GET(ptr)->Unlink();

        //
        // In case there's no other request to serve, set the next interrupt to be 356 years since last powerup (@25kHz).
        //
        Time_SetCompare( NANOCLR_LOCKED_ACCESS_GET(ptrNext)->Next() ? NANOCLR_LOCKED_ACCESS_GET(ptrNext)->EventTimeTicks : HAL_COMPLETION_IDLE_VALUE );

  #if defined(_DEBUG)
        NANOCLR_LOCKED_ACCESS_GET(ptr)->EventTimeTicks = 0;
  #endif  // defined(_DEBUG)

        //// let the ISR turn on interrupts, if it needs to
        NANOCLR_LOCKED_ACCESS_GET(ptr)->Execute();
    }
}

//--//

void HAL_COMPLETION::EnqueueTicks( uint64_t eventTimeTicks )
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
    ASSERT(eventTimeTicks != 0);

    NANOCLR_LOCKED_ACCESS_DECLARATION(HAL_COMPLETION*, ptr);
    NANOCLR_LOCKED_ACCESS_DECLARATION(HAL_COMPLETION*, ptrNext);

    this->EventTimeTicks  = eventTimeTicks;
#if defined(_DEBUG)
    this->Start_RTC_Ticks = HAL_Time_CurrentSysTicks();
#endif

    NANOCLR_LOCKED_ACCESS_EXECUTE(ptr, (HAL_COMPLETION*)g_HAL_Completion_List.FirstNode());

    do
    {
        NANOCLR_LOCKED_ACCESS_EXECUTE(ptrNext, (HAL_COMPLETION*)(NANOCLR_LOCKED_ACCESS_GET(ptr))->Next());

        if( eventTimeTicks < NANOCLR_LOCKED_ACCESS_GET(ptr)->EventTimeTicks )
        {
            break;
        }

        NANOCLR_LOCKED_ACCESS_GET(ptr) = NANOCLR_LOCKED_ACCESS_GET(ptrNext);
    }
    while(NANOCLR_LOCKED_ACCESS_GET(ptr));
    

    GLOBAL_LOCK();
    g_HAL_Completion_List.InsertBeforeNode( NANOCLR_LOCKED_ACCESS_GET(ptr), this );
    GLOBAL_UNLOCK();

    NANOCLR_LOCKED_ACCESS_EXECUTE(ptr, (HAL_COMPLETION*)g_HAL_Completion_List.FirstNode());

    if(this == NANOCLR_LOCKED_ACCESS_GET(ptr))
    {
        Time_SetCompare( eventTimeTicks );
    }
}

//--//

void HAL_COMPLETION::Abort()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();

    NANOCLR_LOCKED_ACCESS_DECLARATION(HAL_COMPLETION*, firstNode);

    NANOCLR_LOCKED_ACCESS_EXECUTE(firstNode, (HAL_COMPLETION*)g_HAL_Completion_List.FirstNode());

    this->Unlink();

#if defined(_DEBUG)
    this->Start_RTC_Ticks = 0;
#endif

    if(NANOCLR_LOCKED_ACCESS_GET(firstNode) == this)
    {
        uint64_t nextTicks;

        GLOBAL_LOCK();
        bool completionListIsEmpty = g_HAL_Completion_List.IsEmpty();
        GLOBAL_UNLOCK();

        if(completionListIsEmpty)
        {
            //
            // In case there's no other request to serve, set the next interrupt to be 356 years since last power up (@25kHz).
            //
            nextTicks = HAL_COMPLETION_IDLE_VALUE;
        }
        else
        {
            NANOCLR_LOCKED_ACCESS_EXECUTE(firstNode, (HAL_COMPLETION*)g_HAL_Completion_List.FirstNode());
            //firstNode = (HAL_COMPLETION*)g_HAL_Completion_List.FirstNode();

            nextTicks = NANOCLR_LOCKED_ACCESS_GET(firstNode)->EventTimeTicks;
        }

        Time_SetCompare( nextTicks );
    }
}

//--//

void HAL_COMPLETION::WaitForInterrupts(uint64_t expireTimeInTicks, uint32_t sleepLevel, uint64_t wakeEvents)
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();

    const int setCompare   = 1;
    const int resetCompare = 2;
    const int nilCompare   = 4;

    HAL_COMPLETION* ptr = (HAL_COMPLETION*)g_HAL_Completion_List.FirstNode();
    int             state;

    if(ptr->Next() == NULL)
    {
        state = setCompare | nilCompare;
    }
    else if(ptr->EventTimeTicks > expireTimeInTicks)
    {
        state = setCompare | resetCompare;
    }
    else
    {
        state = 0;
    }

    if(state & setCompare)
    {
        Time_SetCompare(expireTimeInTicks);
    }

    CPU_Sleep((SLEEP_LEVEL_type)sleepLevel, wakeEvents);

    if(state & (resetCompare | nilCompare))
    {   
        // let's get the first node again
        // it could have changed since CPU_Sleep re-enabled interrupts
        ptr = (HAL_COMPLETION*)g_HAL_Completion_List.FirstNode();
        Time_SetCompare((state & resetCompare) ? ptr->EventTimeTicks : HAL_COMPLETION_IDLE_VALUE);
    }
}

void HAL_COMPLETION::Uninitialize()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
    
    HAL_COMPLETION* ptr;

    while(TRUE)
    {
        GLOBAL_LOCK();
        ptr = (HAL_COMPLETION*)g_HAL_Completion_List.ExtractFirstNode();
        GLOBAL_UNLOCK();

        if(!ptr) 
        {
            break;
        }
        
    }
}
