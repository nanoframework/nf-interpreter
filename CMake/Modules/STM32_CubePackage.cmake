#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# include the STM32 Cube package for the appropriate series, if requested

# check if cube package source was specified or if it's empty (default is empty)
macro(ProcessSTM32CubePackage)

    # process target series, which is in the format "STM32F4xx"
    string(REPLACE "STM32" "" TARGET_SERIES_SHORT_1 "${TARGET_SERIES}")
    string(REPLACE "xx" "" TARGET_SERIES_SHORT "${TARGET_SERIES_SHORT_1}")

    set(NO_STM32_CUBE_PACKAGE_SOURCE TRUE)
    if(NO_STM32_CUBE_PACKAGE_SOURCE)
        if(NOT "${STM32_CUBE_PACKAGE_SOURCE}" STREQUAL "")
            set(NO_STM32_CUBE_PACKAGE_SOURCE FALSE)
        endif()
    endif()

    if(NO_STM32_CUBE_PACKAGE_SOURCE)
        # no STM Cube package source specified, download it from nanoFramework fork

        # check for Git (needed here for advanced warning to user if it's not installed)
        find_package(Git)

        #  check if Git was found, if not report to user and abort
        if(NOT GIT_EXECUTABLE)
            message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
        endif()

        message(STATUS "STM32 ${TARGET_SERIES_SHORT} Cube package from GitHub repo")

        # need to setup a separate CMake project to download the code from the GitHub repository
        # otherwise it won't be available before the actual build step
        configure_file("${PROJECT_SOURCE_DIR}/CMake/STM32.CubePackage.CMakeLists.cmake.in"
                    "${CMAKE_BINARY_DIR}/STM32${TARGET_SERIES_SHORT}-CubePackage_Download/CMakeLists.txt")

        # setup CMake project for STM32_CubePackage download
        execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                        RESULT_VARIABLE result
                        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/STM32${TARGET_SERIES_SHORT}-CubePackage_Download")

        # run build on STM32_CubePackage download CMake project to perform the download
        execute_process(COMMAND ${CMAKE_COMMAND} --build .
                        RESULT_VARIABLE result
                        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/STM32${TARGET_SERIES_SHORT}-CubePackage_Download")

        # add STM32_CubePackage as external project
        # need to specify nanoframework as the active branch
        ExternalProject_Add( 
            STM32${TARGET_SERIES_SHORT}_CubePackage
            PREFIX STM32${TARGET_SERIES_SHORT}_CubePackage
            SOURCE_DIR ${CMAKE_BINARY_DIR}/STM32${TARGET_SERIES_SHORT}_CubePackage_Source
            GIT_REPOSITORY  https://github.com/nanoframework/STM32Cube${TARGET_SERIES_SHORT}
            GIT_TAG "nf-build"  # target specific branch        
            GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
            TIMEOUT 10
            LOG_DOWNLOAD 1
            # Disable all other steps
            INSTALL_COMMAND ""
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
        )      

        # get source dir for STM32_CubePackage CMake project
        ExternalProject_Get_Property(STM32${TARGET_SERIES_SHORT}_CubePackage SOURCE_DIR)

    else()
        # STM32 Cube package source was specified

        # sanity check is source path exists
        if(EXISTS "${STM32_CUBE_PACKAGE_SOURCE}/")
            message(STATUS "STM32 Cube package source from: ${STM32_CUBE_PACKAGE_SOURCE}")

            # check if we already have the sources, no need to copy again
            if(NOT EXISTS "${CMAKE_BINARY_DIR}/STM32${TARGET_SERIES_SHORT}_CubePackage_Source")
                file(COPY "${STM32_CUBE_PACKAGE_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/STM32${TARGET_SERIES_SHORT}_CubePackage_Source")
            else()
                message(STATUS "Using local cache of STM32 Cube package source from ${STM32_CUBE_PACKAGE_SOURCE}")
            endif()
        else()
            message(FATAL_ERROR "Couldn't find STM32 Cube package source at ${STM32_CUBE_PACKAGE_SOURCE}/")
        endif()

        # add STM32_CubePackage as external project
        ExternalProject_Add(
            STM32${TARGET_SERIES_SHORT}_CubePackage
            PREFIX STM32${TARGET_SERIES_SHORT}_CubePackage
            SOURCE_DIR ${CMAKE_BINARY_DIR}/STM32${TARGET_SERIES_SHORT}_CubePackage_Source
            # Disable all other steps
            INSTALL_COMMAND ""
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
        )

        # get source dir for STM32_CubePackage CMake project
        ExternalProject_Get_Property(STM32${TARGET_SERIES_SHORT}_CubePackage SOURCE_DIR)

    endif()

endmacro()
