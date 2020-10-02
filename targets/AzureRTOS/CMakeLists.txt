#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.arm-none-eabi)
include(STM32_CubePackage)
include(nf_utils)

########################################################
# check availability of hex2dfu tool if specified
if(DEFINED TOOL_HEX2DFU_PREFIX)
    if(NOT EXISTS ${TOOL_HEX2DFU_PREFIX}/hex2dfu.exe)
        message(STATUS "")
        message(STATUS "Couldn't find the hex2dfu tool at the specified path: ${TOOL_HEX2DFU_PREFIX}/hex2dfu.exe")
        message(STATUS "Make sure that the CMake option TOOL_HEX2DFU_PREFIX has the correct path.")
        message(STATUS "If you don't have this tool download it from https://github.com/nanoframework/nf-tools/releases")
        message(STATUS "")
        message(FATAL_ERROR "hex2dfu tool not found")
    else()
        set(HEX2DFU_TOOL_AVAILABLE TRUE CACHE INTERNAL "hex2dfu tool available")
    endif()
endif()

# check if AZURERTOS_SOURCE was specified or if it's empty (default is empty)
set(NO_AZURERTOS_SOURCE TRUE)
if(AZURERTOS_SOURCE)
    if(NOT "${AZURERTOS_SOURCE}" STREQUAL "")
        set(NO_AZURERTOS_SOURCE FALSE)
    endif()
endif()

if(NO_AZURERTOS_SOURCE)
    # no AZURERTOS source specified, download it from it's repo

    # check for Git (needed here for advanced warning to user if it's not installed)
    find_package(Git)

    #  check if Git was found, if not report to user and abort
    if(NOT GIT_EXECUTABLE)
        message(FATAL_ERROR "Could not find Git, make sure you have it installed.")
    endif()

    # AZURERTOS version
    set(AZURERTOS_GIT_TAG "master") #TODO: use the master branch for the moment, but we should use a tag, e.g. V6.1_rel
    message(STATUS "RTOS is: AZURERTOS (latest available code from ${AZURERTOS_GIT_TAG})")

    # need to setup a separate CMake project to download the code from the GitHub repository
    # otherwise it won't be available before the actual build step
    configure_file("${PROJECT_SOURCE_DIR}/CMake/AzureRTOS.CMakeLists.cmake.in"
                "${CMAKE_BINARY_DIR}/AzureRTOS_Download/CMakeLists.txt")

    # setup CMake project for AZURERTOS download
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/AzureRTOS_Download")

    # run build on AZURERTOS download CMake project to perform the download
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/AzureRTOS_Download")

    # add AZURERTOS as external project
    ExternalProject_Add(
        AzureRTOS
        PREFIX AzureRTOS
        SOURCE_DIR ${CMAKE_BINARY_DIR}/AzureRTOS_Source
        GIT_REPOSITORY  https://github.com/azure-rtos/threadx
        GIT_TAG ${AZURERTOS_GIT_TAG}  # target specified branch
        GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
        TIMEOUT 10
        LOG_DOWNLOAD 1

        # Disable all other steps
        INSTALL_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
    )

    # get source dir for AzureRTOS CMake project
    ExternalProject_Get_Property(AzureRTOS SOURCE_DIR)

else()
    # AzureRTOS source was specified

    # sanity check is source path exists
    if(EXISTS "${AZURERTOS_SOURCE}/")
        message(STATUS "RTOS is: AzureRTOS (source from: ${AZURERTOS_SOURCE})")

        # check if we already have the sources, no need to copy again
        NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/AzureRTOS_Source/ SOURCE_EXISTS)

        if(NOT ${SOURCE_EXISTS})
            file(COPY "${AZURERTOS_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/AzureRTOS_Source")
        else()
            message(STATUS "Using local cache of AzureRTOS source from ${AZURERTOS_SOURCE}")
        endif()

        set(AZURERTOS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/AzureRTOS_Source/include)
    else()
        message(FATAL_ERROR "Couldn't find AzureRTOS source at ${AZURERTOS_SOURCE}/")
    endif()

    # add AzureRTOS as external project
    ExternalProject_Add(
        AzureRTOS
        PREFIX AzureRTOS
        SOURCE_DIR ${CMAKE_BINARY_DIR}/AzureRTOS_Source
        
        # Disable all other steps
        INSTALL_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
    )

    # get source dir for AzureRTOS CMake project
    ExternalProject_Get_Property(AzureRTOS SOURCE_DIR)

endif()


# # include FatFS if SDCard or USB MSD are ON
# if(NF_FEATURE_HAS_SDCARD OR NF_FEATURE_HAS_USB_MSD)
#     # check if FATFS_SOURCE was specified or if it's empty (default is empty)
#     set(NO_FATFS_SOURCE TRUE)
#     if(FATFS_SOURCE)
#         if(NOT "${FATFS_SOURCE}" STREQUAL "")   
#             set(NO_FATFS_SOURCE FALSE)
#         endif()
#     endif()

#     if(NO_FATFS_SOURCE)
#         # FatFS version
#         set(FATFS_VERSION_EMPTY TRUE)

#         # check if build was requested with a specifc FatFS version
#         if(DEFINED FATFS_VERSION)
#             if(NOT "${FATFS_VERSION}" STREQUAL "")
#                 set(FATFS_VERSION_EMPTY FALSE)
#             endif()
#         endif()

#         # check if build was requested with a specifc FatFS version
#         if(FATFS_VERSION_EMPTY)
#             # no FatFS version actualy specified, must be empty which is fine, we'll default to a known good version
#             set(FATFS_VERSION_TAG "R0.14")
#         else()
#             # set version 
#             set(FATFS_VERSION_TAG "${FATFS_VERSION}")
#         endif()

#         message(STATUS "FatFS version is: ${FATFS_VERSION}")

#         # need to setup a separate CMake project to download the code from the GitHub repository
#         # otherwise it won't be available before the actual build step
#         configure_file("${PROJECT_SOURCE_DIR}/CMake/FatFS.CMakeLists.cmake.in"
#         "${CMAKE_BINARY_DIR}/FatFS_Download/CMakeLists.txt")

#         # setup CMake project for FatFS download
#         execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
#                         RESULT_VARIABLE result
#                         WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/FatFS_Download")

#         # run build on FatFS download CMake project to perform the download
#         execute_process(COMMAND ${CMAKE_COMMAND} --build .
#                         RESULT_VARIABLE result
#                         WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/FatFS_Download")

#         # add FatFS as external project
#         ExternalProject_Add( 
#             FatFS
#             PREFIX FatFS
#             SOURCE_DIR ${CMAKE_BINARY_DIR}/FatFS_Source
#             GIT_REPOSITORY https://github.com/abbrev/fatfs.git #TODO: switch to nF org
#             GIT_TAG ${FATFS_VERSION_TAG}  # target specified branch
#             GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
#             TIMEOUT 10
#             LOG_DOWNLOAD 1
#             INSTALL_COMMAND "${CMAKE_COMMAND}" -E remove "${PROJECT_BINARY_DIR}/FatFS_Source/source/ffconf.h"

#             # Disable all other steps
#             CONFIGURE_COMMAND ""
#             BUILD_COMMAND ""
#         )

#     else()
#         # FatFS source was specified

#         # sanity check is source path exists
#         if(EXISTS "${FATFS_SOURCE}/")
#             message(STATUS "FatFS ${FATFS_VERSION} (source from: ${FATFS_SOURCE})")

#             # check if we already have the sources, no need to copy again
#             NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/FatFS_Source/ SOURCE_EXISTS)

#             if(NOT ${SOURCE_EXISTS})
#                 file(COPY "${FATFS_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/FatFS_Source")
#             else()
#                 message(STATUS "Using local cache of FatFS source from ${FATFS_SOURCE}")
#             endif()
#         else()
#             message(FATAL_ERROR "Couldn't find FatFS source at ${FATFS_SOURCE}/")
#         endif()

#         # add FatFS as external project
#         ExternalProject_Add(
#             FatFS
#             PREFIX FatFS
#             SOURCE_DIR ${CMAKE_BINARY_DIR}/FatFS_Source
#             INSTALL_COMMAND "${CMAKE_COMMAND}" -E remove "${PROJECT_BINARY_DIR}/FatFS_Source/source/ffconf.h"

#             # Disable all other steps
#             CONFIGURE_COMMAND ""
#             BUILD_COMMAND ""
#         )        

#         # get source dir for FatFS CMake project
#         ExternalProject_Get_Property(FatFS SOURCE_DIR)

#     endif()

# endif()

# # if mbed TLS is enabled add it to the build
# if(NF_SECURITY_MBEDTLS)

#     # check if MBEDTLS_SOURCE was specified or if it's empty (default is empty)
#     set(NO_MBEDTLS_SOURCE TRUE)

#     if(MBEDTLS_SOURCE)
#         if(NOT "${MBEDTLS_SOURCE}" STREQUAL "")
#             set(NO_MBEDTLS_SOURCE FALSE)
#         endif()
#     endif()

#     # set options for mbed TLS
#     option(ENABLE_TESTING "no testing when building mbed TLS." OFF)

#     if(NO_MBEDTLS_SOURCE)
#         # no mbed TLS source specified, download it from it's repo

#         # check for Git (needed here for advanced warning to user if it's not installed)
#         find_package(Git)

#         #  check if Git was found, if not report to user and abort
#         if(NOT GIT_EXECUTABLE)
#             message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
#         endif()

#         # set tag for currently supported version
#         set(MBEDTLS_GIT_TAG "mbedtls-2.23.0")

#         # need to setup a separate CMake project to download the code from the GitHub repository
#         # otherwise it won't be available before the actual build step
#         configure_file("${PROJECT_SOURCE_DIR}/CMake/mbedTLS.CMakeLists.cmake.in"
#                     "${CMAKE_BINARY_DIR}/mbedTLS_Download/CMakeLists.txt")

#         # setup CMake project for mbedTLS download
#         execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
#                         RESULT_VARIABLE result
#                         WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/mbedTLS_Download")

#         # run build on mbedTLS download CMake project to perform the download
#         execute_process(COMMAND ${CMAKE_COMMAND} --build .
#                         RESULT_VARIABLE result
#                         WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/mbedTLS_Download")

#         # add mbedTLS as external project
#         ExternalProject_Add(
#             mbedTLS
#             PREFIX mbedTLS
#             SOURCE_DIR ${CMAKE_BINARY_DIR}/mbedTLS_Source
#             GIT_REPOSITORY  https://github.com/nanoframework/mbedtls
#             GIT_TAG ${MBEDTLS_GIT_TAG}  # target specified branch
#             GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
#             TIMEOUT 10
#             LOG_DOWNLOAD 1

#             # Disable all other steps
#             CONFIGURE_COMMAND ""
#             BUILD_COMMAND ""
#             INSTALL_COMMAND ""
#         )

#     else()
#         # mbedTLS source was specified
#         # message(FATAL_ERROR "${CMAKE_BINARY_DIR}/mbedTLS_Source") 

#         # sanity check is source path exists
#         if(EXISTS "${MBEDTLS_SOURCE}/")

#             # check if we already have the sources, no need to copy again
#             NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/mbedTLS_Source/ SOURCE_EXISTS)

#             if(NOT ${SOURCE_EXISTS})
#                 message(STATUS "mbedTLS source from: ${MBEDTLS_SOURCE}")
#                 file(COPY "${MBEDTLS_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/mbedTLS_Source")
#             else()
#                 message(STATUS "Using local cache of mbedTLS source from ${MBEDTLS_SOURCE}")
#             endif()

#             set(MBEDTLS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/mbedTLS_Source/include)
#         else()
#             message(FATAL_ERROR "Couldn't find mbedTLS source at ${MBEDTLS_SOURCE}/")
#         endif()

#         # add mbedTLS as external project
#         ExternalProject_Add(
#             mbedTLS
#             PREFIX mbedTLS
#             SOURCE_DIR ${CMAKE_BINARY_DIR}/mbedTLS_Source

#             # Disable all other steps
#             CONFIGURE_COMMAND ""
#             BUILD_COMMAND ""
#             INSTALL_COMMAND ""
#         )

#     endif()

#     set(mbedTLS_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/3rdparty/humblelogging/include")
#     set(mbedTLS_LIBRARIES "${CMAKE_SHARED_LIBRARY_PREFIX}mbedTLS${CMAKE_SHARED_LIBRARY_SUFFIX}")
#     include_directories(${mbedTLS_INCLUDE_DIRS})

# endif()

# set target base location
# this has to be set before the class library modules are pulled in
if(EXISTS ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/${AZURERTOS_BOARD})
    # set target base location
    set(TARGET_BASE_LOCATION "${PROJECT_SOURCE_DIR}/targets/AzureRTOS/${AZURERTOS_BOARD}")
else()
    # try to find board in the Community targets folder
    if(EXISTS ${PROJECT_SOURCE_DIR}/targets-community/AzureRTOS/${AZURERTOS_BOARD})
        # set target base location
        set(TARGET_BASE_LOCATION "${PROJECT_SOURCE_DIR}/targets-community/AzureRTOS/${AZURERTOS_BOARD}")
    else()
        # board NOT found in targets folder
        # can't continue
        message(FATAL_ERROR "\n\nSorry but support for ${AZURERTOS_BOARD} target is not available...\n\nYou can wait for it to be added or you might want to contribute and start working on a PR for it.\n\n")
    endif()
endif()

# TODO: Overlay custom stuff on top of released source.

# (default is OFF so STM Cube package is NOT included)
option(STM32_CUBE_PACKAGE_REQUIRED "option to include STM Cube package in the build") #TODO: this might be required for the L4 board, but we dont have it. 
if(STM32_CUBE_PACKAGE_REQUIRED)
    ProcessSTM32CubePackage()
endif()

# TODO: should we use SPIFFS, or AzureRTOS custom implementation
# # if support for SPIFFS is enabled add it to the build
# if(NF_FEATURE_USE_SPIFFS)

#     # check if SPIFFS_SOURCE was specified or if it's empty (default is empty)
#     set(NO_SPIFFS_SOURCE TRUE)

#     if(SPIFFS_SOURCE)
#         if(NOT "${SPIFFS_SOURCE}" STREQUAL "")
#             set(NO_SPIFFS_SOURCE FALSE)
#         endif()
#     endif()

#     if(NO_SPIFFS_SOURCE)
#         # no SPIFFS source specified, download it from it's repo

#         # check for Git (needed here for advanced warning to user if it's not installed)
#         find_package(Git)

#         #  check if Git was found, if not report to user and abort
#         if(NOT GIT_EXECUTABLE)
#             message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
#         endif()

#         # need to setup a separate CMake project to download the code from the GitHub repository
#         # otherwise it won't be available before the actual build step
#         configure_file("${PROJECT_SOURCE_DIR}/CMake/SPIFFS.CMakeLists.cmake.in"
#                     "${CMAKE_BINARY_DIR}/SPIFFS_Download/CMakeLists.txt")

#         # setup CMake project for SPIFFS download
#         execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
#                         RESULT_VARIABLE result
#                         WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/SPIFFS_Download")

#         # run build on SPIFFS download CMake project to perform the download
#         execute_process(COMMAND ${CMAKE_COMMAND} --build .
#                         RESULT_VARIABLE result
#                         WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/SPIFFS_Download")

#         # add SPIFFS as external project
#         ExternalProject_Add(
#             SPIFFS
#             PREFIX SPIFFS
#             SOURCE_DIR ${CMAKE_BINARY_DIR}/SPIFFS_Source
#             GIT_REPOSITORY  https://github.com/nanoframework/spiffs
#             GIT_TAG "nf-build"  # target nanoFramework modified branch
#             GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
#             TIMEOUT 10
#             LOG_DOWNLOAD 1

#             # Disable all other steps
#             CONFIGURE_COMMAND ""
#             BUILD_COMMAND ""
#             INSTALL_COMMAND ""
#         )

#     else()
#         # SPIFFS source was specified

#         # sanity check is source path exists
#         if(EXISTS "${SPIFFS_SOURCE}/")

#             # check if we already have the sources, no need to copy again
#             if(NOT EXISTS "${CMAKE_BINARY_DIR}/SPIFFS_Source")
#                 message(STATUS "SPIFFS source from: ${SPIFFS_SOURCE}")
#                 file(COPY "${SPIFFS_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/SPIFFS_Source")
#             else()
#                 message(STATUS "Using local cache of SPIFFS source from ${SPIFFS_SOURCE}")
#             endif()

#             set(SPIFFS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/SPIFFS_Source/include)
#         else()
#             message(FATAL_ERROR "Couldn't find SPIFFS source at ${SPIFFS_SOURCE}/")
#         endif()

#         # add SPIFFS as external project
#         ExternalProject_Add(
#             SPIFFS
#             PREFIX SPIFFS
#             SOURCE_DIR ${CMAKE_BINARY_DIR}/SPIFFS_Source

#             # Disable all other steps
#             CONFIGURE_COMMAND ""
#             BUILD_COMMAND ""
#             INSTALL_COMMAND ""
#         )

#     endif()

#     # get source dir for SPIFFS CMake project
#     ExternalProject_Get_Property(SPIFFS SOURCE_DIR)

#     set(SPIFFS_LIBRARIES "${CMAKE_SHARED_LIBRARY_PREFIX}SPIFFS${CMAKE_SHARED_LIBRARY_SUFFIX}")
#     include_directories(${SPIFFS_INCLUDE_DIRS})

#     add_subdirectory(spiffs)

# endif()

# add target AzureRTOS dirs
add_subdirectory(Include)
add_subdirectory(common)
#add_subdirectory(nanoBooter) 
add_subdirectory(nanoCLR)

# board folder will be added in main CMakeList
