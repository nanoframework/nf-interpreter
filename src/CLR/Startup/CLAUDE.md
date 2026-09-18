# CLR startup — deployment assembly loader

Companion notes for `CLRStartup.cpp`. Rationale lives here; the source stays terse.

## Deployment region crawl (`LoadDeploymentAssemblies` / `ContiguousBlockAssemblies`)

The deployment region holds nanoFramework PE records (`CLR_RECORD_ASSEMBLY`, marker `NFMRK2`)
concatenated back to back, each rounded up to a 4-byte boundary. The loader walks them in order,
loading each one, until it reaches the end of the region.

### Classification is centralized in `CLR_RECORD_ASSEMBLY::CheckHeader()`

`CheckHeader()` (in `TypeSystem.cpp`) is the single place that decides what a candidate record is.
It is shared by the on-device crawler here and by the virtual device
(`targets/netcore/.../CLRStartup.cpp`), so both agree on the rules. It returns a `HeaderStatus`:

- `Valid` — `NFMRK2` marker, header CRC good, string table version current.
- `Erased` — marker all `0xFF` (erased NOR) or all `0x00` (zero-filled media, e.g. the virtual
  device's RAM-backed transient flash). This is the **normal end** of the deployment region.
- `UnsupportedVersion` — a recognized `NFMRK` prefix but not V2 (e.g. the retired `NFMRK1`).
- `NotAnAssembly` — no recognized marker where a record was expected.
- `BadHeaderCrc` / `BadStringTableVersion` — a V2 marker that fails those checks.

Marker classification happens **before** any CRC so an erased record is never CRC'd, and a
foreign/old record is reported for what it is instead of as a generic CRC failure.

### Fail-fast policy

A deployment bundle is one unit produced by one tool run. Anything that is not a loadable `Valid`
record — old/foreign marker, bad CRC, structurally invalid size, native-checksum mismatch, or a
`LoadAssembly` failure — aborts the whole load with a specific HRESULT
(`CLR_E_ASSM_UNSUPPORTED_FORMAT` or `CLR_E_ASSM_WRONG_CHECKSUM`), reported up through `Load()` →
`ClrStartup`. Records are **never skipped**. This is deliberate:

- Skipping a record and loading the rest yields a partially-resolved type system whose only visible
  symptom is a downstream `CLR_E_ENTRYPOINT_NOT_FOUND` / `CLR_E_TYPE_UNAVAILABLE` — which is exactly
  the silent, misleading failure this design removes.
- With no skipping there is no resync, so one bad record can never desync the crawl.
- On MCUboot A/B targets the deployment image is confirmed only by managed code
  (`UpdateManager.ConfirmDeploymentImage`); native startup never confirms image 1. Fail-fast means a
  bad test-swapped deployment never runs and therefore never confirms, so MCUboot reverts it on the
  next boot. Skip-and-continue could let a partial app run and confirm a broken bundle.

On a failed load control falls through to the debugger loop and each failure is printed with its flash offset and HRESULT.

### Why an old (`NFMRK1`) record cannot be sized to skip past

Even if we wanted to skip a foreign record, its size cannot be trusted through the V2 struct. The V1
layout has an extra field after `nativeMethodsChecksum` and `TBL_Max = 16` (V2: 17), so
`startOfTables` begins at a different offset and V2's `TotalSize()` (`startOfTables[TBL_EndOfAssembly]`,
index 17) reads V1's `numOfPatchedMethods`, which is `0`. Advancing by `ROUNDTOMULTIPLE(0,4)` would
loop forever. Hence: classify, and fail-fast — never advance past a record that is not `Valid`.

### Crawl bound (MCUboot)

On MCUboot targets `LoadDeploymentAssemblies` reads the `image_header` at the start of the slot and
bounds the crawl to `ih_hdr_size + ih_img_size`, then seeks past `ih_hdr_size` to the first record.
This means the TLV trailer that follows the signed payload is never examined and can't be mistaken
for a record, and an erased slot (`ih_magic == 0xFFFFFFFF`) is a clean "no deployment". A bounds
check on each record's rounded size also prevents `GoodAssembly()` from CRC'ing past the end of an
XIP/memory-mapped slot (`BlockStorageStream_Read` would otherwise silently clamp the read).
