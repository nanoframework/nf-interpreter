#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)

# include the Gecko HAL and CMSIS for the appropriate series

function(nf_set_silabs_target_series)

    if("${TARGET_SERIES}" STREQUAL "EFM32GG11")
        # WHEN CHANGING THESE MAKE SURE TO UPDATE THE DEV CONTAINERS
        # set(HAL_DRIVER_GIT_TAG v1.7.6)
        # set(CMSIS_DEVICE_GIT_TAG v2.3.6)
    else()
        # series is NOT supported
        message(FATAL_ERROR "\n\nSorry but the ${TARGET_SERIES} is not supported at this time...\nYou can wait for it to be added, or you might want to contribute by working on a PR for it.\n\n")
    endif()

endfunction()


# check if cube package source was specified or if it's empty (default is empty)
macro(nf_add_gecko_sdk)

    FetchContent_GetProperties(gecko_sdk)

    # set include directories
    list(APPEND Gecko_SDK_INCLUDE_DIRS ${gecko_sdk_SOURCE_DIR}/platform/Device/SiliconLabs/EFM32GG11B/Include)

    # general files

    #series specific files
    if("${TARGET_SERIES}" STREQUAL "EFM32GG1")
        
        set(GECKO_SDK_SRCS
            system_efm32gg11b.c
            startup_efm32gg11b.c
            startup_efm32gg11b.S
        )

        foreach(SRC_FILE ${GECKO_SDK_SRCS})

            set(GECKO_SDK_SRCS_SRC_FILE SRC_FILE-NOTFOUND)

            find_file(GECKO_SDK_SRCS_SRC_FILE ${SRC_FILE}
                PATHS 

                ${gecko_sdk_SOURCE_DIR}/platform/Device/SiliconLabs/EFM32GG11B/Source
                ${gecko_sdk_SOURCE_DIR}/platform/Device/SiliconLabs/EFM32GG11B/Source/GCC

                CMAKE_FIND_ROOT_PATH_BOTH
            )

            if (BUILD_VERBOSE)
                message("${SRC_FILE} >> ${GECKO_SDK_SRCS_SRC_FILE}")
            endif()

            list(APPEND Gecko_SDK_SOURCES ${GECKO_SDK_SRCS_SRC_FILE})
            
        endforeach()

        list(REMOVE_DUPLICATES GECKO_SDK_SRCS_INCLUDE_DIRS)

        include(FindPackageHandleStandardArgs)

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
