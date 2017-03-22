//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_BlockStorage.h>
#include <targetHAL.h>

bool BlockStorageStream_Initialize(BlockStorageStream* stream, uint32_t blockUsage)
{
    // 'reset' CurrentIndex
    stream->CurrentIndex = 0;

    stream->BlockLength = 0;

    // set usage according to parameter
    stream->Usage = blockUsage;

    // initialize base address and length from linker symbols
    if(blockUsage == StorageUsage_CLR)
    {
        // set BaseAddress to the start of the region
        stream->BaseAddress = &__nanoImage_start__;
        // set Length to the region size 
        stream->Length = &__nanoImage_end__ - &__nanoImage_start__;
    }
    else if(blockUsage == StorageUsage_DEPLOYMENT)
    {
        // set BaseAddress to the start of the region
        stream->BaseAddress = &__deployment_start__;
        // set Length to the region size 
        stream->Length = &__deployment_end__ - &__deployment_start__;
    }

    return true;
}
