/*
 * jdcolor.c
 *
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains output colorspace conversion routines.
 */

#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpeglib.h"


/* Private subobject */

typedef struct {
  struct jpeg_color_deconverter pub; /* public fields */

  /* Private state for YCC->RGB conversion */
  int * Cr_r_tab;		/* => table for Cr to R conversion */
  int * Cb_b_tab;		/* => table for Cb to B conversion */
  INT32 * Cr_g_tab;		/* => table for Cr to G conversion */
  INT32 * Cb_g_tab;		/* => table for Cb to G conversion */
  /* Private state for CMYK->RGB conversion */
  unsigned long *f;    // Lookup table for K conversion
  unsigned long *gC2R; // Lookup tables for opposite and adjacent components
  unsigned long *gC2G;
  unsigned long *gC2B;
  unsigned long *gM2R;
  unsigned long *gM2G;
  unsigned long *gM2B;
  unsigned long *gY2R;
  unsigned long *gY2G;
  unsigned long *gY2B;
} my_color_deconverter;

typedef my_color_deconverter * my_cconvert_ptr;


/**************** YCbCr -> RGB conversion: most common case **************/

/*
 * YCbCr is defined per CCIR 601-1, except that Cb and Cr are
 * normalized to the range 0..MAXJSAMPLE rather than -0.5 .. 0.5.
 * The conversion equations to be implemented are therefore
 *	R = Y                + 1.40200 * Cr
 *	G = Y - 0.34414 * Cb - 0.71414 * Cr
 *	B = Y + 1.77200 * Cb
 * where Cb and Cr represent the incoming values less CENTERJSAMPLE.
 * (These numbers are derived from TIFF 6.0 section 21, dated 3-June-92.)
 *
 * To avoid floating-point arithmetic, we represent the fractional constants
 * as integers scaled up by 2^16 (about 4 digits precision); we have to divide
 * the products by 2^16, with appropriate rounding, to get the correct answer.
 * Notice that Y, being an integral input, does not contribute any fraction
 * so it need not participate in the rounding.
 *
 * For even more speed, we avoid doing any multiplications in the inner loop
 * by precalculating the constants times Cb and Cr for all possible values.
 * For 8-bit JSAMPLEs this is very reasonable (only 256 entries per table);
 * for 12-bit samples it is still acceptable.  It's not very reasonable for
 * 16-bit samples, but if you want lossless storage you shouldn't be changing
 * colorspace anyway.
 * The Cr=>R and Cb=>B values can be rounded to integers in advance; the
 * values for the G calculation are left scaled up, since we must add them
 * together before rounding.
 */

#define SCALEBITS	16	/* speediest right-shift on some machines */
#define ONE_HALF	((INT32) 1 << (SCALEBITS-1))
#define FIX(x)		((INT32) ((x) * (1L<<SCALEBITS) + 0.5))


/*
 * Initialize tables for YCC->RGB colorspace conversion.
 */

LOCAL(void)
build_ycc_rgb_table (j_decompress_ptr cinfo)
{
  my_cconvert_ptr cconvert = (my_cconvert_ptr) cinfo->cconvert;
  int i;
  INT32 x;
  SHIFT_TEMPS

  cconvert->Cr_r_tab = (int *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				(MAXJSAMPLE+1) * SIZEOF(int));
  cconvert->Cb_b_tab = (int *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				(MAXJSAMPLE+1) * SIZEOF(int));
  cconvert->Cr_g_tab = (INT32 *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				(MAXJSAMPLE+1) * SIZEOF(INT32));
  cconvert->Cb_g_tab = (INT32 *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				(MAXJSAMPLE+1) * SIZEOF(INT32));

  for (i = 0, x = -CENTERJSAMPLE; i <= MAXJSAMPLE; i++, x++) {
    /* i is the actual input pixel value, in the range 0..MAXJSAMPLE */
    /* The Cb or Cr value we are thinking of is x = i - CENTERJSAMPLE */
    /* Cr=>R value is nearest int to 1.40200 * x */
    cconvert->Cr_r_tab[i] = (int)
		    RIGHT_SHIFT(FIX(1.40200) * x + ONE_HALF, SCALEBITS);
    /* Cb=>B value is nearest int to 1.77200 * x */
    cconvert->Cb_b_tab[i] = (int)
		    RIGHT_SHIFT(FIX(1.77200) * x + ONE_HALF, SCALEBITS);
    /* Cr=>G value is scaled-up -0.71414 * x */
    cconvert->Cr_g_tab[i] = (- FIX(0.71414)) * x;
    /* Cb=>G value is scaled-up -0.34414 * x */
    /* We also add in ONE_HALF so that need not do it in inner loop */
    cconvert->Cb_g_tab[i] = (- FIX(0.34414)) * x + ONE_HALF;
  }
}


/*
 * Convert some rows of samples to the output colorspace.
 *
 * Note that we change from noninterleaved, one-plane-per-component format
 * to interleaved-pixel format.  The output buffer is therefore three times
 * as wide as the input buffer.
 * A starting row offset is provided only for the input buffer.  The caller
 * can easily adjust the passed output_buf value to accommodate any row
 * offset required on that side.
 */

METHODDEF(void)
ycc_rgb_convert (j_decompress_ptr cinfo,
		 JSAMPIMAGE input_buf, JDIMENSION input_row,
		 JSAMPARRAY output_buf, int num_rows)
{
  my_cconvert_ptr cconvert = (my_cconvert_ptr) cinfo->cconvert;
  register int y, cb, cr;
  register JSAMPROW outptr;
  register JSAMPROW inptr0, inptr1, inptr2;
  register JDIMENSION col;
  JDIMENSION num_cols = cinfo->output_width;
  /* copy these pointers into registers if possible */
  register JSAMPLE * range_limit = cinfo->sample_range_limit;
  register int * Crrtab = cconvert->Cr_r_tab;
  register int * Cbbtab = cconvert->Cb_b_tab;
  register INT32 * Crgtab = cconvert->Cr_g_tab;
  register INT32 * Cbgtab = cconvert->Cb_g_tab;
  SHIFT_TEMPS

  while (--num_rows >= 0) {
    inptr0 = input_buf[0][input_row];
    inptr1 = input_buf[1][input_row];
    inptr2 = input_buf[2][input_row];
    input_row++;
    outptr = *output_buf++;
    for (col = 0; col < num_cols; col++) {
      y  = GETJSAMPLE(inptr0[col]);
      cb = GETJSAMPLE(inptr1[col]);
      cr = GETJSAMPLE(inptr2[col]);
      /* Range-limiting is essential due to noise introduced by DCT losses. */
      outptr[RGB_RED] =   range_limit[y + Crrtab[cr]];
      outptr[RGB_GREEN] = range_limit[y +
			      ((int) RIGHT_SHIFT(Cbgtab[cb] + Crgtab[cr],
						 SCALEBITS))];
      outptr[RGB_BLUE] =  range_limit[y + Cbbtab[cb]];
      outptr += RGB_PIXELSIZE;
    }	
  }
}


#ifdef JPEG_MMX_SUPPORTED
#include "mcbcr.h"

METHODDEF(void)
ycc_rgb_convert_mmx (j_decompress_ptr cinfo,
		 JSAMPIMAGE input_buf, JDIMENSION input_row,
		 JSAMPARRAY output_buf, int num_rows)
{
  my_cconvert_ptr cconvert = (my_cconvert_ptr) cinfo->cconvert;
  register int y, cb, cr;
  register JSAMPROW outptr;
  register JSAMPROW inptr0, inptr1, inptr2;
  register JDIMENSION col;
  JDIMENSION num_cols = cinfo->output_width;
  /* copy these pointers into registers if possible */
  register JSAMPLE * range_limit = cinfo->sample_range_limit;
  register int * Crrtab = cconvert->Cr_r_tab;
  register int * Cbbtab = cconvert->Cb_b_tab;
  register INT32 * Crgtab = cconvert->Cr_g_tab;
  register INT32 * Cbgtab = cconvert->Cb_g_tab;
  SHIFT_TEMPS

  // Alignment variables - CRK
  JDIMENSION mmx_cols=num_cols&~7;

  while (--num_rows >= 0) {
    inptr0 = input_buf[0][input_row];
    inptr1 = input_buf[1][input_row];
    inptr2 = input_buf[2][input_row];
    input_row++;
    outptr = *output_buf++;
    if (mmx_cols > 0)
      MYCbCr2RGB(mmx_cols, inptr0, inptr1, inptr2, outptr);
    outptr += 3*mmx_cols;
    for (col = mmx_cols; col < num_cols; col++) {
      y  = GETJSAMPLE(inptr0[col]);
      cb = GETJSAMPLE(inptr1[col]);
      cr = GETJSAMPLE(inptr2[col]);
      /* Range-limiting is essential due to noise introduced by DCT losses. */
      outptr[RGB_RED] =   range_limit[y + Crrtab[cr]];
      outptr[RGB_GREEN] = range_limit[y +
			      ((int) RIGHT_SHIFT(Cbgtab[cb] + Crgtab[cr],
						 SCALEBITS))];
      outptr[RGB_BLUE] =  range_limit[y + Cbbtab[cb]];
      outptr += RGB_PIXELSIZE;
    }
  }
  __asm emms	
}
#endif


/**************** Cases other than YCbCr -> RGB **************/


/*
 * Color conversion for no colorspace change: just copy the data,
 * converting from separate-planes to interleaved representation.
 */

METHODDEF(void)
null_convert (j_decompress_ptr cinfo,
	      JSAMPIMAGE input_buf, JDIMENSION input_row,
	      JSAMPARRAY output_buf, int num_rows)
{
  register JSAMPROW inptr, outptr;
  register JDIMENSION count;
  register int num_components = cinfo->num_components;
  JDIMENSION num_cols = cinfo->output_width;
  int ci;

  while (--num_rows >= 0) {
    for (ci = 0; ci < num_components; ci++) {
      inptr = input_buf[ci][input_row];
      outptr = output_buf[0] + ci;
      for (count = num_cols; count > 0; count--) {
	*outptr = *inptr++;	/* needn't bother with GETJSAMPLE() here */
	outptr += num_components;
      }
    }
    input_row++;
    output_buf++;
  }
}


/*
 * Color conversion for grayscale: just copy the data.
 * This also works for YCbCr -> grayscale conversion, in which
 * we just copy the Y (luminance) component and ignore chrominance.
 */

METHODDEF(void)
grayscale_convert (j_decompress_ptr cinfo,
		   JSAMPIMAGE input_buf, JDIMENSION input_row,
		   JSAMPARRAY output_buf, int num_rows)
{
  jcopy_sample_rows(input_buf[0], (int) input_row, output_buf, 0,
		    num_rows, cinfo->output_width);
}

/*
 * Convert grayscale to RGB: just duplicate the graylevel three times.
 * This is provided to support applications that don't want to cope
 * with grayscale as a separate case.
 */

METHODDEF(void)
gray_rgb_convert (j_decompress_ptr cinfo,
          JSAMPIMAGE input_buf, JDIMENSION input_row,
          JSAMPARRAY output_buf, int num_rows)
{
  register JSAMPROW inptr, outptr;
  register JDIMENSION col;
  JDIMENSION num_cols = cinfo->output_width;

  while (--num_rows >= 0) {
    inptr = input_buf[0][input_row++];
    outptr = *output_buf++;
    for (col = 0; col < num_cols; col++) {
      /* We can dispense with GETJSAMPLE() here */
      outptr[RGB_RED] = outptr[RGB_GREEN] = outptr[RGB_BLUE] = inptr[col];
      outptr += RGB_PIXELSIZE;
    }
  }
}

/*
 * Adobe-style YCCK->CMYK conversion.
 * We convert YCbCr to R=1-C, G=1-M, and B=1-Y using the same
 * conversion as above, while passing K (black) unchanged.
 * We assume build_ycc_rgb_table has been called.
 */

METHODDEF(void)
ycck_cmyk_convert (j_decompress_ptr cinfo,
		   JSAMPIMAGE input_buf, JDIMENSION input_row,
		   JSAMPARRAY output_buf, int num_rows)
{
  my_cconvert_ptr cconvert = (my_cconvert_ptr) cinfo->cconvert;
  register int y, cb, cr;
  register JSAMPROW outptr;
  register JSAMPROW inptr0, inptr1, inptr2, inptr3;
  register JDIMENSION col;
  JDIMENSION num_cols = cinfo->output_width;
  /* copy these pointers into registers if possible */
  register JSAMPLE * range_limit = cinfo->sample_range_limit;
  register int * Crrtab = cconvert->Cr_r_tab;
  register int * Cbbtab = cconvert->Cb_b_tab;
  register INT32 * Crgtab = cconvert->Cr_g_tab;
  register INT32 * Cbgtab = cconvert->Cb_g_tab;
  SHIFT_TEMPS

  while (--num_rows >= 0) {
    inptr0 = input_buf[0][input_row];
    inptr1 = input_buf[1][input_row];
    inptr2 = input_buf[2][input_row];
    inptr3 = input_buf[3][input_row];
    input_row++;
    outptr = *output_buf++;
    for (col = 0; col < num_cols; col++) {
      y  = GETJSAMPLE(inptr0[col]);
      cb = GETJSAMPLE(inptr1[col]);
      cr = GETJSAMPLE(inptr2[col]);
      /* Range-limiting is essential due to noise introduced by DCT losses. */
      outptr[0] = range_limit[MAXJSAMPLE - (y + Crrtab[cr])];	/* red */
      outptr[1] = range_limit[MAXJSAMPLE - (y +			/* green */
			      ((int) RIGHT_SHIFT(Cbgtab[cb] + Crgtab[cr],
						 SCALEBITS)))];
      outptr[2] = range_limit[MAXJSAMPLE - (y + Cbbtab[cb])];	/* blue */
      /* K passes through unchanged */
      outptr[3] = inptr3[col];	/* don't need GETJSAMPLE here */
      outptr += 4;
    }
  }
}


/*****************************************************************************
	CMYK SUPPORT [START]
	Provided by John Bronskill, integrated by John Bowler.  Please, if
	your name isn't John don't edit this.
******************************************************************* JohnBo **/
/*****************************************************************************
* CMYKToRGBA *
*------------*
*  @doc INTERNAL
*   @func void | CMYKToRGBA |
*    This function converts CMYK data to RGBA in a cheesy way but with adequate
*    quality. To really do this well you need to know ink and paper types,
*    the new math and all kinds of other stuff. Don't try it at home.
*   @x
*-----------------------------------------------------------------------------
* Created By: John Bronskill                                     Date: 12/9/96
*-----------------------------------------------------------------------------
* Parameters:
*   @parm unsigned char * | To |
*    Output. Pre-allocated buffer to hold converted RGBA data. 
*   @parm unsigned char * | From |
*    Input. Pre-allocated buffer containing the CMYK data to be converted.
*   @parm int | iWidth | 
*    Input. Width (or number of pixels) of image data to be converted. 
*   @x
*
* Returns:
*   @x
*****************************************************************************/
LOCAL(void)
build_cmyk_rgb_table (j_decompress_ptr cinfo)
{
  #if MAXJSAMPLE != 255
    #error CMYK code requires MAXJSAMPLE 255
  #endif

  my_cconvert_ptr cconvert = (my_cconvert_ptr) cinfo->cconvert;

  const long pC2R = 256;  // Parameters which define the color transformation from CMYK->RGB
  const long pC2G = 103;
  const long pC2B = 12;

  const long pM2R = 48;
  const long pM2G = 256;
  const long pM2B = 144;

  const long pY2R = 0;
  const long pY2G = 11;
  const long pY2B = 228;

  const long pK2RGB = 256;

  const long rC2R = 206;
  const long rC2G = 94;
  const long rC2B = 0;

  const long rM2R = 24;
  const long rM2G = 186;
  const long rM2B = 132;

  const long rY2R = 0;
  const long rY2G = 7;
  const long rY2B = 171;

  const long rK2RGB = 223;

  int i;

  cconvert->f = (unsigned long *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				(MAXJSAMPLE+1) * SIZEOF(unsigned long));
  cconvert->gC2R = (unsigned long *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				(MAXJSAMPLE+1) * SIZEOF(unsigned long));
  cconvert->gC2G = (unsigned long *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				(MAXJSAMPLE+1) * SIZEOF(unsigned long));
  cconvert->gC2B = (unsigned long *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				(MAXJSAMPLE+1) * SIZEOF(unsigned long));
  cconvert->gM2R = (unsigned long *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				(MAXJSAMPLE+1) * SIZEOF(unsigned long));
  cconvert->gM2G = (unsigned long *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				(MAXJSAMPLE+1) * SIZEOF(unsigned long));
  cconvert->gM2B = (unsigned long *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				(MAXJSAMPLE+1) * SIZEOF(unsigned long));
  cconvert->gY2R = (unsigned long *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				(MAXJSAMPLE+1) * SIZEOF(unsigned long));
  cconvert->gY2G = (unsigned long *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				(MAXJSAMPLE+1) * SIZEOF(unsigned long));
  cconvert->gY2B = (unsigned long *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				(MAXJSAMPLE+1) * SIZEOF(unsigned long));

  /* The divide macros round to nearest, the f array is pre-scaled by
     255, the other arrays have the range 0..65535. */
#define macroCMYK2RGB(p, r, i) \
   ((i < 192) ? (256*255 - (i)*(r) - 127) : \
   (256*255 - 192*(r) - (((i) - 192)*(255*(p) - 192*(r)) + 31)/63 ))
#define macroScale(x) \
   ((x) * 65793 >> 16)

  #ifdef DEBUG
    // Debug error checking: these invariants must be true, since
    // the table is not image dependent we actually only need to make
    // this check once (ever :-)  This invariant guarantees than macroScale
    // cannot overflow, macroScale, by examination, returns results in
    // the range 0..65535
//    #define VALIDX(x) if (cconvert->x[i] < 0 || cconvert->x[i] > 255*256)
    #define VALIDX(x) if ( cconvert->x[i] > 255*256)\
      WARNMS(cinfo, JERR_BAD_J_COLORSPACE);
  #else
    #define VALIDX(x)
  #endif

  #define SET(Q) \
    cconvert->g ## Q[i] = macroCMYK2RGB(p ## Q, r ## Q, i);\
    VALIDX(g ## Q)\
    cconvert->g ## Q[i] = macroScale(cconvert->g ## Q[i])

  // Initialize the lookup tables
  for(i = 0; i <= MAXJSAMPLE; i++) 
  {
    cconvert->f[i] = macroCMYK2RGB(pK2RGB, rK2RGB, i);
    VALIDX(f)
    // Macro result is in the range 0..255*256, scale to 0..65536,
    // In debug check for overflow.
    SET(C2R);
    SET(C2G);
    SET(C2B);
    SET(M2R);
    SET(M2G);
    SET(M2B);
    SET(Y2R);
    SET(Y2G);
    SET(Y2B);
  }
}


/*----------------------------------------------------------------------------
	Code which converts CMYK->RGB
------------------------------------------------------------------- JohnBo -*/
METHODDEF(void)
cmyk_rgb_convert (j_decompress_ptr cinfo,
		  JSAMPIMAGE input_buf, JDIMENSION input_row,
		  JSAMPARRAY output_buf, int num_rows)
{
  my_cconvert_ptr cconvert = (my_cconvert_ptr) cinfo->cconvert;

  const unsigned long *f = cconvert->f;    // Lookup table for K conversion
  // Lookup tables for opposite and adjacent components:
  const unsigned long *gC2R = cconvert->gC2R;
  const unsigned long *gC2G = cconvert->gC2G;
  const unsigned long *gC2B = cconvert->gC2B;
  const unsigned long *gM2R = cconvert->gM2R;
  const unsigned long *gM2G = cconvert->gM2G;
  const unsigned long *gM2B = cconvert->gM2B;
  const unsigned long *gY2R = cconvert->gY2R;
  const unsigned long *gY2G = cconvert->gY2G;
  const unsigned long *gY2B = cconvert->gY2B;

  JDIMENSION num_cols = cinfo->output_width;

  while (--num_rows >= 0) {
    JSAMPROW inptr0 = input_buf[0][input_row];
    JSAMPROW inptr1 = input_buf[1][input_row];
    JSAMPROW inptr2 = input_buf[2][input_row];
    JSAMPROW inptr3 = input_buf[3][input_row];
    JSAMPROW outptr = *output_buf++;
    JDIMENSION col;

    input_row++;

    for (col = 0; col < num_cols; col++) {
      // Get the CMYK bytes - note that this "CMYK" is actually
      // assumed to be (1-R)(1-G)(1-B)(1-K)
      int C = MAXJSAMPLE - GETJSAMPLE(inptr0[col]);
      int M = MAXJSAMPLE - GETJSAMPLE(inptr1[col]);
      int Y = MAXJSAMPLE - GETJSAMPLE(inptr2[col]);
      int K = MAXJSAMPLE - GETJSAMPLE(inptr3[col]);

      // process them through our mapping, the DEBUG check above
      // guarantees no overflow here.
      outptr[RGB_RED] = (JSAMPLE)
       (((       f[K]
	    * gC2R[C] >> 16)
	    * gM2R[M] >> 16)
	    * gY2R[Y] >> 24);

      outptr[RGB_GREEN] = (JSAMPLE)
       (((       f[K]
	    * gM2G[M] >> 16)
	    * gY2G[Y] >> 16)
	    * gC2G[C] >> 24);

      outptr[RGB_BLUE] = (JSAMPLE)
       (((       f[K]
	    * gY2B[Y] >> 16)
	    * gC2B[C] >> 16)
	    * gM2B[M] >> 24);

      outptr += 3;
    }
  }
}


/*----------------------------------------------------------------------------
	Code which converts YCCK->CMYK->RGB, this must do two steps.
------------------------------------------------------------------- JohnBo -*/
METHODDEF(void)
ycck_rgb_convert (j_decompress_ptr cinfo,
		  JSAMPIMAGE input_buf, JDIMENSION input_row,
		  JSAMPARRAY output_buf, int num_rows)
{
  my_cconvert_ptr cconvert = (my_cconvert_ptr) cinfo->cconvert;

  const unsigned long *f = cconvert->f;    // Lookup table for K conversion
  // Lookup tables for opposite and adjacent components:
  const unsigned long *gC2R = cconvert->gC2R;
  const unsigned long *gC2G = cconvert->gC2G;
  const unsigned long *gC2B = cconvert->gC2B;
  const unsigned long *gM2R = cconvert->gM2R;
  const unsigned long *gM2G = cconvert->gM2G;
  const unsigned long *gM2B = cconvert->gM2B;
  const unsigned long *gY2R = cconvert->gY2R;
  const unsigned long *gY2G = cconvert->gY2G;
  const unsigned long *gY2B = cconvert->gY2B;
  const int * Crrtab = cconvert->Cr_r_tab;
  const int * Cbbtab = cconvert->Cb_b_tab;
  const INT32 * Crgtab = cconvert->Cr_g_tab;
  const INT32 * Cbgtab = cconvert->Cb_g_tab;
  SHIFT_TEMPS

  JDIMENSION num_cols = cinfo->output_width;
  /* copy these pointers into registers if possible */
  JSAMPLE * range_limit = cinfo->sample_range_limit;

  while (--num_rows >= 0) {
    JSAMPROW inptr0 = input_buf[0][input_row];
    JSAMPROW inptr1 = input_buf[1][input_row];
    JSAMPROW inptr2 = input_buf[2][input_row];
    JSAMPROW inptr3 = input_buf[3][input_row];
    JSAMPROW outptr = *output_buf++;
    JDIMENSION col;

    input_row++;

    for (col = 0; col < num_cols; col++) {
      int y  = GETJSAMPLE(inptr0[col]);
      int cb = GETJSAMPLE(inptr1[col]);
      int cr = GETJSAMPLE(inptr2[col]);
      /* Range-limiting is essential due to noise introduced by DCT losses,
	 we could avoid the range limit, but our CMYK->RGB LUTs are only
	 256 entries - we would have to make this cope with out of range
	 values too. */
      // Get the CMYK bytes
      int C = range_limit[y + Crrtab[cr]];	/* cyan (1-red) */
      int M = range_limit[y +			/* magenta (1-green) */
			      ((int) RIGHT_SHIFT(Cbgtab[cb] + Crgtab[cr],
						 SCALEBITS))];
      int Y = range_limit[y + Cbbtab[cb]];	/* yellow (1-blue) */
      /* Need real K here. */
      int K = MAXJSAMPLE - GETJSAMPLE(inptr3[col]);

      // process them through our mapping
      outptr[RGB_RED] = (JSAMPLE)
       (((       f[K]
	    * gC2R[C] >> 16)
	    * gM2R[M] >> 16)
	    * gY2R[Y] >> 24);

      outptr[RGB_GREEN] = (JSAMPLE)
       (((       f[K]
	    * gM2G[M] >> 16)
	    * gY2G[Y] >> 16)
	    * gC2G[C] >> 24);

      outptr[RGB_BLUE] = (JSAMPLE)
       (((       f[K]
	    * gY2B[Y] >> 16)
	    * gC2B[C] >> 16)
	    * gM2B[M] >> 24);

      outptr += 3;
    }
  }
}


/*****************************************************************************
	CMYK SUPPORT [END]
******************************************************************* JohnBo **/

/*****************************************************************************
    tinyclr - 16-Bit RGB SUPPORT
 *****************************************************************************/

METHODDEF(void)
ycc_rgb16_convert (j_decompress_ptr cinfo,
		 JSAMPIMAGE input_buf, JDIMENSION input_row,
		 JSAMPARRAY output_buf, int num_rows)
{
    my_cconvert_ptr cconvert = (my_cconvert_ptr) cinfo->cconvert;
    register int y, cb, cr;
    register JSAMPROW outptr;
    register JSAMPROW inptr0, inptr1, inptr2;
    register JDIMENSION col;
    JDIMENSION num_cols = cinfo->output_width;
    /* copy these pointers into registers if possible */
    register JSAMPLE * range_limit = cinfo->sample_range_limit;
    register int * Crrtab = cconvert->Cr_r_tab;
    register int * Cbbtab = cconvert->Cb_b_tab;
    register INT32 * Crgtab = cconvert->Cr_g_tab;
    register INT32 * Cbgtab = cconvert->Cb_g_tab;
    SHIFT_TEMPS

        int R,G,B;
    while (--num_rows >= 0) {
        inptr0 = input_buf[0][input_row];
        inptr1 = input_buf[1][input_row];
        inptr2 = input_buf[2][input_row];
        input_row++;
        outptr = *output_buf++;
        for (col = 0; col < num_cols; col++) {
            y  = GETJSAMPLE(inptr0[col]);
            cb = GETJSAMPLE(inptr1[col]);
            cr = GETJSAMPLE(inptr2[col]);
            /* Range-limiting is essential due to noise introduced by DCT losses. */
            R = range_limit[y + Crrtab[cr]];
            G = range_limit[y +
                ((int) RIGHT_SHIFT(Cbgtab[cb] + Crgtab[cr],
                SCALEBITS))];
            B = range_limit[y + Cbbtab[cb]];

            *((UINT16*)outptr) = ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);

            outptr += 2;
        }	
    }
}

METHODDEF(void)
rgb_rgb16_convert (j_decompress_ptr cinfo,
		  JSAMPIMAGE input_buf, JDIMENSION input_row,
		  JSAMPARRAY output_buf, int num_rows)
{
    JDIMENSION num_cols = cinfo->output_width;

    while (--num_rows >= 0) {
        JSAMPROW inptr0 = input_buf[0][input_row];
        JSAMPROW inptr1 = input_buf[1][input_row];
        JSAMPROW inptr2 = input_buf[2][input_row];
        JSAMPROW outptr = *output_buf++;
        JDIMENSION col;

        input_row++;

        for (col = 0; col < num_cols; col++) {
            int R = GETJSAMPLE(inptr0[col]);
            int G = GETJSAMPLE(inptr1[col]);
            int B = GETJSAMPLE(inptr2[col]);

            *((UINT16*)outptr) = ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);

            outptr += 2;
        }
    }
}

METHODDEF(void)
gray_rgb16_convert (j_decompress_ptr cinfo,
		  JSAMPIMAGE input_buf, JDIMENSION input_row,
		  JSAMPARRAY output_buf, int num_rows)
{
    JDIMENSION num_cols = cinfo->output_width;

    while (--num_rows >= 0) {
        JSAMPROW inptr0 = input_buf[0][input_row];
        JSAMPROW outptr = *output_buf++;
        JDIMENSION col;

        input_row++;

        for (col = 0; col < num_cols; col++) {
            int value = GETJSAMPLE(inptr0[col]);

            *((UINT16*)outptr) = ((value >> 3) << 11) | ((value >> 2) << 5) | (value >> 3);

            outptr += 2;
        }
    }
}

/*****************************************************************************
    tinyclr - 16-Bit RGB SUPPORT [END]
 *****************************************************************************/

/*
 * Empty method for start_pass.
 */

METHODDEF(void)
start_pass_dcolor (j_decompress_ptr cinfo)
{
  /* no work needed */
}


/*
 * Module initialization routine for output colorspace conversion.
 */

GLOBAL(void)
jinit_color_deconverter (j_decompress_ptr cinfo)
{
  my_cconvert_ptr cconvert;
  int ci;

  cconvert = (my_cconvert_ptr)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				SIZEOF(my_color_deconverter));
  cinfo->cconvert = (struct jpeg_color_deconverter *) cconvert;
  cconvert->pub.start_pass = start_pass_dcolor;

  /* Make sure num_components agrees with jpeg_color_space */
  switch (cinfo->jpeg_color_space) {
  case JCS_GRAYSCALE:
    if (cinfo->num_components != 1)
      ERREXIT(cinfo, JERR_BAD_J_COLORSPACE);
    break;

  case JCS_RGB:
  case JCS_YCbCr:
    if (cinfo->num_components != 3)
      ERREXIT(cinfo, JERR_BAD_J_COLORSPACE);
    break;

  case JCS_CMYK:
  case JCS_YCCK:
    if (cinfo->num_components != 4)
      ERREXIT(cinfo, JERR_BAD_J_COLORSPACE);
    break;

  default:			/* JCS_UNKNOWN can be anything */
    if (cinfo->num_components < 1)
      ERREXIT(cinfo, JERR_BAD_J_COLORSPACE);
    break;
  }

  /* Set out_color_components and conversion method based on requested space.
   * Also clear the component_needed flags for any unused components,
   * so that earlier pipeline stages can avoid useless computation.
   */

  switch (cinfo->out_color_space) {
  case JCS_GRAYSCALE:
    cinfo->out_color_components = 1;
    if (cinfo->jpeg_color_space == JCS_GRAYSCALE ||
	cinfo->jpeg_color_space == JCS_YCbCr) {
      cconvert->pub.color_convert = grayscale_convert;
      /* For color->grayscale conversion, only the Y (0) component is needed */
      for (ci = 1; ci < cinfo->num_components; ci++)
	cinfo->comp_info[ci].component_needed = FALSE;
    } else
      ERREXIT(cinfo, JERR_CONVERSION_NOTIMPL);
    break;

  case JCS_RGB:
    cinfo->out_color_components = RGB_PIXELSIZE;
    if (cinfo->jpeg_color_space == JCS_YCbCr) {
#ifdef JPEG_MMX_SUPPORTED
      /* WARNING: this relies on the DCT selector to select the code used
	 for color space convertion.  This is something of a hack. */
      if (cinfo->dct_method == JDCT_ISLOW_MMX ||
	  cinfo->dct_method == JDCT_IFAST_MMX)
	cconvert->pub.color_convert = ycc_rgb_convert_mmx;
      else
#endif
      cconvert->pub.color_convert = ycc_rgb_convert;
      build_ycc_rgb_table(cinfo);
    } else if (cinfo->jpeg_color_space == JCS_GRAYSCALE) {
      cconvert->pub.color_convert = gray_rgb_convert;
#if RGB_PIXELSIZE == 3	  
    } else if (cinfo->jpeg_color_space == JCS_RGB) {
      cconvert->pub.color_convert = null_convert;
#endif
    } else if (cinfo->jpeg_color_space == JCS_CMYK) {
      cconvert->pub.color_convert = cmyk_rgb_convert;
      build_cmyk_rgb_table(cinfo);
    } else if (cinfo->jpeg_color_space == JCS_YCCK) {
      cconvert->pub.color_convert = ycck_rgb_convert;
      // Need *both* convertion tables!
      build_ycc_rgb_table(cinfo);
      build_cmyk_rgb_table(cinfo);
    } else
      ERREXIT(cinfo, JERR_CONVERSION_NOTIMPL);
    break;

  case JCS_CMYK:
    cinfo->out_color_components = 4;
    if (cinfo->jpeg_color_space == JCS_YCCK) {
      cconvert->pub.color_convert = ycck_cmyk_convert;
      build_ycc_rgb_table(cinfo);
    } else if (cinfo->jpeg_color_space == JCS_CMYK) {
      cconvert->pub.color_convert = null_convert;
    } else
      ERREXIT(cinfo, JERR_CONVERSION_NOTIMPL);
    break;

  case TINYCLR_RGB16:
    // tinyclr - Added support for 5-6-5 16bit RGB format (from YCrCb, RGB, or grayscale)
    cinfo->out_color_components = 2;
    if (cinfo->jpeg_color_space == JCS_YCbCr) {
      cconvert->pub.color_convert = ycc_rgb16_convert;
      build_ycc_rgb_table(cinfo);
    } else if (cinfo->jpeg_color_space == JCS_RGB) {
      cconvert->pub.color_convert = rgb_rgb16_convert;
    } else if (cinfo->jpeg_color_space == JCS_GRAYSCALE) {
      cconvert->pub.color_convert = gray_rgb16_convert;
    } else
      ERREXIT(cinfo, JERR_CONVERSION_NOTIMPL);
    break;

  default:
    /* Permit null conversion to same output space */
    if (cinfo->out_color_space == cinfo->jpeg_color_space) {
      cinfo->out_color_components = cinfo->num_components;
      cconvert->pub.color_convert = null_convert;
    } else			/* unsupported non-null conversion */
      ERREXIT(cinfo, JERR_CONVERSION_NOTIMPL);
    break;
  }

  if (cinfo->quantize_colors)
    cinfo->output_components = 1; /* single colormapped output component */
  else
    cinfo->output_components = cinfo->out_color_components;
}

