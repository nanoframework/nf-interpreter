#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.arm-none-eabi)
include(binutils.FreeRTOS)
include(nf_utils)

# check if FREERTOS_SOURCE was specified or if it's empty (default is empty)
set(NO_FREERTOS_SOURCE TRUE)
if(FREERTOS_SOURCE)
    if(NOT "${FREERTOS_SOURCE}" STREQUAL "")
        set(NO_FREERTOS_SOURCE FALSE)
    endif()
endif()

if(NOT "${TOOL_SRECORD_PREFIX}" STREQUAL "")
    if(NOT EXISTS ${TOOL_SRECORD_PREFIX}/srec_cat.exe)
        message(STATUS "")
        message(STATUS "Couldn't find the srec_cat tool at the specified path: ${TOOL_SRECORD_PREFIX}/srec_cat.exe")
        message(STATUS "Make sure that the CMake option TOOL_SRECORD_PREFIX has the correct path.")
        message(STATUS "If you don't have this tool download it from hhttps://sourceforge.net/projects/srecord/files/srecord-win32/")
        message(STATUS "")
        message(FATAL_ERROR "srec_cat tool not found")
    else()
        set(SRECORD_TOOL_AVAILABLE TRUE CACHE INTERNAL "srec_cat tool available")
    endif()
endif()

if(NO_FREERTOS_SOURCE)
    # no FreeRTOS source specified, download it from it's repo
    
    # check for Git (needed here for advanced warning to user if it's not installed)
    find_package(Git)

    #  check if Git was found, if not report to user and abort
    if(NOT GIT_EXECUTABLE)
        message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
    endif()

    # FreeRTOS version
    set(FREERTOS_VERSION_EMPTY TRUE)

    # check if build was requested with a specifc FreeRTOS version
    if(DEFINED FREERTOS_VERSION)
        if(NOT "${FREERTOS_VERSION}" STREQUAL "")
            set(FREERTOS_VERSION_EMPTY FALSE)
        endif()
    endif()

    # check if build was requested with a specifc FreeRTOS version
    if(FREERTOS_VERSION_EMPTY)
        # no FreeRTOS version actualy specified, must be empty which is fine, we'll default to a known good version
        set(FREERTOS_VERSION_TAG "V10.2.1")
    else()
        # set SVN tag
        set(FREERTOS_VERSION_TAG "${FREERTOS_VERSION}")
    endif()

    message(STATUS "RTOS is: FreeRTOS ${FREERTOS_VERSION} from GitHub repo")

    # need to setup a separate CMake project to download the code from the GitHub repository
    # otherwise it won't be available before the actual build step
    configure_file("${PROJECT_SOURCE_DIR}/CMake/FreeRTOS.CMakeLists.cmake.in"
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
        GIT_TAG ${FREERTOS_VERSION_TAG}  # target specified branch
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
    if(EXISTS "${FREERTOS_SOURCE}/")
        message(STATUS "RTOS is: FreeRTOS (source from: ${FREERTOS_SOURCE})")

        # check if we already have the sources, no need to copy again
        NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/FreeRTOS_Source SOURCE_EXISTS)

        if(NOT ${SOURCE_EXISTS})
            file(COPY "${FREERTOS_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/FreeRTOS_Source/lib/FreeRTOS")
        else()
            message(STATUS "Using local cache of FreeRTOS source from ${FREERTOS_SOURCE}")
        endif()

        set(FREERTOS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/FreeRTOS_Source/lib/include)
    else()
        message(FATAL_ERROR "Couldn't find FreeRTOS source at ${FREERTOS_SOURCE}/")
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

    message(STATUS "CMSIS v${CMSIS_VERSION} from GitHub repo")

    # need to setup a separate CMake project to download the code from the GitHub repository
    # otherwise it won't be available before the actual build step
    configure_file("${PROJECT_SOURCE_DIR}/CMake/CMSIS.CMakeLists.cmake.in"
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
        message(STATUS "CMSIS ${CMSIS_VERSION} (source from: ${CMSIS_SOURCE})")

        # check if we already have the sources, no need to copy again
        NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/CMSIS_Source SOURCE_EXISTS)

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
if(EXISTS ${PROJECT_SOURCE_DIR}/targets/FreeRTOS/NXP/${FREERTOS_BOARD})

    # board found
    message(STATUS "Support for target board '${FREERTOS_BOARD}' found")

    # Define base path for the class libraries
    set(BASE_PATH_FOR_CLASS_LIBRARIES_MODULES ${PROJECT_SOURCE_DIR}/targets/FreeRTOS/NXP/nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets/FreeRTOS/NXP/${FREERTOS_BOARD})

    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/NXP)

    # add TARGET board directory
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/NXP/${FREERTOS_BOARD})

# try to find board in the targets folder
elseif(EXISTS ${PROJECT_SOURCE_DIR}/targets/FreeRTOS/ST/${FREERTOS_BOARD})

    # board found
    message(STATUS "Support for target board '${FREERTOS_BOARD}' found")

    # Define base path for the class libraries
    set(BASE_PATH_FOR_CLASS_LIBRARIES_MODULES ${PROJECT_SOURCE_DIR}/targets/FreeRTOS/ST/nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets/FreeRTOS/ST/${FREERTOS_BOARD})

    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ST)

    # add TARGET board directory
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ST/${FREERTOS_BOARD})

else()

    # try to find NXP board in the Community targets folder
    if(EXISTS ${PROJECT_SOURCE_DIR}/targets-community/FreeRTOS/NXP/${FREERTOS_BOARD})

        # board found
        message(STATUS "Support for target board '${FREERTOS_BOARD}' found in Community targets")

        # Define base path for the class libraries
        set(BASE_PATH_FOR_CLASS_LIBRARIES_MODULES ${PROJECT_SOURCE_DIR}/targets/FreeRTOS/NXP/nanoCLR)

        # set target base location
        set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets-community/FreeRTOS/NXP/${FREERTOS_BOARD})

        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/NXP)

        # add TARGET board directory from Community
        add_subdirectory(${PROJECT_SOURCE_DIR}/targets-community/FreeRTOS/NXP/${FREERTOS_BOARD})

    # try to find STM board in the Community targets folder
    elseif(EXISTS ${PROJECT_SOURCE_DIR}/targets-community/FreeRTOS/ST/${FREERTOS_BOARD})

        # board found
        message(STATUS "Support for target board '${FREERTOS_BOARD}' found in Community targets")

        # Define base path for the class libraries
        set(BASE_PATH_FOR_CLASS_LIBRARIES_MODULES ${PROJECT_SOURCE_DIR}/targets/FreeRTOS/ST/nanoCLR)

        # set target base location
        set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets-community/FreeRTOS/ST/${FREERTOS_BOARD})

        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ST)

        # add TARGET board directory from Community
        add_subdirectory(${PROJECT_SOURCE_DIR}/targets-community/FreeRTOS/ST/${FREERTOS_BOARD})

    else()

        # board NOT found in targets folder
        message(FATAL_ERROR "\n\nSorry but support for ${FREERTOS_BOARD} target is not available...\n\nYou can wait for that to be added or you might want to contribute and start working on a PR for that.\n\n")

    endif()

endif()
