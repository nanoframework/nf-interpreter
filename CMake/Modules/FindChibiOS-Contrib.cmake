#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(chibios-contrib)

# set include directories for ChibiOS-Contrib

# the path is one folder UP from the includes directory to allow a selective inclusion of our own hal_community.h
list(APPEND CHIBIOS_CONTRIB_INCLUDE_DIRS ${chibios-contrib_SOURCE_DIR}/os/hal/include)

####################################################################################
# WHEN ADDING A NEW ChibiOS-Contrib component add the include directory(ies) below
####################################################################################
# component IMACOMPONENT
#list(APPEND CHIBIOS_CONTRIB_INCLUDE_DIRS ${chibios-contrib_SOURCE_DIR}/os/hal/include/IMACOMPONENT)

# USB Host v1
list(APPEND CHIBIOS_CONTRIB_INCLUDE_DIRS ${chibios-contrib_SOURCE_DIR}/os/hal/ports/STM32/LLD/USBHv1)


####################################################################################################################################
# Because we've "hacked" ChibiOS overlay mechanism used by the community contributions we can't add the "official" hal_community.c #
# Instead we have to add the required and equivalent calls that exist in the official source file                                  #
# in our hal_community.c @ targets/ChibiOS/_nf-overlay/os/hal/src/hal_community.c                                          #
# the include paths are to be added to our hal_community.h @ targets/ChibiOS/_nf-overlay/os/hal/include/hal_community.h    #
####################################################################################################################################


####################################################################################################
# WHEN ADDING A NEW ChibiOS-Contrib component add the source file(s) specific to it below 
####################################################################################################
# component IMACOMPONENT
# list(APPEND CHIBIOS_CONTRIB_SOURCES ${chibios-contrib_SOURCE_DIR}/os/hal/src/IMACOMPONENT/source-file-1.c)
# list(APPEND CHIBIOS_CONTRIB_SOURCES ${chibios-contrib_SOURCE_DIR}/os/hal/src/IMACOMPONENT/source-file-2.c)
# list(APPEND CHIBIOS_CONTRIB_SOURCES ${chibios-contrib_SOURCE_DIR}/os/hal/src/IMACOMPONENT/source-file-3.c)

# USB Host v1
list(APPEND CHIBIOS_CONTRIB_SOURCES ${chibios-contrib_SOURCE_DIR}/os/hal/src/hal_usbh.c)
list(APPEND CHIBIOS_CONTRIB_SOURCES ${chibios-contrib_SOURCE_DIR}/os/hal/src/usbh/hal_usbh_msd.c)
list(APPEND CHIBIOS_CONTRIB_SOURCES ${chibios-contrib_SOURCE_DIR}/os/hal/src/usbh/hal_usbh_desciter.c)
list(APPEND CHIBIOS_CONTRIB_SOURCES ${chibios-contrib_SOURCE_DIR}/os/hal/src/usbh/hal_usbh_hub.c)
list(APPEND CHIBIOS_CONTRIB_SOURCES ${chibios-contrib_SOURCE_DIR}/os/hal/src/hal_usb_msd.c)
list(APPEND CHIBIOS_CONTRIB_SOURCES ${chibios-contrib_SOURCE_DIR}/os/hal/ports/STM32/LLD/USBHv1/hal_usbh_lld.c)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ChibiOS-Contrib DEFAULT_MSG CHIBIOS_CONTRIB_INCLUDE_DIRS CHIBIOS_CONTRIB_SOURCES)
