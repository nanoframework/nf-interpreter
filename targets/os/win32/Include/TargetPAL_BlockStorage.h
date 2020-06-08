//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _TARGET_PAL_BLOCKSTORAGE_H_
#define _TARGET_PAL_BLOCKSTORAGE_H_ 1

//--//

#include <nanoPAL_BlockStorage.h>
#include <nanoHAL_Types.h>

#if defined(__GNUC__)
#define HAL_Time_Sleep_MicroSeconds_BS(x) HAL_Time_Sleep_MicroSeconds_InRam(x)

#else
#define HAL_Time_Sleep_MicroSeconds_BS(x) HAL_Time_Sleep_MicroSeconds(x)

#endif 




// for convert byte size to storage sector size
#define CONVERTBYTETOSECTORSIZE(x,y)     ( (size_t)x + (size_t)y-1)/(size_t)y

#define CONVERTBYTETOSECTORSIZE_2(x,y,z)   ( (size_t)x + (size_t)y-1)/(size_t)z

/////////////////////////////////////////////////////////
// Description:
//   Definitions for the Block Storage device interface
//   for the .NET Micro Framework
// 
// Remarks:
//   The general design of the API is strongly influenced
//   by the Windows Embedded CE Flash Media Driver (FMD)
//   API. However, It is not intended to be a direct port
//   of the FMD architecture to the .NET MF. This design 
//   overcomes some of the limitations of the CE design
//   and includes functionalty an support that is specific
//   to the .NET MF.
//
//   In particular this design adds the following:
//   * Generalized block storage rather than assuming 
//     a Resident Flash Array (RFA)
//   * Support for multiple instances of block storage
//     devices instead of only one
//   * Support for legacy CLR Block usage information to 
//     Identify for the system where various types of
//     information reside in the storage device. 
//   * Support for direct use of NOR flash without forcing
//     additional layering for metadata if an FS is not 
//     needed or desired on the NOR flash.
//   * Support for XIP storage media like NOR and ROM.
//   * Use of an interface (as a structure of function
//     pointers) to allow layering and standardized support
//     for various common functionality. (e.g. treating 
//     NOR flash with varying sized sectors and no metadata
//     as if it was NAND with fixed sized sectors including
//     metadata. Thus this kind functionality need only be
//     implemented once)
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


// struct BlockRange
// {
   

// public:

//     static bool IsBlocknanoBooterAgnostic( uint32_t BlockType )
//     {
//         // The only blocks that should be distinguished by nanoBooter are CONFIG, 
//         // Bootstrap and reserved blocks (DirtyBit is another version of CONFIG).
//         if( BlockType == BlockRange_BLOCKTYPE_BOOTSTRAP || 
//             BlockType == BlockRange_BLOCKTYPE_CONFIG    ||
//             BlockType == BlockRange_BLOCKTYPE_RESERVED  ||
//             BlockType == BlockRange_BLOCKTYPE_DIRTYBIT)
//         {
//             return FALSE;
//         }    

//         return TRUE;        
//     }
    
//     bool IsReserved() const      { return ((RangeType & BlockRange_RESERVED) == BlockRange_RESERVED);       }
//     bool IsReadOnly() const      { return ((RangeType & BlockRange_READONLY) == BlockRange_READONLY);       }

//     bool IsReservedData() const    { return ((RangeType & (BlockRange_RESERVED | BlockRange_DATATYPE_RAW )) == (BlockRange_RESERVED | BlockRange_DATATYPE_RAW )); }
//     bool HasManagedCode() const { return ((RangeType & (BlockRange_DATATYPE_MANAGEDCODE    )) == (BlockRange_DATATYPE_MANAGEDCODE    )); }

//     bool IsCode() const               { return ((RangeType & BlockRange_USAGE_MASK) == BlockUsage_CODE);     }
//     bool IsBootstrap() const        { return ((RangeType & BlockRange_USAGE_MASK) == BlockUsage_BOOTSTRAP);}
//     bool IsDirtyBit() const         { return ((RangeType & BlockRange_BLOCKTYPE_CONFIG) == BlockRange_BLOCKTYPE_DIRTYBIT);}
//     bool IsConfig() const           { return ((RangeType & BlockRange_BLOCKTYPE_CONFIG) == BlockRange_BLOCKTYPE_CONFIG); }
//     bool IsDeployment() const  { return ((RangeType & BlockRange_USAGE_MASK) == BlockUsage_DEPLOYMENT);}
//     bool IsFileSystem() const     { return ((RangeType & BlockRange_USAGE_MASK) == BlockUsage_FILESYSTEM); }
//     uint32_t GetBlockCount() const    { return (EndBlock - StartBlock + 1); }

//     // NOTE: This is the application native code only (not including the managed DAT section)
//     //       and thus is different from the old MEMORY_USAGE_CODE which contained both 
//     //       the Native code application and the DAT section. Obviously these inlines can be
//     //       altered or added upon to test for any combination of the flags as desired but seperating
//     //       the DAT region out on it's own allows for locating ANY managed code sections of storage
//     bool IsLegacyCode() const    { return (IsCode());     }
//     uint32_t GetBlockUsage() const { return (RangeType & BlockRange_USAGE_MASK); }

        
//     /*
//        Due to the lack of a defined bit ordering for bit fields in the C/C++
//        languages a bit field structure isn't actually used but this should
//        help clarify the layout and intent of the constant declarations below.
        
//         //MSB
//         unsigned EXECUTABLE:1;
//         unsigned RESERVED:1;
//         unsigned READONLY:1;
//         unsigned UNUSEDBITS:13;
        
//         // The lower 16 bits are used to define the specific
//         // usage for blocks when the RESERVED bit set
//         unsigned BlockType:4
//         unsigned Usage:12;
//         //LSB
//     */    
//     uint32_t RangeType;
//     uint32_t StartBlock;
//     uint32_t EndBlock;
// };    

// /////////////////////////////////////////////////////////
// // Description:
// //    This structure defines characteristics of a particular
// //    region of a block device.
// // 
// // Remarks:
// //    There is often more than one instance of this structure for each 
// //    block device. 
// //
// //    The BytesPerBlock value is an optimization to prevent the need
// //    to routinely caclulate it from SectorsPerBlock * DataBytesPerSector
// //
// struct BlockRegionInfo
// {
//     uint32_t      Size()                                const { return (NumBlocks * BytesPerBlock);            }
//     ByteAddress BlockAddress(uint32_t blockIndex)       const { return (Start + (blockIndex * BytesPerBlock)); }
//     uint32_t      OffsetFromBlock(uint32_t Address)       const { return ((Address - Start) % BytesPerBlock);    }
//     uint32_t      BlockIndexFromAddress(uint32_t Address) const { return ((Address - Start) / BytesPerBlock);    }

//     ByteAddress     Start;                  // Starting Sector address
//     uint32_t          NumBlocks;              // total number of blocks in this region
//     uint32_t          BytesPerBlock;          // Total number of bytes per block

//     uint32_t            NumBlockRanges;
//     const BlockRange *BlockRanges;
// };

// /////////////////////////////////////////////////////////
// // Description:
// //    This structure defines characteristics of a particular
// //    block device.
// // 
// // Remarks:
// //    THere is only one instance of this structure for each 
// //    block device. 
// //

// struct MediaAttribute
// {
//     bool Removable :1;
//     bool SupportsXIP:1;
//     bool WriteProtected:1;
//     bool SupportsCopyBack:1;
//     bool ErasedBitsAreZero:1;
// };
    
// ///////////////////////////////////////////////////////////
    
// struct DeviceBlockInfo
// {
//     // indicates if the storage media is removeable

//     MediaAttribute Attribute;

//     // Maximum Sector Write Time.
//     uint32_t MaxSectorWrite_uSec;    

//     // Maximum erase time for the a block
//     uint32_t MaxBlockErase_uSec;    

//     // Bytes Per Sector
//     uint32_t BytesPerSector;         

//     // Total Size
//     ByteAddress Size;

//     // count of regions in the flash.
//     uint32_t NumRegions;

//     // pointer to an array (NumRegions long) of region information
//     const BlockRegionInfo *Regions;

//     __nfweak SectorAddress PhysicalToSectorAddress( const BlockRegionInfo* pRegion, ByteAddress phyAddress ) const;

//     __nfweak bool FindRegionFromAddress(ByteAddress Address, uint32_t &BlockRegionIndex, uint32_t &BlockRangeIndex ) const;

//     __nfweak bool FindForBlockUsage(uint32_t BlockUsage, ByteAddress &Address, uint32_t &BlockRegionIndex, uint32_t &BlockRangeIndex ) const;

//     __nfweak bool FindNextUsageBlock(uint32_t BlockUsage, ByteAddress &Address, uint32_t &BlockRegionIndex, uint32_t &BlockRangeIndex ) const;
// };

// ///////////////////////////////////////////////////////////////
// // Description:
// //    This structure describes the sector Metadata used for wear
// //    leveling. 
// //
// // Remarks:
// //    This structure emulates the typical physical layout of the
// //    extra area of flash. The wear leveling layer for NAND and
// //    NOR flash supplied by Microsoft for Windows CE makes use
// //    of 8 bytes of the Sector Extra Info area.
// //
// //    This information is designed to match that used in
// //    Windows Embedded CE systems so that the FAL algorithms from
// //    CE can be more easily ported to the .NET MF. 
// //
// // The following is a typical representation of how the extra area
// // is utilized:
// //- - - - - - - - - - - - - - - - 
// //|R|R|R|R|O|V|R|R|E|E|E|E|E|E|E|E|
// //- - - - - - - - - - - - - - - -
// //
// //The following table describes each element.
// //
// //Element  Description  
// //   R     Reserved bytes used by the FAL
// //   O     Byte for use by the OEM
// //   V     Byte indicating if the block is valid (a.k.a. bad)
// //   E     Bytes typically used for by a NAND driver for ECC
// //
// struct GNU_PACKED SectorMetadata
// {
//     unsigned long dwReserved1; // Used by the FAL to hold the logical to physical sector mapping information.
//     unsigned char bOEMReserved; // For use by OEM. See OEMReservedBits for more information.
//     unsigned char bBadBlock;    // Indicates if a block is bad.
//     unsigned short wReserved2;   // Used by the FAL to maintain state information about the sector.
    
//     // TODO: Check ECC algorithm implementations on CE to see what data type works most conveniently for this
//     uint32_t ECC[2];     // Error Correction Code [Should be all 0xFF if not used]

//     // Remarks:
//     //    Any sectors that the OEM does not want the wear leveling
//     //    code to touch should have both of these bits set. This
//     //    includes the sectors that include the boot loader and any
//     //    other flash data that exists at fixed locations.
//     //
//     // Note:
//     //   Because only full blocks can be erased, all sectors within
//     //   a block should have the same values for these flags.
//     //
//     static const unsigned char OEM_BLOCK_RESERVED = 0x01;
//     static const unsigned char OEM_BLOCK_READONLY = 0x02;
// };

//--//

// struct BLOCK_CONFIG
// {
//     GPIO_FLAG               WriteProtectionPin;
//     const DeviceBlockInfo*  BlockDeviceInformation;
// };

// UNDONE: FIXME: struct MEMORY_MAPPED_NOR_BLOCK_CONFIG
//{
//    BLOCK_CONFIG            BlockConfig;
//    CPU_MEMORY_CONFIG       Memory;
//    uint32_t                  ChipProtection;
//    uint32_t                  ManufacturerCode;
//    uint32_t                  DeviceCode;
//};

// /////////////////////////////////////////////////////////
// // Description:
// //    This structure defines an interface for block devices
// // 
// // Remarks:
// //    It is possible a given system might have more than one
// //    storage device type. This interface abstracts the
// //    hardware sepcifics from the rest of the system.
// //
// //    All of the functions take at least one void* parameter
// //    that normally points to a driver specific data structure
// //    containing hardware specific settings to use. This
// //    allows a single driver to support multiple instances of
// //    the same type of storage device in the system.
// //
// //    The sector read and write functions provide a parameter
// //    for Sector Metadata. The metadata is used for flash arrays
// //    without special controllers to manage wear leveling etc...
// //    (mostly for directly attached NOR and NAND). The metadata
// //    is used by upper layers for wear leveling to ensure that
// //    data is moved around on the flash when writing to prevent
// //    failure of the device from too many erase cycles on a sector. 
// // 
// // TODO:
// //    Define standard method of notification that media is
// //    removed for all removeable media. This will likely
// //    be a continuation so that the FS Manager can mount 
// //    an FS and then notify the managed app of the new FS.
// //
// struct IBlockStorageDevice
// {
//     /////////////////////////////////////////////////////////
//     // Description:
//     //    Initializes a given block device for use
//     // 
//     // Input:
//     //
//     // Returns:
//     //   true if succesful; false if not
//     //
//     // Remarks:
//     //    No other functions in this interface may be called
//     //    until after Init returns.
//     //
//     bool (*InitializeDevice)(void*);
    
//     /////////////////////////////////////////////////////////
//     // Description:
//     //    Initializes a given block device for use
//     // 
//     // Returns:
//     //   true if succesful; false if not
//     //
//     // Remarks:
//     //   De initializes the device when no longer needed
//     //
//     bool (*UninitializeDevice)(void*);

//     /////////////////////////////////////////////////////////
//     // Description:
//     //    Gets the information describing the device
//     //
//     const DeviceBlockInfo*  (*GetDeviceInfo)(void*);
    
//     /////////////////////////////////////////////////////////
//     // Description:
//     //    Reads data from a set of sectors
//     //
//     // Input:
//     //    StartSector - Starting Sector for the read
//     //    NumSectors  - Number of sectors to read
//     //    pSectorBuff - pointer to buffer to read the data into.
//     //                  Must be large enough to hold all of the data
//     //                  being read.
//     //
//     //    pSectorMetadata - pointer to an array of structured (one for each sector)
//     //                      for the extra sector information.
//     // 
//     // Returns:
//     //   true if succesful; false if not
//     //
//     // Remarks:
//     //   This function reads the number of sectors specified from the device.
//     //   
//     //   pSectorBuff may be NULL. This is to allow for reading just the metadata.
//     // 
//     //   pSectorMetadata can be set to NULL if the caller does not need the extra
//     //   data.
//     //
//     //   If the device does not support sector Metadata it should fail if the 
//     //   pSectorMetadata parameter is not NULL.
//     //
//     bool (*Read)(void*, ByteAddress StartSector, uint32_t NumBytes, unsigned char* pSectorBuff);

//     /////////////////////////////////////////////////////////
//     // Description:
//     //    Writes data to a set of sectors
//     //
//     // Input:
//     //    StartSector - Starting Sector for the write
//     //    NumSectors  - Number of sectors to write
//     //    pSectorBuff - pointer to data to write.
//     //                  Must be large enough to hold complete sectors
//     //                  for the number of sectors being written.
//     //
//     //    pSectorMetadata - pointer to an array of structures (one for each sector)
//     //                      for the extra sector information.
//     // 
//     // Returns:
//     //   true if succesful; false if not
//     //
//     // Remarks:
//     //   This function reads the number of sectors specified from the device.
//     //   The SectorMetadata is used for flash arrays without special controllers
//     //   to manage wear leveling etc... (mostly for NOR and NAND). The metadata
//     //   is used by upper layers to ensure that data is moved around on the flash
//     //   when writing to prevent failure of the device from too many erase cycles
//     //   on a sector. 
//     //   
//     //   If the device does not support sector Metadata it should fail if the 
//     //   pSectorMetadata parameter is not NULL.
//     //
//     //   pSectorMetadata can be set to NULL if the caller does not need the extra
//     //   data. Implementations must not attempt to write data through a NULL pointer! 
//     //
//     bool (*Write)(void*, ByteAddress Address, uint32_t NumBytes, unsigned char* pSectorBuf, bool ReadModifyWrite);

//     bool (*Memset)(void*, ByteAddress Address, unsigned char Data, uint32_t NumBytes);

//     bool (*GetSectorMetadata)(void*, ByteAddress SectorStart, SectorMetadata* pSectorMetadata);

//     bool (*SetSectorMetadata)(void*, ByteAddress SectorStart, SectorMetadata* pSectorMetadata);

//     /////////////////////////////////////////////////////////
//     // Description:
//     //    Check a block is erased or not.
//     // 
//     // Input:
//     //    BlockStartAddress - Logical Sector Address
//     //
//     // Returns:
//     //   true if it is erassed, otherwise false
//     //
//     // Remarks:
//     //    Check  the block containing the sector address specified.
//     //    
//     bool (*IsBlockErased)(void*, ByteAddress BlockStartAddress, uint32_t BlockLength);

//     /////////////////////////////////////////////////////////
//     // Description:
//     //    Erases a block
//     // 
//     // Input:
//     //    Address - Logical Sector Address
//     //
//     // Returns:
//     //   true if succesful; false if not
//     //
//     // Remarks:
//     //    Erases the block containing the sector address specified.
//     //    
//     bool (*EraseBlock)(void*, ByteAddress Address);
    
//     /////////////////////////////////////////////////////////
//     // Description:
//     //   Changes the power state of the device
//     // 
//     // Input:
//     //    State   - true= power on; false = power off
//     //
//     // Remarks:
//     //   This function allows systems to conserve power by 
//     //   shutting down the hardware when the system is 
//     //   going into low power states.
//     //
//     void (*SetPowerState)(void*, uint32_t State);

//     uint32_t (*MaxSectorWrite_uSec)(void*);

//     uint32_t (*MaxBlockErase_uSec)(void*);
// };


// ////////////////////////////////////////////////
// // Description:
// //   Binding context for a driver and the physical device
// //
// // Remarks:
// //   The design pattern here effectively mimics a C++ class
// //   with virtuals. The reason virtuals are not used is that
// //   the .NET MF supports a wide variety of compiler/Linker
// //   tool chains and some of them bring in a large Run-time
// //   library footprint when Certain C++ language features are
// //   used. Since a major goal of the .NET MF is to reduce
// //   code footprint we avoid anything that brings in additional
// //   library code. 
// //

// struct BlockStorageDevice : public HAL_DblLinkedNode<BlockStorageDevice>
// {

// public:

//     /////////////////////////////////////////////////////////
//     // Description:
//     //    Initializes a given block device for use
//     // 
//     // Returns:
//     //   true if succesful; false if not
//     //
//     // Remarks:
//     //    No other functions in this interface may be called
//     //    until after Init returns.
//     //
//     bool InitializeDevice() { return this->m_BSD->InitializeDevice( this->m_context ); }
    
//     /////////////////////////////////////////////////////////
//     // Description:
//     //    Initializes a given block device for use
//     // 
//     // Returns:
//     //   true if succesful; false if not
//     //
//     // Remarks:
//     //   De initializes the device when no longer needed
//     //
//     bool UninitializeDevice() { return this->m_BSD->UninitializeDevice( this->m_context ); }

//     /////////////////////////////////////////////////////////
//     // Description:
//     //    Gets the information describing the device
//     //
//     const DeviceBlockInfo* GetDeviceInfo() { return this->m_BSD->GetDeviceInfo( this->m_context ); }

//     /////////////////////////////////////////////////////////
//     // Description:
//     //    Reads data from a set of sectors
//     //
//     // Input:
//     //    StartSector - Starting Sector for the read
//     //    NumSectors  - Number of sectors to read
//     //    pSectorBuff - pointer to buffer to read the data into.
//     //                  Must be large enough to hold all of the data
//     //                  being read.
//     //
//     //    pSectorMetadata - pointer to an array of structured (one for each sector)
//     //                      for the extra sector information.
//     // 
//     // Returns:
//     //   true if succesful; false if not
//     //
//     // Remarks:
//     //   This function reads the number of sectors specified from the device.
//     //   
//     //   pSectorBuff may be NULL. This is to allow for reading just the metadata.
//     // 
//     //   pSectorMetadata can be set to NULL if the caller does not need the extra
//     //   data.
//     //
//     //   If the device does not support sector Metadata it should fail if the 
//     //   pSectorMetadata parameter is not NULL.
//     //
//     bool Read(ByteAddress Address, uint32_t NumBytes, unsigned char* pSectorBuff) 
//     {
//         return this->m_BSD->Read(this->m_context, Address, NumBytes, pSectorBuff);
//     }

//     /////////////////////////////////////////////////////////
//     // Description:
//     //    Writes data to a set of sectors
//     //
//     // Input:
//     //    StartSector - Starting Sector for the write
//     //    NumSectors  - Number of sectors to write
//     //    pSectorBuff - pointer to data to write.
//     //                  Must be large enough to hold complete sectors
//     //                  for the number of sectors being written.
//     //
//     //    pSectorMetadata - pointer to an array of structures (one for each sector)
//     //                      for the extra sector information.
//     // 
//     // Returns:
//     //   true if succesful; false if not
//     //
//     // Remarks:
//     //   This function reads the number of sectors specified from the device.
//     //   The SectorMetadata is used for flash arrays without special controllers
//     //   to manage wear leveling etc... (mostly for NOR and NAND). The metadata
//     //   is used by upper layers to ensure that data is moved around on the flash
//     //   when writing to prevent failure of the device from too many erase cycles
//     //   on a sector. 
//     //   
//     //   If the device does not support sector Metadata it should fail if the 
//     //   pSectorMetadata parameter is not NULL.
//     //
//     //   pSectorMetadata can be set to NULL if the caller does not need the extra
//     //   data. Implementations must not attempt to write data through a NULL pointer! 
//     //
//     bool Write(ByteAddress Address, uint32_t NumBytes, unsigned char* pSectorBuf, bool ReadModifyWrite) 
//     {
//         return this->m_BSD->Write(this->m_context, Address, NumBytes, pSectorBuf, ReadModifyWrite);
//     }

//     bool Memset(ByteAddress Address, unsigned char Data, uint32_t NumBytes)
//     {
//         return this->m_BSD->Memset(this->m_context, Address, Data, NumBytes);
//     }

//     bool GetSectorMetadata(ByteAddress SectorStart, SectorMetadata* pSectorMetadata)
//     {
//         return this->m_BSD->GetSectorMetadata(this->m_context, SectorStart, pSectorMetadata);
//     }

//     bool SetSectorMetadata(ByteAddress SectorStart, SectorMetadata* pSectorMetadata)
//     {
//         return this->m_BSD->SetSectorMetadata(this->m_context, SectorStart, pSectorMetadata);
//     }

//     /////////////////////////////////////////////////////////
//     // Description:
//     //    Check a block is erased or not
//     // 
//     // Input:
//     //    Address - Logical Sector Address
//     //
//     // Returns:
//     //   true it is erased; false if not
//     //
//     // Remarks:
//     //    check the block containing the sector address specified.
//     //    
//     bool IsBlockErased(ByteAddress BlockStartAddress, uint32_t BlockLength)  { return this->m_BSD->IsBlockErased(this->m_context, BlockStartAddress, BlockLength); }


//     /////////////////////////////////////////////////////////
//     // Description:
//     //    Erases a block
//     // 
//     // Input:
//     //    Address - Logical Sector Address
//     //
//     // Returns:
//     //   true if succesful; false if not
//     //
//     // Remarks:
//     //    Erases the block containing the sector address specified.
//     //    
//     bool EraseBlock(ByteAddress Address) const { return this->m_BSD->EraseBlock(this->m_context, Address); }


//     /////////////////////////////////////////////////////////
//     // Description:
//     //   Changes the power state of the device
//     // 
//     // Input:
//     //    State   - true= power on; false = power off
//     //
//     // Remarks:
//     //   This function allows systems to conserve power by 
//     //   shutting down the hardware when the system is 
//     //   going into low power states.
//     //
//     void SetPowerState( uint32_t State ) { this->m_BSD->SetPowerState(this->m_context, State);  }

//     bool FindRegionFromAddress(ByteAddress Address, uint32_t &BlockRegionIndex, uint32_t &BlockRangeIndex ) 
//     {
//         const DeviceBlockInfo* pDevInfo = GetDeviceInfo();

//         return pDevInfo->FindRegionFromAddress( Address, BlockRegionIndex, BlockRangeIndex );
//     }

//     bool FindForBlockUsage(uint32_t blockUsage, ByteAddress &Address, uint32_t &BlockRegionIndex, uint32_t &BlockRangeIndex ) 
//     {
//         const DeviceBlockInfo* pDevInfo = GetDeviceInfo();

//         return pDevInfo->FindForBlockUsage( blockUsage, Address, BlockRegionIndex, BlockRangeIndex );
//     }

//     bool FindNextUsageBlock(uint32_t blockUsage, ByteAddress &Address, uint32_t &BlockRegionIndex, uint32_t &BlockRangeIndex ) 
//     {
//         const DeviceBlockInfo* pDevInfo = GetDeviceInfo();

//         return pDevInfo->FindNextUsageBlock( blockUsage, Address, BlockRegionIndex, BlockRangeIndex );
//     }
    

//     uint32_t MaxSectorWrite_uSec()
//     {
//         return this->m_BSD->MaxSectorWrite_uSec(this->m_context);
//     }

//     uint32_t MaxBlockErase_uSec()
//     {
//         return this->m_BSD->MaxBlockErase_uSec(this->m_context);
//     }

//     IBlockStorageDevice* m_BSD;     // Vtable for this device
//     void*                m_context; // configuration for this instance of this driver

// };
    
// -- global List

// struct BlockStorageList
// {
//     // initailize the storage
//     __nfweak static void Initialize();
    
//     // walk through list of devices and calls Init() function
//     __nfweak static bool InitializeDevices();

//     // walk through list of devices and calls UnInit() function
//     __nfweak static bool UnInitializeDevices();

//     // add pBSD to the list
//     // If Init=true, the Init() will be called.
//     __nfweak static bool AddDevice( BlockStorageDevice* pBSD, IBlockStorageDevice* vtable, void* config, bool Init);

//     // remove pBSD from the list
//     // Uninit = true, UnInit() will be called.
//     __nfweak static bool RemoveDevice( BlockStorageDevice* pBSD, bool UnInit);

//     // Find the right Device with the corresponding phyiscal address.
//     // 
//     __nfweak static bool FindDeviceForPhysicalAddress( BlockStorageDevice** pBSD, uint32_t PhysicalAddress, ByteAddress &BlockAddress);

//     __nfweak static BlockStorageDevice* GetFirstDevice();
    
//     __nfweak static BlockStorageDevice* GetNextDevice( BlockStorageDevice& device );

//     // returns number of devices has been declared in the system
//     __nfweak static uint32_t GetNumDevices();

//     // pointer to the BlockStorageDevice which is the primary device with CONFIG block
//     static BlockStorageDevice* s_primaryDevice;
    
// private:
//     // global pointer of all the storage devices
//     static HAL_DblLinkedList<BlockStorageDevice> s_deviceList; 

//     static bool s_Initialized;
// };

////////////////////////////////////////////////////////////////////////////////

// functions to included all the devices to be added in the system
void BlockStorage_AddDevices();

///////////////////////////////////////////////////////////////////////////////

#define FLASH_PROTECTION_KEY      0x1055AADD



#define FLASH_BEGIN_PROGRAMMING_FAST()             { GLOBAL_LOCK(FlashIrq)
#define FLASH_BEGIN_PROGRAMMING(x)                 { uint32_t FlashOperationStatus = Flash_StartOperation( x )
#define FLASH_END_PROGRAMMING(banner,address)      Flash_EndOperation( FlashOperationStatus ); }
#define FLASH_END_PROGRAMMING_FAST(banner,address) }


//--//

#endif // _TARGET_PAL_BLOCKSTORAGE_H_

