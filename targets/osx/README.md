# .NET nanoFramework nanoCLR - macOS Host Target (Scaffold)

This target provides an initial macOS host scaffold at `targets/osx`.

## Status

The target currently provides:

- A CMake + Ninja build entrypoint for macOS.
- A minimal host glue structure aligned with `targets/win32`.
- A smoke executable (`--help`, `--version`, banner output).
- A GitHub Actions workflow that builds and runs smoke on macOS.

This target is intentionally a scaffold baseline.

## Build

Apple Silicon:

```bash
cmake -S targets/osx -B build/osx -G Ninja -DNANO_OSX_ARCH=arm64 -DNANO_OSX_ENABLE_SMOKE=ON
cmake --build build/osx
./build/osx/bin/nanoFramework.nanoCLR --version
```

Intel macOS:

```bash
cmake -S targets/osx -B build/osx -G Ninja -DNANO_OSX_ARCH=x86_64 -DNANO_OSX_ENABLE_SMOKE=ON
cmake --build build/osx
./build/osx/bin/nanoFramework.nanoCLR --version
```

## Configuration Options

`NANO_OSX_ARCH`
- Default: `arm64`
- Accepted values: `arm64`, `x86_64`

`NANO_OSX_ENABLE_SMOKE`
- Default: `ON`
- Enables minimal command-line smoke behavior.

## Current Limitations (Intentional)

This target does not yet include:

- Managed filesystem support.
- Networking support.
- Integration with the shared CLR runtime startup path (`src/CLR/**`).
- Full timer/event behavior (placeholder implementation only).

## Follow-up Roadmap

1. Wire a minimal shared runtime startup path (`src/CLR/**`) behind small host portability shims.
2. Replace placeholder PAL/HAL glue with deterministic POSIX/macOS implementations.
3. Enable optional managed payload smoke execution.
4. Add networking/TLS support in a dedicated follow-up change.
