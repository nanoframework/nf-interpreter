//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// FatFs diskio layer for the MCUboot bootloader on ST_STM32F769I_DISCOVERY.
// Drives physical drive 0 (SD card) via the ChibiOS SDC driver (SDCD1 / SDMMC1).
// Mirrors the structure of targets/ChibiOS/_FatFs/fatfs_diskio.c but is
// self-contained so that the nanoCLR diskio can evolve independently.

#include "hal.h"
#include "ffconf.h"
#include "ff.h"
#include "diskio.h"

#define SDC_DRIVE 0

/*-----------------------------------------------------------------------*/
/* Initialize a drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE pdrv)
{
    if (pdrv != SDC_DRIVE)
    {
        return STA_NOINIT;
    }

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

    return 0;
}

/*-----------------------------------------------------------------------*/
/* Return disk status                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv)
{
    if (pdrv != SDC_DRIVE)
    {
        return STA_NOINIT;
    }

    DSTATUS stat = 0;

    if (blkGetDriverState(&SDCD1) != BLK_READY)
    {
        stat |= STA_NOINIT;
    }

    if (blkIsWriteProtected(&SDCD1))
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
    if (pdrv != SDC_DRIVE)
    {
        return RES_PARERR;
    }

    if (blkGetDriverState(&SDCD1) != BLK_READY)
    {
        return RES_NOTRDY;
    }

    if (blkRead(&SDCD1, sector, buff, count) != HAL_SUCCESS)
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
    if (pdrv != SDC_DRIVE)
    {
        return RES_PARERR;
    }

    if (blkGetDriverState(&SDCD1) != BLK_READY)
    {
        return RES_NOTRDY;
    }

    if (blkWrite(&SDCD1, sector, buff, count) != HAL_SUCCESS)
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
    if (pdrv != SDC_DRIVE)
    {
        return RES_PARERR;
    }

    if (blkGetDriverState(&SDCD1) != BLK_READY)
    {
        return RES_NOTRDY;
    }

    BlockDeviceInfo bdi;

    switch (cmd)
    {
        case CTRL_SYNC:
            return RES_OK;

        case GET_SECTOR_COUNT:
            if (blkGetInfo(&SDCD1, &bdi) != HAL_SUCCESS)
            {
                return RES_ERROR;
            }
            *(DWORD *)buff = bdi.blk_num;
            return RES_OK;

#if FF_MAX_SS > FF_MIN_SS
        case GET_SECTOR_SIZE:
            if (blkGetInfo(&SDCD1, &bdi) != HAL_SUCCESS)
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
