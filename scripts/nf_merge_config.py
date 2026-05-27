#!/usr/bin/env python3

# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

"""
Merge a board defconfig with an optional user config overlay into a full .config.

Usage:
    python nf_merge_config.py <kconfig_root> <defconfig> <user_overlay> <output_config>

Arguments:
    kconfig_root   Path to the root Kconfig file (e.g. /path/to/nf-interpreter/Kconfig)
    defconfig      Path to the board/target minimal defconfig
    user_overlay   Path to the optional user config fragment (pass "" or a
                   non-existent path to skip the overlay step)
    output_config  Path where the resulting full .config should be written

The user_overlay file, if it exists, is loaded on top of the board defconfig
using Kconfig's non-replacing load (replace=False), so only the symbols
explicitly listed in the overlay are modified.
"""
import sys
import os

# Allow running from any directory — kconfiglib needs srctree when Kconfig
# files use relative `source` directives.  The caller (NF_Kconfig.cmake) sets
# this via WORKING_DIRECTORY, but guard against direct invocations.
if "srctree" not in os.environ:
    os.environ["srctree"] = os.getcwd()

import kconfiglib


def main():
    if len(sys.argv) != 5:
        print(__doc__)
        sys.exit(1)

    kconfig_root, defconfig_path, user_overlay, output_config = sys.argv[1:]

    if not os.path.isfile(kconfig_root):
        print(f"Error: Kconfig root not found: {kconfig_root}", file=sys.stderr)
        sys.exit(1)

    if not os.path.isfile(defconfig_path):
        print(f"Error: defconfig not found: {defconfig_path}", file=sys.stderr)
        sys.exit(1)

    kconf = kconfiglib.Kconfig(kconfig_root, warn=False)
    kconf.load_config(defconfig_path)

    if user_overlay and os.path.isfile(user_overlay):
        kconf.load_config(user_overlay, replace=False)

    kconf.write_config(output_config)


if __name__ == "__main__":
    main()
