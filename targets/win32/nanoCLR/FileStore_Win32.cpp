//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"



////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CLR_RT_FileStore::LoadFile( const wchar_t* szFile, CLR_RT_Buffer& vec )
{
    NANOCLR_HEADER();

    FILE* stream;
    if(_wfopen_s(&stream, szFile, L"rb" ) != 0)
    {
        wprintf( L"Cannot open '%s'!\n", szFile );
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    /*********/ fseek( stream, 0, SEEK_END );
    long size = ftell( stream              );
    /*********/ fseek( stream, 0, SEEK_SET );

    vec.resize( size );

    if(vec.size() && fread( &vec[ 0 ], vec.size(), 1, stream ) != 1)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }


    NANOCLR_CLEANUP();

    if(stream)
    {
        fclose( stream );
    }

    NANOCLR_CLEANUP_END();
}

HRESULT CLR_RT_FileStore::SaveFile( const wchar_t* szFile, const CLR_RT_Buffer& vec )
{
    NANOCLR_HEADER();
        
    const CLR_UINT8* buf = NULL;
    size_t size = vec.size();

    if(size > 0)
    {
        buf = &vec[ 0 ];
    }

    NANOCLR_SET_AND_LEAVE(SaveFile( szFile, buf, size ));

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_FileStore::SaveFile( const wchar_t* szFile, const CLR_UINT8* buf, size_t size )
{
    NANOCLR_HEADER();
    FILE* stream;

    if(_wfopen_s(&stream, szFile, L"wb" ) != 0)
    {
        wprintf( L"Cannot open '%s' for writing!\n", szFile );
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    if(buf != NULL && size != 0)
    {
        if(fwrite( buf, size, 1, stream ) != 1)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
    }

    NANOCLR_CLEANUP();

    if(stream)
    {
        fclose( stream );
    }

    NANOCLR_CLEANUP_END();
}

//--//

HRESULT CLR_RT_FileStore::ExtractTokensFromFile( const wchar_t* szFileName, std::vector< std::wstring >& vec, const wchar_t* separators, bool fNoComments )
{
    NANOCLR_HEADER();

    CLR_RT_Buffer buf;

    NANOCLR_CHECK_HRESULT(LoadFile( szFileName, buf ));

    ExtractTokens( buf, vec, separators, fNoComments );

    NANOCLR_NOCLEANUP();
}

void CLR_RT_FileStore::ExtractTokens( const CLR_RT_Buffer& buf, CLR_RT_StringVector& vec, const wchar_t* separators, bool fNoComments )
{
    std::wstring tmp;
    char*        szBufA = NULL;
    wchar_t*       szBufW = NULL;
    const wchar_t*      src;
    size_t       len;

    if (buf.size() == 0) { return; }
    else if(buf.size() >= 2 && buf[ 0 ] == 0xFF && buf[ 1 ] == 0xFE)
    {
        len = (buf.size() - 2) / sizeof(wchar_t);

        src = (const wchar_t*)&buf[ 2 ];
    }
    else
    {
        len = buf.size() / sizeof(char);

        szBufA = new char[ len+1 ]; memcpy( szBufA, &buf[ 0 ], len ); szBufA[ len ] = 0;
        CLR_RT_UnicodeHelper::ConvertFromUTF8( szBufA, tmp );

        src = tmp.c_str();
        len = tmp.size();
    }

    //--//

    szBufW = new wchar_t[ len+1 ]; memcpy( szBufW, src, len * sizeof(wchar_t) ); szBufW[ len ] = 0;

    vec.reserve( len / 60 );

    ExtractTokensFromBuffer( szBufW, vec, separators, fNoComments );

    //--//

    delete[] szBufW;
    delete[] szBufA;
}

void CLR_RT_FileStore::ExtractTokensFromBuffer( wchar_t* szLine, CLR_RT_StringVector& vec, const wchar_t* separators, bool fNoComments )
{
    while(*szLine)
    {
        wchar_t* szNextLine = szLine;

        while(*szNextLine)
        {
            if(*szNextLine == '\r' || *szNextLine == '\n')
            {
                while(*szNextLine == '\r' || *szNextLine == '\n')
                {
                    *szNextLine++ = 0;
                }

                break;
            }

            szNextLine++;
        }

        if(fNoComments == false || szLine[ 0 ] != '#')
        {
            CLR_RT_FileStore::ExtractTokensFromString( szLine, vec, separators );
        }

        szLine = szNextLine;
    }
}

void CLR_RT_FileStore::ExtractTokensFromString( const wchar_t* szLine, CLR_RT_StringVector& vec, const wchar_t* separators )
{
    if(separators)
    {
        while(szLine[ 0 ] && wcschr( separators, szLine[0] )) szLine++;

        while(szLine[ 0 ])
        {
            wchar_t  token        [ 2048 ];
            wchar_t  tokenExpanded[ 2048 ];
            bool   fQuote = false;
            size_t pos;

            for(pos=0; pos<MAXSTRLEN(token); )
            {
                wchar_t c = *szLine++;

                if(c == 0)
                {
                    szLine--;
                    break;
                }

                if(fQuote)
                {
                    if(c == '\\')
                    {
                        c = *szLine++;;
                    }
                    else
                    if(c == '"')
                    {
                        fQuote = false;
                        continue;
                    }
                }
                else if(c == '"')
                {
                    fQuote = true;
                    continue;
                }
                else
                {
                    if(wcschr( separators, c )) break;
                }

                token[ pos++ ] = c;
            }
            token[ pos ] = 0;

            ::ExpandEnvironmentStringsW( token, tokenExpanded, MAXSTRLEN(tokenExpanded) );
            vec.push_back( tokenExpanded );

            while(szLine[ 0 ] && wcschr( separators, szLine[ 0 ] )) szLine++;
        }
    }
    else
    {
        vec.push_back( szLine );
    }
}

