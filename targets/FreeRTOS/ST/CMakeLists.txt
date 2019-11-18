#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

include(STM32_CubePackage)

# (default is OFF so STM Cube package is NOT included)
option(STM32_CUBE_PACKAGE_REQUIRED "option to include STM Cube package in the build")

if(STM32_CUBE_PACKAGE_REQUIRED)
    ProcessSTM32CubePackage()
endif()

# check if FREERTOS_SOURCE was specified or if it's empty (default is empty)
set(NO_FREERTOS_SOURCE TRUE)
if(FREERTOS_SOURCE)
    if(NOT "${FREERTOS_SOURCE}" STREQUAL "")
        set(NO_FREERTOS_SOURCE FALSE)
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
        if(NOT EXISTS "${CMAKE_BINARY_DIR}/FreeRTOS_Source")
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

# ST common directories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/common)
