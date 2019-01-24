#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# set include directories for ChibiOS-Contrib

list(APPEND CHIBIOS_CONTRIB_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS-Contrib_Source/os/hal/include/)

####################################################################################
# WHEN ADDING A NEW ChibiOS-Contrib component add the include directory(ies) bellow
####################################################################################
# component IMACOMPONENT
#list(APPEND CHIBIOS_CONTRIB_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS-Contrib_Source/os/hal/include/IMACOMPONENT)


#######################################################################################################################################
# Because we've "hacked" ChibiOS overlay mechanism used by the community contributions we can't add the "official" hal_community.c
# Instead we have to add the requried and equivalent calls existing in the official source file
# in our hal_community.c @ targets\CMSIS-OS\ChibiOS\nf-overlay\os\hal\src\hal_community.c
#########################################################################################################################


####################################################################################################
# WHEN ADDING A NEW ChibiOS-Contrib component add the source file(s) specific to it bellow 
####################################################################################################
# component IMACOMPONENT
# list(APPEND CHIBIOS_CONTRIB_SOURCES ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/IMACOMPONENT/source-file-1.c)
# list(APPEND CHIBIOS_CONTRIB_SOURCES ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/IMACOMPONENT/source-file-2.c)
# list(APPEND CHIBIOS_CONTRIB_SOURCES ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/IMACOMPONENT/source-file-3.c)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CHIBIOS_CONTRIB DEFAULT_MSG CHIBIOS_CONTRIB_INCLUDE_DIRS CHIBIOS_CONTRIB_SOURCES)
