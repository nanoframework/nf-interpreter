//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOPAL_BLOCKSTORAGE_H_
#define _NANOPAL_BLOCKSTORAGE_H_ 1

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <nanoWeak.h>

//////////////////////////////////////////////////////////
// Description:
//   Flags to Indicate the status of a particular block in
//   flash. ALL sectors for a block *MUST* share the same
//   status.
//
// Remarks:
//   When the block is reserved the lower 16 bits specifies a
//   USAGE_XXXX value to indicate the purpose of the block.
// 
//   The BlockRange information is technically dynamic in the 
//   sense that it is not fixed or defined in hardware. Rather,
//   it is defined by the system designer. However, the BlockRange
//   info does not and *MUST* not change at run-time.  The only
//   exceptions to this rule is in the boot loader where the
//   storage device is being formated or re-laid out as part
//   of an update etc... and when an FS discovers a bad block
//   it should mark it as bad. WIthout this restriction
//   implementing an FS is virtually impossible since the FS
//   would have to deal with the possibility that the usage for
//   a Block could change at any point in time.
//
typedef enum BlockUsage
{
    BlockUsage_BOOTSTRAP   = 0x0010,
    BlockUsage_CODE        = 0x0020,
    BlockUsage_CONFIG      = 0x0030,
    BlockUsage_FILESYSTEM  = 0x0040,    

    BlockUsage_DEPLOYMENT  = 0x0050, 

    BlockUsage_UPDATE      = 0x0060,

    BlockUsage_SIMPLE_A    = 0x0090,
    BlockUsage_SIMPLE_B    = 0x00A0,
    
    BlockUsage_STORAGE_A   = 0x00E0,
    BlockUsage_STORAGE_B   = 0x00F0,


    ANY         = 0x0000,
    
}BlockUsage;

// upper 4 bits of USAGE_XXXX identifies the base type of info stored in the block
// using these values it is possible to scan a device for all managed code or all
// native code without worrying about the finer details of what it's for.

// Block contains XIP system native code
#define BlockRange_DATATYPE_NATIVECODE  ((uint32_t)0x1000)  
// Block contains managed code assemblies
#define BlockRange_DATATYPE_MANAGEDCODE ((uint32_t)0x2000)  
// Block contains raw data
#define BlockRange_DATATYPE_RAW         ((uint32_t)0x4000)  

// use to mark the block is used for special purpose
#define BlockRange_ATTRIB_PRIMARY       ((uint32_t)0x10000) 


#define BlockRange_EXECUTABLE           ((uint32_t)0x80000000)
#define BlockRange_RESERVED             ((uint32_t)0x40000000)
#define BlockRange_READONLY             ((uint32_t)0x20000000)

// Values for the Usage information (This helps map the new storage APIs to the needs of existing code)
#define BlockRange_ALL_MASK             ((uint32_t)0xFFFFFFFF)
#define BlockRange_USAGE_MASK           ((uint32_t)0x000000FF)
#define BlockRange_NON_USAGE_MASK       ((uint32_t)0xFFFFFF00)

// for secondary devices to set dirtybits         
#define BlockRange_BLOCKTYPE_RESERVED   (BlockRange_RESERVED)
#define BlockRange_BLOCKTYPE_DIRTYBIT   (                            BlockRange_RESERVED |       BlockRange_DATATYPE_RAW         | BlockUsage_CONFIG)    
// Configuration data that contains all the unique data 
#define BlockRange_BLOCKTYPE_CONFIG     (BlockRange_ATTRIB_PRIMARY | BlockRange_RESERVED |       BlockRange_DATATYPE_RAW        | BlockUsage_CONFIG)     

// Boot loader and boot strap code
#define BlockRange_BLOCKTYPE_BOOTSTRAP  (BlockRange_EXECUTABLE     | BlockRange_RESERVED |       BlockRange_DATATYPE_NATIVECODE  | BlockUsage_BOOTSTRAP)  
// CLR or other native code "application"
#define BlockRange_BLOCKTYPE_CODE       (BlockRange_EXECUTABLE     | BlockRange_RESERVED |       BlockRange_DATATYPE_NATIVECODE  | BlockUsage_CODE)       
// Deployment area for MFdeploy & Visual Studio
#define BlockRange_BLOCKTYPE_DEPLOYMENT (                            BlockRange_RESERVED |       BlockRange_DATATYPE_MANAGEDCODE | BlockUsage_DEPLOYMENT)
// Part A of Simple Storage
#define BlockRange_BLOCKTYPE_SIMPLE_A   (                            BlockRange_RESERVED |       BlockRange_DATATYPE_RAW         | BlockUsage_SIMPLE_A)
// Part B of Simple Storage
#define BlockRange_BLOCKTYPE_SIMPLE_B   (                            BlockRange_RESERVED |       BlockRange_DATATYPE_RAW         | BlockUsage_SIMPLE_B)   
// Part A of EWR Storage
#define BlockRange_BLOCKTYPE_STORAGE_A  (                            BlockRange_RESERVED |       BlockRange_DATATYPE_RAW         | BlockUsage_STORAGE_A ) 
// Part B of EWR Storage
#define BlockRange_BLOCKTYPE_STORAGE_B  (                            BlockRange_RESERVED |       BlockRange_DATATYPE_RAW         | BlockUsage_STORAGE_B) 
// File System 
#define BlockRange_BLOCKTYPE_FILESYSTEM (                            BlockRange_DATATYPE_RAW                                     | BlockUsage_FILESYSTEM) 
// Used for MFUpdate for firmware/assembly/etc updates
#define BlockRange_BLOCKTYPE_UPDATE     (                            BlockRange_RESERVED |       BlockRange_DATATYPE_RAW         | BlockUsage_UPDATE)     

////////////////////////////////////////////////////////
//

typedef struct BLOCKRANGE BlockRange;

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

__nfweak bool BlockStorageStream_IsXIP(BlockStorageStream* stream);
__nfweak bool BlockStorageStream_IsReadModifyWrite(BlockStorageStream* stream);
__nfweak bool BlockStorageStream_SetReadModifyWrite(BlockStorageStream* stream);

__nfweak bool BlockStorageStream_Initialize(BlockStorageStream* stream, uint32_t blockUsage);
__nfweak bool BlockStorageStream_InitializeWithBlockStorageDevice(BlockStorageStream* stream, uint32_t blockUsage, BlockStorageDevice* pDevice);
__nfweak bool BlockStorageStream_NextStream(BlockStorageStream* stream);
__nfweak bool BlockStorageStream_PrevStream(BlockStorageStream* stream);
__nfweak bool BlockStorageStream_Seek(BlockStorageStream* stream, int32_t offset, SeekOrigin origin);
__nfweak bool BlockStorageStream_Write(BlockStorageStream* stream, unsigned char* data, uint32_t length);
__nfweak bool BlockStorageStream_Erase(BlockStorageStream* stream, uint32_t length);
__nfweak bool BlockStorageStream_ReadIntoBuffer(BlockStorageStream* stream, unsigned char* buffer, uint32_t length);
__nfweak bool BlockStorageStream_Read(BlockStorageStream* stream, unsigned char** buffer, uint32_t length);
__nfweak uint32_t BlockStorageStream_CurrentAddress(BlockStorageStream* stream);
__nfweak bool BlockStorageStream_IsErased(BlockStorageStream* stream, uint32_t length);

#ifdef __cplusplus
}
#endif

#define BLOCKSTORAGESTREAM_STREAM_SEEK_NEXT_BLOCK               ((uint32_t)0x7FFFFFFF)
#define BLOCKSTORAGESTREAM_STREAM_SEEK_PREV_BLOCK               ((uint32_t)0x7FFFFFFE)

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

};

#endif // _NANOPAL_BLOCKSTORAGE_H_
