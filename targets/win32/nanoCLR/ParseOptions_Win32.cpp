//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"


////////////////////////////////////////////////////////////////////////////////////////////////////

CLR_RT_ParseOptions::Parameter::Parameter( const wchar_t* szName, const wchar_t* szDescription )
{
    m_szName        = szName;
    m_szDescription = szDescription;
}

//--//

CLR_RT_ParseOptions::Parameter_Generic::Parameter_Generic( const wchar_t* szName, const wchar_t* szDescription ) : CLR_RT_ParseOptions::Parameter( szName, szDescription )
{
}

bool CLR_RT_ParseOptions::Parameter_Generic::Parse( const wchar_t* arg )
{
    m_data = arg;

    return true;
}

//--//

CLR_RT_ParseOptions::Parameter_String::Parameter_String( std::wstring* data, const wchar_t* szName, const wchar_t* szDescription ) : CLR_RT_ParseOptions::Parameter( szName, szDescription )
{
    m_dataPtr = data ? data : &m_dataParsed;
}

bool CLR_RT_ParseOptions::Parameter_String::Parse( const wchar_t* arg )
{
    *m_dataPtr = arg;

    return true;
}

//--//

CLR_RT_ParseOptions::Parameter_Boolean::Parameter_Boolean( bool* data, const wchar_t* szName, const wchar_t* szDescription ) : CLR_RT_ParseOptions::Parameter( szName, szDescription )
{
    m_dataPtr = data ? data : &m_dataParsed;
}

bool CLR_RT_ParseOptions::Parameter_Boolean::Parse( const wchar_t* arg )
{
    int num;

    if(!_wcsicmp( arg, L"true" ) ||
       !_wcsicmp( arg, L"on"   )  )
    {
        *m_dataPtr = true;
        return true;
    }

    if(!_wcsicmp( arg, L"false" ) ||
       !_wcsicmp( arg, L"off"   )  )
    {
        *m_dataPtr = false;
        return true;
    }

    if(swscanf_s( arg, L"%d", &num ) == 0)
    {
        wprintf( L"Expecting a boolean for parameter '%s': %s\n\n", m_szName, arg );
        return false;
    }

    *m_dataPtr = (num != 0);
    return true;
}

//--//

CLR_RT_ParseOptions::Parameter_Integer::Parameter_Integer( int* data, const wchar_t* szName, const wchar_t* szDescription ) : CLR_RT_ParseOptions::Parameter( szName, szDescription )
{
    m_dataPtr = data ? data : &m_dataParsed;
}

bool CLR_RT_ParseOptions::Parameter_Integer::Parse( const wchar_t* arg )
{
    if(swscanf_s( arg, L"%d", m_dataPtr ) == 0)
    {
        wprintf( L"Expecting a number for parameter '%s': %s\n\n", m_szName, arg );
        return false;
    }

    return true;
}

//--//

CLR_RT_ParseOptions::Parameter_Float::Parameter_Float( float* data, const wchar_t* szName, const wchar_t* szDescription ) : CLR_RT_ParseOptions::Parameter( szName, szDescription )
{
    m_dataPtr = data ? data : &m_dataParsed;
}

bool CLR_RT_ParseOptions::Parameter_Float::Parse( const wchar_t* arg )
{
    if(swscanf_s( arg, L"%f", m_dataPtr ) == 0)
    {
        wprintf( L"Expecting a number for parameter '%s': %s\n\n", m_szName, arg );
        return false;
    }

    return true;
}

//--//

CLR_RT_ParseOptions::Command::Command( const wchar_t* szName, const wchar_t* szDescription )
{
    m_szName        = szName;
    m_szDescription = szDescription;
}

bool CLR_RT_ParseOptions::Command::Parse( CLR_RT_StringVector& argv, size_t& pos, CLR_RT_ParseOptions& options )
{
    size_t argc = argv.size();

    for(ParameterListIter it = m_params.begin(); it != m_params.end(); it++)
    {
        Parameter* param = *it;

        if(pos >= argc)
        {
            if(options.m_commands.size() > 1)
            {
                wprintf( L"Missing parameter for option '%s': %s\n\n", m_szName, param->m_szName );
            }
            else
            {                                
                wprintf( L"Missing parameter for option '%s'\n\n", param->m_szName );
            }

            return false;
        }
        else
        {
            if(param->Parse( argv[ pos++ ].c_str() ) == false)
            {
                return false;
            }
        }
    }

    return true;
}

HRESULT CLR_RT_ParseOptions::Command::Execute()
{
    NANOCLR_HEADER();

    NANOCLR_NOCLEANUP_NOLABEL();
}

//--//

CLR_RT_ParseOptions::Command_SetFlag::Command_SetFlag( bool* data, const wchar_t* szName, const wchar_t* szDescription ) : CLR_RT_ParseOptions::Command( szName, szDescription )
{
    m_dataPtr = data ? data : &m_dataParsed;
}

bool CLR_RT_ParseOptions::Command_SetFlag::Parse( CLR_RT_StringVector& argv, size_t& pos, CLR_RT_ParseOptions& options)
{
    *m_dataPtr = true;
    return true;
}

//--//

CLR_RT_ParseOptions::CLR_RT_ParseOptions()
{
    m_fVerbose = false;
}

HRESULT CLR_RT_ParseOptions::ExtractOptionsFromFile( const wchar_t* szFileName )
{
    NANOCLR_HEADER();

    CLR_RT_StringVector vec;

    NANOCLR_CHECK_HRESULT(CLR_RT_FileStore::ExtractTokensFromFile( szFileName, vec ));

    NANOCLR_CHECK_HRESULT(ProcessOptions( vec ));

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_ParseOptions::ReprocessOptions()
{
    NANOCLR_HEADER();

    if(CommandLineArgs.size() == 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NULL_REFERENCE);
    }

    NANOCLR_SET_AND_LEAVE(ProcessOptions( CommandLineArgs ));

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_ParseOptions::ProcessOptions( CLR_RT_StringVector& argv )
{
    NANOCLR_HEADER();

    if(CommandLineArgs.size() == 0)
    {
        CommandLineArgs = CLR_RT_StringVector(argv);
    }

    size_t argc = argv.size();

    for(size_t i=0; i<argc; )
    {
        CommandListIter it;
        const wchar_t*         arg = argv[ i ].c_str();

        for(it = m_commands.begin(); it != m_commands.end(); it++)
        {
            Command* cmd = *it;

            if(!_wcsicmp( arg, cmd->m_szName ))
            {
                if(m_fVerbose)
                {
                    size_t len = cmd->m_params.size();
                    size_t pos = i;

                    wprintf( L"Processing" );

                    wprintf( L" %s", arg );
                    while(len-- > 0 && pos < argc) wprintf( L" %s", argv[ ++pos ].c_str() );
                    wprintf( L"\n" );
                }

                i++;

                if(cmd->Parse( argv, i, *this ) == false)
                {
                    Usage();
                    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                }


                NANOCLR_CHECK_HRESULT(cmd->Execute());
                break;
            }
        }

        if(it == m_commands.end())
        {
            wprintf( L"Unknown option '%s'\n\n", arg );

            Usage();
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }

    NANOCLR_NOCLEANUP();
}

void CLR_RT_ParseOptions::Usage()
{
    CommandListIter   it;
    ParameterListIter it2;
    size_t            maxCmdLen = 0;

    for(it = m_commands.begin(); it != m_commands.end(); it++)
    {
        Command*     cmd    = *it;
        std::wstring option = cmd->m_szName;
        size_t       cmdLen = wcslen( cmd->m_szName );
        size_t       maxLen = 0;

        wprintf( L"    %s\n", cmd->m_szDescription );
        for(it2 = cmd->m_params.begin(); it2 != cmd->m_params.end(); it2++)
        {
            Parameter* param = *it2;
            size_t     len   = wcslen( param->m_szName );

            option += L" ";
            option += param->m_szName;

            if(maxLen < len) maxLen = len;
        }

        if(m_commands.size() > 1)
        {
            wprintf( L"    %s\n", option.c_str());
        }

        for(it2 = cmd->m_params.begin(); it2 != cmd->m_params.end(); it2++)
        {
            Parameter* param = *it2;

            wprintf( L"      %-*s = %s\n", maxLen, param->m_szName, param->m_szDescription );
        }
        wprintf( L"\n" );
    }
}

void CLR_RT_ParseOptions::PushArguments( int argc, wchar_t* argv[], CLR_RT_StringVector& vec )
{
    for(int i=0; i<argc; i++)
    {
        vec.push_back( argv[ i ] );
    }
}

