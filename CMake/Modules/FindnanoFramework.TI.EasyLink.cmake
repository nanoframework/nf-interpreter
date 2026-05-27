#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.TI.EasyLink")

# set include directories
list(APPEND nanoFramework.TI.EasyLink_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND nanoFramework.TI.EasyLink_INCLUDE_DIRS ${TARGET_BASE_LOCATION})
list(APPEND nanoFramework.TI.EasyLink_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/TI_SimpleLink/_common/easylink)
list(APPEND nanoFramework.TI.EasyLink_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/TI_SimpleLink/_common)

# source files
set(nanoFramework.TI.EasyLink_SRCS
    
    # source files for EasyLink
    EasyLink.c

    # targetEasyLinkCC13xx_CC26xx_RadioTask.c

    # source files of the assembly
    nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController.cpp
    nf_ti_easylink.cpp
)

foreach(SRC_FILE ${nanoFramework.TI.EasyLink_SRCS})

    set(nanoFramework.TI.EasyLink_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(nanoFramework.TI.EasyLink_SRC_FILE ${SRC_FILE}
        PATHS

            ${BASE_PATH_FOR_THIS_MODULE}
            ${TARGET_BASE_LOCATION}
            ${CMAKE_SOURCE_DIR}/src/nanoFramework.TI.EasyLink
            ${CMAKE_SOURCE_DIR}/targets/TI_SimpleLink/_common/easylink
            ${CMAKE_SOURCE_DIR}/targets/TI_SimpleLink/_nanoCLR

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${nanoFramework.TI.EasyLink_SRC_FILE}")
    endif()

    list(APPEND nanoFramework.TI.EasyLink_SOURCES ${nanoFramework.TI.EasyLink_SRC_FILE})
    
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.TI.EasyLink DEFAULT_MSG nanoFramework.TI.EasyLink_INCLUDE_DIRS nanoFramework.TI.EasyLink_SOURCES)
