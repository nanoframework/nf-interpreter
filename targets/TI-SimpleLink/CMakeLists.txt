#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.arm-none-eabi)
include(binutils.TI_SimpleLink)
include(nf_utils)

# add target TI SimpleLink dirs
add_subdirectory(common)
add_subdirectory(Include)
add_subdirectory(nanoCLR)

########################################
if("${TARGET_SERIES}" STREQUAL "CC32xx")
    
    # CC32xx series is using TI SimpleLink CC32xx SDK and TI RTOS
    
    # check if TI_SL_CC32xx_SDK_SOURCE was specified or if it's empty (default is empty)
    set(NO_TI_SL_CC32xx_SDK_SOURCE TRUE)
    if(TI_SL_CC32xx_SDK_SOURCE)
        if(NOT "${TI_SL_CC32xx_SDK_SOURCE}" STREQUAL "")
            set(NO_TI_SL_CC32xx_SDK_SOURCE FALSE)
        endif()
    endif()

    if(NO_TI_SL_CC32xx_SDK_SOURCE)
        # no TI SimpleLink CC32xx SDK source specified, download it from our repo

        # check for Git (needed here for advanced warning to user if it's not installed)
        find_package(Git)

        #  check if Git was found, if not report to user and abort
        if(NOT GIT_EXECUTABLE)
            message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
        endif()

        message(STATUS "TI SimpleLink CC32xx SDK (latest available code from v3.10.00.04")
        set(TI_SL_CC32xx_SDK_TAG "4.10.00.07")

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
            NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/TI_SL_CC32xx_SDK_SOURCE/ SOURCE_EXISTS)

            if(NOT ${SOURCE_EXISTS})
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
    
    # check if TI_XDCTOOLS_SOURCE was specified or if it's empty (default is empty)
    set(NO_TI_XDCTOOLS_SOURCE TRUE)
    if(TI_XDCTOOLS_SOURCE)
        if(NOT "${TI_XDCTOOLS_SOURCE}" STREQUAL "")
            set(NO_TI_XDCTOOLS_SOURCE FALSE)
        endif()
    endif()
    if(NO_TI_XDCTOOLS_SOURCE)
        # no TI XDCTools source specified, download it from it's repo
        
        # check for Git (needed here for advanced warning to user if it's not installed)
        find_package(Git)

        #  check if Git was found, if not report to user and abort
        if(NOT GIT_EXECUTABLE)
            message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
        endif()

        # TI XDCTools version
        set(TI_XDCTOOLS_VERSION_EMPTY TRUE)

        # check if build was requested with a specifc TI XDCTools version
        if(DEFINED TI_XDCTOOLS_VERSION)
            if(NOT "${TI_XDCTOOLS_VERSION}" STREQUAL "")
                set(TI_XDCTOOLS_VERSION_EMPTY FALSE)
            endif()
        endif()

        # TI XDCTools version
        set(TI_XDCTOOLS_VERSION_EMPTY TRUE)

        # check if build was requested with a specifc TI XDCTools version
        if(TI_XDCTOOLS_VERSION_EMPTY)
            # no TI XDCTools version actualy specified, must be empty which is fine, we'll default to a known good version
            set(TI_XDCTOOLS_VERSION_TAG "3.61.00.16")
        else()
            # set SVN tag
            set(TI_XDCTOOLS_VERSION_TAG "${TI_XDCTOOLS_VERSION}")
        endif()

        message(STATUS "TI XDCTools ${TI_XDCTOOLS_VERSION} from GitHub repo")

        # need to setup a separate CMake project to download the code from the GitHub repository
        # otherwise it won't be available before the actual build step
        configure_file("${PROJECT_SOURCE_DIR}/CMake/TI_XDCTools.CMakeLists.cmake.in"
        "${CMAKE_BINARY_DIR}/TI_XDCTools_Download/CMakeLists.txt")

        # setup CMake project for TI XDCTools download
        execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                        RESULT_VARIABLE result
                        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/TI_XDCTools_Download")

        # run build on TI XDCTools download CMake project to perform the download
        execute_process(COMMAND ${CMAKE_COMMAND} --build .
                        RESULT_VARIABLE result
                        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/TI_XDCTools_Download")

        # add TI XDCTools as external project
        ExternalProject_Add(
            TI_XDCTools
            PREFIX TI_XDCTools
            SOURCE_DIR ${CMAKE_BINARY_DIR}/TI_XDCTools_Source
            GIT_REPOSITORY  https://github.com/nanoframework/TI_XDCTools
            GIT_TAG ${TI_XDCTools_TAG}  # target specified branch
            GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
            TIMEOUT 10
            LOG_DOWNLOAD 1
        
            # Disable all other steps
            INSTALL_COMMAND ""
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
        )  

        # get source dir for TI XDCTools CMake project
        ExternalProject_Get_Property(TI_XDCTools SOURCE_DIR)
        set(TI_XDCTools_INCLUDE_DIR "${CMAKE_BINARY_DIR}/TI_XDCTools_Source/packages" PARENT_SCOPE)

    else()
        # TI XDCTools source was specified

        # sanity check is source path exists
        if(EXISTS "${TI_XDCTOOLS_SOURCE}/")
            message(STATUS "TI XDCTools (source from: ${TI_XDCTOOLS_SOURCE})")

            # check if we already have the sources, no need to copy again
            NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/TI_XDCTools_Source/ SOURCE_EXISTS)

            if(NOT ${SOURCE_EXISTS})
                file(COPY "${TI_XDCTOOLS_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/TI_XDCTools_Source")
            else()
                message(STATUS "Using local cache of TI XDCTools source from ${TI_XDCTOOLS_SOURCE}")
            endif()

        else()
            message(FATAL_ERROR "Couldn't find TI XDCTools source at ${TI_XDCTOOLS_SOURCE}/")
        endif()

        # add TI XDCTools as external project
        ExternalProject_Add(
            TI_XDCTools
            PREFIX TI_XDCTools
            SOURCE_DIR ${CMAKE_BINARY_DIR}/TI_XDCTools_Source
            # Disable all other steps
            INSTALL_COMMAND ""
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
        )        

        # get source dir for TI XDCTools CMake project
        ExternalProject_Get_Property(TI_XDCTools SOURCE_DIR)
        set(TI_XDCTools_INCLUDE_DIR ${CMAKE_BINARY_DIR}/TI_XDCTools_Source/packages PARENT_SCOPE)

    endif()

    # need to override the toolchain lib directory
    set(TOOLCHAIN_INC_DIR ${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib)
    set(TOOLCHAIN_LIB_DIR ${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib)
    
############################################
elseif( "${TARGET_SERIES}" STREQUAL "CC13x2_26x2" )
    
    # CC13x2 and CC26x2 series are using TI SimpleLink CC13x2_26x2 SDK and TI RTOS

    # check if TI_SL_CC13x2_26x2_SDK_SOURCE was specified or if it's empty (default is empty)
    set(NO_TI_SL_CC13x2_26x2_SDK_SOURCE TRUE)
    if(TI_SL_CC13x2_26x2_SDK_SOURCE)
        if(NOT "${TI_SL_CC13x2_26x2_SDK_SOURCE}" STREQUAL "")
            set(NO_TI_SL_CC13x2_26x2_SDK_SOURCE FALSE)
        endif()
    endif()

    if(NO_TI_SL_CC13x2_26x2_SDK_SOURCE)
        # no TI SimpleLink CC13x2_26x2 SDK source specified, download it from our repo

        # check for Git (needed here for advanced warning to user if it's not installed)
        find_package(Git)

        #  check if Git was found, if not report to user and abort
        if(NOT GIT_EXECUTABLE)
            message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
        endif()

        message(STATUS "TI SimpleLink CC13x2_26x2 SDK (latest available code from v3.20.00.68")
        set(TI_SL_CC13x2_26x2_SDK_TAG "3.20.00.68")

        # need to setup a separate CMake project to download the code from the GitHub repository
        # otherwise it won't be available before the actual build step
        configure_file("${PROJECT_SOURCE_DIR}/CMake/SimpleLinkCC13x2_26x2Sdk.CMakeLists.cmake.in"
                    "${CMAKE_BINARY_DIR}/SimpleLinkCC13x2_26x2Sdk_Download/CMakeLists.txt")

        # setup CMake project for TI SimpleLink CC13x2_26x2 SDK download
        execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                        RESULT_VARIABLE result
                        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/SimpleLinkCC13x2_26x2Sdk_Download")

        # run build on TI SimpleLink CC13x2_26x2 SDK download CMake project to perform the download
        execute_process(COMMAND ${CMAKE_COMMAND} --build .
                        RESULT_VARIABLE result
                        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/SimpleLinkCC13x2_26x2Sdk_Download")

        # add TI SimpleLink CC13x2_26x2 SDK as external project
        ExternalProject_Add(
            SimpleLinkCC13x2_26x2SDK
            PREFIX SimpleLinkCC13x2_26x2Sdk
            SOURCE_DIR ${CMAKE_BINARY_DIR}/SimpleLinkCC13x2_26x2Sdk_Source
            GIT_REPOSITORY  https://github.com/nanoframework/SimpleLink_CC13x2_26x2_SDK
            GIT_TAG ${TI_SL_CC32xx_SDK_TAG}  # target specified branch
            GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
            TIMEOUT 10
            LOG_DOWNLOAD 1

            # Disable all other steps
            INSTALL_COMMAND ""
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
        )

        # get source dir for TI SimpleLink CC13x2_26x2 SDK CMake project
        ExternalProject_Get_Property(SimpleLinkCC13x2_26x2SDK SOURCE_DIR)

    else()
        # TI SimpleLink CC13x2_26x2 SDK source was specified

        # sanity check is source path exists
        if(EXISTS "${TI_SL_CC13x2_26x2_SDK_SOURCE}/")
            message(STATUS "TI SimpleLink CC13x2_26x2 SDK source from: ${TI_SL_CC13x2_26x2_SDK_SOURCE}")

            # check if we already have the sources, no need to copy again
            NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/TI_SL_CC13x2_26x2_SDK_SOURCE/ SOURCE_EXISTS)

            if(NOT ${SOURCE_EXISTS})
                file(COPY "${TI_SL_CC13x2_26x2_SDK_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/SimpleLinkCC13x2_26x2Sdk_Source")
            else()
                message(STATUS "Using local cache of TI SimpleLink CC13x2_26x2 SDK source from ${TI_SL_CC13x2_26x2_SDK_SOURCE}")
            endif()

            set(SimpleLinkCC13x2_26x2SDK_INCLUDE_DIR ${CMAKE_BINARY_DIR}/SimpleLinkCC13x2_26x2SdkS_Source/ti/devices/cc32xx)
        else()
            message(FATAL_ERROR "Couldn't find TI SimpleLink CC13x2_26x2 SDK source at ${TI_SL_CC13x2_26x2_SDK_SOURCE}/")
        endif()

        # add TI SimpleLink CC13x2_26x2 SDK as external project
        ExternalProject_Add(
            SimpleLinkCC13x2_26x2SDK
            PREFIX SimpleLinkCC13x2_26x2Sdk
            SOURCE_DIR ${CMAKE_BINARY_DIR}/SimpleLinkCC13x2_26x2Sdk_Source

            # Disable all other steps
            INSTALL_COMMAND ""
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
        )

        # get source dir for TI SimpleLink CC13x2_26x2 SDK CMake project
        ExternalProject_Get_Property(SimpleLinkCC13x2_26x2SDK SOURCE_DIR)
    endif()
    
    # check if TI_XDCTOOLS_SOURCE was specified or if it's empty (default is empty)
    set(NO_TI_XDCTOOLS_SOURCE TRUE)
    if(TI_XDCTOOLS_SOURCE)
        if(NOT "${TI_XDCTOOLS_SOURCE}" STREQUAL "")
            set(NO_TI_XDCTOOLS_SOURCE FALSE)
        endif()
    endif()

    if(NO_TI_XDCTOOLS_SOURCE)
        # no TI XDCTools source specified, download it from it's repo
        
        # check for Git (needed here for advanced warning to user if it's not installed)
        find_package(Git)

        #  check if Git was found, if not report to user and abort
        if(NOT GIT_EXECUTABLE)
            message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
        endif()

        # TI XDCTools version
        set(TI_XDCTOOLS_VERSION_EMPTY TRUE)

        # check if build was requested with a specifc TI XDCTools version
        if(DEFINED TI_XDCTOOLS_VERSION)
            if(NOT "${TI_XDCTOOLS_VERSION}" STREQUAL "")
                set(TI_XDCTOOLS_VERSION_EMPTY FALSE)
            endif()
        endif()

        # TI XDCTools version
        set(TI_XDCTOOLS_VERSION_EMPTY TRUE)

        # check if build was requested with a specifc TI XDCTools version
        if(TI_XDCTOOLS_VERSION_EMPTY)
            # no TI XDCTools version actualy specified, must be empty which is fine, we'll default to a known good version
            set(TI_XDCTOOLS_VERSION_TAG "3.61.00.16")
        else()
            # set SVN tag
            set(TI_XDCTOOLS_VERSION_TAG "${TI_XDCTOOLS_VERSION}")
        endif()

        message(STATUS "TI XDCTools ${TI_XDCTOOLS_VERSION} from GitHub repo")

        # need to setup a separate CMake project to download the code from the GitHub repository
        # otherwise it won't be available before the actual build step
        configure_file("${PROJECT_SOURCE_DIR}/CMake/TI_XDCTools.CMakeLists.cmake.in"
        "${CMAKE_BINARY_DIR}/TI_XDCTools_Download/CMakeLists.txt")

        # setup CMake project for TI XDCTools download
        execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                        RESULT_VARIABLE result
                        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/TI_XDCTools_Download")

        # run build on TI XDCTools download CMake project to perform the download
        execute_process(COMMAND ${CMAKE_COMMAND} --build .
                        RESULT_VARIABLE result
                        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/TI_XDCTools_Download")

        # add TI XDCTools as external project
        ExternalProject_Add(
            TI_XDCTools
            PREFIX TI_XDCTools
            SOURCE_DIR ${CMAKE_BINARY_DIR}/TI_XDCTools_Source
            GIT_REPOSITORY  https://github.com/nanoframework/TI_XDCTools
            GIT_TAG ${TI_XDCTools_TAG}  # target specified branch
            GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
            TIMEOUT 10
            LOG_DOWNLOAD 1
        
            # Disable all other steps
            INSTALL_COMMAND ""
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
        )  

        # get source dir for TI XDCTools CMake project
        ExternalProject_Get_Property(TI_XDCTools SOURCE_DIR)
        set(TI_XDCTools_INCLUDE_DIR ${CMAKE_BINARY_DIR}/TI_XDCTools_Source/packages PARENT_SCOPE)

    else()
        # TI XDCTools source was specified

        # sanity check is source path exists
        if(EXISTS "${TI_XDCTOOLS_SOURCE}/")
            message(STATUS "TI XDCTools (source from: ${TI_XDCTOOLS_SOURCE})")

            # check if we already have the sources, no need to copy again
            NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/TI_XDCTools_Source/ SOURCE_EXISTS)

            if(NOT ${SOURCE_EXISTS})
                file(COPY "${TI_XDCTOOLS_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/TI_XDCTools_Source")
            else()
                message(STATUS "Using local cache of TI XDCTools source from ${TI_XDCTOOLS_SOURCE}")
            endif()

        else()
            message(FATAL_ERROR "Couldn't find TI XDCTools source at ${TI_XDCTOOLS_SOURCE}/")
        endif()

        # add TI XDCTools as external project
        ExternalProject_Add(
            TI_XDCTools
            PREFIX TI_XDCTools
            SOURCE_DIR ${CMAKE_BINARY_DIR}/TI_XDCTools_Source
            # Disable all other steps
            INSTALL_COMMAND ""
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
        )        

        # get source dir for TI XDCTools CMake project
        ExternalProject_Get_Property(TI_XDCTools SOURCE_DIR)
        set(TI_XDCTools_INCLUDE_DIR "${CMAKE_BINARY_DIR}/TI_XDCTools_Source/packages" PARENT_SCOPE)

    endif()

    # need to override the toolchain lib directory
    set(TOOLCHAIN_INC_DIR ${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib)
    set(TOOLCHAIN_LIB_DIR ${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib)

endif()
