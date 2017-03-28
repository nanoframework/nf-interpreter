//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_BlockStorage.h>
#include <nanoWeak.h>

/////////////////////////////////////////////////////
// DeviceBlockInfo stubs

__nfweak SectorAddress DeviceBlockInfo_PhysicalToSectorAddress(DeviceBlockInfo* blockInfo,  const BlockRegionInfo* pRegion, ByteAddress phyAddress)
{
    return phyAddress;
}

__nfweak bool DeviceBlockInfo_FindRegionFromAddress(DeviceBlockInfo* blockInfo, ByteAddress address, unsigned int* blockRegionIndex, unsigned int* blockRangeIndex)
{
    return false;        
}

__nfweak bool DeviceBlockInfo_FindNextUsageBlock(DeviceBlockInfo* blockInfo, unsigned int blockUsage, unsigned int* address, unsigned int* blockRegionIndex, unsigned int* blockRangeIndex)
{
    return false;
}

__nfweak bool DeviceBlockInfo_FindForBlockUsage(DeviceBlockInfo* blockInfo, unsigned int blockUsage, unsigned int* address, unsigned int* blockRegionIndex, unsigned int* blockRangeIndex)
{
    return false;
}


/////////////////////////////////////////////////////
// BlockStorageStream stubs

__nfweak bool BlockStorageStream_IsXIP(BlockStorageStream *stream)
{
    return false;
}

__nfweak bool BlockStorageStream_IsReadModifyWrite(BlockStorageStream *stream)
{
    return false;
}

__nfweak bool BlockStorageStream_SetReadModifyWrite(BlockStorageStream *stream)
{
    return false;
}

__nfweak bool BlockStorageStream_Initialize(BlockStorageStream *stream, unsigned int blockUsage)
{
    return false;
}

__nfweak bool BlockStorageStream_InitializeWithBlockStorageDevice(BlockStorageStream *stream, unsigned int blockUsage, BlockStorageDevice *pDevice)
{
    return false;
}

bool BlockStorageStream_NextStream(BlockStorageStream* stream)
{
    if(stream->Device == NULL || BlockStorageDevice_Prev(stream->Device) == NULL) return false;

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
                stream->Device = BlockStorageDevice_Prev(stream->Device);
                
                if(stream->Device == NULL || BlockStorageDevice_Prev(stream->Device) == NULL)
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
    if(stream->Device == NULL || BlockStorageDevice_Prev(stream->Device) == NULL) return false;

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
                stream->Device = BlockStorageDevice_Prev(stream->Device);
                
                if(stream->Device == NULL || BlockStorageDevice_Prev(stream->Device) == NULL)
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

bool BlockStorageStream_Seek(BlockStorageStream *stream, unsigned int offset, SeekOrigin origin)
{
    unsigned int seekIndex;

    switch (origin)
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

    while (seekIndex >= (int32_t)stream->Length)
    {
        // out of region address space
        return false;
    }

    while (seekIndex < 0)
    {
        // out of region address space
        return false;
    }

    stream->CurrentIndex = seekIndex;

    return true;
}

__nfweak bool BlockStorageStream_Write(BlockStorageStream *stream, unsigned char *data, unsigned int length)
{
    return false;
}

__nfweak bool BlockStorageStream_Erase(BlockStorageStream *stream, unsigned int length)
{
    return false;
}

__nfweak bool BlockStorageStream_ReadIntoBuffer(BlockStorageStream *stream, unsigned char *buffer, unsigned int length)
{
    return false;
}

bool BlockStorageStream_Read(BlockStorageStream* stream, unsigned char** buffer, unsigned int length)
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

unsigned int BlockStorageStream_CurrentAddress(BlockStorageStream *stream)
{
    return stream->BaseAddress + stream->CurrentIndex;
}

__nfweak bool BlockStorageStream_IsErased(BlockStorageStream *stream, unsigned int length)
{
    return false;
}

/////////////////////////////////////////////////////
// BlockStorageDevice stubs

__nfweak BlockStorageDevice* BlockStorageDevice_Next(BlockStorageDevice* device)
{
    return NULL;
}

__nfweak BlockStorageDevice* BlockStorageDevice_Prev(BlockStorageDevice* device)
{
    return NULL;
}

/////////////////////////////////////////////////////////
// Description:
//    Initializes a given block device for use
// 
// Returns:
//   true if succesful; false if not
//
// Remarks:
//    No other functions in this interface may be called
//    until after Init returns.
//
bool BlockStorageDevice_InitializeDevice(BlockStorageDevice* device)
{
    return device->m_BSD->InitializeDevice(device->m_context);
}

/////////////////////////////////////////////////////////
// Description:
//    Initializes a given block device for use
// 
// Returns:
//   true if succesful; false if not
//
// Remarks:
//   De initializes the device when no longer needed
//
bool BlockStorageDevice_UninitializeDevice(BlockStorageDevice* device)
{
    return device->m_BSD->UninitializeDevice(device->m_context);
}

/////////////////////////////////////////////////////////
// Description:
//    Gets the information describing the device
//
DeviceBlockInfo* BlockStorageDevice_GetDeviceInfo(BlockStorageDevice* device)
{
    return device->m_BSD->GetDeviceInfo(device->m_context);
}

/////////////////////////////////////////////////////////
// Description:
//    Reads data from a set of sectors
//
// Input:
//    startAddress - Starting Sector for the read
//    numBytes  - Number of sectors to read
//    buffer - pointer to buffer to read the data into.
//                  Must be large enough to hold all of the data
//                  being read.
//
//    pSectorMetadata - pointer to an array of structured (one for each sector)
//                      for the extra sector information.
// 
// Returns:
//   true if succesful; false if not
//
// Remarks:
//   This function reads the number of sectors specified from the device.
//   
//   pSectorBuff may be NULL. This is to allow for reading just the metadata.
// 
//   pSectorMetadata can be set to NULL if the caller does not need the extra
//   data.
//
//   If the device does not support sector Metadata it should fail if the 
//   pSectorMetadata parameter is not NULL.
//
bool BlockStorageDevice_Read(BlockStorageDevice* device, unsigned int startAddress, unsigned int numBytes, unsigned char* buffer)
{
     return device->m_BSD->Read(device->m_context, startAddress, numBytes, buffer);
}

/////////////////////////////////////////////////////////
// Description:
//    Writes data to a set of sectors
//
// Input:
//    startAddress - Starting Sector for the write
//    numBytes  - Number of sectors to write
//    buffer - pointer to data to write.
//                  Must be large enough to hold complete sectors
//                  for the number of sectors being written.
//
//    pSectorMetadata - pointer to an array of structures (one for each sector)
//                      for the extra sector information.
// 
// Returns:
//   true if succesful; false if not
//
// Remarks:
//   This function reads the number of sectors specified from the device.
//   The SectorMetadata is used for flash arrays without special controllers
//   to manage wear leveling etc... (mostly for NOR and NAND). The metadata
//   is used by upper layers to ensure that data is moved around on the flash
//   when writing to prevent failure of the device from too many erase cycles
//   on a sector. 
//   
//   If the device does not support sector Metadata it should fail if the 
//   pSectorMetadata parameter is not NULL.
//
//   pSectorMetadata can be set to NULL if the caller does not need the extra
//   data. Implementations must not attempt to write data through a NULL pointer! 
//
bool BlockStorageDevice_Write(BlockStorageDevice* device, unsigned int startAddress, unsigned int numBytes, unsigned char* buffer, bool readModifyWrite)
{
    return device->m_BSD->Write(device->m_context, startAddress, numBytes, buffer, readModifyWrite);
}

bool BlockStorageDevice_Memset(BlockStorageDevice* device, unsigned int startAddress, unsigned char buffer, unsigned int numBytes)
{
    return device->m_BSD->Memset(device->m_context, startAddress, buffer, numBytes);
}

//__nfweak bool BlockStorageDevice_GetSectorMetadata(BlockStorageDevice* device, unsigned int sectorStart, SectorMetadata* pSectorMetadata);
//__nfweak bool BlockStorageDevice_SetSectorMetadata(BlockStorageDevice* device, unsigned int sectorStart, SectorMetadata* pSectorMetadata);

/////////////////////////////////////////////////////////
// Description:
//    Check a block is erased or not
// 
// Input:
//    Address - Logical Sector Address
//
// Returns:
//   true it is erased; false if not
//
// Remarks:
//    check the block containing the sector address specified.
//  
bool BlockStorageDevice_IsBlockErased(BlockStorageDevice* device, unsigned int blockStartAddress, unsigned int length)
{
    return device->m_BSD->IsBlockErased(device->m_context, blockStartAddress, length);
}

/////////////////////////////////////////////////////////
// Description:
//    Erases a block
// 
// Input:
//    Address - Logical Sector Address
//
// Returns:
//   true if succesful; false if not
//
// Remarks:
//    Erases the block containing the sector address specified.
//
bool BlockStorageDevice_EraseBlock(BlockStorageDevice* device, unsigned int address)
{
    return device->m_BSD->EraseBlock(device->m_context, address);
}

/////////////////////////////////////////////////////////
// Description:
//   Changes the power state of the device
// 
// Input:
//    state   - true= power on; false = power off
//
// Remarks:
//   This function allows systems to conserve power by 
//   shutting down the hardware when the system is 
//   going into low power states.
//
void BlockStorageDevice_SetPowerState(BlockStorageDevice* device, unsigned int state)
{
    device->m_BSD->SetPowerState(device->m_context, state);
}

bool BlockStorageDevice_FindRegionFromAddress(BlockStorageDevice* device, unsigned int address, unsigned int* blockRegionIndex, unsigned int* blockRangeIndex)
{
    const DeviceBlockInfo* pDevInfo = BlockStorageDevice_GetDeviceInfo(device);

    return DeviceBlockInfo_FindRegionFromAddress(pDevInfo, address, blockRegionIndex, blockRangeIndex);
}

bool BlockStorageDevice_FindForBlockUsage(BlockStorageDevice* device, unsigned int blockUsage, unsigned int* address, unsigned int* blockRegionIndex, unsigned int* blockRangeIndex)
{
    const DeviceBlockInfo* pDevInfo = BlockStorageDevice_GetDeviceInfo(device);

    return DeviceBlockInfo_FindForBlockUsage(pDevInfo, blockUsage, address, blockRegionIndex, blockRangeIndex);
}

bool BlockStorageDevice_FindNextUsageBlock(BlockStorageDevice* device, unsigned int blockUsage, unsigned int* address, unsigned int* blockRegionIndex, unsigned int* blockRangeIndex)
{
    const DeviceBlockInfo* pDevInfo = BlockStorageDevice_GetDeviceInfo(device);

    return DeviceBlockInfo_FindNextUsageBlock(pDevInfo, blockUsage, address, blockRegionIndex, blockRangeIndex);
}

///////////////////////////////////////////////////
// BlockStorageList declarations 

// // walk through list of devices and calls Init() function
// __nfweak bool BlockStorageList_InitializeDevices()
// {
//     return false;
// }

// // walk through list of devices and calls UnInit() function
// __nfweak bool BlockStorageList_UnInitializeDevices()
// {
//     return false;
// }

__nfweak bool BlockStorageList_AddDevice(BlockStorageDevice* pBSD, IBlockStorageDevice* vtable, void* config, bool init)
{
    return false;
}

// __nfweak bool BlockStorageList_RemoveDevice(BlockStorageDevice* pBSD, bool unInit)
// {
//     return false;
// }

__nfweak bool BlockStorageList_FindDeviceForPhysicalAddress(BlockStorageDevice** pBSD, unsigned int physicalAddress, ByteAddress* blockAddress)
{
    return false;
}

__nfweak BlockStorageDevice* BlockStorageList_GetFirstDevice()
{
    return NULL;
}

// __nfweak BlockStorageDevice* BlockStorageList_GetNextDevice(BlockStorageDevice* device)
// {
//     return NULL;
// }

// // returns number of devices has been declared in the system
// __nfweak unsigned int BlockStorageList_GetNumDevices()
// {
//     return 0;
// }

/////////////////////////////////////////////////////
