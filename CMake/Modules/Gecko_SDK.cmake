#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)

# include the Gecko HAL and CMSIS for the appropriate series

function(nf_set_silabs_target_series)

    # process target series, which is in the format "SILABS_xxxxx"
    string(REPLACE "SILABS" "" TARGET_SERIES_SHORT "${TARGET_SERIES}")

    # store the series name for later use
    set(TARGET_SERIES_SHORT ${TARGET_SERIES_SHORT} CACHE INTERNAL "Silabs series short name")

    if("${TARGET_SERIES}" STREQUAL "EFM32GG1")
        # WHEN CHANGING THESE MAKE SURE TO UPDATE THE DEV CONTAINERS
        # set(HAL_DRIVER_GIT_TAG v1.7.6)
        # set(CMSIS_DEVICE_GIT_TAG v2.3.6)
    else()
        # series is NOT supported
        message(FATAL_ERROR "\n\nSorry but the ${TARGET_SERIES} is not supported at this time...\nYou can wait for it to be added, or you might want to contribute by working on a PR for it.\n\n")
    endif()

endfunction()


# check if cube package source was specified or if it's empty (default is empty)
macro(nf_process_gecko_sdk)

    FetchContent_GetProperties(gecko_sdk)

    # general files

    #series specific files
    if("${TARGET_SERIES}" STREQUAL "EFM32GG1")
        
    set(GECKO_SDK_SRCS
        
        # RT
        chcore.c
        chcoreasm.S
    )

    endif()

endmacro()

# macro to be called from binutils to add Gecko SDK as a library
# optional EXTRA_INCLUDES with include paths to be added to the library
# optional EXTRA_COMPILE_DEFINITIONS with compiler definitions to be added to the library
macro(nf_add_gecko_sdk)
   
    # parse arguments
    cmake_parse_arguments(NFGCKSDK "" "BUILD_TARGET" "EXTRA_INCLUDES;EXTRA_COMPILE_DEFINITIONS" ${ARGN})
    
    if("${NFGCKSDK_BUILD_TARGET}" STREQUAL "${NANOBOOTER_PROJECT_NAME}")
        set(CONFIG_FILES_PATH "${TARGET_BASE_LOCATION}/nanoBooter")
    elseif("${NFGCKSDK_BUILD_TARGET}" STREQUAL "${NANOCLR_PROJECT_NAME}")
        set(CONFIG_FILES_PATH "${TARGET_BASE_LOCATION}/nanoCLR")
    endif()

    # add THESE has a library
    set(LIB_NAME gecko_sdk)

    add_library(
        ${LIB_NAME} STATIC 
        gecko_sdk_SOURCES
    )

    target_include_directories(
        ${LIB_NAME} 
        PUBLIC 
            "gecko_sdk_INCLUDE_DIRS}"
            ${NFGCKSDK_EXTRA_INCLUDES}
    )

    nf_set_compile_options(TARGET ${LIB_NAME} BUILD_TARGET ${NFGCKSDK_BUILD_TARGET})
    nf_set_compile_definitions(TARGET ${LIB_NAME} EXTRA_COMPILE_DEFINITIONS ${NFGCKSDK_EXTRA_COMPILE_DEFINITIONS} BUILD_TARGET ${NFGCKSDK_BUILD_TARGET})
    nf_set_link_options(TARGET ${LIB_NAME})

    # add alias
    add_library("nano::${LIB_NAME}" ALIAS ${LIB_NAME})
    
endmacro()
