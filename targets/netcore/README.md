# Windows virtual-device networking

The current Windows virtual device (`nanoFramework.nanoCLR`) uses native Winsock
for IPv4 TCP/UDP and DNS. TLS clients and servers use statically linked
**mbedTLS**, not Schannel, OpenSSL, lwIP or a managed networking relay.

The socket table, asynchronous resolver, managed networking bridge and mbedTLS
implementation are shared with the Linux host target under
[`src/PAL/Network`](../../src/PAL/Network). Both targets include the shared
`Host_Network.h` and `Host_Tls.h` headers directly. Windows-specific adapter
enumeration lives beside the Windows DLL project; Linux interface enumeration
remains in the POSIX target. No POSIX platform shims are added to the Windows
include path.

## Building

Install Visual Studio 2022 (or newer) with:

- Desktop development with C++, including the project's v143 toolset.
- A Windows SDK.
- C++ CMake tools for Windows, or CMake on `PATH`.
- Git, for downloading the pinned mbedTLS sources and submodules.

With Visual Studio 2026 and only the v145 toolset installed, add
`/p:PlatformToolset=v145` to the build command; installing v143 is not required
when explicitly overriding the toolset.

Set `LITTLEFS_PATH` to an existing littlefs checkout, as required by the virtual
device. From a Visual Studio Developer Command Prompt:

```bat
msbuild targets\netcore\nanoCLR.sln /restore /m /p:Configuration=Debug /p:Platform=x64
```

The native project also supports `Platform=Win32`. It builds the repository-pinned
mbedTLS version through `Network.targets` and `targets/netcore/CMakeLists.txt`.
Architecture-specific build trees are kept under `build/mbedtls`; Debug and
Release install directories are separate. No mbedTLS DLL is required at runtime.
The mbedTLS license is copied beside the output and included in the CLI package.

Optional MSBuild properties:

- `MBEDTLS_SOURCE`: an existing checkout of the repository-supported mbedTLS
  version, including its submodules; otherwise sources are fetched.
- `CMakeExecutable`: full path to CMake if automatic discovery is unsuitable.
- `MbedTlsGenerator`: override the Visual Studio CMake generator.

## Networking behavior

- IPv4 TCP clients/servers, UDP (including empty datagrams), DNS, socket options,
  numeric IPv4 addresses and read-only interface enumeration.
- `SOCKET` values remain native-width, including on x64. Managed socket handles
  are separate monotonically allocated integers, not truncated Windows handles.
- Native sockets are nonblocking. A one-shot Winsock `select` worker and a
  loopback wake socket integrate readiness with the CLR scheduler; DNS runs on a
  separate worker. Shutdown joins both workers before `WSACleanup`.
- Up to 128 live sockets and 16 outstanding resolver jobs.
- `DontRoute` is supported on send and rejected on receive with error 10045.
  Windows linger timeouts must fit the native unsigned 16-bit field; larger or
  negative values fail explicitly.
- `GetAdaptersAddresses` supplies interface indices, one IPv4 address/subnet,
  gateway, hardware address and up to two per-adapter IPv4 DNS servers.
  Loopback and unconfigured adapters remain enumerable. Availability excludes
  loopback, PPP and tunnel adapters and requires an up adapter with IPv4.
- The host owns routes, address assignment, DNS policy and firewall rules.
  Managed interface reconfiguration, Wi-Fi provisioning, IPv6 and network-change
  notifications are not implemented. DNS shutdown can wait for an OS lookup.

Loopback refers to the Windows host, not a WSL distribution or container.
Windows Firewall may require permission for an application's listening port.
The CLI's wire-protocol TCP port is separate from application sockets.

## TLS and CA trust

TLS 1.2/1.3, client connections and server identities use the same implementation
and trust policy as [Linux TLS](../posix/README.md#linux-tls):

1. A managed per-connection CA certificate.
2. A managed global bundle from `CertificateManager.AddCaCertificateBundle`.
3. The CLI's `--ca-bundle` file option.
4. The `NANOCLR_CA_BUNDLE` environment variable.

```powershell
$env:NANOCLR_CA_BUNDLE = 'C:\certificates\roots.pem'
nanoclr run --assemblies C:\app\mscorlib.pe C:\app\System.Net.pe C:\app\App.pe

# Override the environment-selected file:
nanoclr run --ca-bundle C:\certificates\other-roots.pem --assemblies C:\app\mscorlib.pe C:\app\System.Net.pe C:\app\App.pe
```

No roots are embedded or shipped, and the Windows certificate store is not
automatically imported. A CA path is optional when managed code supplies trust.
Without any trust source, verified clients fail explicitly. Invalid configured
files never silently fall back. Windows CA paths support Unicode.

Explicit `SslVerification.NoVerification` disables chain, date and hostname/IP
verification for that context only and does not load CA roots. Encryption
without authentication is vulnerable to server impersonation; use this only
when deliberately required. Default verification remains enabled.

Servers use a managed `X509Certificate2` certificate/private-key identity and
do not require a CA path. Mutual TLS/client identities and TLS 1.0/1.1 remain
unsupported.

## Validation

Build and run matching System.Net managed tests on the Windows DLL, including
TCP/UDP round trips, unknown-handle polling, TLS 1.2/1.3 verified and unverified
clients, wrong CA/hostname failures, server identities and peer shutdown.
Deploy matching NFMRK2 assemblies and the System.Net native contract implemented
by the checkout; arbitrary released NuGet assemblies may not match.

Linux regression runs validate the shared implementation but do **not** substitute
for a native Windows build and runtime test.
