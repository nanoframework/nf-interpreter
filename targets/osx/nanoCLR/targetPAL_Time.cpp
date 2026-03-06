//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <atomic>
#include <cstdint>
#include "targetPAL_time.h"

namespace
{
std::atomic<uint64_t> s_nextCompareValue{0};
} // namespace

extern "C" void Time_SetCompare(uint64_t compareValue)
{
    // TODO: replace with a real completion timer queue wired into HAL_COMPLETION.
    s_nextCompareValue.store(compareValue, std::memory_order_relaxed);
}

uint64_t nanoCLR_OSX_GetNextCompareValue()
{
    return s_nextCompareValue.load(std::memory_order_relaxed);
}
