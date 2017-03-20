//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_STORAGE_H_
#define _TARGET_STORAGE_H_ 1

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

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

typedef struct BlockStorageStream
{
    uint32_t BaseAddress;
    uint32_t CurrentIndex;
    uint32_t Length;
    uint32_t Usage;
    uint32_t Flags;

}BlockStorageStream;

typedef struct BlockStorageDevice
{

}BlockStorageDevice;

#define BlockStorageStream_CurrentAddress(stream)    (stream->BaseAddress + stream->CurrentIndex)

#ifdef __cplusplus
extern "C" {
#endif

bool BlockStorageStream_Initialize(BlockStorageStream* stream, uint32_t storageUsage); 
bool BlockStorageStream_Read(BlockStorageStream* stream, unsigned char** buffer, uint32_t length);
bool BlockStorageStream_Seek(BlockStorageStream* stream, int32_t offset, SeekOrigin origin);

#ifdef __cplusplus
}
#endif

#endif  //_TARGET_STORAGE_H_
