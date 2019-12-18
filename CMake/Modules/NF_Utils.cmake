#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# check if a diretory exists and is not empty
macro(NF_DIRECTORY_EXISTS_NOT_EMPTY path pathExists)

    if(EXISTS "${path}")

        file(GLOB RESULT LIST_DIRECTORIES false "${path}")
        list(LENGTH RESULT RES_LEN)
        if(RES_LEN EQUAL 0)
            # DIR is empty
            set(${pathExists} OFF)
        else()
            # DIR exists and it's NOT empty
            set(${pathExists} ON)
        endif()

    else()
        set(${pathExists} OFF)
    endif()

endmacro()
