//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nf_system_data_sqlite.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeOpen___I4__STRING,
    Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeClose___I4,
    Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativePrepare___I4__STRING__I4__BYREF_I4,
    Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeErrorMessage___STRING,
    Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeStep___I4__I4,
    Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeFinalize___I4__I4,
    Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeColumnCount___I4__I4,
    Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeColumnName___STRING__I4__I4,
    Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeColumnType___I4__I4__I4,
    Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeColumnLong___I8__I4__I4,
    Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeColumnText___STRING__I4__I4,
    Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeColumnDouble___R8__I4__I4,
    Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeColumnBlobLength___I4__I4__I4,
    Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeColumnBlobData___VOID__I4__I4__SZARRAY_U1,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_System_Data_Sqlite =
{
    "nanoFramework.System.Data.Sqlite",
    0x86272AD6,
    method_lookup,
    { 1, 0, 0, 0 }
};

// clang-format on
