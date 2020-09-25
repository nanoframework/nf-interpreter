#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

###################################################################################################################################
# WHEN ADDING A NEW series add the respective name to the list below along with the CMake files with GCC options and source files
###################################################################################################################################

# check if the series name is supported 

set(TI_SIMPLELINK_SUPPORTED_SERIES "CC13x2_26x2" "CC32xx" CACHE INTERNAL "supported TI series names")

list(FIND TI_SIMPLELINK_SUPPORTED_SERIES ${TARGET_SERIES} TARGET_SERIES_NAME_INDEX)
if(TI_SIMPLELINK_SUPPORTED_SERIES EQUAL -1)
    if(TARGET_SERIES_NAME_INDEX EQUAL -1)
        message(FATAL_ERROR "\n\nSorry but the ${TARGET_SERIES} is not supported at this time...\nYou can't wait for it to be added, or you might want to contribute by working on a PR for it.\n\n")
    else()
        # series is supported by TI
        set(TARGET_VENDOR "TI" CACHE INTERNAL "target vendor is TI")
    endif()
endif()

# including here the CMake files for the source files specific to the target series
include(TI_SimpleLink_${TARGET_SERIES}_sources)

# and here the GCC options tuned for the target series 
include(TI_SimpleLink_${TARGET_SERIES}_GCC_options)

# includes for TI_RTOS
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source/kernel/tirtos/packages")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source/kernel/tirtos/packages/ti/sysbios")

# include(FindPackageHandleStandardArgs)

# FIND_PACKAGE_HANDLE_STANDARD_ARGS(TI_SimpleLink DEFAULT_MSG TI_SimpleLink_INCLUDE_DIRS TI_SimpleLink_SOURCES)
