//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"

//--//

// HAL_DblLinkedList<BlockStorageDevice> BlockStorageList::s_deviceList;
// BlockStorageDevice*                   BlockStorageList::s_primaryDevice = NULL;
static BlockStorageDevice *s_primaryDevice = NULL;

//--//

// void BlockStorageList::Initialize()
//{
//}
void BlockStorageList_Initialize()
{

}

//
// bool BlockStorageList::InitializeDevices()
//{
//    return true;
//}
//
// bool BlockStorageList::UnInitializeDevices()
//{
//    return true;
//}
//
// bool BlockStorageList::AddDevice( BlockStorageDevice* pBSD, IBlockStorageDevice* vtable, void* config, bool Init)
//{
//    return true;
//}
bool BlockStorageList_AddDevice(BlockStorageDevice *pBSD, IBlockStorageDevice *vtable, void *config, bool init)
{
    s_primaryDevice = pBSD;

    // TODO what to do with vtable and config??
    return true;
}
//
// bool BlockStorageList::RemoveDevice( BlockStorageDevice* pBSD, bool UnInit)
//{
//    return true;
//}

// BlockStorageDevice* BlockStorageList::GetFirstDevice()
//{
//    return NULL;
//}
BlockStorageDevice *BlockStorageList_GetFirstDevice()
{
    return s_primaryDevice;
}

BlockStorageDevice *BlockStorageList_GetNextDevice(BlockStorageDevice *device)
{
    return NULL;
}

unsigned int BlockStorageList_GetNumDevices()
{
    return TARGET_BLOCKSTORAGE_COUNT;
}

// bool BlockStorageList::FindDeviceForPhysicalAddress( BlockStorageDevice** pBSD, unsigned int PhysicalAddress,
// ByteAddress &SectAddress)
//{
//    *pBSD = NULL;
//    return false;
//}
bool BlockStorageList_FindDeviceForPhysicalAddress(
    BlockStorageDevice **pBSD,
    unsigned int physicalAddress,
    ByteAddress *blockAddress)
{
    return false;
}

// bool BlockStorageStream::Initialize(unsigned int blockUsage)
//{
//    return false;
//}
//
// bool BlockStorageStream::Initialize(unsigned int usage, BlockStorageDevice* pDevice)
//{
//    return false;
//}
//
//
// unsigned int BlockStorageStream::CurrentAddress()
//{
//    return 0xFFFFFFFF;
//}
//
// bool BlockStorageStream::PrevStream()
//{
//    return false;
//}
//
// bool BlockStorageStream::NextStream()
//{
//    return false;
//}
//
// bool BlockStorageStream::Seek( signed int offset, SeekOrigin origin )
//{
//    return true;
//}
//
// bool BlockStorageStream::Erase( unsigned int length )
//{
//    return true;
//}
//
// bool BlockStorageStream::Write( unsigned char* data  , unsigned int length )
//{
//    return true;
//}
//
// bool BlockStorageStream::ReadIntoBuffer( unsigned char* pBuffer, unsigned int length )
//{
//    return true;
//}
//
// bool BlockStorageStream::Read( unsigned char** ppBuffer, unsigned int length )
//{
//    return true;
//}
//

//--//

// SectorAddress DeviceBlockInfo::PhysicalToSectorAddress( const BlockRegionInfo* pRegion, ByteAddress phyAddress )
// const
//{
//    return phyAddress;
//}
//
// bool DeviceBlockInfo::FindRegionFromAddress(ByteAddress Address, unsigned int &BlockRegionIndex, unsigned int
// &BlockRangeIndex ) const
//{
//    return false;
//}
//
// bool DeviceBlockInfo::FindNextUsageBlock(unsigned int BlockUsage, ByteAddress &Address, unsigned int
// &BlockRegionIndex, unsigned int &BlockRangeIndex ) const
//{
//    return false;
//}
//
// bool DeviceBlockInfo::FindForBlockUsage(unsigned int BlockUsage, ByteAddress &Address, unsigned int
// &BlockRegionIndex, unsigned int &BlockRangeIndex ) const
//{
//    return false;
//}

//--//
