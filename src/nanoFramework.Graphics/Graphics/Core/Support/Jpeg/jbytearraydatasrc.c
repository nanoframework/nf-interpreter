//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

// a byte array data source for the JPEG decoder implemented according to the structure of 
// jpeg_source_mgr


#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"

typedef struct {
    struct jpeg_source_mgr pub;	// public fields
    JOCTET buffer[2];		// EOI buffer (so we don't have to call allocate() )
} my_source_mgr;

typedef my_source_mgr* my_src_ptr;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
METHODDEF(void)
init_source(j_decompress_ptr cinfo)
{
    // do nothing
}
#pragma GCC diagnostic pop

METHODDEF(boolean)
fill_input_buffer(j_decompress_ptr cinfo)
{
    // since the initial buffer contains all the data that we have, 
    // if this method is called, it means we're out of data already
    // so we make up fake data to keep the decoder alive

    my_src_ptr src = (my_src_ptr)cinfo->src;

    WARNMS(cinfo, JWRN_JPEG_EOF);

    // Insert a fake EOI marker
    src->buffer[0] = (JOCTET)0xFF;
    src->buffer[1] = (JOCTET)JPEG_EOI;

    src->pub.next_input_byte = src->buffer;
    src->pub.bytes_in_buffer = 2;

    return TRUE;
}

// Keeping the library's original implementation 
METHODDEF(void)
skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
    my_src_ptr src = (my_src_ptr)cinfo->src;

    /* Just a dumb implementation for now.  Could use fseek() except
    * it doesn't work on pipes.  Not clear that being smart is worth
    * any trouble anyway --- large skips are infrequent.
    */
    if (num_bytes > 0) {
        while (num_bytes > (long)src->pub.bytes_in_buffer) {
            num_bytes -= (long)src->pub.bytes_in_buffer;
            (void)fill_input_buffer(cinfo);
            /* note we assume that fill_input_buffer will never return FALSE,
            * so suspension need not be handled.
            */
        }
        src->pub.next_input_byte += (CLR_INT32)num_bytes;
        src->pub.bytes_in_buffer -= (CLR_INT32)num_bytes;
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
METHODDEF(void)
term_source(j_decompress_ptr cinfo)
{
    // do nothing
}
#pragma GCC diagnostic pop

GLOBAL(void)
jpeg_byte_array_src(j_decompress_ptr cinfo, CLR_UINT8* source, CLR_INT32 size)
{
    my_src_ptr src;

    if (cinfo->src == NULL) {	// first time for this JPEG object?
        cinfo->src = (struct jpeg_source_mgr*)
            (*cinfo->mem->alloc_small) ((j_common_ptr)cinfo, JPOOL_PERMANENT,
                SIZEOF(my_source_mgr));
    }

    src = (my_src_ptr)cinfo->src;
    src->pub.init_source = init_source;
    src->pub.fill_input_buffer = fill_input_buffer;
    src->pub.skip_input_data = skip_input_data;
    src->pub.resync_to_restart = jpeg_resync_to_restart; // use default method
    src->pub.term_source = term_source;
    src->pub.bytes_in_buffer = size;
    src->pub.next_input_byte = source; // We make the entire byte array the "buffer"
}
