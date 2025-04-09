#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)

macro(ProcessGSDKPackage)

    # process target name, which is in the format "SL_xxxxxxxx"
    string(REPLACE "SL_" "SL" TARGET_BOARD_SHORT "${TARGET_BOARD}")
    
    # store the target name for later use
    set(TARGET_BOARD_SHORT ${TARGET_BOARD_SHORT} CACHE INTERNAL "Gecko SDK board short name")

endmacro()
