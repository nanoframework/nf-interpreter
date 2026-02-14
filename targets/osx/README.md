# .NET nanoFramework nanoCLR — macOS Host Target (Initial Scaffold)

This folder introduces an initial macOS host target under:

- targets/osx

The purpose of this PR is to establish a buildable, CI-validated entry point for a native macOS host nanoCLR implementation, without impacting existing targets.

---

## Status

This PR provides:

- A CMake + Ninja build entrypoint for macOS.
- A minimal host glue structure aligned with targets/win32.
- A smoke-test executable (--help, --version, banner output).
- GitHub Actions workflow validating macOS build and execution.

This is intentionally a scaffolding baseline.

No shared CLR runtime integration is included yet.

---

## Scope of this PR

This PR:

- Adds a new, isolated target (targets/osx).
- Does not modify existing Win32 or RTOS targets.
- Does not alter shared runtime behavior.
- Adds CI coverage for macOS builds.

This ensures zero regression risk to existing platforms.

---

## Build

cmake -S targets/osx -B build/osx -G Ninja -DNANO_OSX_ARCH=arm64 -DNANO_OSX_ENABLE_SMOKE=ON
cmake --build build/osx
./build/osx/bin/nanoFramework.nanoCLR --version

For Intel macOS:

cmake -S targets/osx -B build/osx -G Ninja -DNANO_OSX_ARCH=x86_64 -DNANO_OSX_ENABLE_SMOKE=ON
cmake --build build/osx

---

## Configuration Options

NANO_OSX_ARCH
- Default: arm64
- Accepted values: arm64, x86_64

NANO_OSX_ENABLE_SMOKE
- Default: ON
- Enables minimal command-line smoke behavior.

---

## Current Limitations (Intentional)

This initial scaffold does not include:

- Managed filesystem support.
- Networking support.
- Integration with the shared CLR runtime startup path (src/CLR/**).
- Full timer/event implementation (placeholder only).

These are deferred to follow-up PRs.

---

## Motivation

This target enables:

- macOS-native CI validation.
- A clear landing zone for future host nanoCLR work.
- Gradual portability improvements without destabilizing existing targets.

By introducing a minimal scaffold first, subsequent runtime integration can be implemented incrementally and safely.

---

## Follow-up Roadmap (Incremental PRs)

1. Wire minimal shared runtime startup path (src/CLR/**) behind small host portability shims.
2. Introduce a lightweight Host abstraction layer to replace Windows-specific assumptions in VIRTUAL_DEVICE paths.
3. Replace placeholder PAL/HAL glue with deterministic POSIX/macOS implementations.
4. Enable optional managed payload smoke execution.
5. Add networking/TLS support in a dedicated follow-up PR.
