//
// Copyright (c) .NET Foundation and Contributors
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
        GLOBAL_LOCK();

        g_HAL_Continuation_List.LinkAtBack( this );

        GLOBAL_UNLOCK();
    }
}

void HAL_CONTINUATION::Abort()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
    GLOBAL_LOCK();

    this->Unlink();

    GLOBAL_UNLOCK();
}

bool HAL_CONTINUATION::Dequeue_And_Execute()
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();

    // use this as the return value
    // helpfull to make the call to release the global mutext happens 
    bool result;

    HAL_CONTINUATION* ptr = NULL;
    GLOBAL_LOCK();
    ptr = g_HAL_Continuation_List.ExtractFirstNode();
    GLOBAL_UNLOCK();

    if(ptr == NULL )
    {
        result = false;
    }
    else
    {
        //SystemState_SetNoLock( SYSTEM_STATE_NO_CONTINUATIONS );

        HAL_CALLBACK call = ptr->Callback;

        call.Execute();

        //SystemState_ClearNoLock( SYSTEM_STATE_NO_CONTINUATIONS );   // nestable

        result = true;
    }

    return result;
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
    GLOBAL_LOCK();
    
    HAL_CONTINUATION* ptr;

    while(TRUE)
    {
        ptr = (HAL_CONTINUATION*)g_HAL_Continuation_List.ExtractFirstNode();
        
        if(!ptr)
        {
            break;
        }
    }
    
    GLOBAL_UNLOCK();
}
