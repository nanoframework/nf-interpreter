//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Graphics.h"
#include "giffile.h"
#include "Gif.h"
#include "lzwread.h"

// Initialization routine for GifDecoder struct. When it's finished,
// the header field would be loaded already.
HRESULT GifDecoder::GifInitDecompress(const CLR_UINT8 *src, CLR_UINT32 srcSize)
{
    NANOCLR_HEADER();

    colorTableSize = 0;

    isTransparentColorUnique = false;

    // set up the CLR_RT_ByteArrayReader
    NANOCLR_CHECK_HRESULT(source.Init((CLR_UINT8 *)src, srcSize));

    NANOCLR_CHECK_HRESULT(source.Read(&header, sizeof(GifFileHeader)));

    if (memcmp(header.signature, "GIF87a", 6) && memcmp(header.signature, "GIF89a", 6))
    {
        // Unrecognized signature
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    // If the file contains a global color table, we'll read it.
    if (header.globalcolortableflag)
    {
        colorTableSize = 1 << ((header.globalcolortablesize) + 1);

        NANOCLR_CHECK_HRESULT(ReadColorTable());
    }

    NANOCLR_NOCLEANUP();
}

// Takes in the output buffer, and do the decompression.
// Note that the output buffer _must_ be at least of size widthInBytes*height
// Also, GifInitDecompress() _must_ be called BEFORE GifStartDecompress()
HRESULT GifDecoder::GifStartDecompress(CLR_GFX_Bitmap *bitmap)
{
    NANOCLR_HEADER();

    if (!source.IsValid())
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    if (bitmap == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (bitmap->m_bm.m_width != header.LogicScreenWidth || bitmap->m_bm.m_height != header.LogicScreenHeight)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    output = bitmap;

    while (true)
    {
        CLR_UINT8 chunkType = 0;

        // Read in the chunk type.
        NANOCLR_CHECK_HRESULT(source.Read1Byte(&chunkType));

        switch (chunkType)
        {
            case 0x2C: // Image Chunk
                NANOCLR_SET_AND_LEAVE(
                    ProcessImageChunk()); // We're done after reading in the first frame (no multi-frame support)
                break;

            case 0x3B: // Terminator Chunk
                NANOCLR_SET_AND_LEAVE(S_OK);
                break;

            case 0x21: // Extension
                // Read in the extension chunk type.
                NANOCLR_CHECK_HRESULT(source.Read1Byte(&chunkType));

                switch (chunkType)
                {
                    case 0xF9:
                        NANOCLR_CHECK_HRESULT(ProcessGraphicControlChunk());
                        break;

                    case 0xFE: // Comment Chunk
                    case 0x01: // Plain Text Chunk
                    case 0xFF: // APplication Extension Chunk
                        NANOCLR_CHECK_HRESULT(
                            ProcessUnwantedChunk()); // We don't care about any of these chunks, so just jump past it
                        break;

                    default:
                        // Unknown chunk type
                        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
                }
                break;
            default:
                // Unknown chunk type
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
    }

    NANOCLR_NOCLEANUP();
}

struct ProcessImageChunkHelperParam
{
    LZWDecompressor *lzwDec;
    bool flushing;
    int usedDecBufferSize;
    int currentDecBufferIndex;

    GifDecoder *decoder;

    bool needInit;
    bool done;
};

HRESULT GifDecoder::ProcessImageChunk()
{
    NANOCLR_HEADER();

    LZWDecompressor *lzwDec = NULL;
    GifImageDescriptor currentImageDescriptor;

    NANOCLR_CHECK_HRESULT(source.Read(&currentImageDescriptor, sizeof(GifImageDescriptor)));

    // If the image descriptor's width and height is different from the ones specified in the header,
    // we'll bail. Also, we ignore the imagedescriptor's top and left, and assume it's 0, 0 always.
    if (currentImageDescriptor.width != header.LogicScreenWidth ||
        currentImageDescriptor.height != header.LogicScreenHeight)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }

    // We don't support interlaced GIFs
    if (currentImageDescriptor.interlaceflag)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }

    if (currentImageDescriptor.localcolortableflag)
    {
        // If there's a local color table, then overwrite the global color table (if there was one)
        // Note that we're taking a shortcut here to simply overwrite the global table because
        // we don't support multi-frames/animated Gif.
        colorTableSize = 1 << ((currentImageDescriptor.localcolortablesize) + 1);

        NANOCLR_CHECK_HRESULT(ReadColorTable());
    }

    if (colorTableSize == 0)
    {
        // We don't have a valid color table (global or local)
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    // Read the codesize from the file.  The GIF decoder needs this to begin.
    CLR_UINT8 lzwCodeSize;

    NANOCLR_CHECK_HRESULT(source.Read1Byte(&lzwCodeSize));

    if ((lzwCodeSize < 2) || (lzwCodeSize > 8))
    {
        // Minimum code size should be within the range of 2 - 8
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    // Create a LZW decompressor object based on lzw code size
    // We Allocate the LZWDecompressor on the heap, rather than the stack, since it contains a 16kb token table.
    lzwDec = (LZWDecompressor *)CLR_RT_Memory::Allocate(sizeof(LZWDecompressor));
    CHECK_ALLOCATION(lzwDec);

    lzwDec->LZWDecompressorInit(lzwCodeSize);

    GFX_Rect rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = currentImageDescriptor.width - 1;
    rect.bottom = currentImageDescriptor.height - 1;

    ProcessImageChunkHelperParam param;

    param.lzwDec = lzwDec;
    param.decoder = this;
    param.needInit = true;
    param.flushing = false;
    param.done = false;
    param.currentDecBufferIndex = 0;

    output->SetPixelsHelper(
        rect,
        PAL_GFX_Bitmap::c_SetPixelsConfig_NoClip | PAL_GFX_Bitmap::c_SetPixelsConfig_NoClipChecks,
        (GFX_SetPixelsCallback)&ProcessImageChunkHelper,
        &param);

    NANOCLR_CLEANUP();

    // If we successfully allocated the decompressor, we need to release it.
    if (lzwDec)
        CLR_RT_Memory::Release(lzwDec);

    NANOCLR_CLEANUP_END();
}

void GifDecoder::SetupFlushing(void *p)
{
    ProcessImageChunkHelperParam *param = (ProcessImageChunkHelperParam *)p;
    param->flushing = true;
    param->usedDecBufferSize = param->decoder->decBufferSize - param->lzwDec->m_cbOut;
    param->currentDecBufferIndex = 0;
}

// Returns true if there are no more inputs to decode (i.e. we're done), false if otherwise
// Note that it'll return true if we ran into some error condition as well.
bool GifDecoder::DecodeUntilFlush(void *p)
{
    ProcessImageChunkHelperParam *param = (ProcessImageChunkHelperParam *)p;

    ASSERT(param->flushing == false);

    while (true)
    {
        if (param->lzwDec->m_fNeedOutput == true)
        {
            if (param->needInit == false)
                SetupFlushing(param);

            param->lzwDec->m_pbOut = param->decoder->decBuffer;
            param->lzwDec->m_cbOut = param->decoder->decBufferSize;
            param->lzwDec->m_fNeedOutput = false;

            if (param->needInit == false)
                return false;
        }

        if (param->lzwDec->m_fNeedInput == true)
        {
            // The decompressor needs more input, we'll read in the next
            // data block, and populate the fields accordingly.
            CLR_UINT8 blockSize;
            HRESULT hr;

            if (FAILED(hr = param->decoder->source.Read1Byte(&blockSize)))
                return true;

            if (blockSize > 0) // we have a valid block
            {
                const CLR_UINT8 *sourceCurPos = param->decoder->source.source;

                // Advance the marker for the stream
                if (FAILED(hr = param->decoder->source.Skip(blockSize)))
                    return true;

                param->lzwDec->m_pbIn = sourceCurPos;
                param->lzwDec->m_cbIn = blockSize;
                param->lzwDec->m_fNeedInput = false;
            }
            else
            {
                // We've reached the end of the data subblocks, flush out the already decompressed data and leave
                if (param->needInit == false)
                {
                    SetupFlushing(param);
                }
                return true;
            }
        }

        param->needInit = false;

        if (param->lzwDec->FProcess() == false)
        {
            SetupFlushing(param);
            return true;
        }
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
CLR_UINT32 GifDecoder::ProcessImageChunkHelper(int x, int y, CLR_UINT32 flags, CLR_UINT16 &opacity, void *param)
{
    ProcessImageChunkHelperParam *myParam = (ProcessImageChunkHelperParam *)param;

    // If we're not already in the middle of a flushing, try to decode more data
    if (myParam->flushing == false)
    {
        // However, if we've already decoded all that we can, just leave with invalid color
        if (myParam->done)
        {
            return PAL_GFX_Bitmap::c_InvalidColor;
        }

        if (DecodeUntilFlush(myParam) == true)
        {
            myParam->done = true;

            // Check again in case DecodeUntilFlushing is return true because of an error
            if (myParam->flushing == false)
            {
                return PAL_GFX_Bitmap::c_InvalidColor;
            }
        }
    }

    if (flags & PAL_GFX_Bitmap::c_SetPixels_NewRow)
    {
    }

    opacity = PAL_GFX_Bitmap::c_OpacityOpaque;
    CLR_UINT32 color = myParam->decoder->colorTable[myParam->decoder->decBuffer[myParam->currentDecBufferIndex]];

    myParam->currentDecBufferIndex++;

    if (myParam->currentDecBufferIndex == myParam->usedDecBufferSize)
    {
        myParam->flushing = false;
    }

    return color;
}

#pragma GCC diagnostic pop

HRESULT GifDecoder::ProcessGraphicControlChunk()
{
    NANOCLR_HEADER();

    NANOCLR_CHECK_HRESULT(source.Read(&gce, sizeof(GifGraphicControlExtension)));

    if (gce.transparentcolorflag)
    {
        int count = colorTableSize;

        while (!isTransparentColorUnique)
        {
            transparentColor += c_bigPrimeNumber;
            transparentColor &= 0x00FFFFFF; // make sure it's still a valid color
            isTransparentColorUnique = true;
            for (int i = 0; i < colorTableSize; i++)
            {
                if (colorTable[i] == transparentColor)
                {
                    isTransparentColorUnique = false;
                    break;
                }
            }
            count--;
            if (count <= 0)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            }
        }

        colorTable[gce.transparentcolorindex] = transparentColor;

        output->m_palBitmap.transparentColor = transparentColor;
    }

    NANOCLR_CHECK_HRESULT(source.Skip(1)); // skip the block terminator

    NANOCLR_NOCLEANUP();
}

HRESULT GifDecoder::ProcessUnwantedChunk()
{
    NANOCLR_HEADER();

    CLR_UINT8 blockSize = 0;

    do // if we went pass the end of source, Read1Byte() or Skip() will complain and exit
    {
        NANOCLR_CHECK_HRESULT(source.Read1Byte(&blockSize));
        NANOCLR_CHECK_HRESULT(source.Skip(blockSize));
    } while (blockSize > 0); // Keep reading data subblocks until we reach a terminator block (0x00)

    NANOCLR_NOCLEANUP();
}

// These functions Read in the 24bit RGB values from the source and
// convert/store it as the native color format in the colorTable[]
HRESULT GifDecoder::ReadColorTable()
{
    NANOCLR_HEADER();

    // Record the first entry in the source stream
    GifPaletteEntry *curEntry = (GifPaletteEntry *)source.source;

    // Advance the marker for source first
    // (note that colorTableSize is filled in for us before the call of this function)
    NANOCLR_CHECK_HRESULT(source.Skip(colorTableSize * sizeof(GifPaletteEntry)));

    // Since we're looping through all the colorTable entries already, we'll use
    // this chance to find a unique transparent color, if possible
    isTransparentColorUnique = true; // Assume it's true until found otherwise;
    transparentColor = c_defaultTransparentColor;

    // We can now walk through the entries and convert them as we go

    for (int i = 0; i < colorTableSize; i++)
    {
        colorTable[i] =
            ((CLR_UINT32)curEntry->red) | (((CLR_UINT32)curEntry->green) << 8) | (((CLR_UINT32)curEntry->blue) << 16);
        if (colorTable[i] == transparentColor)
        {
            isTransparentColorUnique = false;
        }
        curEntry++;
    }

    NANOCLR_NOCLEANUP();
}
