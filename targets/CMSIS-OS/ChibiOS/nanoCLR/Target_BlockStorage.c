//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <targetHAL.h>

bool BlockStorageStream_Init(BlockStorageStream* stream)
{
    // init function pointers in structure 
    //stream->IsXIP = &BlockStorageStream_IsXIP; 
    //stream->IsReadModifyWrite = &BlockStorageStream_IsReadModifyWrite; 
    //stream->SetReadModifyWrite = &BlockStorageStream_SetReadModifyWrite; 
    stream->Initialize = &BlockStorageStream_Initialize; 
    //stream->InitializeWithBlockStorageDevice = &BlockStorageStream_InitializeWithBlockStorageDevice; 
    stream->NextStream = &BlockStorageStream_NextStream; 
    stream->PrevStream = &BlockStorageStream_PrevStream; 
    stream->Seek = &BlockStorageStream_Seek;
    //stream->Write = &BlockStorageStream_Write;
    //stream->Erase = &BlockStorageStream_Erase;
    stream->ReadIntoBuffer = &BlockStorageStream_ReadIntoBuffer;
    stream->Read = &BlockStorageStream_Read;
    stream->CurrentAddress = &BlockStorageStream_CurrentAddress;
    //stream->IsErased = &BlockStorageStream_IsErased;
}

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

bool BlockStorageStream_NextStream(BlockStorageStream* stream)
{
    if(stream->Device == NULL || stream->Device->Prev(stream) == NULL) return false;

    const BlockRegionInfo* pRegion;
    const DeviceBlockInfo* pDevInfo;// UNDONE: FIXME:  = stream->Device->GetDeviceInfo();

    BlockStorageStream orig;

    memcpy(&orig, stream, sizeof(orig));

    do
    {
        bool fLastIndex = false;
        
        if(stream->RangeIndex == 0) 
        {
            if(stream->RegionIndex == 0)
            {
                stream->Device = stream->Device->Prev(stream);
                
                if(stream->Device == NULL || stream->Device->Prev(stream) == NULL)
                {
                    memcpy(stream, &orig, sizeof(orig));
                    return false;
                }
                
                // UNDONE: FIXME: pDevInfo = stream->Device->GetDeviceInfo();
            
                stream->RegionIndex = pDevInfo->NumRegions - 1;
            }
            else
            {
                stream->RegionIndex--;
            }

            fLastIndex = true;
        }
        else
        {
            stream->RangeIndex--;
        }

        pRegion = &pDevInfo->Regions[stream->RegionIndex];

        if(fLastIndex)
        {
            stream->RangeIndex = pRegion->NumBlockRanges - 1;
        }

    } while( BlockRange_GetBlockUsage(pRegion->BlockRanges[stream->RangeIndex]) != stream->Usage );

    // if( pDevInfo->Attribute.SupportsXIP ) Flags |= c_BlockStorageStream__XIP;
    // else Flags &= ~c_BlockStorageStream__XIP;
    
    stream->BlockLength  = pRegion->BytesPerBlock;
    stream->BaseAddress  = pRegion->Start + pRegion->BlockRanges[stream->RangeIndex].StartBlock * stream->BlockLength;
    stream->Length       = BlockRange_GetBlockCount(pRegion->BlockRanges[stream->RangeIndex]) * stream->BlockLength;
    stream->CurrentIndex = 0;

    return true;
}

bool BlockStorageStream_PrevStream(BlockStorageStream* stream)
{
    if(stream->Device == NULL || stream->Device->Prev(stream) == NULL) return false;

    const BlockRegionInfo* pRegion;
    const DeviceBlockInfo* pDevInfo;// UNDONE: FIXME:  = this->Device->GetDeviceInfo();

    BlockStorageStream orig;

    memcpy(&orig, stream, sizeof(orig));

    do
    {
        bool fLastIndex = false;
        
        if(stream->RangeIndex == 0) 
        {
            if(stream->RegionIndex == 0)
            {
                stream->Device = stream->Device->Prev(stream);
                
                if(stream->Device == NULL || stream->Device->Prev(stream) == NULL)
                {
                    memcpy(stream, &orig, sizeof(orig));
                    return false;
                }
                
                // UNDONE: FIXME: pDevInfo = this->Device->GetDeviceInfo();
            
                stream->RegionIndex = pDevInfo->NumRegions - 1;
            }
            else
            {
                stream->RegionIndex--;
            }

            fLastIndex = true;
        }
        else
        {
            stream->RangeIndex--;
        }

        pRegion = &pDevInfo->Regions[stream->RegionIndex];

        if(fLastIndex)
        {
            stream->RangeIndex = pRegion->NumBlockRanges - 1;
        }

    } while( BlockRange_GetBlockUsage(pRegion->BlockRanges[stream->RangeIndex]) != stream->Usage );

    // if( pDevInfo->Attribute.SupportsXIP ) Flags |= c_BlockStorageStream__XIP;
    // else Flags &= ~c_BlockStorageStream__XIP;
    
    stream->BlockLength  = pRegion->BytesPerBlock;
    stream->BaseAddress  = pRegion->Start + pRegion->BlockRanges[stream->RangeIndex].StartBlock * stream->BlockLength;
    stream->Length       = BlockRange_GetBlockCount(pRegion->BlockRanges[stream->RangeIndex]) * stream->BlockLength;
    stream->CurrentIndex = 0;

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

uint32_t BlockStorageStream_CurrentAddress(BlockStorageStream* stream)
{
    return stream->BaseAddress + stream->CurrentIndex;
}
