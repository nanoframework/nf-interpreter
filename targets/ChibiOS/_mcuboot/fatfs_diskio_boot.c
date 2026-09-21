//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// FatFs diskio layer for the ChibiOS MCUboot bootloader.
//
// Serves the physical drives the media import step can read update images from:
//   - SD card via the ChibiOS SDC driver (SDCD1)            when HAL_USE_SDC
//   - USB mass storage via ChibiOS-Contrib USBH MSD (MSBLKD[0]) when HAL_USBH_USE_MSD
//
// Physical drive numbers follow the volume order the target's ffconf.h derives from the
// same two switches (FF_VOLUME_STRS "D","E" = SD, USB; a single medium is drive 0), so the
// same ffconf.h serves nanoCLR and the bootloader. Both devices go through the generic
// ChibiOS block-device API; the board brings them up (sdcConnect / usbhmsdLUNConnect)
// from its media table before FatFs touches them.
//
// Mirrors the structure of targets/ChibiOS/_FatFs/fatfs_diskio.c but is self-contained
// so that the nanoCLR diskio can evolve independently.

#include "hal.h"
#include "ffconf.h"
#include "ff.h"
#include "diskio.h"

#if HAL_USBH_USE_MSD
#include "usbh/dev/msd.h"
#endif

#if HAL_USE_SDC
#define SDC_DRIVE 0
#define USB_DRIVE 1
#else
#define USB_DRIVE 0
#endif

// Resolve a physical drive number to its block device, NULL if not configured.
static BaseBlockDevice *drive_to_blk(BYTE pdrv)
{
    switch (pdrv)
    {
#if HAL_USE_SDC
        case SDC_DRIVE:
            return (BaseBlockDevice *)&SDCD1;
#endif

#if HAL_USBH_USE_MSD
        case USB_DRIVE:
            return (BaseBlockDevice *)&MSBLKD[0];
#endif

        default:
            return NULL;
    }
}

/*-----------------------------------------------------------------------*/
/* Initialize a drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE pdrv)
{
    BaseBlockDevice *blk = drive_to_blk(pdrv);

    if (blk == NULL)
    {
        return STA_NOINIT;
    }

#if HAL_USE_SDC
    // the SD card can be brought up here if the board did not do it already
    if (pdrv == SDC_DRIVE)
    {
        if (blkGetDriverState(&SDCD1) == BLK_STOP)
        {
            sdcStart(&SDCD1, NULL);
        }

        if (blkGetDriverState(&SDCD1) != BLK_READY)
        {
            if (sdcConnect(&SDCD1) != HAL_SUCCESS)
            {
                return STA_NOINIT;
            }
        }
    }
#endif

    // USB MSD is enumerated and connected by the board before the mount; just report it
    if (blkGetDriverState(blk) != BLK_READY)
    {
        return STA_NOINIT;
    }

    return 0;
}

/*-----------------------------------------------------------------------*/
/* Return disk status                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv)
{
    BaseBlockDevice *blk = drive_to_blk(pdrv);

    if (blk == NULL)
    {
        return STA_NOINIT;
    }

    DSTATUS stat = 0;

    if (blkGetDriverState(blk) != BLK_READY)
    {
        stat |= STA_NOINIT;
    }

    if (blkIsWriteProtected(blk))
    {
        stat |= STA_PROTECT;
    }

    return stat;
}

/*-----------------------------------------------------------------------*/
/* Read sectors                                                          */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    BaseBlockDevice *blk = drive_to_blk(pdrv);

    if (blk == NULL)
    {
        return RES_PARERR;
    }

    if (blkGetDriverState(blk) != BLK_READY)
    {
        return RES_NOTRDY;
    }

    if (blkRead(blk, sector, buff, count) != HAL_SUCCESS)
    {
        return RES_ERROR;
    }

    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write sectors                                                         */
/*-----------------------------------------------------------------------*/

#if !FF_FS_READONLY
DRESULT disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    BaseBlockDevice *blk = drive_to_blk(pdrv);

    if (blk == NULL)
    {
        return RES_PARERR;
    }

    if (blkGetDriverState(blk) != BLK_READY)
    {
        return RES_NOTRDY;
    }

    if (blkWrite(blk, sector, buff, count) != HAL_SUCCESS)
    {
        return RES_ERROR;
    }

    return RES_OK;
}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    BaseBlockDevice *blk = drive_to_blk(pdrv);

    if (blk == NULL)
    {
        return RES_PARERR;
    }

    if (blkGetDriverState(blk) != BLK_READY)
    {
        return RES_NOTRDY;
    }

    BlockDeviceInfo bdi;

    switch (cmd)
    {
        case CTRL_SYNC:
            return RES_OK;

        case GET_SECTOR_COUNT:
            if (blkGetInfo(blk, &bdi) != HAL_SUCCESS)
            {
                return RES_ERROR;
            }
            *(DWORD *)buff = bdi.blk_num;
            return RES_OK;

#if FF_MAX_SS > FF_MIN_SS
        case GET_SECTOR_SIZE:
            if (blkGetInfo(blk, &bdi) != HAL_SUCCESS)
            {
                return RES_ERROR;
            }
            *(WORD *)buff = bdi.blk_size;
            return RES_OK;
#endif

        default:
            return RES_PARERR;
    }
}
