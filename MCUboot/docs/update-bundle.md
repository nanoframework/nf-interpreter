# Update Bundle (`nanoUpdate-*.bin`)

A single file that carries both signed MCUboot images: a nanoCLR (image 0) and
the deployment built for it (image 1). The bootloader imports it from an SD card or a USB
mass storage device, exactly like the loose `nanoCLR-*.bin` / `nanoDeployment-*.bin` files
(see `MCUboot/include/mcuboot_media_import.h`), but as one unit.

Use a bundle when a CLR and a deployment must go together, e.g. when the native contract
changed. With loose files nothing prevents a medium from carrying one half of the pair, or
halves from two different builds.

This repository only consumes bundles. Producing them is left to external tooling (nanoff,
build pipelines), following the format below.

## File format

All fields are little-endian. The manifest starts at file offset 0.

```
offset  size  field
0       8     marker          ASCII "NFUPDATE", no terminator
8       2     format_version  1
10      2     header_size     bytes from offset 0 to the end of the entry table
12      1     entry_count     1..MCUBOOT_IMAGE_NUMBER (2)
13      3     reserved        0
16      12    entry[0]
28      12    entry[1]        (when entry_count is 2)
...

entry:
0       1     image           MCUboot image index (0 = nanoCLR, 1 = deployment)
1       3     reserved        0
4       4     offset          file offset of the signed image, multiple of 4
8       4     size            size of the signed image (header + payload + TLVs)
```

The C definitions are `mcuboot_bundle_header` / `mcuboot_bundle_entry` in
`MCUboot/include/mcuboot_media_import.h`.

Each entry is a complete `imgtool sign` output, byte for byte what would otherwise be a
loose `nanoCLR-*.bin` / `nanoDeployment-*.bin` file. Sign each image for its own slot (see
"Finding the right `--slot-size`" in [multi-image-management.md](multi-image-management.md)).
Bytes between the entry table and the first image, and between images, are ignored (padding).

Example layout with both images:

```
0x00000  manifest  "NFUPDATE" 1 0x28 2 | {0, 0x00028, 0x5F3A4} | {1, 0x5F3CC, 0x1B210}
0x00028  image 0   signed nanoCLR      (0x5F3A4 bytes)
0x5F3CC  image 1   signed deployment   (0x1B210 bytes)
```

### Validation

The bootloader ignores a bundle as a whole, and logs why, when:

- the marker or the format version doesn't match,
- `entry_count` is 0 or larger than `MCUBOOT_IMAGE_NUMBER`,
- `header_size` is smaller than the manifest plus its entries, or past the end of the file,
- an entry has an unknown or duplicate image index,
- an entry is empty, misaligned, starts inside the manifest, reaches past the end of the
  file, or overlaps another entry,
- any entry fails the checks applied to a loose file: valid MCUboot header, a SHA-256
  TLV, and a size within the usable size of that image's secondary slot.

The manifest has no checksum or signature of its own. It only says where the images are:
each image is authenticated by its own SHA-256 TLV (checked again against flash after the
copy) and by MCUboot's signature check at swap time, as for any other staged image.

## Import behaviour

- **Precedence:** on each medium a valid bundle is looked for first. The images it carries
  are never taken from loose files on that medium; images it doesn't carry still are.
  When several bundles are present, the one with the highest versions wins (image 0 version
  first, then image 1).
- **Per image:** every entry goes through the same decision as a loose file (interrupted
  swap, identical image already in the primary slot, used marker, revert or pending state).
  An image that is already installed is simply not staged again, so a bundle whose CLR is
  current only updates the deployment.
- **As a unit:**
  - if any entry has to be deferred (e.g. MCUboot must finish an interrupted swap first),
    the whole bundle is deferred to a later boot,
  - every image to stage is copied and hash checked before any of them is marked pending.
    If one copy fails, every secondary slot touched is erased and nothing is staged,
  - all images are marked pending only after all of them landed intact, the deployment first: if
    power fails in between, only the deployment is pending, which rolls back on its own (and the
    next boot imports the rest of the bundle, as it is not marked used yet).
- **Used marker:** `<bundle name>.used`, with one stamp line per entry and per device, the
  same as for loose files. The file is never deleted, so one medium can update many devices.
- If a bundle carries an image that was already staged from an earlier medium in the media
  table, the bundle is ignored on this medium (loose files there are still considered).

## After the swap: kept or reverted as a pair

MCUboot updates and reverts each image independently. A bundle installs two images in one
boot, and the following keeps them together (see also [confirmation-timeout.md](confirmation-timeout.md)
and [multi-image-management.md](multi-image-management.md)):

- **Paired confirmation.** When both images were test-swapped in the same boot, the CLR doesn't
  confirm image 0 at startup. Confirming the deployment (`UpdateManager.ConfirmDeploymentImage()`,
  or `Monitor_ImageConfirm` for image 1) confirms image 0 right after it. A deployment that
  fails to load, has no entry point, or isn't confirmed before the confirmation timeout or the
  next reset is reverted together with its CLR.
- **Pass order.** The bootloader runs the nanoCLR pass before the deployment pass. A reset
  during the deployment swap reverts the (unconfirmed) CLR and resumes the deployment swap.
  The old CLR can't load the new, unconfirmed deployment, so it resets and MCUboot reverts
  that too. The device ends up on its previous pair.

In both cases the bundle stays marked as used. A pair reverted this way is not imported again
from the same medium; remove the `.used` file to retry.
