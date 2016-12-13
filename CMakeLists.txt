cmake_minimum_required(VERSION 3.0)
include(CMakeToolsHelpers OPTIONAL)

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

# add source directory
add_subdirectory("source")
