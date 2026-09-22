//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// FatFs implementation of the MCUboot media import ops (see mcuboot_media_fatfs.h).
//
// The medium is an ordinary FatFs volume (the same SD card / USB stick the application
// uses through the file system). Update files are looked up in its root directory:
//   <volume>/nano-clr-update-*.bin              image 0 (nanoCLR)
//   <volume>/nano-deployment-update-*.bin       image 1 (deployment)
//   <volume>/<update file><MCUBOOT_IMPORT_USED_SUFFIX>  "used" marker: one stamp per line
// (patterns from mcuboot_media_import.h; FatFs FF_USE_FIND provides the wildcard match).
//
// The marker is a plain text file so an operator can see which devices (by unique id)
// already consumed the file. It is appended to, never rewritten, and a read-only
// medium simply leaves it as is (the engine copes with that).

#include <string.h>

#include <bootutil/bootutil_log.h>
#include <mcuboot_media_fatfs.h>

#if !FF_USE_FIND
#error "mcuboot_media_fatfs.c needs FF_USE_FIND (f_findfirst/f_findnext) in ffconf.h"
#endif

// Longest path built here: "<volume>/<name><suffix>" — volumes are 2-3 chars.
#define MEDIA_PATH_MAX (MCUBOOT_MEDIA_NAME_MAX + 16U)

// Build "<volume>/<name>[<suffix>]" into out. Returns false if it does not fit.
static bool build_path(const mcuboot_media_fatfs_ctx *ctx, const char *name, const char *suffix, char *out, size_t cap)
{
    size_t volLen = strlen(ctx->volume);
    size_t nameLen = strlen(name);
    size_t sufLen = (suffix != NULL) ? strlen(suffix) : 0U;

    // volume + '/' + name + suffix + NUL
    if (volLen + 1U + nameLen + sufLen + 1U > cap)
    {
        return false;
    }

    memcpy(out, ctx->volume, volLen);
    out[volLen] = '/';
    memcpy(out + volLen + 1U, name, nameLen);

    if (sufLen > 0U)
    {
        memcpy(out + volLen + 1U + nameLen, suffix, sufLen);
    }

    out[volLen + 1U + nameLen + sufLen] = '\0';

    return true;
}

static int fatfs_mount(void *p)
{
    mcuboot_media_fatfs_ctx *ctx = (mcuboot_media_fatfs_ctx *)p;

    ctx->fileOpen = false;

    if (ctx->device_init != NULL && ctx->device_init() != 0)
    {
        return -1;
    }

    // immediate mount so an absent/unformatted medium is detected right here
    FRESULT fr = f_mount(&ctx->fs, ctx->volume, 1);

    if (fr != FR_OK)
    {
        BOOT_LOG_ERR("media import: f_mount(%s) failed (FRESULT %d)", ctx->volume, (int)fr);

        if (ctx->device_deinit != NULL)
        {
            ctx->device_deinit();
        }

        return -1;
    }

    return 0;
}

static void fatfs_unmount(void *p)
{
    mcuboot_media_fatfs_ctx *ctx = (mcuboot_media_fatfs_ctx *)p;

    if (ctx->fileOpen)
    {
        f_close(&ctx->file);
        ctx->fileOpen = false;
    }

    (void)f_unmount(ctx->volume);

    if (ctx->device_deinit != NULL)
    {
        ctx->device_deinit();
    }
}

// n-th root-directory entry matching the image's pattern. Directories and names that do
// not fit the caller's buffer are skipped (not counted).
static int fatfs_find(void *p, uint8_t image, uint32_t n, char *name, size_t cap)
{
    mcuboot_media_fatfs_ctx *ctx = (mcuboot_media_fatfs_ctx *)p;
    const char *pattern = mcuboot_media_pattern_for_image(image);
    char root[8];
    DIR dir;
    FILINFO info;
    int rc = -1;

    if (pattern == NULL || !build_path(ctx, "", NULL, root, sizeof(root)))
    {
        return -1;
    }

    FRESULT fr = f_findfirst(&dir, &info, root, pattern);

    while (fr == FR_OK && info.fname[0] != '\0')
    {
        if ((info.fattrib & AM_DIR) == 0 && strlen(info.fname) < cap)
        {
            if (n == 0U)
            {
                strcpy(name, info.fname);
                rc = 0;
                break;
            }

            n--;
        }

        fr = f_findnext(&dir, &info);
    }

    f_closedir(&dir);

    return rc;
}

static int fatfs_open(void *p, const char *name, uint32_t *size)
{
    mcuboot_media_fatfs_ctx *ctx = (mcuboot_media_fatfs_ctx *)p;
    char path[MEDIA_PATH_MAX];

    if (ctx->fileOpen || !build_path(ctx, name, NULL, path, sizeof(path)))
    {
        return -1;
    }

    if (f_open(&ctx->file, path, FA_READ) != FR_OK)
    {
        return -1;
    }

    ctx->fileOpen = true;
    *size = (uint32_t)f_size(&ctx->file);

    return 0;
}

static int fatfs_read(void *p, uint32_t off, void *dst, uint32_t len)
{
    mcuboot_media_fatfs_ctx *ctx = (mcuboot_media_fatfs_ctx *)p;
    UINT read = 0;

    if (!ctx->fileOpen)
    {
        return -1;
    }

    if (f_tell(&ctx->file) != (FSIZE_t)off && f_lseek(&ctx->file, (FSIZE_t)off) != FR_OK)
    {
        return -1;
    }

    if (f_read(&ctx->file, dst, (UINT)len, &read) != FR_OK || read != (UINT)len)
    {
        return -1;
    }

    return 0;
}

static void fatfs_close(void *p)
{
    mcuboot_media_fatfs_ctx *ctx = (mcuboot_media_fatfs_ctx *)p;

    if (ctx->fileOpen)
    {
        f_close(&ctx->file);
        ctx->fileOpen = false;
    }
}

static bool fatfs_is_used(void *p, const char *name, const char *stamp)
{
    mcuboot_media_fatfs_ctx *ctx = (mcuboot_media_fatfs_ctx *)p;
    char path[MEDIA_PATH_MAX];
    FIL *marker = &ctx->marker;
    bool found = false;

    if (!build_path(ctx, name, MCUBOOT_IMPORT_USED_SUFFIX, path, sizeof(path)))
    {
        return false;
    }

    if (f_open(marker, path, FA_READ) != FR_OK)
    {
        // no marker yet
        return false;
    }

    char line[MCUBOOT_MEDIA_STAMP_MAX + 2U];

    while (!found && f_gets(line, sizeof(line), marker) != NULL)
    {
        // strip the line terminator(s)
        size_t n = strlen(line);

        while (n > 0U && (line[n - 1U] == '\n' || line[n - 1U] == '\r'))
        {
            line[--n] = '\0';
        }

        found = (strcmp(line, stamp) == 0);
    }

    f_close(marker);

    return found;
}

static int fatfs_mark_used(void *p, const char *name, const char *stamp)
{
    mcuboot_media_fatfs_ctx *ctx = (mcuboot_media_fatfs_ctx *)p;
    char path[MEDIA_PATH_MAX];
    FIL *marker = &ctx->marker;
    int rc = -1;

    if (!build_path(ctx, name, MCUBOOT_IMPORT_USED_SUFFIX, path, sizeof(path)))
    {
        return -1;
    }

    if (f_open(marker, path, FA_WRITE | FA_OPEN_APPEND) != FR_OK)
    {
        return -1;
    }

    if (f_puts(stamp, marker) >= 0 && f_puts("\r\n", marker) >= 0 && f_sync(marker) == FR_OK)
    {
        rc = 0;
    }

    f_close(marker);

    return rc;
}

const mcuboot_media_ops mcuboot_media_fatfs_ops = {
    .mount = fatfs_mount,
    .unmount = fatfs_unmount,
    .find = fatfs_find,
    .open = fatfs_open,
    .read = fatfs_read,
    .close = fatfs_close,
    .is_used = fatfs_is_used,
    .mark_used = fatfs_mark_used,
};
