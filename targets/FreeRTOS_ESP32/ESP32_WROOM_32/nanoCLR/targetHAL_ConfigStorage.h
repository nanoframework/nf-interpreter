//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_v2.h>
#include <nanoWeak.h>
#include "Esp32_os.h"

#define CONFIG_ERROR      0xffffffff

#define CONFIG_TYPE_NETWORK     'C'
#define CONFIG_TYPE_WIRELESS    'W'
#define CONFIG_TYPE_WIRELESSAP  'A'
#define CONFIG_TYPE_CERTIFICATE 'X'


// Config_Initialise
//
// Initialise underlying filesystem
//
void Config_Initialise();

//
//  Config_OpenFile -
//
//  Open file for configuration / index
//  For the case of NVS we just open the NVS partition, save file details, handle
//  
// Parameters:-
//   configuration      : Type of configuration block to open
//   configurationIndex : Index of type to open
//   write              : True to create/overwrite file
//
// Return :-
//    true  : returns handle
//    false -1
//
uint32_t Config_OpenFile( DeviceConfigurationOption configuration, uint32_t configurationIndex, bool write );

//
//  Config_CloseFile - Close opened file / NVS system
//  
// Parameters:-
//   handle : Handle returned from Config_openFile
//
// return:-
//    true - File closed ok 
//    false- Error/File not open/Invalid handle
//
bool Config_CloseFile( uint32_t handle );

//
//  Config_FileSize - Return size of file
//
// Parameters:-
//   handle : Handle returned from Config_openFile
//
// return:-
//    File size 
//    -1 = Error/Not found/Invalid handle
//
int32_t Config_FileSize( uint32_t handle );

//
//  Config_WriteFile -  Write data to file system
//
// Parameters:-
//   handle    : Handle returned from Config_openFile
//   pData     : Pointer data to write
//   writeSize : Size of data to write
//
// return:-
//    true - OK 
//    false- Error/Not found
//
bool Config_WriteFile(uint32_t handle, uint8_t * pData, int32_t writeSize );


//
// Config_ReadFile - Read the data from opened file
//
// Parameters:-
//   handle : Handle returned from Config_openFile
//   pData  : Ponter to where data is to be stored
//
// return:-
//    true - read length 
//    false- -1 , Error/Not found/Invalid handle
//
int32_t Config_ReadFile( uint32_t handle, uint8_t * pData, int32_t maxSizeData );

