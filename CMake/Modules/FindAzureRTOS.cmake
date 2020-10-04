#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

###################################################################################################################################
# WHEN ADDING A NEW series add the respective name to the list bellow along with the CMake files with GCC options and source files
###################################################################################################################################

# check if the series name is supported 

set(AZURERTOS_SUPPORTED_SERIES STM32L4xx CACHE INTERNAL "supported series names for AzureRTOS")

list(FIND AZURERTOS_SUPPORTED_SERIES ${TARGET_SERIES} TARGET_SERIES_NAME_INDEX)

if(TARGET_SERIES_NAME_INDEX EQUAL -1)

    message(FATAL_ERROR "\n\nSorry but ${TARGET_SERIES} is not supported at this time...\nYou can wait for it to be added, or you might want to contribute and start working on a PR for it.\n\n")

endif()

# including here the CMake files for the source files specific to the target series
include(AZURERTOS_${TARGET_SERIES}_sources)
# and here the GCC options tuned for the target series 
include(AZURERTOS_${TARGET_SERIES}_GCC_options)

message("AzureRTOS board series is ${TARGET_SERIES}") # debug helper

# set include directories for AzureRTOS
list(APPEND AZURERTOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/AzureRTOS_Source/include)

# source files and GCC options according to target vendor and series

# source files for AZURERTOS
set(AZURERTOS_SRCS
    croutine.c
    event_groups.c
    list.c
    queue.c
    stream_buffer.c
    tasks.c
    timers.c

)

foreach(SRC_FILE ${AZURERTOS_SRCS})
    set(AZURERTOS_SRC_FILE SRC_FILE -NOTFOUND)
    find_file(AZURERTOS_SRC_FILE ${SRC_FILE}
        PATHS

            ${PROJECT_BINARY_DIR}/AzureRTOS_Source

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    message("${SRC_FILE} >> ${AZURERTOS_SRC_FILE}") # debug helper
    list(APPEND AZURERTOS_SOURCES ${AZURERTOS_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(AZURERTOS DEFAULT_MSG AZURERTOS_INCLUDE_DIRS AZURERTOS_SOURCES)
