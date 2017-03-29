#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

cmake_minimum_required(VERSION 3.7)
include(CMakeToolsHelpers OPTIONAL)
include(ExternalProject)

# set(CMAKE_VERBOSE_MAKEFILE 1)  # debug helper

# the following prevents launchin a build in the source tree
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# fatal error and message explaining this 
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(STATUS "\n-- ###############################################\n")
    message(STATUS "Please run the build outside of the source tree.\n\n")
    message(STATUS "Hint: create a 'build' folder and run CMake from there..")
    message(STATUS "###############################################\n\n")
    message(FATAL_ERROR "Build launched in the source tree.")
endif()
#########################################


########################################################
# path to local CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)
########################################################


######################################################
# set build type to release if not specified otherwise
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()
######################################################

######################################################
# set time stamp of build
string(TIMESTAMP BUILD_TIMESTAMP UTC)
######################################################

#######################
# handle RTOSes choice

# # sanity check here for invalid RTOS option
# set(RTOS_SUPPORTED CHIBIOS FREERTOS MBEDOS RTXRTOS RTXRTOS2 CACHE INTERNAL "supported RTOS options")
# list(FIND RTOS_SUPPORTED ${RTOS} RTOS_INDEX)
# if(RTOS_INDEX EQUAL -1)
#     message(FATAL_ERROR "\n\nSorry but ${RTOS} is not supported at this time...\nYou can wait for that to be added or you might want to contribute and start working on a PR for that.\n\n")
# endif()

# string(COMPARE EQUAL "CHIBIOS" "${RTOS}"  RTOS_CHIBIOS_CHECK)
# string(COMPARE EQUAL "FREERTOS" "${RTOS}" RTOS_FREERTOS_CHECK)
# string(COMPARE EQUAL "MBEDOS"   "${RTOS}" RTOS_MBEDOS_CHECK)
# string(COMPARE EQUAL "RTXRTOS"  "${RTOS}" RTOS_RTX_RTOS_CHECK)
# string(COMPARE EQUAL "RTXRTOS2" "${RTOS}" RTOS_RTX_RTOS2_CHECK)

##########################################################################
# default RTOS choice is ChibiOS (no other RTOS is supported at this time)
set(RTOS_CHIBIOS_CHECK TRUE)
##########################################################################

if(RTOS_FREERTOS_CHECK) 
    set(RTOS_FREERTOS_CHECK TRUE)
    set(RTOS_CHIBIOS_CHECK FALSE)
    set(RTOS_MBEDOS_CHECK FALSE)
    set(RTOS_RTX_RTOS_CHECK FALSE)
    set(RTOS_RTX_RTOS2_CHECK FALSE)
endif()

if(RTOS_MBEDOS_CHECK) 
    set(RTOS_MBEDOS_CHECK TRUE)
    set(RTOS_CHIBIOS_CHECK FALSE)
    set(RTOS_FREERTOS_CHECK FALSE)
    set(RTOS_RTX_RTOS_CHECK FALSE)
    set(RTOS_RTX_RTOS2_CHECK FALSE)
endif()

if(RTOS_CHIBIOS_CHECK) 
    set(RTOS_CHIBIOS_CHECK TRUE)
    set(RTOS_FREERTOS_CHECK FALSE)
    set(RTOS_MBEDOS_CHECK FALSE)
    set(RTOS_RTX_RTOS_CHECK FALSE)
    set(RTOS_RTX_RTOS2_CHECK FALSE)
endif()

if(RTOS_RTX_RTOS_CHECK) 
    set(RTOS_RTX_RTOS_CHECK TRUE)
    set(RTOS_CHIBIOS_CHECK FALSE)
    set(RTOS_FREERTOS_CHECK FALSE)
    set(RTOS_MBEDOS_CHECK FALSE)
    set(RTOS_RTX_RTOS2_CHECK FALSE)
endif()

if(RTOS_RTX_RTOS2_CHECK) 
    set(RTOS_RTX_RTOS2_CHECK TRUE)
    set(RTOS_CHIBIOS_CHECK FALSE)
    set(RTOS_FREERTOS_CHECK FALSE)
    set(RTOS_MBEDOS_CHECK FALSE)
    set(RTOS_RTX_RTOS_CHECK FALSE)
endif()

#######################


##########################################
# handle FPU preference, if any
set(USE_FPU_IS_TRUE FALSE)

if(USE_FPU)
    if(NOT "${USE_FPU}" STREQUAL "")

        if("${USE_FPU}" STREQUAL "TRUE")
            set(USE_FPU_IS_TRUE TRUE CACHE INTERNAL "FPU preference")
        elseif("${USE_FPU}" STREQUAL "FALSE")
            set(USE_FPU_IS_TRUE FALSE CACHE INTERNAL "FPU preference")
        else()
            # something other that TRUE or FALSE...
            message(FATAL_ERROR "\n\n'${USE_FPU}' is an invalid option for USE_FPU. Acceptable values are TRUE or FALSE.\n\n")
        endif()    

    endif()
endif()

# message("FPU preference is: ${USE_FPU_IS_TRUE}")  # debug helper
##########################################


##########################################
# set default toolchain to GCC
set(TOOLCHAIN GCC)
# have it lower case too for file names
string(TOLOWER TOOLCHAIN_LOWER ${TOOLCHAIN}) 
##########################################

if(RTOS_CHIBIOS_CHECK)

    message(STATUS "\nSetting Toolchain file for ChibiOS \n")
    # set toolchain file
    set(CMAKE_TOOLCHAIN_FILE CMake/toolchain.ChibiOS.${TOOLCHAIN}.cmake)

# elseif(RTOS_MBEDOS_CHECK)

#     message(STATUS "\nSetting Toolchain file for mbed \n")
#     # set toolchain file
#     set(CMAKE_TOOLCHAIN_FILE CMake/toolchain.mbed.${TOOLCHAIN}.cmake)

# else()

#     # find out the chip vendor in order to move on with the appropriate configuration
#     string(REGEX MATCH "^[S][T][M]32" CHIP_VENDOR_STM32 "${TARGET_CHIP}")
#     string(COMPARE EQUAL "STM32" "${CHIP_VENDOR_STM32}" CHIP_VENDOR_STM32_CHECK)

#     if(CHIP_VENDOR_STM32_CHECK)
#         # vendor is ST and toolchain is GCC
#         message(STATUS "Chip vendor is ST. Chip is STM32.")

#         # set CMSIS include directories
#         include_directories(STM32CMSIS_INCLUDE_DIRS)

#         # set toolchain file for cross-compiling with CMake
#         # for this vendor/chip it will be 
#     # elseif(CHIP_VENDOR_??_CHECK)
#     #
#     #     # vendor is ?? and toolchain is GCC
#     #     message("Chip vendor is ??. Chip is ???.")
#     #     set(CMAKE_TOOLCHAIN_FILE CMake/???.cmake)
#     # 
#     else()
#         message(STATUS "\n-- ###############################################\n")
#         message(STATUS "Unknow vendor or chip. Supported vendors/chips:\n-- ST's STM32 (e.g. STM32F407VG)\n")
#         message(STATUS "###############################################\n\n")
#         message(FATAL_ERROR "Unknow vendor or chip in TARGET_CHIP")
#     endif()

#     # set toolchain file
#     set(CMAKE_TOOLCHAIN_FILE CMake/toolchain.${CHIP_VENDOR_STM32}.${TOOLCHAIN}.cmake)
endif()

#########################################


project(nanoFramework VERSION 0.0.0.0)

#######################
message(STATUS "")
message(STATUS "Building nanoFramework version ${PROJECT_VERSION} using build type '${CMAKE_BUILD_TYPE}'.")
message(STATUS "Source directory is '${PROJECT_SOURCE_DIR}'.")
message(STATUS "Build  directory is '${PROJECT_BINARY_DIR}'.")
message(STATUS "Toolchain is '${TOOLCHAIN}'.")
message(STATUS "")
#######################


#################################################################
# nanoFramework features
#################################################################

#################################################################
# debugger (default is TRUE to include debugger in release image
set(NF_FEATURE_DEBUGGER TRUE)

if(NF_FEATURE_DEBUGGER)
    if(NOT "${NF_FEATURE_DEBUGGER}" STREQUAL "")

        if("${NF_FEATURE_DEBUGGER}" STREQUAL "TRUE")
            set(NF_FEATURE_DEBUGGER TRUE CACHE INTERNAL "NF feature DEBUGGER")
        elseif("${NF_FEATURE_DEBUGGER}" STREQUAL "FALSE")
            set(NF_FEATURE_DEBUGGER FALSE CACHE INTERNAL "NF feature DEBUGGER")
        else()
            # something other that TRUE or FALSE...
            message(FATAL_ERROR "\n\n'${NF_FEATURE_DEBUGGER}' is an invalid option for NF_FEATURE_DEBUGGER. Acceptable values are TRUE or FALSE.\n\n")
        endif()    

    endif()
endif()

#################################################################


#######################
# ChibiOS
if(RTOS_CHIBIOS_CHECK)

    # check if CHIBIOS_SOURCE was specified or if it's empty (default is empty)
    set(NO_CHIBIOS_SOURCE TRUE)
    if(CHIBIOS_SOURCE)
        if(NOT "${CHIBIOS_SOURCE}" STREQUAL "")
            set(NO_CHIBIOS_SOURCE FALSE)
        endif()
    endif()

    if(NO_CHIBIOS_SOURCE)
        # no CHIBIOS source specified, download it from it's repo

        # hack to make the FindGit to work in Windows platforms (check the module comment for details)
        include(Hack_SetGitSearchPath)

        # check for Git (needed here for advanced warning to user if it's not installed)
        find_package(Git)

        #  check if Git was found, if not report to user and abort
        if(NOT GIT_EXECUTABLE)
        message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
        endif()

        # check if build was requested with a specifc ChibiOS version
        if(DEFINED CHIBIOS_VERSION)

            if("${CHIBIOS_VERSION}" STREQUAL "")
                set(CHIBIOS_VERSION_EMPTY TRUE)
            endif()

            if(CHIBIOS_VERSION_EMPTY)
                # no ChibiOS version actualy specified, must be empty which is fine, we'll grab the code from master branch
                message(STATUS "RTOS is: CHIBIOS (latest available code from trunk)")
                set(CHIBIOS_GIT_TAG "master")
            else()
                message(STATUS "RTOS is: ChibiRTOS v${CHIBIOS_VERSION}")

                # branch naming follows the pattern 'stable_16.1.x'
                # need to extract the major and minor numbers from the CHIBIOS_VERSION parameter 
                # find 1st dot in version number
                string(FIND ${CHIBIOS_VERSION} "." CHIBIOS_VERSION_DOT_INDEX)
                # find 2nd dot in version number
                string(FIND ${CHIBIOS_VERSION} "." CHIBIOS_VERSION_DOT_INDEX REVERSE)
                # extract 'short' version  
                string(SUBSTRING ${CHIBIOS_VERSION} 0 ${CHIBIOS_VERSION_DOT_INDEX} CHIBIOS_SHORT_VERSION)

                # set branch name
                set(CHIBIOS_GIT_TAG "stable_${CHIBIOS_SHORT_VERSION}.x")
            endif()
            
        endif()

        # need to setup a separate CMake project to download the code from the GitHub repository
        # otherwise it won't be available before the actual build step
        configure_file("CMake/ChibiOS.CMakeLists.cmake.in"
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
            GIT_REPOSITORY  https://github.com/ChibiOS/ChibiOS/
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

            file(COPY "${CHIBIOS_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/ChibiOS_Source")
            set(CHIBIOS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/ChibiOS_Source/include)
        else()
            message(FATAL_ERROR "Couldn't find ChibiOS source at ${CHIBIOS_SOURCE}/")
        endif()

    endif()

    # set CMSIS RTOS include directory
    include_directories( ${CMSIS_RTOS_INCLUDE_DIR})
   
        # Assume no community board ... until proven otherwise
    set(CHIBIOS_COMMUNITY_TARGET FALSE CACHE INTERNAL "Community target flag")
    # try to find board in chibios source 
    if(EXISTS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/boards/${CHIBIOS_BOARD})
        # Board found, nothing to do
    else()
        if(EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/boards/${CHIBIOS_BOARD})
            # board found in overlay, nothing to do 
        else()
            if(EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD})
                # board found in targets, nothing to do
            else()
                # try to find board in the Community targets folder
                if(EXISTS ${PROJECT_SOURCE_DIR}/targets-community/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD})
                    # set flag for this being a community board
                    set(CHIBIOS_COMMUNITY_TARGET TRUE CACHE INTERNAL "Community target flag")
                endif()
            endif()
        endif()
    endif()

    # add target CMSIS OS folders
    add_subdirectory(targets/CMSIS-OS/common)
    add_subdirectory(targets/CMSIS-OS/nanoBooter)
    add_subdirectory(targets/CMSIS-OS/nanoCLR)

    # add target ChibiOS dirs
    add_subdirectory(targets/CMSIS-OS/ChibiOS/Include)
    add_subdirectory(targets/CMSIS-OS/ChibiOS/common)
    add_subdirectory(targets/CMSIS-OS/ChibiOS/nanoBooter)
    add_subdirectory(targets/CMSIS-OS/ChibiOS/nanoCLR)

    # try to find board in source 
    if(EXISTS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/boards/${CHIBIOS_BOARD})
        # board found
        message(STATUS "ChibiOS board '${CHIBIOS_BOARD}' found")
    else()

        # board NOT found in source
        # try to find it in nanoFramework ChibiOS 'overlay'
        if(EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/boards/${CHIBIOS_BOARD})
            # board found
            message(STATUS "ChibiOS board '${CHIBIOS_BOARD}' found in nanoFramework overlay")
        else()

            # board NOT found in ChibiOS 'overlay'
            # try to find it in the target boards
            if(EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD})
                # board found!
                # in this case it's mandatory that the board definitions (board.c and board.h) are present in the target folder
                if( EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}/board.c AND
                    EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}/board.h)
                    # everything seems to be in order
                    message(STATUS "ChibiOS board '${CHIBIOS_BOARD}' (including board definition files) found in nanoFramework targets")
                else()
                    message(FATAL_ERROR "\n\nSorry but the board definition files (board.c and board.h) for ${CHIBIOS_BOARD} seem to be missing in the target directory...\n\n")
                endif()

            else()
                # board NOT found in target boards
                # try to find it in the Community target boards
                if(EXISTS ${PROJECT_SOURCE_DIR}/targets-community/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD})
                    # board found!
                    # in this case it's mandatory that the board definitions (board.c and board.h) are present in the target folder
                    if( EXISTS ${PROJECT_SOURCE_DIR}/targets-community/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}/board.c AND
                        EXISTS ${PROJECT_SOURCE_DIR}/targets-community/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}/board.h)
                        # everything seems to be in order
                        message(STATUS "ChibiOS board '${CHIBIOS_BOARD}' (including board definition files) found in Community targets")
                    else()
                        message(FATAL_ERROR "\n\nSorry but the board definition files (board.c and board.h) for ${CHIBIOS_BOARD} seem to be missing in any of the target directories...\n\n")
                    endif()
                else()
                    message(FATAL_ERROR "\n\nSorry but ${CHIBIOS_BOARD} seems to be missing in the available list of the ChibiOS supported boards...\n\n")
                endif()

            endif()

        endif()

    endif()

    # try to find board in the targets folder
    if(EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD})
        # board found
        message(STATUS "Support for target board '${CHIBIOS_BOARD}' found")
        
        # add TARGET board directory
        add_subdirectory("targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}")

    else()

        # try to find board in the Community targets folder
        if(EXISTS ${PROJECT_SOURCE_DIR}/targets-community/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD})
            # board found
            message(STATUS "Support for target board '${CHIBIOS_BOARD}' found in Community targets")

            # add TARGET board directory from Community
            add_subdirectory("targets-community/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}")

        else()
        # board NOT found in targets folder
            message(FATAL_ERROR "\n\nSorry but support for ${CHIBIOS_BOARD} target is not available...\n\You can wait for that to be added or you might want to contribute and start working on a PR for that.\n\n")
        endif()

    endif()

endif()

# #######################
# # FreeRTOS
# elseif(RTOS_FREERTOS_CHECK)

#     # check if FREERTOS_SOURCE was specified or if it's empty (default is empty)
#     set(NO_FREERTOS_SOURCE TRUE)
#     if(FREERTOS_SOURCE)
#         if(NOT "${FREERTOS_SOURCE}" STREQUAL "")
#             set(NO_FREERTOS_SOURCE FALSE)
#         endif()
#     endif()

#     if(NO_FREERTOS_SOURCE)
#         # no FreeRTOS source specified, download it from it's repo

#         # check for SVN (needed here for advanced warning to user if it's not installed)
#         find_package(Subversion)

#         #  check is SVN was found, if not report to user and abort
#         if(NOT Subversion_SVN_EXECUTABLE)
#         message(FATAL_ERROR "error: could not find SVN, make sure you have it installed.")
#         endif()

#         # check if build was requested with a specifc FreeRTOS version
#         if(DEFINED FREERTOS_VERSION)

#             if("${FREERTOS_VERSION}" STREQUAL "")
#                 set(FREERTOS_VERSION_EMPTY TRUE)
#             endif()

#             if(FREERTOS_VERSION_EMPTY)
#                 # no FreeRTOS version actualy specified, must be empty which is fine, we'll grab the code from the trunk
#                 message(STATUS "RTOS is: FreeRTOS (latest available code from trunk)")
#                 set(FREERTOS_SVN_REPOSITORY "https://svn.code.sf.net/p/freertos/code/trunk")
#             else()
#                 message(STATUS "RTOS is: FreeRTOS v${FREERTOS_VERSION}")
#                 set(FREERTOS_SVN_REPOSITORY "https://svn.code.sf.net/p/freertos/code/tags/V${FREERTOS_VERSION}")
#             endif()
            
#         endif()

#         # need to setup a separate CMake project to download the code from the SVN repository
#         # otherwise it won't be available before the actual build step
#         configure_file("CMake/FreeRTOS.CMakeLists.cmake.in"
#                     "${CMAKE_BINARY_DIR}/FreeRTOS_Download/CMakeLists.txt")
        
#         # setup CMake project for FreeRTOS download
#         execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
#                         RESULT_VARIABLE result
#                         WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/FreeRTOS_Download")

#         # run build on FreeRTOS download CMake project to perform the download
#         execute_process(COMMAND ${CMAKE_COMMAND} --build .
#                         RESULT_VARIABLE result
#                         WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/FreeRTOS_Download")
        
#         # add FreeRTOS as external project
#         ExternalProject_Add( 
#             FreeRTOS
#             PREFIX FreeRTOS
#             SOURCE_DIR ${CMAKE_BINARY_DIR}/FreeRTOS_Source
#             SVN_REPOSITORY ${FREERTOS_SVN_REPOSITORY}/FreeRTOS/Source
#             TIMEOUT 10
#             LOG_DOWNLOAD 1
#             # Disable all other steps
#             INSTALL_COMMAND ""
#             CONFIGURE_COMMAND ""
#             BUILD_COMMAND ""
#         )

#         # get source dir for FreeRTOS CMake project
#         ExternalProject_Get_Property(FreeRTOS SOURCE_DIR)
#         set(FREERTOS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/FreeRTOS_Source/include)

#     else()
#         # FreeRTOS source was specified

#         # sanity check is source path exists
#         if(EXISTS "${FREERTOS_SOURCE}/")
#             message(STATUS "RTOS is: FreeRTOS (source from: ${FREERTOS_SOURCE})")

#             file(COPY "${FREERTOS_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/FreeRTOS_Source")
#             set(FREERTOS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/FreeRTOS_Source/include)
#         else()
#             message(FATAL_ERROR "Couldn't find FreeRTOS source at ${FREERTOS_SOURCE}/")
#         endif()

#     endif()

#     # set CMSIS RTOS include directory
#     include_directories( ${CMSIS_RTOS_INCLUDE_DIR})
   
#     # add source directory
#     add_subdirectory("src")

# #######################
# # mbed OS
# elseif(RTOS_MBEDOS_CHECK)

#     # check if MBEDOS_SOURCE was specified or if it's empty (default is empty)
#     set(NO_MBEDOS_SOURCE TRUE)
#     if(MBEDOS_SOURCE)
#         if(NOT "${MBEDOS_SOURCE}" STREQUAL "")
#             set(NO_MBEDOS_SOURCE FALSE)
#         endif()
#     endif()

#     # check is a target was specified
#     if(MBED_TARGET)
#         if(NOT "${MBEDOS_SOURCE}" STREQUAL "")
#             message(FATAL_ERROR "\n\nYou need to specify a valid ${MBED_TARGET} when mbed OS is the RTOS option\n\n")
#         endif()
#     else()
#         message(FATAL_ERROR "\n\nYou need to specify a valid ${MBED_TARGET} when mbed OS is the RTOS option\n\n")
#     endif()

#     if(NO_MBEDOS_SOURCE)
#         # no mbed RTOS source specified, download it from it's repo

#         # hack to make the FindGit to work in Windows platforms (check the module comment for details)
#         include(Hack_SetGitSearchPath)

#         # check for Git (needed here for advanced warning to user if it's not installed)
#         find_package(Git)

#         #  check if Git was found, if not report to user and abort
#         if(NOT GIT_EXECUTABLE)
#         message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
#         endif()

#         message(STATUS "RTOS is: mbed RTOS (latest available code from GitHub repo)")

#         # need to setup a separate CMake project to download the code from the SVN repository
#         # otherwise it won't be available before the actual build step
#         configure_file("CMake/mbedOS.CMakeLists.cmake.in"
#                     "${CMAKE_BINARY_DIR}/mbedOS_Download/CMakeLists.txt")
        
#         # setup CMake project for mbed RTOS download
#         execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
#                         RESULT_VARIABLE result
#                         WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/mbedOS_Download")

#         # run build on mbed RTOS download CMake project to perform the download
#         execute_process(COMMAND ${CMAKE_COMMAND} --build .
#                         RESULT_VARIABLE result
#                         WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/mbedOS_Download")
               
#         # download mbedOS source from official GitHub repo
#         ExternalProject_Add( 
#             mbedOS
#             PREFIX mbed-OS
#             SOURCE_DIR ${CMAKE_BINARY_DIR}/mbedOS_Source
#             GIT_REPOSITORY  https://github.com/ARMmbed/mbed-os/
#             GIT_TAG master  # target master branch
#             GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
#             TIMEOUT 10
#             LOG_DOWNLOAD 1
            
#             # Disable all other steps
#             CONFIGURE_COMMAND ""
#             BUILD_COMMAND ""
#             INSTALL_COMMAND ""
#         )

#         # get source dir for mbed RTOS CMake project
#         # ExternalProject_Get_Property(mbedOS SOURCE_DIR)
#         # set(MBEDOS_INCLUDE_DIRS ${MBEDOS_SOURCE_DIR})

#         # set update disconnected to prevent updates on builds
#         set_property(DIRECTORY ${MBEDOS_SOURCE_DIR} PROPERTY EP_UPDATE_DISCONNECTED 1)

#     else()
#         # mbed OS source location was specified

#         if(NOT EXISTS "${PROJECT_BINARY_DIR}/mbedOS_Source")
#             # need to copy srouce to build directory
#             message(STATUS "RTOS is: mbed OS (copying source from: ${MBEDOS_SOURCE})")

#             file(COPY "${MBEDOS_SOURCE}/" DESTINATION "${PROJECT_BINARY_DIR}/mbedOS_Source")
#         elseif()
#             # source directory  already at build folder so we are done here
#             message(STATUS "RTOS is: mbed OS (source from: ${MBEDOS_SOURCE} already at build directory)")
#         endif()

#         # set(MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mbedOS_Source)

#     endif()

#     # find_package(MBEDOPTIONS REQUIRED)
   
#     # add source directory
#     add_subdirectory("targets/os/mbed-os/nanoBooter")

# #######################
# # RTX RTOS
# elseif(RTOS_RTX_RTOS_CHECK)

#     # hack to make the FindGit to work in Windows platforms (check the module comment for details)
#     include(Hack_SetGitSearchPath)

#     # check for Git (needed here for advanced warning to user if it's not installed)
#     find_package(Git)

#     #  check if Git was found, if not report to user and abort
#     if(NOT GIT_EXECUTABLE)
#       message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
#     endif()

#     # need to setup a separate CMake project to download the code from the SVN repository
#     # otherwise it won't be available before the actual build step
#     configure_file("CMake/RTXRTOS.CMakeLists.cmake.in"
#                    "${CMAKE_BINARY_DIR}/RTXRTOS_Download/CMakeLists.txt")
    
#     # setup CMake project for RTX RTOS download
#     execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
#                     RESULT_VARIABLE result
#                     WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/RTXRTOS_Download")

#     # run build on RTX RTOS download CMake project to perform the download
#     execute_process(COMMAND ${CMAKE_COMMAND} --build .
#                     RESULT_VARIABLE result
#                     WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/RTXRTOS_Download")
    
#     # download RTX RTOS source from official GitHub repo
#     ExternalProject_Add( 
#         RTXRTOS
#         PREFIX RTX-RTOS
#         SOURCE_DIR ${CMAKE_BINARY_DIR}/RTXRTOS_Source
#         GIT_REPOSITORY  https://github.com/ARM-software/CMSIS_5
#         GIT_TAG master  # target master branch
#         GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
#         TIMEOUT 10
#         LOG_DOWNLOAD 1
        
#         # Disable all other steps
#         CONFIGURE_COMMAND ""
#         BUILD_COMMAND ""
#         INSTALL_COMMAND ""
#     )

#     # get source dir for RTX RTOS CMake project
#     ExternalProject_Get_Property(RTXRTOS SOURCE_DIR)
#     set(RTXRTOS_INCLUDE_DIRS ${RTXRTOS_SOURCE_DIR})
   
#     # add source directory
#     add_subdirectory("src")

# #######################
# # RTX RTOS2
# elseif(RTOS_RTX_RTOS2_CHECK)

#     message(FATAL_ERROR "
# #############################################################
# support for RTX RTOS2 is in the works, please check latter...
# #############################################################")

#     # hack to make the FindGit to work in Windows platforms (check the module comment for details)
#     include(Hack_SetGitSearchPath)

#     # check for Git (needed here for advanced warning to user if it's not installed)
#     find_package(Git)

#     #  check if Git was found, if not report to user and abort
#     if(NOT GIT_EXECUTABLE)
#       message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
#     endif()

#     # need to setup a separate CMake project to download the code from the SVN repository
#     # otherwise it won't be available before the actual build step
#     configure_file("CMake/RTXRTOS.CMakeLists.cmake.in"
#                    "${CMAKE_BINARY_DIR}/RTXRTOS_Download/CMakeLists.txt")
    
#     # setup CMake project for RTX RTOS2 download (same as RTX RTOS)
#     execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
#                     RESULT_VARIABLE result
#                     WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/RTXRTOS_Download")

#     # run build on RTX RTOS2 download CMake project to perform the download
#     execute_process(COMMAND ${CMAKE_COMMAND} --build .
#                     RESULT_VARIABLE result
#                     WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/RTXRTOS_Download")
    
#     # download RTX RTOS2 source from official GitHub repo (same as RTX RTOS)
#     ExternalProject_Add( 
#         RTXRTOS2
#         PREFIX RTX-RTOS
#         SOURCE_DIR ${CMAKE_BINARY_DIR}/RTXRTOS_Source
#         GIT_REPOSITORY  https://github.com/ARM-software/CMSIS_5
#         GIT_TAG master  # target master branch
#         GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
#         TIMEOUT 10
#         LOG_DOWNLOAD 1
        
#         # Disable all other steps
#         CONFIGURE_COMMAND ""
#         BUILD_COMMAND ""
#         INSTALL_COMMAND ""
#     )

#     # get source dir for RTX RTOS2 CMake project
#     ExternalProject_Get_Property(RTXRTOS2 SOURCE_DIR)
#     set(RTXRTOS2_INCLUDE_DIRS ${RTXRTOS2_SOURCE_DIR})
   
#     # add source directory
#     add_subdirectory("src")

# #######################

# # no RTOS specifed
# else()
    
#     set(NORTOS TRUE)

#     message(STATUS "NO RTOS was specified")

#     # include CMSIS, HAL and drivers  
#     if(CHIP_VENDOR_STM32_CHECK)
#         # vendor is ST, chip is STM32

#         # ST Cube package with CMSIS and SMT32 HAL
#         add_subdirectory("stcube_repository")

#     # elseif(CHIP_VENDOR_??_CHECK)
#     #
#     #     # vendor is ?? and toolchain is GCC
#     #     add_subdirectory("???")
#     endif()    
   
#     # add source directory
#     add_subdirectory("src")
 
# endif()
