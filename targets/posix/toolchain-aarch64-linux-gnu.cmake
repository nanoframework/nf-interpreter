#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# CMake toolchain file for cross-compiling to Linux arm64 (aarch64)
# on an x86_64 Linux host using the aarch64-linux-gnu GCC cross-toolchain.
#
# Install the cross-toolchain with:
#   sudo apt-get install -y gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
#
# Usage (passed automatically by CI; not needed for native arm64 builds):
#   cmake -S targets/posix -B build/posix -G Ninja \
#         -DCMAKE_TOOLCHAIN_FILE=targets/posix/toolchain-aarch64-linux-gnu.cmake

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_C_COMPILER   aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

# Do not search host paths for libraries/includes/programs.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
