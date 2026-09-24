# Update Sessions

How an upgrade image is staged into a secondary slot while nanoCLR runs.

Code: `MCUboot/include/MCUboot_UpdateSession.h`, `MCUboot/common/MCUboot_UpdateSession.c`
(plain C, compiled into the nanoCLR-side `nf_mcuboot_port` library). Consumers:
`src/nanoFramework.Runtime.InFieldUpdate/` (managed API) and `src/CLR/Debugger/Debugger.cpp`
(Wire Protocol `Monitor_ImageWrite` / `Monitor_ImageErase`).

## Design goals

1. One writer per image at a time. Managed code, the Wire Protocol and native code all go
   through the same registry, so chunks from two writers can never interleave.
2. Sequential writes with no caller bookkeeping. The registry owns the write position.
3. Resume after any reboot without a persistent record. The secondary slot itself is the
   state.
4. Verify the staged image on the device before scheduling the swap.
5. One definition of where the image lives inside a secondary slot, shared with the
   bootloader's media importer.

## Registry

`MCUboot_UpdateSession.c` keeps one `Ifu_Session` per image (`MCUBOOT_IMAGE_NUMBER`):

| Field | Meaning |
|---|---|
| `Open`, `Owner` | slot claimed, and by whom (`IFU_OWNER_MANAGED`, `_WIRE`, `_NATIVE`) |
| `Token` | opaque value returned on open; every later call must present it |
| `ImageOffset`, `UsableSize` | slot geometry from `Ifu_ImageOffset()` / `Ifu_UsableSize()` |
| `TotalLength` | image length declared by the caller (header + payload + TLVs) |
| `NextOffset` | image-relative position of the next chunk |
| `HeaderKnown`, `Header` | MCUboot header once seen (first chunk, or resume) |
| `WriteInFlight` | a flash operation is executing outside the lock |

The token is not a secret. It exists so that a stale handle (after complete/abort) or a
handle belonging to a different session is rejected with `IFU_E_BAD_TOKEN`. It is built
from a counter and a caller-supplied seed (the system tick), never zero.

### Lifecycle

```
                 Ifu_SessionStart ─────────────┐
   (closed) ──── Ifu_SessionResume ────────────┼──► (open) ──── Ifu_SessionWrite ──┐
      ▲                                        │      ▲                             │
      │                                        │      └─────────────────────────────┘
      │      Ifu_SessionComplete == IFU_OK ────┤
      │      Ifu_SessionComplete (any failure  │      Ifu_SessionComplete == IFU_E_INCOMPLETE
      │        other than INCOMPLETE)          │        keeps the session open
      │      Ifu_SessionAbort(eraseSlot)       │
      └──────  Ifu_SessionReleaseAll  ─────────┘
```

Rules enforced by the registry:

- `Start`/`Resume` fail with `IFU_E_BUSY` when another owner holds the image, when a write is
  in flight, or while `Ifu_EraseSecondary()` is erasing the slot. The **same owner** reopening
  replaces its own session, so an abandoned handle cannot wedge the image.
- `Start`/`Resume` fail with `IFU_E_SWAP_IN_FLIGHT` when `boot_swap_type_multi(image)` is not
  `BOOT_SWAP_TYPE_NONE`, and with `IFU_E_TOO_LARGE` when `totalLength > Ifu_UsableSize()`.
- `Start` erases the whole slot (trailer included, so no stale pending marker survives).
- `Write` appends at `NextOffset`. The first chunk must carry a full `struct image_header`
  with `IMAGE_MAGIC` and `hdr_size + img_size + protect_tlv_size <= totalLength`, otherwise
  `IFU_E_BAD_MAGIC`. Writing past `TotalLength` is `IFU_E_TOO_LARGE`. On failure the position
  does not advance.
- `Abort(eraseSlot = false)` is a pause: the partial image stays and `Resume` picks it up.
- `Ifu_EraseSecondary()` is refused with `IFU_E_BUSY` while any session is open. It is the
  only way to erase a slot outside a session (managed `EraseSecondaryImage`, Wire Protocol
  `Monitor_ImageErase`).

## Image offset inside the secondary slot

`Ifu_ImageOffset(fa)` in `MCUboot_ImageSlotInfo.c` is the single source of truth:

| Swap mode | Image starts at |
|---|---|
| `MCUBOOT_SWAP_USING_OFFSET` | `MCUBOOT_LOGICAL_SECTOR_SIZE` (or the size of sector 0 when no logical sector is configured) |
| swap-using-move / overwrite-only | 0 |

`Ifu_UsableSize(fa) = fa_size - 2 * Ifu_ImageOffset(fa)`: one sector is skipped at the start,
and the primary reserves one for the swap trailer. `Ifu_ReadSlotInfo()` reads secondary-slot
headers at this offset; the bootloader media importer writes there; the session registry
writes there. Primary slots always hold the image at offset 0.

## Resume without a persistent record

`Ifu_SessionResume(image, owner, totalLength, expectedHeader, expectedHeaderLen, seed, out)`:

1. Read the `struct image_header` at `ImageOffset`. No `IMAGE_MAGIC` → `IFU_E_NO_IMAGE`.
2. If `expectedHeader` is given, byte-compare up to 32 bytes → `IFU_E_HEADER_MISMATCH`.
3. `span = hdr_size + img_size + protect_tlv_size` must not exceed `totalLength`. If the
   unprotected TLV info at `span` is already stored and parses, `span + it_tlv_tot` must not
   exceed `totalLength` either → `IFU_E_HEADER_MISMATCH`.
4. High-water mark: scan backwards from `totalLength` in 256-byte reads for the last byte
   that is not `flash_area_erased_val(fa)`.
5. Rewind: `flash_area_get_sector()` for the block holding the high-water mark. If that block
   also holds the header → `IFU_E_NO_IMAGE` (caller starts fresh). Otherwise the block is
   erased and `NextOffset = block start - ImageOffset`. If the mark equals `totalLength`
   nothing is erased and the caller can go straight to `Complete`.

Why this is sound:

- **Trailing erased-value bytes.** Payload bytes equal to 0xFF at the tail make the mark
  under-estimate. Harmless: the caller re-sends bytes that are already stored, and
  programming a NOR byte with the erased value is a no-op.
- **Partially programmed pages.** A page interrupted mid-program cannot be repaired by
  rewriting (bits only clear), so the block containing the mark is always erased. This also
  satisfies ECC / write-once internal flash: the resumed range is freshly erased.
- **Swap trailer.** The scan is bounded by `totalLength <= UsableSize`, so it never reaches
  the trailer area at the end of the slot.
- **Previous image after a swap.** After a completed swap the secondary slot holds the old
  primary image with a valid header at the same offset. Only the caller can tell that from a
  paused download, which is what `expectedHeader` is for (the managed API documents a 32-byte
  HTTP range request `bytes=0-31` as the cheap way to get it).

Cost: no extra flash wear, no configuration record, works identically for every swap mode.

## Completion

`Ifu_SessionComplete(image, token, out)`:

1. `NextOffset == TotalLength`, else `IFU_E_INCOMPLETE` (session stays open).
2. Re-read the header; locate the unprotected TLV info at `span`; require
   `IMAGE_TLV_INFO_MAGIC` and `span + it_tlv_tot <= TotalLength`; walk the TLVs for
   `IMAGE_TLV_SHA256` → `IFU_E_BAD_TLV` on any structural problem.
3. SHA-256 (tinycrypt, already part of the MCUboot tree) over `[0, span)` — exactly what
   `imgtool` hashes — compared with the TLV digest → `IFU_E_HASH_MISMATCH`.
4. `boot_set_pending_multi(image, 0)`: a revertible test swap on the next reboot. Failure is
   reported as `IFU_E_FLASH`.

The signature is still verified by MCUboot at boot. On a verification failure the slot is
left as is; the caller decides between `Abort(erase)` and a fresh `Start`.

Application flow for the deployment image:
`Complete == Success` → `RequestReboot()` → MCUboot swaps → new application validates itself →
`ConfirmDeploymentImage()`; without confirmation MCUboot reverts on the following reboot.

## Concurrency

Every registry mutation runs between `Ifu_SessionLock()` / `Ifu_SessionUnlock()`. Flash I/O
runs outside the lock and is serialised by the token plus `WriteInFlight`; any other registry
call on that image gets `IFU_E_BUSY` while a flash operation executes.

The lock hooks are weak no-ops in `MCUboot_UpdateSession.c`. `targets/ChibiOS/_nanoCLR/targetHAL.cpp`
overrides them with `chSysLock()` / `chSysUnlock()` because Wire Protocol handlers run on
the receiver thread, concurrently with the CLR. Other RTOS ports add their own override when
their MCUboot port is completed.

Soft reboot (CLR-only restart, e.g. a Visual Studio deploy) registers `Ifu_SessionReleaseAll()`
through `HAL_AddSoftRebootHandler()` the first time a managed session is opened. The registry
is RAM-only; flash is untouched, so a paused download survives and can be resumed.

## Wire Protocol mapping

The wire format is unchanged; the device keeps the session on the host's behalf
(`s_wireSessionToken[image]` in `Debugger.cpp`).

| Command | Behaviour |
|---|---|
| `Monitor_ImageWrite`, secondary slot, `Offset == 0` | `Ifu_SessionStart(image, IFU_OWNER_WIRE, TotalSize)` |
| `Monitor_ImageWrite`, secondary slot, any chunk | `Offset` must equal the session's `NextOffset` (`Monitor_Image_Error_BadOffset`, reply carries the expected offset), then `Ifu_SessionWrite` |
| `Monitor_ImageWrite`, last chunk (`NextOffset >= TotalSize`) | `Ifu_SessionComplete` (verifies and marks pending) |
| `Monitor_ImageWrite`, primary slot | unchanged developer path: direct write at offset 0, no session |
| `Monitor_ImageErase`, secondary slot | `Ifu_EraseSecondary` |

Status mapping (`Ifu_StatusToWireError`): `BUSY`/`BAD_TOKEN` → `Busy` (9), `BAD_OFFSET` →
`BadOffset` (10), verification failures → `BadImage` (11), `TOO_LARGE` → `TooLarge`,
`BAD_MAGIC` → `BadMagic`, `SWAP_IN_FLIGHT` → `SetPending`, `FLASH` → `Write`.
`nf-debugger`'s `Engine.WriteImage` already sends `TotalSize` on every chunk and follows
`NextOffset`, so it works unchanged.

## Managed API mapping

`nanoFramework.Runtime.InFieldUpdate` (native version 100.0.0.2):

| Managed | Native |
|---|---|
| `StartUpdateSession(image, totalLength)` → `UpdateSession` or `null` | `Ifu_SessionStart(…, IFU_OWNER_MANAGED, …)` |
| `ResumeUpdateSession(image, totalLength, expectedHeader)` | `Ifu_SessionResume` |
| `StoreImageChunk(session, data, offset, count)` (`Stream.Write` convention) | `Ifu_SessionWrite(data + offset, count)`; `NextOffset` written back into the object |
| `CompleteUpdateSession(session)` → `UpdateSessionResult` | `Ifu_SessionComplete` |
| `AbortUpdateSession(session, eraseSlot)` | `Ifu_SessionAbort` |
| `EraseSecondaryImage(image)` | `Ifu_EraseSecondary` |
| `GetUpdateSessionOwner(image)` | `Ifu_SessionOwner` |
| `GetLastSessionError()` | last `Ifu_Status` recorded by any of the above |

`UpdateSessionResult` mirrors `Ifu_Status` one to one; `UpdateSessionOwner` mirrors the
`IFU_OWNER_*` codes. Keep them in sync when adding values.

## Media import

The bootloader importer (`MCUboot_media_import.c`) runs before nanoCLR, so it cannot collide
with a run-time session and does not use the registry. It shares `Ifu_ImageOffset()` /
`Ifu_UsableSize()` so that images staged from SD/USB and images staged at run time land in
the same place and are reported identically by `Ifu_ReadSlotInfo()`.

## Test matrix

Covered by the unit tests in the class library repository (hardware only, MCUboot target):
session lifecycle and ownership, `Busy`/`BadToken`/`TooLarge`, `Stream.Write` argument
semantics, first-chunk header validation, sequential position, pause/resume including a
pause inside a run of 0xFF payload bytes, `HeaderMismatch`/`NoImage`, `Incomplete`/`BadTlv`/
`HashMismatch`/`Success` with the pending flag, cross-thread ownership.

Not covered automatically: reboot mid-session (manual: reset the board between chunks, then
`Resume`), Wire Protocol collision (manual: `nanoff` write while a managed session is open →
error 9), media-import interplay, `SwapInFlight`, flash failures.
