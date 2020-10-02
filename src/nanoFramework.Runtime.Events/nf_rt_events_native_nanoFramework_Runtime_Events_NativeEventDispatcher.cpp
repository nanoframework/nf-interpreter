//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include "nf_rt_events_native.h"


HRESULT Library_nf_rt_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher::EnableInterrupt___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_NativeEventDispatcher *pNativeDisp = NULL;
    
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    if(pThis[ FIELD__disposed ].NumericByRef().s1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    NANOCLR_CHECK_HRESULT(GetEventDispatcher( stack, pNativeDisp ));

    // Calls driver to enable interrupts.  Consider that there could be no driver 
    // associated to this object so check that the driver methods are set 
    if(pNativeDisp->driverMethods == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }
    
    NANOCLR_CHECK_HRESULT(pNativeDisp->driverMethods->enableProcessor( pNativeDisp, true ));
    
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_rt_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher::DisableInterrupt___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
   
    CLR_RT_HeapBlock_NativeEventDispatcher *pNativeDisp = NULL;
    
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    if(pThis[ FIELD__disposed ].NumericByRef().s1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    NANOCLR_CHECK_HRESULT(GetEventDispatcher( stack, pNativeDisp ));
    
    // Calls driver to enable interrupts.  Consider that there could be no driver 
    // associated to this object so check that the driver methods are set 
    // we will be tolerant in this case and not throw any exception
    if(pNativeDisp->driverMethods != NULL)
    {
        NANOCLR_CHECK_HRESULT(pNativeDisp->driverMethods->enableProcessor( pNativeDisp, false ));
    }
    
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_rt_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher::Dispose___VOID__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
        
    CLR_RT_HeapBlock_NativeEventDispatcher *pNativeDisp = NULL;
    
    CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

    NANOCLR_CHECK_HRESULT(GetEventDispatcher( stack, pNativeDisp ));
    
    // Cleanup the HAL queue from the instance of associated CLR_RT_HeapBlock_NativeEventDispatcher 
    pNativeDisp->RemoveFromHALQueue();
    
    // Calls driver to enable interrupts.  Consider that there could be no driver 
    // associated to this object so check that the driver methods are set 
    // we will be tolerant in this case and not throw any exception
    if(pNativeDisp->driverMethods != NULL)
    {
        NANOCLR_CHECK_HRESULT(pNativeDisp->driverMethods->cleanupProcessor( pNativeDisp )); 
    }
    
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_rt_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher::_ctor___VOID__STRING__U8( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); 

    CLR_RT_DriverInterruptMethods*          pDriverMethods;
    const CLR_RT_NativeAssemblyData*        pNativeDriverData;
    CLR_RT_HeapBlock_NativeEventDispatcher* pNativeDisp = NULL;
    
    const char *                            driverName;    
    uint64_t                        driverData;
    
    CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

    // Retrieve paramenters; 
    if (stack.Arg1().DataType() != DATATYPE_OBJECT) 
    {   
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);                      
    }

    driverName = stack.Arg1().RecoverString();  FAULT_ON_NULL(driverName);
    driverData = stack.Arg2().NumericByRef().u8;

    // Throw NULL exception if string is empty. 
    if(hal_strlen_s( driverName ) == 0)
    {  
        NANOCLR_CHECK_HRESULT(CLR_E_ARGUMENT_NULL);
    }
    
    // Retrives pointers to driver implemented functions.
    pNativeDriverData = GetAssemblyNativeData( driverName );
    
    // Validates check sum and presence of the structure.
    if(pNativeDriverData == NULL || pNativeDriverData->m_checkSum != DRIVER_INTERRUPT_METHODS_CHECKSUM)
    {
       NANOCLR_CHECK_HRESULT(CLR_E_DRIVER_NOT_REGISTERED);
    }

    // Get pointer to CLR_RT_DriverInterruptMethods
    pDriverMethods = (CLR_RT_DriverInterruptMethods *)pNativeDriverData->m_pNativeMethods;
    
    // Check that all methods are present:
    if( pDriverMethods->initProcessor == NULL || 
        pDriverMethods->enableProcessor == NULL || 
        pDriverMethods->cleanupProcessor == NULL)
    {
       NANOCLR_CHECK_HRESULT(CLR_E_DRIVER_NOT_REGISTERED);
    }

    // So we found driver by name and now we create instance of CLR_RT_HeapBlock_NativeEventDispatcher
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_NativeEventDispatcher::CreateInstance( *pThis, pThis[ FIELD___NativeEventDispatcher ] ));

    // Initialize the driver with and provide the instance of CLR_RT_HeapBlock_NativeEventDispatcher 
    NANOCLR_CHECK_HRESULT(GetEventDispatcher( stack, pNativeDisp ));

    // Now call the driver. First save pointer to driver data.
    pNativeDisp->driverMethods = pDriverMethods;

    NANOCLR_CHECK_HRESULT(pDriverMethods->initProcessor( pNativeDisp, driverData ));
    
    NANOCLR_NOCLEANUP();
}

//--//

CLR_RT_ObjectToEvent_Source* Library_nf_rt_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher::GetEventDispReference( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_HARDWARE();

    CLR_RT_HeapBlock* pThis = stack.This();

    return CLR_RT_ObjectToEvent_Source::ExtractInstance( pThis[ FIELD___NativeEventDispatcher ] );
}

HRESULT Library_nf_rt_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher::GetEventDispatcher( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock_NativeEventDispatcher*& event )
{
    NATIVE_PROFILE_CLR_HARDWARE();

    NANOCLR_HEADER();

    event = GetEventDispatcher( stack );
    if(event == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    NANOCLR_NOCLEANUP();
}

CLR_RT_HeapBlock_NativeEventDispatcher* Library_nf_rt_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher::GetEventDispatcher( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_HARDWARE();

    CLR_RT_ObjectToEvent_Source* src = GetEventDispReference( stack );

    return (src == NULL) ? NULL : (CLR_RT_HeapBlock_NativeEventDispatcher*)src->m_eventPtr;
}
