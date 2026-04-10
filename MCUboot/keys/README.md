# MCUboot Key Management

This directory contains signing keys for the nanoFramework MCUboot integration.

> ⚠️ **The development key in this directory (`dev-signing-key.pem`) is committed to
> the public repository and is NOT SECURE.  It must never be used for production
> firmware releases.**

---

## Key Files

| File | Purpose | Committed? |
|------|---------|-----------|
| `dev-signing-key.pem` | ECDSA P-256 private key — development/CI builds only | Yes (intentional) |
| `root-ec256.pem` | ECDSA P-256 public key — embedded in the MCUboot binary | Yes |
| `prod-signing-key.pem` | Production private key — **must never be committed** | **No** |

---

## Generating a New Key Pair

Use `imgtool` from the MCUboot project (install via `pip install imgtool`):

```bash
# Generate a new ECDSA P-256 key pair
imgtool keygen --key dev-signing-key.pem --type ecdsa-p256

# Extract the public key as a C source file for embedding in MCUboot
imgtool getpub --key dev-signing-key.pem --lang c > signing_pub_key.c
```

The `signing_pub_key.c` output contains:

```c
const unsigned char ecdsa_pub_key[] = { /* ... */ };
const unsigned int ecdsa_pub_key_len = /* ... */;
```

This file is compiled into the MCUboot bootloader binary.  Once MCUboot is flashed,
the public key is **immutable** — only firmware signed with the corresponding private key
will be accepted.

---

## Signing a Firmware Image

After building nanoCLR, sign the binary with `imgtool`:

```bash
imgtool sign \
    --key dev-signing-key.pem \
    --align 4 \
    --version 1.0.0.0 \
    --header-size 0x200 \
    --pad-header \
    --slot-size <slot_size_bytes> \
    nanoCLR.bin \
    nanoCLR-signed.bin
```

For ESP32 targets, use `--header-size 0x20` and `--align 32` (if flash encryption is enabled).

Verify a signed image:

```bash
imgtool verify --key root-ec256.pem nanoCLR-signed.bin
```

---

## Key Distribution Policy

| Key type | Storage | Who holds it | Build usage |
|---------|---------|-------------|------------|
| Development key (`dev-signing-key.pem`) | This repo | All developers, CI | Debug and CI firmware builds |
| Production key | Azure Key Vault / HSM | Release CI pipeline only | Official nanoFramework firmware releases |
| Public key (`root-ec256.pem`) | This repo, compiled into MCUboot | All developers, CI | Embedded in every MCUboot binary |

---

## Key Rotation

When a production key needs to be rotated:

1. Generate a new key pair with `imgtool keygen`.
2. Rebuild MCUboot with the new public key embedded.
3. Flash the new MCUboot binary to all devices **before** distributing firmware signed with the new key.
4. All devices must be updated to the new MCUboot before old-key firmware stops working.
5. Revoke the old key from the key vault.

> ⚠️ Key rotation requires a device firmware update campaign.  Plan carefully.

---

## imgtool Quick Reference

```bash
# Install imgtool
pip install imgtool

# Generate key
imgtool keygen --key <keyfile.pem> --type ecdsa-p256

# Extract public key (C source)
imgtool getpub --key <keyfile.pem> --lang c

# Sign image
imgtool sign --key <keyfile.pem> --align <n> --version <x.y.z.b> \
             --header-size <hdr> --pad-header --slot-size <size> \
             <input.bin> <output-signed.bin>

# Verify signed image
imgtool verify --key <pubkey.pem> <signed.bin>

# Show image header/TLV info
imgtool imginfo <signed.bin>
```
