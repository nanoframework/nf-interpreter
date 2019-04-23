//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ff.h>
#include "win_storage_native.h"
#include <target_windows_storage_config.h>
#include <Target_Windows_Storage.h>
#include <nanoHAL_Windows_Storage.h>

#if HAL_USBH_USE_MSD
#include "usbh/dev/msd.h"
#endif

// flags for file system ready
#if HAL_USE_SDC
extern bool sdCardFileSystemReady;
#endif
#if HAL_USBH_USE_MSD
extern bool usbMsdFileSystemReady;
#endif

// defining these types here to make it shorter and improve code readability
typedef Library_win_storage_native_Windows_Storage_StorageFolder StorageFolder;
typedef Library_win_storage_native_Windows_Storage_StorageFile StorageFile;

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

HRESULT StorageFolder::GetRemovableStorageFoldersNative___SZARRAY_WindowsStorageStorageFolder( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    char* stringBuffer;
    uint32_t driveCount = 0;
    char workingDrive[sizeof(DRIVE_PATH_LENGTH)];
    uint16_t driveIterator = 0;

    CLR_RT_HeapBlock* storageFolder;
    CLR_RT_TypeDef_Index storageFolderTypeDef;
    CLR_RT_HeapBlock* hbObj;
    CLR_RT_HeapBlock& top   = stack.PushValue();

    //  default to NULL (which is the expected outcome when no devices are connected)
    hbObj = top.Dereference();
    hbObj->SetObjectReference( NULL );

  #if HAL_USE_SDC
    // is the SD card file system ready?
    if(sdCardFileSystemReady)
    {
        // add count
        driveCount++;
    }
  #endif

  #if HAL_USBH_USE_MSD
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

    if(driveCount > 0)
    {
        // there are driver present and enumerated

        // get a pointer to the first object in the array (which is of type <StorageFolder>)
        storageFolder = (CLR_RT_HeapBlock*)top.DereferenceArray()->GetFirstElement();

        // create an instance of <StorageFolder>
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*storageFolder, storageFolderTypeDef));

        // loop until we've loaded all the possible drives
        // because we are iterating through an enum, need to use its integer values
        for(; driveIterator < driveCount; driveIterator++ )
        {
            // fill the folder name and path
            switch(driveIterator)
            {
                case 0:
                    memcpy(workingDrive, INDEX0_DRIVE_PATH, DRIVE_PATH_LENGTH);
                    break;

                case 1:
                    memcpy(workingDrive, INDEX1_DRIVE_PATH, DRIVE_PATH_LENGTH);
                    break;

                default:
                    // shouldn't reach here
                    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
                    break;
            }

            // dereference the object in order to reach its fields
            hbObj = storageFolder->Dereference();

            // set the managed fields
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[ StorageFolder::FIELD___name ], workingDrive ));
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[ StorageFolder::FIELD___path ], workingDrive ));

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
                NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[ StorageFolder::FIELD___name ], stringBuffer ));


                // free stringBuffer
                platform_free(stringBuffer);
            }

            // move pointer to the next folder item
            storageFolder++;
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_storage_native_Windows_Storage_StorageFolder::GetInternalStorageFoldersNative___SZARRAY_WindowsStorageStorageFolder(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER(); 
 
    (void)stack; 
 
    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub()); 
 
    NANOCLR_NOCLEANUP(); 
}

HRESULT StorageFolder::GetStorageFoldersNative___SZARRAY_WindowsStorageStorageFolder( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Index    storageFolderTypeDef;
    CLR_RT_HeapBlock*       storageFolder;
    CLR_RT_HeapBlock*       hbObj;
    SYSTEMTIME              fileInfoTime;

    const char* workingPath;

    DIR             currentDirectory;
    FRESULT         operationResult;
    static FILINFO  fileInfo;
    uint16_t        directoryCount = 0;
    char*           stringBuffer = NULL;
    char*           workingBuffer = NULL;

    CLR_RT_HeapBlock& top   = stack.PushValue();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
    // get a pointer to the path in managed field
    workingPath = pThis[ StorageFolder::FIELD___path ].DereferenceString()->StringText();

    // open directory
    operationResult = f_opendir(&currentDirectory, workingPath);

    if(operationResult != FR_OK)
    {
        if(operationResult == FR_INVALID_DRIVE)
        {
            // failed to change drive
            NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
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
                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[ StorageFolder::FIELD___name ], fileInfo.fname ));

                    // clear working buffer
                    memset(workingBuffer, 0, 2 * FF_LFN_BUF + 1);

                    // compose directory path
                    strcat(workingBuffer, workingPath);
                    strcat(workingBuffer, fileInfo.fname);
                    
                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[ StorageFolder::FIELD___path ], workingBuffer ));
                    
                    // compute directory date
                    fileInfoTime = GetDateTime(fileInfo.fdate, fileInfo.ftime);

                    // get a reference to the dateCreated managed field...
                    CLR_RT_HeapBlock& dateFieldRef = hbObj[ StorageFolder::FIELD___dateCreated ];
                    CLR_INT64* pRes = (CLR_INT64*)&dateFieldRef.NumericByRef().s8;
                    // ...and set it with the fileInfoTime
                    *pRes = HAL_Time_ConvertFromSystemTime( &fileInfoTime );

                    // move the storage folder pointer to the next item in the array
                    storageFolder++;
                }
            }
        }
    }

    NANOCLR_CLEANUP();

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

    NANOCLR_CLEANUP_END();
}

HRESULT StorageFolder::GetStorageFilesNative___SZARRAY_WindowsStorageStorageFile__U4__U4( CLR_RT_StackFrame& stack )
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
    memcpy(workingDrive, pThis[ StorageFolder::FIELD___path ].DereferenceString()->StringText(), DRIVE_LETTER_LENGTH);
        
    // get a pointer to the path in managed field
    workingPath = pThis[ StorageFolder::FIELD___path ].DereferenceString()->StringText();

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
                            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[ StorageFile::FIELD___name ], fileInfo.fname ));

                            // clear working buffer
                            memset(workingBuffer, 0, 2 * FF_LFN_BUF + 1);

                            // compose file path
                            strcat(workingBuffer, workingPath);
                            strcat(workingBuffer, fileInfo.fname);

                            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[ StorageFile::FIELD___path ], workingBuffer ));

                            // compute directory date
                            fileInfoTime = GetDateTime(fileInfo.fdate, fileInfo.ftime);

                            // get a reference to the dateCreated managed field...
                            CLR_RT_HeapBlock& dateFieldRef = hbObj[ StorageFile::FIELD___dateCreated ];
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
        // failed to change drive
        NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
    }

    NANOCLR_CLEANUP();

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

    NANOCLR_CLEANUP_END();
}

HRESULT StorageFolder::CreateFileNative___WindowsStorageStorageFile__STRING__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Index    storageFileTypeDef;
    CLR_RT_HeapBlock*       storageFile;

    const char* fileName;
    const char* workingPath;

    CreationCollisionOption options;

    FIL             file;
    static FILINFO  fileInfo;
    SYSTEMTIME      fileInfoTime;    
    FRESULT         operationResult;
    uint8_t         modeFlags = 0;
    char*           filePath = NULL;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // get creation collision options
    options = (CreationCollisionOption)stack.Arg2().NumericByRef().u4;
        
    // get a pointer to the StorageFolder path in managed field
    workingPath = pThis[ StorageFolder::FIELD___path ].DereferenceString()->StringText();

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
    strcat(filePath, workingPath);
    strcat(filePath, fileName);

    // change directory
    operationResult = f_chdir(workingPath);

    if(operationResult != FR_OK)
    {
        if(operationResult == FR_INVALID_DRIVE)
        {
            // failed to change drive
            NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
        }
        // error opening the directoty
        NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
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
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( storageFile[ StorageFile::FIELD___name ], fileName ));

            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( storageFile[ StorageFile::FIELD___path ], filePath ));

            // get the date time details and fill in the managed field
            // compute directory date
            fileInfoTime = GetDateTime(fileInfo.fdate, fileInfo.ftime);

            // get a reference to the dateCreated managed field...
            CLR_RT_HeapBlock& dateFieldRef = storageFile[ StorageFile::FIELD___dateCreated ];
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

    NANOCLR_CLEANUP();

    // free buffer memory, if allocated
    if(filePath != NULL)
    {
        platform_free(filePath);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT StorageFolder::CreateFolderNative___WindowsStorageStorageFolder__STRING__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Index    storageFolderTypeDef;
    CLR_RT_HeapBlock*       storageFolder;

    const char* folderName;
    const char* workingPath;

    CreationCollisionOption options;

    static FILINFO  fileInfo;
    SYSTEMTIME      fileInfoTime;    
    FRESULT         operationResult;
    char*           folderPath = NULL;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // get creation collision options
    options = (CreationCollisionOption)stack.Arg2().NumericByRef().u4;
        
    // get a pointer to the path in managed field
    workingPath = pThis[ StorageFolder::FIELD___path ].DereferenceString()->StringText();

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
    strcat(folderPath, workingPath);
    strcat(folderPath, folderName);

    // change directory
    operationResult = f_chdir(workingPath);

    if(operationResult != FR_OK)
    {
        if(operationResult == FR_INVALID_DRIVE)
        {
            // failed to change drive
            NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
        }
        
        // error opening the directoty
        NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
    }
    else
    {
        // handle request for open if it exists and replace existing
        if( (options == CreationCollisionOption_OpenIfExists) ||
            (options == CreationCollisionOption_ReplaceExisting))
        {
            operationResult = f_stat(folderPath, &fileInfo);
        }
        else
        {
            // create directory
            operationResult = f_mkdir(folderPath);
        }

        // process operation result according to creation options
        if( (operationResult != FR_OK) &&
            (options == CreationCollisionOption_FailIfExists))
        {
            // folder already exists
            NANOCLR_SET_AND_LEAVE(CLR_E_PATH_ALREADY_EXISTS);
        }
        if( (operationResult == FR_NO_FILE) &&
            (options == CreationCollisionOption_OpenIfExists))
        {
            // folder doesn't exist
            NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
        }
        
        if(operationResult == FR_OK)
        {
            // folder created get the details or...
            // ...(if already exists) skip
            if(fileInfo.fattrib == 0)
            {
                f_stat(folderPath, &fileInfo);
            }

            // compose return object
            // find <StorageFolder> type, don't bother checking the result as it exists for sure
            g_CLR_RT_TypeSystem.FindTypeDef( "StorageFolder", "Windows.Storage", storageFolderTypeDef );

            // create a <StorageFolder>
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(stack.PushValue(), storageFolderTypeDef));
            
            // get a handle to the storage folder
            storageFolder = stack.TopValue().Dereference();

            // folder name
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( storageFolder[ StorageFolder::FIELD___name ], folderName ));

            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( storageFolder[ StorageFolder::FIELD___path ], folderPath ));

            // get the date time details and fill in the managed field
            // compute directory date
            fileInfoTime = GetDateTime(fileInfo.fdate, fileInfo.ftime);

            // get a reference to the dateCreated managed field...
            CLR_RT_HeapBlock& dateFieldRef = storageFolder[ StorageFolder::FIELD___dateCreated ];
            CLR_INT64* pRes = (CLR_INT64*)&dateFieldRef.NumericByRef().s8;
            // ...and set it with the fileInfoTime
            *pRes = HAL_Time_ConvertFromSystemTime( &fileInfoTime );
        }
        else
        {
            // failed to create the folder
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
        }
    }

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
	FRESULT     operationResult;

	// get a pointer to the managed object instance and check that it's not NULL
	CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

	// get a pointer to the path in managed field
	workingPath = pThis[StorageFolder::FIELD___path].DereferenceString()->StringText();

	// remove folder
	operationResult = f_unlink(workingPath);
	if (operationResult == FR_INVALID_NAME)
	{
		// Invalid path
		NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
	}
	else if (operationResult == FR_DENIED)
	{
		// folder not empty
		NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_EMPTY);
	}

	else if (operationResult != FR_OK)
	{
		// folder doesn't exist
		NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
	}

	NANOCLR_CLEANUP();

	NANOCLR_CLEANUP_END();
}

HRESULT Library_win_storage_native_Windows_Storage_StorageFolder::RenameFolderNative___VOID__STRING(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();

	const char* workingPath;
	const char* desiredPath;
	FRESULT     operationResult;

	// get a pointer to the managed object instance and check that it's not NULL
	CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

	// get a pointer to the path in managed field
	workingPath = pThis[StorageFolder::FIELD___path].DereferenceString()->StringText();

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
		NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
	}

	NANOCLR_CLEANUP();

	NANOCLR_CLEANUP_END();
}

HRESULT Library_win_storage_native_Windows_Storage_StorageFolder::GetFolderNative___WindowsStorageStorageFolder__STRING(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();

	CLR_RT_TypeDef_Index    storageFolderTypeDef;
	CLR_RT_HeapBlock*       storageFolder;

	const char* folderName;
	const char* workingPath;

	FILINFO     fileInfo;
	SYSTEMTIME  fileInfoTime;

	FRESULT     operationResult;
	char*       folderPath = NULL;
	
	CLR_INT64*  pRes;

	// get a pointer to the managed object instance and check that it's not NULL
	CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

	// get a pointer to the path in managed field
	workingPath = pThis[StorageFolder::FIELD___path].DereferenceString()->StringText();

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
	strcat(folderPath, workingPath);

	// Add "\" to path if required
	if (folderPath[hal_strlen_s(folderPath) - 1] != '\\')
	{
		strcat(folderPath, "\\");
	}

	strcat(folderPath, folderName);

	// check if directory exists
	operationResult = f_stat(folderPath, &fileInfo);
	if (operationResult != FR_OK)
	{
		// folder doesn't exist
		NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
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
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(storageFolder[StorageFolder::FIELD___name], folderName));

            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(storageFolder[StorageFolder::FIELD___path], folderPath));

            // get the date time details and fill in the managed field
            // compute directory date
            fileInfoTime = GetDateTime(fileInfo.fdate, fileInfo.ftime);

            // get a reference to the dateCreated managed field...
            CLR_RT_HeapBlock& dateFieldRef = storageFolder[StorageFolder::FIELD___dateCreated];
            pRes = (CLR_INT64*)&dateFieldRef.NumericByRef().s8;
            // ...and set it with the fileInfoTime
            *pRes = HAL_Time_ConvertFromSystemTime(&fileInfoTime);
        }
    }

	NANOCLR_CLEANUP();

	// free buffer memory, if allocated
	if (folderPath != NULL)
	{
		platform_free(folderPath);
	}

	NANOCLR_CLEANUP_END();
}
