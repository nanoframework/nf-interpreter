#!/usr/bin/env python3
 
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

"""
Validate defconfig files in the nanoFramework repository.

This script is intended for CI use.  It loads defconfig file(s) into
kconfiglib against the KCONFIG_ROOT schema, checks for parse errors and
undefined symbols, and optionally verifies canonical (minimal) form.

When --target is given, only the defconfig for that CMake preset is
validated.  Without --target, all defconfigs under targets/ and
targets-community/ are validated.

Exit codes:
    0   All defconfigs are valid.
    1   One or more defconfigs failed validation.

Usage:
    python validate_defconfigs.py [--target PRESET] [--check-canonical] [--verbose]
"""

import argparse
import json
import os
import sys
import tempfile
import glob

# Ensure srctree is set so Kconfig relative sources resolve.
_REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
os.environ.setdefault("srctree", _REPO_ROOT)

import kconfiglib


def find_defconfigs(repo_root: str) -> list[str]:
    """Return all defconfig file paths in the repo."""
    patterns = [
        "targets/ESP32/defconfig/*_defconfig",
        "targets/ChibiOS/*/defconfig",
        "targets/ChibiOS/*/defconfig_*",
        "targets/FreeRTOS/**/defconfig",
        "targets/TI_SimpleLink/*/defconfig",
        "targets/ThreadX/**/defconfig",
        "targets/ThreadX/**/defconfig_*",
        "targets-community/ChibiOS/*/defconfig",
        "targets-community/TI_SimpleLink/*/defconfig",
    ]
    results = []
    for pat in patterns:
        results.extend(glob.glob(os.path.join(repo_root, pat), recursive=True))
    # Deduplicate and sort
    return sorted(set(results))


def find_defconfig_for_preset(repo_root: str, preset_name: str) -> str | None:
    """Look up the defconfig path for a CMake preset by scanning CMakePresets.json files."""
    for preset_file in glob.glob(
        os.path.join(repo_root, "targets*/**/CMakePresets.json"), recursive=True
    ):
        try:
            with open(preset_file, "r", encoding="utf-8") as f:
                data = json.load(f)
        except (json.JSONDecodeError, OSError):
            continue
        for preset in data.get("configurePresets", []):
            if preset.get("name") == preset_name:
                defconfig = preset.get("cacheVariables", {}).get("NF_TARGET_DEFCONFIG")
                if defconfig:
                    return os.path.join(repo_root, defconfig)
    return None


def validate_defconfig(
    kconfig_root: str,
    defconfig_path: str,
    check_canonical: bool,
    verbose: bool,
) -> bool:
    """Validate a single defconfig. Returns True on success."""
    rel = os.path.relpath(defconfig_path, _REPO_ROOT)

    try:
        kconf = kconfiglib.Kconfig(kconfig_root, warn=False)
        kconf.load_config(defconfig_path)
    except Exception as exc:
        print(f"  FAIL  {rel}: load error: {exc}")
        return False

    # Check for undefined symbols referenced in the defconfig
    try:
        with open(defconfig_path, "r", encoding="utf-8") as f:
            lines = f.readlines()
    except OSError as exc:
        print(f"  FAIL  {rel}: read error: {exc}")
        return False

    undefined = []
    for line in lines:
        line = line.strip()
        if not line:
            continue
        # Handle "# CONFIG_FOO is not set" lines (disabled symbols)
        if line.startswith("# CONFIG_") and line.endswith(" is not set"):
            sym_name = line[len("# CONFIG_"):-len(" is not set")]
            if sym_name not in kconf.syms:
                undefined.append(sym_name)
            continue
        if line.startswith("#"):
            continue
        if "=" in line:
            sym_name = line.split("=", 1)[0]
            if sym_name.startswith("CONFIG_"):
                sym_name = sym_name[len("CONFIG_"):]
            if sym_name not in kconf.syms:
                undefined.append(sym_name)

    if undefined:
        print(f"  FAIL  {rel}: undefined symbols: {', '.join(undefined)}")
        return False

    if check_canonical:
        # Save to a temp file in minimal (savedefconfig) form and compare
        tmp_path = None
        try:
            with tempfile.NamedTemporaryFile(
                mode="w", suffix="_defconfig", delete=False
            ) as tmp:
                tmp_path = tmp.name
            kconf.write_min_config(tmp_path)

            with open(defconfig_path, "r", encoding="utf-8") as f:
                original = f.read()
            with open(tmp_path, "r", encoding="utf-8") as f:
                canonical = f.read()

            if original != canonical:
                print(f"  WARN  {rel}: not in canonical form (run savedefconfig)")
                if verbose:
                    # Show a brief diff summary
                    orig_lines = set(original.strip().splitlines())
                    canon_lines = set(canonical.strip().splitlines())
                    added = canon_lines - orig_lines
                    removed = orig_lines - canon_lines
                    if removed:
                        print(f"         - extra lines: {len(removed)}")
                    if added:
                        print(f"         + missing lines: {len(added)}")
                # Non-canonical form is a warning, not a failure
        except Exception as exc:
            print(f"  WARN  {rel}: canonical check failed: {exc}")
        finally:
            if tmp_path is not None and os.path.exists(tmp_path):
                os.unlink(tmp_path)

    if verbose:
        print(f"  OK    {rel}")
    return True


def main():
    parser = argparse.ArgumentParser(description="Validate nanoFramework defconfig files")
    parser.add_argument(
        "--target",
        metavar="PRESET",
        help="CMake preset name — validate only that target's defconfig",
    )
    parser.add_argument(
        "--check-canonical",
        action="store_true",
        help="Warn if defconfigs are not in minimal canonical form",
    )
    parser.add_argument("--verbose", "-v", action="store_true", help="Show all results")
    args = parser.parse_args()

    kconfig_root = os.path.join(_REPO_ROOT, "Kconfig")
    if not os.path.isfile(kconfig_root):
        print(f"Error: Kconfig root not found: {kconfig_root}", file=sys.stderr)
        sys.exit(1)

    if args.target:
        path = find_defconfig_for_preset(_REPO_ROOT, args.target)
        if path is None:
            print(
                f"Error: no defconfig found for preset '{args.target}'",
                file=sys.stderr,
            )
            sys.exit(1)
        if not os.path.isfile(path):
            print(
                f"Error: defconfig file does not exist: {path}",
                file=sys.stderr,
            )
            sys.exit(1)
        defconfigs = [path]
    else:
        defconfigs = find_defconfigs(_REPO_ROOT)
        if not defconfigs:
            print("Error: no defconfig files found", file=sys.stderr)
            sys.exit(1)

    print(f"Validating {len(defconfigs)} defconfig files...")

    failures = 0
    for dc in defconfigs:
        if not validate_defconfig(kconfig_root, dc, args.check_canonical, args.verbose):
            failures += 1

    print()
    if failures:
        print(f"FAILED: {failures}/{len(defconfigs)} defconfigs have errors")
        sys.exit(1)
    else:
        print(f"PASSED: all {len(defconfigs)} defconfigs are valid")
        sys.exit(0)


if __name__ == "__main__":
    main()
