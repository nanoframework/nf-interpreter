#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.arm-none-eabi)
include(binutils.AzureRTOS)
include(nf_utils)

# check if RTOS_SOURCE_FOLDER was specified or if it's empty (default is empty)
set(NO_RTOS_SOURCE_FOLDER TRUE)
if(RTOS_SOURCE_FOLDER)
    if(NOT "${RTOS_SOURCE_FOLDER}" STREQUAL "")
        set(NO_RTOS_SOURCE_FOLDER FALSE)
    endif()
endif()

# if(NOT "${TOOL_SRECORD_PREFIX}" STREQUAL "")
#     if(NOT EXISTS ${TOOL_SRECORD_PREFIX}/srec_cat.exe)
#         message(STATUS "")
#         message(STATUS "Couldn't find the srec_cat tool at the specified path: ${TOOL_SRECORD_PREFIX}/srec_cat.exe")
#         message(STATUS "Make sure that the CMake option TOOL_SRECORD_PREFIX has the correct path.")
#         message(STATUS "If you don't have this tool download it from hhttps://sourceforge.net/projects/srecord/files/srecord-win32/")
#         message(STATUS "")
#         message(FATAL_ERROR "srec_cat tool not found")
#     else()
#         set(SRECORD_TOOL_AVAILABLE TRUE CACHE INTERNAL "srec_cat tool available")
#     endif()
# endif()

if(NO_RTOS_SOURCE_FOLDER)
    # no AzureRTOS source specified, download it from it's repo
    
    # check for Git (needed here for advanced warning to user if it's not installed)
    find_package(Git)

    #  check if Git was found, if not report to user and abort
    if(NOT GIT_EXECUTABLE)
        message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
    endif()

    # AzureRTOS version
    set(RTOS_VERSION_EMPTY TRUE)

    # check if build was requested with a specifc AzureRTOS version
    if(DEFINED RTOS_VERSION)
        if(NOT "${RTOS_VERSION}" STREQUAL "")
            set(RTOS_VERSION_EMPTY FALSE)
        endif()
    endif()

    # check if build was requested with a specifc AzureRTOS version
    if(RTOS_VERSION_EMPTY)
        # no AzureRTOS version actualy specified, must be empty which is fine, we'll default to a known good version
        set(RTOS_VERSION "master")
    else()
        # set SVN tag
        set(RTOS_VERSION "${RTOS_VERSION}")
    endif()

    message(STATUS "RTOS is: AzureRTOS ${RTOS_VERSION} from GitHub repo")

    # need to setup a separate CMake project to download the code from the GitHub repository
    # otherwise it won't be available before the actual build step
    configure_file("${PROJECT_SOURCE_DIR}/CMake/AzureRTOS.CMakeLists.cmake.in"
                "${CMAKE_BINARY_DIR}/AzureRTOS_Download/CMakeLists.txt")

    # setup CMake project for AzureRTOS download
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/AzureRTOS_Download")

    # run build on AzureRTOS download CMake project to perform the download
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/AzureRTOS_Download")

    # add AzureRTOS as external project
    ExternalProject_Add( 
        AzureRTOS
        PREFIX AzureRTOS
        SOURCE_DIR ${CMAKE_BINARY_DIR}/AzureRTOS_Source
        GIT_REPOSITORY https://github.com/azure-rtos/threadx.git
        GIT_TAG ${RTOS_VERSION}  # target specified branch
        GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
        TIMEOUT 10
        LOG_DOWNLOAD 1
        # Disable all other steps
        INSTALL_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
    )

else()
    # AzureRTOS source was specified

    # sanity check is source path exists
    if(EXISTS "${RTOS_SOURCE_FOLDER}/")
        message(STATUS "RTOS is: AzureRTOS (source from: ${RTOS_SOURCE_FOLDER})")

        # check if we already have the sources, no need to copy again
        NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/AzureRTOS_Source/ SOURCE_EXISTS)

        if(NOT ${SOURCE_EXISTS})
            file(COPY "${RTOS_SOURCE_FOLDER}/" DESTINATION "${CMAKE_BINARY_DIR}/AzureRTOS_Source/common/src") #TODO: is this correct?
        else()
            message(STATUS "Using local cache of AzureRTOS source from ${RTOS_SOURCE_FOLDER}")
        endif()

        set(AZURERTOS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/AzureRTOS_Source/common/inc) #TODO is this correct?
    else()
        message(FATAL_ERROR "Couldn't find AzureRTOS source at ${RTOS_SOURCE_FOLDER}/")
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

# # check if CMSIS_SOURCE was specified or if it's empty (default is empty)
# set(NO_CMSIS_SOURCE TRUE)
# if(CMSIS_SOURCE)
#     if(NOT "${CMSIS_SOURCE}" STREQUAL "")
#         set(NO_CMSIS_SOURCE FALSE)
#     endif()
# endif()

# if(NO_CMSIS_SOURCE)

#     # check for Git (needed here for advanced warning to user if it's not installed)
#     find_package(Git)

#     #  check if Git was found, if not report to user and abort
#     if(NOT GIT_EXECUTABLE)
#         message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
#     endif()

#     # CMSIS version
#     set(CMSIS_VERSION_EMPTY TRUE)

#     # check if build was requested with a specifc CMSIS version
#     if(DEFINED CMSIS_VERSION)
#         if(NOT "${CMSIS_VERSION}" STREQUAL "")
#             set(CMSIS_VERSION_EMPTY FALSE)
#         endif()
#     endif()

#     # check if build was requested with a specifc CMSIS version
#     if(CMSIS_VERSION_EMPTY)
#         # no CMSIS version actualy specified, must be empty which is fine, we'll default to a known good version
#         set(CMSIS_GIT_TAG "5.5.1")
#     else()
#         # set Git tag
#         set(CMSIS_GIT_TAG "${CMSIS_VERSION}")
#     endif()            

#     message(STATUS "CMSIS v${CMSIS_VERSION} from GitHub repo")

#     # need to setup a separate CMake project to download the code from the GitHub repository
#     # otherwise it won't be available before the actual build step
#     configure_file("${PROJECT_SOURCE_DIR}/CMake/CMSIS.CMakeLists.cmake.in"
#                 "${CMAKE_BINARY_DIR}/CMSIS_Download/CMakeLists.txt")

#     # setup CMake project for CMSIS download
#     execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
#                     RESULT_VARIABLE result
#                     WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/CMSIS_Download")

                    
#     # run build on CMSIS download CMake project to perform the download
#     execute_process(COMMAND ${CMAKE_COMMAND} --build .
#                     RESULT_VARIABLE result
#                     WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/CMSIS_Download")

#     # add CMSIS as external project
#     ExternalProject_Add( 
#         CMSIS
#         PREFIX CMSIS
#         SOURCE_DIR ${CMAKE_BINARY_DIR}/CMSIS_Source
#         GIT_REPOSITORY  https://github.com/ARM-software/CMSIS_5.git
#         GIT_TAG ${CMSIS_GIT_TAG}  # target specified branch
#         GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
#         TIMEOUT 10
#         LOG_DOWNLOAD 1
#         # Disable all other steps
#         INSTALL_COMMAND ""
#         CONFIGURE_COMMAND ""
#         BUILD_COMMAND ""
#     )  
# else()
#     # CMSIS source was specified

#     # sanity check is source path exists
#     if(EXISTS "${CMSIS_SOURCE}/")
#         message(STATUS "CMSIS ${CMSIS_VERSION} (source from: ${CMSIS_SOURCE})")

#         # check if we already have the sources, no need to copy again
#         NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/CMSIS_Source/ SOURCE_EXISTS)

#         if(NOT ${SOURCE_EXISTS})
#             file(COPY "${CMSIS_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/CMSIS_Source")
#         else()
#             message(STATUS "Using local cache of CMSIS source from ${CMSIS_SOURCE}")
#         endif()

#         set(CMSIS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/CMSIS_Source/include)
#     else()
#         message(FATAL_ERROR "Couldn't find CMSIS source at ${CMSIS_SOURCE}/")
#     endif()

#     # add CMSIS as external project
#     ExternalProject_Add(
#         CMSIS
#         PREFIX CMSIS
#         SOURCE_DIR ${CMAKE_BINARY_DIR}/CMSIS_Source
#         # Disable all other steps
#         INSTALL_COMMAND ""
#         CONFIGURE_COMMAND ""
#         BUILD_COMMAND ""
#     )        

#     # get source dir for CMSIS CMake project
#     ExternalProject_Get_Property(CMSIS SOURCE_DIR)

# endif()

# AzureRTOS common directories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/common)

# try to find board in the targets folder
# and set target base location and base path for class libs
# this has to be set before the class library modules are pulled in
if(EXISTS ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/NXP/${AZURERTOS_BOARD})

    # board found
    message(STATUS "Support for target board '${AZURERTOS_BOARD}' found")

    # Define base path for the class libraries
    set(BASE_PATH_FOR_CLASS_LIBRARIES_MODULES ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/NXP/nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/NXP/${AZURERTOS_BOARD})

    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/NXP)

    # add TARGET board directory
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/NXP/${AZURERTOS_BOARD})

# try to find board in the targets folder
elseif(EXISTS ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/ST/${AZURERTOS_BOARD})

    # board found
    message(STATUS "Support for target board '${AZURERTOS_BOARD}' found")

    # Define base path for the class libraries
    set(BASE_PATH_FOR_CLASS_LIBRARIES_MODULES ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/ST/nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/ST/${AZURERTOS_BOARD})

    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ST)

    # add TARGET board directory
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ST/${AZURERTOS_BOARD})

else()

    # try to find NXP board in the Community targets folder
    if(EXISTS ${PROJECT_SOURCE_DIR}/targets-community/AzureRTOS/NXP/${AZURERTOS_BOARD})

        # board found
        message(STATUS "Support for target board '${AZURERTOS_BOARD}' found in Community targets")

        # Define base path for the class libraries
        set(BASE_PATH_FOR_CLASS_LIBRARIES_MODULES ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/NXP/nanoCLR)

        # set target base location
        set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets-community/AzureRTOS/NXP/${AZURERTOS_BOARD})

        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/NXP)

        # add TARGET board directory from Community
        add_subdirectory(${PROJECT_SOURCE_DIR}/targets-community/AzureRTOS/NXP/${AZURERTOS_BOARD})

    # try to find STM board in the Community targets folder
    elseif(EXISTS ${PROJECT_SOURCE_DIR}/targets-community/AzureRTOS/ST/${AZURERTOS_BOARD})

        # board found
        message(STATUS "Support for target board '${AZURERTOS_BOARD}' found in Community targets")

        # Define base path for the class libraries
        set(BASE_PATH_FOR_CLASS_LIBRARIES_MODULES ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/ST/nanoCLR)

        # set target base location
        set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets-community/AzureRTOS/ST/${AZURERTOS_BOARD})

        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ST)

        # add TARGET board directory from Community
        add_subdirectory(${PROJECT_SOURCE_DIR}/targets-community/AzureRTOS/ST/${AZURERTOS_BOARD})

    else()

        # board NOT found in targets folder
        message(FATAL_ERROR "\n\nSorry but support for ${AZURERTOS_BOARD} target is not available...\n\nYou can wait for it to be added or you might want to contribute and start working on a PR for it.\n\n")

    endif()

endif()
