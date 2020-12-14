#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.arm-none-eabi)
include(binutils.AzureRTOS)
include(nf_utils)

# Define PLATFORM base path
set(BASE_PATH_FOR_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

# check if RTOS_SOURCE_FOLDER was specified or if it's empty (default is empty)
set(NO_RTOS_SOURCE_FOLDER TRUE)
if(RTOS_SOURCE_FOLDER)
    if(NOT "${RTOS_SOURCE_FOLDER}" STREQUAL "")
        set(NO_RTOS_SOURCE_FOLDER FALSE)
    endif()
endif()

if(NO_RTOS_SOURCE_FOLDER)
    # no AzureRTOS source specified, download it from it's repo
    
    # check for Git (needed here for advanced warning to user if it's not installed)
    find_package(Git)

    #  check if Git was found, if not report to user and abort
    if(NOT GIT_EXECUTABLE)
        message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
    endif()

    # AzureRTOS version
    set(RTOS_VERSION_EMPTY TRUE)

    # check if build was requested with a specifc AzureRTOS version
    if(DEFINED RTOS_VERSION)
        if(NOT "${RTOS_VERSION}" STREQUAL "")
            set(RTOS_VERSION_EMPTY FALSE)
        endif()
    endif()

    # check if build was requested with a specifc AzureRTOS version
    if(RTOS_VERSION_EMPTY)
        # no AzureRTOS version actualy specified, must be empty which is fine, we'll default to a known good version
        set(RTOS_VERSION "v6.1.2_rel")
    endif()

    message(STATUS "RTOS is: AzureRTOS ${RTOS_VERSION} from GitHub repo")

    # need to setup a separate CMake project to download the code from the GitHub repository
    # otherwise it won't be available before the actual build step
    configure_file(${PROJECT_SOURCE_DIR}/CMake/AzureRTOS.CMakeLists.cmake.in
                    ${CMAKE_BINARY_DIR}/AzureRTOS_Download/CMakeLists.txt)

    # setup CMake project for AzureRTOS download
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/AzureRTOS_Download)

    # run build on AzureRTOS download CMake project to perform the download
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/AzureRTOS_Download)

else()
    # AzureRTOS source was specified

    # sanity check is source path exists
    if(EXISTS "${RTOS_SOURCE_FOLDER}/")
        message(STATUS "RTOS is: AzureRTOS (source from: ${RTOS_SOURCE_FOLDER})")

        # check if we already have the sources, no need to copy again
        NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/AzureRTOS_Source/ SOURCE_EXISTS)

        if(NOT ${SOURCE_EXISTS})
            file(COPY "${RTOS_SOURCE_FOLDER}/" DESTINATION "${CMAKE_BINARY_DIR}/AzureRTOS_Source/common/src") #TODO: is this correct?
        else()
            message(STATUS "Using local cache of AzureRTOS source from ${RTOS_SOURCE_FOLDER}")
        endif()

        set(AZURERTOS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/AzureRTOS_Source/common/inc) #TODO is this correct?
    else()
        message(FATAL_ERROR "Couldn't find AzureRTOS source at ${RTOS_SOURCE_FOLDER}/")
    endif()

endif()

# AzureRTOS common directories
add_subdirectory(_common)

# try to find board in the targets folder
# and set target base location and base path for class libs
# this has to be set before the class library modules are pulled in
if(EXISTS ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/ST/${TARGET_BOARD})

    # board found
    message(STATUS "Support for target board '${TARGET_BOARD}' found")

    # Define base path for the class libraries
    set(BASE_PATH_FOR_CLASS_LIBRARIES_MODULES ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/ST/nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/ST/${TARGET_BOARD})

    add_subdirectory(ST)

    # add TARGET board directory
    add_subdirectory(ST/${TARGET_BOARD})

# try to find board in the targets folder
elseif(EXISTS ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/ST/${TARGET_BOARD})

    # board found
    message(STATUS "Support for target board '${TARGET_BOARD}' found")

    # Define base path for the class libraries
    set(BASE_PATH_FOR_CLASS_LIBRARIES_MODULES ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/ST/nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/ST/${TARGET_BOARD})

    add_subdirectory(ST)

    # add TARGET board directory
    add_subdirectory(ST/${TARGET_BOARD})

else()

    # try to find board in the Community targets folder
    if(EXISTS ${PROJECT_SOURCE_DIR}/targets-community/AzureRTOS/ST/${TARGET_BOARD})

        # board found
        message(STATUS "Support for target board '${TARGET_BOARD}' found in Community targets")

        # Define base path for the class libraries
        set(BASE_PATH_FOR_CLASS_LIBRARIES_MODULES ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/ST/nanoCLR)

        # set target base location
        set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets-community/AzureRTOS/ST/${TARGET_BOARD})

        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ST)

        # add TARGET board directory from Community
        add_subdirectory(${PROJECT_SOURCE_DIR}/targets-community/AzureRTOS/ST/${TARGET_BOARD})

    # try to find STM board in the Community targets folder
    elseif(EXISTS ${PROJECT_SOURCE_DIR}/targets-community/AzureRTOS/ST/${TARGET_BOARD})

        # board found
        message(STATUS "Support for target board '${TARGET_BOARD}' found in Community targets")

        # Define base path for the class libraries
        set(BASE_PATH_FOR_CLASS_LIBRARIES_MODULES ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/ST/nanoCLR)

        # set target base location
        set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets-community/AzureRTOS/ST/${TARGET_BOARD})

        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ST)

        # add TARGET board directory from Community
        add_subdirectory(${PROJECT_SOURCE_DIR}/targets-community/AzureRTOS/ST/${TARGET_BOARD})

    else()

        # board NOT found in targets folder
        message(FATAL_ERROR "\n\nSorry but support for ${TARGET_BOARD} target is not available...\n\nYou can wait for it to be added or you might want to contribute and start working on a PR for it.\n\n")

    endif()

endif()

# Azure RTOS settings and inclusion of build system
set(THREADX_ARCH "cortex_m4")
set(THREADX_TOOLCHAIN "gnu")
set(TX_USER_FILE "${TARGET_BASE_LOCATION}/tx_user.h" CACHE STRING "Enable TX user configuration")

add_subdirectory(${CMAKE_BINARY_DIR}/AzureRTOS_Source AzureSource)
