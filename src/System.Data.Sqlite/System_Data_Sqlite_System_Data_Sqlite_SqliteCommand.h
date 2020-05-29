#ifndef _SYSTEM_DATA_SQLITE_SYSTEM_DATA_SQLITE_SYSTEM_DATA_SQLITE_SQLITECOMMAND_H_
#define _SYSTEM_DATA_SQLITE_SYSTEM_DATA_SQLITE_SYSTEM_DATA_SQLITE_SQLITECOMMAND_H_

namespace System_Data_Sqlite
{
    namespace System_Data_Sqlite
    {
        struct SqliteCommand
        {
            // Helper Functions to access fields of managed object
            // Declaration of stubs. These functions are implemented by Interop code developers

            signed int NativeOpen( const char* param0, HRESULT &hr );

            signed int NativeClose(  HRESULT &hr );

            signed int NativePrepare( const char* param0, signed int param1,  param2, HRESULT &hr );

            const char* NativeErrorMessage(  HRESULT &hr );

            signed int NativeStep( signed int param0, HRESULT &hr );

            signed int NativeFinalize( signed int param0, HRESULT &hr );

            signed int NativeColumnCount( signed int param0, HRESULT &hr );

            const char* NativeColumnName( signed int param0, signed int param1, HRESULT &hr );

            signed int NativeColumnType( signed int param0, signed int param1, HRESULT &hr );

            int64_t NativeColumnLong( signed int param0, signed int param1, HRESULT &hr );

            const char* NativeColumnText( signed int param0, signed int param1, HRESULT &hr );

            double NativeColumnDouble( signed int param0, signed int param1, HRESULT &hr );

            signed int NativeColumnBlobLength( signed int param0, signed int param1, HRESULT &hr );

            void NativeColumnBlobData( signed int param0, signed int param1, CLR_RT_TypedArray_UINT8 param2, HRESULT &hr );

        };
    }
}

#endif //_SYSTEM_DATA_SQLITE_SYSTEM_DATA_SQLITE_SYSTEM_DATA_SQLITE_SQLITECOMMAND_H_
