# validate is there is a series defined
if(NOT STM32_SERIES)
    message(FATAL_ERROR "No STM32_SERIES specified")
endif()

# message(STATUS "--${CMAKE_CURRENT_SOURCE_DIR}/Custom_ROMID.zip--")
# execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_CURRENT_SOURCE_DIR}/Custom_ROMID.zip" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


# check if series folder already exists 
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${STM32_SERIES}")

    # ST Cube package folder is missing

    # validate package version
    if(NOT PACKAGE_VERSION)
        message(FATAL_ERROR "\nNo PACKAGE_VERSION specifed.\nPlease use -DPACKAGE_VERSION=N.N.N with an existing valid ST Cube package version.")
    endif()

    # package version has to have format N.N.N or N.NN.N
    string(REGEX MATCH "[0-9].[0-9][0-9]?.[0-9]" PACKAGE_VERSION_CHECK "${PACKAGE_VERSION}")
    string(COMPARE EQUAL "${PACKAGE_VERSION}" "${PACKAGE_VERSION_CHECK}" PACKAGE_VERSION_VALID)

    if(NOT PACKAGE_VERSION_VALID)
        message(FATAL_ERROR "Invalid package version (PACKAGE_VERSION_CHECK). Must have format N.N.N or N.NN.N")
    else()
        message(STATUS "STM32 Cube package is: ${PACKAGE_VERSION}")
    endif()
    #####


    # check if this package version is for a patch (version number is different than 0, like in 1.4.2)
    string(FIND "${PACKAGE_VERSION}" "." LAST_DOT_POSTION REVERSE)
    string(LENGTH "${PACKAGE_VERSION}" PACKAGE_VERSION_LENGHT)
    math(EXPR PACKAGE_VERSION_REVISION_POSITION "${LAST_DOT_POSTION} + 1")
    string(SUBSTRING "${PACKAGE_VERSION}" ${PACKAGE_VERSION_REVISION_POSITION} 1 PACKAGE_VERSION_REVISION)

    if(${PACKAGE_VERSION_REVISION} GREATER 0)

        set(PACK_IS_PATCH TRUE CACHE INTERNAL "STM Cube package is a patch")
        # packages with patches are stored the "base" version 
        # example: package version is 1.13.1
        # this will be version 1.13.0, stored in a folder with this name 
        # initial package is unziped to that folder
        # patch package is unziped overwriting original files 

        # rename ST Cube package name to remove revision 
        string(SUBSTRING "${PACKAGE_VERSION}" 0 ${PACKAGE_VERSION_REVISION_POSITION} PACKAGE_VERSION)
        set(PACKAGE_VERSION "${PACKAGE_VERSION}0")

    else()

        set(PACK_IS_PATCH FALSE CACHE INTERNAL "STM Cube package is NOT a patch")

    endif()
    #####


    # package version without dots
    string(REPLACE "." "" PACKAGE_VERSION_NUMERIC ${PACKAGE_VERSION})

    # compose the file name for the ZIPed STM32 Cube package
    set(STM32_CUBE_PACK_NAME "stm32cube_fw_${STM32_SERIES_LOWER}_v${PACKAGE_VERSION_NUMERIC}.zip")

    # compose the folder name for the STM32 Cube package
    set(STM32_CUBE_PACK_FOLDER_NAME "STM32Cube_FW_${STM32_SERIES}_V${PACKAGE_VERSION}")

    # download base URL
    set(STM32_CUBE_PACK_BASEURL "http://www.st.com/resource/en/firmware2")

    # clean destination directory
    # TODO add config parameter to force clean/reinstall package
    #file(REMOVE_RECURSE "${PROJECT_BINARY_DIR}/${STM32_SERIES}")

    # check if ST Cube package file already exists, if not download it
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${STM32_CUBE_PACK_NAME}")

        # download STM Cube package to stcube_repository folder 
        # don't download it to build folder so it doesn't get deleted on cleanup
        message(STATUS "Downloading STM32 Cube package from ${STM32_CUBE_PACK_BASEURL}/${STM32_CUBE_PACK_NAME}")
        message(STATUS "This can take a while...")
        file(DOWNLOAD "${STM32_CUBE_PACK_BASEURL}/${STM32_CUBE_PACK_NAME}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/${STM32_CUBE_PACK_NAME}" SHOW_PROGRESS)
    endif()

    # unzip the package file
    message(STATUS "Unpacking STM32 Cube package. This can take a while...")
    # ZIP output is at the binary directory
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_CURRENT_SOURCE_DIR}/${STM32_CUBE_PACK_NAME}" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

    # check if package is a patch
    if(PACK_IS_PATCH)

        # compose file name for patch 
        # rename ST Cube package name to remove revision 
        string(SUBSTRING "${PACKAGE_VERSION}" 0 ${PACKAGE_VERSION_REVISION_POSITION} PACKAGE_VERSION)
        set(PACKAGE_VERSION "${PACKAGE_VERSION}${PACKAGE_VERSION_REVISION}")

        # package version without dots
        string(REPLACE "." "" PACKAGE_VERSION_NUMERIC ${PACKAGE_VERSION})
        # compose the file name for the ZIPed STM32 Cube package
        set(STM32_CUBE_PACK_NAME "stm32cube_fw_${STM32_SERIES_LOWER}_v${PACKAGE_VERSION_NUMERIC}.zip")

        # check if file already exists, if not download it
        if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${STM32_CUBE_PACK_NAME}")

            # download STM Cube package to stcube_repository folder 
            # don't download it to build folder so it doesn't get deleted on cleanup
            message(STATUS "Downloading STM32 Cube package from ${STM32_CUBE_PACK_BASEURL}/${STM32_CUBE_PACK_NAME}")
            message(STATUS "This can take a while...")
            file(DOWNLOAD "${STM32_CUBE_PACK_BASEURL}/${STM32_CUBE_PACK_NAME}"
                        "${CMAKE_CURRENT_SOURCE_DIR}/${STM32_CUBE_PACK_NAME}" SHOW_PROGRESS)
        endif()

        # unzip the package file
        message(STATUS "Unpacking STM32 Cube package patch. This can take a while...")
        # ZIP output is at the binary directory
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_CURRENT_SOURCE_DIR}/${STM32_CUBE_PACK_NAME}" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

    endif()

    # remove folders not required for build
    FILE(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/${STM32_CUBE_PACK_FOLDER_NAME}/_htmresc")
    FILE(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/${STM32_CUBE_PACK_FOLDER_NAME}/Documentation")
    FILE(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/${STM32_CUBE_PACK_FOLDER_NAME}/Utilities/PC_Software")
    FILE(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/${STM32_CUBE_PACK_FOLDER_NAME}/Utilities/Media")
    FILE(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/${STM32_CUBE_PACK_FOLDER_NAME}/Projects")

    # rename folder as CMake extract doesn't accept a destination directory
    file(RENAME "${CMAKE_CURRENT_SOURCE_DIR}/${STM32_CUBE_PACK_FOLDER_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/${STM32_SERIES}")

endif()

# series folder exists at repo

if(NOT EXISTS "${PROJECT_BINARY_DIR}/stcube/${STM32_SERIES}")
    # series folder doesn't exist at build folder so copy it there
    message(STATUS "STM32 package already available. Skiping download and copying to build directory.")

    # copy series directory to build directory
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/${STM32_SERIES}" DESTINATION "${PROJECT_BINARY_DIR}/stcube")
else()
    # series folder already at build folder so we are done here
    message(STATUS "STM32 package already at build directory. Skiping download and copy.")
endif()


# set ST Cube directory for this family
set(STM32Cube_DIR "${PROJECT_BINARY_DIR}/stcube/${STM32_SERIES}" CACHE INTERNAL "path to ST Cube package files")
