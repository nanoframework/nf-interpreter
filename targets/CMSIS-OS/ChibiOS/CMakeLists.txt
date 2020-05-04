#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.arm-none-eabi)
include(binutils.ChibiOS)
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

# check if CHIBIOS_SOURCE was specified or if it's empty (default is empty)
set(NO_CHIBIOS_SOURCE TRUE)
if(CHIBIOS_SOURCE)
    if(NOT "${CHIBIOS_SOURCE}" STREQUAL "")
        set(NO_CHIBIOS_SOURCE FALSE)
    endif()
endif()

# check if CHIBIOS-Contrib_SOURCE was specified or if it's empty (default is empty)
set(NO_CHIBIOS_CONTRIB_SOURCE TRUE)
if(CHIBIOS_CONTRIB_SOURCE)
    if(NOT "${CHIBIOS_CONTRIB_SOURCE}" STREQUAL "")
        set(NO_CHIBIOS_CONTRIB_SOURCE FALSE)
    endif()
endif()

if(NO_CHIBIOS_SOURCE)
    # no CHIBIOS source specified, download it from it's repo

    # check for Git (needed here for advanced warning to user if it's not installed)
    find_package(Git)

    #  check if Git was found, if not report to user and abort
    if(NOT GIT_EXECUTABLE)
        message(FATAL_ERROR "Could not find Git, make sure you have it installed.")
    endif()

    # ChibiOS version
    set(CHIBIOS_GIT_TAG "stable_19.1.x")
    message(STATUS "RTOS is: CHIBIOS (latest available code from ${CHIBIOS_GIT_TAG})")

    # need to setup a separate CMake project to download the code from the GitHub repository
    # otherwise it won't be available before the actual build step
    configure_file("${PROJECT_SOURCE_DIR}/CMake/ChibiOS.CMakeLists.cmake.in"
                "${CMAKE_BINARY_DIR}/ChibiOS_Download/CMakeLists.txt")

    # setup CMake project for ChibiOS download
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/ChibiOS_Download")

    # run build on ChibiOS download CMake project to perform the download
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/ChibiOS_Download")

    # add ChibiOS as external project
    ExternalProject_Add(
        ChibiOS
        PREFIX ChibiOS
        SOURCE_DIR ${CMAKE_BINARY_DIR}/ChibiOS_Source
        GIT_REPOSITORY  https://github.com/nanoframework/chibios
        GIT_TAG ${CHIBIOS_GIT_TAG}  # target specified branch
        GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
        TIMEOUT 10
        LOG_DOWNLOAD 1

        # Disable all other steps
        INSTALL_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
    )

    # get source dir for ChibiOS CMake project
    ExternalProject_Get_Property(ChibiOS SOURCE_DIR)

else()
    # ChibiOS source was specified

    # sanity check is source path exists
    if(EXISTS "${CHIBIOS_SOURCE}/")
        message(STATUS "RTOS is: ChibiOS (source from: ${CHIBIOS_SOURCE})")

        # check if we already have the sources, no need to copy again
        NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/ChibiOS_Source/ SOURCE_EXISTS)

        if(NOT ${SOURCE_EXISTS})
            file(COPY "${CHIBIOS_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/ChibiOS_Source")
        else()
            message(STATUS "Using local cache of ChibiOS source from ${CHIBIOS_SOURCE}")
        endif()

        set(CHIBIOS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/ChibiOS_Source/include)
    else()
        message(FATAL_ERROR "Couldn't find ChibiOS source at ${CHIBIOS_SOURCE}/")
    endif()

    # add ChibiOS as external project
    ExternalProject_Add(
        ChibiOS
        PREFIX ChibiOS
        SOURCE_DIR ${CMAKE_BINARY_DIR}/ChibiOS_Source
        
        # Disable all other steps
        INSTALL_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
    )

    # get source dir for ChibiOS CMake project
    ExternalProject_Get_Property(ChibiOS SOURCE_DIR)

endif()

if(CHIBIOS_CONTRIB_REQUIRED)
    if(NO_CHIBIOS_CONTRIB_SOURCE)
        # no CHIBIOS_CONTRIB source specified, download it from it's repo

        # check for Git (needed here for advanced warning to user if it's not installed)
        find_package(Git)

        #  check if Git was found, if not report to user and abort
        if(NOT GIT_EXECUTABLE)
            message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
        endif()

        message(STATUS "ChibiOS-Contrib source from GitHub repo")

        # need to setup a separate CMake project to download the code from the GitHub repository
        # otherwise it won't be available before the actual build step
        configure_file("${PROJECT_SOURCE_DIR}/CMake/ChibiOS-Contrib.CMakeLists.cmake.in"
                    "${CMAKE_BINARY_DIR}/ChibiOS-Contrib_Download/CMakeLists.txt")

        # setup CMake project for ChibiOS-Contrib download
        execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                        RESULT_VARIABLE result
                        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/ChibiOS-Contrib_Download")

        # run build on ChibiOS-Contrib download CMake project to perform the download
        execute_process(COMMAND ${CMAKE_COMMAND} --build .
                        RESULT_VARIABLE result
                        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/ChibiOS-Contrib_Download")

        # add ChibiOS-Contrib as external project
        # need to specify nanoframework as the active branch
        ExternalProject_Add( 
            ChibiOS-Contrib
            PREFIX ChibiOS-Contrib
            SOURCE_DIR ${CMAKE_BINARY_DIR}/ChibiOS-Contrib_Source
            GIT_REPOSITORY  https://github.com/nanoframework/ChibiOS-Contrib
            GIT_TAG nanoframework  # target specified branch
            GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
            TIMEOUT 10
            LOG_DOWNLOAD 1
            # Disable all other steps
            INSTALL_COMMAND ""
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
        )        

        # get source dir for ChibiOS-Contrib CMake project
        ExternalProject_Get_Property(ChibiOS-Contrib SOURCE_DIR)

    else()
        # ChibiOS-Contrib source was specified

        # sanity check is source path exists
        if(EXISTS "${CHIBIOS_CONTRIB_SOURCE}/")
            message(STATUS "ChibiOS-Contrib source from: ${CHIBIOS_CONTRIB_SOURCE}")

            # check if we already have the sources, no need to copy again
            NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/ChibiOS-Contrib_Source/ SOURCE_EXISTS)

            if(NOT ${SOURCE_EXISTS})
                file(COPY "${CHIBIOS_CONTRIB_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/ChibiOS-Contrib_Source")
            else()
                message(STATUS "Using local cache of ChibiOS-Contrib source from ${CHIBIOS_CONTRIB_SOURCE}")
            endif()

            set(CHIBIOS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/ChibiOS_Source/include)
        else()
            message(FATAL_ERROR "Couldn't find ChibiOS-Contrib source at ${CHIBIOS_CONTRIB_SOURCE}/")
        endif()

        # add ChibiOS-Contrib as external project
        ExternalProject_Add(
            ChibiOS-Contrib
            PREFIX ChibiOS-Contrib
            SOURCE_DIR ${CMAKE_BINARY_DIR}/ChibiOS-Contrib_Source
            # Disable all other steps
            INSTALL_COMMAND ""
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
        )

        # get source dir for ChibiOS-Contrib CMake project
        ExternalProject_Get_Property(ChibiOS-Contrib SOURCE_DIR)

    endif()
endif()

# include FatFS if SDCard or USB MSD are ON
if(NF_FEATURE_HAS_SDCARD OR NF_FEATURE_HAS_USB_MSD)
    # check if FATFS_SOURCE was specified or if it's empty (default is empty)
    set(NO_FATFS_SOURCE TRUE)
    if(FATFS_SOURCE)
        if(NOT "${FATFS_SOURCE}" STREQUAL "")   
            set(NO_FATFS_SOURCE FALSE)
        endif()
    endif()

    if(NO_FATFS_SOURCE)
        # FatFS version
        set(FATFS_VERSION_EMPTY TRUE)

        # check if build was requested with a specifc FatFS version
        if(DEFINED FATFS_VERSION)
            if(NOT "${FATFS_VERSION}" STREQUAL "")
                set(FATFS_VERSION_EMPTY FALSE)
            endif()
        endif()

        # check if build was requested with a specifc FatFS version
        if(FATFS_VERSION_EMPTY)
            # no FatFS version actualy specified, must be empty which is fine, we'll default to a known good version
            set(FATFS_VERSION_TAG "R0.14")
        else()
            # set version 
            set(FATFS_VERSION_TAG "${FATFS_VERSION}")
        endif()

        message(STATUS "FatFS version is: ${FATFS_VERSION}")

        # need to setup a separate CMake project to download the code from the GitHub repository
        # otherwise it won't be available before the actual build step
        configure_file("${PROJECT_SOURCE_DIR}/CMake/FatFS.CMakeLists.cmake.in"
        "${CMAKE_BINARY_DIR}/FatFS_Download/CMakeLists.txt")

        # setup CMake project for FatFS download
        execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                        RESULT_VARIABLE result
                        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/FatFS_Download")

        # run build on FatFS download CMake project to perform the download
        execute_process(COMMAND ${CMAKE_COMMAND} --build .
                        RESULT_VARIABLE result
                        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/FatFS_Download")

        # add FatFS as external project
        ExternalProject_Add( 
            FatFS
            PREFIX FatFS
            SOURCE_DIR ${CMAKE_BINARY_DIR}/FatFS_Source
            GIT_REPOSITORY https://github.com/abbrev/fatfs.git #TODO: switch to nF org
            GIT_TAG ${FATFS_VERSION_TAG}  # target specified branch
            GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
            TIMEOUT 10
            LOG_DOWNLOAD 1
            INSTALL_COMMAND "${CMAKE_COMMAND}" -E remove "${PROJECT_BINARY_DIR}/FatFS_Source/source/ffconf.h"

            # Disable all other steps
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
        )

    else()
        # FatFS source was specified

        # sanity check is source path exists
        if(EXISTS "${FATFS_SOURCE}/")
            message(STATUS "FatFS ${FATFS_VERSION} (source from: ${FATFS_SOURCE})")

            # check if we already have the sources, no need to copy again
            NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/FatFS_Source/ SOURCE_EXISTS)

            if(NOT ${SOURCE_EXISTS})
                file(COPY "${FATFS_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/FatFS_Source")
            else()
                message(STATUS "Using local cache of FatFS source from ${FATFS_SOURCE}")
            endif()
        else()
            message(FATAL_ERROR "Couldn't find FatFS source at ${FATFS_SOURCE}/")
        endif()

        # add FatFS as external project
        ExternalProject_Add(
            FatFS
            PREFIX FatFS
            SOURCE_DIR ${CMAKE_BINARY_DIR}/FatFS_Source
            INSTALL_COMMAND "${CMAKE_COMMAND}" -E remove "${PROJECT_BINARY_DIR}/FatFS_Source/source/ffconf.h"

            # Disable all other steps
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
        )        

        # get source dir for FatFS CMake project
        ExternalProject_Get_Property(FatFS SOURCE_DIR)

    endif()

endif()

# if mbed TLS is enabled add it to the build
if(NF_SECURITY_MBEDTLS)

    # check if MBEDTLS_SOURCE was specified or if it's empty (default is empty)
    set(NO_MBEDTLS_SOURCE TRUE)

    if(MBEDTLS_SOURCE)
        if(NOT "${MBEDTLS_SOURCE}" STREQUAL "")
            set(NO_MBEDTLS_SOURCE FALSE)
        endif()
    endif()

    # set options for mbed TLS
    option(ENABLE_TESTING "no testing when building mbed TLS." OFF)

    if(NO_MBEDTLS_SOURCE)
        # no mbed TLS source specified, download it from it's repo

        # check for Git (needed here for advanced warning to user if it's not installed)
        find_package(Git)

        #  check if Git was found, if not report to user and abort
        if(NOT GIT_EXECUTABLE)
            message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
        endif()

        # set tag for currently supported version
        set(MBEDTLS_GIT_TAG "mbedtls-2.16.3")

        # need to setup a separate CMake project to download the code from the GitHub repository
        # otherwise it won't be available before the actual build step
        configure_file("${PROJECT_SOURCE_DIR}/CMake/mbedTLS.CMakeLists.cmake.in"
                    "${CMAKE_BINARY_DIR}/mbedTLS_Download/CMakeLists.txt")

        # setup CMake project for mbedTLS download
        execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                        RESULT_VARIABLE result
                        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/mbedTLS_Download")

        # run build on mbedTLS download CMake project to perform the download
        execute_process(COMMAND ${CMAKE_COMMAND} --build .
                        RESULT_VARIABLE result
                        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/mbedTLS_Download")

        # add mbedTLS as external project
        ExternalProject_Add(
            mbedTLS
            PREFIX mbedTLS
            SOURCE_DIR ${CMAKE_BINARY_DIR}/mbedTLS_Source
            GIT_REPOSITORY  https://github.com/nanoframework/mbedtls
            GIT_TAG ${MBEDTLS_GIT_TAG}  # target specified branch
            GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
            TIMEOUT 10
            LOG_DOWNLOAD 1

            # Disable all other steps
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
            INSTALL_COMMAND ""
        )

    else()
        # mbedTLS source was specified
        # message(FATAL_ERROR "${CMAKE_BINARY_DIR}/mbedTLS_Source") 

        # sanity check is source path exists
        if(EXISTS "${MBEDTLS_SOURCE}/")

            # check if we already have the sources, no need to copy again
            NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/mbedTLS_Source/ SOURCE_EXISTS)

            if(NOT ${SOURCE_EXISTS})
                message(STATUS "mbedTLS source from: ${MBEDTLS_SOURCE}")
                file(COPY "${MBEDTLS_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/mbedTLS_Source")
            else()
                message(STATUS "Using local cache of mbedTLS source from ${MBEDTLS_SOURCE}")
            endif()

            set(MBEDTLS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/mbedTLS_Source/include)
        else()
            message(FATAL_ERROR "Couldn't find mbedTLS source at ${MBEDTLS_SOURCE}/")
        endif()

        # add mbedTLS as external project
        ExternalProject_Add(
            mbedTLS
            PREFIX mbedTLS
            SOURCE_DIR ${CMAKE_BINARY_DIR}/mbedTLS_Source

            # Disable all other steps
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
            INSTALL_COMMAND ""
        )

    endif()

    set(mbedTLS_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/3rdparty/humblelogging/include")
    set(mbedTLS_LIBRARIES "${CMAKE_SHARED_LIBRARY_PREFIX}mbedTLS${CMAKE_SHARED_LIBRARY_SUFFIX}")
    include_directories(${mbedTLS_INCLUDE_DIRS})

endif()

# set target base location
# this has to be set before the class library modules are pulled in
if(EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD})
    # set target base location
    set(TARGET_BASE_LOCATION "${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}")
else()
    # try to find board in the Community targets folder
    if(EXISTS ${PROJECT_SOURCE_DIR}/targets-community/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD})
        # set target base location
        set(TARGET_BASE_LOCATION "${PROJECT_SOURCE_DIR}/targets-community/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}")
    else()
        # board NOT found in targets folder
        # can't continue
        message(FATAL_ERROR "\n\nSorry but support for ${CHIBIOS_BOARD} target is not available...\n\nYou can wait for that to be added or you might want to contribute and start working on a PR for that.\n\n")
    endif()
endif()

# set CMSIS RTOS include directory
include_directories( ${CMSIS_RTOS_INCLUDE_DIR})

# need to find board definition files (board.c and board.h)

# assume no community board... until proven otherwise
set(CHIBIOS_COMMUNITY_TARGET FALSE CACHE INTERNAL "Community target flag")

# start search in nanoFramework ChibiOS 'overlay' folder
if(EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/boards/${CHIBIOS_BOARD})
    # board found
    # if it's on nF overlay board.c and board.h exist there for sure
    set(CHIBIOS_BOARD_DEFINITIONS_LOCATION "Board definition files taken from nanoFramework overlay" CACHE INTERNAL "Location of board definition files")
else()
    # board NOT found in ChibiOS 'overlay'

    # try to find it in the target boards
    if(EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD})
        # board found
        # check if the board definition files are available at the target folder
        if( EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}/board.c AND
            EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}/board.h)
            # definition files found
            set(CHIBIOS_BOARD_DEFINITIONS_LOCATION "Board definition files taken from target folder" CACHE INTERNAL "Location of board definition files")

        else()
            # board.c and board.h are NOT in the target folder, try to find them in the official distribution

            if(EXISTS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/boards/${CHIBIOS_BOARD})
                # board found
                # if it's on the ChibiOS official distribution board.c and board.h exist here for sure
                set(CHIBIOS_BOARD_DEFINITIONS_LOCATION "Board definition files taken from official ChibiOS distribution" CACHE INTERNAL "Location of board definition files")

            else()
                # board NOT found in official distribution
                # quit now as there is no were else to search for these
                message(FATAL_ERROR "\n\nSorry but couldn't find definition files for ${CHIBIOS_BOARD} in the available list of ChibiOS supported boards...\n\n")

            endif()
        endif()

    else()
        # try to find board in the Community targets folder
        if(EXISTS ${PROJECT_SOURCE_DIR}/targets-community/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD})
            # set flag for this being a community board
            set(CHIBIOS_COMMUNITY_TARGET TRUE CACHE INTERNAL "Community target flag")
        else()
            # board NOT found in official distribution
            # quit now as there is no were else to search for these
            message(FATAL_ERROR "\n\nSorry but couldn't find definition files for ${CHIBIOS_BOARD} in the available list of ChibiOS supported boards...\n\n")
        endif()

    endif()

endif()

# (default is OFF so STM Cube package is NOT included)
option(STM32_CUBE_PACKAGE_REQUIRED "option to include STM Cube pcakge in the build")
if(STM32_CUBE_PACKAGE_REQUIRED)
    ProcessSTM32CubePackage()
endif()

# if support for SPIFFS is enabled add it to the build
if(NF_FEATURE_USE_SPIFFS)

    # check if SPIFFS_SOURCE was specified or if it's empty (default is empty)
    set(NO_SPIFFS_SOURCE TRUE)

    if(SPIFFS_SOURCE)
        if(NOT "${SPIFFS_SOURCE}" STREQUAL "")
            set(NO_SPIFFS_SOURCE FALSE)
        endif()
    endif()

    if(NO_SPIFFS_SOURCE)
        # no SPIFFS source specified, download it from it's repo

        # check for Git (needed here for advanced warning to user if it's not installed)
        find_package(Git)

        #  check if Git was found, if not report to user and abort
        if(NOT GIT_EXECUTABLE)
            message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
        endif()

        # need to setup a separate CMake project to download the code from the GitHub repository
        # otherwise it won't be available before the actual build step
        configure_file("${PROJECT_SOURCE_DIR}/CMake/SPIFFS.CMakeLists.cmake.in"
                    "${CMAKE_BINARY_DIR}/SPIFFS_Download/CMakeLists.txt")

        # setup CMake project for SPIFFS download
        execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                        RESULT_VARIABLE result
                        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/SPIFFS_Download")

        # run build on SPIFFS download CMake project to perform the download
        execute_process(COMMAND ${CMAKE_COMMAND} --build .
                        RESULT_VARIABLE result
                        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/SPIFFS_Download")

        # add SPIFFS as external project
        ExternalProject_Add(
            SPIFFS
            PREFIX SPIFFS
            SOURCE_DIR ${CMAKE_BINARY_DIR}/SPIFFS_Source
            GIT_REPOSITORY  https://github.com/nanoframework/spiffs
            GIT_TAG "nf-build"  # target nanoFramework modified branch
            GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
            TIMEOUT 10
            LOG_DOWNLOAD 1

            # Disable all other steps
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
            INSTALL_COMMAND ""
        )

    else()
        # SPIFFS source was specified

        # sanity check is source path exists
        if(EXISTS "${SPIFFS_SOURCE}/")

            # check if we already have the sources, no need to copy again
            if(NOT EXISTS "${CMAKE_BINARY_DIR}/SPIFFS_Source")
                message(STATUS "SPIFFS source from: ${SPIFFS_SOURCE}")
                file(COPY "${SPIFFS_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/SPIFFS_Source")
            else()
                message(STATUS "Using local cache of SPIFFS source from ${SPIFFS_SOURCE}")
            endif()

            set(SPIFFS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/SPIFFS_Source/include)
        else()
            message(FATAL_ERROR "Couldn't find SPIFFS source at ${SPIFFS_SOURCE}/")
        endif()

        # add SPIFFS as external project
        ExternalProject_Add(
            SPIFFS
            PREFIX SPIFFS
            SOURCE_DIR ${CMAKE_BINARY_DIR}/SPIFFS_Source

            # Disable all other steps
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
            INSTALL_COMMAND ""
        )

    endif()

    # get source dir for SPIFFS CMake project
    ExternalProject_Get_Property(SPIFFS SOURCE_DIR)

    set(SPIFFS_LIBRARIES "${CMAKE_SHARED_LIBRARY_PREFIX}SPIFFS${CMAKE_SHARED_LIBRARY_SUFFIX}")
    include_directories(${SPIFFS_INCLUDE_DIRS})

    add_subdirectory(spiffs)

endif()

# add target ChibiOS dirs
add_subdirectory(Include)
add_subdirectory(common)
add_subdirectory(nanoBooter)
add_subdirectory(nanoCLR)

# board folder will be added in main CMakeList
