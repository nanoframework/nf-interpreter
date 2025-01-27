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

#include "corlib_native.h"

HRESULT Library_corlib_native_System_Threading_SpinWait::SpinOnce___VOID(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    (void)stack;

    // basically... do nothing!

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_corlib_native_System_Threading_SpinWait::SpinUntil___STATIC__VOID__SystemTimeSpan(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_CHECK_HRESULT(Spin(stack, true));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Threading_SpinWait::SpinUntil___STATIC__VOID__I4(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_CHECK_HRESULT(Spin(stack, false));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Threading_SpinWait::Spin(CLR_RT_StackFrame &stack, bool isTimeSpan)
{
    NANOCLR_HEADER();

    CLR_INT64 *timeout;
    CLR_INT64 timeoutMiliseconds;
    CLR_RT_HeapBlock hbTimeout;
    bool longRunning;
    bool eventResult = true;

    if (isTimeSpan)
    {
        timeout = Library_corlib_native_System_TimeSpan::GetValuePtr(stack.Arg0());
        FAULT_ON_NULL_ARG(timeout);

        if (*timeout < -1)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
        }

        if (*timeout == -1)
        {
            // this is infinite timeout
            timeoutMiliseconds = *timeout;
        }
        else
        {
            // convert to milliseconds
            timeoutMiliseconds = *timeout / TIME_CONVERSION__TO_MILLISECONDS;
        }
    }
    else
    {
        timeoutMiliseconds = stack.Arg0().NumericByRef().s4;
    }

    longRunning = timeoutMiliseconds > (CLR_INT64)CLR_RT_Thread::c_TimeQuantum_Milliseconds;

    if (longRunning)
    {
        // this is longer than the thread time quantum
        hbTimeout.SetInteger(timeoutMiliseconds * TIME_CONVERSION__TO_MILLISECONDS);

        // if m_customState == 0 then push timeout on to eval stack[0] then move to m_customState = 1
        NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

        if (stack.m_customState == 1)
        {
            // bump custom state
            stack.m_customState = 2;
        }

        while (eventResult)
        {
            // non-blocking wait allowing other threads to run while we wait for the timeout to occur
            NANOCLR_CHECK_HRESULT(
                g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_NoEvent, eventResult));

            // don't bother checking the result, it will be a timeout for sure
        }

        // pop timeout heap block from stack
        stack.PopValue();
    }
    else
    {
        // just delay
        OS_DELAY(timeoutMiliseconds);
    }

    NANOCLR_NOCLEANUP();
}
