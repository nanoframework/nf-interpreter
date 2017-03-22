//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _SUPPORT_NANOSUPPORT_H_
#define _SUPPORT_NANOSUPPORT_H_

#if defined(__cplusplus) && !(defined(_WIN32) || defined(WIN32))
extern "C" {
#endif

unsigned int SUPPORT_ComputeCRC(const void* rgBlock, int nLength, unsigned int crc);

#if defined(__cplusplus) && !(defined(_WIN32) || defined(WIN32))
}
#endif

//typedef bool (*WRITE_MEMORY_FUNCT)( unsigned int Address, unsigned int NumBytes, const unsigned char * pSectorBuff );
//typedef bool (*READ_MEMORY_FUNCT) ( unsigned int Address, unsigned int NumBytes, unsigned char * pSectorBuff );
//
//int LZ77_Decompress( unsigned char* inBuf, int inSize, unsigned char* outBuf, int outSize );
//int LZ77_Decompress( unsigned char* inBuf, int inSize, unsigned char* outBuf, int outSize, WRITE_MEMORY_FUNCT writeMem, READ_MEMORY_FUNCT readMem );

//--//

#if defined(_WIN32) || defined(WIN32)

bool LZ77_Compress(const wchar_t* inFileText, const wchar_t* outFileText, unsigned char* prefix, size_t prefixLength);

#endif

#endif // _SUPPORT_NANOSUPPORT_H_

