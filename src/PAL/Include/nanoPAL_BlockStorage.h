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

/////////////////////////////////////////////////////////
// Description:
//   Definitions for the Block Storage device interface
//   for the nanoFramework
// 
// Terminology:
//  Block Storage Device - device that stores data addressable
//                         as a chunk as opposed to byte or unsigned short level
//
//  Sector - smallest unit of storage for a Block device. Data is
//           read or written as a complete sector(unless the device
//           supports XIP which allows byte addresssing and reading
//           directly from the processor memory bus)
//
//  Block  - smallest eraseable unit of the storage device. A block 
//           contains one or more sectors that are erased as a whole.
//           All sectors within a block are always the same size.
//
//  Region - Group of adjacent blocks that all contain sectors of the
//           same size.
//  
//  Execute-In-Place (XIP) - Attribute of some storage types, NOR, ROM,
//          RAM, that allows direct execution of code from the device.
//          XIP capable devices are at least read-only and byte addresable
//          on the CPU memory bus.
//


// In flash media the distinction between a block and a sector is important
// so we define the sector address to make it clear.
typedef unsigned int ByteAddress; 
typedef unsigned int SectorAddress;

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
#define BlockRange_DATATYPE_NATIVECODE  ((unsigned int)0x1000)  
// Block contains managed code assemblies
#define BlockRange_DATATYPE_MANAGEDCODE ((unsigned int)0x2000)  
// Block contains raw data
#define BlockRange_DATATYPE_RAW         ((unsigned int)0x4000)  

// use to mark the block is used for special purpose
#define BlockRange_ATTRIB_PRIMARY       ((unsigned int)0x10000) 


#define BlockRange_EXECUTABLE           ((unsigned int)0x80000000)
#define BlockRange_RESERVED             ((unsigned int)0x40000000)
#define BlockRange_READONLY             ((unsigned int)0x20000000)

// Values for the Usage information (This helps map the new storage APIs to the needs of existing code)
#define BlockRange_ALL_MASK             ((unsigned int)0xFFFFFFFF)
#define BlockRange_USAGE_MASK           ((unsigned int)0x000000FF)
#define BlockRange_NON_USAGE_MASK       ((unsigned int)0xFFFFFF00)

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

#define BlockRange_GetBlockCount(block)                 (block->EndBlock - block->StartBlock + 1)
#define BlockRange_GetBlockUsage(block)                 (block.RangeType & BlockRange_USAGE_MASK)
#define BlockRange_IsDeployment(block)                  ((block.RangeType & BlockRange_USAGE_MASK) == BlockUsage_DEPLOYMENT) 
#define BlockRange_IsConfig(block)                      ((block.RangeType & BlockRange_BLOCKTYPE_CONFIG) == BlockRange_BLOCKTYPE_CONFIG) 

struct BLOCKRANGE
{
    unsigned int RangeType;
    unsigned int StartBlock;
    unsigned int EndBlock;
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

#define BlockRegionInfo_Size(region)                            (region->NumBlocks * region->BytesPerBlock)
#define BlockRegionInfo_BlockAddress(region, blockIndex)        (region->Start + (blockIndex * region->BytesPerBlock))
#define BlockRegionInfo_OffsetFromBlock(region, address)        ((address - region->Start) % region->BytesPerBlock)
#define BlockRegionInfo_BlockIndexFromAddress(region, address)  ((address - region->Start) / region->BytesPerBlock)

struct BLOCKREGIONINFO
{
    ByteAddress         Start;                  // Starting Sector address
    unsigned int            NumBlocks;              // total number of blocks in this region
    unsigned int            BytesPerBlock;          // Total number of bytes per block

    unsigned int            NumBlockRanges;
    const BlockRange    *BlockRanges;
};

/////////////////////////////////////////////////////////

typedef struct DEVICEBLOCKINFO DeviceBlockInfo;

typedef enum MediaAttribute
{
    MediaAttribute_Removable            = 0x0001,
    MediaAttribute_SupportsXIP          = 0x0002,
    MediaAttribute_WriteProtected       = 0x0004,
    MediaAttribute_SupportsCopyBack     = 0x0008,
    MediaAttribute_ErasedBitsAreZero    = 0x0010,

}MediaAttribute;

#ifdef __cplusplus
extern "C" {
#endif

__nfweak SectorAddress DeviceBlockInfo_PhysicalToSectorAddress(DeviceBlockInfo* blockInfo, const BlockRegionInfo* pRegion, ByteAddress phyAddress);
__nfweak bool DeviceBlockInfo_FindRegionFromAddress(DeviceBlockInfo* blockInfo, ByteAddress address, unsigned int* blockRegionIndex, unsigned int* blockRangeIndex);
__nfweak bool DeviceBlockInfo_FindForBlockUsage(DeviceBlockInfo* blockInfo, unsigned int blockUsage, unsigned int* address, unsigned int* blockRegionIndex, unsigned int* blockRangeIndex);
__nfweak bool DeviceBlockInfo_FindNextUsageBlock(DeviceBlockInfo* blockInfo, unsigned int blockUsage, unsigned int* address, unsigned int* blockRegionIndex, unsigned int* blockRangeIndex);

#ifdef __cplusplus
}
#endif

typedef struct DEVICEBLOCKINFO
{
    MediaAttribute Attribute;

    // count of regions in the flash.
    unsigned int NumRegions;

    // pointer to an array (NumRegions long) of region information
    const BlockRegionInfo *Regions;
};

/////////////////////////////////////////////////////////
// Description:
//    This structure defines an interface for block devices
// 
// Remarks:
//    It is possible a given system might have more than one
//    storage device type. This interface abstracts the
//    hardware sepcifics from the rest of the system.
//
//    All of the functions take at least one void* parameter
//    that normally points to a driver specific data structure
//    containing hardware specific settings to use. This
//    allows a single driver to support multiple instances of
//    the same type of storage device in the system.
//
//    The sector read and write functions provide a parameter
//    for Sector Metadata. The metadata is used for flash arrays
//    without special controllers to manage wear leveling etc...
//    (mostly for directly attached NOR and NAND). The metadata
//    is used by upper layers for wear leveling to ensure that
//    data is moved around on the flash when writing to prevent
//    failure of the device from too many erase cycles on a sector. 
// 
// TODO:
//    Define standard method of notification that media is
//    removed for all removeable media. This will likely
//    be a continuation so that the FS Manager can mount 
//    an FS and then notify the managed app of the new FS.
//
struct IBlockStorageDevice
{
    /////////////////////////////////////////////////////////
    // Description:
    //    Initializes a given block device for use
    // 
    // Input:
    //
    // Returns:
    //   true if succesful; false if not
    //
    // Remarks:
    //    No other functions in this interface may be called
    //    until after Init returns.
    //
    bool (*InitializeDevice)(void*);
    
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
    bool (*UninitializeDevice)(void*);

    /////////////////////////////////////////////////////////
    // Description:
    //    Gets the information describing the device
    //
    const DeviceBlockInfo*  (*GetDeviceInfo)(void*);
    
    /////////////////////////////////////////////////////////
    // Description:
    //    Reads data from a set of sectors
    //
    // Input:
    //    StartSector - Starting Sector for the read
    //    NumSectors  - Number of sectors to read
    //    pSectorBuff - pointer to buffer to read the data into.
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
    bool (*Read)(void*, ByteAddress StartSector, unsigned int NumBytes, unsigned char* pSectorBuff);

    /////////////////////////////////////////////////////////
    // Description:
    //    Writes data to a set of sectors
    //
    // Input:
    //    StartSector - Starting Sector for the write
    //    NumSectors  - Number of sectors to write
    //    pSectorBuff - pointer to data to write.
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
    bool (*Write)(void*, ByteAddress Address, unsigned int NumBytes, unsigned char* pSectorBuf, bool ReadModifyWrite);

    bool (*Memset)(void*, ByteAddress Address, unsigned char Data, unsigned int NumBytes);

    //bool (*GetSectorMetadata)(void*, ByteAddress SectorStart, SectorMetadata* pSectorMetadata);

    //bool (*SetSectorMetadata)(void*, ByteAddress SectorStart, SectorMetadata* pSectorMetadata);

    /////////////////////////////////////////////////////////
    // Description:
    //    Check a block is erased or not.
    // 
    // Input:
    //    BlockStartAddress - Logical Sector Address
    //
    // Returns:
    //   true if it is erassed, otherwise false
    //
    // Remarks:
    //    Check  the block containing the sector address specified.
    //    
    bool (*IsBlockErased)(void*, ByteAddress BlockStartAddress, unsigned int BlockLength);

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
    bool (*EraseBlock)(void*, ByteAddress Address);
    
    /////////////////////////////////////////////////////////
    // Description:
    //   Changes the power state of the device
    // 
    // Input:
    //    State   - true= power on; false = power off
    //
    // Remarks:
    //   This function allows systems to conserve power by 
    //   shutting down the hardware when the system is 
    //   going into low power states.
    //
    void (*SetPowerState)(void*, unsigned int State);

    unsigned int (*MaxSectorWrite_uSec)(void*);

    unsigned int (*MaxBlockErase_uSec)(void*);
};

/////////////////////////////////////////////////////////////////////
typedef struct BLOCKSTORAGEDEVICE BlockStorageDevice;

#ifdef __cplusplus
extern "C" {
#endif

__nfweak BlockStorageDevice* BlockStorageDevice_Next(BlockStorageDevice* device);
__nfweak BlockStorageDevice* BlockStorageDevice_Prev(BlockStorageDevice* device);
__nfweak bool BlockStorageDevice_InitializeDevice(BlockStorageDevice* device);
__nfweak bool BlockStorageDevice_UninitializeDevice(BlockStorageDevice* device);
__nfweak DeviceBlockInfo* BlockStorageDevice_GetDeviceInfo(BlockStorageDevice* device);
__nfweak bool BlockStorageDevice_Read(BlockStorageDevice* device, unsigned int address, unsigned int numBytes, unsigned char* pSectorBuff);
__nfweak bool BlockStorageDevice_Write(BlockStorageDevice* device, unsigned int address, unsigned int numBytes, unsigned char* pSectorBuf, bool readModifyWrite);
__nfweak bool BlockStorageDevice_Memset(BlockStorageDevice* device, unsigned int address, unsigned char data, unsigned int NumBytes);
//__nfweak bool BlockStorageDevice_GetSectorMetadata(BlockStorageDevice* device, unsigned int sectorStart, SectorMetadata* pSectorMetadata);
//__nfweak bool BlockStorageDevice_SetSectorMetadata(BlockStorageDevice* device, unsigned int sectorStart, SectorMetadata* pSectorMetadata);
__nfweak bool BlockStorageDevice_IsBlockErased(BlockStorageDevice* device, unsigned int blockStartAddress, unsigned int blockLength);
__nfweak bool BlockStorageDevice_EraseBlock(BlockStorageDevice* device, unsigned int address);
__nfweak void BlockStorageDevice_SetPowerState(BlockStorageDevice* device, unsigned int state);
__nfweak bool BlockStorageDevice_FindRegionFromAddress(BlockStorageDevice* device, unsigned int address, unsigned int* blockRegionIndex, unsigned int* blockRangeIndex);
__nfweak bool BlockStorageDevice_FindForBlockUsage(BlockStorageDevice* device, unsigned int blockUsage, unsigned int* address, unsigned int* blockRegionIndex, unsigned int* blockRangeIndex);
__nfweak bool BlockStorageDevice_FindNextUsageBlock(BlockStorageDevice* device, unsigned int blockUsage, unsigned int* address, unsigned int* blockRegionIndex, unsigned int* blockRangeIndex);
__nfweak unsigned int BlockStorageDevice_MaxSectorWrite_uSec(BlockStorageDevice* device);
__nfweak unsigned int BlockStorageDevice_MaxBlockErase_uSec(BlockStorageDevice* device);

#ifdef __cplusplus
}
#endif

struct BLOCKSTORAGEDEVICE
{
    // from templace class HAL_DblLinkedNode<BlockStorageDevice>
    BlockStorageDevice* m_nextNode;
    BlockStorageDevice* m_prevNode;

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

__nfweak bool BlockStorageStream_Initialize(BlockStorageStream* stream, unsigned int blockUsage);
__nfweak bool BlockStorageStream_InitializeWithBlockStorageDevice(BlockStorageStream* stream, unsigned int blockUsage, BlockStorageDevice* pDevice);
__nfweak bool BlockStorageStream_NextStream(BlockStorageStream* stream);
__nfweak bool BlockStorageStream_PrevStream(BlockStorageStream* stream);
__nfweak bool BlockStorageStream_Seek(BlockStorageStream* stream, int32_t offset, SeekOrigin origin);
__nfweak bool BlockStorageStream_Write(BlockStorageStream* stream, unsigned char* data, unsigned int length);
__nfweak bool BlockStorageStream_Erase(BlockStorageStream* stream, unsigned int length);
__nfweak bool BlockStorageStream_ReadIntoBuffer(BlockStorageStream* stream, unsigned char* buffer, unsigned int length);
__nfweak bool BlockStorageStream_Read(BlockStorageStream* stream, unsigned char** buffer, unsigned int length);
__nfweak unsigned int BlockStorageStream_CurrentAddress(BlockStorageStream* stream);
__nfweak bool BlockStorageStream_IsErased(BlockStorageStream* stream, unsigned int length);

#ifdef __cplusplus
}
#endif

#define BLOCKSTORAGESTREAM_STREAM_SEEK_NEXT_BLOCK               ((unsigned int)0x7FFFFFFF)
#define BLOCKSTORAGESTREAM_STREAM_SEEK_PREV_BLOCK               ((unsigned int)0x7FFFFFFE)

#define BLOCKSTORAGESTREAM_c_BlockStorageStream__XIP            ((unsigned int)0x00000001)
#define BLOCKSTORAGESTREAM_c_BlockStorageStream__ReadModWrite   ((unsigned int)0x00000002)

struct BLOCKSTORAGESTREAM
{
    unsigned int BaseAddress;
    unsigned int CurrentIndex;
    unsigned int Length;
    unsigned int BlockLength;    
    unsigned int Usage;
    unsigned int Flags;
    unsigned int RegionIndex;
    unsigned int RangeIndex;
    unsigned int CurrentUsage;
    BlockStorageDevice *Device;

};

#endif // _NANOPAL_BLOCKSTORAGE_H_
