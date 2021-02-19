#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(nf_utils)


# check if nRF Connect SDK folder was specified or if it's empty (default is empty)
set(NO_NRF_CONNECT_SDK_SOURCE_FOLDER TRUE)
if(NRF_CONNECT_SDK_SOURCE_FOLDER)
    if(NOT "${NRF_CONNECT_SDK_SOURCE_FOLDER}" STREQUAL "")
        set(NO_NRF_CONNECT_SDK_SOURCE_FOLDER FALSE)
    endif()
endif()

if(NO_NRF_CONNECT_SDK_SOURCE_FOLDER)
    # no nRF Connect SDK source specified, download it from it's repo
    
    # check for Git (needed here for advanced warning to user if it's not installed)
    find_package(Git)

    #  check if Git was found, if not report to user and abort
    if(NOT GIT_EXECUTABLE)
        message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
    endif()

    # N version
    set(NRF_CONNECT_SDK_VERSION_EMPTY TRUE)

    # check if build was requested with a specifc nRF Connect SDK version
    if(DEFINED NRF_CONNECT_SDK_VERSION)
        if(NOT "${NRF_CONNECT_SDK_VERSION}" STREQUAL "")
            set(NRF_CONNECT_SDK_VERSION_EMPTY FALSE)
        endif()
    endif()

    # check if build was requested with a specifc nRF Connect SDK version
    if(NRF_CONNECT_SDK_VERSION_EMPTY)
        # no nRF Connect SDK version actualy specified, must be empty which is fine, we'll default to a known good version
        set(NRF_CONNECT_SDK_VERSION "v1.4.2")
    endif()

    message(STATUS "nRF Connect SDK ${NRF_CONNECT_SDK_VERSION} from GitHub repo")

    # need to setup a separate CMake project to download the code from the GitHub repository
    # otherwise it won't be available before the actual build step
    configure_file(${PROJECT_SOURCE_DIR}/CMake/nRFConnectSdk.CMakeLists.cmake.in
                    ${CMAKE_BINARY_DIR}/nRFConnectSdk_Download/CMakeLists.txt)

    # setup CMake project for nRF Connect SDK download
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/nRFConnectSdk_Download)

    # run build on nRF Connect SDK download CMake project to perform the download
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/nRFConnectSdk_Download)

else()
    # nRF Connect SDK source was specified

    # sanity check is source path exists
    if(EXISTS "${NRF_CONNECT_SDK_SOURCE_FOLDER}/")
        message(STATUS "nRF Connect SDK (source from: ${NRF_CONNECT_SDK_SOURCE_FOLDER})")

        # check if we already have the sources, no need to copy again
        NF_DIRECTORY_EXISTS_NOT_EMPTY(${CMAKE_BINARY_DIR}/nRFConnectSdk_Source/ SOURCE_EXISTS)

        if(NOT ${SOURCE_EXISTS})
            file(COPY "${NRF_CONNECT_SDK_SOURCE_FOLDER}/" DESTINATION "${CMAKE_BINARY_DIR}/nRFConnectSdk_Source/")
        else()
            message(STATUS "Using local cache of nRF Connect SDK source from ${NRF_CONNECT_SDK_SOURCE_FOLDER}")
        endif()

        set(AZURERTOS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/nRFConnectSdk_Source/common/inc) #TODO is this correct?
    else()
        message(FATAL_ERROR "Couldn't find nRF Connect SDK source at ${NRF_CONNECT_SDK_SOURCE_FOLDER}/")
    endif()

endif()

# make vars global
set(NANOCLR_PROJECT_SOURCES ${NANOCLR_PROJECT_SOURCES} CACHE INTERNAL "make global")
set(NANOCLR_PROJECT_INCLUDE_DIRS ${NANOCLR_PROJECT_INCLUDE_DIRS} CACHE INTERNAL "make global")

list(APPEND TARGET_AZURERTOS_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/_include)

# add platform dirs
add_subdirectory(_include)
add_subdirectory(_common)
add_subdirectory(_nanoBooter)
add_subdirectory(_nanoCLR)
