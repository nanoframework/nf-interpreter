#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.TI.EasyLink")

# set include directories
list(APPEND nanoFramework.TI.EasyLink_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND nanoFramework.TI.EasyLink_INCLUDE_DIRS ${TARGET_BASE_LOCATION})
list(APPEND nanoFramework.TI.EasyLink_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/TI-SimpleLink/common/easylink)
list(APPEND nanoFramework.TI.EasyLink_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/TI-SimpleLink/common)

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
            ${PROJECT_SOURCE_DIR}/src/nanoFramework.TI.EasyLink
            ${PROJECT_SOURCE_DIR}/targets/TI-SimpleLink/common/easylink
            ${PROJECT_SOURCE_DIR}/targets/TI-SimpleLink/nanoCLR

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${nanoFramework.TI.EasyLink_SRC_FILE}") # debug helper
    list(APPEND nanoFramework.TI.EasyLink_SOURCES ${nanoFramework.TI.EasyLink_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.TI.EasyLink DEFAULT_MSG nanoFramework.TI.EasyLink_INCLUDE_DIRS nanoFramework.TI.EasyLink_SOURCES)
