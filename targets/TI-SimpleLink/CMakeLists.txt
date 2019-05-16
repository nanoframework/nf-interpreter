#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# set target base location
# this has to be set before the class library modules are pulled in
if(EXISTS ${PROJECT_SOURCE_DIR}/targets/TI-SimpleLink/${TI_BOARD})
    # set target base location
    set(TARGET_BASE_LOCATION "${PROJECT_SOURCE_DIR}/targets/TI-SimpleLink/${TI_BOARD}")
    message(STATUS "Support for target board '${TI_BOARD}' found")
else()
    # board NOT found
    # can't continue
    message(FATAL_ERROR "\n\nSorry but support for ${TI_BOARD} target is not available...\n\nYou can wait for that to be added or you might want to contribute and start working on a PR for that.\n\n")
endif()

# Define base path for the class libraries
set(BASE_PATH_FOR_CLASS_LIBRARIES_MODULES "${PROJECT_SOURCE_DIR}/targets/TI-SimpleLink/nanoCLR")

# add target TI SimpleLink dirs
add_subdirectory(common)
add_subdirectory(Include)
add_subdirectory(nanoCLR)

if("${TARGET_SERIES}" STREQUAL "CC32xx")
    
    # CC32xx series is using TI SimpleLink CC32xx SDK and FreeRTOS
    
    # check if TI_SL_CC32xx_SDK_SOURCE was specified or if it's empty (default is empty)
    set(NO_TI_SL_CC32xx_SDK_SOURCE TRUE)
    if(TI_SL_CC32xx_SDK_SOURCE)
        if(NOT "${TI_SL_CC32xx_SDK_SOURCE}" STREQUAL "")
            set(NO_TI_SL_CC32xx_SDK_SOURCE FALSE)
        endif()
    endif()

    if(NO_TI_SL_CC32xx_SDK_SOURCE)
        # no TI SimpleLink CC32xx SDK source specified, download it from our repo

        # hack to make the FindGit to work in Windows platforms (check the module comment for details)
        include(Hack_SetGitSearchPath)

        # check for Git (needed here for advanced warning to user if it's not installed)
        find_package(Git)

        #  check if Git was found, if not report to user and abort
        if(NOT GIT_EXECUTABLE)
            message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
        endif()

        message(STATUS "TI SimpleLink CC32xx SDK (latest available code from v3.10.00.04")
        set(TI_SL_CC32xx_SDK_TAG "3.10.00.04")

        # need to setup a separate CMake project to download the code from the GitHub repository
        # otherwise it won't be available before the actual build step
        configure_file("${PROJECT_SOURCE_DIR}/CMake/SimpleLinkCC32xxSdk.CMakeLists.cmake.in"
                    "${CMAKE_BINARY_DIR}/SimpleLinkCC32xxSdk_Download/CMakeLists.txt")

        # setup CMake project for TI SimpleLink CC32xx SDK download
        execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                        RESULT_VARIABLE result
                        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/SimpleLinkCC32xxSdk_Download")

        # run build on TI SimpleLink CC32xx SDK download CMake project to perform the download
        execute_process(COMMAND ${CMAKE_COMMAND} --build .
                        RESULT_VARIABLE result
                        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/SimpleLinkCC32xxSdk_Download")

        # add TI SimpleLink CC32xx SDK as external project
        ExternalProject_Add(
            SimpleLinkCC32xxSDK
            PREFIX SimpleLinkCC32xxSdk
            SOURCE_DIR ${CMAKE_BINARY_DIR}/SimpleLinkCC32xxSdk_Source
            GIT_REPOSITORY  https://github.com/nanoframework/SimpleLink_CC32xx_SDK
            GIT_TAG ${TI_SL_CC32xx_SDK_TAG}  # target specified branch
            GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
            TIMEOUT 10
            LOG_DOWNLOAD 1

            # Disable all other steps
            INSTALL_COMMAND ""
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
        )

        # get source dir for TI SimpleLink CC32xx SDK CMake project
        ExternalProject_Get_Property(SimpleLinkCC32xxSDK SOURCE_DIR)

    else()
        # TI SimpleLink CC32xx SDK source was specified

        # sanity check is source path exists
        if(EXISTS "${TI_SL_CC32xx_SDK_SOURCE}/")
            message(STATUS "TI SimpleLink CC32xx SDK source from: ${TI_SL_CC32xx_SDK_SOURCE}")

            # check if we already have the sources, no need to copy again
            if(NOT EXISTS "${CMAKE_BINARY_DIR}/TI_SL_CC32xx_SDK_SOURCE")
                file(COPY "${TI_SL_CC32xx_SDK_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/SimpleLinkCC32xxSdk_Source")
            else()
                message(STATUS "Using local cache of TI SimpleLink CC32xx SDK source from ${TI_SL_CC32xx_SDK_SOURCE}")
            endif()

            set(SimpleLinkCC32xxSDK_INCLUDE_DIR ${CMAKE_BINARY_DIR}/SimpleLinkCC32xxSdkS_Source/ti/devices/cc32xx)
        else()
            message(FATAL_ERROR "Couldn't find TI SimpleLink CC32xx SDK source at ${TI_SL_CC32xx_SDK_SOURCE}/")
        endif()

        # add TI SimpleLink CC32xx SDK as external project
        ExternalProject_Add(
            SimpleLinkCC32xxSDK
            PREFIX SimpleLinkCC32xxSdk
            SOURCE_DIR ${CMAKE_BINARY_DIR}/SimpleLinkCC32xxSdk_Source

            # Disable all other steps
            INSTALL_COMMAND ""
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
        )

        # get source dir for TI SimpleLink CC32xx SDK CMake project
        ExternalProject_Get_Property(SimpleLinkCC32xxSDK SOURCE_DIR)
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
        
        # check for SVN (needed here for advanced warning to user if it's not installed)
        find_package(Subversion)

        #  check if SVN was found, if not report to user and abort
        if(NOT SUBVERSION_FOUND)
            message(FATAL_ERROR "error: could not find SVN, make sure you have it installed.")
        endif()

        # FreeRTOS version
        set(FREERTOS_VERSION_EMPTY TRUE)

        # check if build was requested with a specifc FreeRTOS version
        if(DEFINED FREERTOS_VERSION)
            if(NOT "${FREERTOS_VERSION}" STREQUAL "")
                set(FREERTOS_VERSION_EMPTY FALSE)
            endif()
        endif()

        # FreeRTOS version
        set(FREERTOS_VERSION_EMPTY TRUE)

        # check if build was requested with a specifc FreeRTOS version
        if(FREERTOS_VERSION_EMPTY)
            # no FreeRTOS version actualy specified, must be empty which is fine, we'll default to a known good version
            set(FREERTOS_VERSION_TAG "V10.1.1")
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
            SVN_REPOSITORY https://svn.code.sf.net/p/freertos/code/tags/${FREERTOS_VERSION_TAG}/FreeRTOS/Source/
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
                file(COPY "${FREERTOS_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/FreeRTOS_Source")
            else()
                message(STATUS "Using local cache of FreeRTOS source from ${FREERTOS_SOURCE}")
            endif()

            set(FREERTOS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/FreeRTOS_Source/include)
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

    add_subdirectory(${TI_BOARD})

endif()
