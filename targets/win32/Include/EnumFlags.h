//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#pragma once

#include <type_traits>

// Use "old school template" as doing this in real templates is just a major PITA or unreadable gunk.
#define ENUM_FLAGS(t)                                                                                                  \
    inline t operator|(t a, t b)                                                                                       \
    {                                                                                                                  \
        return static_cast<t>(                                                                                         \
            static_cast<std::underlying_type<t>::type>(a) | static_cast<std::underlying_type<t>::type>(b));            \
    }                                                                                                                  \
    inline t &operator|=(t &a, t b)                                                                                    \
    {                                                                                                                  \
        a = static_cast<t>(                                                                                            \
            static_cast<std::underlying_type<t>::type>(a) | static_cast<std::underlying_type<t>::type>(b));            \
        return a;                                                                                                      \
    }
