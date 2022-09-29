#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

#
# If the CPU supports trusted execution, 
# only add in non-secure side of OS.
#

if (CONFIG_ARCH_HAS_TRUSTED_EXECUTION)
    add_subdirectory_ifdef(CONFIG_TRUSTED_EXECUTION_NONSECURE nf_lib)
else()
    add_subdirectory(nf_lib)
endif()

