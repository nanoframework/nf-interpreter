#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

option(NF_PROFILE_NEW_CALLS "option to support profilling new function calls" OFF)
option(NF_PROFILE_NEW_ALLOCATIONS "option to support profilling new object allocations" OFF)
option(NF_TRACE_MEMORY_STATS "option to enable trace of memory stats" OFF)

if(NF_PROFILE_NEW_CALLS)
    set(NANOCLR_PROFILE_NEW_CALLS TRUE CACHE INTERNAL "option to support profilling new function calls")
endif()
if(NF_PROFILE_NEW_ALLOCATIONS)
    set(NANOCLR_PROFILE_NEW_ALLOCATIONS TRUE CACHE INTERNAL "option to support profilling new object allocations")
endif()
if(NF_TRACE_MEMORY_STATS)
    set(NANOCLR_TRACE_MEMORY_STATS TRUE CACHE INTERNAL "option to enable trace of memory stats")
endif()
