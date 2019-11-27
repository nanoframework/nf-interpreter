#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

include(nf_utils)

# if mbed TLS is enabled add it to the build
if(USE_SECURITY_MBEDTLS_OPTION)

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

        # sanity check is source path exists
        if(EXISTS "${MBEDTLS_SOURCE}/")

            # check if we already have the sources, no need to copy again
            NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/mbedTLS_Source SOURCE_EXISTS)

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

    # get source dir for mbedTLS CMake project
    ExternalProject_Get_Property(mbedTLS SOURCE_DIR)

    set(mbedTLS_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/3rdparty/humblelogging/include")
    set(mbedTLS_LIBRARIES "${CMAKE_SHARED_LIBRARY_PREFIX}mbedTLS${CMAKE_SHARED_LIBRARY_SUFFIX}")
    include_directories(${mbedTLS_INCLUDE_DIRS})

endif()
