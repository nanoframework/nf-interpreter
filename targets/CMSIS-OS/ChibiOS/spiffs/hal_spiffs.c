//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <spiffs.h>
#include <spiffs_config.h>
#include <target_spiffs.h>
#include <nanoHAL_v2.h>

extern s32_t hal_spiffs_erase(u32_t addr, u32_t size);
extern s32_t hal_spiffs_read(u32_t addr, u32_t size, u8_t *dst);
extern s32_t hal_spiffs_write(u32_t addr, u32_t size, u8_t *src);

extern uint8_t target_spiffs_init();

mutex_t spiffs_mutex;

spiffs fs;
spiffs_config spiffs_cfg;
bool spiffsFileSystemReady;

#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t spiffs_work_buffer[SPIFFS_WORK_BUFFER_SIZE];

#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t spiffs_fd_space[SPIFFS_FILE_DESCRIPTORS_SPACE];

#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t spiffs_cache[SPIFFS_CACHE_SIZE];

// initialization of SPIFFS: configurations, data structures, drivers and lock
uint8_t hal_spiffs_config()
{
    uint8_t mountResult;

    // low level hardware and drivers initializations
    target_spiffs_init();

    osalMutexObjectInit(&spiffs_mutex);

    // setup SPIFFS configurations
    spiffs_cfg.phys_size = SPIFFS_TOTAL_SIZE;
    spiffs_cfg.phys_addr = 0;
    spiffs_cfg.phys_erase_block = SPIFFS_ERASE_BLOCK_SIZE;
    spiffs_cfg.log_block_size = SPIFFS_LOGICAL_BLOCK_SIZE;
    spiffs_cfg.log_page_size = SPIFFS_LOGICAL_PAGE_SIZE;

    // setup pointers to HAL functions
    spiffs_cfg.hal_erase_f = hal_spiffs_erase;
    spiffs_cfg.hal_read_f = hal_spiffs_read;
    spiffs_cfg.hal_write_f = hal_spiffs_write;

    mountResult =  SPIFFS_mount(&fs,
                                &spiffs_cfg,
                                spiffs_work_buffer,
                                spiffs_fd_space,
                                SPIFFS_FILE_DESCRIPTORS_SPACE,
                                spiffs_cache,
                                SPIFFS_CACHE_SIZE,
                                0);

    if( mountResult != SPIFFS_OK &&
        SPIFFS_errno(&fs) == SPIFFS_ERR_NOT_A_FS)
    {
        // looks like SPIFFS is not formated
        
        // need to unmount 1st
        SPIFFS_unmount(&fs);

        // now format
        if (SPIFFS_format(&fs) != SPIFFS_OK)
        {
            return -1;
        }

        // finally try mounting it again
        mountResult =  SPIFFS_mount(&fs,
                                    &spiffs_cfg,
                                    spiffs_work_buffer,
                                    spiffs_fd_space,
                                    SPIFFS_FILE_DESCRIPTORS_SPACE,
                                    spiffs_cache,
                                    SPIFFS_CACHE_SIZE,
                                    0);      
    }

  #if !defined(BUILD_RTM)

    ASSERT(mountResult == SPIFFS_OK)

  #endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // // code block to assist testing SPIFFS
    // char writeBuf[] = {"Hello! if you get this message, congratulations, that's because SPIFFS is working on your device!!"};
    // char readBuf[sizeof(writeBuf)];
    
    // spiffs_file fd = SPIFFS_open(&fs, "file1.txt", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
    // if (SPIFFS_write(&fs, fd, writeBuf, sizeof(writeBuf)) < 0)
    // {
    //     return -1;
    // }
    
    // SPIFFS_close(&fs, fd);    
    // fd = SPIFFS_open(&fs, "file1.txt", SPIFFS_RDWR, 0);
    // if (SPIFFS_read(&fs, fd, readBuf, sizeof(readBuf)) < 0)
    // {
    //     return -1;
    // }
    
    // SPIFFS_close(&fs, fd);
    
    // uint32_t total = 0;
    // uint32_t used_space = 0;
    // SPIFFS_info(&fs, &total, &used_space);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    // mirror SPIFFS mount result into global flag
    spiffsFileSystemReady = (mountResult == SPIFFS_OK);

    return mountResult;
}

void hal_spiffs_lock()
{
    osalMutexLock(&spiffs_mutex);
}

void hal_spiffs_unlock()
{
    osalMutexUnlock(&spiffs_mutex);
}
