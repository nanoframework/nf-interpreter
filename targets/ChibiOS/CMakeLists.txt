#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
include(binutils.common)
include(binutils.arm-none-eabi)
include(binutils.ChibiOS)
include(STM32_CubePackage)

# Set target series
nf_set_stm32_target_series()

# Define PLATFORM base path
set(BASE_PATH_FOR_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

########################################################
# check availability of hex2dfu tool if specified
if(DEFINED TOOL_HEX2DFU_PREFIX)
    if(NOT EXISTS ${TOOL_HEX2DFU_PREFIX}/hex2dfu.exe AND NOT EXISTS ${TOOL_HEX2DFU_PREFIX}/hex2dfu)
        message(STATUS "")
        message(STATUS "Couldn't find the hex2dfu tool at the specified path: ${TOOL_HEX2DFU_PREFIX}/hex2dfu.exe or ${TOOL_HEX2DFU_PREFIX}/hex2dfu")
        message(STATUS "Make sure that the CMake option TOOL_HEX2DFU_PREFIX has the correct path.")
        message(STATUS "If you don't have this tool download it from https://github.com/nanoframework/nf-tools/releases")
        message(STATUS "")
        message(FATAL_ERROR "hex2dfu tool not found")
    else()
        set(HEX2DFU_TOOL_AVAILABLE TRUE CACHE INTERNAL "hex2dfu tool available")
    endif()
endif()

########################################################
# check availability of SRecord tool, if specified
if(DEFINED TOOL_SRECORD_PREFIX)
    if(NOT EXISTS ${TOOL_SRECORD_PREFIX}/srec_cat.exe AND NOT EXISTS ${TOOL_SRECORD_PREFIX}/srec_cat)
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

# check if CHIBIOS_SOURCE_FOLDER was specified or if it's empty (default is empty)
set(NO_CHIBIOS_SOURCE_FOLDER TRUE)
if(CHIBIOS_SOURCE_FOLDER)
    if(NOT ${CHIBIOS_SOURCE_FOLDER} STREQUAL "")
        set(NO_CHIBIOS_SOURCE_FOLDER FALSE)
    endif()
endif()

# check if CHIBIOS-Contrib_SOURCE was specified or if it's empty (default is empty)
set(NO_CHIBIOS_CONTRIB_SOURCE TRUE)
if(CHIBIOS_CONTRIB_SOURCE)
    if(NOT ${CHIBIOS_CONTRIB_SOURCE} STREQUAL "")
        set(NO_CHIBIOS_CONTRIB_SOURCE FALSE)
    endif()
endif()

# ChibiOS version
set(RTOS_VERSION_EMPTY TRUE)

# check if build was requested with a specifc ChibiOS version
if(DEFINED RTOS_VERSION)
    if(NOT "${RTOS_VERSION}" STREQUAL "")
        set(RTOS_VERSION_EMPTY FALSE)
    endif()
endif()

# check if build was requested with a specifc ChibiOS version
if(RTOS_VERSION_EMPTY)
    # no ChibiOS version actualy specified, must be empty which is fine, we'll default to a known good version
    # WHEN CHANGING THIS MAKE SURE TO UPDATE THE DEV CONTAINERS
    # WHEN CHANGING THIS MAKE SURE TO UPDATE AzureRTOS\ST\CMakeLists.txt
    set(RTOS_VERSION "stable_21.11.x")
endif()

if(NO_CHIBIOS_SOURCE_FOLDER)
    # no CHIBIOS source specified, download it from it's repo
    message(STATUS "RTOS is: ChibiOS ${RTOS_VERSION} from SVN official repo")

    FetchContent_Declare(
        chibios
        SVN_REPOSITORY http://svn.code.sf.net/p/chibios/code/branches/${RTOS_VERSION}
        SVN_REVISION -rHEAD
    )

else()
    # ChibiOS source was specified


    message(STATUS "RTOS is: ChibiOS ${RTOS_VERSION} (source from: ${CHIBIOS_SOURCE_FOLDER})")
    FetchContent_Declare(
        chibios
        SOURCE_DIR ${CHIBIOS_SOURCE_FOLDER}
    )


endif()

# Check if population has already been performed
FetchContent_GetProperties(chibios)
if(NOT chibios_POPULATED)
    # Fetch the content using previously declared details
    FetchContent_MakeAvailable(chibios)
endif()

if(CHIBIOS_CONTRIB_REQUIRED)
    if(NO_CHIBIOS_CONTRIB_SOURCE)
        # no CHIBIOS_CONTRIB source specified, download it from it's repo

        FetchContent_Declare(
            chibios-contrib
            GIT_REPOSITORY https://github.com/ChibiOS/ChibiOS-Contrib.git
            GIT_TAG chibios-21.11.x
        )

        message(STATUS "ChibiOS-Contrib from GitHub repo")

    else()
        # ChibiOS-Contrib source was specified

        message(STATUS "ChibiOS-Contrib (source from: ${CHIBIOS_CONTRIB_SOURCE})")

        FetchContent_Declare(
            chibios-contrib
            SOURCE_DIR ${CHIBIOS_CONTRIB_SOURCE}
        )

    endif()

    # Check if population has already been performed
    FetchContent_GetProperties(chibios-contrib)
    if(NOT chibios-contrib_POPULATED)
        # Fetch the content using previously declared details
        FetchContent_MakeAvailable(chibios-contrib)
    endif()

endif()


# include FatFS if SDCard or USB MSD are ON
if(NF_FEATURE_HAS_SDCARD OR NF_FEATURE_HAS_USB_MSD)

    # check if FATFS_SOURCE was specified or if it's empty (default is empty)
    set(NO_FATFS_SOURCE TRUE)
    if(FATFS_SOURCE)
        if(NOT ${FATFS_SOURCE} STREQUAL "")   
            set(NO_FATFS_SOURCE FALSE)
        endif()
    endif()

    # FatFS version
    set(FATFS_VERSION_EMPTY TRUE)

    # check if build was requested with a specifc FatFS version
    if(DEFINED FATFS_VERSION)
        if(NOT ${FATFS_VERSION} STREQUAL "")
            set(FATFS_VERSION_EMPTY FALSE)
        endif()
    endif()

    # check if build was requested with a specifc FatFS version
    if(FATFS_VERSION_EMPTY)
        # no FatFS version actualy specified, must be empty which is fine, we'll default to a known good version
        # WHEN CHANGING THIS MAKE SURE TO UPDATE THE DEV CONTAINERS
        set(FATFS_VERSION_TAG "R0.15a")
    else()
        # set version 
        set(FATFS_VERSION_TAG ${FATFS_VERSION})
    endif()

    if(NO_FATFS_SOURCE)
        message(STATUS "FatFS ${FATFS_VERSION_TAG} from GitHub repo")

        FetchContent_Declare(
            fatfs
            GIT_REPOSITORY https://github.com/abbrev/fatfs.git
            GIT_TAG ${FATFS_VERSION_TAG}
        )

    else()
        # FatFS source was specified

        message(STATUS "FatFS ${FATFS_VERSION_TAG} (source from: ${FATFS_SOURCE})")

        FetchContent_Declare(
            fatfs
            SOURCE_DIR ${FATFS_SOURCE}
        )

    endif()

    # Check if population has already been performed
    FetchContent_GetProperties(fatfs)
    if(NOT fatfs_POPULATED)
        # Fetch the content using previously declared details
        FetchContent_MakeAvailable(fatfs)
    endif()

    # have to delete the ffconf.h template from the repo
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E remove ${fatfs_SOURCE_DIR}/source/ffconf.h
    )

endif()

# if networking is enabled add lwIP to the build
if(USE_NETWORKING_OPTION)

    nf_add_lwip_library()

endif()


# if Mbed TLS is enabled add it to the build
if(USE_SECURITY_MBEDTLS_OPTION OR NF_REQUIRES_MBEDTLS)

    nf_add_mbedtls_library()

endif()

# set target base location
# this has to be set before the class library modules are pulled in
if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/targets/ChibiOS/${TARGET_BOARD})
    # set target base location
    set(TARGET_BASE_LOCATION ${CMAKE_SOURCE_DIR}/targets/ChibiOS/${TARGET_BOARD} CACHE INTERNAL "make global")
else()
    # try to find board in the Community targets folder
    if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/targets-community/ChibiOS/${TARGET_BOARD})
        # set target base location
        set(TARGET_BASE_LOCATION ${CMAKE_SOURCE_DIR}/targets-community/ChibiOS/${TARGET_BOARD} CACHE INTERNAL "make global")
    else()
        # board NOT found in targets folder
        # can't continue
        message(FATAL_ERROR "\n\nSorry but support for ${TARGET_BOARD} target is not available...\n\nYou can wait for that to be added or you might want to contribute and start working on a PR for that.\n\n")
    endif()
endif()

# set CMSIS RTOS include directory
include_directories( ${CMSIS_RTOS_INCLUDE_DIR})

# need to find board definition files (board.c and board.h)

# start search in nanoFramework ChibiOS 'overlay' folder
if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/boards/${TARGET_BOARD})
    # board found
    # if it's on nF overlay board.c and board.h exist there for sure
    set(CHIBIOS_BOARD_DEFINITIONS_LOCATION "Board definition files taken from nanoFramework overlay" CACHE INTERNAL "Location of board definition files")
    set(CHIBIOS_BOARD_DEFINITIONS_PATH ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/boards/${TARGET_BOARD} CACHE INTERNAL "Path of board definition files")
else()
    # board NOT found in ChibiOS 'overlay'

    # try to find it in the target boards
    if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/targets/ChibiOS/${TARGET_BOARD})
        # board found
        # check if the board definition files are available at the target folder
        if( EXISTS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/${TARGET_BOARD}/board.c AND
            EXISTS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/${TARGET_BOARD}/board.h)
            # definition files found
            set(CHIBIOS_BOARD_DEFINITIONS_LOCATION "Board definition files taken from target folder" CACHE INTERNAL "Location of board definition files")
            set(CHIBIOS_BOARD_DEFINITIONS_PATH ${CMAKE_SOURCE_DIR}/targets/ChibiOS/${TARGET_BOARD} CACHE INTERNAL "Path of board definition files")

        else()
            # board.c and board.h are NOT in the target folder, try to find them in the official distribution

            if(IS_DIRECTORY ${chibios_SOURCE_DIR}/os/hal/boards/${TARGET_BOARD})
                # board found
                # if it's on the ChibiOS official distribution board.c and board.h exist here for sure
                set(CHIBIOS_BOARD_DEFINITIONS_LOCATION "Board definition files taken from official ChibiOS distribution" CACHE INTERNAL "Location of board definition files")
                set(CHIBIOS_BOARD_DEFINITIONS_PATH ${chibios_SOURCE_DIR}/os/hal/boards/${TARGET_BOARD} CACHE INTERNAL "Path of board definition files")

            else()
                # board NOT found in official distribution
                # quit now as there is no were else to search for these
                message(FATAL_ERROR "\n\nSorry but couldn't find definition files for ${TARGET_BOARD} in the available list of ChibiOS supported boards...\n\n")

            endif()
        endif()

    else()

        # try to find board in the Community targets folder
        if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/targets-community/ChibiOS/${TARGET_BOARD})
            # set flag for this being a community board
            set(IS_COMMUNITY_TARGET TRUE CACHE INTERNAL "Community target flag")


            # check if the board definition files are available at the target folder
            if( EXISTS ${CMAKE_SOURCE_DIR}/targets-community/ChibiOS/${TARGET_BOARD}/board.c AND
                EXISTS ${CMAKE_SOURCE_DIR}/targets-community/ChibiOS/${TARGET_BOARD}/board.h)

                # definition files found
                set(CHIBIOS_BOARD_DEFINITIONS_LOCATION "Board definition files taken from Community Targets" CACHE INTERNAL "Location of board definition files")
                set(CHIBIOS_BOARD_DEFINITIONS_PATH ${CMAKE_SOURCE_DIR}/targets-community/ChibiOS/${TARGET_BOARD} CACHE INTERNAL "Path of board definition files")

            else()

                # board.c and board.h are NOT in the target folder, try to find them in the official distribution
                if(IS_DIRECTORY ${chibios_SOURCE_DIR}/os/hal/boards/${TARGET_BOARD})
                    # board found
                    # if it's on the ChibiOS official distribution board.c and board.h exist here for sure
                    set(CHIBIOS_BOARD_DEFINITIONS_LOCATION "Board definition files taken from official ChibiOS distribution" CACHE INTERNAL "Location of board definition files")
                    set(CHIBIOS_BOARD_DEFINITIONS_PATH ${chibios_SOURCE_DIR}/os/hal/boards/${TARGET_BOARD} CACHE INTERNAL "Path of board definition files")

                else()
                    # board NOT found in official distribution
                    # quit now as there is no were else to search for these
                    message(FATAL_ERROR "\n\nSorry but couldn't find definition files for ${TARGET_BOARD} in the available list of ChibiOS supported boards...\n\n")

                endif()

            endif()

        else()
            # board NOT found in official distribution
            # quit now as there is no were else to search for these
            message(FATAL_ERROR "\n\nSorry but couldn't find definition files for ${TARGET_BOARD} in the available list of ChibiOS supported boards...\n\n")
        endif()

    endif()

endif()

# (default is OFF so STM Cube package is NOT included)
option(STM32_CUBE_PACKAGE_REQUIRED "option to include STM Cube pcakge in the build")
if(STM32_CUBE_PACKAGE_REQUIRED)
    ProcessSTM32CubePackage()
endif()

# if support for littlefs is enabled add it to the build
if(NF_FEATURE_USE_LITTLEFS_OPTION)

    # check if LITTLEFS_SOURCE was specified or if it's empty (default is empty)
    set(NO_LITTLEFS_SOURCE TRUE)
    if(LITTLEFS_SOURCE)
        if(NOT ${LITTLEFS_SOURCE} STREQUAL "")
            set(NO_LITTLEFS_SOURCE FALSE)
        endif()
    endif()

    # LITTLEFS version
    set(LITTLEFS_VERSION_EMPTY TRUE)

    # check if build was requested with a specifc littlefs version
    if(DEFINED LITTLEFS_VERSION)
        if(NOT ${LITTLEFS_VERSION} STREQUAL "")
            set(LITTLEFS_VERSION_EMPTY FALSE)
        endif()
    endif()

    # check if build was requested with a specifc littlefs version
    if(LITTLEFS_VERSION_EMPTY)
        # no littlefs version actualy specified, must be empty which is fine, we'll default to a known good version
        # WHEN CHANGING THIS MAKE SURE TO UPDATE THE DEV CONTAINERS AND PIPELINE YAML
        set(LITTLEFS_VERSION_TAG "v2.9.3")
    else()
        # set version 
        set(LITTLEFS_VERSION_TAG ${LITTLEFS_VERSION})
    endif()

    if(NO_LITTLEFS_SOURCE)
        message(STATUS "littlefs ${LITTLEFS_VERSION_TAG} from GitHub repo")

        FetchContent_Declare(
            littlefs
            GIT_REPOSITORY https://github.com/littlefs-project/littlefs.git
            GIT_TAG ${LITTLEFS_VERSION_TAG}
        )

    else()
        # littlefs source was specified

        message(STATUS "littlefs ${LITTLEFS_VERSION_TAG} (source from: ${LITTLEFS_SOURCE})")

        FetchContent_Declare(
            littlefs
            SOURCE_DIR ${LITTLEFS_SOURCE}
        )

    endif()

    # Check if population has already been performed
    FetchContent_GetProperties(littlefs)
    if(NOT littlefs_POPULATED)
        # Fetch the content using previously declared details
        FetchContent_MakeAvailable(littlefs)
    endif()

endif()

# add target ChibiOS dirs
add_subdirectory(_include)
add_subdirectory(_common)
add_subdirectory(_nanoBooter)
add_subdirectory(_nanoCLR)

# board folder will be added in main CMakeList
