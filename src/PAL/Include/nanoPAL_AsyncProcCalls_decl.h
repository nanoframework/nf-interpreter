//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOPAL_ASYNCPROCCALLS_DECL_H_
#define _NANOPAL_ASYNCPROCCALLS_DECL_H_ 1

#include <nanoWeak.h>

//--//

#if NATIVE_PROFILE_PAL & NATIVE_PROFILE_PAL_ASYNC_PROC_CALL__flag
    #define NATIVE_PROFILE_PAL_ASYNC_PROC_CALL() Native_Profiler profiler_obj
#else
    #define NATIVE_PROFILE_PAL_ASYNC_PROC_CALL()
#endif

typedef void (*HAL_CALLBACK_FPN)( void* arg );

struct HAL_CALLBACK
{
public:
    HAL_CALLBACK_FPN EntryPoint;
    void*            Argument;

public:
    void Initialize( HAL_CALLBACK_FPN EntryPoint, void* Argument )
    {
        this->EntryPoint = EntryPoint;
        this->Argument   = Argument;
    }

    void SetArgument( void* Argument )
    {
        this->Argument = Argument;
    }

    HAL_CALLBACK_FPN GetEntryPoint() const { return this->EntryPoint; }
    void*            GetArgument  () const { return this->Argument  ; }

    void Execute() const
    {
        HAL_CALLBACK_FPN EntryPoint = this->EntryPoint;
        void*            Argument   = this->Argument; 

        if(EntryPoint)
        {
            EntryPoint( Argument );
        }
    }
};


struct HAL_CONTINUATION : public HAL_DblLinkedNode<HAL_CONTINUATION>
{

private:
    HAL_CALLBACK Callback;

public:
    void InitializeCallback( HAL_CALLBACK_FPN EntryPoint, void* Argument = NULL );

    void SetArgument( void* Argument )
    {
        Callback.SetArgument( Argument );
    }

    HAL_CALLBACK_FPN GetEntryPoint() const { return Callback.GetEntryPoint(); }
    void*            GetArgument  () const { return Callback.GetArgument  (); }

    void Execute() const { Callback.Execute(); }

    bool IsLinked();
    void Enqueue();
    void Abort  ();

    //--//

    static void Uninitialize();

    static void InitializeList();

    static bool Dequeue_And_Execute();
};

//--//

struct HAL_COMPLETION : public HAL_CONTINUATION
{
    uint64_t EventTimeTicks;
    bool   ExecuteInISR;

#if defined(_DEBUG)
    uint64_t Start_RTC_Ticks;
#endif

    void InitializeForISR( HAL_CALLBACK_FPN EntryPoint, void* Argument = NULL )
    {
        ExecuteInISR = true;

        InitializeCallback( EntryPoint, Argument );
    }

    void InitializeForUserMode( HAL_CALLBACK_FPN EntryPoint, void* Argument = NULL )
    {
        ExecuteInISR = false;

        InitializeCallback( EntryPoint, Argument );
    }

    void EnqueueTicks               ( uint64_t eventTimeTicks        );
    void EnqueueDelta64             ( uint64_t miliSecondsFromNow    );
    void EnqueueDelta               ( uint32_t miliSecondsFromNow        );
    
    void Abort();

    void Execute();

    //--//

    static void Uninitialize();

    static void InitializeList();

    static void DequeueAndExec();

    static void WaitForInterrupts( uint64_t expireTimeInSysTicks, uint32_t sleepLevel, uint64_t wakeEvents );
};

//--//

#endif //_NANOPAL_ASYNCPROCCALLS_DECL_H_
