#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# check if cube package source was specified or if it's empty (default is empty)
macro(ProcessLoRaMacNodePackage)

    set(NO_LORAMAC_NODE_PACKAGE_SOURCE TRUE)
    if(NO_LORAMAC_NODE_PACKAGE_SOURCE)
        if(NOT "${LORAMAC_NODE_PACKAGE_SOURCE}" STREQUAL "")
            set(NO_LORAMAC_NODE_PACKAGE_SOURCE FALSE)
        endif()
    endif()

    if(NO_LORAMAC_NODE_PACKAGE_SOURCE)
        # no LoRaMac Node Package source specified, download it

        # check for Git (needed here for advanced warning to user if it's not installed)
        find_package(Git)

        #  check if Git was found, if not report to user and abort
        if(NOT GIT_EXECUTABLE)
            message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
        endif()

        # LoRaMac Node Package version
        set(LORAMAC_NODE_VERSION_EMPTY TRUE)

        # check if build was requested with a specifc LoRaMac Node Package version
        if(DEFINED LORAMAC_NODE_VERSION)
            if(NOT "${LORAMAC_NODE_VERSION}" STREQUAL "")
                set(LORAMAC_NODE_VERSION_EMPTY FALSE)
            endif()
        endif()

        # check if build was requested with a specifc LoRaMac Node Package version
        if(LORAMAC_NODE_VERSION_EMPTY)
            # no LoRaMac Node Package version actualy specified, must be empty which is fine, we'll default to a known good version
            set(LORA_MAC_NODE_GIT_TAG "develop")
        else()
            # set tag
            set(LORA_MAC_NODE_GIT_TAG "${LORAMAC_NODE_VERSION}")
        endif()

        message(STATUS "LoRaMac Node Package package from GitHub repo")

        # need to setup a separate CMake project to download the code from the GitHub repository
        # otherwise it won't be available before the actual build step
        configure_file("${PROJECT_SOURCE_DIR}/CMake/LoRaMacNode.CMakeLists.cmake.in"
                    "${CMAKE_BINARY_DIR}/LoRaMacNode_Download/CMakeLists.txt")

        # setup CMake project for LoRaMac Node Package download
        execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                        RESULT_VARIABLE result
                        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/LoRaMacNode_Download")

        # run build on LoRaMac Node Package download CMake project to perform the download
        execute_process(COMMAND ${CMAKE_COMMAND} --build .
                        RESULT_VARIABLE result
                        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/LoRaMacNode_Download")

        # add LoRaMac Node Package as external project
        # need to specify nanoframework as the active branch
        ExternalProject_Add( 
            LoRaMacNode
            PREFIX LoRaMacNode
            SOURCE_DIR ${CMAKE_BINARY_DIR}/LoRaMacNode_Source
            GIT_REPOSITORY  https://github.com/Lora-net/LoRaMac-node
            GIT_TAG ${LORA_MAC_NODE_GIT_TAG}  # target specified branch
            GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
            TIMEOUT 10
            LOG_DOWNLOAD 1
            # Disable all other steps
            INSTALL_COMMAND ""
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
        )      

        # get source dir for LoRaMac Node Package CMake project
        ExternalProject_Get_Property(LoRaMacNode SOURCE_DIR)

    else()
        # LoRaMac Node Package  source was specified

        # sanity check is source path exists
        if(EXISTS "${LORAMAC_NODE_PACKAGE_SOURCE}/")
            message(STATUS "LoRaMac Node Package  source from: ${LORAMAC_NODE_PACKAGE_SOURCE}")

            # check if we already have the sources, no need to copy again
            if(NOT EXISTS "${CMAKE_BINARY_DIR}/LoRaMacNode_Source")
                file(COPY "${LORAMAC_NODE_PACKAGE_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/LoRaMacNodeSource")
            else()
                message(STATUS "Using local cache of LoRaMac Node Package  source from ${LORAMAC_NODE_PACKAGE_SOURCE}")
            endif()
        else()
            message(FATAL_ERROR "Couldn't find LoRaMac Node Package  source at ${LORAMAC_NODE_PACKAGE_SOURCE}/")
        endif()

        # add LoRaMac Node Package as external project
        ExternalProject_Add(
            LoRaMacNode
            PREFIX LoRaMacNode
            SOURCE_DIR ${CMAKE_BINARY_DIR}/LoRaMacNode_Source
            # Disable all other steps
            INSTALL_COMMAND ""
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
        )

        # get source dir for LoRaMac Node Package CMake project
        ExternalProject_Get_Property(LoRaMacNode SOURCE_DIR)

    endif()

endmacro()
