#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(freertos)

###################################################################################################################################
# WHEN ADDING A NEW series add the respective name to the list bellow along with the CMake files with GCC options and source files
###################################################################################################################################

# check if the series name is supported 

set(FreeRTOS_SUPPORTED_SERIES IMXRT10xx CACHE INTERNAL "supported series names for FreeRTOS")

list(FIND FreeRTOS_SUPPORTED_SERIES ${TARGET_SERIES} TARGET_SERIES_NAME_INDEX)

if(TARGET_SERIES_NAME_INDEX EQUAL -1)

    message(FATAL_ERROR "\n\nSorry but ${TARGET_SERIES} is not supported at this time...\nYou can wait for that to be added or you might want to contribute and start working on a PR for that.\n\n")

endif()

# including here the CMake files for the source files specific to the target series
include(FreeRTOS_${TARGET_SERIES}_sources)
# and here the GCC options tuned for the target series 
include(FreeRTOS_${TARGET_SERIES}_GCC_options)

# message("FreeRTOS board series is ${TARGET_SERIES}") # debug helper

# set include directories for FreeRTOS
list(APPEND FreeRTOS_INCLUDE_DIRS ${freertos_SOURCE_DIR}/include)
list(APPEND FreeRTOS_INCLUDE_DIRS ${freertos_SOURCE_DIR}/portable/GCC/ARM_CM7/r0p1)

# source files and GCC options according to target vendor and series

# source files for FreeRTOS
set(FreeRTOS_SRCS
    croutine.c
    event_groups.c
    list.c
    queue.c
    stream_buffer.c
    tasks.c
    timers.c

)

foreach(SRC_FILE ${FreeRTOS_SRCS})

    set(FreeRTOS_SRC_FILE SRC_FILE -NOTFOUND)

    find_file(FreeRTOS_SRC_FILE ${SRC_FILE}
        PATHS

        ${freertos_SOURCE_DIR}

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${FreeRTOS_SRC_FILE}") # debug helper
    endif()
     
    list(APPEND FreeRTOS_SOURCES ${FreeRTOS_SRC_FILE})

endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(FreeRTOS DEFAULT_MSG FreeRTOS_INCLUDE_DIRS FreeRTOS_SOURCES)
