//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

__nfweak void CLR_RT_Random::Initialize()
{
    CLR_INT64  st  = HAL_Time_CurrentTime();

    srand((unsigned int)st);
}

__nfweak void CLR_RT_Random::Initialize( int seed )
{
    srand(seed);
}

__nfweak uint32_t CLR_RT_Random::Next()
{
    return rand();
}

#if !defined(NANOCLR_EMULATED_FLOATINGPOINT)

__nfweak double CLR_RT_Random::NextDouble()
{
    // Next() will return value between 0 - 0x7FFFFFFF (inclusive)
    return ((double)rand()) / ((double)0x7FFFFFFF);
}
#else

__nfweak CLR_INT64 CLR_RT_Random::NextDouble()
{
    // Next() will return value between 0 - 0x7FFFFFFF (inclusive)

    return  ((CLR_INT64 )rand() ) / ((CLR_INT64)0x7FFFFFFF >> CLR_RT_HeapBlock::HB_DoubleShift);
}

#endif

__nfweak void CLR_RT_Random::NextBytes( unsigned char* buffer, unsigned int count )
{
    unsigned int i;

    for(i = 0; i < count; i++)
    {
        buffer[i] = (unsigned char)rand();
    }
}
