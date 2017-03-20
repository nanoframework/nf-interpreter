//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <targetHAL.h>

bool BlockStorageStream_Initialize(BlockStorageStream* stream, uint32_t storageUsage)
{
    // 'reset' CurrentIndex
    stream->CurrentIndex = 0;

    // set usage according to parameter
    stream->Usage = storageUsage;

    // initialize base address and length from linker symbols
    if(storageUsage == StorageUsage_CLR)
    {
        // set BaseAddress to the start of the region
        stream->BaseAddress = &__nanoImage_start__;
        // set Length to the region size 
        stream->Length = &__nanoImage_end__ - &__nanoImage_start__;
    }
    else if(storageUsage == StorageUsage_DEPLOYMENT)
    {
        // set BaseAddress to the start of the region
        stream->BaseAddress = &__deployment_start__;
        // set Length to the region size 
        stream->Length = &__deployment_end__ - &__deployment_start__;
    }

    return true;
}

bool BlockStorageStream_Read(BlockStorageStream* stream, unsigned char** buffer, uint32_t length)
{
    if(stream == NULL || !buffer) return false;

    unsigned char* pBuffer = *buffer;
    bool fXIPFound = false;

    while(length > 0)
    {
        int readLen = length;

        if((stream->CurrentIndex + readLen) > stream->Length)
        {
            readLen = stream->Length - stream->CurrentIndex;
        }

        if(!fXIPFound)
        {
            *buffer = (unsigned char*)(BlockStorageStream_CurrentAddress(stream));
            
            fXIPFound = true;
        }

        length       -= readLen;
        pBuffer      += readLen;
        stream->CurrentIndex += readLen;    
    }

    return true;
}

bool BlockStorageStream_Seek(BlockStorageStream* stream, int32_t offset, SeekOrigin origin)
{
    uint32_t seekIndex;
    
    switch(origin)
    {
        case BlockStorageStream_SeekBegin:
            seekIndex = 0;
            break;

        case BlockStorageStream_SeekCurrent:
            seekIndex = stream->CurrentIndex;
            break;

        case BlockStorageStream_SeekEnd:
            seekIndex = stream->Length;
            break;

        default:

            return false;
    }
    
    seekIndex += offset;
    
    while(seekIndex >= (int32_t)stream->Length)
    {
        // out of region address space
        return false;
    }
    
    while(seekIndex < 0)
    {
        // out of region address space
        return false;
    }

    stream->CurrentIndex = seekIndex;

    return true;
}
