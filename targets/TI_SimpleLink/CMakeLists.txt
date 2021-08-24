#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
include(binutils.common)
include(binutils.arm-none-eabi)
include(binutils.TI_SimpleLink)

# Define PLATFORM base path
set(BASE_PATH_FOR_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

# add target TI SimpleLink dirs
add_subdirectory(_common)
add_subdirectory(_include)
add_subdirectory(_nanoCLR)

########################################
if("${TARGET_SERIES}" STREQUAL "CC32xx")
    
    # CC32xx series is using TI SimpleLink CC32xx SDK and TI RTOS
    
    # check if TI_SL_CC32xx_SDK_SOURCE was specified or if it's empty (default is empty)
    set(NO_TI_SL_CC32xx_SDK_SOURCE TRUE)
    if(TI_SL_CC32xx_SDK_SOURCE)
        if(NOT "${TI_SL_CC32xx_SDK_SOURCE}" STREQUAL "")
            set(NO_TI_SL_CC32xx_SDK_SOURCE FALSE)
        endif()
    endif()

    # WHEN CHANGING THIS MAKE SURE TO UPDATE THE DEV CONTAINERS
    set(TI_SL_CC32xx_SDK_TAG "4.10.00.07")

    if(NO_TI_SL_CC32xx_SDK_SOURCE)
        # no TI SimpleLink CC32xx SDK source specified, download it from our repo

        message(STATUS "TI SimpleLink CC32xx SDK ${TI_SL_CC32xx_SDK_TAG}")

        FetchContent_Declare(
            simplelinkcc32xxsdk
            GIT_REPOSITORY https://github.com/nanoframework/SimpleLink_CC32xx_SDK
            GIT_TAG ${TI_SL_CC32xx_SDK_TAG}
        )

    else()
        # TI SimpleLink CC32xx SDK source was specified

        # sanity check is source path exists
        if(EXISTS "${TI_SL_CC32xx_SDK_SOURCE}/")
            message(STATUS "TI SimpleLink CC32xx SDK ${TI_SL_CC32xx_SDK_TAG} source from: ${TI_SL_CC32xx_SDK_SOURCE}")

            FetchContent_Declare(
                simplelinkcc32xxsdk
                GIT_REPOSITORY ${TI_SL_CC32xx_SDK_SOURCE}
                GIT_TAG ${TI_SL_CC32xx_SDK_TAG}
            )

        else()
            message(FATAL_ERROR "Couldn't find TI SimpleLink CC32xx SDK source at ${TI_SL_CC32xx_SDK_SOURCE}/")
        endif()

    endif()
    
    FetchContent_GetProperties(simplelinkcc32xxsdk)
    FetchContent_Populate(simplelinkcc32xxsdk)

    # check if TI_XDCTOOLS_SOURCE was specified or if it's empty (default is empty)
    set(NO_TI_XDCTOOLS_SOURCE TRUE)
    if(TI_XDCTOOLS_SOURCE)
        if(NOT "${TI_XDCTOOLS_SOURCE}" STREQUAL "")
            set(NO_TI_XDCTOOLS_SOURCE FALSE)
        endif()
    endif()

    # TI XDCTools version
    set(TI_XDCTOOLS_VERSION_EMPTY TRUE)

    # check if build was requested with a specifc TI XDCTools version
    if(TI_XDCTOOLS_VERSION_EMPTY)
        # no TI XDCTools version actualy specified, must be empty which is fine, we'll default to a known good version
        # WHEN CHANGING THIS MAKE SURE TO UPDATE THE DEV CONTAINERS
        set(TI_XDCTOOLS_VERSION_TAG "3.62.00.08")
    else()
        # set SVN tag
        set(TI_XDCTOOLS_VERSION_TAG "${TI_XDCTOOLS_VERSION}")
    endif()

    if(NO_TI_XDCTOOLS_SOURCE)
        # no TI XDCTools source specified, download it from it's repo

        message(STATUS "TI XDCTools ${TI_XDCTOOLS_VERSION} from GitHub repo")

        FetchContent_Declare(
            ti_xdctools
            GIT_REPOSITORY https://github.com/nanoframework/TI_XDCTools
            GIT_TAG ${TI_XDCTOOLS_VERSION_TAG}
        )

    else()
        # TI XDCTools source was specified

        # sanity check is source path exists
        if(EXISTS "${TI_XDCTOOLS_SOURCE}/")
            message(STATUS "TI XDCTools ${TI_XDCTOOLS_VERSION} (source from: ${TI_XDCTOOLS_SOURCE})")

            FetchContent_Declare(
                ti_xdctools
                GIT_REPOSITORY ${TI_XDCTOOLS_SOURCE}
                GIT_TAG ${TI_XDCTOOLS_VERSION_TAG}
            )

        else()
            message(FATAL_ERROR "Couldn't find TI XDCTools source at ${TI_XDCTOOLS_SOURCE}/")
        endif()

    endif()
    
    FetchContent_GetProperties(ti_xdctools)
    FetchContent_Populate(ti_xdctools)

    set(TI_XDCTools_INCLUDE_DIR ${ti_xdctools_SOURCE_DIR}/packages/ PARENT_SCOPE)

    # need to override the toolchain lib directory
    set(TOOLCHAIN_INC_DIR ${simplelinkcc32xxsdk_SOURCE_DIR}/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib)
    set(TOOLCHAIN_LIB_DIR ${simplelinkcc32xxsdk_SOURCE_DIR}/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib)
    
############################################
elseif( "${TARGET_SERIES}" STREQUAL "CC13x2_26x2" )
    
    # CC13x2 and CC26x2 series are using TI SimpleLink CC13x2_26x2 SDK and TI RTOS

    # check if TI_SL_CC13x2_26x2_SDK_SOURCE was specified or if it's empty (default is empty)
    set(NO_TI_SL_CC13x2_26x2_SDK_SOURCE TRUE)
    if(TI_SL_CC13x2_26x2_SDK_SOURCE)
        if(NOT "${TI_SL_CC13x2_26x2_SDK_SOURCE}" STREQUAL "")
            set(NO_TI_SL_CC13x2_26x2_SDK_SOURCE FALSE)
        endif()
    endif()

    # WHEN CHANGING THIS MAKE SURE TO UPDATE THE DEV CONTAINERS
    set(TI_SL_CC13x2_26x2_SDK_TAG "5.10.00.48" CACHE INTERNAL "TI CC13x2_26x2 SDK tag")

    if(NO_TI_SL_CC13x2_26x2_SDK_SOURCE)
        # no TI SimpleLink CC13x2_26x2 SDK source specified, download it from our repo

        message(STATUS "TI SimpleLink CC13x2_26x2 SDK (latest available code from v${TI_SL_CC13x2_26x2_SDK_TAG})")

        FetchContent_Declare(
            simplelinkcc13x2_26x2sdk
            GIT_REPOSITORY https://github.com/nanoframework/SimpleLink_CC13x2_26x2_SDK
            GIT_TAG ${TI_SL_CC13x2_26x2_SDK_TAG}
        )

    else()
        # TI SimpleLink CC13x2_26x2 SDK source was specified


        # sanity check is source path exists
        if(EXISTS "${TI_SL_CC13x2_26x2_SDK_SOURCE}/")
            message(STATUS "TI SimpleLink CC13x2_26x2 SDK ${TI_SL_CC13x2_26x2_SDK_TAG} source from: ${TI_SL_CC13x2_26x2_SDK_SOURCE}")

            FetchContent_Declare(
                simplelinkcc13x2_26x2sdk
                GIT_REPOSITORY ${TI_SL_CC13x2_26x2_SDK_SOURCE}
                GIT_TAG ${TI_SL_CC13x2_26x2_SDK_TAG}
            )

        else()
            message(FATAL_ERROR "Couldn't find TI SimpleLink CC13x2_26x2 SDK source at ${TI_SL_CC13x2_26x2_SDK_SOURCE}/")
        endif()

    endif()

    FetchContent_GetProperties(simplelinkcc13x2_26x2sdk)
    FetchContent_Populate(simplelinkcc13x2_26x2sdk)
    
    # check if TI_SYSCONFIG_SOURCE was specified or if it's empty (default is empty)
    set(NO_TI_SYSCONFIG_SOURCE TRUE)
    if(TI_SYSCONFIG_SOURCE)
        if(NOT "${TI_SYSCONFIG_SOURCE}" STREQUAL "")
            set(NO_TI_SYSCONFIG_SOURCE FALSE)
        endif()
    endif()

    # TI SysConfig version
    set(TI_SYSCONFIG_VERSION_EMPTY TRUE)

    # check if build was requested with a specifc TI SysConfig version
    if(TI_SYSCONFIG_VERSION_EMPTY)
        # no TI SysConfig version actualy specified, must be empty which is fine, we'll default to a known good version
            # WHEN CHANGING THIS MAKE SURE TO UPDATE THE DEV CONTAINERS
        set(TI_SYSCONFIG_VERSION_TAG "1.8.1")
    else()
        # set Git tag
        set(TI_SYSCONFIG_VERSION_TAG "${TI_SYSCONFIG_VERSION}")
    endif()
    
    if(NO_TI_SYSCONFIG_SOURCE)
        # no TI SysConfig source specified, download it from it's repo
        
        message(STATUS "TI SysConfig ${TI_SYSCONFIG_VERSION_TAG} from GitHub repo")
        
        FetchContent_Declare(
            ti_sysconfig
            GIT_REPOSITORY https://github.com/nanoframework/TI_SysConfig
            GIT_TAG ${TI_SYSCONFIG_VERSION_TAG}
        )

    else()
        # TI SysConfig source was specified

        # sanity check is source path exists
        if(EXISTS "${TI_SYSCONFIG_SOURCE}/")
            message(STATUS "TI SysConfig ${TI_SYSCONFIG_VERSION_TAG} (source from: ${TI_SYSCONFIG_SOURCE})")
           
            FetchContent_Declare(
                ti_sysconfig
                GIT_REPOSITORY ${TI_SYSCONFIG_SOURCE}
                GIT_TAG ${TI_SYSCONFIG_VERSION_TAG}
            )
    
        else()
            message(FATAL_ERROR "Couldn't find TI SysConfig source at ${TI_SYSCONFIG_SOURCE}/")
        endif()

    endif()

    FetchContent_GetProperties(ti_sysconfig)
    FetchContent_Populate(ti_sysconfig)
        
    # check if TI_XDCTOOLS_SOURCE was specified or if it's empty (default is empty)
    set(NO_TI_XDCTOOLS_SOURCE TRUE)
    if(TI_XDCTOOLS_SOURCE)
        if(NOT "${TI_XDCTOOLS_SOURCE}" STREQUAL "")
            set(NO_TI_XDCTOOLS_SOURCE FALSE)
        endif()
    endif()
    
    # TI XDCTools version
    set(TI_XDCTOOLS_VERSION_EMPTY TRUE)

    # check if build was requested with a specifc TI XDCTools version
    if(TI_XDCTOOLS_VERSION_EMPTY)
        # no TI XDCTools version actualy specified, must be empty which is fine, we'll default to a known good version
        # WHEN CHANGING THIS MAKE SURE TO UPDATE THE DEV CONTAINERS
        set(TI_XDCTOOLS_VERSION_TAG "3.62.00.08")
    else()
        # set tag
        set(TI_XDCTOOLS_VERSION_TAG "${TI_XDCTOOLS_VERSION}")
    endif()

    if(NO_TI_XDCTOOLS_SOURCE)
        # no TI XDCTools source specified, download it from it's repo

        message(STATUS "TI XDCTools ${TI_XDCTOOLS_VERSION_TAG} from GitHub repo")

        FetchContent_Declare(
            ti_xdctools
            GIT_REPOSITORY https://github.com/nanoframework/TI_XDCTools
            GIT_TAG ${TI_XDCTOOLS_VERSION_TAG}
        )

    else()
        # TI XDCTools source was specified

        # sanity check is source path exists
        if(EXISTS "${TI_XDCTOOLS_SOURCE}/")
            message(STATUS "TI XDCTools ${TI_XDCTOOLS_VERSION_TAG} (source from: ${TI_XDCTOOLS_SOURCE})")

            FetchContent_Declare(
                ti_xdctools
                SOURCE_DIR ${TI_XDCTOOLS_SOURCE}
                # GIT_TAG ${TI_XDCTOOLS_VERSION_TAG}
            )

        else()
            message(FATAL_ERROR "Couldn't find TI XDCTools source at ${TI_XDCTOOLS_SOURCE}/")
        endif()

    endif()
    
    FetchContent_GetProperties(ti_xdctools)
    FetchContent_Populate(ti_xdctools)

    set(TI_XDCTools_INCLUDE_DIR ${ti_xdctools_SOURCE_DIR}/packages/ PARENT_SCOPE)
     
    # need to override the toolchain lib directory
    set(TOOLCHAIN_INC_DIR ${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib)
    set(TOOLCHAIN_LIB_DIR ${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib)

endif()
