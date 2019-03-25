//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"
#include <nanoPAL.h>

HAL_DblLinkedList<HAL_CONTINUATION> g_HAL_Continuation_List;
extern HAL_DblLinkedList<HAL_CONTINUATION> g_HAL_Completion_List;

//--//

bool HAL_CONTINUATION::IsLinked()
{
    return ((HAL_DblLinkedNode<HAL_CONTINUATION>*)this)->IsLinked();
}

void HAL_CONTINUATION::InitializeList()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
    g_HAL_Continuation_List.Initialize();
}

void HAL_CONTINUATION::Enqueue()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
    if(this->GetEntryPoint() != NULL)
    {
        GLOBAL_LOCK(irq);

        g_HAL_Continuation_List.LinkAtBack( this );

        GLOBAL_UNLOCK(irq);
    }
}

void HAL_CONTINUATION::Abort()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
    GLOBAL_LOCK(irq);

    this->Unlink();

    GLOBAL_UNLOCK(irq);
}

bool HAL_CONTINUATION::Dequeue_And_Execute()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
    GLOBAL_LOCK(irq);
    
    HAL_CONTINUATION* ptr = g_HAL_Continuation_List.ExtractFirstNode();
    if(ptr == NULL )
    {
        GLOBAL_UNLOCK(irq);
        return FALSE;
    }
    
    SystemState_SetNoLock( SYSTEM_STATE_NO_CONTINUATIONS );

    HAL_CALLBACK call = ptr->Callback;

    GLOBAL_UNLOCK(irq);

    call.Execute();

    GLOBAL_LOCK(irq);

    SystemState_ClearNoLock( SYSTEM_STATE_NO_CONTINUATIONS );   // nestable

    GLOBAL_UNLOCK(irq);
    return TRUE;
}

void HAL_CONTINUATION::InitializeCallback( HAL_CALLBACK_FPN entryPoint, void* argument )
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
    Initialize();
    
    Callback.Initialize( entryPoint, argument );
}

void HAL_CONTINUATION::Uninitialize()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
    GLOBAL_LOCK(irq);
    
    HAL_CONTINUATION* ptr;

    while(TRUE)
    {
        ptr = (HAL_CONTINUATION*)g_HAL_Continuation_List.ExtractFirstNode();
        
        if(!ptr)
        {
            break;
        }
    }
    
    GLOBAL_UNLOCK(irq);
}
