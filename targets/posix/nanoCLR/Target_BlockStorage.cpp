//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Block storage stubs for the POSIX host target.
// There is no physical block storage on a POSIX host; all managed assembly
// loading goes through the in-memory nanoCLR_LoadAssembly() API.

#include <cstdint>
#include <cstring>
#include <nanoPAL_BlockStorage.h>

// ---------- BlockStorageList ----------

void BlockStorageList_Initialize()   { /* no devices */ }
bool BlockStorageList_InitializeDevices()   { return true; }
bool BlockStorageList_UnInitializeDevices() { return true; }

bool BlockStorageList_AddDevice(
    BlockStorageDevice *p, IBlockStorageDevice *v, void *c, bool i)
{ (void)p; (void)v; (void)c; (void)i; return false; }

bool BlockStorageList_RemoveDevice(BlockStorageDevice *p, bool u)
{ (void)p; (void)u; return false; }

bool BlockStorageList_FindDeviceForPhysicalAddress(
    BlockStorageDevice **p, unsigned int a, ByteAddress *b)
{ if (p) *p = NULL; (void)a; (void)b; return false; }

BlockStorageDevice *BlockStorageList_GetFirstDevice()            { return NULL; }
BlockStorageDevice *BlockStorageList_GetNextDevice(BlockStorageDevice *d) { (void)d; return NULL; }
unsigned int BlockStorageList_GetNumDevices()                    { return 0; }

// ---------- BlockStorageDevice ----------

DeviceBlockInfo *BlockStorageDevice_GetDeviceInfo(BlockStorageDevice *d) { (void)d; return NULL; }
bool BlockStorageDevice_InitializeDevice(BlockStorageDevice *d)          { (void)d; return false; }
bool BlockStorageDevice_UninitializeDevice(BlockStorageDevice *d)        { (void)d; return false; }

bool BlockStorageDevice_Read(BlockStorageDevice *d, ByteAddress a, unsigned int n, unsigned char *b)
{ (void)d; (void)a; (void)n; (void)b; return false; }

bool BlockStorageDevice_Write(BlockStorageDevice *d, ByteAddress a, unsigned int n, unsigned char *b, bool r)
{ (void)d; (void)a; (void)n; (void)b; (void)r; return false; }

bool BlockStorageDevice_Memset(BlockStorageDevice *d, ByteAddress a, unsigned char v, unsigned int n)
{ (void)d; (void)a; (void)v; (void)n; return false; }

bool BlockStorageDevice_IsBlockErased(BlockStorageDevice *d, ByteAddress a, unsigned int n)
{ (void)d; (void)a; (void)n; return false; }

bool BlockStorageDevice_EraseBlock(BlockStorageDevice *d, ByteAddress a) { (void)d; (void)a; return false; }
void BlockStorageDevice_SetPowerState(BlockStorageDevice *d, unsigned int s) { (void)d; (void)s; }

bool BlockStorageDevice_FindRegionFromAddress(
    BlockStorageDevice *d, ByteAddress a, unsigned int *ri, unsigned int *rgi)
{ (void)d; (void)a; (void)ri; (void)rgi; return false; }

bool BlockStorageDevice_FindForBlockUsage(
    BlockStorageDevice *d, unsigned int u, ByteAddress *a, unsigned int *ri, unsigned int *rgi)
{ (void)d; (void)u; (void)a; (void)ri; (void)rgi; return false; }

bool BlockStorageDevice_FindNextUsageBlock(
    BlockStorageDevice *d, unsigned int u, ByteAddress *a, unsigned int *ri, unsigned int *rgi)
{ (void)d; (void)u; (void)a; (void)ri; (void)rgi; return false; }

BlockStorageDevice *BlockStorageDevice_Next(BlockStorageDevice *d) { (void)d; return NULL; }
BlockStorageDevice *BlockStorageDevice_Prev(BlockStorageDevice *d) { (void)d; return NULL; }

bool BlockStorageDevice_GetMemoryMappedAddress(
    BlockStorageDevice *d, unsigned int ri, unsigned int rgi, ByteAddress *a)
{ (void)d; (void)ri; (void)rgi; (void)a; return false; }

// ---------- Target-specific ----------

void BlockStorage_AddDevices() { /* no devices on POSIX host */ }

// ---------- BlockStorageStream ----------

bool BlockStorageStream_Initialize(BlockStorageStream *s, uint32_t u) { (void)s; (void)u; return false; }
bool BlockStorageStream_Read(BlockStorageStream *s, unsigned char **b, unsigned int n) { (void)s; (void)b; (void)n; return false; }
bool BlockStorageStream_Write(BlockStorageStream *s, unsigned char *b, unsigned int n) { (void)s; (void)b; (void)n; return false; }
bool BlockStorageStream_Erase(BlockStorageStream *s, unsigned int n) { (void)s; (void)n; return false; }
bool BlockStorageStream_Seek(BlockStorageStream *s, unsigned int o, SeekOrigin r) { (void)s; (void)o; (void)r; return false; }
bool BlockStorageStream_NextStream(BlockStorageStream *s) { (void)s; return false; }
bool BlockStorageStream_PrevStream(BlockStorageStream *s) { (void)s; return false; }
