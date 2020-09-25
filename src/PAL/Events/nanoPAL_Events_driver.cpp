//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL.h>

PalEventDriver g_palEventDriver;

bool PalEventDriver::s_initialized = FALSE;

HRESULT PalEventDriver::Initialize()
{
    if (!g_palEventDriver.s_initialized)
    {
        g_palEventDriver.listenerList.Initialize();
        PalEventDriver::s_initialized = TRUE;
    }

    return S_OK;
}

HRESULT PalEventDriver::Uninitialize()
{
    if (PalEventDriver::s_initialized)
    {
        PalEventDriver::s_initialized = FALSE;

        /// Remove all nodes.
        g_palEventDriver.listenerList.Initialize();
    }

    return S_OK;
}

HRESULT PalEventDriver::PostEvent(uint32_t e, uint32_t param)
{
    PalEventListener *listener = g_palEventDriver.listenerList.FirstNode();
        
    while(listener->Next() != NULL)
    {
        if (listener->m_eventMask & e)
        {
            listener->m_palEventListener(e, param);
        }

        listener = listener->Next();
    }

    return S_OK;
}

HRESULT PalEventDriver::EnlistListener(PalEventListener* listener)
{
    listener->Initialize();
    g_palEventDriver.listenerList.LinkAtBack(listener);

    return S_OK;
}
