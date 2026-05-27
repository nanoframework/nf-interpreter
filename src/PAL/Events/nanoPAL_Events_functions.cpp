//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL.h>

HRESULT PalEvent_Initialize()
{
    return g_palEventDriver.Initialize();
}

HRESULT PalEvent_Uninitialize()
{
    return g_palEventDriver.Uninitialize();
}

HRESULT PalEvent_Post(unsigned int e, unsigned int param)
{
    return g_palEventDriver.PostEvent(e, param);
}

HRESULT PalEvent_Enlist(PalEventListener* listener)
{
    return g_palEventDriver.EnlistListener(listener);
}
