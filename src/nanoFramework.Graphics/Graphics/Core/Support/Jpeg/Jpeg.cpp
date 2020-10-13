//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nanoCLR_Types.h"
#include "nanoCLR_Runtime.h"
#include "Graphics.h"

#include <setjmp.h>

extern "C"
{
#include "jpeglib.h"
#include "jerror.h"
}

////////////////////////////////////////////////////////////////////////////////////////////////////


// JPEG Error handling struct / routine
struct JPEGErrorManager
{
    jpeg_error_mgr pub;
    int         setjmpBuffer[32];
};




void JPEGErrorHandler(j_common_ptr cinfo)
{
    JPEGErrorManager* errorManager = (JPEGErrorManager*)cinfo->err;
    longjmp(errorManager->setjmpBuffer, 1);
}

struct CreateInstanceJpegHelperParam
{
    CLR_UINT8* curBuffer;
    int        index;
    JSAMPARRAY buffer;
    jpeg_decompress_struct* cinfo;
};

HRESULT CLR_GFX_Bitmap::CreateInstanceJpeg(CLR_RT_HeapBlock& ref, const CLR_UINT8* data, const CLR_UINT32 size)
{
    NANOCLR_HEADER();

    jpeg_decompress_struct        cinfo;
    CLR_GFX_Bitmap* bitmap = NULL;
    CreateInstanceJpegHelperParam param;
    GFX_Rect                      rect;

    param.curBuffer = NULL;

    // Set up the error handling
    JPEGErrorManager errorManager;
    cinfo.err = jpeg_std_error(&errorManager.pub);
    errorManager.pub.error_exit = JPEGErrorHandler;

    if (setjmp(errorManager.setjmpBuffer))
    {
        if (cinfo.err->msg_code == JERR_OUT_OF_MEMORY)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
    }

    // Create the decompression engine
    jpeg_create_decompress(&cinfo);
    jpeg_byte_array_src(&cinfo, (CLR_UINT8*)data, size);
    jpeg_read_header(&cinfo, TRUE);

    // Set output to 16bit 5-6-5 RGB format
    // We can add support for other bit-depth in the future
    cinfo.out_color_space = JCS_RGB;
    cinfo.do_fancy_upsampling = FALSE;

    jpeg_start_decompress(&cinfo);

    // At this point cinfo would have all the correct output dimension info
    CLR_GFX_BitmapDescription bm;
    if (bm.BitmapDescription_Initialize(cinfo.output_width, cinfo.output_height, CLR_GFX_BitmapDescription::c_NativeBpp) == false)
    {
        // if the resulting bitmap doesn't match our constraints, stop the decompression
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // Allocate the memory that the decompressed bitmap would need
    NANOCLR_CHECK_HRESULT(CLR_GFX_Bitmap::CreateInstance(ref, bm));

    NANOCLR_CHECK_HRESULT(CLR_GFX_Bitmap::GetInstanceFromGraphicsHeapBlock(ref, bitmap));

    // Do the actual decompression
    rect.left = 0;
    rect.top = 0;
    rect.right = cinfo.output_width - 1;
    rect.bottom = cinfo.output_height - 1;

    param.curBuffer = (CLR_UINT8*)CLR_RT_Memory::Allocate(cinfo.output_width * 3);  CHECK_ALLOCATION(param.curBuffer);
    param.buffer = (JSAMPARRAY) & (param.curBuffer);
    param.cinfo = &cinfo;

    bitmap->SetPixelsHelper(rect, PAL_GFX_Bitmap::c_SetPixelsConfig_NoClip | PAL_GFX_Bitmap::c_SetPixelsConfig_NoClipChecks, &CreateInstanceJpegHelper, &param);

    NANOCLR_CLEANUP();

    if (param.curBuffer) CLR_RT_Memory::Release(param.curBuffer);
    jpeg_destroy_decompress(&cinfo);

    NANOCLR_CLEANUP_END();
}
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
CLR_UINT32 CLR_GFX_Bitmap::CreateInstanceJpegHelper(int x, int y, CLR_UINT32 flags, CLR_UINT16& opacity, void* param)
{
    CreateInstanceJpegHelperParam* myParam = (CreateInstanceJpegHelperParam*)param;

    if (flags & PAL_GFX_Bitmap::c_SetPixels_NewRow)
    {
        jpeg_read_scanlines(myParam->cinfo, myParam->buffer, 1);

        myParam->index = 0;
    }

    opacity = PAL_GFX_Bitmap::c_OpacityOpaque;

    CLR_UINT32 r = myParam->curBuffer[myParam->index];
    CLR_UINT32 g = myParam->curBuffer[myParam->index + 1];
    CLR_UINT32 b = myParam->curBuffer[myParam->index + 2];

    myParam->index += 3;

    return r | (g << 8) | (b << 16);
}
#pragma GCC diagnostic pop
