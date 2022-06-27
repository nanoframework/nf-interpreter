#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#


# set include directories
list(APPEND nanoFramework.System.Data.Sqlite_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/src/nanoFramework.System.Data.Sqlite")

# source files
set(nanoFramework.System.Data.Sqlite_SRCS

    # source files of the assembly
    nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand.cpp
    shell.c
    sqlite3.c
)

foreach(SRC_FILE ${nanoFramework.System.Data.Sqlite_SRCS})

    set(nanoFramework.System.Data.Sqlite_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(nanoFramework.System.Data.Sqlite_SRC_FILE ${SRC_FILE}
        PATHS

            # path for source files of this module
            ${CMAKE_SOURCE_DIR}/src/nanoFramework.System.Data.Sqlite

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${nanoFramework.System.Data.SqLite_SRC_FILE}")
    endif()

    list(APPEND nanoFramework.System.Data.Sqlite_SOURCES ${nanoFramework.System.Data.Sqlite_SRC_FILE})
    
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.System.Data.Sqlite DEFAULT_MSG nanoFramework.System.Data.Sqlite_INCLUDE_DIRS nanoFramework.System.Data.Sqlite_SOURCES)
