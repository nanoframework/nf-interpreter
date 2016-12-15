cmake_minimum_required(VERSION 3.0)
include(CMakeToolsHelpers OPTIONAL)
include(ExternalProject)

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


##########################################
# set default toolchain to GCC
set(TOOLCHAIN GCC)
# have it lower case too for file names
string(TOLOWER TOOLCHAIN_LOWER ${TOOLCHAIN}) 
##########################################

# find out the chip vendor in order to move on with the appropriate configuration
string(REGEX MATCH "^[S][T][M]32" CHIP_VENDOR_STM32 "${TARGET_CHIP}")
string(COMPARE EQUAL "STM32" "${CHIP_VENDOR_STM32}" CHIP_VENDOR_STM32_CHECK)

if(CHIP_VENDOR_STM32_CHECK)
    # vendor is ST and toolchain is GCC
    message(STATUS "Chip vendor is ST. Chip is STM32.")
    # set toolchain file for cross-compiling with CMake
    # for this vendor/chip it will be 
# elseif(CHIP_VENDOR_??_CHECK)
#
#     # vendor is ?? and toolchain is GCC
#     message("Chip vendor is ??. Chip is ???.")
#     set(CMAKE_TOOLCHAIN_FILE CMake/???.cmake)
# 
else()
    message(STATUS "\n-- ###############################################\n")
    message(STATUS "Unknow vendor or chip. Supported vendors/chips:\n-- ST's STM32 (e.g. STM32F407VG)\n")
    message(STATUS "###############################################\n\n")
    message(FATAL_ERROR "Unknow vendor or chip in TARGET_CHIP")
endif()

# set toolchain file
set(CMAKE_TOOLCHAIN_FILE CMake/toolchain.${CHIP_VENDOR_STM32}.${TOOLCHAIN}.cmake)

#########################################


project(nanoFramework VERSION 0.0.0)

#######################
message(STATUS "")
message(STATUS "Building nano Framework version ${PROJECT_VERSION} using build type '${CMAKE_BUILD_TYPE}'.")
message(STATUS "Source directory is '${PROJECT_SOURCE_DIR}'.")
message(STATUS "Build  directory is '${PROJECT_BINARY_DIR}'.")
message(STATUS "Toolchain is '${TOOLCHAIN}'.")
message(STATUS "")
#######################


# include CMSIS, HAL and drivers  
if(CHIP_VENDOR_STM32_CHECK)
    # vendor is ST, chip is STM32

    # ST Cube package with CMSIS and SMT32 HAL
    add_subdirectory("stcube_repository")

# elseif(CHIP_VENDOR_??_CHECK)
#
#     # vendor is ?? and toolchain is GCC
#     add_subdirectory("???")
endif()

#######################
# handle RTOSes stuff

string(COMPARE EQUAL "FREERTOS" "${RTOS}" RTOS_FREERTOS_CHECK)
string(COMPARE EQUAL "MBEDRTOS" "${RTOS}" RTOS_MBED_RTOS_CHECK)

#######################
# FreeRTOS
if(RTOS_FREERTOS_CHECK)

    # check for SVN (needed here for advanced warning to user if it's not installed)
    find_package(Subversion)

    #  check is SVN was found, if not report to user and abort
    if(NOT Subversion_SVN_EXECUTABLE)
      message(FATAL_ERROR "error: could not find SVN, make sure you have it installed.")
    endif()

    # check if build was requested with a specifc FreeRTOS version
    if(DEFINED FREERTOS_VERSION)

        if("${FREERTOS_VERSION}" STREQUAL "")
            set(FREERTOS_VERSION_EMPTY TRUE)
        endif()

        if(FREERTOS_VERSION_EMPTY)
            # no FreeRTOS version actualy specified, must be empty which is fine, we'll grab the code from the trunk
            message(STATUS "RTOS is: FreeRTOS (latest available code from trunk)")
            set(FREERTOS_SVN_REPOSITORY "https://svn.code.sf.net/p/freertos/code/trunk")
        else()
            message(STATUS "RTOS is: FreeRTOS v${FREERTOS_VERSION}")
            set(FREERTOS_SVN_REPOSITORY "https://svn.code.sf.net/p/freertos/code/tags/V${FREERTOS_VERSION}")
        endif()
        
    endif()

    # download FreeRTOS source from official SVN repo
    ExternalProject_Add( 
        FreeRTOS
        PREFIX FreeRTOS
        SOURCE_DIR ${CMAKE_BINARY_DIR}/FreeRTOS-Source
        SVN_REPOSITORY ${FREERTOS_SVN_REPOSITORY}/FreeRTOS/Source
        TIMEOUT 10
        LOG_DOWNLOAD 1
        # Disable all other steps
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
    )

    # get source dir for FreeRTOS CMake project
    ExternalProject_Get_Property(FreeRTOS SOURCE_DIR)
    set(FREERTOS_INCLUDE_DIRS ${FREERTOS_SOURCE_DIR})

#######################
# mBed RTOS
elseif(RTOS_MBED_RTOS_CHECK)

    # hack to make the FindGit to work in Windows platforms (check the module comment for details)
    include(Hack_SetGitSearchPath)

    # check for Git (needed here for advanced warning to user if it's not installed)
    find_package(Git)

    #  check if Git was found, if not report to user and abort
    if(NOT GIT_EXECUTABLE)
      message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
    endif()

    # download FreeRTOS source from official SVN repo
    ExternalProject_Add( 
        mBedRTOS
        PREFIX mBed-RTOS
        SOURCE_DIR ${CMAKE_BINARY_DIR}/mBed-RTOS-Source
        GIT_REPOSITORY  https://github.com/ARMmbed/mbed-os/
        GIT_TAG master  # target master branch
        GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
        TIMEOUT 10
        LOG_DOWNLOAD 1
        
        # Disable all other steps
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
    )

    # get source dir for mBed RTOS CMake project
    ExternalProject_Get_Property(mBedRTOS SOURCE_DIR)
    set(MBEDRTOS_INCLUDE_DIRS ${MBEDRTOS_SOURCE_DIR})

#######################
# no RTOS specifed
else()

    message(STATUS "NO RTOS was specified")

endif()

# add source directory
add_subdirectory("source")
