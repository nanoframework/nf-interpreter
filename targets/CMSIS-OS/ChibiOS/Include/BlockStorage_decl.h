//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _DRIVERS_PAL_BLOCKSTORAGE_H_
#define _DRIVERS_PAL_BLOCKSTORAGE_H_ 1

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <nanoWeak.h>

typedef struct BLOCKRANGE BlockRange;

typedef enum BlockRange_BLOCKTYPE
{
    BlockRange_BLOCKTYPE_CODE       = 0xC0001020,       // CLR or other native code "application"
    BlockRange_BLOCKTYPE_DEPLOYMENT = 0x40002050,       // Deployment area for ANT & Visual Studio

}BlockRange_BLOCKTYPE;

#define BlockRange_USAGE_MASK         ((uint32_t)0x000000FF)

#define BlockRange_GetBlockCount(block)                (block.EndBlock - block.StartBlock + 1)
#define BlockRange_GetBlockUsage(block)                (block.RangeType & BlockRange_USAGE_MASK)

struct BLOCKRANGE
{
    uint32_t RangeType;
    uint32_t StartBlock;
    uint32_t EndBlock;
};

/////////////////////////////////////////////////////////
// Description:
//    This structure defines characteristics of a particular
//    region of a block device.
// 
// Remarks:
//    There is often more than one instance of this structure for each 
//    block device. 
//
//    The BytesPerBlock value is an optimization to prevent the need
//    to routinely calculate it from SectorsPerBlock * DataBytesPerSector
//

typedef struct BLOCKREGIONINFO BlockRegionInfo;

#define BlockRegionInfo_Size(region)                            (region.NumBlocks * region.BytesPerBlock)
#define BlockRegionInfo_BlockAddress(region, blockIndex)        (region.Start + (blockIndex * region.BytesPerBlock))
#define BlockRegionInfo_OffsetFromBlock(region, address)        ((address - region.Start) % region.BytesPerBlock)
#define BlockRegionInfo_BlockIndexFromAddress(region, address)  ((address - region.Start) / region.BytesPerBlock)

struct BLOCKREGIONINFO
{
    uint32_t            Start;                  // Starting Sector address
    uint32_t            NumBlocks;              // total number of blocks in this region
    uint32_t            BytesPerBlock;          // Total number of bytes per block

    uint32_t            NumBlockRanges;
    const BlockRange    *BlockRanges;
};

/////////////////////////////////////////////////////////

typedef struct DEVICEBLOCINFO DeviceBlockInfo;

typedef struct DEVICEBLOCINFO
{
    // count of regions in the flash.
    uint32_t NumRegions;

    // pointer to an array (NumRegions long) of region information
    const BlockRegionInfo *Regions;

    // __nfweak SectorAddress PhysicalToSectorAddress( const BlockRegionInfo* pRegion, ByteAddress phyAddress ) const;

    // __nfweak BOOL FindRegionFromAddress(ByteAddress Address, UINT32 &BlockRegionIndex, UINT32 &BlockRangeIndex ) const;

    // __nfweak BOOL FindForBlockUsage(UINT32 BlockUsage, ByteAddress &Address, UINT32 &BlockRegionIndex, UINT32 &BlockRangeIndex ) const;

    // __nfweak BOOL FindNextUsageBlock(UINT32 BlockUsage, ByteAddress &Address, UINT32 &BlockRegionIndex, UINT32 &BlockRangeIndex ) const;
};

typedef struct BLOCKSTORAGEDEVICE BlockStorageDevice;

struct BLOCKSTORAGEDEVICE
{
    // from templace class HAL_DblLinkedNode<BlockStorageDevice>
    BlockStorageDevice* m_nextNode;
    BlockStorageDevice* m_prevNode;

    BlockStorageDevice* (*Next)(BlockStorageDevice* device);
    BlockStorageDevice* (*Prev)(BlockStorageDevice* device);

    ///////////

    bool (*InitializeDevice)(BlockStorageDevice* device);
    bool (*UninitializeDevice)(BlockStorageDevice* device);
    DeviceBlockInfo* (*GetDeviceInfo)(BlockStorageDevice* device);
    bool (*Read)(BlockStorageDevice* device, uint32_t address, uint32_t numBytes, unsigned char* pSectorBuff);
    //bool (*Write)(BlockStorageDevice* device, uint32_t address, uint32_t numBytes, unsigned char* pSectorBuf, bool readModifyWrite);
    bool (*Memset)(BlockStorageDevice* device, uint32_t address, unsigned char data, uint32_t NumBytes);
    //bool (*GetSectorMetadata)(BlockStorageDevice* device, uint32_t sectorStart, SectorMetadata* pSectorMetadata);
    //bool (*SetSectorMetadata)(BlockStorageDevice* device, uint32_t sectorStart, SectorMetadata* pSectorMetadata);
    //bool (*IsBlockErased(BlockStorageDevice* device, uint32_t blockStartAddress, uint32_t blockLength);
    //bool (*EraseBlock)(BlockStorageDevice* device, uint32_t address);
    void (*SetPowerState)(BlockStorageDevice* device, uint32_t state);
    bool (*FindRegionFromAddress)(BlockStorageDevice* device, uint32_t address, void** BlockRegionIndex, void** BlockRangeIndex);
    bool (*FindForBlockUsage)(BlockStorageDevice* device, uint32_t blockUsage, void** address, void** blockRegionIndex, void** blockRangeIndex);
    bool (*FindNextUsageBlock)(BlockStorageDevice* device, uint32_t blockUsage, void** address, void** blockRegionIndex, void** blockRangeIndex);
    uint32_t (*MaxSectorWrite_uSec)(BlockStorageDevice* device);
    uint32_t (*MaxBlockErase_uSec)(BlockStorageDevice* device);

    //IBlockStorageDevice* m_BSD;
    void*                m_context;
};

/////////////////////////////////////////////////////
// BlockStorageStream declarations

typedef struct BLOCKSTORAGESTREAM BlockStorageStream;

typedef enum SeekOrigin
{
    BlockStorageStream_SeekBegin   = 0,
    BlockStorageStream_SeekCurrent = 1,
    BlockStorageStream_SeekEnd     = 2,

}SeekOrigin;

typedef enum StorageUsage
{
    StorageUsage_CLR         = 0x0020,
    StorageUsage_DEPLOYMENT  = 0x0050, 

}StorageUsage;

#ifdef __cplusplus
extern "C" {
#endif

//__nfweak bool BlockStorageStream_IsXIP(BlockStorageStream* stream);
//__nfweak bool BlockStorageStream_IsReadModifyWrite(BlockStorageStream* stream);
//__nfweak bool BlockStorageStream_SetReadModifyWrite(BlockStorageStream* stream);

__nfweak bool BlockStorageStream_Initialize(BlockStorageStream* stream, uint32_t blockUsage);
//__nfweak bool BlockStorageStream_InitializeWithBlockStorageDevice(BlockStorageStream* stream, uint32_t blockUsage, BlockStorageDevice* pDevice);
__nfweak bool BlockStorageStream_NextStream(BlockStorageStream* stream);
__nfweak bool BlockStorageStream_PrevStream(BlockStorageStream* stream);
__nfweak bool BlockStorageStream_Seek(BlockStorageStream* stream, int32_t offset, SeekOrigin origin);
//__nfweak bool BlockStorageStream_Write(BlockStorageStream* stream, unsigned char* data, uint32_t length);
//__nfweak bool BlockStorageStream_Erase(BlockStorageStream* stream, uint32_t length);
__nfweak bool BlockStorageStream_ReadIntoBuffer(BlockStorageStream* stream, unsigned char* buffer, uint32_t length);
__nfweak bool BlockStorageStream_Read(BlockStorageStream* stream, unsigned char** buffer, uint32_t length);
__nfweak uint32_t BlockStorageStream_CurrentAddress(BlockStorageStream* stream);
//__nfweak bool BlockStorageStream_IsErased(BlockStorageStream* stream, uint32_t length);

bool BlockStorageStream_Init(BlockStorageStream* stream);

#ifdef __cplusplus
}
#endif

#define BLOCKSTORAGESTREAM_STREAM_SEEK_NEXT_BLOCK   ((uint32_t)0x7FFFFFFF)
#define BLOCKSTORAGESTREAM_STREAM_SEEK_PREV_BLOCK   ((uint32_t)0x7FFFFFFE)

#define BLOCKSTORAGESTREAM_c_BlockStorageStream__XIP            ((uint32_t)0x00000001)
#define BLOCKSTORAGESTREAM_c_BlockStorageStream__ReadModWrite   ((uint32_t)0x00000002)

struct BLOCKSTORAGESTREAM
{
    uint32_t BaseAddress;
    uint32_t CurrentIndex;
    uint32_t Length;
    uint32_t BlockLength;    
    uint32_t Usage;
    uint32_t Flags;
    uint32_t RegionIndex;
    uint32_t RangeIndex;
    uint32_t CurrentUsage;
    BlockStorageDevice *Device;
    
    bool (*IsXIP)(BlockStorageStream *stream);
    bool (*IsReadModifyWrite)(BlockStorageStream *stream);
    void (*SetReadModifyWrite)(BlockStorageStream *stream);

    bool (*Initialize)(BlockStorageStream *stream, uint32_t blockUsage); 
//    bool (*InitializeWithBlockStorageDevice)(BlockStorageStream *stream, uint32_t blockUsage, BlockStorageDevice* pDevice); 
    bool (*NextStream)(BlockStorageStream *stream);
    bool (*PrevStream)(BlockStorageStream *stream);
    bool (*Seek)(BlockStorageStream* stream, int32_t offset, SeekOrigin origin);
//    bool (*Write)(BlockStorageStream* stream, unsigned char* data, uint32_t length);
//    bool (*Erase)(BlockStorageStream* stream, uint32_t length);
    bool (*ReadIntoBuffer)(BlockStorageStream* stream, unsigned char* buffer, uint32_t length);
    bool (*Read)(BlockStorageStream* stream, unsigned char** buffer, uint32_t length);
    uint32_t (*CurrentAddress)(BlockStorageStream* stream);
//    bool (*IsErased)(BlockStorageStream *stream, uint32_t length);

};

/////////////////////////////////////////////////////

#endif  // _DRIVERS_PAL_BLOCKSTORAGE_H_
