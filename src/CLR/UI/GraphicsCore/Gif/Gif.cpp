////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Graphics.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CLR_GFX_Bitmap::CreateInstanceGif( CLR_RT_HeapBlock& ref, const CLR_UINT8* data, const CLR_UINT32 size )
{
    NATIVE_PROFILE_CLR_GRAPHICS();
    NANOCLR_HEADER();

    GifDecoder*               decoder;
    CLR_GFX_Bitmap*           bitmap   = NULL;
    CLR_GFX_BitmapDescription bm;

    // Allocate the decoder
    decoder = (GifDecoder*)CLR_RT_Memory::Allocate( sizeof(GifDecoder) );  CHECK_ALLOCATION(decoder);

    // Initialize the decompression engine
    NANOCLR_CHECK_HRESULT( decoder->GifInitDecompress( data, size ) );

    // At this point decoder would have all the correct output dimension info, so we initialized the CLR_GFX_BitmapDescription
    // with that information, note that we currently only support decompression into the native bpp
    if(bm.BitmapDescription_Initialize( decoder->header.LogicScreenWidth, decoder->header.LogicScreenHeight, CLR_GFX_BitmapDescription::c_NativeBpp ) == false)
    {
        // if the resulting bitmap doesn't match our constraints, stop the decompression
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // Allocate the memory that the decompressed bitmap would need
    NANOCLR_CHECK_HRESULT(CLR_GFX_Bitmap::CreateInstance( ref, bm ));

    NANOCLR_CHECK_HRESULT(CLR_GFX_Bitmap::GetInstanceFromHeapBlock (ref, bitmap));

    NANOCLR_CHECK_HRESULT(decoder->GifStartDecompress( bitmap ));

    NANOCLR_CLEANUP();

    if (decoder) CLR_RT_Memory::Release( decoder );

    NANOCLR_CLEANUP_END();
}
