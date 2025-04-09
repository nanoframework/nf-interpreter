#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(maxim_micros_sdk)
FetchContent_GetProperties(cmsis)

###################################################################################################################################
# WHEN ADDING A NEW SERIES, add the respective name to the list below along with the CMake files with GCC options and source files
###################################################################################################################################

# check if the series name is supported 

set(MAXIM_SDK_SUPPORTED_SERIES "MAX78000" CACHE INTERNAL "supported MAX series names")

list(FIND MAXIM_SDK_SUPPORTED_SERIES ${TARGET_SERIES} TARGET_SERIES_NAME_INDEX)
if(TARGET_SERIES_NAME_INDEX EQUAL -1)
    message(FATAL_ERROR "\n\nSorry but the ${TARGET_SERIES} is not supported at this time...\nYou can wait for it to be added, or you might want to contribute by working on a PR for it.\n\n")
endif()

# including here the CMake files for the source files specific to the target series
include(AzureRTOS_${TARGET_SERIES}_sources)
# and here the GCC options tuned for the target series 
include(AzureRTOS_${TARGET_SERIES}_GCC_options)

list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${cmsis_SOURCE_DIR}/CMSIS/Core/Include)

set(MaximMicrosSDK_SRCS

    # CMSIS
    heap.c
    # board.c
    # stdio.c
    # led.c
    # pb.c
    # tft_fthr.c
    # camera.c
    # ov7692.c
    # sccb.c
    # max20303.c

)

foreach(SRC_FILE ${MaximMicrosSDK_SRCS})

    set(MaximMicrosSDK_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(MaximMicrosSDK_SRC_FILE ${SRC_FILE}
        PATHS 
        
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/CMSIS
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/CMSIS/Device/Maxim/MAX78000/Source

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${MaximMicrosSDK_SRC_FILE}")
    endif()

    list(APPEND MaximMicrosSDK_SOURCES ${MaximMicrosSDK_SRC_FILE})

endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(MaximMicrosSDK DEFAULT_MSG MaximMicrosSDK_INCLUDE_DIRS MaximMicrosSDK_SOURCES)
