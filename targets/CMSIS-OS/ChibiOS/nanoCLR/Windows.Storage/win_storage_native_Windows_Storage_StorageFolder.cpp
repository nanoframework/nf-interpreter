//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ff.h>
#include "win_storage_native.h"
#include <target_windows_storage_config.h>


// FatFs define for size of file name members
// ANSI/OEM at DBCS
#define FF_LFN_BUF  255
#define SDCARD_DRIVE_LETTER  "D:\\"


// defining this type here to make it shorter and improve code readability
typedef Library_win_storage_native_Windows_Storage_StorageFolder StorageFolderType;

SYSTEMTIME GetDateTime(uint16_t date, uint16_t time)
{
    SYSTEMTIME fileTime;

    memset(&fileTime, 0, sizeof(SYSTEMTIME));

    // date (bit15:9) Year origin from 1980 (0..127)
    fileTime.wYear = (date >> 9) + 1980;
    // date (bit8:5) Month (1..12)
    fileTime.wMonth = (date >> 5) & 0x000F;
    // date (bit4:0) Day (1..31)
    fileTime.wDay = date & 0x001F;

    // time (bit15:11) Hour (0..23)
    fileTime.wHour = (time >> 11) & 0x001F;
    // time (bit10:5) Minute (0..59)
    fileTime.wMinute = (time >> 5) & 0x003F;
    // time (bit4:0) Second / 2 (0..29)
    fileTime.wSecond = time & 0x001F;

    return fileTime;
}

HRESULT Library_win_storage_native_Windows_Storage_StorageFolder::GetRemovableStorageFoldersNative___SZARRAY_WindowsStorageStorageFolder( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    char* stringBuffer;
    uint32_t driveCount = 0;

    CLR_RT_HeapBlock* storageFolder;
    CLR_RT_TypeDef_Index storateFolderTypeDef;
    CLR_RT_HeapBlock* hbObj;
    CLR_RT_HeapBlock& top   = stack.PushValue();

    //  default to NULL (which is the expected outcome when no devices are connected)
    hbObj = top.Dereference();
    hbObj->SetObjectReference( NULL );

    // is there an SD card inserted and is driver in ready state?
    if(sdc_lld_is_card_inserted(&SD_CARD_DRIVER) && SD_CARD_DRIVER.state == BLK_READY)
    {
        driveCount++;
    }

    // FIXME (no support for USB thumb drive)
    // is there an USB thumb driver inserted?
    // driveCount++;

    if(sdc_lld_is_card_inserted(&SD_CARD_DRIVER) && SD_CARD_DRIVER.state == BLK_READY)
    {
        // find <StorageFolder> type definition, don't bother checking the result as it exists for sure
        g_CLR_RT_TypeSystem.FindTypeDef( "StorageFolder", "Windows.Storage", storateFolderTypeDef );
    
        // create an array of <StorageFolder>
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( top, driveCount, storateFolderTypeDef ));

        // get a pointer to the first object in the array (which is of type <StorageFolder>)
        storageFolder = (CLR_RT_HeapBlock*)top.DereferenceArray()->GetFirstElement();

        // create an instance of <StorageFolder>
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*storageFolder, storateFolderTypeDef));

        // dereference the object in order to reach its fields
        hbObj = storageFolder->Dereference();

        // set the managed fields
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[ FIELD___name ], SDCARD_DRIVE_LETTER ));
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[ FIELD___path ], SDCARD_DRIVE_LETTER ));

        // malloc stringBuffer to work with FS
        stringBuffer = (char*)platform_malloc(FF_LFN_BUF + 1);

        // sanity check for successfull malloc
        if(stringBuffer == NULL)
        {
            // failed to allocate memory
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }
        else
        {
            // read the drive volume label
            // don't bother checking the result, if anything goes wrong we'll end up with an empty string which is OK
            f_getlabel(SDCARD_DRIVE_LETTER, stringBuffer, NULL);

            // add the driver letter separated it with an empty space, if the volume label isn't empty
            if(*stringBuffer != '\0')
            {
                strcat(stringBuffer, " ");
            }
            strcat(stringBuffer, "(");
            strcat(stringBuffer, SDCARD_DRIVE_LETTER);
            strcat(stringBuffer, ")");

            // set the field with the volume label
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[ FIELD___name ], stringBuffer ));


            // free stringBuffer
            platform_free(stringBuffer);
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_storage_native_Windows_Storage_StorageFolder::GetStorageFoldersNative___SZARRAY_WindowsStorageStorageFolder( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Index    storateFolderTypeDef;
    CLR_RT_HeapBlock*       storageFolder;
    CLR_RT_HeapBlock*       hbObj;
    SYSTEMTIME              fileInfoTime;

    const char* path;
    char drive[2];

    DIR             currentDirectory;
    FRESULT         operationResult;
    static FILINFO  fileInfo;
    uint16_t        directoryCount = 0;
    char*           stringBuffer = NULL;
    char*           workingBuffer = NULL;

    CLR_RT_HeapBlock& top   = stack.PushValue();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
    
    // is there an SD card inserted and is driver in ready state?
    if(!sdc_lld_is_card_inserted(&SD_CARD_DRIVER) || SD_CARD_DRIVER.state != BLK_READY)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
    }
        
    // copy the first 2 letters of the path for the drive
    // path is 'D:\folder\file.txt', so we need 'D:'
    memcpy(drive, pThis[ FIELD___name ].DereferenceString()->StringText(), 2);
        
    // get a pointer to the path in managed field
    path = pThis[ FIELD___path ].DereferenceString()->StringText();

    // change drive
    if(f_chdrive(drive) == FR_OK)
    {
        // need to perform this in two passes
        // 1st: count the directory objects
        // 2nd: create the array items with each directory object

        // open directory
        operationResult = f_opendir(&currentDirectory, path);

        // perform 1st pass
        for (;;)
        {
            // read next directory item
            operationResult = f_readdir(&currentDirectory, &fileInfo);
            
            // break on error or at end of dir
            if (operationResult != FR_OK || fileInfo.fname[0] == 0)
            {
                break;
            }

            // check if this is a directory
            if (fileInfo.fattrib & AM_DIR) 
            {
                directoryCount++;
            }
        }

        // find <StorageFolder> type, don't bother checking the result as it exists for sure
        g_CLR_RT_TypeSystem.FindTypeDef( "StorageFolder", "Windows.Storage", storateFolderTypeDef );

        // create an array of <StorageFolder>
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( top, directoryCount, storateFolderTypeDef ));
        
        // get a pointer to the first object in the array (which is of type <StorageFolder>)
        storageFolder = (CLR_RT_HeapBlock*)top.DereferenceArray()->GetFirstElement();

        if(directoryCount > 0)
        {
            // allocate memory for buffers
            stringBuffer = (char*)platform_malloc(FF_LFN_BUF + 1);
            workingBuffer = (char*)platform_malloc(2 * FF_LFN_BUF + 1);

            // sanity check for successfull malloc
            if(stringBuffer == NULL || workingBuffer == NULL)
            {
                // failed to allocate memory
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }

            // perform 2nd pass
            // need to rewind the directory read index first
            f_readdir(&currentDirectory, NULL);
            
            for (;;)
            {
                // read next directory item
                operationResult = f_readdir(&currentDirectory, &fileInfo);
                
                // break on error or at end of dir
                if (operationResult != FR_OK || fileInfo.fname[0] == 0)
                {
                    break;
                }

                // check if this is a directory
                if (fileInfo.fattrib & AM_DIR) 
                {
                    // create an instance of <StorageFolder>
                    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*storageFolder, storateFolderTypeDef));

                    // dereference the object in order to reach its fields
                    hbObj = storageFolder->Dereference();

                    // directory name
                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[ FIELD___name ], fileInfo.fname ));

                    // clear working buffer
                    memset(workingBuffer, 0, 2 * FF_LFN_BUF + 1);

                    // compose directory path
                    strcat(workingBuffer, path);
                    strcat(workingBuffer, fileInfo.fname);
                    
                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[ FIELD___path ], workingBuffer ));
                    
                    // compute directory date
                    fileInfoTime = GetDateTime(fileInfo.fdate, fileInfo.ftime);

                    // get a reference to the dateCreated managed field...
                    CLR_RT_HeapBlock& dateFieldRef = hbObj[ FIELD___dateCreated ];
                    CLR_INT64* pRes = (CLR_INT64*)&dateFieldRef.NumericByRef().s8;
                    // ...and set it with the fileInfoTime
                    *pRes = HAL_Time_ConvertFromSystemTime( &fileInfoTime );

                    // move the storage folder pointer to the next item in the array
                    storageFolder++;
                }
            }
        }
        else
        {
            // failed to open directory
            NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
        }
    }
    else
    {
        // failed to change drive
        NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
    }

    NANOCLR_CLEANUP();

    // close directory
    f_closedir(&currentDirectory);

    // free buffers memory
    platform_free(stringBuffer);
    platform_free(workingBuffer);

    NANOCLR_CLEANUP_END();
}
