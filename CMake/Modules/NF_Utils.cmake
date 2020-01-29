#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# check if a directory exists
# going throuhg the directory to find if it's not empty takes a lot of time because it sweeps all files
# simplifying this now to speed up local builds
macro(NF_DIRECTORY_EXISTS_NOT_EMPTY path pathExists)

    if(EXISTS "${path}")
        set(${pathExists} ON)
    else()
        set(${pathExists} OFF)
    endif()

endmacro()
