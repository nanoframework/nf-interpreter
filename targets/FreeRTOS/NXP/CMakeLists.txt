#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

include(nf_utils)

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

# check if CMSIS_SOURCE was specified or if it's empty (default is empty)
set(NO_CMSIS_SOURCE TRUE)
if(CMSIS_SOURCE)
    if(NOT "${CMSIS_SOURCE}" STREQUAL "")
        set(NO_CMSIS_SOURCE FALSE)
    endif()
endif()

if(NO_CMSIS_SOURCE)

    # check for Git (needed here for advanced warning to user if it's not installed)
    find_package(Git)

    #  check if Git was found, if not report to user and abort
    if(NOT GIT_EXECUTABLE)
        message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
    endif()

    # CMSIS version
    set(CMSIS_VERSION_EMPTY TRUE)

    # check if build was requested with a specifc CMSIS version
    if(DEFINED CMSIS_VERSION)
        if(NOT "${CMSIS_VERSION}" STREQUAL "")
            set(CMSIS_VERSION_EMPTY FALSE)
        endif()
    endif()

    # check if build was requested with a specifc CMSIS version
    if(CMSIS_VERSION_EMPTY)
        # no CMSIS version actualy specified, must be empty which is fine, we'll default to a known good version
        set(CMSIS_GIT_TAG "5.5.1")
    else()
        # set Git tag
        set(CMSIS_GIT_TAG "${CMSIS_VERSION}")
    endif()            

    message(STATUS "CMSIS v${CMSIS_GIT_TAG} from GitHub repo")

    # need to setup a separate CMake project to download the code from the GitHub repository
    # otherwise it won't be available before the actual build step
    configure_file( ${PROJECT_SOURCE_DIR}/CMake/CMSIS.CMakeLists.cmake.in
                    ${CMAKE_BINARY_DIR}/CMSIS_Download/CMakeLists.txt)

    # setup CMake project for CMSIS download
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/CMSIS_Download")

                    
    # run build on CMSIS download CMake project to perform the download
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/CMSIS_Download")

    # add CMSIS as external project
    ExternalProject_Add( 
        CMSIS
        PREFIX CMSIS
        SOURCE_DIR ${CMAKE_BINARY_DIR}/CMSIS_Source
        GIT_REPOSITORY  https://github.com/ARM-software/CMSIS_5.git
        GIT_TAG ${CMSIS_GIT_TAG}  # target specified branch
        GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
        TIMEOUT 10
        LOG_DOWNLOAD 1
        # Disable all other steps
        INSTALL_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
    )  
else()
    # CMSIS source was specified

    # sanity check is source path exists
    if(EXISTS "${CMSIS_SOURCE}/")
        message(STATUS "CMSIS (source from: ${CMSIS_SOURCE})")

        # check if we already have the sources, no need to copy again
        if(NOT EXISTS "${CMAKE_BINARY_DIR}/CMSIS_Source")
            file(COPY "${CMSIS_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/CMSIS_Source")
        else()
            message(STATUS "Using local cache of CMSIS source from ${CMSIS_SOURCE}")
        endif()

        set(CMSIS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/CMSIS_Source/include)
    else()
        message(FATAL_ERROR "Couldn't find CMSIS source at ${CMSIS_SOURCE}/")
    endif()

    # add CMSIS as external project
    ExternalProject_Add(
        CMSIS
        PREFIX CMSIS
        SOURCE_DIR ${CMAKE_BINARY_DIR}/CMSIS_Source
        # Disable all other steps
        INSTALL_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
    )        

    # get source dir for CMSIS CMake project
    ExternalProject_Get_Property(CMSIS SOURCE_DIR)

endif()

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
        INSTALL_COMMAND ${CMAKE_COMMAND} -E remove ${PROJECT_BINARY_DIR}/FatFS_Source/source/ffconf.h

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
        INSTALL_COMMAND ${CMAKE_COMMAND} -E remove ${PROJECT_BINARY_DIR}/FatFS_Source/source/ffconf.h

        # Disable all other steps
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
        NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/LWIP_Source/ SOURCE_EXISTS)

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
