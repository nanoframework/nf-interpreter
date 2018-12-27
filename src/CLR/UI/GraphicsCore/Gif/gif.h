////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _GIF_H_
#define _GIF_H_

#include "nanoCLR_Types.h"
#include "nanoCLR_Runtime.h"
#include "giffile.h"

struct GifDecoder
{
    GifFileHeader header;
    
    CLR_UINT32 transparentColor;

    HRESULT GifInitDecompress ( const CLR_UINT8* src, CLR_UINT32 srcSize );
    HRESULT GifStartDecompress( CLR_GFX_Bitmap* bitmap );

private:

    int    colorTableSize;
    CLR_UINT32 colorTable[256];
    
    bool   isTransparentColorUnique;

    static const CLR_UINT32 c_defaultTransparentColor = 0x00561234;
    static const CLR_UINT32 c_bigPrimeNumber          = 22073;

    GifGraphicControlExtension gce;

    CLR_RT_ByteArrayReader source;

    CLR_GFX_Bitmap* output;

    const static int decBufferSize = 1024;
    CLR_UINT8            decBuffer[decBufferSize];

    HRESULT ProcessImageChunk         ();
    HRESULT ProcessGraphicControlChunk();
    HRESULT ProcessUnwantedChunk      ();

    HRESULT ReadColorTable     ();

    static CLR_UINT32 ProcessImageChunkHelper( int x, int y, CLR_UINT32 flags, CLR_UINT16& opacity, void* param );
    static void SetupFlushing( void* param );
    static bool DecodeUntilFlush( void* param );
};

#endif
