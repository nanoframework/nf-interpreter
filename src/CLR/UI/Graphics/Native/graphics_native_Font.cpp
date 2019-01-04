////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Graphics.h"

HRESULT Library_graphics_native_Font::CharWidth___I4__CHAR( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_GRAPHICS();
    NANOCLR_HEADER();

    CLR_GFX_Font*             font;
    CLR_GFX_FontCharacterInfo chr;
    CLR_UINT32                width = 0;

    NANOCLR_CHECK_HRESULT(GetFont( stack, font ));

    font->GetCharInfo( stack.Arg1().NumericByRef().u2, chr );

    if (chr.isValid) width = chr.width;

    stack.SetResult_I4( width );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_graphics_native_Font::get_Height___I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_GRAPHICS();
    NANOCLR_HEADER();

    CLR_GFX_Font* font;
    
    NANOCLR_CHECK_HRESULT(GetFont( stack, font ));

    stack.SetResult_I4( font->m_font.m_metrics.m_height );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_graphics_native_Font::get_AverageWidth___I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_GRAPHICS();
    NANOCLR_HEADER();

    CLR_GFX_Font* font;
    
    NANOCLR_CHECK_HRESULT(GetFont( stack, font ));

    stack.SetResult_I4( font->m_font.m_metrics.m_aveCharWidth );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_graphics_native_Font::get_MaxWidth___I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_GRAPHICS();
    NANOCLR_HEADER();

    CLR_GFX_Font* font;
    
    NANOCLR_CHECK_HRESULT(GetFont( stack, font ));

    stack.SetResult_I4( font->m_font.m_metrics.m_maxCharWidth );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_graphics_native_Font::get_Ascent___I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_GRAPHICS();
    NANOCLR_HEADER();

    CLR_GFX_Font* font;
    
    NANOCLR_CHECK_HRESULT(GetFont( stack, font ));

    stack.SetResult_I4( font->m_font.m_metrics.m_ascent );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_graphics_native_Font::get_Descent___I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_GRAPHICS();
    NANOCLR_HEADER();

    CLR_GFX_Font* font;
    
    NANOCLR_CHECK_HRESULT(GetFont( stack, font ));

    stack.SetResult_I4( font->m_font.m_metrics.m_descent );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_graphics_native_Font::get_InternalLeading___I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_GRAPHICS();
    NANOCLR_HEADER();

    CLR_GFX_Font* font;
    
    NANOCLR_CHECK_HRESULT(GetFont( stack, font ));

    stack.SetResult_I4( font->m_font.m_metrics.m_internalLeading );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_graphics_native_Font::get_ExternalLeading___I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_GRAPHICS();
    NANOCLR_HEADER();

    CLR_GFX_Font* font;
    
    NANOCLR_CHECK_HRESULT(GetFont( stack, font ));

    stack.SetResult_I4( font->m_font.m_metrics.m_externalLeading );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_graphics_native_Font::ComputeExtent___VOID__STRING__BYREF_I4__BYREF_I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_GRAPHICS();
    NANOCLR_HEADER();

    CLR_GFX_Font*     font;
    CLR_RT_HeapBlock* pArgs;
    LPCSTR            szText;
    CLR_RT_HeapBlock  hbWidth;
    CLR_RT_HeapBlock  hbHeight;
    int               width;
    CLR_INT32         kerning;

    NANOCLR_CHECK_HRESULT(GetFont( stack, font ));

    pArgs   = &(stack.Arg1());
    szText  = pArgs[ 0 ].RecoverString(); FAULT_ON_NULL(szText);
    kerning = pArgs[ 3 ].NumericByRef().s4;

    width = font->StringOut( szText, -1, kerning, NULL, 0, 0, 0 );

    hbWidth .SetInteger( (CLR_INT32)width                           ); NANOCLR_CHECK_HRESULT(hbWidth .StoreToReference( pArgs[ 1 ], 0 ));
    hbHeight.SetInteger( (CLR_INT32)font->m_font.m_metrics.m_height ); NANOCLR_CHECK_HRESULT(hbHeight.StoreToReference( pArgs[ 2 ], 0 ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_graphics_native_Font::ComputeTextInRect___VOID__STRING__BYREF_I4__BYREF_I4__I4__I4__I4__I4__U4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_GRAPHICS();
    NANOCLR_HEADER();

    CLR_GFX_Font*     font;
    CLR_RT_HeapBlock* pArgs;
    LPCSTR            szText;
    CLR_RT_HeapBlock  hbWidth;
    CLR_RT_HeapBlock  hbHeight;
    int               renderWidth;
    int               renderHeight;
    int               xRelStart;
    int               yRelStart;
    int               availableWidth;
    int               availableHeight;
    CLR_UINT32        flags;

    NANOCLR_CHECK_HRESULT(GetFont( stack, font ));

    pArgs           = &(stack.Arg1());
    szText          = pArgs[ 0 ].RecoverString(); FAULT_ON_NULL(szText);
    xRelStart       = pArgs[ 3 ].NumericByRef().s4;
    yRelStart       = pArgs[ 4 ].NumericByRef().s4;
    availableWidth  = pArgs[ 5 ].NumericByRef().s4;
    availableHeight = pArgs[ 6 ].NumericByRef().s4;
    flags           = pArgs[ 7 ].NumericByRef().u4;

    NANOCLR_CHECK_HRESULT(CLR_GFX_Bitmap::DrawTextInRect(szText, xRelStart, yRelStart, renderWidth, renderHeight, NULL, 0, 0, availableWidth, availableHeight, flags, 0, font));

    hbWidth .SetInteger( (CLR_INT32)renderWidth  ); NANOCLR_CHECK_HRESULT(hbWidth .StoreToReference( pArgs[ 1 ], 0 ));
    hbHeight.SetInteger( (CLR_INT32)renderHeight ); NANOCLR_CHECK_HRESULT(hbHeight.StoreToReference( pArgs[ 2 ], 0 ));

    NANOCLR_NOCLEANUP();
}

//--//

HRESULT Library_graphics_native_Font::GetFont( CLR_RT_HeapBlock* pThis, CLR_GFX_Font*& font )
{
    NATIVE_PROFILE_CLR_GRAPHICS();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_BinaryBlob* blob;

    if(pThis) pThis = pThis->Dereference(); FAULT_ON_NULL(pThis);
    
#if defined(NANOCLR_APPDOMAINS)        
    if(pThis->DataType() == DATATYPE_TRANSPARENT_PROXY)
    {
        NANOCLR_CHECK_HRESULT(pThis->TransparentProxyValidate());
        pThis = pThis->TransparentProxyDereference();
    }
#endif

    blob = pThis[ FIELD__m_font ].DereferenceBinaryBlob(); 
    
    if(!blob || blob->DataType() != DATATYPE_BINARY_BLOB_HEAD) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

    font = (CLR_GFX_Font*)blob->GetData();

    NANOCLR_NOCLEANUP();
}

HRESULT Library_graphics_native_Font::GetFont( CLR_RT_StackFrame& stack, CLR_GFX_Font*& font )
{
    NATIVE_PROFILE_CLR_GRAPHICS();
    return GetFont( &stack.Arg0(), font );
}

