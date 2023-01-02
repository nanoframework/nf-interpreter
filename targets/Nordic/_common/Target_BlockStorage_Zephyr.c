//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include <zephyr.h>
#include <drivers/flash.h>
#include <device.h>
#include <ncs_version.h>

#include <Target_BlockStorage.h>
#include <Device_BlockStorage.h>
#include <Target_BlockStorage_ZephyrFlashDriver.h>

static const struct device *flash_device;
static const struct flash_parameters *flash_params;
bool ZephyrFlashDriver_InitializeDevice(void *context)
{
    (void)context;
    //#if (NCS_VERSION_MAJOR < 2)
    flash_device = device_get_binding(FLASH_DRIVER_NAME);
    if (flash_device)
    {
        flash_params = flash_get_parameters(flash_device);
        return true;
    }
    // #else
    //     flash_device = DEVICE_DT_GET(FLASH_DRIVER);
    // 	if (device_is_ready(flash_device)) {
    // 		printk("Flash device not ready\n");
    // 		return true;
    // 	}
    // #endif
    printk("***** Failed to open Flash driver. *****\n");
    return false;
}

bool ZephyrFlashDriver_UninitializeDevice(void *context)
{
    (void)context;

    // Zephyr driver takes care of this, so always true
    return true;
}

DeviceBlockInfo *ZephyrFlashDriver_GetDeviceInfo(void *context)
{

    MEMORY_MAPPED_NOR_BLOCK_CONFIG *config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG *)context;

    return config->BlockConfig.BlockDeviceInformation;
}

bool ZephyrFlashDriver_Read(void *context, ByteAddress startAddress, unsigned int numBytes, unsigned char *buffer)
{
    (void)context;

    if (flash_device == 0)
    {
        printk("ERROR flash_device not initialized\n");
        return false;
    }

    // Zephyr read call
    int rc = flash_read(flash_device, startAddress, buffer, numBytes);

    if (rc)
    {
        printk("ZephyrFlashDriver_Read error %d addr %x len %d\n", rc, startAddress, numBytes);
        return false;
    }

    return true;
}

bool ZephyrFlashDriver_Write(
    void *context,
    ByteAddress startAddress,
    unsigned int numBytes,
    unsigned char *buffer,
    bool readModifyWrite)
{
    (void)context;
    (void)readModifyWrite;

    if (flash_device == 0)
    {
        printk("ERROR flash_device not initialized\n");
        return false;
    }

    int rc = flash_write(flash_device, startAddress, buffer, numBytes);

    if (rc)
    {
        return false;
    }

    return true;
}

#define CHK_ERASE_BUF_SIZE 64
bool ZephyrFlashDriver_IsBlockErased(void *context, ByteAddress blockAddress, unsigned int length)
{
    (void)context;
    bool fErased = true;
    char buf[CHK_ERASE_BUF_SIZE];
    int i, rc, len;

    if (flash_device == 0)
    {
        printk("ERROR flash_device not initialized\n");
        return false;
    }

    while (fErased && (length > 0))
    {
        if (length < CHK_ERASE_BUF_SIZE)
        {
            len = length;
        }
        else
        {
            len = CHK_ERASE_BUF_SIZE;
        }

        rc = flash_read(flash_device, blockAddress, buf, len);

        if (rc)
        {
            printk("ZephyrFlashDriver_IsBlockErased, Error reading flash at %x len %d\n", blockAddress, len);
            fErased = false;
        }
        else
        {
            for (i = 0; i < len; i++)
            {
                if (buf[i] != flash_params->erase_value)
                {
                    fErased = false;
                    break;
                }
            }

            if (fErased)
            {
                blockAddress += len;
                length -= len;
            }
        }
    }

    return fErased;
}

bool ZephyrFlashDriver_EraseBlock(void *context, ByteAddress address)
{
    (void)context;
    int rc;
    struct flash_pages_info info;

    if (flash_device == 0)
    {
        printk("ERROR flash_device not initialized\n");
        return false;
    }

    // get info on the page
    rc = flash_get_page_info_by_offs(flash_device, address, &info);

    if (rc)
    {
        printk("ZephyrFlashDriver_EraseBlock. flash_get_page_info_by_offs error %d\n", rc);
        return false;
    }

    // erase that page.
    rc = flash_erase(flash_device, info.start_offset, info.size);

    if (rc)
    {
        printk("ZephyrFlashDriver_EraseBlock flash_erase error %d\n", rc);
        return false;
    }

    return true;
}
