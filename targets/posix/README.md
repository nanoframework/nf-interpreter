# .NET nanoFramework nanoCLR - POSIX Host Target

This target provides a host build of nanoCLR for macOS and Linux at `targets/posix`.

## Status

The target is fully functional as a managed-code execution host:

- Produces `nanoFramework.nanoCLR.dylib` (macOS) / `.so` (Linux) and a thin test harness `nanoFramework.nanoCLR.test`, matching the architecture of the win32 virtual-device target.
- The full CLR core (`src/CLR/Core/`), CoreLib (`src/CLR/CorLib/`), GC, type system, execution engine, and thread scheduler are compiled and linked.
- Managed PE assemblies are loaded from memory buffers and executed end-to-end; the nanoFramework.Json test suite passes 249/249 tests.
- All native assemblies from the win32 target that are applicable to a software host are registered (see [Native Assembly Table](#native-assembly-table) below).
- CMake + Ninja build tested on macOS 14 arm64 and Ubuntu 22.04 x86_64 via GitHub Actions.

## Build

### macOS (Apple Silicon)

```bash
cmake -S targets/posix -B build/posix -G Ninja -DNANO_POSIX_ARCH=arm64
cmake --build build/posix
```

### macOS (Intel)

```bash
cmake -S targets/posix -B build/posix -G Ninja -DNANO_POSIX_ARCH=x86_64
cmake --build build/posix
```

### Linux

```bash
cmake -S targets/posix -B build/posix -G Ninja
cmake --build build/posix
```

## Configuration Options

`NANO_POSIX_ARCH` *(macOS only)*
- Default: `arm64`
- Accepted values: `arm64`, `x86_64`
- Has no effect on Linux (CMake uses the host architecture automatically).

`NANO_POSIX_ENABLE_SMOKE`
- Default: `ON`
- Enables minimal command-line smoke behaviour.

## Running Managed Tests

Pass PE files directly to the test harness in the same order used by the nanoclr global tool:

```bash
./build/posix/bin/nanoFramework.nanoCLR.test \
    mscorlib.pe \
    nanoFramework.UnitTestLauncher.pe \
    nanoFramework.TestFramework.pe \
    <additional dependency PEs...> \
    NFUnitTest.pe
```

## POSIX Shim Headers

`targets/posix/Include/` contains shim headers searched **before** the shared CLR/HAL/PAL paths. They shadow headers that would otherwise pull in Windows-only content (e.g. `nanoPAL.h`, `nanoHAL_Boot.h`) and provide no-op replacements for optional subsystems not present on a POSIX host.

The macro `PLATFORM_POSIX_HOST` is defined by the build system and gates POSIX-specific branches in the shared headers.

## Native Assembly Table

The interop table in `posix_stubs.cpp` registers the same portable set as the win32 virtual-device target. Hardware/network assemblies (GPIO, I2C, SPI, IO.Ports) are included with `NotImplementedStub()` bodies — they satisfy the PE checksum check at load time so tests that reference these assemblies don't abort, but calling their native methods throws `NotImplementedException`.

| Assembly | Notes |
|---|---|
| `mscorlib` | Full native implementation |
| `nanoFramework.Runtime.Native` | Full |
| `nanoFramework.System.Collections` | Full |
| `nanoFramework.System.Text` | Full |
| `System.Math` | Full |
| `System.Runtime.Serialization` | Full |
| `nanoFramework.ResourceManager` | Full |
| `nanoFramework.System.IO.Hashing` | Full (CRC32) |
| `nanoFramework.Runtime.Events` | Full |
| `nanoFramework.Runtime.Events_EventSink_DriverProcs` | Full |
| `System.Device.Gpio` | Stubs — `NotImplementedException` |
| `System.Device.I2c` | Stubs — `NotImplementedException` |
| `System.Device.Spi` | Stubs — `NotImplementedException` |
| `System.IO.Ports` | Stubs — `NotImplementedException` |

Not included (require a network stack or platform filesystem not present on the host):
`nanoFramework.Networking.Sntp`, `System.Net`, `System.IO.FileSystem`.

## Known Limitations

- No networking or TLS support.
- No managed filesystem (`System.IO.FileSystem`) — PAL filesystem calls are no-ops.
- `System.Device.Gpio/I2c/Spi` and `System.IO.Ports` native methods throw `NotImplementedException`.
- Wire Protocol transport is not connected (debugger attach not supported).
