// clang-format off

//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nf_system_data_sqlite.h"
#include "sqlite3.h"
//#include "sqlite3ext.h"

#define SQLITE_OMIT_WAL
#define SQLITE_OMIT_LOAD_EXTENSION
#define SQLITE_OMIT_AUTHORIZATION
#define SQLITE_OMIT_EXPLAIN
#define SQLITE_THREADSAFE=0
#define SQLITE_TEMP_STORE=3
#define SQLITE_ENABLE_MEMSYS5

//#define SQLITE_ENABLE_HIDDEN_COLUMNS
#define HAVE_ISNAN=0
#define SQLITE_HAVE_ISNAN=0
#define SQLITE_DEBUG
#define HAVE_LOCALTIME=0
#define HAVE_LOCALTIME_S=0

HRESULT Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeOpen___I4__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());


    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeClose___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
    // sqlite3_close(db);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativePrepare___I4__STRING__I4__BYREF_I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
    // rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    // if( rc != SQLITE_OK ){
    //     fprintf(stderr, "SQL error: %s\n", zErrMsg);
    //     sqlite3_free(zErrMsg);
    // } else {
    //     fprintf(stdout, "Table created successfully\n");
    // }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeErrorMessage___STRING( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeStep___I4__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeFinalize___I4__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeColumnCount___I4__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeColumnName___STRING__I4__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeColumnType___I4__I4__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeColumnLong___I8__I4__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeColumnText___STRING__I4__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeColumnDouble___R8__I4__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeColumnBlobLength___I4__I4__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_data_sqlite_System_Data_Sqlite_SqliteCommand::NativeColumnBlobData___VOID__I4__I4__SZARRAY_U1( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

// clang-format on
