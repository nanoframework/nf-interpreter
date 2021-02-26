//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"

//using namespace Microsoft::SPOT::Emulator;

// From minheap.cpp
static unsigned char* s_Memory_Start  = NULL;
static unsigned int s_Memory_Length = 1024*1024*10;


void HeapLocation( unsigned char*& BaseAddress, unsigned int& SizeInBytes )
{
    if(!s_Memory_Start)
    {
        s_Memory_Start = (unsigned char*)::VirtualAlloc( NULL, s_Memory_Length, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );

        if(s_Memory_Start)
        {
            memset( s_Memory_Start, 0xEA, s_Memory_Length );
        }

        HalSystemConfig.RAM1.Base = (unsigned int)(size_t)s_Memory_Start;
        HalSystemConfig.RAM1.Size = (unsigned int)(size_t)s_Memory_Length;
    }

    BaseAddress = s_Memory_Start;
    SizeInBytes = s_Memory_Length;
}


static unsigned char* s_CustomHeap_Start  = NULL;

void CustomHeapLocation( unsigned char*& BaseAddress, unsigned int& SizeInBytes )
{
    if(!s_CustomHeap_Start)
    {
        s_CustomHeap_Start = (unsigned char*)::VirtualAlloc( NULL, s_Memory_Length, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );

        if(s_CustomHeap_Start)
        {
            memset( s_CustomHeap_Start, 0xEA, s_Memory_Length );
        }
    }

    BaseAddress = s_CustomHeap_Start;
    SizeInBytes = s_Memory_Length;
}

