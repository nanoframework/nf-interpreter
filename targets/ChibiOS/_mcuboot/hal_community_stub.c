//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>

void halCommunityInit(void)
{
#if defined(HAL_USE_USBH) && (HAL_USE_USBH == TRUE)
    usbhInit();
#endif
}
