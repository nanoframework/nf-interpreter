//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#include "nf_networking_sntp.h"


HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::Start___STATIC__VOID( CLR_RT_StackFrame& stack )
{
    (void)stack;

    NANOCLR_HEADER();

    sntp_init();

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::Stop___STATIC__VOID( CLR_RT_StackFrame& stack )
{
    (void)stack;

    NANOCLR_HEADER();

    sntp_stop();

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::UpdateNow___STATIC__VOID( CLR_RT_StackFrame& stack )
{
    (void)stack;

    NANOCLR_HEADER();

    // this is just a stop and start of the SNTP client
    sntp_stop();
    sntp_init();

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::get_IsStarted___STATIC__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::get_Server1___STATIC__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::set_Server1___STATIC__VOID__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::get_Server2___STATIC__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::set_Server2___STATIC__VOID__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}
