# MCUboot Multi-Image Management

This document describes the two-image MCUboot configuration used on all nanoFramework
MCUboot-enabled targets: how the two images are laid out and signed, why
`IMAGE_TLV_DEPENDENCY` is not usable here, the `MCUBOOT_VALIDATE_PRIMARY_SLOT` policy for
Image 1, and how the MCUboot IFU path coexists with the existing Wire Protocol development
workflow.

---

## Multi-Image Configuration

### IMAGE_NUMBER = 2

All MCUboot-enabled nanoFramework targets use `MCUBOOT_IMAGE_NUMBER = 2`. This enables
MCUboot's independent multi-image management:

| Image | Slot IDs | Content | Primary Slot Location | Secondary Slot Location |
|---|---|---|---|---|
| **Image 0** | `FLASH_AREA_IMAGE_0_PRIMARY`, `FLASH_AREA_IMAGE_0_SECONDARY` | nanoCLR firmware binary | Internal MCU flash | Same device (ESP32) or external storage (STM32) |
| **Image 1** | `FLASH_AREA_IMAGE_1_PRIMARY`, `FLASH_AREA_IMAGE_1_SECONDARY` | Managed application deployment assemblies | Internal MCU flash (`deploy_0`) | Same device (ESP32) or external storage (STM32) (`deploy_1`) |

Both images can be independently updated and independently rolled back. An update to
Image 0 (CLR) does not require a simultaneous update to Image 1 (deployment), and
vice versa.

### Flash Area IDs

The slot IDs are defined in `MCUboot/include/sysflash/sysflash.h` following the MCUboot
upstream convention:

```c
#define FLASH_AREA_BOOTLOADER           0   /* MCUboot itself */
#define FLASH_AREA_IMAGE_0_PRIMARY      1   /* Image 0 primary  (nanoCLR active) */
#define FLASH_AREA_IMAGE_0_SECONDARY    2   /* Image 0 secondary (nanoCLR upgrade) */
#define FLASH_AREA_IMAGE_1_PRIMARY      3   /* Image 1 primary  (deployment active) */
#define FLASH_AREA_IMAGE_1_SECONDARY    4   /* Image 1 secondary (deployment upgrade) */
```

The concrete flash addresses for each slot are defined per-target in `flash_map_stm32.c`
or `flash_map_esp32.c`. See the per-target `MCUboot-flash-layout.md` files for exact
addresses and sizes.

---

## Two-Pass Boot Sequence

`main()` in `targets/ChibiOS/_mcuboot/mcuboot_main.c` runs MCUboot's own
image-validation-and-upgrade logic twice, once per image, instead of calling the
single-pass `boot_go()` most MCUboot integrations use.

### Why two passes

Image 1 (deployment) holds data the CLR reads, not firmware MCUboot can boot. A
never-provisioned `deploy_0` must not be able to veto booting a valid nanoCLR, so each
image needs its own pass with its own independent pass/fail outcome:

* **Pass 1, image 1** — the update loop runs before the bootable-image check, so a staged
  deployment in `deploy_1` is swapped into `deploy_0` even though the pass then reports
  failure (there being no bootable code in that slot). The result of this pass is
  deliberately discarded for that reason.
* **Pass 2, image 0** — the actual boot decision. `deploy_0` is masked out of this pass, so
  a never-provisioned or otherwise invalid deployment slot cannot veto a valid nanoCLR.

### `boot_go_for_image_id()` and the image mask

Both passes call `boot_go_for_image_id()`, defined in upstream mcuboot's `loader.c`
(`boot/bootutil/src/loader.c`, pulled in by `MCUboot/CMakeLists.txt` via CMake
`FetchContent` - not a file in this repo) — MCUboot's own public entry point for
validating and booting a single image by index, not a nanoFramework patch. It sets
MCUboot's internal image mask to that one image, and every multi-image loop in `loader.c`
honours the mask, so each pass considers only the image it was called for.

### Explicit `boot_state_init()`, no matching `boot_state_clear()`

Each pass calls `boot_state_init(bootState)` immediately before its
`boot_go_for_image_id()` call. Unlike `boot_go()`, `boot_go_for_image_id()` does not
initialise the loader state itself, and pass 2 must not inherit pass 1's image headers,
swap types or secondary-slot offsets — so both passes init explicitly. `boot_go()`'s
matching `boot_state_clear()` call is not needed here: its body is entirely compiled under
`MCUBOOT_ENC_IMAGES`, which nanoFramework does not define.

### Consequence for signing

Because image 0 is masked out during pass 1, its header is never read during that pass.
See [Why it does not work here](#why-it-does-not-work-here) for what that means for
`IMAGE_TLV_DEPENDENCY`.

---

## IMAGE_TLV_DEPENDENCY — Deployment-to-CLR Version Checking

> **Status: not supported. Do not sign deployment images with `--dependencies`.**
> Doing so does not merely fail the dependency — it cancels the swap of *every* image,
> nanoCLR included, leaving the device on its existing firmware and deployment.

MCUboot lets an image declare a minimum version of another image via the
`IMAGE_TLV_DEPENDENCY (0x40)` TLV, embedded by passing `--dependencies` to
`imgtool sign`. The obvious use would be for a deployment package (Image 1) to declare
the minimum nanoCLR version its assemblies need, so MCUboot refuses to swap it in against
an older CLR.

### Why it does not work here

`mcuboot_main.c` runs MCUboot as two `boot_go_for_image_id()` passes — see
[Two-Pass Boot Sequence](#two-pass-boot-sequence) for the full mechanics. Image 0 is
masked during the deployment pass, so its header is never read during that pass. A
dependency naming Image 0 then compares against `0.0.0.0`, fails, and
`boot_verify_dependencies()` responds by forcing `BOOT_SWAP_TYPE` to `NONE` for **all**
images — so a pending nanoCLR update is silently discarded too.

Enforcing a minimum CLR version for a deployment therefore has to happen above MCUboot,
in the managed application or the deployment tooling, not in the image trailer.

### Who signs what

nanoFramework's own build signs only two things, in `CMake/binutils.common.cmake`: the
nanoCLR image, and an empty deployment placeholder for `deploy_0`. Neither passes
`--dependencies`.

Real deployment images are **not** produced by this repository. A developer signs their
compiled C# application externally with `imgtool`, choosing the key, version and slot
size for their target. The constraint above binds that external step: whatever else those
invocations carry, `--dependencies` must not be among it.

### Related: version comparison

MCUboot compares `major.minor.revision` only, unless `MCUBOOT_VERSION_CMP_USE_BUILD_NUMBER`
is defined — see `boot_compare_version()` in `bootutil_loader.c`. nanoFramework versions are
4-component, so `mcuboot_config.h` defines it unconditionally for all targets.

### Signing a deployment image

The supported form — no `--dependencies`:

```bash
imgtool sign \
    --key dev-signing-key.pem \
    --align 4 \
    --version 1.2.0+0 \
    --header-size 0x400 \
    --pad-header \
    --slot-size <deploy_slot_size_bytes> \
    deployment.bin \
    deployment-signed.bin
```

Two things to get right:

* `--header-size` must match `CONFIG_NF_MCUBOOT_HEADER_SIZE` for the target — `0x400` on
  ORGPAL_PALTHREE, not the `0x200` Kconfig default.
* `--slot-size` is **image 1's usable size, not image 0's, and not either slot's physical
  size.** See below for where to find it.

Versions are `maj.min.rev[+build]` (`imgtool/version.py`); a four-dot `1.2.0.0` is a parse
error, so a nanoFramework 4-component version is written `1.2.0+0`.

### Finding the right `--slot-size`

`--slot-size` is the only size check `imgtool sign` performs at sign time. Get it wrong and
that check doesn't fail loudly — the image signs successfully against the wrong ceiling,
and an oversized image only fails on the device, at swap time.

Every target's `MCUboot-flash-layout.md` has a "Usable image size" table with an
`imgtool --slot-size` column - one row per image. Always use the **Image 1 (deploy)** row
and the **usable** value, never image 0's row and never either slot's physical size:
MCUboot reserves one whole logical sector for the swap trailer, so usable is always one
logical sector less than physical. Image 0's value is applied automatically by the build
(`NF_MCUBOOT_SLOT_SIZE`); image 1's is not - nothing but this table catches a wrong value
before the device does.

---

## Wire Protocol Coexistence

The Wire Protocol is nanoFramework's proprietary debug and deployment protocol, used
by `nanoff` and the Visual Studio extension. It writes raw managed assemblies directly
to the deployment flash region (`deploy_0` — Image 1 primary slot).

### Two update paths

```
┌──────────────────────────────────────────────────────────────────────┐
│  Wire Protocol (nanoff / VS extension)                                    │
│  └── writes raw managed assemblies directly to deploy_0               │
│      (Image 1 primary slot)                                               │
│      - MCUboot header + SHA256 TLV footer synthesised by the debugger      │
│      - Unsigned - no ECDSA signature                                      │
│      - Device boots next time with new assemblies already in deploy_0     │
│      - Unchanged developer workflow                                       │
└──────────────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────────────────────┐
│  MCUboot IFU path (In-Field Update)                                      │
│  └── stages signed Image 1 package in deploy_1                          │
│      (Image 1 secondary slot)                                             │
│      - Full MCUboot image: header + assemblies + TLVs + signature         │
│      - Optional IMAGE_TLV_DEPENDENCY for CLR version check                │
│      - MCUboot validates and swaps deploy_1 → deploy_0 on next boot      │
│      - Rollback available (swap-using-move / swap-using-offset)           │
└──────────────────────────────────────────────────────────────────────┘
```

### Key design principle

The Wire Protocol writes **raw binary data** with no MCUboot image header. MCUboot does
not intercept or validate Wire Protocol writes — they go directly to `deploy_0` via the
nanoFramework block storage driver. This preserves the existing development workflow
unchanged.

MCUboot's Image 1 management only activates when a valid MCUboot image is staged in
`deploy_1` (the secondary slot). The presence of a valid MCUboot image trailer in
`deploy_1` triggers the swap on the next boot.

---

## MCUBOOT_VALIDATE_PRIMARY_SLOT for Image 1

The `MCUBOOT_VALIDATE_PRIMARY_SLOT` setting controls whether MCUboot re-validates the
primary slot image signature on **every boot** (not just after a swap). For Image 1
(deployment), this setting must be build-type dependent because of the Wire Protocol
coexistence requirement.

### Build-type policy

| Build type | `MCUBOOT_VALIDATE_PRIMARY_SLOT` (Image 1) | Effect |
|---|---|---|
| **Debug / dev** (`NF_BUILD_RTM=OFF`) | **Disabled** | MCUboot boots `deploy_0` without validating its contents. Wire Protocol can write raw assemblies to `deploy_0` and the device boots normally on the next reset. |
| **RTM** (`NF_BUILD_RTM=ON`) | **Enabled** | MCUboot re-validates the `deploy_0` signature on every boot. All assembly updates must arrive via a signed MCUboot Image 1 IFU package through the IFU path. Wire Protocol is disabled in RTM builds anyway. |

### Configuration mapping

In `MCUboot/include/mcuboot_config/mcuboot_config.h`:

```c
// MCUBOOT_VALIDATE_PRIMARY_SLOT is enabled only in RTM builds.
// In debug builds, Wire Protocol writes raw assemblies to deploy_0 (Image 1 primary)
// without a MCUboot header; primary slot validation would reject those writes.
#if defined(CONFIG_NF_BUILD_RTM)
#define MCUBOOT_VALIDATE_PRIMARY_SLOT   1
#else
#define MCUBOOT_VALIDATE_PRIMARY_SLOT   0
#endif
```

### Wire Protocol is NOT affected by Image 0 validation

`MCUBOOT_VALIDATE_PRIMARY_SLOT` for Image 0 (nanoCLR firmware) is always enabled.
Wire Protocol never writes to the nanoCLR slot — it only writes to `deploy_0` (Image 1).
Therefore, enabling Image 0 primary slot validation does not affect the Wire Protocol
workflow.

---

## Signing Requirement Summary

| Update path | Image signed? | Signature required by MCUboot? | Who performs signing |
|---|---|---|---|
| Wire Protocol → `deploy_0` | No | No (validation disabled in Debug builds) | N/A |
| MCUboot IFU → `deploy_1` → `deploy_0` | **Yes** | **Yes** | Build pipeline (`imgtool sign`) |
| MCUboot IFU → Image 1 secondary → primary | **Yes** | **Yes** | Build pipeline (`imgtool sign`) |
| nanoff firmware flash → Image 0 primary | **Yes** | **Yes** | CI build (`imgtool sign` post-build step) |

The signing requirement applies **only to the IFU staging path** (`deploy_1` / Image 0
secondary). Direct Wire Protocol writes to `deploy_0` are **never** required to be signed
and do not go through the MCUboot upgrade path.

---

## Deployment Image (Image 1) Signing in the Build Pipeline

Image 1 (deployment package) signing is a **Phase 1+** deliverable. The IFU client
(nanoff or a managed app) is responsible for signing the deployment package before
staging it in `deploy_1`.

For Image 0 (nanoCLR), the `imgtool sign` post-build step is already integrated into
the CMake build system (`CMake/binutils.common.cmake`), gated by
`NF_FEATURE_HAS_MCUBOOT`. See `MCUboot/keys/README.md` for signing commands.
