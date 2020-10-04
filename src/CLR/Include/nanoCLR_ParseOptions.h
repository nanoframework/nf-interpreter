//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOCLR_PARSEOPTIONS_H_
#define _NANOCLR_PARSEOPTIONS_H_

#include <nanoCLR_Runtime.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)

struct CLR_RT_ParseOptions
{
    typedef std::map< std::wstring, CLR_RT_Buffer* > BufferMap;
    typedef BufferMap::iterator                      BufferMapIter;

    struct Parameter
    {
        const wchar_t* m_szName;
        const wchar_t* m_szDescription;

        Parameter( const wchar_t* szName, const wchar_t* szDescription );

        virtual bool Parse( const wchar_t* val ) = 0;
    };

    typedef std::vector< Parameter* > ParameterList;
    typedef ParameterList::iterator   ParameterListIter;

    CLR_RT_StringVector CommandLineArgs;

    //--//

    struct Parameter_Generic : Parameter
    {
        std::wstring m_data;

        Parameter_Generic( const wchar_t* szName, const wchar_t* szDescription );

        virtual bool Parse( const wchar_t* arg );
    };

    struct Parameter_String : Parameter
    {
        std::wstring  m_dataParsed;
        std::wstring* m_dataPtr;

        Parameter_String( std::wstring* data, const wchar_t* szName, const wchar_t* szDescription );

        virtual bool Parse( const wchar_t* arg );
    };

    struct Parameter_Boolean : Parameter
    {
        bool  m_dataParsed;
        bool* m_dataPtr;

        Parameter_Boolean( bool* data, const wchar_t* szName, const wchar_t* szDescription );

        virtual bool Parse( const wchar_t* arg );
    };

    struct Parameter_Integer : Parameter
    {
        int  m_dataParsed;
        int* m_dataPtr;

        Parameter_Integer( int* data, const wchar_t* szName, const wchar_t* szDescription );

        virtual bool Parse( const wchar_t* arg );
    };

    struct Parameter_Float : Parameter
    {
        float  m_dataParsed;
        float* m_dataPtr;

        Parameter_Float( float* data, const wchar_t* szName, const wchar_t* szDescription );

        virtual bool Parse( const wchar_t* arg );
    };

    //--//

    struct Command
    {
        const wchar_t*       m_szName;
        const wchar_t*       m_szDescription;
        ParameterList m_params;

        Command( const wchar_t* szName, const wchar_t* szDescription );

        virtual bool Parse( CLR_RT_StringVector& argv, size_t& pos, CLR_RT_ParseOptions& options );

        virtual HRESULT Execute();
    };

    typedef std::list< Command* > CommandList;
    typedef CommandList::iterator CommandListIter;

    //--//

    struct Command_SetFlag : Command
    {
        bool  m_dataParsed;
        bool* m_dataPtr;

        Command_SetFlag( bool* data, const wchar_t* szName, const wchar_t* szDescription );

        virtual bool Parse( CLR_RT_StringVector& argv, size_t& pos, CLR_RT_ParseOptions& options );
    };

    //--//

    bool        m_fVerbose;
    CommandList m_commands;

    //--//

    CLR_RT_ParseOptions();

    HRESULT ExtractOptionsFromFile( const wchar_t* szFileName );

    HRESULT ReprocessOptions();
    HRESULT ProcessOptions(                          CLR_RT_StringVector& vec );
    void    PushArguments ( int argc, wchar_t* argv[], CLR_RT_StringVector& vec );

    virtual void Usage();
};

#endif

#endif // _NANOCLR_PARSEOPTIONS_H_
