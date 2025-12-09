//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_INT 0x7fffffff

#define UTF8_VALID_CONTINUATION(ch) ((ch) >= 0x80 && (ch) <= 0xBF)

#define UTF8_VALID_ASCII(ch) ((ch) >= 0x00 && (ch) <= 0x7F)

#define UTF8_CHECK_CONTINUATION(ch, src)                                                                               \
    ch = (CLR_UINT32) * src++;                                                                                         \
    if (!UTF8_VALID_CONTINUATION(ch))                                                                                  \
    return -1

#define UTF8_LOAD_CONTINUATION(ch, ch2, src)                                                                           \
    ch = (CLR_UINT32) * src++;                                                                                         \
    if (!UTF8_VALID_CONTINUATION(ch))                                                                                  \
        return -1;                                                                                                     \
    ch2 = (ch2 << 6) | (ch & 0x3F)

//--//

int CLR_RT_UnicodeHelper::CountNumberOfCharacters(int max)
{
    NATIVE_PROFILE_CLR_CORE();
    const CLR_UINT8 *pSrc = m_inputUTF8;
    int num = 0;
    // If 'max' is negative, treat it as unlimited by setting 'maxRemaining' to the maximum possible integer value.
    // Otherwise, use the provided 'max' value.
    int maxRemaining = (max < 0) ? MAX_INT : max;

    while (maxRemaining > 0 && *pSrc)
    {
        CLR_UINT32 ch = (CLR_UINT32)*pSrc++;
        maxRemaining--;
        // Treat embedded null as null terminator
        if (ch == 0)
        {
            break;
        }

        // ASCII
        if (ch < 0x80)
        {
            num++;
        }
        // UTF-8 two bytes
        else if ((ch & 0xE0) == 0xC0)
        {
            // 2-byte sequence
            if (maxRemaining >= 1 && pSrc[0] != 0)
            {
                if (UTF8_VALID_CONTINUATION(pSrc[0]))
                {
                    CLR_UINT32 fullCh = ((ch & 0x1F) << 6) | (pSrc[0] & 0x3F);
                    if (fullCh >= 0x80)
                    {
                        // Valid
                        num++;
                        pSrc++;
                        maxRemaining--;
                        continue;
                    }
                }
            }
            // Invalid sequence
            num++;
        }
        // UTF-8 three bytes
        else if ((ch & 0xF0) == 0xE0)
        {
            // 3-byte sequence
            if (maxRemaining >= 2 && pSrc[0] != 0 && pSrc[1] != 0)
            {
                int validCount = 0;
                if (UTF8_VALID_CONTINUATION(pSrc[0]))
                {
                    validCount++;
                }

                if (validCount == 1 && UTF8_VALID_CONTINUATION(pSrc[1]))
                {
                    validCount++;
                }

                if (validCount == 2)
                {
                    CLR_UINT32 fullCh = ((ch & 0x0F) << 12) | ((pSrc[0] & 0x3F) << 6) | (pSrc[1] & 0x3F);
                    if (fullCh >= 0x0800 && !(fullCh >= 0xD800 && fullCh <= 0xDFFF))
                    {
                        // Valid
                        num++;
                        pSrc += 2;
                        maxRemaining -= 2;
                        continue;
                    }
                }
                else
                {
                    // Advance over valid continuation bytes in invalid sequence
                    pSrc += validCount;
                    maxRemaining -= validCount;
                }
            }
            // Invalid sequence
            num++;
        }
        // UTF-8 four bytes
        else if ((ch & 0xF8) == 0xF0)
        {
            // 4-byte sequence
            if (maxRemaining >= 3 && pSrc[0] != 0 && pSrc[1] != 0 && pSrc[2] != 0)
            {
                // Validate each continuation byte individually
                int validCount = 0;

                if (UTF8_VALID_CONTINUATION(pSrc[0]))
                {
                    validCount++;
                }

                if (validCount == 1 && UTF8_VALID_CONTINUATION(pSrc[1]))
                {
                    validCount++;
                }

                if (validCount == 2 && UTF8_VALID_CONTINUATION(pSrc[2]))
                {
                    validCount++;
                }

                if (validCount == 3)
                {
                    CLR_UINT32 fullCh =
                        ((ch & 0x07) << 18) | ((pSrc[0] & 0x3F) << 12) | ((pSrc[1] & 0x3F) << 6) | (pSrc[2] & 0x3F);
                    // Strict range check
                    if (fullCh >= 0x10000 && fullCh <= 0x10FFFF)
                    {
                        // Valid supplementary character
                        num += 2;
                        pSrc += 3;
                        maxRemaining -= 3;
                        continue;
                    }
                }
                else
                {
                    // Advance past all continuation bytes in invalid sequence
                    pSrc += validCount;
                    maxRemaining -= validCount;
                }
            }
            // Count invalid sequence
            num++;
        }
        else
        {
            // Invalid starter byte
            num++;
        }
    }

    return num;
}

int CLR_RT_UnicodeHelper::CountNumberOfBytes(int max)
{
    NATIVE_PROFILE_CLR_CORE();
    const CLR_UINT16 *pSrc = m_inputUTF16;
    int num = 0;

    while (max != 0 && *pSrc)
    {
        CLR_UINT16 ch = *pSrc++;
        if (max > 0)
        {
            max--;
        }

        if (ch < 0x0080)
        {
            num += 1;
        }
        else if (ch < 0x0800)
        {
            num += 2;
        }
        else if (ch >= HIGH_SURROGATE_START && ch <= HIGH_SURROGATE_END)
        {
            if (*pSrc >= LOW_SURROGATE_START && *pSrc <= LOW_SURROGATE_END)
            {
                num += 4;
                pSrc++;
                if (max > 0)
                    max--;
            }
            else
            {
                num += 3;
            }
        }
        else if (ch >= LOW_SURROGATE_START && ch <= LOW_SURROGATE_END)
        {
            num += 3;
        }
        else
        {
            num += 3;
        }
    }

    return num;
}

//--//

// dev note: need the pragma below because there are a couple of 'smart' hacks in
// the switch cases to improve the algorithm
#ifdef __GNUC__
#pragma GCC diagnostic push
// -Wimplicit-fallthrough option was added in GCC 7
#if (__GNUC__ >= 7)
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif
#endif

bool CLR_RT_UnicodeHelper::ConvertFromUTF8(int iMaxChars, bool fJustMove, int iMaxBytes)
{
    NATIVE_PROFILE_CLR_CORE();
    const CLR_UINT8 *inputUTF8 = m_inputUTF8;
    CLR_UINT16 *outputUTF16 = m_outputUTF16;
    int outputUTF16_size = m_outputUTF16_size;
    bool res = true;

    if (iMaxBytes == -1)
    {
        iMaxBytes = 0x7FFFFFFF;
    }

    while (iMaxChars != 0 && iMaxBytes > 0)
    {
        CLR_UINT32 ch = (CLR_UINT32)*inputUTF8++;
        iMaxBytes--;

        if (ch == 0)
        {
            // Treat embedded null as null terminator
            break;
        }

        // ASCII
        if (ch < 0x80)
        {
            if (!fJustMove)
            {
                if (outputUTF16_size < 1)
                {
                    inputUTF8--;
                    res = false;
                    break;
                }
                *outputUTF16++ = (CLR_UINT16)ch;
                outputUTF16_size--;
            }

            if (iMaxChars > 0)
            {
                iMaxChars--;
            }
        }
        // UTF-8 two bytes
        else if ((ch & 0xE0) == 0xC0)
        {
            if (iMaxBytes < 1)
            {
                // Not enough bytes remaining - output replacement and don't advance
                inputUTF8--;
                iMaxBytes++;
                goto invalid_sequence;
            }

            CLR_UINT32 ch2 = (CLR_UINT32)inputUTF8[0]; // Lookahead without advancing

            if (!UTF8_VALID_CONTINUATION(ch2))
            {
                goto invalid_sequence;
            }

            CLR_UINT32 fullCh = ((ch & 0x1F) << 6) | (ch2 & 0x3F);

            if (fullCh < 0x80)
            {
                // Overlong encoding - advance past the continuation byte
                inputUTF8++;
                iMaxBytes--;
                goto invalid_sequence;
            }

            // Valid sequence: advance and process
            inputUTF8++;
            iMaxBytes--;

            if (!fJustMove)
            {
                if (outputUTF16_size < 1)
                {
                    inputUTF8 -= 2;
                    iMaxBytes += 2;
                    res = false;
                    break;
                }
                *outputUTF16++ = (CLR_UINT16)fullCh;
                outputUTF16_size--;
            }

            if (iMaxChars > 0)
            {
                iMaxChars--;
            }
        }
        // UTF-8 three bytes
        else if ((ch & 0xF0) == 0xE0)
        {
            if (iMaxBytes < 2)
            {
                // Not enough bytes remaining
                inputUTF8--;
                iMaxBytes++;
                goto invalid_sequence;
            }

            CLR_UINT32 ch2 = (CLR_UINT32)inputUTF8[0];
            CLR_UINT32 ch3 = (CLR_UINT32)inputUTF8[1];
            int validCount = 0;

            if (UTF8_VALID_CONTINUATION(ch2))
            {
                validCount++;
            }

            if (validCount == 1 && UTF8_VALID_CONTINUATION(ch3))
            {
                validCount++;
            }

            if (validCount < 2)
            {
                // Advance only over valid continuation bytes before outputting replacement
                inputUTF8 += validCount;
                iMaxBytes -= validCount;
                goto invalid_sequence;
            }

            CLR_UINT32 fullCh = ((ch & 0x0F) << 12) | ((ch2 & 0x3F) << 6) | (ch3 & 0x3F);

            // Check for valid range and surrogates
            if (fullCh < 0x0800 || (fullCh >= 0xD800 && fullCh <= 0xDFFF))
            {
                // Invalid - advance past all continuation bytes
                inputUTF8 += 2;
                iMaxBytes -= 2;
                goto invalid_sequence;
            }

            inputUTF8 += 2;
            iMaxBytes -= 2;

            if (!fJustMove)
            {
                if (outputUTF16_size < 1)
                {
                    inputUTF8 -= 3;
                    iMaxBytes += 3;
                    res = false;
                    break;
                }
                *outputUTF16++ = (CLR_UINT16)fullCh;
                outputUTF16_size--;
            }

            if (iMaxChars > 0)
            {
                iMaxChars--;
            }
        }
        // UTF-8 four bytes
        else if ((ch & 0xF8) == 0xF0)
        {
            // Check if we have at least 3 continuation bytes
            if (iMaxBytes < 3)
            {
                // Not enough bytes remaining
                inputUTF8--;
                iMaxBytes++;
                goto invalid_sequence;
            }

            // Look ahead at continuation bytes
            CLR_UINT32 ch2 = (CLR_UINT32)inputUTF8[0];
            CLR_UINT32 ch3 = (CLR_UINT32)inputUTF8[1];
            CLR_UINT32 ch4 = (CLR_UINT32)inputUTF8[2];

            // Validate each continuation byte individually
            int validCount = 0;

            if (UTF8_VALID_CONTINUATION(ch2))
            {
                validCount++;
            }

            if (validCount == 1 && UTF8_VALID_CONTINUATION(ch3))
            {
                validCount++;
            }

            if (validCount == 2 && UTF8_VALID_CONTINUATION(ch4))
            {
                validCount++;
            }

            if (validCount < 3)
            {
                // Advance only over valid continuation bytes
                inputUTF8 += validCount;
                iMaxBytes -= validCount;
                goto invalid_sequence;
            }

            // All continuation bytes are valid, check range
            CLR_UINT32 fullCh = ((ch & 0x07) << 18) | ((ch2 & 0x3F) << 12) | ((ch3 & 0x3F) << 6) | (ch4 & 0x3F);

            // Strict range check
            if (fullCh < 0x10000 || fullCh > 0x10FFFF)
            {
                // Invalid - advance past all continuation bytes
                inputUTF8 += 3;
                iMaxBytes -= 3;
                goto invalid_sequence;
            }

            // Valid sequence: advance pointer
            inputUTF8 += 3;
            iMaxBytes -= 3;

            if (!fJustMove)
            {
                if (outputUTF16_size < 2)
                {
                    inputUTF8 -= 4;
                    iMaxBytes += 4;
                    res = false;
                    break;
                }

                CLR_UINT32 temp = fullCh - 0x10000;
                *outputUTF16++ = (CLR_UINT16)((temp >> 10) + HIGH_SURROGATE_START);
                *outputUTF16++ = (CLR_UINT16)((temp & 0x3FF) + LOW_SURROGATE_START);
                outputUTF16_size -= 2;
            }

            if (iMaxChars > 0)
            {
                if (iMaxChars == 1)
                {
                    inputUTF8 -= 4;
                    iMaxBytes += 4;
                    res = false;
                    break;
                }
                iMaxChars -= 2;
            }
        }
        else
        {
            goto invalid_sequence;
        }

        continue;

    invalid_sequence:
        // Output replacement character
        if (!fJustMove)
        {
            if (outputUTF16_size < 1)
            {
                res = false;
                break;
            }
            *outputUTF16++ = 0xFFFD;
            outputUTF16_size--;
        }

        if (iMaxChars > 0)
        {
            iMaxChars--;
        }

        // Pointer has already been advanced appropriately before jumping here
    }

    if (!fJustMove)
    {
        if (outputUTF16_size >= 1)
        {
            outputUTF16[0] = 0;
        }
    }

    m_inputUTF8 = inputUTF8;
    m_outputUTF16 = outputUTF16;
    m_outputUTF16_size = outputUTF16_size;

    return res;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

// move backward in the UTF8 input
bool CLR_RT_UnicodeHelper::MoveBackwardInUTF8(const char *utf8StringStart, int iMaxChars)
{
    // Validate input parameters
    const CLR_UINT8 *startBoundary = (const CLR_UINT8 *)utf8StringStart;
    if (m_inputUTF8 <= startBoundary || iMaxChars < 1)
    {
        return false;
    }

    const CLR_UINT8 *currentPos = m_inputUTF8;
    int movedChars = 0;

    while (movedChars < iMaxChars && currentPos > startBoundary)
    {
        // Move back one byte
        currentPos--;
        movedChars++;

        // Find start of UTF-8 sequence
        const CLR_UINT8 *sequenceStart = currentPos;
        while (sequenceStart > startBoundary && (*(sequenceStart - 1) & 0xC0) == 0x80) // Previous byte is continuation
        {
            sequenceStart--;
        }

        // Validate sequence start byte
        CLR_UINT8 leadByte = *sequenceStart;
        int expectedLength = 0;

        if ((leadByte & 0x80) == 0x00)
        {
            expectedLength = 1; // ASCII
        }
        else if ((leadByte & 0xE0) == 0xC0)
        {
            expectedLength = 2; // 2-byte sequence
        }
        else if ((leadByte & 0xF0) == 0xE0)
        {
            expectedLength = 3; // 3-byte sequence
        }
        else if ((leadByte & 0xF8) == 0xF0)
        {
            expectedLength = 4; // 4-byte sequence
        }
        else
        {
            // Invalid lead byte - treat as single byte character
            currentPos = sequenceStart;
            continue;
        }

        // Check if we have a complete sequence
        bool validSequence = true;
        for (int i = 1; i < expectedLength; i++)
        {
            if (sequenceStart + i >= m_inputUTF8)
            {
                // Sequence extends beyond original position
                validSequence = false;
                break;
            }
            if ((sequenceStart[i] & 0xC0) != 0x80)
            {
                // Invalid continuation byte
                validSequence = false;
                break;
            }
        }

        if (validSequence)
        {
            // Valid sequence - move to its start
            currentPos = sequenceStart;
        }
        else
        {
            // Invalid sequence - treat lead byte as single character
            currentPos = sequenceStart;
        }
    }

    // Only update position if we moved full count
    if (movedChars == iMaxChars)
    {
        m_inputUTF8 = currentPos;
        return true;
    }

    // Partial move - update position but return failure
    m_inputUTF8 = currentPos;
    return false;
}

bool CLR_RT_UnicodeHelper::ConvertToUTF8(int iMaxChars, bool fJustMove)
{
    NATIVE_PROFILE_CLR_CORE();
    const CLR_UINT16 *inputUTF16 = m_inputUTF16;
    CLR_UINT8 *outputUTF8 = m_outputUTF8;
    int outputUTF8_size = m_outputUTF8_size;
    CLR_UINT32 ch;
    bool res = true;

    while (iMaxChars > 0)
    {
        ch = (CLR_UINT32)*inputUTF16++;
        // Removed break on null to handle embedded nulls
        iMaxChars--;

        if (ch < 0x0080)
        {
            if (!fJustMove)
            {
                if (outputUTF8_size < 1)
                {
                    inputUTF16--;
                    res = false;
                    break;
                }
                *outputUTF8++ = (CLR_UINT8)ch;
                outputUTF8_size--;
            }
        }
        else if (ch < 0x0800)
        {
            if (!fJustMove)
            {
                if (outputUTF8_size < 2)
                {
                    inputUTF16--;
                    res = false;
                    break;
                }
                *outputUTF8++ = 0xC0 | (ch >> 6);
                *outputUTF8++ = 0x80 | (ch & 0x3F);
                outputUTF8_size -= 2;
            }
        }
        else if (ch >= HIGH_SURROGATE_START && ch <= HIGH_SURROGATE_END)
        {
            if (iMaxChars < 1)
            {
                // Unpaired high surrogate
                if (!fJustMove)
                {
                    if (outputUTF8_size < 3)
                    {
                        inputUTF16--;
                        res = false;
                        break;
                    }
                    *outputUTF8++ = 0xEF;
                    *outputUTF8++ = 0xBF;
                    *outputUTF8++ = 0xBD; // U+FFFD
                    outputUTF8_size -= 3;
                }
                // iMaxChars is already 0, so loop will exit
                continue;
            }

            CLR_UINT16 ch2 = *inputUTF16++;
            iMaxChars--;

            if (ch2 < LOW_SURROGATE_START || ch2 > LOW_SURROGATE_END)
            {
                // Invalid surrogate pair - high surrogate followed by non-low surrogate
                if (!fJustMove)
                {
                    if (outputUTF8_size < 3)
                    {
                        inputUTF16 -= 2;
                        res = false;
                        break;
                    }
                    *outputUTF8++ = 0xEF;
                    *outputUTF8++ = 0xBF;
                    *outputUTF8++ = 0xBD; // U+FFFD
                    outputUTF8_size -= 3;
                }
                // Put back the second character for next iteration
                inputUTF16--;
                iMaxChars++;
                continue;
            }

            ch = 0x10000 + (((ch - HIGH_SURROGATE_START) << 10) | (ch2 - LOW_SURROGATE_START));

            if (!fJustMove)
            {
                if (outputUTF8_size < 4)
                {
                    inputUTF16 -= 2;
                    res = false;
                    break;
                }
                *outputUTF8++ = 0xF0 | (ch >> 18);
                *outputUTF8++ = 0x80 | ((ch >> 12) & 0x3F);
                *outputUTF8++ = 0x80 | ((ch >> 6) & 0x3F);
                *outputUTF8++ = 0x80 | (ch & 0x3F);
                outputUTF8_size -= 4;
            }
        }
        else if (ch >= LOW_SURROGATE_START && ch <= LOW_SURROGATE_END)
        {
            // Unpaired low surrogate
            if (!fJustMove)
            {
                if (outputUTF8_size < 3)
                {
                    inputUTF16--;
                    res = false;
                    break;
                }
                *outputUTF8++ = 0xEF;
                *outputUTF8++ = 0xBF;
                *outputUTF8++ = 0xBD; // U+FFFD
                outputUTF8_size -= 3;
            }
        }
        else
        {
            if (!fJustMove)
            {
                if (outputUTF8_size < 3)
                {
                    inputUTF16--;
                    res = false;
                    break;
                }
                *outputUTF8++ = 0xE0 | (ch >> 12);
                *outputUTF8++ = 0x80 | ((ch >> 6) & 0x3F);
                *outputUTF8++ = 0x80 | (ch & 0x3F);
                outputUTF8_size -= 3;
            }
        }
    }

    if (res && !fJustMove && outputUTF8_size > 0)
    {
        *outputUTF8 = 0;
    }

    m_inputUTF16 = inputUTF16;
    m_outputUTF8 = outputUTF8;
    m_outputUTF8_size = outputUTF8_size;

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(VIRTUAL_DEVICE)

void CLR_RT_UnicodeHelper::ConvertToUTF8(const std::wstring &src, std::string &dst)
{
    if (src.size())
    {
        char *szBuf;
        int iSize;

        iSize = ::WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, nullptr, 0, nullptr, nullptr);

        if (iSize > 0)
        {
            szBuf = new char[iSize];

            iSize = ::WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, szBuf, iSize, nullptr, nullptr);
            if (iSize > 0)
            {
                dst = szBuf;
            }

            delete[] szBuf;
        }
    }
    else
    {
        dst.erase();
    }
}

void CLR_RT_UnicodeHelper::ConvertFromUTF8(const std::string &src, std::wstring &dst)
{
    if (src.size())
    {
        wchar_t *szBuf;
        int iSize;

        iSize = ::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, nullptr, 0);

        if (iSize > 0)
        {
            szBuf = new wchar_t[iSize];

            iSize = ::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, szBuf, iSize);
            if (iSize > 0)
            {
                dst = szBuf;
            }

            delete[] szBuf;
        }
    }
    else
    {
        dst.erase();
    }
}

#endif

UnicodeString::UnicodeString()
{
    m_wCharArray = nullptr;
    m_length = 0;
}

UnicodeString::~UnicodeString()
{
    Release();
}

HRESULT UnicodeString::Assign(const char *string)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    int byteLength = 0;

    Release();

    m_unicodeHelper.SetInputUTF8(string);

    m_length = m_unicodeHelper.CountNumberOfCharacters();
    if (m_length < 0)
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

    byteLength = (m_length + 1) * sizeof(CLR_UINT16);

    m_wCharArray = (CLR_UINT16 *)CLR_RT_Memory::Allocate(byteLength);
    CHECK_ALLOCATION(m_wCharArray);

    m_unicodeHelper.m_outputUTF16 = m_wCharArray;
    m_unicodeHelper.m_outputUTF16_size = m_length + 1;
    m_unicodeHelper.SetInputUTF8(string); // Reset

    if (!m_unicodeHelper.ConvertFromUTF8(-1, false))
    {
        CLR_RT_Memory::Release(m_wCharArray);
        m_wCharArray = NULL;
        m_length = 0;
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    m_wCharArray[m_length] = 0;

    NANOCLR_NOCLEANUP();
}

void UnicodeString::Release()
{
    if (m_wCharArray != nullptr)
    {
        CLR_RT_Memory::Release(m_wCharArray);
        m_wCharArray = NULL;
    }
    m_length = 0;
}
