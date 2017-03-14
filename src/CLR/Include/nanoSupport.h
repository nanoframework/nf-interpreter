//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _SUPPORT_NANOSUPPORT_H_
#define _SUPPORT_NANOSUPPORT_H_

typedef unsigned int       UINT32;


#if defined(__cplusplus) && !(defined(_WIN32) || defined(WIN32))
extern "C" {
#endif

UINT32 SUPPORT_ComputeCRC(const void* rgBlock, int nLength, UINT32 crc);

#if defined(__cplusplus) && !(defined(_WIN32) || defined(WIN32))
}
#endif

//typedef BOOL (*WRITE_MEMORY_FUNCT)( UINT32 Address, UINT32 NumBytes, const BYTE * pSectorBuff );
//typedef BOOL (*READ_MEMORY_FUNCT) ( UINT32 Address, UINT32 NumBytes, BYTE * pSectorBuff );
//
//int LZ77_Decompress( UINT8* inBuf, int inSize, UINT8* outBuf, int outSize );
//int LZ77_Decompress( UINT8* inBuf, int inSize, UINT8* outBuf, int outSize, WRITE_MEMORY_FUNCT writeMem, READ_MEMORY_FUNCT readMem );

//--//

#if defined(_WIN32) || defined(WIN32)

bool LZ77_Compress(LPCWSTR inFileText, LPCWSTR outFileText, UINT8* prefix, size_t prefixLength);

#endif

#endif // _SUPPORT_NANOSUPPORT_H_

