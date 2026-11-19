//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"

#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

#include "StringTableData.cpp"

////////////////////////////////////////////////////////////////////////////////////////////////////

void CLR_RT_Assembly::InitString()
{
    NATIVE_PROFILE_CLR_CORE();
}

static const char *GetSharedString(size_t index)
{
    if (index >= c_CLR_StringTable_RootCount)
    {
        return &c_CLR_StringTable_Data
            [c_CLR_StringTable_Lookup[c_CLR_StringTable_CheckpointCount + index - c_CLR_StringTable_RootCount]];
    }

    const char *p = &c_CLR_StringTable_Data[c_CLR_StringTable_Lookup[index >> c_CLR_StringTable_StrideShift]];

    for (size_t skip = index & ((1 << c_CLR_StringTable_StrideShift) - 1); skip > 0; skip--)
    {
        while (*p++ != 0)
        {
        }
    }

    return p;
}

const char *CLR_RT_Assembly::GetString(CLR_STRING i)
{
    NATIVE_PROFILE_CLR_CORE();
    static const CLR_STRING iMax = 0xFFFF - c_CLR_StringTable_Size;

    if (i >= iMax)
    {
        return GetSharedString((CLR_STRING)0xFFFF - i);
    }

    return &(((const char *)GetTable(TBL_Strings))[i]);
}

bool CLR_RT_Assembly::IsSharedString(CLR_STRING i)
{
    return i >= (CLR_STRING)(0x10000 - c_CLR_StringTable_Size);
}

bool CLR_RT_Assembly::StringEquals(CLR_STRING i, const char *text, CLR_STRING textId)
{
    if (IsSharedString(i) && IsSharedString(textId))
    {
        return i == textId;
    }

    return strcmp(GetString(i), text) == 0;
}

#if defined(VIRTUAL_DEVICE)

void CLR_RT_Assembly::InitString(std::map<std::string, CLR_OFFSET> &map)
{
    NATIVE_PROFILE_CLR_CORE();

    map.clear();

    for (size_t index = 0; index < c_CLR_StringTable_Size; index++)
    {
        map[GetSharedString(index)] = (CLR_OFFSET)(0xFFFF - index);
    }
}

#endif
