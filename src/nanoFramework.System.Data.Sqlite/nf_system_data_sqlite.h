// clang-format off

//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _NF_SYSTEM_DATA_SQLITE_H_
#define _NF_SYSTEM_DATA_SQLITE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

typedef enum __nfpack SQLiteColumnType
{
    SQLiteColumnType_None = 0,
    SQLiteColumnType_Integer = 1,
    SQLiteColumnType_Float = 2,
    SQLiteColumnType_Text = 3,
    SQLiteColumnType_Blob = 4,
    SQLiteColumnType_Null = 5,
} SQLiteColumnType;

typedef enum __nfpack SQLiteErrorCode
{
    SQLiteErrorCode_Unknown = -1,
    SQLiteErrorCode_Ok = 0,
    SQLiteErrorCode_Error = 1,
    SQLiteErrorCode_Internal = 2,
    SQLiteErrorCode_Perm = 3,
    SQLiteErrorCode_Abort = 4,
    SQLiteErrorCode_Busy = 5,
    SQLiteErrorCode_Locked = 6,
    SQLiteErrorCode_NoMem = 7,
    SQLiteErrorCode_ReadOnly = 8,
    SQLiteErrorCode_Interrupt = 9,
    SQLiteErrorCode_IoErr = 10,
    SQLiteErrorCode_Corrupt = 11,
    SQLiteErrorCode_NotFound = 12,
    SQLiteErrorCode_Full = 13,
    SQLiteErrorCode_CantOpen = 14,
    SQLiteErrorCode_Protocol = 15,
    SQLiteErrorCode_Empty = 16,
    SQLiteErrorCode_Schema = 17,
    SQLiteErrorCode_TooBig = 18,
    SQLiteErrorCode_Constraint = 19,
    SQLiteErrorCode_Mismatch = 20,
    SQLiteErrorCode_Misuse = 21,
    SQLiteErrorCode_NoLfs = 22,
    SQLiteErrorCode_Auth = 23,
    SQLiteErrorCode_Format = 24,
    SQLiteErrorCode_Range = 25,
    SQLiteErrorCode_NotADb = 26,
    SQLiteErrorCode_Notice = 27,
    SQLiteErrorCode_Warning = 28,
    SQLiteErrorCode_Row = 100,
    SQLiteErrorCode_Done = 101,
    SQLiteErrorCode_NonExtendedMask = 255,
} SQLiteErrorCode;

struct Library_nf_system_data_sqlite_System_Data_Sqlite_ResultSet
{
    static const int FIELD__<RowCount>k__BackingField = 1;
    static const int FIELD__<ColumnCount>k__BackingField = 2;
    static const int FIELD__<ColumnNames>k__BackingField = 3;
    static const int FIELD__<Data>k__BackingField = 4;

    //--//

};

struct Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand
{
    static const int FIELD__disposed = 1;
    static const int FIELD__nativePointer = 2;

    NANOCLR_NATIVE_DECLARE(NativeOpen___I4__STRING);
    NANOCLR_NATIVE_DECLARE(NativeClose___I4);
    NANOCLR_NATIVE_DECLARE(NativePrepare___I4__STRING__I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(NativeErrorMessage___STRING);
    NANOCLR_NATIVE_DECLARE(NativeStep___I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeFinalize___I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeColumnCount___I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeColumnName___STRING__I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeColumnType___I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeColumnLong___I8__I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeColumnText___STRING__I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeColumnDouble___R8__I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeColumnBlobLength___I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeColumnBlobData___VOID__I4__I4__SZARRAY_U1);

    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_System_Data_Sqlite;

#endif  //_NF_SYSTEM_DATA_SQLITE_H_

// clang-format on
