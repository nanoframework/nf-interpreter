#!/usr/bin/env python3
#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#
# nf_genconfig.py — Generate nf_config.h from a Kconfig configuration.
#
# Wraps kconfiglib's write_autoconf() and additionally emits
#   #define CONFIG_FOO 0
# for every bool/tristate symbol that is disabled (absent from the standard
# output).  This makes every CONFIG_* symbol unconditionally defined so that
# #if CONFIG_FOO compiles cleanly under -Werror=undef without requiring each
# source file to know which symbols exist.
#
# Usage:
#   python nf_genconfig.py <Kconfig-root> <.config> <output-header>
#

import sys
import os
import kconfiglib

# Resolve type constants — esp_kconfiglib (used inside ESP-IDF virtualenvs)
# exposes them under esp_kconfiglib.core rather than at the package top level.
try:
    _BOOL     = kconfiglib.BOOL
    _TRISTATE = kconfiglib.TRISTATE
except AttributeError:
    try:
        import esp_kconfiglib.core as _kcore
        _BOOL     = _kcore.BOOL
        _TRISTATE = _kcore.TRISTATE
    except (ImportError, AttributeError):
        # Fallback to numeric values (stable across kconfiglib v12+)
        _BOOL     = 3
        _TRISTATE = 4

def main():
    if len(sys.argv) != 4:
        print(f"Usage: {sys.argv[0]} <Kconfig-root> <.config> <output-header>", file=sys.stderr)
        sys.exit(1)

    kconfig_root = sys.argv[1]
    dot_config   = sys.argv[2]
    header_file  = sys.argv[3]

    # Load Kconfig schema and apply the resolved .config
    os.environ.setdefault("srctree", os.getcwd())
    kconf = kconfiglib.Kconfig(kconfig_root, warn_to_stderr=False)
    kconf.load_config(dot_config)

    # Remove any stale nf_config.h before writing.
    if os.path.exists(header_file):
        os.remove(header_file)

    # Write the standard autoconf header (enabled symbols only)
    kconf.write_autoconf(header_file)

    # Append #define CONFIG_X 0 for every bool/tristate symbol that is
    # disabled so that #if CONFIG_X is always well-defined (-Werror=undef).
    with open(header_file, "r") as f:
        content = f.read()

    defined = set()
    for line in content.splitlines():
        if line.startswith("#define CONFIG_"):
            defined.add(line.split()[1])

    disabled = []
    for sym in kconf.unique_defined_syms:
        if sym.type in (_BOOL, _TRISTATE):
            name = "CONFIG_" + sym.name
            if name not in defined:
                disabled.append(name)

    if disabled:
        with open(header_file, "a") as f:
            f.write("\n/* Disabled Kconfig symbols — defined as 0 for -Werror=undef compatibility */\n")
            for name in disabled:
                f.write(f"#define {name} 0\n")

if __name__ == "__main__":
    main()
