# MCUboot Multi-Image Configuration and Wire Protocol Coexistence

This document describes the two-image MCUboot configuration used on all nanoFramework
MCUboot-enabled targets, and how the MCUboot IFU path coexists with the existing
Wire Protocol development workflow.

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

## IMAGE_TLV_DEPENDENCY — Deployment-to-CLR Version Checking

When a managed application deployment update (Image 1) is staged in `deploy_1`,
it can declare a minimum CLR version requirement using the MCUboot
`IMAGE_TLV_DEPENDENCY (0x40)` TLV entry. MCUboot validates this dependency at boot time
before allowing the Image 1 swap to proceed.

### How it works

1. When signing the deployment Image 1 package, the build pipeline includes a
   `--dependencies` argument to `imgtool sign`:
   ```
   imgtool sign \
       --key dev-signing-key.pem \
       --version <deploy_version> \
       --dependencies "(0, <min_clr_version>+)" \
       ...
   ```
2. This embeds a `IMAGE_TLV_DEPENDENCY` TLV that says: *"Image 0 must be at least
   version `<min_clr_version>`"*.
3. At boot, MCUboot checks the active Image 0 (CLR) version against the dependency
   before swapping in the new Image 1.
4. If the CLR version is too old, MCUboot rejects the Image 1 swap and boots the
   existing deployment. The managed application can read the rejection reason via SMP
   / mcumgr in Phase 1+.

### imgtool signing example

```bash
# Sign Image 1 (deployment) with a CLR version dependency
imgtool sign \
    --key dev-signing-key.pem \
    --align 4 \
    --version 1.2.0.0 \
    --header-size 0x200 \
    --pad-header \
    --slot-size <deploy_slot_size_bytes> \
    --dependencies "(0,1.5.0.0+)" \
    deployment.bin \
    deployment-signed.bin
```

The `(0,1.5.0.0+)` argument declares: *Image 0 (nanoCLR) must be version 1.5.0.0 or
higher*.

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
│      - No MCUboot image header                                            │
│      - No signature required                                              │
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
