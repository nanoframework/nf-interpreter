//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOCLR_WIN32_H_
#define _NANOCLR_WIN32_H_

////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_Windows_FastSleep( signed __int64 ticks );

bool HAL_Windows_IsShutdownPending();

void HAL_Windows_AcquireGlobalLock();
void HAL_Windows_ReleaseGlobalLock();
bool HAL_Windows_HasGlobalLock();

unsigned __int64 HAL_Windows_GetPerformanceTicks();

void HAL_Windows_Debug_Print( char* szText );
                        
////////////////////////////////////////////////////////////////////////////////////////////////////

class HAL_Mutex
{
    CRITICAL_SECTION m_data;

public:
    HAL_Mutex();
    ~HAL_Mutex();

    //--//

    void Lock  ();
    void Unlock();
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _NANOCLR_WIN32_H_

