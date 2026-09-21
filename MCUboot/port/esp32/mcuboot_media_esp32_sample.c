//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// SAMPLE — not part of any build.
//
// Shows what a platform other than ChibiOS has to provide to reuse the MCUboot media
// import engine (MCUboot/common/mcuboot_media_import.c, interface in
// MCUboot/include/mcuboot_media_import.h) for an SD card mounted through the ESP-IDF
// VFS + FatFs stack. The engine itself is platform-neutral; a port supplies:
//
//   1. one mcuboot_media_ops implementation per filesystem/media type
//      (mount/unmount, find, open/read/close, is_used/mark_used),
//   2. mcuboot_media_table()   — the media to sweep, in order,
//   3. mcuboot_media_device_id() — a stable unique id for the per-device "used" marker,
//   4. optionally mcuboot_media_watchdog_feed() (weak no-op in the engine).
//
// Then call mcuboot_media_import_run() from the bootloader main, after the flash devices
// backing the flash_area_* API are ready and before boot_go().
//
// The ChibiOS reference implementation is targets/ChibiOS/_mcuboot/mcuboot_media_fatfs.c
// (FatFs ops) plus the board table in targets/ChibiOS/<BOARD>/MCUboot/mcuboot_media_boot.c.
//
// ESP-IDF specifics sketched here (untested):
//   - esp_vfs_fat_sdmmc_mount() mounts the card at a VFS mount point, so plain POSIX
//     opendir/readdir + stdio serve the enumeration and file access,
//   - the "used" marker is the same "<image file>.used" text file, one stamp per line,
//   - the device id is the base MAC from eFuse.

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <dirent.h>

#include <esp_vfs_fat.h>
#include <sdmmc_cmd.h>
#include <driver/sdmmc_host.h>
#include <esp_mac.h>

#include <mcuboot_media_import.h>

typedef struct
{
    const char *mountPoint; // e.g. "/sdcard"
    sdmmc_card_t *card;
    FILE *file;
} esp32_sd_ctx;

// Minimal glob: '*' matches any run, '?' one character; case-insensitive (FAT names).
static bool glob_match(const char *pattern, const char *name)
{
    while (*pattern != '\0')
    {
        if (*pattern == '*')
        {
            pattern++;

            if (*pattern == '\0')
            {
                return true;
            }

            for (const char *n = name; *n != '\0'; n++)
            {
                if (glob_match(pattern, n))
                {
                    return true;
                }
            }

            return false;
        }

        if (*name == '\0')
        {
            return false;
        }

        if (*pattern != '?' && tolower((unsigned char)*pattern) != tolower((unsigned char)*name))
        {
            return false;
        }

        pattern++;
        name++;
    }

    return *name == '\0';
}

static bool build_path(const esp32_sd_ctx *ctx, const char *name, const char *suffix, char *out, size_t cap)
{
    int n = snprintf(out, cap, "%s/%s%s", ctx->mountPoint, name, (suffix != NULL) ? suffix : "");

    return (n > 0) && ((size_t)n < cap);
}

// n-th regular file in the mount point root matching the image's pattern.
static int sd_find(void *p, uint8_t image, uint32_t n, char *name, size_t cap)
{
    esp32_sd_ctx *ctx = (esp32_sd_ctx *)p;
    const char *pattern = mcuboot_media_pattern_for_image(image);
    int rc = -1;

    if (pattern == NULL)
    {
        return -1;
    }

    DIR *dir = opendir(ctx->mountPoint);

    if (dir == NULL)
    {
        return -1;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_DIR || !glob_match(pattern, entry->d_name) || strlen(entry->d_name) >= cap)
        {
            continue;
        }

        if (n == 0)
        {
            strcpy(name, entry->d_name);
            rc = 0;
            break;
        }

        n--;
    }

    closedir(dir);

    return rc;
}

static int sd_mount(void *p)
{
    esp32_sd_ctx *ctx = (esp32_sd_ctx *)p;

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t cfg = {
        .format_if_mount_failed = false,
        .max_files = 2,
    };

    ctx->file = NULL;

    return (esp_vfs_fat_sdmmc_mount(ctx->mountPoint, &host, &slot, &cfg, &ctx->card) == ESP_OK) ? 0 : -1;
}

static void sd_unmount(void *p)
{
    esp32_sd_ctx *ctx = (esp32_sd_ctx *)p;

    if (ctx->file != NULL)
    {
        fclose(ctx->file);
        ctx->file = NULL;
    }

    (void)esp_vfs_fat_sdcard_unmount(ctx->mountPoint, ctx->card);
    ctx->card = NULL;
}

static int sd_open(void *p, const char *name, uint32_t *size)
{
    esp32_sd_ctx *ctx = (esp32_sd_ctx *)p;
    char path[128];

    if (ctx->file != NULL || !build_path(ctx, name, NULL, path, sizeof(path)))
    {
        return -1;
    }

    ctx->file = fopen(path, "rb");

    if (ctx->file == NULL)
    {
        return -1;
    }

    fseek(ctx->file, 0, SEEK_END);
    *size = (uint32_t)ftell(ctx->file);
    fseek(ctx->file, 0, SEEK_SET);

    return 0;
}

static int sd_read(void *p, uint32_t off, void *dst, uint32_t len)
{
    esp32_sd_ctx *ctx = (esp32_sd_ctx *)p;

    if (ctx->file == NULL || fseek(ctx->file, (long)off, SEEK_SET) != 0)
    {
        return -1;
    }

    return (fread(dst, 1, len, ctx->file) == len) ? 0 : -1;
}

static void sd_close(void *p)
{
    esp32_sd_ctx *ctx = (esp32_sd_ctx *)p;

    if (ctx->file != NULL)
    {
        fclose(ctx->file);
        ctx->file = NULL;
    }
}

static bool sd_is_used(void *p, const char *name, const char *stamp)
{
    esp32_sd_ctx *ctx = (esp32_sd_ctx *)p;
    char path[128];
    char line[MCUBOOT_MEDIA_STAMP_MAX + 2];
    bool found = false;

    if (!build_path(ctx, name, MCUBOOT_IMPORT_USED_SUFFIX, path, sizeof(path)))
    {
        return false;
    }

    FILE *marker = fopen(path, "r");

    if (marker == NULL)
    {
        return false;
    }

    while (!found && fgets(line, sizeof(line), marker) != NULL)
    {
        line[strcspn(line, "\r\n")] = '\0';
        found = (strcmp(line, stamp) == 0);
    }

    fclose(marker);

    return found;
}

static int sd_mark_used(void *p, const char *name, const char *stamp)
{
    esp32_sd_ctx *ctx = (esp32_sd_ctx *)p;
    char path[128];

    if (!build_path(ctx, name, MCUBOOT_IMPORT_USED_SUFFIX, path, sizeof(path)))
    {
        return -1;
    }

    FILE *marker = fopen(path, "a");

    if (marker == NULL)
    {
        return -1;
    }

    int rc = (fprintf(marker, "%s\r\n", stamp) > 0) ? 0 : -1;

    fclose(marker);

    return rc;
}

static const mcuboot_media_ops s_esp32SdOps = {
    .mount = sd_mount,
    .unmount = sd_unmount,
    .find = sd_find,
    .open = sd_open,
    .read = sd_read,
    .close = sd_close,
    .is_used = sd_is_used,
    .mark_used = sd_mark_used,
};

static esp32_sd_ctx s_sdCtx = {
    .mountPoint = "/sdcard",
};

static const mcuboot_media s_mediaTable[] = {
    {.name = "SD card", .ops = &s_esp32SdOps, .ctx = &s_sdCtx},
};

const mcuboot_media *mcuboot_media_table(uint32_t *count)
{
    *count = sizeof(s_mediaTable) / sizeof(s_mediaTable[0]);
    return s_mediaTable;
}

// Base MAC address from eFuse, 6 bytes -> 12 lowercase hex characters.
size_t mcuboot_media_device_id(char *hex, size_t cap)
{
    uint8_t mac[6];

    if (cap < 13 || esp_efuse_mac_get_default(mac) != ESP_OK)
    {
        return 0;
    }

    int n = snprintf(hex, cap, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return (n > 0) ? (size_t)n : 0;
}

// Optional: feed the task watchdog / RTC WDT here if the bootloader arms one.
void mcuboot_media_watchdog_feed(void)
{
}
