//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#if (HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE)
#include <ff.h>
#endif
#include "win_storage_native_target.h"
#include <target_windows_storage_config.h>
#include <Target_Windows_Storage.h>
#include <nanoHAL_Windows_Storage.h>

#if (HAL_USBH_USE_MSD == TRUE)
#include "usbh/dev/msd.h"
#endif
#if (USE_SPIFFS_FOR_STORAGE == TRUE)
#include <hal_spiffs.h>
#endif

// flags for file system ready
#if (HAL_USE_SDC == TRUE)
extern bool sdCardFileSystemReady;
#endif
#if (HAL_USBH_USE_MSD == TRUE)
extern bool usbMsdFileSystemReady;
#endif
#if (USE_SPIFFS_FOR_STORAGE == TRUE)
extern bool spiffsFileSystemReady;
extern spiffs fs;
#endif

void CombinePath(char * outpath, const char * path1, const char * path2)
{
	strcat(outpath, path1);
	
	// Add "\" to path if required
	if (outpath[hal_strlen_s(outpath) - 1] != '\\')
	{
		strcat(outpath, "\\");
	}
	strcat(outpath, path2);
}

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
    char workingDrive[sizeof(DRIVE_PATH_LENGTH)];
    uint16_t maxDriveCount = 0;

    CLR_RT_HeapBlock* storageFolder;
    CLR_RT_TypeDef_Index storageFolderTypeDef;
    CLR_RT_HeapBlock* hbObj;
    CLR_RT_HeapBlock& top   = stack.PushValue();

  #if HAL_USE_SDC
    // increase max drive count
    maxDriveCount++;

    // is the SD card file system ready?
    if(sdCardFileSystemReady)
    {
        // add count
        driveCount++;
    }
  #endif

  #if HAL_USBH_USE_MSD
    // increase max drive count
    maxDriveCount++;
    
    // is the USB mass storage device file system ready?
    if(usbMsdFileSystemReady)
    {
        // add count
        driveCount++;
    }    
  #endif

    // start composing the reply
    // find <StorageFolder> type definition, don't bother checking the result as it exists for sure
    g_CLR_RT_TypeSystem.FindTypeDef( "StorageFolder", "Windows.Storage", storageFolderTypeDef );

    // create an array of <StorageFolder>
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( top, driveCount, storageFolderTypeDef ));

  #if (HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE)

    if(driveCount > 0)
    {
        // there are driver present and enumerated

        // get a pointer to the first object in the array (which is of type <StorageFolder>)
        storageFolder = (CLR_RT_HeapBlock*)top.DereferenceArray()->GetFirstElement();
        
        // loop until we've loaded all the possible drives
        // because we are iterating through an enum, need to use its integer values
        for(uint16_t driveIterator = 0; driveIterator < maxDriveCount; driveIterator++ )
        {
            // fill the folder name and path

          #if HAL_USE_SDC
            // SD card has index 0
            if(driveIterator == SD_CARD_DRIVE_INDEX_NUMERIC)
            {
                // is the SD card file system ready?
                if(sdCardFileSystemReady)
                {
                    memcpy(workingDrive, INDEX0_DRIVE_PATH, DRIVE_PATH_LENGTH);
                }
                else
                {
                    // SD card file system is not ready and/or no drive is enumerated
                    continue;
                }
            }
          #endif

          #if HAL_USBH_USE_MSD
            // USB MSD has index 0 or 1, depending on the us of SD card
            if(driveIterator == USB_MSD_DRIVE_INDEX_NUMERIC)
            {
                // is the USB mass storage device file system ready?
                if(usbMsdFileSystemReady)
                {
                    memcpy(workingDrive, INDEX1_DRIVE_PATH, DRIVE_PATH_LENGTH);
                }
                else
                {
                    // USB mass storage file system is not ready and/or no drive is enumerated
                    continue;
                }
            }
          #endif

            // create an instance of <StorageFolder>
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*storageFolder, storageFolderTypeDef));

            // dereference the object in order to reach its fields
            hbObj = storageFolder->Dereference();

            // set the managed fields
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___name ], workingDrive ));
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___path ], workingDrive ));

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
                // clear buffer
                memset(stringBuffer, 0, FF_LFN_BUF + 1);

                // read the drive volume label
                // don't bother checking the result, if anything goes wrong we'll end up with an empty string which is OK
                f_getlabel(workingDrive, stringBuffer, NULL);

                // add the driver letter separated it with an empty space, if the volume label isn't empty
                if(*stringBuffer != '\0')
                {
                    strcat(stringBuffer, " ");
                }
                strcat(stringBuffer, "(");
                strcat(stringBuffer, workingDrive);
                strcat(stringBuffer, ")");

                // set the field with the volume label
                NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___name ], stringBuffer ));


                // free stringBuffer
                platform_free(stringBuffer);
            }

            // move pointer to the next folder item
            storageFolder++;
        }
    }

  #endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_storage_native_Windows_Storage_StorageFolder::GetInternalStorageFoldersNative___SZARRAY_WindowsStorageStorageFolder(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();

    uint32_t driveCount = 0;
    char workingDrive[sizeof(DRIVE_PATH_LENGTH)];

    CLR_RT_HeapBlock* storageFolder;
    CLR_RT_TypeDef_Index storageFolderTypeDef;
    CLR_RT_HeapBlock* hbObj;
    CLR_RT_HeapBlock& top   = stack.PushValue();

  #if (USE_SPIFFS_FOR_STORAGE == TRUE)
    // is the SPIFFS file system available and mounted?
    if(spiffsFileSystemReady)
    {
        // add count
        driveCount++;
    }
  #endif

    // start composing the reply
    // find <StorageFolder> type definition, don't bother checking the result as it exists for sure
    g_CLR_RT_TypeSystem.FindTypeDef( "StorageFolder", "Windows.Storage", storageFolderTypeDef );

    // create an array of <StorageFolder>
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( top, driveCount, storageFolderTypeDef ));

    if(driveCount > 0)
    {
        // SPIFFS is mounted
        // currently there is support for a single SPIFFS instance

        // get a pointer to the first object in the array (which is of type <StorageFolder>)
        storageFolder = (CLR_RT_HeapBlock*)top.DereferenceArray()->GetFirstElement();

        // create an instance of <StorageFolder>
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*storageFolder, storageFolderTypeDef));

        // fill the folder name and path
        memcpy(workingDrive, INTERNAL_DRIVE_PATH, DRIVE_PATH_LENGTH);

        // dereference the object in order to reach its fields
        hbObj = storageFolder->Dereference();

        // set the managed fields
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___name ], workingDrive ));
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___path ], workingDrive ));
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_storage_native_Windows_Storage_StorageFolder::GetStorageFoldersNative___SZARRAY_WindowsStorageStorageFolder( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Index    storageFolderTypeDef;
    CLR_RT_HeapBlock*       storageFolder;
    CLR_RT_HeapBlock*       hbObj;
    SYSTEMTIME              fileInfoTime;

  #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))

    const char* workingPath;
    char workingDrive[DRIVE_LETTER_LENGTH];

    DIR             currentDirectory;
    FRESULT         operationResult;
    static FILINFO  fileInfo;
    uint16_t        directoryCount = 0;
    char*           stringBuffer = NULL;
    char*           workingBuffer = NULL;

    CLR_RT_HeapBlock& top   = stack.PushValue();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // copy the first 2 letters of the path for the drive
    // path is 'D:\folder\file.txt', so we need 'D:'
    memcpy(workingDrive, pThis[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___path ].DereferenceString()->StringText(), DRIVE_LETTER_LENGTH);

    // get a pointer to the path in managed field
    workingPath = pThis[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___path ].DereferenceString()->StringText();

    // open directory
    operationResult = f_opendir(&currentDirectory, workingPath);

    if(operationResult != FR_OK)
    {
        if(operationResult == FR_INVALID_DRIVE)
        {
            // failed to change drive

            // check if the working drive is the SPIFFS drive
          #if (USE_SPIFFS_FOR_STORAGE == TRUE)
            if(WORKING_DRIVE_IS_INTERNAL_DRIVE)
            {
                // this is the SPIFFS drive,
                // do nothing here and return an empty collection because folders aren't supported in SPIFFS
            }
            else
            {
          #else
            {
          #endif
                // invalid drive
                NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
            }
        }

        // error or directory empty
        // find <StorageFolder> type, don't bother checking the result as it exists for sure
        g_CLR_RT_TypeSystem.FindTypeDef( "StorageFolder", "Windows.Storage", storageFolderTypeDef );

        // create an array of <StorageFolder>
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( top, directoryCount, storageFolderTypeDef ));
    }
    else
    {
        // need to perform this in two steps
        // 1st: count the directory objects
        // 2nd: create the array items with each directory object

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
            // but skip if:
            // - has system attribute set
            // - has hidden attribute set
            if ((fileInfo.fattrib & AM_DIR) &&
                !(fileInfo.fattrib & AM_SYS) &&
                !(fileInfo.fattrib & AM_HID))
            {
                directoryCount++;
            }
        }

        // find <StorageFolder> type, don't bother checking the result as it exists for sure
        g_CLR_RT_TypeSystem.FindTypeDef( "StorageFolder", "Windows.Storage", storageFolderTypeDef );

        // create an array of <StorageFolder>
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( top, directoryCount, storageFolderTypeDef ));
        
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
                // but skip if:
                // - has system attribute set
                // - has hidden attribute set
                if ((fileInfo.fattrib & AM_DIR) &&
                    !(fileInfo.fattrib & AM_SYS) &&
                    !(fileInfo.fattrib & AM_HID))
                {
                    // create an instance of <StorageFolder>
                    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*storageFolder, storageFolderTypeDef));

                    // dereference the object in order to reach its fields
                    hbObj = storageFolder->Dereference();

                    // directory name
                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___name ], fileInfo.fname ));

                    // clear working buffer
                    memset(workingBuffer, 0, 2 * FF_LFN_BUF + 1);

                    // compose directory path
                    CombinePath(workingBuffer, workingPath, fileInfo.fname);
                    
                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___path ], workingBuffer ));
                    
                    // compute directory date
                    fileInfoTime = GetDateTime(fileInfo.fdate, fileInfo.ftime);

                    // get a reference to the dateCreated managed field...
                    CLR_RT_HeapBlock& dateFieldRef = hbObj[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___dateCreated ];
                    CLR_INT64* pRes = (CLR_INT64*)&dateFieldRef.NumericByRef().s8;
                    // ...and set it with the fileInfoTime
                    *pRes = HAL_Time_ConvertFromSystemTime( &fileInfoTime );

                    // move the storage folder pointer to the next item in the array
                    storageFolder++;
                }
            }
        }
    }

  #elif (USE_SPIFFS_FOR_STORAGE == TRUE)

    // SPIFFS doesn't support folders so return an empty collection
    CLR_RT_HeapBlock& top   = stack.PushValue();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // find <StorageFolder> type, don't bother checking the result as it exists for sure
    g_CLR_RT_TypeSystem.FindTypeDef( "StorageFolder", "Windows.Storage", storageFolderTypeDef );

    // create an array of <StorageFolder>
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( top, directoryCount, storageFolderTypeDef ));

  #endif

    NANOCLR_CLEANUP();

  #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))
    // close directory
    f_closedir(&currentDirectory);

    // free buffers memory, if allocated
    if(stringBuffer != NULL)
    {
        platform_free(stringBuffer);
    }
    if(workingBuffer != NULL)
    {
        platform_free(workingBuffer);
    }
  #endif

    NANOCLR_CLEANUP_END();
}

HRESULT Library_win_storage_native_Windows_Storage_StorageFolder::GetStorageFilesNative___SZARRAY_WindowsStorageStorageFile__U4__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Index    storageFileTypeDef;
    CLR_RT_HeapBlock*       storageFile;
    CLR_RT_HeapBlock*       hbObj;
    SYSTEMTIME              fileInfoTime;

    const char* workingPath;
    char workingDrive[DRIVE_LETTER_LENGTH];

    uint32_t startIndex;
    uint32_t maxItemsToRetrieve;
    uint32_t itemIndex = 0;

  #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))

    DIR             currentDirectory;
    FRESULT         operationResult;
    static FILINFO  fileInfo;
    uint16_t        fileCount = 0;
    char*           stringBuffer = NULL;
    char*           workingBuffer = NULL;

    CLR_RT_HeapBlock& top   = stack.PushValue();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // get start index
    startIndex = stack.Arg1().NumericByRef().u4;

    // get max items to retrieve
    maxItemsToRetrieve = stack.Arg2().NumericByRef().u4;

    // copy the first 2 letters of the path for the drive
    // path is 'D:\folder\file.txt', so we need 'D:'
    memcpy(workingDrive, pThis[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___path ].DereferenceString()->StringText(), DRIVE_LETTER_LENGTH);
        
    // get a pointer to the path in managed field
    workingPath = pThis[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___path ].DereferenceString()->StringText();

    // change drive
    if(f_chdrive(workingDrive) == FR_OK)
    {
        // open directory
        operationResult = f_opendir(&currentDirectory, workingPath);

        if(operationResult != FR_OK)
        {
            // error or directory empty
            // find <StorageFile> type, don't bother checking the result as it exists for sure
            g_CLR_RT_TypeSystem.FindTypeDef( "StorageFile", "Windows.Storage", storageFileTypeDef );

            // create an array of <StorageFile>
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( top, fileCount, storageFileTypeDef ));
        }
        else
        {
            // need to perform this in two steps
            // 1st: count the file objects
            // 2nd: create the array items with each file object

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

                // check if this is a file
                // but skip if:
                // - has system attribute set
                // - has hidden attribute set
                if ((fileInfo.fattrib & AM_ARC) &&
                    !(fileInfo.fattrib & AM_SYS) &&
                    !(fileInfo.fattrib & AM_HID))
                {
                    // check if this file is within the requested parameters
                    if( (itemIndex >= startIndex) &&
                        (fileCount < maxItemsToRetrieve))
                    {
                        fileCount++;
                    }

                    itemIndex++;
                }
            }

            // find <StorageFile> type, don't bother checking the result as it exists for sure
            g_CLR_RT_TypeSystem.FindTypeDef( "StorageFile", "Windows.Storage", storageFileTypeDef );

            // create an array of <StorageFile>
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( top, fileCount, storageFileTypeDef ));
            
            // get a pointer to the first object in the array (which is of type <StorageFile>)
            storageFile = (CLR_RT_HeapBlock*)top.DereferenceArray()->GetFirstElement();

            if(fileCount > 0)
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

                // and reset the file iterator vars too
                itemIndex = 0;
                fileCount = 0;

                
                for (;;)
                {
                    // read next directory item
                    operationResult = f_readdir(&currentDirectory, &fileInfo);
                    
                    // break on error or at end of dir
                    if (operationResult != FR_OK || fileInfo.fname[0] == 0)
                    {
                        break;
                    }

                    // check if this is a file
                    // but skip if:
                    // - has system attribute set
                    // - has hidden attribute set
                    if ((fileInfo.fattrib & AM_ARC) &&
                        !(fileInfo.fattrib & AM_SYS) &&
                        !(fileInfo.fattrib & AM_HID))
                    {
                        // check if this file is within the requested parameters
                        if( (itemIndex >= startIndex) &&
                            (fileCount < maxItemsToRetrieve))
                        {
                            // create an instance of <StorageFile>
                            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*storageFile, storageFileTypeDef));

                            // dereference the object in order to reach its fields
                            hbObj = storageFile->Dereference();

                            // file name
                            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___name ], fileInfo.fname ));

                            // clear working buffer
                            memset(workingBuffer, 0, 2 * FF_LFN_BUF + 1);

                            // compose file path
                            CombinePath(workingBuffer, workingPath, fileInfo.fname);

                            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___path ], workingBuffer ));

                            // compute directory date
                            fileInfoTime = GetDateTime(fileInfo.fdate, fileInfo.ftime);

                            // get a reference to the dateCreated managed field...
                            CLR_RT_HeapBlock& dateFieldRef = hbObj[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___dateCreated ];
                            CLR_INT64* pRes = (CLR_INT64*)&dateFieldRef.NumericByRef().s8;
                            // ...and set it with the fileInfoTime
                            *pRes = HAL_Time_ConvertFromSystemTime( &fileInfoTime );

                            // move the storage folder pointer to the next item in the array
                            storageFile++;

                            // update iterator var
                            fileCount++;
                        }

                        // update iterator var
                        itemIndex++;
                    }
                }
            }
            else
            {
                // empty directory
                // find <StorageFile> type, don't bother checking the result as it exists for sure
                g_CLR_RT_TypeSystem.FindTypeDef( "StorageFile", "Windows.Storage", storageFileTypeDef );

                // create an array of <StorageFile>
                NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( top, fileCount, storageFileTypeDef ));
            }
        }
    }
    else
    {
        // check if the working drive is the SPIFFS drive and...
        // ... that the path is the drive root (because SPIFFS doesn't support folders)
      #if (USE_SPIFFS_FOR_STORAGE == TRUE)
        if( (WORKING_DRIVE_IS_INTERNAL_DRIVE) &&
            (hal_strlen_s(workingPath) == DRIVE_PATH_LENGTH - 1))
        {
            // this is the SPIFFS drive
            spiffs_DIR drive;
            struct spiffs_dirent e;
            struct spiffs_dirent *pe = &e;

            // need to perform this in two steps
            // 1st: count the file objects
            // 2nd: create the array items with each file object

            // perform 1st pass
            SPIFFS_opendir(&fs, "/", &drive);
            while ((pe = SPIFFS_readdir(&drive, pe)))
            {
                fileCount++;
            }

            // have to close dir
            SPIFFS_closedir(&drive);

            // find <StorageFile> type, don't bother checking the result as it exists for sure
            g_CLR_RT_TypeSystem.FindTypeDef( "StorageFile", "Windows.Storage", storageFileTypeDef );

            // create an array of <StorageFile>
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( top, fileCount, storageFileTypeDef ));
            
            // get a pointer to the first object in the array (which is of type <StorageFile>)
            storageFile = (CLR_RT_HeapBlock*)top.DereferenceArray()->GetFirstElement();

            if(fileCount > 0)
            {
                // allocate memory for buffer
                workingBuffer = (char*)platform_malloc(2 * FF_LFN_BUF + 1);

                // sanity check for successfull malloc
                if(workingBuffer == NULL)
                {
                    // failed to allocate memory
                    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
                }

                // perform 2nd pass
                // need to open the directory again
                SPIFFS_opendir(&fs, "/", &drive);

                // and reset the file iterator vars too
                itemIndex = 0;
                fileCount = 0;
                pe = &e;
                
                while ((pe = SPIFFS_readdir(&drive, pe)))
                {
                    // create an instance of <StorageFile>
                    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*storageFile, storageFileTypeDef));

                    // dereference the object in order to reach its fields
                    hbObj = storageFile->Dereference();

                    // file name
                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___name ], (const char*)pe->name ));

                    // clear working buffer
                    memset(workingBuffer, 0, 2 * FF_LFN_BUF + 1);

                    // compose file path
                    CombinePath(workingBuffer, workingPath, (const char*)pe->name);

                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___path ], workingBuffer ));

                    // SPIFFS files don't have date information

                    // move the storage folder pointer to the next item in the array
                    storageFile++;
                }

                // done here, close dir
                SPIFFS_closedir(&drive);
            }
        }
        else
        {
      #else
        {
      #endif
            // failed to change drive
            NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
        }
    }

  #elif (USE_SPIFFS_FOR_STORAGE == TRUE)

    CLR_RT_HeapBlock& top   = stack.PushValue();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    spiffs_DIR drive;
    struct spiffs_dirent e;
    struct spiffs_dirent *pe = &e;

    // need to perform this in two steps
    // 1st: count the file objects
    // 2nd: create the array items with each file object

    // perform 1st pass
    SPIFFS_opendir(&fs, "/", &drive);
    while ((pe = SPIFFS_readdir(&drive, pe)))
    {
        fileCount++;
    }

    // have to close dir
    SPIFFS_closedir(&drive);

    // find <StorageFile> type, don't bother checking the result as it exists for sure
    g_CLR_RT_TypeSystem.FindTypeDef( "StorageFile", "Windows.Storage", storageFileTypeDef );

    // create an array of <StorageFile>
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( top, fileCount, storageFileTypeDef ));
    
    // get a pointer to the first object in the array (which is of type <StorageFile>)
    storageFile = (CLR_RT_HeapBlock*)top.DereferenceArray()->GetFirstElement();

    if(fileCount > 0)
    {
        // allocate memory for buffer
        workingBuffer = (char*)platform_malloc(2 * FF_LFN_BUF + 1);

        // sanity check for successfull malloc
        if(workingBuffer == NULL)
        {
            // failed to allocate memory
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        // perform 2nd pass
        // need to open the directory again
        SPIFFS_opendir(&fs, "/", &drive);

        // and reset the file iterator vars too
        itemIndex = 0;
        fileCount = 0;
        
        while ((pe = SPIFFS_readdir(&drive, pe)))
        {
            // create an instance of <StorageFile>
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*storageFile, storageFileTypeDef));

            // dereference the object in order to reach its fields
            hbObj = storageFile->Dereference();

            // file name
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___name ], (const char*)pe->name ));

            // clear working buffer
            memset(workingBuffer, 0, 2 * FF_LFN_BUF + 1);

            // compose file path
            CombinePath(workingBuffer, workingPath, (const char*)pe->name);

            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___path ], workingBuffer ));

            // SPIFFS files don't have date information

            // move the storage folder pointer to the next item in the array
            storageFile++;
        }

        // done here, close dir
        SPIFFS_closedir(&drive);
    }
  #endif

    NANOCLR_CLEANUP();

  #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))
    // close directory
    f_closedir(&currentDirectory);
  #endif

    // free buffers memory, if allocated
    if(stringBuffer != NULL)
    {
        platform_free(stringBuffer);
    }
    if(workingBuffer != NULL)
    {
        platform_free(workingBuffer);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_win_storage_native_Windows_Storage_StorageFolder::CreateFileNative___WindowsStorageStorageFile__STRING__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Index    storageFileTypeDef;
    CLR_RT_HeapBlock*       storageFile;

    const char* fileName;
    const char* workingPath;
    char workingDrive[DRIVE_LETTER_LENGTH];

    CreationCollisionOption options;

  #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))

    FIL             file;
    static FILINFO  fileInfo;
    SYSTEMTIME      fileInfoTime;    
    FRESULT         operationResult;
    uint8_t         modeFlags = 0;
    char*           filePath = NULL;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // copy the first 2 letters of the path for the drive
    // path is 'D:\folder\file.txt', so we need 'D:'
    memcpy(workingDrive, pThis[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___path ].DereferenceString()->StringText(), DRIVE_LETTER_LENGTH);

    // get creation collision options
    options = (CreationCollisionOption)stack.Arg2().NumericByRef().u4;
        
    // get a pointer to the StorageFolder path in managed field
    workingPath = pThis[ Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___path ].DereferenceString()->StringText();

    // get a pointer to the desired file name
    fileName = stack.Arg1().DereferenceString()->StringText();

    // setup file path
    filePath = (char*)platform_malloc(2 * FF_LFN_BUF + 1);

    // sanity check for successfull malloc
    if(filePath == NULL)
    {
        // failed to allocate memory
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // clear working buffer
    memset(filePath, 0, 2 * FF_LFN_BUF + 1);

    // compose file path
    CombinePath(filePath, workingPath, fileName);

    // change directory
    operationResult = f_chdir(workingPath);

    if(operationResult != FR_OK)
    {
        if(operationResult == FR_INVALID_DRIVE)
        {
           // check if the working drive is the SPIFFS drive
          #if (USE_SPIFFS_FOR_STORAGE == TRUE)
            if(WORKING_DRIVE_IS_INTERNAL_DRIVE)
            {
                // this is the SPIFFS drive
                // proceed to create the file
                
                // compute mode flags from CreationCollisionOption
                switch (options)
                {
                    case CreationCollisionOption_ReplaceExisting:
                        modeFlags = SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR;
                        break;

                    case CreationCollisionOption_FailIfExists:
                        modeFlags = SPIFFS_RDWR;
                        break;

                    case CreationCollisionOption_OpenIfExists:
                        modeFlags = SPIFFS_CREAT | SPIFFS_RDWR;
                        break;
                
                    case CreationCollisionOption_GenerateUniqueName:
                        // this operation is not supported in nanoFramework
                        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                        break;

                    default:
                        break;
                }

                spiffs_file fd = SPIFFS_open(&fs, fileName, modeFlags, 0);
                if (fd < 0) 
                {
                    int32_t error = SPIFFS_errno(&fs);

                    // process operation result according to creation options
                    if( (error == SPIFFS_ERR_FILE_EXISTS) &&
                        (options == CreationCollisionOption_FailIfExists))
                    {
                        // file already exists
                        NANOCLR_SET_AND_LEAVE(CLR_E_PATH_ALREADY_EXISTS);
                    }
                    if(options == CreationCollisionOption_OpenIfExists)
                    {
                        // file doesn't exist
                        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
                    }

                    // failed to create the file
                    NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
                }
                else
                {
                    // file created (or opened) succesfully
                    // OK to close it
                    SPIFFS_close(&fs, fd);

                    // compose return object
                    // find <StorageFile> type, don't bother checking the result as it exists for sure
                    g_CLR_RT_TypeSystem.FindTypeDef( "StorageFile", "Windows.Storage", storageFileTypeDef );

                    // create a <StorageFile>
                    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(stack.PushValue(), storageFileTypeDef));
                    
                    // get a handle to the storage file
                    storageFile = stack.TopValue().Dereference();

                    // file name
                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( storageFile[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___name ], fileName ));

                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( storageFile[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___path ], filePath ));

                    // SPIFFS don't have date time details, so we are done here
                }          
            }
            else
            {
          #else
            {
          #endif
                // invalid drive
                NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
            }            
        }
        else
        {
            // error opening the directoty
            NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
        }
    }
    else
    {
        // compute mode flags from CreationCollisionOption
        switch (options)
        {
            case CreationCollisionOption_ReplaceExisting:
                modeFlags = FA_CREATE_ALWAYS;
                break;

            case CreationCollisionOption_FailIfExists:
                modeFlags = FA_CREATE_NEW;
                break;

            case CreationCollisionOption_OpenIfExists:
                modeFlags = FA_OPEN_EXISTING;
                break;
        
            case CreationCollisionOption_GenerateUniqueName:
                // this operation is not supported in nanoFramework
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;

            default:
                break;
        }

        // open file
        operationResult = f_open(&file, filePath, modeFlags);

        // process operation result according to creation options
        if( (operationResult == FR_EXIST) &&
            (options == CreationCollisionOption_FailIfExists))
        {
            // file already exists
            NANOCLR_SET_AND_LEAVE(CLR_E_PATH_ALREADY_EXISTS);
        }
        if( (operationResult == FR_NO_FILE) &&
            (options == CreationCollisionOption_OpenIfExists))
        {
            // file doesn't exist
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
        }

        if(operationResult == FR_OK)
        {
            // file created (or opened) succesfully
            // OK to close it
            f_close(&file);
            
            // now get the details
            f_stat(fileName, &fileInfo);

            // compose return object
            // find <StorageFile> type, don't bother checking the result as it exists for sure
            g_CLR_RT_TypeSystem.FindTypeDef( "StorageFile", "Windows.Storage", storageFileTypeDef );

            // create a <StorageFile>
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(stack.PushValue(), storageFileTypeDef));
            
            // get a handle to the storage file
            storageFile = stack.TopValue().Dereference();

            // file name
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( storageFile[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___name ], fileName ));

            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( storageFile[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___path ], filePath ));

            // get the date time details and fill in the managed field
            // compute directory date
            fileInfoTime = GetDateTime(fileInfo.fdate, fileInfo.ftime);

            // get a reference to the dateCreated managed field...
            CLR_RT_HeapBlock& dateFieldRef = storageFile[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___dateCreated ];
            CLR_INT64* pRes = (CLR_INT64*)&dateFieldRef.NumericByRef().s8;
            // ...and set it with the fileInfoTime
            *pRes = HAL_Time_ConvertFromSystemTime( &fileInfoTime );
        }
        else
        {
            // failed to create the file
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
        }
    }

  #elif (USE_SPIFFS_FOR_STORAGE == TRUE)

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // copy the first 2 letters of the path for the drive
    // path is 'D:\folder\file.txt', so we need 'D:'
    memcpy(workingDrive, pThis[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___path ].DereferenceString()->StringText(), DRIVE_LETTER_LENGTH);

    // get creation collision options
    options = (CreationCollisionOption)stack.Arg2().NumericByRef().u4;
        
    // get a pointer to the StorageFolder path in managed field
    workingPath = pThis[ Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___path ].DereferenceString()->StringText();

    // get a pointer to the desired file name
    fileName = stack.Arg1().DereferenceString()->StringText();

    // setup file path
    filePath = (char*)platform_malloc(2 * FF_LFN_BUF + 1);

    // sanity check for successfull malloc
    if(filePath == NULL)
    {
        // failed to allocate memory
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // clear working buffer
    memset(filePath, 0, 2 * FF_LFN_BUF + 1);

    // compose file path
    CombinePath(filePath, workingPath, fileName);
               
    // compute mode flags from CreationCollisionOption
    switch (options)
    {
        case CreationCollisionOption_ReplaceExisting:
            modeFlags = SPIFFS_CREAT | SPIFFS_TRUNC;
            break;

        case CreationCollisionOption_FailIfExists:
            modeFlags = 0;
            break;

        case CreationCollisionOption_OpenIfExists:
            modeFlags = SPIFFS_CREAT;
            break;
    
        case CreationCollisionOption_GenerateUniqueName:
            // this operation is not supported in nanoFramework
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;

        default:
            break;
    }

    spiffs_file fd = SPIFFS_open(&fs, filePath, modeFlags, 0);
    if (fd < 0) 
    {
        int32_t error = SPIFFS_errno(&fs);

        // process operation result according to creation options
        if( (error == SPIFFS_ERR_FILE_EXISTS) &&
            (options == CreationCollisionOption_FailIfExists))
        {
            // file already exists
            NANOCLR_SET_AND_LEAVE(CLR_E_PATH_ALREADY_EXISTS);
        }
        if(options == CreationCollisionOption_OpenIfExists)
        {
            // file doesn't exist
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
        }

        // failed to create the file
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }
    else
    {
        // file created (or opened) succesfully
        // OK to close it
        SPIFFS_close(&fs, fd);

        // compose return object
        // find <StorageFile> type, don't bother checking the result as it exists for sure
        g_CLR_RT_TypeSystem.FindTypeDef( "StorageFile", "Windows.Storage", storageFileTypeDef );

        // create a <StorageFile>
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(stack.PushValue(), storageFileTypeDef));
        
        // get a handle to the storage file
        storageFile = stack.TopValue().Dereference();

        // file name
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( storageFile[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___name ], fileName ));

        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( storageFile[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___path ], filePath ));

        // SPIFFS don't have date time details, so we are done here
    }
  #endif

    NANOCLR_CLEANUP();

    // free buffer memory, if allocated
    if(filePath != NULL)
    {
        platform_free(filePath);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_win_storage_native_Windows_Storage_StorageFolder::CreateFolderNative___WindowsStorageStorageFolder__STRING__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Index    storageFolderTypeDef;
    CLR_RT_HeapBlock*       storageFolder;

    const char* folderName;
    const char* workingPath;
    char workingDrive[DRIVE_LETTER_LENGTH];

    CreationCollisionOption options;

  #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))

    static FILINFO  fileInfo;
    SYSTEMTIME      fileInfoTime;    
    FRESULT         operationResult;
    char*           folderPath = NULL;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // copy the first 2 letters of the path for the drive
    // path is 'D:\folder\file.txt', so we need 'D:'
    memcpy(workingDrive, pThis[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___path ].DereferenceString()->StringText(), DRIVE_LETTER_LENGTH);

    // get creation collision options
    options = (CreationCollisionOption)stack.Arg2().NumericByRef().u4;
        
    // get a pointer to the path in managed field
    workingPath = pThis[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___path ].DereferenceString()->StringText();

    // get a pointer to the desired folder name
    folderName = stack.Arg1().DereferenceString()->StringText();

    folderPath = (char*)platform_malloc(2 * FF_LFN_BUF + 1);

    // sanity check for successfull malloc
    if(folderPath == NULL)
    {
        // failed to allocate memory
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // clear working buffer
    memset(folderPath, 0, 2 * FF_LFN_BUF + 1);

    // compose folder path
    CombinePath(folderPath, workingPath, folderName);
    
    // check if folder exists
    operationResult = f_stat(folderPath, &fileInfo);

    // folder doesn't exist
    if (operationResult == FR_NO_FILE)
    {
        if (options == CreationCollisionOption_OpenIfExists)
        {
            // folder doesn't exist
            NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
        }
        else 
        {
            // create directory
            operationResult = f_mkdir(folderPath);

            if(operationResult == FR_OK)
            {
                operationResult = f_stat(folderPath, &fileInfo);              
            }
            else
            {
                // failed to create the folder
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
            }
        }
    }
    else 
    {
        if (options == CreationCollisionOption_FailIfExists)
        {
            // folder already exists
            NANOCLR_SET_AND_LEAVE(CLR_E_PATH_ALREADY_EXISTS);
        }
    }

    if(operationResult == FR_OK)
    {
        // compose return object
        // find <StorageFolder> type, don't bother checking the result as it exists for sure
        g_CLR_RT_TypeSystem.FindTypeDef( "StorageFolder", "Windows.Storage", storageFolderTypeDef );

        // create a <StorageFolder>
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(stack.PushValue(), storageFolderTypeDef));
        
        // get a handle to the storage folder
        storageFolder = stack.TopValue().Dereference();

        // folder name
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( storageFolder[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___name ], folderName ));

        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( storageFolder[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___path ], folderPath ));

        // get the date time details and fill in the managed field
        // compute directory date
        fileInfoTime = GetDateTime(fileInfo.fdate, fileInfo.ftime);

        // get a reference to the dateCreated managed field...
        CLR_RT_HeapBlock& dateFieldRef = storageFolder[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___dateCreated ];
        CLR_INT64* pRes = (CLR_INT64*)&dateFieldRef.NumericByRef().s8;
        // ...and set it with the fileInfoTime
        *pRes = HAL_Time_ConvertFromSystemTime( &fileInfoTime );
    }
    else
    {
        // failed to get folder details
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }
    
  #elif (USE_SPIFFS_FOR_STORAGE == TRUE)

    // throw not supported exception because folders aren't supported in SPIFFS
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

  #endif

    NANOCLR_CLEANUP();

    // free buffer memory, if allocated
    if(folderPath != NULL)
    {
        platform_free(folderPath);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_win_storage_native_Windows_Storage_StorageFolder::DeleteFolderNative___VOID(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();

	const char* workingPath;
    // need extra room for the change dir command
    char workingDrive[DRIVE_LETTER_LENGTH + 2];

  #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))

	FRESULT     operationResult;

	// get a pointer to the managed object instance and check that it's not NULL
	CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

	// get a pointer to the path in managed field
	workingPath = pThis[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___path].DereferenceString()->StringText();

	// remove folder
	operationResult = f_unlink(workingPath);
	if (operationResult == FR_INVALID_NAME)
	{
		// Invalid path
		NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
	}
	else if (operationResult == FR_DENIED)
	{
        // this could be because the folder is not empty or because it's the current folder
        
        // change directory to the parent directory using f_chdir("2:..")

        // copy the first 2 letters of the path for the drive
        // path is 'D:\folder\file.txt', so we need 'D:'
        memcpy(workingDrive, pThis[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___path ].DereferenceString()->StringText(), DRIVE_LETTER_LENGTH);
        // make sure there is a terminator
        workingDrive[2] = '\0';
        strcat(workingDrive, "..");

        // change dir to parent
        operationResult = f_chdir(workingDrive);

        // try remove again
        operationResult = f_unlink(workingPath);

        if (operationResult == FR_DENIED)
        {
            // folder not empty
            NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_EMPTY);
        }
        else if (operationResult != FR_OK)
        {
            // something else is failing
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
        }
	}
	else if (operationResult != FR_OK)
	{
		// folder doesn't exist
      #if (USE_SPIFFS_FOR_STORAGE == TRUE)
        if(WORKING_DRIVE_IS_INTERNAL_DRIVE)
        {
            // this is the SPIFFS drive
            // throw not supported exception because folders aren't supported in SPIFFS
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }
        else
        {
      #else
        {
      #endif
            NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
        }
	}

  #elif (USE_SPIFFS_FOR_STORAGE == TRUE)

    // throw not supported exception because folders aren't supported in SPIFFS
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

  #endif

	NANOCLR_CLEANUP();

	NANOCLR_CLEANUP_END();
}

HRESULT Library_win_storage_native_Windows_Storage_StorageFolder::RenameFolderNative___VOID__STRING(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();

	const char* workingPath;
	const char* desiredPath;
    char workingDrive[DRIVE_LETTER_LENGTH];

  #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))

	FRESULT     operationResult;

	// get a pointer to the managed object instance and check that it's not NULL
	CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // copy the first 2 letters of the path for the drive
    // path is 'D:\folder\file.txt', so we need 'D:'
    memcpy(workingDrive, pThis[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___path ].DereferenceString()->StringText(), DRIVE_LETTER_LENGTH);

	// get a pointer to the path in managed field
	workingPath = pThis[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___path].DereferenceString()->StringText();

	// get a pointer to the desired folder name
	desiredPath = stack.Arg1().DereferenceString()->StringText();

	// rename folder
	operationResult = f_rename(workingPath, desiredPath);
	if (operationResult == FR_INVALID_NAME)
	{
		// invalid path
		NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
	}
	else if (operationResult != FR_OK)
	{
		// folder doesn't exist
      #if (USE_SPIFFS_FOR_STORAGE == TRUE)
        if(WORKING_DRIVE_IS_INTERNAL_DRIVE)
        {
            // this is the SPIFFS drive
            // throw not supported exception because folders aren't supported in SPIFFS
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }
        else
        {
      #else
        {
      #endif
            NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
        }
	}

  #elif (USE_SPIFFS_FOR_STORAGE == TRUE)

    // throw not supported exception because folders aren't supported in SPIFFS
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

  #endif

	NANOCLR_CLEANUP();

	NANOCLR_CLEANUP_END();
}

HRESULT Library_win_storage_native_Windows_Storage_StorageFolder::GetFolderNative___WindowsStorageStorageFolder__STRING( CLR_RT_StackFrame& stack )
{
	NANOCLR_HEADER();

	CLR_RT_TypeDef_Index    storageFolderTypeDef;
	CLR_RT_HeapBlock*       storageFolder;

	const char* folderName;
	const char* workingPath;
	char*       folderPath = NULL;

  #if (USE_SPIFFS_FOR_STORAGE == TRUE)
    char workingDrive[DRIVE_LETTER_LENGTH];
  #endif

  #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))

	FILINFO     fileInfo;
	SYSTEMTIME  fileInfoTime;

	FRESULT     operationResult;
	
	CLR_INT64*  pRes;

	// get a pointer to the managed object instance and check that it's not NULL
	CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

	// get a pointer to the path in managed field
	workingPath = pThis[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___path].DereferenceString()->StringText();

	// get a pointer to the desired folder name
	folderName = stack.Arg1().DereferenceString()->StringText();

	folderPath = (char*)platform_malloc(2 * FF_LFN_BUF + 1);

	// sanity check for successfull malloc
	if (folderPath == NULL)
	{
		// failed to allocate memory
		NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
	}

	// clear working buffer
	memset(folderPath, 0, 2 * FF_LFN_BUF + 1);

	// compose folder path
    CombinePath(folderPath, workingPath, folderName);

	// check if directory exists
	operationResult = f_stat(folderPath, &fileInfo);
	if (operationResult != FR_OK)
	{
		// folder doesn't exist
      #if (USE_SPIFFS_FOR_STORAGE == TRUE)
        if(WORKING_DRIVE_IS_INTERNAL_DRIVE)
        {
            // this is the SPIFFS drive
            // throw not supported exception because folders aren't supported in SPIFFS
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }
        else
        {
      #else
        {
      #endif
            NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
        }
	}
    else
    {
        // is this a file?
        if ( !(fileInfo.fattrib & AM_DIR) )
        {
            // Path represents a file
            NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
        }
        else
        {
            // compose return object
            // find <StorageFolder> type, don't bother checking the result as it exists for sure
            g_CLR_RT_TypeSystem.FindTypeDef("StorageFolder", "Windows.Storage", storageFolderTypeDef);

            // create a <StorageFolder>
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(stack.PushValue(), storageFolderTypeDef));

            // get a handle to the storage folder
            storageFolder = stack.TopValue().Dereference();

            // folder name
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(storageFolder[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___name], folderName));

            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(storageFolder[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___path], folderPath));

            // get the date time details and fill in the managed field
            // compute directory date
            fileInfoTime = GetDateTime(fileInfo.fdate, fileInfo.ftime);

            // get a reference to the dateCreated managed field...
            CLR_RT_HeapBlock& dateFieldRef = storageFolder[Library_win_storage_native_Windows_Storage_StorageFolder::FIELD___dateCreated];
            pRes = (CLR_INT64*)&dateFieldRef.NumericByRef().s8;
            // ...and set it with the fileInfoTime
            *pRes = HAL_Time_ConvertFromSystemTime(&fileInfoTime);
        }
    }

  #elif (USE_SPIFFS_FOR_STORAGE == TRUE)

    // throw not supported exception because folders aren't supported in SPIFFS
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

  #endif

	NANOCLR_CLEANUP();

	// free buffer memory, if allocated
	if (folderPath != NULL)
	{
		platform_free(folderPath);
	}

	NANOCLR_CLEANUP_END();
}
