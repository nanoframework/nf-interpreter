//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "hal.h"
#include "ff.h"
#include "diskio.h"

extern MMCDriver MMCD1;
extern void SdCardWaitSpi(void);

DSTATUS disk_status(BYTE drive)
{
    if (drive != 0)
    {
        return STA_NOINIT;
    }

    DSTATUS status = 0;
    if (blkGetDriverState(&MMCD1) != BLK_READY)
    {
        status |= STA_NOINIT;
    }
    if (blkIsWriteProtected(&MMCD1))
    {
        status |= STA_PROTECT;
    }
    return status;
}

DSTATUS disk_initialize(BYTE drive)
{
    return disk_status(drive);
}

DRESULT disk_read(BYTE drive, BYTE *buffer, LBA_t sector, UINT count)
{
    if (drive != 0 || count == 0)
    {
        return RES_PARERR;
    }
    if (blkGetDriverState(&MMCD1) != BLK_READY)
    {
        return RES_NOTRDY;
    }

    SdCardWaitSpi();
    return blkRead(&MMCD1, sector, buffer, count) ? RES_ERROR : RES_OK;
}

#if !FF_FS_READONLY
DRESULT disk_write(BYTE drive, const BYTE *buffer, LBA_t sector, UINT count)
{
    if (drive != 0 || count == 0)
    {
        return RES_PARERR;
    }
    if (blkGetDriverState(&MMCD1) != BLK_READY)
    {
        return RES_NOTRDY;
    }

    SdCardWaitSpi();
    return blkWrite(&MMCD1, sector, buffer, count) ? RES_ERROR : RES_OK;
}
#endif

DRESULT disk_ioctl(BYTE drive, BYTE command, void *buffer)
{
    BlockDeviceInfo info;

    if (drive != 0)
    {
        return RES_PARERR;
    }
    if (blkGetDriverState(&MMCD1) != BLK_READY)
    {
        return RES_NOTRDY;
    }

    switch (command)
    {
        case CTRL_SYNC:
            SdCardWaitSpi();
            return blkSync(&MMCD1) ? RES_ERROR : RES_OK;

        case GET_SECTOR_COUNT:
            if (buffer == NULL)
            {
                return RES_PARERR;
            }
            if (blkGetInfo(&MMCD1, &info))
            {
                return RES_ERROR;
            }
            *(LBA_t *)buffer = info.blk_num;
            return RES_OK;

        default:
            return RES_PARERR;
    }
}

DWORD get_fattime(void)
{
#if HAL_USE_RTC
    RTCDateTime time;
    rtcGetTime(&RTCD1, &time);
    return rtcConvertDateTimeToFAT(&time);
#else
    return (1U << 16) | (1U << 21);
#endif
}
