#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.arm-none-eabi)
include(binutils.FreeRTOS)
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

if(NOT "${TOOL_SRECORD_PREFIX}" STREQUAL "")
    if(NOT EXISTS ${TOOL_SRECORD_PREFIX}/srec_cat.exe)
        message(STATUS "")
        message(STATUS "Couldn't find the srec_cat tool at the specified path: ${TOOL_SRECORD_PREFIX}/srec_cat.exe")
        message(STATUS "Make sure that the CMake option TOOL_SRECORD_PREFIX has the correct path.")
        message(STATUS "If you don't have this tool download it from https://sourceforge.net/projects/srecord/files/srecord-win32/")
        message(STATUS "")
        message(FATAL_ERROR "srec_cat tool not found")
    else()
        set(SRECORD_TOOL_AVAILABLE TRUE CACHE INTERNAL "srec_cat tool available")
    endif()
endif()

if(NO_RTOS_SOURCE_FOLDER)
    # no FreeRTOS source specified, download it from it's repo
    
    # check for Git (needed here for advanced warning to user if it's not installed)
    find_package(Git)

    #  check if Git was found, if not report to user and abort
    if(NOT GIT_EXECUTABLE)
        message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
    endif()

    # FreeRTOS version
    set(RTOS_VERSION_EMPTY TRUE)

    # check if build was requested with a specifc FreeRTOS version
    if(DEFINED RTOS_VERSION)
        if(NOT "${RTOS_VERSION}" STREQUAL "")
            set(RTOS_VERSION_EMPTY FALSE)
        endif()
    endif()

    # check if build was requested with a specifc FreeRTOS version
    if(RTOS_VERSION_EMPTY)
        # no FreeRTOS version actualy specified, must be empty which is fine, we'll default to a known good version
        set(RTOS_VERSION_TAG "V10.4.1-kernel-only")
    else()
        # set SVN tag
        set(RTOS_VERSION_TAG "${RTOS_VERSION}")
    endif()

    message(STATUS "RTOS is: FreeRTOS ${RTOS_VERSION_TAG} from GitHub repo")

    # need to setup a separate CMake project to download the code from the GitHub repository
    # otherwise it won't be available before the actual build step
    configure_file("${CMAKE_SOURCE_DIR}/CMake/FreeRTOS.CMakeLists.cmake.in"
    "${CMAKE_BINARY_DIR}/FreeRTOS_Download/CMakeLists.txt")

    # setup CMake project for FreeRTOS download
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/FreeRTOS_Download")

    # run build on FreeRTOS download CMake project to perform the download
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/FreeRTOS_Download")

    # add FreeRTOS as external project
    ExternalProject_Add( 
        FreeRTOS
        PREFIX FreeRTOS
        SOURCE_DIR ${CMAKE_BINARY_DIR}/FreeRTOS_Source
        GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
        GIT_TAG ${RTOS_VERSION}  # target specified branch
        GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
        TIMEOUT 10
        LOG_DOWNLOAD 1
        # Disable all other steps
        INSTALL_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
    )                     

else()
    # FreeRTOS source was specified

    # sanity check is source path exists
    if(EXISTS "${RTOS_SOURCE_FOLDER}/")
        message(STATUS "RTOS is: FreeRTOS (source from: ${RTOS_SOURCE_FOLDER})")

        # check if we already have the sources, no need to copy again
        NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/FreeRTOS_Source/ SOURCE_EXISTS)

        if(NOT ${SOURCE_EXISTS})
            file(COPY "${RTOS_SOURCE_FOLDER}/" DESTINATION "${CMAKE_BINARY_DIR}/FreeRTOS_Source/lib/FreeRTOS")
        else()
            message(STATUS "Using local cache of FreeRTOS source from ${RTOS_SOURCE_FOLDER}")
        endif()

        set(FREERTOS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/FreeRTOS_Source/lib/include)
    else()
        message(FATAL_ERROR "Couldn't find FreeRTOS source at ${RTOS_SOURCE_FOLDER}/")
    endif()

    # add FreeRTOS as external project
    ExternalProject_Add(
        FreeRTOS
        PREFIX FreeRTOS
        SOURCE_DIR ${CMAKE_BINARY_DIR}/FreeRTOS_Source
        # Disable all other steps
        INSTALL_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
    )        

    # get source dir for FreeRTOS CMake project
    ExternalProject_Get_Property(FreeRTOS SOURCE_DIR)

endif()

# check if CMSIS_SOURCE was specified or if it's empty (default is empty)
set(NO_CMSIS_SOURCE TRUE)
if(CMSIS_SOURCE)
    if(NOT "${CMSIS_SOURCE}" STREQUAL "")
        set(NO_CMSIS_SOURCE FALSE)
    endif()
endif()

if(NO_CMSIS_SOURCE)

    # check for Git (needed here for advanced warning to user if it's not installed)
    find_package(Git)

    #  check if Git was found, if not report to user and abort
    if(NOT GIT_EXECUTABLE)
        message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
    endif()

    # CMSIS version
    set(CMSIS_VERSION_EMPTY TRUE)

    # check if build was requested with a specifc CMSIS version
    if(DEFINED CMSIS_VERSION)
        if(NOT "${CMSIS_VERSION}" STREQUAL "")
            set(CMSIS_VERSION_EMPTY FALSE)
        endif()
    endif()

    # check if build was requested with a specifc CMSIS version
    if(CMSIS_VERSION_EMPTY)
        # no CMSIS version actualy specified, must be empty which is fine, we'll default to a known good version
        set(CMSIS_GIT_TAG "5.5.1")
    else()
        # set Git tag
        set(CMSIS_GIT_TAG "${CMSIS_VERSION}")
    endif()            

    message(STATUS "CMSIS v${CMSIS_GIT_TAG} from GitHub repo")

    # need to setup a separate CMake project to download the code from the GitHub repository
    # otherwise it won't be available before the actual build step
    configure_file("${CMAKE_SOURCE_DIR}/CMake/CMSIS.CMakeLists.cmake.in"
                "${CMAKE_BINARY_DIR}/CMSIS_Download/CMakeLists.txt")

    # setup CMake project for CMSIS download
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/CMSIS_Download")

                    
    # run build on CMSIS download CMake project to perform the download
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/CMSIS_Download")

    # add CMSIS as external project
    ExternalProject_Add( 
        CMSIS
        PREFIX CMSIS
        SOURCE_DIR ${CMAKE_BINARY_DIR}/CMSIS_Source
        GIT_REPOSITORY  https://github.com/ARM-software/CMSIS_5.git
        GIT_TAG ${CMSIS_GIT_TAG}  # target specified branch
        GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
        TIMEOUT 10
        LOG_DOWNLOAD 1
        # Disable all other steps
        INSTALL_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
    )  
else()
    # CMSIS source was specified

    # sanity check is source path exists
    if(EXISTS "${CMSIS_SOURCE}/")
        message(STATUS "CMSIS (source from: ${CMSIS_SOURCE})")

        # check if we already have the sources, no need to copy again
        NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/CMSIS_Source/ SOURCE_EXISTS)

        if(NOT ${SOURCE_EXISTS})
            file(COPY "${CMSIS_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/CMSIS_Source")
        else()
            message(STATUS "Using local cache of CMSIS source from ${CMSIS_SOURCE}")
        endif()

        set(CMSIS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/CMSIS_Source/include)
    else()
        message(FATAL_ERROR "Couldn't find CMSIS source at ${CMSIS_SOURCE}/")
    endif()

    # add CMSIS as external project
    ExternalProject_Add(
        CMSIS
        PREFIX CMSIS
        SOURCE_DIR ${CMAKE_BINARY_DIR}/CMSIS_Source
        # Disable all other steps
        INSTALL_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
    )        

    # get source dir for CMSIS CMake project
    ExternalProject_Get_Property(CMSIS SOURCE_DIR)

endif()

# FreeRTOS common directories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/common)

# try to find board in the targets folder
# and set target base location and base path for class libs
# this has to be set before the class library modules are pulled in
if(EXISTS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/NXP/${TARGET_BOARD})

    # board found
    message(STATUS "Support for target board '${TARGET_BOARD}' found")

    # Define base path for the class libraries
    SET_BASE_PATH_FOR_LIBRARIES_MODULES(${CMAKE_SOURCE_DIR}/targets/FreeRTOS/NXP/nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/NXP/${TARGET_BOARD})

    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/NXP)

    # add TARGET board directory
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/NXP/${TARGET_BOARD})

# try to find board in the targets folder
elseif(EXISTS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/ST/${TARGET_BOARD})

    # board found
    message(STATUS "Support for target board '${TARGET_BOARD}' found")

    # Define base path for the class libraries
    SET_BASE_PATH_FOR_LIBRARIES_MODULES(${CMAKE_SOURCE_DIR}/targets/FreeRTOS/ST/nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/ST/${TARGET_BOARD})

    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ST)

    # add TARGET board directory
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ST/${TARGET_BOARD})

else()

    # try to find NXP board in the Community targets folder
    if(EXISTS ${CMAKE_SOURCE_DIR}/targets-community/FreeRTOS/NXP/${TARGET_BOARD})

        # board found
        message(STATUS "Support for target board '${TARGET_BOARD}' found in Community targets")

        # Define base path for the class libraries
        SET_BASE_PATH_FOR_LIBRARIES_MODULES(${CMAKE_SOURCE_DIR}/targets/FreeRTOS/NXP/nanoCLR)

        # set target base location
        set(TARGET_BASE_LOCATION ${CMAKE_SOURCE_DIR}/targets-community/FreeRTOS/NXP/${TARGET_BOARD})

        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/NXP)

        # add TARGET board directory from Community
        add_subdirectory(${CMAKE_SOURCE_DIR}/targets-community/FreeRTOS/NXP/${TARGET_BOARD})

    # try to find STM board in the Community targets folder
    elseif(EXISTS ${CMAKE_SOURCE_DIR}/targets-community/FreeRTOS/ST/${TARGET_BOARD})

        # board found
        message(STATUS "Support for target board '${TARGET_BOARD}' found in Community targets")

        # Define base path for the class libraries
        SET_BASE_PATH_FOR_LIBRARIES_MODULES(${CMAKE_SOURCE_DIR}/targets/FreeRTOS/ST/nanoCLR)

        # set target base location
        set(TARGET_BASE_LOCATION ${CMAKE_SOURCE_DIR}/targets-community/FreeRTOS/ST/${TARGET_BOARD})

        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ST)

        # add TARGET board directory from Community
        add_subdirectory(${CMAKE_SOURCE_DIR}/targets-community/FreeRTOS/ST/${TARGET_BOARD})

    else()

        # board NOT found in targets folder
        message(FATAL_ERROR "\n\nSorry but support for ${TARGET_BOARD} target is not available...\n\nYou can wait for that to be added or you might want to contribute and start working on a PR for that.\n\n")

    endif()

endif()
