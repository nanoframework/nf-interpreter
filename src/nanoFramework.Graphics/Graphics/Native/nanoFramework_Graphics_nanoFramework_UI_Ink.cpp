//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Graphics.h"
#include "TouchDevice.h"
#include "nanoFramework_Graphics.h"
#include "Ink.h"

extern InkDriver g_InkDriver;

// Save a copy of the bimapped used by inking
static CLR_RT_HeapBlock* m_InkPinnedBitmap = NULL;

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Ink::SetInkRegion___STATIC__VOID__U4__I4__I4__I4__I4__I4__I4__I4__nanoFrameworkUIBitmap(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();

    CLR_GFX_Bitmap* bitmap = NULL;
    PAL_GFX_Bitmap* palBmp = NULL;

    CLR_RT_HeapBlock* pArgs = &stack.Arg0();

    if (m_InkPinnedBitmap != NULL)
    {
        m_InkPinnedBitmap->Unpin();
        m_InkPinnedBitmap = NULL;
    }

    InkRegionInfo inkRegionInfo;
    inkRegionInfo.X1 = pArgs[1].NumericByRef().s4;
    inkRegionInfo.Y1 = pArgs[2].NumericByRef().s4;
    inkRegionInfo.X2 = pArgs[3].NumericByRef().s4;
    inkRegionInfo.Y2 = pArgs[4].NumericByRef().s4;
    inkRegionInfo.BorderWidth = pArgs[5].NumericByRef().s4;

    GFX_Pen pen = { pArgs[6].NumericByRef().u4, pArgs[7].NumericByRef().s4 };
    inkRegionInfo.Pen = pen;

    m_InkPinnedBitmap = pArgs[8].Dereference();

    if (m_InkPinnedBitmap != NULL)
    {
        // we are drawing on the object in the PAL therefore it should not move
        m_InkPinnedBitmap->Pin();

        NANOCLR_CHECK_HRESULT(CLR_GFX_Bitmap::GetInstanceFromGraphicsHeapBlock(m_InkPinnedBitmap[CLR_GFX_Bitmap::FIELD__m_bitmap], bitmap));

        palBmp = &(bitmap->m_palBitmap);

        palBmp->clipping.left = inkRegionInfo.BorderWidth;
        palBmp->clipping.right = palBmp->width - inkRegionInfo.BorderWidth;
        palBmp->clipping.top = inkRegionInfo.BorderWidth;
        palBmp->clipping.bottom = palBmp->height - inkRegionInfo.BorderWidth;
    }
    else
    {
        // we are disabling the collection
    }

    inkRegionInfo.Bmp = palBmp;

    if (palBmp == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if ((inkRegionInfo.X1 >= inkRegionInfo.X2) || (inkRegionInfo.Y1 >= inkRegionInfo.Y2))
    {
        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
    }

    NANOCLR_CHECK_HRESULT(g_InkDriver.SetRegion(&inkRegionInfo));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Ink::ResetInkRegion___STATIC__VOID(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();

    if (stack.m_customState == 0) {}; //  eliminate unused parameter error

    NANOCLR_CHECK_HRESULT(g_InkDriver.ResetRegion());

    NANOCLR_NOCLEANUP();
}
