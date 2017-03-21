//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOPAL_BLOCKSTORAGE_H_
#define _NANOPAL_BLOCKSTORAGE_H_ 1

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

#endif // _NANOPAL_BLOCKSTORAGE_H_
