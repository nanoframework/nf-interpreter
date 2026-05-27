#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)
include(binutils.TI_SimpleLink)

nf_add_platform_sysconfig_steps(CC1352 CC13X2)

nf_setup_target_build(
    
    CLR_EXTRA_SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/CC1352R1_LAUNCHXL_fxns.c

    CLR_EXTRA_COMPILE_DEFINITIONS
        -DDeviceFamily_CC13X2

    CLR_LINKER_FILE 
        CC13x2_26x2_CLR
  
    CLR_EXTRA_LINK_FLAGS
        "-Wl,-T,${CMAKE_CURRENT_BINARY_DIR}/configPkg/linker.cmd"

    CLR_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/TI_SimpleLink/common,--defsym=__crt_heap_size__=0x0"
)

target_compile_definitions(NF_NativeAssemblies PUBLIC -DDeviceFamily_CC13X2)
