#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

###################################################################################################################################
# WHEN ADDING A NEW series add the respective name to the list bellow along with the CMake files with GCC options and source files
###################################################################################################################################

# check if the series name is supported 

set(FREERTOS_SUPPORTED_SERIES "IMXRT10xx" CACHE INTERNAL "supported series names for FreeRTOS")
list(FIND FREERTOS_SUPPORTED_SERIES ${TARGET_SERIES} TARGET_SERIES_NAME_INDEX)
if(TARGET_SERIES_NAME_INDEX EQUAL -1)
    message(FATAL_ERROR "\n\nSorry but ${TARGET_SERIES} is not supported at this time...\nYou can wait for that to be added or you might want to contribute and start working on a PR for that.\n\n")
endif()

# including here the CMake files for the source files specific to the target series
include(FREERTOS_${TARGET_SERIES}_sources)
# and here the GCC options tuned for the target series 
include(FREERTOS_${TARGET_SERIES}_GCC_options)

# message("FreeRTOS board series is ${TARGET_SERIES}") # debug helper

# set include directories for FreeRTOS
list(APPEND FREERTOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/FreeRTOS_Source/include) 
list(APPEND FREERTOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/FreeRTOS_Source/portable/GCC/ARM_CM7/r0p1)

# source files and GCC options according to target vendor and series

# source files for FreeRTOS
set(FREERTOS_SRCS

    croutine.c
    event_groups.c
    list.c
    queue.c
    stream_buffer.c
    tasks.c
    timers.c

)

foreach(SRC_FILE ${FREERTOS_SRCS})
    set(FREERTOS_SRC_FILE SRC_FILE -NOTFOUND)
    find_file(FREERTOS_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_BINARY_DIR}/FreeRTOS_Source/

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${FREERTOS_SRC_FILE}") # debug helper
    list(APPEND FREERTOS_SOURCES ${FREERTOS_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(FREERTOS DEFAULT_MSG FREERTOS_INCLUDE_DIRS FREERTOS_SOURCES)
