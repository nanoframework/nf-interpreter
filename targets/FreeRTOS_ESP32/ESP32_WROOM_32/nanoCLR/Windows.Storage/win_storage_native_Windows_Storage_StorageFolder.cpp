//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

//#include <ff.h>
#include "win_storage_native.h"
#include <target_windows_storage_config.h>
#include <Target_Windows_Storage.h>
#include <nanoHAL_Windows_Storage.h>
#include <target_platform.h>

#include "dirent.h"

// defining these types here to make it shorter and improve code readability
typedef Library_win_storage_native_Windows_Storage_StorageFolder StorageFolder;
typedef Library_win_storage_native_Windows_Storage_StorageFile StorageFile;

//
//  Converts from windows type path       "c:\folder\folder\file.ext"
//  to linux type path used in ESP32 VFS  "/c/folder/folder/file.exe
//  where /c is the mount point
//
char * ConvertToESP32Path(const char * filepath)
{
    char * startPath = NULL;
    char * path = NULL;

    int pathlen = hal_strlen_s(filepath);
    startPath = (char*)platform_malloc(pathlen + 1);
    path = startPath;
    hal_strcpy_s(path, pathlen + 1, filepath);

    if (hal_strlen_s(path) >= 2)
    {
        // Map  Drive: -> /C
        char drive = *path;
        *path++ = '/';
        *path++ = drive;

        // Convert '\' to '/'
        while (*path)
        {
            if (*path == '\\')
                *path++ = '/';
            else
                path++;
        }
    }

    return startPath;
}

SYSTEMTIME GetDateTime(time_t * time)
{
    SYSTEMTIME fileTime;

    memset(&fileTime, 0, sizeof(SYSTEMTIME));

    tm * ptm = gmtime(time);

    // Year from 1601
    fileTime.wYear = ptm->tm_year + 1900;  // from Year based 1900
    // Month 1-12
    fileTime.wMonth = ptm->tm_mon + 1;    // from Month 0-11
    // Day of month 1 - 31
    fileTime.wDay = ptm->tm_mday;		 
    // Day of week 0 = Sunday
    fileTime.wDayOfWeek = ptm->tm_wday;   // 0 = Sunday
    // Hour 0 to 23.  
    fileTime.wHour = ptm->tm_hour;		  
    // Minutes 0 to 59. 
    fileTime.wMinute = ptm->tm_min;		 
    // seconds 0 to 59
    fileTime.wSecond = ptm->tm_sec;
    
    return fileTime;
}

uint64_t GetFileTimeFromPath(char * path)
{
    char *      workingPath = NULL;
    struct stat fileInfo;
    SYSTEMTIME  fileInfoTime;

    // Convert folder path back to ESP32 VFS format so
    // we can stat folder
    workingPath = ConvertToESP32Path(path);

    memset(&fileInfo, 0, sizeof(fileInfo));

    // compute folder/file date/time
    if (stat(workingPath, &fileInfo) == 0)
    {
        fileInfoTime = GetDateTime(&fileInfo.st_ctime);
    }
    else
    {
        // Spiffs don't have stat so zero date/time
        memset(&fileInfoTime, 0, sizeof(SYSTEMTIME));
        // Make it a valid date
        fileInfoTime.wYear = 1900;
        fileInfoTime.wMonth = 1;
        fileInfoTime.wDay = 1;
    }

    if (workingPath != NULL)
    {
        platform_free(workingPath);
    }

    return HAL_Time_ConvertFromSystemTime(&fileInfoTime);
}


HRESULT StorageFolder::GetRemovableStorageFoldersNative___SZARRAY_WindowsStorageStorageFolder( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    char* stringBuffer;
    uint32_t driveCount = 0;
    char workingDrive[sizeof(DRIVE_PATH_LENGTH)];
    uint16_t driveIterator = 0;

    bool sdCardDrivePresent = false;

    CLR_RT_HeapBlock* storageFolder;
    CLR_RT_TypeDef_Index storageFolderTypeDef;
    CLR_RT_HeapBlock* hbObj;
    CLR_RT_HeapBlock& top   = stack.PushValue();


 #if HAL_USE_SDC
     // add count
     driveCount++;

     // Esp32 doesn't currently have card present so make always true
     sdCardDrivePresent = true;
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
        for(; driveIterator < SUPPORTED_DRIVES_COUNT; driveIterator++ )
        {
            // fill the folder name and path
            if (driveIterator == 0 && sdCardDrivePresent == true )
            {
                memcpy(workingDrive, INDEX0_DRIVE_PATH, DRIVE_PATH_LENGTH);
            }
            else
            {
                // skip this drive type
                continue;
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

				// TODO call FATFS method for this as function not present in VFS
                // read the drive volume label
                // don't bother checking the result, if anything goes wrong we'll end up with an empty string which is OK
//                f_getlabel(workingDrive, stringBuffer, NULL);

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
    {
        char workingDrive[sizeof(DRIVE_PATH_LENGTH)];

        uint32_t driveCount = 0;

        CLR_RT_HeapBlock* storageFolder;
        CLR_RT_TypeDef_Index storageFolderTypeDef;
        CLR_RT_HeapBlock* hbObj;
        CLR_RT_HeapBlock& top = stack.PushValue();

        // One internal drive
        driveCount++;

        // start composing the reply
        // find <StorageFolder> type definition, don't bother checking the result as it exists for sure
        g_CLR_RT_TypeSystem.FindTypeDef("StorageFolder", "Windows.Storage", storageFolderTypeDef);

        // create an array of <StorageFolder>
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, driveCount, storageFolderTypeDef));

        if (driveCount > 0)
        {
            // get a pointer to the first object in the array (which is of type <StorageFolder>)
            storageFolder = (CLR_RT_HeapBlock*)top.DereferenceArray()->GetFirstElement();

            // create an instance of <StorageFolder>
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*storageFolder, storageFolderTypeDef));

            // dereference the object in order to reach its fields
            hbObj = storageFolder->Dereference();

            memcpy(workingDrive, INTERNAL_DRIVE_PATH, DRIVE_PATH_LENGTH);

            // set the managed fields
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(hbObj[StorageFolder::FIELD___name], workingDrive));
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(hbObj[StorageFolder::FIELD___path], workingDrive));
        }
    }
    NANOCLR_NOCLEANUP();
}


HRESULT StorageFolder::GetStorageFoldersNative___SZARRAY_WindowsStorageStorageFolder( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Index    storageFolderTypeDef;
    CLR_RT_HeapBlock*       storageFolder;
    CLR_RT_HeapBlock*       hbObj;

    const char* managedPath;

    DIR *           dir = NULL;
    struct dirent * dirInfo;

    uint16_t        directoryCount = 0;
    char*           stringBuffer = NULL;
    char*           workingBuffer = NULL;
    char *          workingPath = NULL;

    CLR_RT_HeapBlock& top   = stack.PushValue();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
    // get a pointer to the path in managed field
    managedPath = pThis[ StorageFolder::FIELD___path ].DereferenceString()->StringText();

    // Convert to ESP32 form path for VFS ( linux like )
    // returns allocated converted path, must be freed	
    workingPath = ConvertToESP32Path(managedPath);

    // open directory
    dir = opendir(workingPath);
    if(dir)
    {
        // need to perform this in two steps
        // 1st: count the directory objects
        // 2nd: create the array items with each directory object

        // perform 1st pass
        for (;;)
        {
            // read next directory item
            dirInfo = readdir(dir);

            // break on error or at end of dir
            if (dirInfo == NULL)
            {
                break;
            }

            // check if this is a directory
            // but skip if:
            if (dirInfo->d_type == DT_DIR)
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
            rewinddir(dir);
            
            for (;;)
            {
                // read next directory item
                dirInfo = readdir(dir);

                // break on error or at end of dir
                if (dirInfo == NULL)
                {
                    break;
                }

                // check if this is a directory
                // but skip if:
                if (dirInfo->d_type == DT_DIR)
                {
                    // create an instance of <StorageFolder>
                    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*storageFolder, storageFolderTypeDef));

                    // dereference the object in order to reach its fields
                    hbObj = storageFolder->Dereference();

                    // directory name
                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[ StorageFolder::FIELD___name ], dirInfo->d_name));

                    // clear working buffer
                    memset(workingBuffer, 0, 2 * FF_LFN_BUF + 1);

                    // compose return directory path
                    strcat(workingBuffer, managedPath);
                    strcat(workingBuffer, "\\");
                    strcat(workingBuffer, dirInfo->d_name);
                    
                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbObj[ StorageFolder::FIELD___path ], workingBuffer ));

                    // get a reference to the dateCreated managed field...
                    CLR_RT_HeapBlock& dateFieldRef = hbObj[ StorageFolder::FIELD___dateCreated ];
                    CLR_INT64* pRes = (CLR_INT64*)&dateFieldRef.NumericByRef().s8;
                    // ...and set it 
                    *pRes = GetFileTimeFromPath(workingBuffer);

                    // move the storage folder pointer to the next item in the array
                    storageFolder++;
                }
            }
        }
    

    }
	else
	{
		// error or directory empty
		// find <StorageFolder> type, don't bother checking the result as it exists for sure
		g_CLR_RT_TypeSystem.FindTypeDef("StorageFolder", "Windows.Storage", storageFolderTypeDef);

		// create an array of <StorageFolder>
		NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, directoryCount, storageFolderTypeDef));
	}

    NANOCLR_CLEANUP();

    if (dir)
    {
        // close directory
        closedir(dir);
    }

    // free buffers memory, if allocated
    if(stringBuffer != NULL)
    {
        platform_free(stringBuffer);
    }
    if(workingBuffer != NULL)
    {
        platform_free(workingBuffer);
    }
    if (workingPath != NULL)
    {
        platform_free(workingPath);
    }

    NANOCLR_CLEANUP_END();
}



HRESULT StorageFolder::GetStorageFilesNative___SZARRAY_WindowsStorageStorageFile__U4__U4(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();

	CLR_RT_TypeDef_Index    storageFileTypeDef;
	CLR_RT_HeapBlock*       storageFile;
	CLR_RT_HeapBlock*       hbObj;

	const char* managedPath;
	char * workingPath = NULL;

	uint32_t startIndex;
	uint32_t maxItemsToRetrieve;
	uint32_t itemIndex = 0;

	DIR *           dir = NULL;
	struct dirent * dirInfo;
	uint16_t        fileCount = 0;
	char*           stringBuffer = NULL;
	char*           workingBuffer = NULL;

	CLR_RT_HeapBlock& top = stack.PushValue();

	// get a pointer to the managed object instance and check that it's not NULL
	CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

	// get start index
	startIndex = stack.Arg1().NumericByRef().u4;

	// get max items to retrieve
	maxItemsToRetrieve = stack.Arg2().NumericByRef().u4;


	// get a pointer to the path in managed field
	managedPath = pThis[StorageFolder::FIELD___path].DereferenceString()->StringText();

	// Convert to ESP32 form path ( linux like )
	// return allocated converted path, must be freed
	workingPath = ConvertToESP32Path(managedPath);

	dir = opendir(workingPath);
	if (dir)
	{
		// need to perform this in two steps
		// 1st: count the file objects
		// 2nd: create the array items with each file object

		// perform 1st pass
		for (;;)
		{
			// read next directory item
			dirInfo = readdir(dir);

			// break on error or at end of dir
			if (dirInfo == NULL)
			{
				break;
			}

			// check if this is a file
			// but skip if diretory:
			if (dirInfo->d_type == DT_REG)
			{
				// check if this file is within the requested parameters
				if ((itemIndex >= startIndex) &&
					(fileCount < maxItemsToRetrieve))
				{
					fileCount++;
				}

				itemIndex++;
			}
		}

		// find <StorageFile> type, don't bother checking the result as it exists for sure
		g_CLR_RT_TypeSystem.FindTypeDef("StorageFile", "Windows.Storage", storageFileTypeDef);

		// create an array of <StorageFile>
		NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, fileCount, storageFileTypeDef));

		// get a pointer to the first object in the array (which is of type <StorageFile>)
		storageFile = (CLR_RT_HeapBlock*)top.DereferenceArray()->GetFirstElement();

		if (fileCount > 0)
		{
			// allocate memory for buffers
			stringBuffer = (char*)platform_malloc(FF_LFN_BUF + 1);
			workingBuffer = (char*)platform_malloc(2 * FF_LFN_BUF + 1);

			// sanity check for successfull malloc
			if (stringBuffer == NULL || workingBuffer == NULL)
			{
				// failed to allocate memory
				NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
			}

			// perform 2nd pass
			// need to rewind the directory read index first
			rewinddir(dir);

			// and reset the file iterator vars too
			itemIndex = 0;
			fileCount = 0;


			for (;;)
			{
				// read next directory item
				dirInfo = readdir(dir);

				// break on error or at end of dir
				if (dirInfo == NULL)
				{
					break;
				}

				// check if this is a file
				// but skip if diretory:
				if (dirInfo->d_type == DT_REG)
				{
					// check if this file is within the requested parameters
					if ((itemIndex >= startIndex) &&
						(fileCount < maxItemsToRetrieve))
					{
						// create an instance of <StorageFile>
						NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*storageFile, storageFileTypeDef));

						// dereference the object in order to reach its fields
						hbObj = storageFile->Dereference();

						// file name
						NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(hbObj[StorageFile::FIELD___name], dirInfo->d_name));

						// clear working buffer
						memset(workingBuffer, 0, 2 * FF_LFN_BUF + 1);

						// compose file path
						strcat(workingBuffer, managedPath);
						strcat(workingBuffer, "\\");
						strcat(workingBuffer, dirInfo->d_name);

						NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(hbObj[StorageFile::FIELD___path], workingBuffer));

						// get a reference to the dateCreated managed field...
						CLR_RT_HeapBlock& dateFieldRef = hbObj[StorageFile::FIELD___dateCreated];
						CLR_INT64* pRes = (CLR_INT64*)&dateFieldRef.NumericByRef().s8;
						// ...and set it 
						*pRes = GetFileTimeFromPath(workingBuffer);

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
			g_CLR_RT_TypeSystem.FindTypeDef("StorageFile", "Windows.Storage", storageFileTypeDef);

			// create an array of <StorageFile>
			NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, fileCount, storageFileTypeDef));
		}
	}
	else
	{
		// error or directory empty
		// find <StorageFile> type, don't bother checking the result as it exists for sure
		g_CLR_RT_TypeSystem.FindTypeDef("StorageFile", "Windows.Storage", storageFileTypeDef);

		// create an array of <StorageFile>
		NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, fileCount, storageFileTypeDef));
	}

	NANOCLR_CLEANUP();

	// close directory
	if (dir)
	{
		closedir(dir);
	}

    // free buffers memory, if allocated
    if(stringBuffer != NULL)
    {
        platform_free(stringBuffer);
    }
    if(workingBuffer != NULL)
    {
        platform_free(workingBuffer);
    }
    if (workingPath != NULL)
    {
        platform_free(workingPath);
    }

    NANOCLR_CLEANUP_END();
}


HRESULT StorageFolder::CreateFileNative___WindowsStorageStorageFile__STRING__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Index    storageFileTypeDef;
    CLR_RT_HeapBlock*       storageFile;

    const char* fileName;
    const char* managedPath;
    char * workingPath = NULL;

    CreationCollisionOption options;
    bool   fileExists = false;

    FILE *          file;
    char            mode[5] = { 'r' };
    char*           filePath = NULL;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // get creation collision options
    options = (CreationCollisionOption)stack.Arg2().NumericByRef().u4;
        
    // get a pointer to the StorageFolder path in managed field
    managedPath = pThis[ StorageFolder::FIELD___path ].DereferenceString()->StringText();

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
    strcat(filePath, managedPath);

	// Add "\" to path if required
	if (filePath[hal_strlen_s(filePath) - 1] != '\\')
	{
		strcat(filePath, "\\");
	}
	
	strcat(filePath, fileName);

    // Convert to ESP32 VFS path 
    // return allocated converted path, must be freed
    workingPath = ConvertToESP32Path(filePath);

    // Check if file exists
    file = fopen(workingPath, "r");
    if (file)
    {
        fclose(file);
        fileExists = true;
    }

    // compute mode flags from CreationCollisionOption
    switch (options)
    {
    case CreationCollisionOption_ReplaceExisting:
        mode[0] = 'w';
        mode[1] = '+';
        break;

    case CreationCollisionOption_FailIfExists:
        // Check if it exists
        if (fileExists)
        {
            // file already exists
            NANOCLR_SET_AND_LEAVE(CLR_E_PATH_ALREADY_EXISTS);
        }

        //modeFlags = FA_CREATE_NEW;
        mode[0] = 'w';
        break;

    case CreationCollisionOption_OpenIfExists:
        // Open existing file & truncate
        mode[0] = 'r';
        mode[1] = '+';
        break;

    case CreationCollisionOption_GenerateUniqueName:
        // this operation is not supported in nanoFramework
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        break;

    default:
        break;
    }

    // open file
    file = fopen(workingPath, mode);
    if (file)
    {
        // file created (or opened) succesfully
        // OK to close it
        fclose(file);

        // compose return object
        // find <StorageFile> type, don't bother checking the result as it exists for sure
        g_CLR_RT_TypeSystem.FindTypeDef( "StorageFile", "Windows.Storage", storageFileTypeDef );
            
        // create a <StorageFile>
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(stack.PushValue(), storageFileTypeDef));
                        
        // get a handle to the storage file
        storageFile = stack.TopValue().Dereference();
            
        // file name
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( storageFile[ StorageFile::FIELD___name ], fileName ));
        
        // file path
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( storageFile[ StorageFile::FIELD___path ], filePath ));

        // get a reference to the dateCreated managed field...
        CLR_RT_HeapBlock& dateFieldRef = storageFile[ StorageFile::FIELD___dateCreated ];
        CLR_INT64* pRes = (CLR_INT64*)&dateFieldRef.NumericByRef().s8;

        // ...and set it 
        *pRes = GetFileTimeFromPath(workingPath);
    }
    else
    {
        // file doesn't exist
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
    }

    NANOCLR_CLEANUP();

    // free buffer memory, if allocated
    if(filePath != NULL)
    {
        platform_free(filePath);
    }

    if (workingPath != NULL)
    {
        platform_free(workingPath);
    }

    NANOCLR_CLEANUP_END();
}



HRESULT StorageFolder::CreateFolderNative___WindowsStorageStorageFolder__STRING__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Index    storageFolderTypeDef;
    CLR_RT_HeapBlock*       storageFolder;

    const char* folderName;
    char*       workingPath = NULL;
    const char* managedPath;

    CreationCollisionOption options;

    static struct stat fileInfo;

    int             operationResult;
    char*           folderPath = NULL;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // get creation collision options
    options = (CreationCollisionOption)stack.Arg2().NumericByRef().u4;
        
    // get a pointer to the path in managed field
    managedPath = pThis[ StorageFolder::FIELD___path ].DereferenceString()->StringText();

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
    strcat(folderPath, managedPath);
	
	// Add "\" to path if required
	if (folderPath[hal_strlen_s(folderPath) - 1] != '\\')
	{
		strcat(folderPath, "\\");
	}

    strcat(folderPath, folderName);

    // Convert to ESP32 form path ( linux like )
    // return allocated converted path, must be freed
    workingPath = ConvertToESP32Path(folderPath);

    // handle request for open if it exists and replace existing
    if( (options == CreationCollisionOption_OpenIfExists) ||
        (options == CreationCollisionOption_ReplaceExisting))
    {
        operationResult = stat(workingPath, &fileInfo);
        if (operationResult < 0) operationResult = errno;
    }
    else
    {
        // create directory
        operationResult = mkdir(workingPath,0);
        if (operationResult < 0) operationResult = errno;

        if (operationResult == ENOTSUP)
        {
            // Folders not supported, SPIFFS volume
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }
    }

    // process operation result according to creation options
    if( (operationResult == EEXIST ) &&
        (options == CreationCollisionOption_FailIfExists))
    {
        // folder already exists
        NANOCLR_SET_AND_LEAVE(CLR_E_PATH_ALREADY_EXISTS);
    }

    if( (operationResult == ENOENT) &&
        (options == CreationCollisionOption_OpenIfExists))
    {
        // folder doesn't exist
        NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
    }

    if(operationResult == 0)
    {
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
        // get a reference to the dateCreated managed field...
        CLR_RT_HeapBlock& dateFieldRef = storageFolder[ StorageFolder::FIELD___dateCreated ];
        CLR_INT64* pRes = (CLR_INT64*)&dateFieldRef.NumericByRef().s8;

        // get the date time details and fill in the managed field
        // compute directory date
        // ...and set it 
        *pRes = GetFileTimeFromPath(workingPath);
    }
    else
    {
        // failed to create the folder
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }

    NANOCLR_CLEANUP();

    // free buffer memory, if allocated
    if(folderPath != NULL)
    {
        platform_free(folderPath);
    }
    if (workingPath != NULL)
    {
        platform_free(workingPath);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_win_storage_native_Windows_Storage_StorageFolder::DeleteFolderNative___VOID(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();

	char*       workingPath = NULL;
	const char* managedPath;
	int         operationResult;

	// get a pointer to the managed object instance and check that it's not NULL
	CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

	// get a pointer to the path in managed field
	managedPath = pThis[StorageFolder::FIELD___path].DereferenceString()->StringText();

	// Convert to ESP32 form path ( linux like )
	// return allocated converted path, must be freed
	workingPath = ConvertToESP32Path(managedPath);

	// Delete folder, 0=succesfull
	operationResult = rmdir(workingPath);
	if (operationResult < 0) operationResult = errno;
	if (operationResult == ENOENT)
	{
		// Invalid path
		NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
	}
	else if (operationResult == EACCES)
	{
		// folder not empty
		NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_EMPTY);
	}
	
	else if (operationResult != 0)
	{
		// folder doesn't exist
		NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
	}

	NANOCLR_CLEANUP();

	// free buffer memory, if allocated
	if (workingPath != NULL)
	{
		platform_free(workingPath);
	}

	NANOCLR_CLEANUP_END();
}

HRESULT Library_win_storage_native_Windows_Storage_StorageFolder::RenameFolderNative___VOID__STRING(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();

	char*       workingPath = NULL;
	char*       desiredWorkingPath = NULL;
	const char* managedPath;
	const char* desiredPath;
	int         operationResult;

	// get a pointer to the managed object instance and check that it's not NULL
	CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

	// get a pointer to the path in managed field
	managedPath = pThis[StorageFolder::FIELD___path].DereferenceString()->StringText();

	// Convert to ESP32 form path ( linux like )
	// return allocated converted path, must be freed
	workingPath = ConvertToESP32Path(managedPath);

	// get a pointer to the desired folder name
	desiredPath = stack.Arg1().DereferenceString()->StringText();
	desiredWorkingPath = ConvertToESP32Path(desiredPath);

	// rename folder, 0=succesfull
	operationResult = rename(workingPath, desiredWorkingPath);
	if (operationResult < 0) operationResult = errno;
	if (operationResult == ENOENT)
	{
		// Invalid path
		NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
	}
	else if (operationResult != 0)
	{
		// folder doesn't exist
		NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
	}

	NANOCLR_CLEANUP();

	// free buffer memory, if allocated
	if (workingPath != NULL)
	{
		platform_free(workingPath);
	}
	if (desiredWorkingPath != NULL)
	{
		platform_free(desiredWorkingPath);
	}

	NANOCLR_CLEANUP_END();
}

HRESULT Library_win_storage_native_Windows_Storage_StorageFolder::GetFolderNative___WindowsStorageStorageFolder__STRING(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();

	CLR_RT_TypeDef_Index    storageFolderTypeDef;
	CLR_RT_HeapBlock*       storageFolder;

	const char* folderName;
	char*       workingPath = NULL;
	const char* managedPath;

	static struct stat fileInfo;

	int             operationResult;
	char*           folderPath = NULL;

	// get a pointer to the managed object instance and check that it's not NULL
	CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

	// get a pointer to the path in managed field
	managedPath = pThis[StorageFolder::FIELD___path].DereferenceString()->StringText();

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
	strcat(folderPath, managedPath);

	// Add "\" to path if required
	if (folderPath[hal_strlen_s(folderPath) - 1] != '\\')
	{
		strcat(folderPath, "\\");
	}

	strcat(folderPath, folderName);

	// Convert to ESP32 form path ( linux like )
	// return allocated converted path, must be freed
	workingPath = ConvertToESP32Path(folderPath);

	// Check Directory exists directory
	operationResult = stat(workingPath, &fileInfo);
	if (operationResult < 0) operationResult = errno;

	if ( operationResult == ENOENT )
	{
		// folder doesn't exist
		NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
	}

	if (operationResult == 0)
	{
		// Is this a file
		if (fileInfo.st_mode & S_IFREG)
		{
			// Path represents a file
			NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
		}


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
		// get a reference to the dateCreated managed field...
		CLR_RT_HeapBlock& dateFieldRef = storageFolder[StorageFolder::FIELD___dateCreated];
		CLR_INT64* pRes = (CLR_INT64*)&dateFieldRef.NumericByRef().s8;

		// get the date time details and fill in the managed field
		// compute directory date
		// ...and set it 
		*pRes = GetFileTimeFromPath(workingPath);
	}
	else
	{
		// failed to create the folder
		NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
	}

	NANOCLR_CLEANUP();

	// free buffer memory, if allocated
	if (folderPath != NULL)
	{
		platform_free(folderPath);
	}
	if (workingPath != NULL)
	{
		platform_free(workingPath);
	}

	NANOCLR_CLEANUP_END();
}
