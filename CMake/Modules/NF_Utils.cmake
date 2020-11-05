#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# check if a directory exists
# going throuhg the directory to find if it's not empty takes a lot of time because it sweeps all files
# simplifying this now to speed up local builds
macro(NF_DIRECTORY_EXISTS_NOT_EMPTY path pathExists)

    if(EXISTS "${path}")
        set(${pathExists} TRUE)
    else()
        set(${pathExists} FALSE)
    endif()

endmacro()

# define the base path for the library modules
macro(SET_BASE_PATH_FOR_LIBRARIES_MODULES path)
    set(BASE_PATH_FOR_CLASS_LIBRARIES_MODULES ${path})
endmacro()