# .NET nanoFramework nanoCLR - POSIX Host Target

This target provides a host build of nanoCLR for macOS and Linux at `targets/posix`.

## Status

The target is fully functional as a managed-code execution host:

- Produces `nanoFramework.nanoCLR.dylib` (macOS) / `.so` (Linux) and a standalone application runner `nanoFramework.nanoCLR.test`, matching the architecture of the win32 virtual-device target. The `.test` suffix is part of the runner's name, not a separate test suite.
- The full CLR core (`src/CLR/Core/`), CoreLib (`src/CLR/CorLib/`), GC, type system, execution engine, and thread scheduler are compiled and linked.
- Managed PE assemblies are loaded from memory buffers and executed by the CLR.
- All native assemblies from the win32 target that are applicable to a software host are registered (see [Native Assembly Table](#native-assembly-table) below).
- CMake + Ninja builds are configured in GitHub Actions for macOS and Linux.

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

Linux TLS statically links the repository-pinned mbedTLS (currently 3.6.7);
no OpenSSL or system mbedTLS development/runtime packages are required.
The first configure fetches its sources using Git. For an existing checkout,
pass `-DMBEDTLS_SOURCE=/absolute/path/to/mbedtls`.

```bash
cmake -S targets/posix -B build/posix -G Ninja
cmake --build build/posix
```

Keep the generated `bin/`, `lib/` and `share/` directories together when deploying.
`share/nanoFramework/` contains the default CA bundle and third-party licenses.

## Configuration Options

`NANO_POSIX_ARCH` *(macOS only)*
- Default: `arm64`
- Accepted values: `arm64`, `x86_64`
- Has no effect on Linux (CMake uses the host architecture automatically).

`NANO_POSIX_ENABLE_SMOKE`
- Default: `ON`
- Builds the standalone command-line application runner, including its startup banner/version output.

## Linux Networking (including WSL)

Linux builds register `System.Net` and use the Linux socket API directly. No plugin,
embedded TCP/IP stack, desktop .NET relay, root privileges, or network-interface
configuration is required. The application uses the host's network namespace,
routing, DNS configuration, permissions and firewall rules.

Supported in this first implementation:

- IPv4 TCP client/server: connect, bind, listen, accept, send/receive, half-close,
  local/remote endpoints and EOF.
- IPv4 UDP, including connected UDP, one-message-per-call boundaries, source
  endpoints and zero-length datagrams.
- Nonblocking native descriptors with managed waits and timeouts handled by the
  CLR scheduler. A one-shot `poll` worker wakes the CLR's existing socket event;
  idle sockets are not repeatedly polled for writable readiness.
- Native DNS (`getaddrinfo`, including `/etc/hosts`) on a worker thread, returning
  IPv4 results. The CLR remains schedulable while resolution is pending. Each
  managed lookup has a 30-second deadline; up to 16 lookups can be outstanding.
- Numeric IPv4 parsing/formatting and `NetworkInterface.GetIsNetworkAvailable()`.
  Availability means an up/running, non-loopback/non-point-to-point IPv4 interface exists, not that
  an Internet service or DNS server is reachable.
- Read-only `NetworkInterface.GetAllNetworkInterfaces()`: actual Linux interfaces,
  including loopback and currently unconfigured links, sorted by native interface
  index. Reports one IPv4 address/subnet per link, a default IPv4 gateway from
  `/proc/net/route` (lowest metric per interface), hardware address and up to two
  system-wide IPv4 DNS entries from `/etc/resolv.conf`. The resolver itself still
  uses the host's native resolver, including any split-DNS policy.
- Common integer socket options: reuse address, keepalive, broadcast, don't route,
  inline out-of-band data, send/receive buffer sizes, accept status, socket error,
  TCP no-delay, IPv4 TTL/TOS; `FIONREAD`/`Socket.Available`.
- TLS clients and HTTPS servers using statically linked mbedTLS, TLS 1.2/1.3, configurable CA trust
  or explicit managed CA certificates/bundles; see [Linux TLS](#linux-tls).

The native socket table is limited to 128 live sockets. CLR handles are independent
of OS descriptor numbers. All descriptors are nonblocking and close-on-exec;
closed handles cannot accidentally identify newly opened sockets. Connections and
resolver requests are discarded on runtime cleanup/soft reboot. Final shutdown
joins both workers before unloading native code.

### Managed WSL example

Build a nanoFramework console application referencing `System.Net` and using the
following entry point. Use CoreLibrary/System.Net assemblies and the metadata
processor matching this branch's native contracts and PE format; arbitrary
released NuGet assemblies may not match. Deploy the dependency PEs before the
application PE, as in [Running Managed Applications](#running-managed-applications).

All relative PE paths are resolved against the shell's current working directory,
not against the runner's directory. Use absolute paths if launching from elsewhere.
Error `0x9b000000` means an unsupported assembly format: this branch requires
`NFMRK2`, not the `NFMRK1` files produced by older metadata processors. Rebuild all
managed dependencies with matching tools/contracts; renaming files or changing
their format marker does not convert them.

In another terminal in the **same WSL distribution/network namespace**, start a
one-connection TCP echo server from Bash using `socat` (install on Ubuntu/WSL
with `sudo apt install socat`):

```bash
socat TCP4-LISTEN:9090,bind=127.0.0.1,reuseaddr EXEC:/bin/cat
```

Run the following client as the **nanoFramework application**:

```csharp
using System;
using System.Diagnostics;
using System.Net;
using System.Net.Sockets;

public class Program
{
    public static void Main()
    {
        IPHostEntry localhost = Dns.GetHostEntry("localhost");
        if (localhost.AddressList.Length == 0)
        {
            throw new Exception("DNS returned no IPv4 addresses.");
        }

        using (Socket socket = new Socket(
            AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp))
        {
            socket.ReceiveTimeout = 3000;
            socket.SendTimeout = 3000;
            socket.Connect(new IPEndPoint(IPAddress.Parse("127.0.0.1"), 9090));
            byte[] message = new byte[] { 80, 73, 78, 71 };
            socket.Send(message);
            byte[] reply = new byte[message.Length];
            int received = 0;
            while (received < reply.Length)
            {
                int count = socket.Receive(
                    reply, received, reply.Length - received, SocketFlags.None);
                if (count == 0)
                {
                    throw new Exception("Unexpected TCP EOF.");
                }
                received += count;
            }
            for (int i = 0; i < message.Length; ++i)
            {
                if (reply[i] != message[i])
                {
                    throw new Exception("Incorrect echo.");
                }
            }
        }
        Debug.WriteLine("DNS lookup and TCP echo completed.");
    }
}
```

Loopback refers to the namespace of the process running nanoCLR: a Docker
container's loopback is not the WSL host's loopback. Run both peers together, or
deliberately expose/use a reachable host endpoint. Reaching services on Windows
depends on the WSL networking mode and Windows firewall; that is host setup, not
virtual-device driver configuration.

### Deliberately unsupported

IPv6, mutual TLS, multicast-specific options, Wi-Fi provisioning,
SNTP and interface reconfiguration are not implemented here. Native
blocking mode and unmapped flags/options fail explicitly. Network-change events
are not raised. `NetworkHelper`/DHCP/static-IP operations should not be used to
configure the host; Linux already owns that configuration.

Interface objects are snapshots; a new enumeration refreshes them. Native refresh
after a rejected managed configuration change uses the stored Linux interface
index, not its position in the enumeration. The count/enumerate pair uses one
snapshot so link changes cannot shift entries mid-enumeration. Loopback and
non-Ethernet links report `Unknown`; Ethernet-compatible links report `Ethernet`,
without claiming wireless provisioning capabilities. DHCP and automatic-DNS
flags are false because a snapshot cannot determine the host's configuration
policy, not because the host must be statically configured. Missing addresses or
default gateways are `0.0.0.0`; no adapter or connectivity is fabricated.

The OS resolver cannot be forcibly cancelled safely: a managed timeout/cancel
discards the request, but an active native lookup may finish later. A slow lookup
delays subsequent queued lookups, and final shutdown waits for that active OS
call. Readiness waiting remains independent of DNS.

macOS and Windows networking are unchanged. Application sockets do not provide or
enable debugger/wire-protocol transport.

## Linux TLS

Linux TLS uses the repository-pinned mbedTLS, the host clock and operating-system
entropy. It does not automatically load the host's certificate store.
For outgoing client connections, chain verification, validity dates and the
requested DNS hostname/IP identity are always checked. SNI is sent for DNS names.
TLS 1.0/1.1 and client-side `SslVerification.NoVerification` are explicitly unsupported;
`SslProtocols.None` negotiates TLS 1.2/1.3.

Trust selection, in order:

1. A CA passed through `HttpWebRequest.HttpsAuthentCert` or
   `SslStream.AuthenticateAsClient(..., ca, ...)` is the **exclusive** trust store
   for that TLS context. It is not appended to any default roots.
2. If no per-connection CA is supplied and
   `CertificateManager.AddCaCertificateBundle(byte[])` was called, the supplied
   process-local bundle is the exclusive default for subsequently created contexts.
   It is cleared on runtime cleanup/soft reboot and is not persisted on disk.
3. Otherwise the runner's **`--ca-bundle <path>` option** selects a CA file,
   overriding `NANOCLR_CA_BUNDLE` for that process.
4. Without the option, **`NANOCLR_CA_BUNDLE`** selects a CA file. This also works
   when another application loads the native runtime library.
5. With neither option nor environment variable, the runtime loads the shipped
   **`share/nanoFramework/ca-bundle.pem`**, relative to the loaded native library,
   not the shell's working directory.

For example, configure a PEM CA bundle using the environment:

```bash
export NANOCLR_CA_BUNDLE=/absolute/path/to/company-ca.pem
```

Or add the option before the PE arguments in your launch command:

```bash
./build/posix/bin/nanoFramework.nanoCLR.test --ca-bundle /absolute/path/to/company-ca.pem \
    mscorlib.pe \
    nanoFramework.Runtime.Events.pe \
    <additional dependency PEs...> \
    MyApplication.pe
```

The option is currently Linux-only, like application networking. Relative custom
paths are resolved against the process's working directory. CA files may contain
a PEM bundle or one DER certificate, up to 1 MiB. A missing, empty, malformed or
unreadable selected file fails TLS explicitly: it never falls back to the shipped
bundle or host store. Per-request certificates and the managed global bundle
take precedence over file configuration. File changes affect newly created TLS
contexts; existing contexts retain their original trust.

To opt into a host-provided PEM bundle, explicitly set `NANOCLR_CA_BUNDLE` to its
path. `SSL_CERT_FILE` and `SSL_CERT_DIR` are not used. In a container, any configured
path must exist inside the container.

Explicit certificates accept a PEM certificate/bundle or one DER certificate.
Empty/malformed certificates and verification failures are errors: they never
fall back to default trust. Existing TLS contexts retain their original trust when
the process-wide default changes.

For example:

```csharp
using (HttpWebRequest request = (HttpWebRequest)WebRequest.Create("https://www.bing.com/"))
{
    request.SslProtocols = SslProtocols.Tls12;
    request.SslVerification = SslVerification.VerifyPeer;
    request.Timeout = 30000;
    request.ReadWriteTimeout = 30000;

    // Optional: the PEM contents of the CA certificate(s), not a file path.
    // Omit this assignment to use the configured or shipped default CA bundle.
    request.HttpsAuthentCert = new X509Certificate(caPem);

    using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
    {
        // Consume the response stream as usual.
    }
}
```

The certificate parameter above is a trust anchor, not a client identity or
certificate pin. Outgoing client certificates, mutual TLS, stored device
certificates, and revocation fetching/OCSP are not implemented. X509Certificate
issuer/subject/validity metadata and X509Certificate2 private keys are parsed
using mbedTLS.

TLS uses nonblocking send/receive callbacks and the existing one-shot socket event worker.
Handshakes and reads/writes yield to the CLR on either WANT_READ or WANT_WRITE,
including when a read needs to write TLS protocol traffic. Decrypted buffered
bytes are available without waiting for another socket event. Handshakes have
a 30-second deadline; I/O uses the existing managed stream timeout. Contexts
are limited to 16, are invalidated on cleanup, and are freed on managed disposal.
TLS errors are logged with mbedTLS diagnostics and surfaced as managed
cryptographic exceptions (HTTP may wrap these in `WebException`).

### HTTPS servers

`nanoFramework.WebServer` and `SslStream.AuthenticateAsServer` can serve HTTPS.
Supply an `X509Certificate2` containing the server certificate and its matching
private key. Certificate chains use PEM with the leaf first, followed by
intermediates; a single DER certificate is also accepted. Private keys accept
PEM or DER, including encrypted formats supported by mbedTLS, with the password
passed to `X509Certificate2`. Missing keys, malformed keys, incorrect passwords
and certificate/key mismatches fail explicitly.

For a local development certificate, run this Bash command on the host
(OpenSSL is only a certificate-generation tool here, not a runtime dependency):

```bash
openssl req -x509 -newkey rsa:2048 -sha256 -days 7 -nodes \
    -subj "/CN=localhost" \
    -addext "subjectAltName=DNS:localhost,IP:127.0.0.1" \
    -addext "basicConstraints=critical,CA:FALSE" \
    -addext "extendedKeyUsage=serverAuth" \
    -keyout server-key.pem -out server-cert.pem
```

Provide the generated PEM **contents**, not their file paths, to your application,
for example through private development resources. Managed filesystem access
is not implemented on this target. Do not commit private keys or publish them
with application packages.

```csharp
using System;
using System.Net.Security;
using System.Security.Cryptography.X509Certificates;
using System.Threading;
using nanoFramework.WebServer;

public static class HttpsHost
{
    public static void Run(string certificatePem, string privateKeyPem, string password = null)
    {
        var identity = new X509Certificate2(certificatePem, privateKeyPem, password);
        using (var server = new WebServer(8443, HttpProtocol.Https))
        {
            server.HttpsCert = identity;
            server.SslProtocols = SslProtocols.None;
            server.CommandReceived += (sender, e) =>
            {
                e.Context.Response.ContentType = "text/plain";
                WebServer.OutputAsStream(e.Context.Response, "Hello from nanoFramework HTTPS");
            };
            if (!server.Start())
            {
                throw new Exception("HTTPS server failed to start.");
            }
            Thread.Sleep(Timeout.Infinite);
        }
    }
}
```

Use compatible managed `nanoFramework.WebServer` and
`nanoFramework.System.Net.Http.Server` assemblies plus their dependencies.
The HTTP server package provides `System.Net.Http`; do not deploy the client-only
assembly in its place. All assemblies must match this branch's runtime/PE format.
WebServer sources targeting the older `SpanByte` API need porting to
`Span<byte>` when rebuilt against the 2.0 CoreLibrary.

From the same WSL distribution/network namespace, verify the server without
disabling certificate checks:

```bash
curl --cacert server-cert.pem https://localhost:8443/
```

The hostname/IP must match the server certificate's SAN, and clients must trust
its issuing CA (or the self-signed certificate for this local example). Port
8443 does not require root. Limit network exposure using the host firewall;
reachability from Windows or another machine depends on WSL networking.

The outgoing CA bundle setting does **not** supply a server identity or configure
which clients can connect. Servers do not request client certificates; mutual
TLS requests are rejected rather than silently downgraded. For direct SslStream
use, call `AuthenticateAsServer(identity, false, SslProtocols.None)`.
Client-side certificate verification remains mandatory.

### Default CA bundle provenance and updates

The shipped bundle is curl's Mozilla CA extraction dated **2026-10-20**:
[upstream bundle](https://curl.se/ca/cacert-2026-10-20.pem),
[source and license information](https://curl.se/docs/caextract.html).
Its SHA-256 is
`796f1389bfea13c0ed78f663b765baaff54a508e5483786bd6ece73c5e3e3cbe`.
It is distributed under MPL 2.0; its license and the mbedTLS license are copied
alongside the bundle by CMake. No local machine's trust store is copied into it.
This is a CA bundle, not a browser's complete trust-policy implementation.
Maintainers must refresh the bundle for CA additions/distrust changes and keep
the shared mbedTLS dependency pin current for security updates.

## Running Managed Applications

Pass the application's PE file and all matching dependency PEs directly to the
standalone runner, with dependencies before the application:

```bash
./build/posix/bin/nanoFramework.nanoCLR.test \
    mscorlib.pe \
    nanoFramework.Runtime.Events.pe \
    <additional dependency PEs...> \
    MyApplication.pe
```

Include only the dependencies your application references. For networking/HTTPS,
these may include `System.Net`, `System.Net.Http`, `System.IO.Streams`,
`nanoFramework.System.Text`, `nanoFramework.System.Collections` and
`System.Threading`, plus their dependencies. All PEs must match this runtime's
native contracts and assembly format.

## POSIX Shim Headers

`targets/posix/Include/` contains shim headers searched **before** the shared CLR/HAL/PAL paths. They shadow headers that would otherwise pull in Windows-only content (e.g. `nanoPAL.h`, `nanoHAL_Boot.h`) and provide no-op replacements for optional subsystems not present on a POSIX host.

The macro `PLATFORM_POSIX_HOST` is defined by the build system and gates POSIX-specific branches in the shared headers.

## Native Assembly Table

The interop table in `posix_stubs.cpp` registers the same portable set as the win32 virtual-device target. Hardware assemblies (GPIO, I2C, SPI, IO.Ports) are included with `NotImplementedStub()` bodies — they satisfy the PE checksum check at load time so applications referencing these assemblies can load, but calling their native methods throws `NotImplementedException`. Linux additionally registers the socket-backed `System.Net` assembly described above.

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
| `System.Net` | Linux only: native IPv4 sockets/DNS, read-only interfaces, TLS clients and HTTPS servers; other APIs remain unsupported |

Not included (require a network stack or platform filesystem not present on the host):
`nanoFramework.Networking.Sntp`, `System.IO.FileSystem`. `System.Net` is not included
on macOS.

## Known Limitations

- Linux networking is IPv4-only; TLS clients and HTTPS servers are supported with the limitations above.
- No application networking implementation on macOS.
- No managed filesystem (`System.IO.FileSystem`) — PAL filesystem calls are no-ops.
- `System.Device.Gpio/I2c/Spi` and `System.IO.Ports` native methods throw `NotImplementedException`.
- Wire Protocol transport is not connected (debugger attach not supported).
