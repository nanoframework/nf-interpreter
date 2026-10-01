//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// nanoFramework management group (64) for MCUboot serial recovery.
//
// boot_serial.c hands every request outside the default (0) and image (1) groups to
// bs_peruser_system_specific() when MCUBOOT_PERUSER_MGMT_GROUP_ENABLED is 1, and emits
// whatever gets encoded into the CBOR state when this returns 0.
//
// Supported:
//   group 64, cmd 5 (read) - device info:
//     { "target": <str>, "mcuboot_ver": <str>, "nanomcuboot_ver": <str> }
//
// Anything else replies { "rc": 8 } (ENOTSUP), so clients get a prompt answer
// instead of a timeout.

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <zcbor_encode.h>

#include "boot_serial_priv.h"

// identity strings, supplied by MCUboot/CMakeLists.txt
#ifndef NF_MCUBOOT_TARGET_NAME
#define NF_MCUBOOT_TARGET_NAME "unknown"
#endif

#ifndef NF_MCUBOOT_UPSTREAM_VERSION
#define NF_MCUBOOT_UPSTREAM_VERSION "unknown"
#endif

#ifndef NF_NANOMCUBOOT_BUILD_VERSION
#define NF_NANOMCUBOOT_BUILD_VERSION "unknown"
#endif

// implemented in boot_serial.c: rewinds the response CBOR state (cs) to an empty buffer
extern void reset_cbor_state(void);

// command IDs in the nanoFramework management group
#define NF_MGMT_ID_DEVICE_INFO 5

// maximum number of entries in the device info map
#define NF_DEVICE_INFO_MAP_ENTRIES 3

// Encodes a text string key/value pair from a NUL-terminated string.
static bool nf_put_str(zcbor_state_t *cs, const char *key, const char *value)
{
    return zcbor_tstr_encode_ptr(cs, key, strlen(key)) && zcbor_tstr_encode_ptr(cs, value, strlen(value));
}

static bool nf_encode_device_info(zcbor_state_t *cs)
{
    return zcbor_map_start_encode(cs, NF_DEVICE_INFO_MAP_ENTRIES) &&
           nf_put_str(cs, "target", NF_MCUBOOT_TARGET_NAME) &&
           nf_put_str(cs, "mcuboot_ver", NF_MCUBOOT_UPSTREAM_VERSION) &&
           nf_put_str(cs, "nanomcuboot_ver", NF_NANOMCUBOOT_BUILD_VERSION) &&
           zcbor_map_end_encode(cs, NF_DEVICE_INFO_MAP_ENTRIES);
}

static void nf_encode_rc(zcbor_state_t *cs, uint32_t rc)
{
    zcbor_map_start_encode(cs, 1);
    zcbor_tstr_put_lit(cs, "rc");
    zcbor_uint32_put(cs, rc);
    zcbor_map_end_encode(cs, 1);
}

int bs_peruser_system_specific(const struct nmgr_hdr *hdr, const char *buffer, int len, zcbor_state_t *cs)
{
    (void)buffer;
    (void)len;

    if (hdr->nh_group == MGMT_GROUP_ID_PERUSER && hdr->nh_id == NF_MGMT_ID_DEVICE_INFO &&
        hdr->nh_op == NMGR_OP_READ)
    {
        if (nf_encode_device_info(cs))
        {
            return MGMT_ERR_OK;
        }

        reset_cbor_state();
        nf_encode_rc(cs, MGMT_ERR_ENOMEM);

        return MGMT_ERR_OK;
    }

    nf_encode_rc(cs, MGMT_ERR_ENOTSUP);

    return MGMT_ERR_OK;
}
