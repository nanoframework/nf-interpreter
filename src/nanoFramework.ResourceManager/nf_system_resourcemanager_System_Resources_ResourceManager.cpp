//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nf_system_resourcemanager.h"


HRESULT Library_nf_system_resourcemanager_System_Resources_ResourceManager::GetObjectInternal___OBJECT__I2( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_Assembly_Instance   assm;
    CLR_UINT32                 size;
    const CLR_RECORD_RESOURCE* resource;
    CLR_RT_Assembly*           pAssm;
    const CLR_UINT8*           buf;
    CLR_RT_HeapBlock*          pThis          =   stack.This();
    CLR_RT_HeapBlock*          pArgs          = &(stack.Arg1());
    CLR_UINT32                 resourceFileId = pThis[ FIELD___resourceFileId ].NumericByRefConst().s4; 
    CLR_RT_HeapBlock&          top            = stack.PushValueAndClear();

    //
    // Set up for restart on out of memory.
    //
    if(stack.m_customState == 0)
    {
        stack.m_customState  = 1;
        stack.m_flags       |= CLR_RT_StackFrame::c_CompactAndRestartOnOutOfMemory;
    }

    NANOCLR_CHECK_HRESULT(Library_corlib_native_System_Reflection_Assembly::GetTypeDescriptor( *pThis[ FIELD___assembly ].Dereference(), assm ));
    
    NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.LocateResource( assm, resourceFileId, pArgs[ 0 ].NumericByRefConst().s2, resource, size ));

    if(resource != NULL) //otherwise NULL is returned
    {
        pAssm = assm.m_assm;
        buf = pAssm->GetResourceData( resource->offset );
        
        switch(resource->kind)
        {
        case CLR_RECORD_RESOURCE::RESOURCE_String:
            {
                NANOCLR_SET_AND_LEAVE(CLR_RT_HeapBlock_String::CreateInstance( top, (const char*)buf, pAssm ));
            }
            break;
        
        case CLR_RECORD_RESOURCE::RESOURCE_Binary:
            {
                NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( top, size, g_CLR_RT_WellKnownTypes.m_UInt8 ));

                memcpy( top.DereferenceArray()->GetFirstElement(), buf, size );
            }
            break;

        default:
            _ASSERTE(false); 
            break;
        }
    }
    
    NANOCLR_NOCLEANUP();
}


HRESULT Library_nf_system_resourcemanager_System_Resources_ResourceManager::GetObjectInternal___OBJECT__I2__I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_Assembly_Instance   assm;
    CLR_UINT32                 size;
    const CLR_RECORD_RESOURCE* resource;
    CLR_RT_Assembly*           pAssm;
    const CLR_UINT8*           buf;
    CLR_RT_HeapBlock*          pThis          =   stack.This();
    CLR_RT_HeapBlock*          pArgs          = &(stack.Arg1());
    CLR_UINT32                 resourceFileId = pThis[ FIELD___resourceFileId ].NumericByRefConst().s4; 
    CLR_RT_HeapBlock&          top            = stack.PushValueAndClear();
    CLR_UINT32                 offset         = pArgs[ 1 ].NumericByRef().s4;
    CLR_UINT32                 length         = pArgs[ 2 ].NumericByRef().s4;

    //
    // Set up for restart on out of memory.
    //
    if(stack.m_customState == 0)
    {
        stack.m_customState  = 1;
        stack.m_flags       |= CLR_RT_StackFrame::c_CompactAndRestartOnOutOfMemory;
    }

    NANOCLR_CHECK_HRESULT(Library_corlib_native_System_Reflection_Assembly::GetTypeDescriptor( *pThis[FIELD___assembly].Dereference(), assm ));
    
    NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.LocateResource( assm, resourceFileId, pArgs[ 0 ].NumericByRefConst().s2, resource, size ));

    
    if(resource != NULL) //otherwise NULL is returned
    {
        pAssm = assm.m_assm;
        buf = pAssm->GetResourceData( resource->offset );
        
        switch(resource->kind)
        {
        //
        // Chunked access is only possible for binary resources
        //
        case CLR_RECORD_RESOURCE::RESOURCE_String:
        case CLR_RECORD_RESOURCE::RESOURCE_Bitmap:
        case CLR_RECORD_RESOURCE::RESOURCE_Font:
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);                
            }
            break;
        case CLR_RECORD_RESOURCE::RESOURCE_Binary:
            {
                // throw out-of-range when offset is too large...
                if(offset >= size)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);
                }
                // ... but trim length if necessary
                else
                {
                    if(offset + length >= size)
                    {
                        length = size - offset;
                    }
                }
                
                NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( top, length, g_CLR_RT_WellKnownTypes.m_UInt8 ));

                memcpy( top.DereferenceArray()->GetFirstElement() , buf + offset, length );
            }
            break;

        default:
            _ASSERTE(false); 
            break;
        }
    }
    
    NANOCLR_NOCLEANUP();   
}

HRESULT Library_nf_system_resourcemanager_System_Resources_ResourceManager::FindResource___STATIC__I4__STRING__SystemReflectionAssembly( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_Assembly_Instance assm;
    CLR_INT32                resourceFileId;
    CLR_RT_HeapBlock*        pArgs = &stack.Arg0();

    const char* szText = pArgs[ 0 ].RecoverString(); FAULT_ON_NULL(szText);
    
    NANOCLR_CHECK_HRESULT(Library_corlib_native_System_Reflection_Assembly::GetTypeDescriptor( *pArgs[ 1 ].Dereference(), assm ));

    NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.LocateResourceFile( assm, szText, resourceFileId ));

    stack.SetResult_I4( resourceFileId );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_resourcemanager_System_Resources_ResourceManager::GetObject___STATIC__OBJECT__SystemResourcesResourceManager__SystemEnum( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock& blkResourceManager = stack.Arg0();
    CLR_RT_HeapBlock& blkEnumObj         = stack.Arg1();    
    CLR_RT_HeapBlock* blkVT              = blkEnumObj.Dereference();
    CLR_RT_HeapBlock* blkEnum            = blkVT + 1;
    CLR_RT_MethodDef_Instance md;

    if(stack.m_customState == 0)
    {
        stack.m_customState = 1;

        FAULT_ON_NULL(blkVT);
        
        if(blkEnum->DataType() != DATATYPE_I2 && blkEnum->DataType() != DATATYPE_U2) NANOCLR_SET_AND_LEAVE( CLR_E_INVALID_PARAMETER );
        
        //call back into ResourceManager.GetObjectFromId(short id);

        _SIDE_ASSERTE(md.InitializeFromIndex( g_CLR_RT_WellKnownMethods.m_ResourceManager_GetObjectFromId ));

        NANOCLR_CHECK_HRESULT( stack.MakeCall( md, &blkResourceManager, blkEnum, 1 ));
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_resourcemanager_System_Resources_ResourceManager::GetObject___STATIC__OBJECT__SystemResourcesResourceManager__SystemEnum__I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock& blkResourceManager = stack.Arg0();
    CLR_RT_MethodDef_Instance md;

    if(stack.m_customState == 0)
    {
        stack.m_customState = 1;

        //call back into ResourceManager.GetObjectFromId(short id, int offset, int length);

        _SIDE_ASSERTE(md.InitializeFromIndex( g_CLR_RT_WellKnownMethods.m_ResourceManager_GetObjectChunkFromId ));

        NANOCLR_CHECK_HRESULT( stack.MakeCall( md, &blkResourceManager, &stack.Arg1(), 3 ));
    }

    NANOCLR_NOCLEANUP();
}

