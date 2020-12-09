//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOHAL_GRAPHICS_H_
#define _NANOHAL_GRAPHICS_H_ 1

#include <target_platform.h>

#if (NANOCLR_GRAPHICS == TRUE)
#include "Display.h"
#include "DisplayInterface.h"
#include "TouchDevice.h"
#include "TouchInterface.h"
#include "GraphicsMemoryHeap.h"
#include "Graphics.h"

extern DisplayInterface g_DisplayInterface;
extern DisplayDriver g_DisplayDriver;

extern TouchDevice g_TouchDevice;
extern TouchInterface g_TouchInterface;

extern GraphicsMemoryHeap g_GraphicsMemoryHeap;
#endif

#endif
