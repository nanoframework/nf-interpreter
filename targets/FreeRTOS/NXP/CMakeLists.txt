#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

include(nf_utils)

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
        GIT_REPOSITORY https://github.com/MateuszKlatecki/fatfs.git
        GIT_TAG ${FATFS_VERSION_TAG}  # target specified branch
        GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
        TIMEOUT 10
        LOG_DOWNLOAD 1
        # Disable all other steps
        INSTALL_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
    )

else()
    # FatFS source was specified

    # sanity check is source path exists
    if(EXISTS "${FATFS_SOURCE}/")
        message(STATUS "FatFS ${FATFS_VERSION} (source from: ${FATFS_SOURCE})")

        # check if we already have the sources, no need to copy again
        NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/FatFS_Source SOURCE_EXISTS)

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
        # Disable all other steps
        INSTALL_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
    )        

    # get source dir for FatFS CMake project
    ExternalProject_Get_Property(FatFS SOURCE_DIR)

endif()

# check if LWIP_SOURCE was specified or if it's empty (default is empty)
set(NO_LWIP_SOURCE TRUE)
if(LWIP_SOURCE)
    if(NOT "${LWIP_SOURCE}" STREQUAL "")
        set(NO_LWIP_SOURCE FALSE)
    endif()
endif()

if(NO_LWIP_SOURCE)

    # check for Git (needed here for advanced warning to user if it's not installed)
    find_package(Git)

    #  check if Git was found, if not report to user and abort
    if(NOT GIT_EXECUTABLE)
        message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
    endif()

    # LWIP version
    set(LWIP_VERSION_EMPTY TRUE)

    # check if build was requested with a specifc LWIP version
    if(DEFINED LWIP_VERSION)
        if(NOT "${LWIP_VERSION}" STREQUAL "")
            set(LWIP_VERSION_EMPTY FALSE)
        endif()
    endif()

    # check if build was requested with a specifc LWIP version
    if(LWIP_VERSION_EMPTY)
        # no LWIP version actualy specified, must be empty which is fine, we'll default to a known good version
        set(LWIP_GIT_TAG "STABLE-2_0_3_RELEASE")
    else()
        # set Git tag
        set(LWIP_GIT_TAG "${LWIP_VERSION}")
    endif()            

    message(STATUS "LWIP ${LWIP_VERSION} from savannah.nongnu.org repo")

    # need to setup a separate CMake project to download the code from the GitHub repository
    # otherwise it won't be available before the actual build step
    configure_file("${PROJECT_SOURCE_DIR}/CMake/LWIP.CMakeLists.cmake.in"
                "${CMAKE_BINARY_DIR}/LWIP_Download/CMakeLists.txt")

    # setup CMake project for LWIP download
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/LWIP_Download")

    # run build on LWIP download CMake project to perform the download
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/LWIP_Download")

    # add LWIP as external project
    ExternalProject_Add( 
        LWIP
        PREFIX LWIP
        SOURCE_DIR ${CMAKE_BINARY_DIR}/LWIP_Source
        GIT_REPOSITORY  https://git.savannah.nongnu.org/git/lwip.git
        GIT_TAG ${LWIP_GIT_TAG}  # target specified branch
        GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
        TIMEOUT 10
        LOG_DOWNLOAD 1
        # Disable all other steps
        INSTALL_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
    )  
else()
    # LWIP source was specified

    # sanity check is source path exists
    if(EXISTS "${LWIP_SOURCE}/")
        message(STATUS "LWIP ${LWIP_VERSION} (source from: ${LWIP_SOURCE})")

        # check if we already have the sources, no need to copy again
        NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/LWIP_Source SOURCE_EXISTS)

        if(NOT ${SOURCE_EXISTS})
            file(COPY "${LWIP_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/LWIP_Source")
        else()
            message(STATUS "Using local cache of LWIP source from ${LWIP_SOURCE}")
        endif()
    else()
        message(FATAL_ERROR "Couldn't find LWIP source at ${LWIP_SOURCE}/")
    endif()

    # add LWIP as external project
    ExternalProject_Add(
        LWIP
        PREFIX LWIP
        SOURCE_DIR ${CMAKE_BINARY_DIR}/LWIP_Source
        # Disable all other steps
        INSTALL_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
    )        

    # get source dir for LWIP CMake project
    ExternalProject_Get_Property(LWIP SOURCE_DIR)

endif()

# append FatFS files
if(USE_FILESYSTEM_OPTION)
    list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/fatfs/diskio.c")
    list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/fatfs/fsl_sd_disk.c")
    list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/fatfs/ffsystem.c")

    list(APPEND NANOCLR_PROJECT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/fatfs)

endif()

# append networking files, if enabled
if(USE_NETWORKING_OPTION)
    list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/LwIP/ethernetif.c")

    list(APPEND NANOCLR_PROJECT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/LwIP)

    # append mbed TLS entropy generator, if hardware has it
    # if(NF_SECURITY_MBEDTLS AND USE_RNG)
    #     list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/mbedtls_entropy_hardware_pool.c")
    # endif()

endif()

# make vars global
set(NANOCLR_PROJECT_SOURCES ${NANOCLR_PROJECT_SOURCES} CACHE INTERNAL "make global")
set(NANOCLR_PROJECT_INCLUDE_DIRS ${NANOCLR_PROJECT_INCLUDE_DIRS} CACHE INTERNAL "make global")


list(APPEND TARGET_FREERTOS_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

# add platform dirs
add_subdirectory(include)
add_subdirectory(common)
add_subdirectory(nanoBooter)
add_subdirectory(nanoCLR)
