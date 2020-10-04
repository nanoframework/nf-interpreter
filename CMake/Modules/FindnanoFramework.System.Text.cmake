#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#


# set include directories
list(APPEND nanoFramework.System.Text_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/HAL/Include")
list(APPEND nanoFramework.System.Text_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/nanoFramework.System.Text")

# source files
set(nanoFramework.System.Text_SRCS

    # source files of the assembly
    nf_system_text_System_Text_UTF8Decoder.cpp
    nf_system_text_System_Text_UTF8Encoding.cpp
    nf_system_text.cpp
)

foreach(SRC_FILE ${nanoFramework.System.Text_SRCS})
    set(nanoFramework.System.Text_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(nanoFramework.System.Text_SRC_FILE ${SRC_FILE}
        PATHS

            # path for source files of this module
            ${PROJECT_SOURCE_DIR}/src/nanoFramework.System.Text

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${nanoFramework.System.Text_SRC_FILE}") # debug helper
    list(APPEND nanoFramework.System.Text_SOURCES ${nanoFramework.System.Text_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.System.Text DEFAULT_MSG nanoFramework.System.Text_INCLUDE_DIRS nanoFramework.System.Text_SOURCES)
