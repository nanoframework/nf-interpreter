//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"
#include <nanoprintf.h>

// must be big enough to fit the biggest number
// decorated with negative signs, group separators, etc.
#define FORMAT_RESULT_BUFFER_SIZE 128
#define FORMAT_FMTSTR_BUFFER_SIZE 10

bool Library_corlib_native_System_Number::ParseFormat(char *format, char *formatChar, int *precision)
{
    bool ret = true;

    // parse received format
    if (format != NULL && format[0] != 0)
    {
        *formatChar = format[0];
        *precision = -1;

        char *cur = format;
        while (*++cur != 0)
        {
            if ((*cur) < '0' || (*cur) > '9')
            {
                ret = false;
                break;
            }

            if (*precision == -1)
            {
                *precision = ((*cur) - '0');
            }
            else
            {
                *precision = (*precision) * 10 + ((*cur) - '0');
            }
        }
    }
    else
    {
        // defaults
        *formatChar = 'G';
        *precision = -1;
    }

    return ret;
}

bool Library_corlib_native_System_Number::ValidateFormatChar(char *formatChar, bool isInteger)
{
    switch (*formatChar)
    {
        // these need to be integers
        case 'x':
        case 'X':
        case 'd':
        case 'D':
            return isInteger;

        // all the others shoudl be OK or will be caught afterwards
        default:
            return true;
    }
}

bool Library_corlib_native_System_Number::GetFormatSpec(char *format, bool isInteger, char *formatChar, int *precision)
{
    if (ParseFormat(format, formatChar, precision))
    {
        return ValidateFormatChar(formatChar, isInteger);
    }
    else
    {
        return false;
    }
}

int Library_corlib_native_System_Number::DoPrintfOnDataType(char *buffer, char *formatStr, CLR_RT_HeapBlock *value)
{
    int ret = -1;

    CLR_DataType dataType = value->DataType();

    switch (dataType)
    {
        case DATATYPE_I1:
            ret = (int)snprintf(buffer, FORMAT_RESULT_BUFFER_SIZE, formatStr, value->NumericByRef().s1);
            break;
        case DATATYPE_U1:
            ret = (int)snprintf(buffer, FORMAT_RESULT_BUFFER_SIZE, formatStr, value->NumericByRef().u1);
            break;
        case DATATYPE_I2:
            ret = (int)snprintf(buffer, FORMAT_RESULT_BUFFER_SIZE, formatStr, value->NumericByRef().s2);
            break;
        case DATATYPE_U2:
            ret = (int)snprintf(buffer, FORMAT_RESULT_BUFFER_SIZE, formatStr, value->NumericByRef().u2);
            break;
        case DATATYPE_I4:
            ret = (int)snprintf(buffer, FORMAT_RESULT_BUFFER_SIZE, formatStr, value->NumericByRef().s4);
            break;
        case DATATYPE_U4:
            ret = (int)snprintf(buffer, FORMAT_RESULT_BUFFER_SIZE, formatStr, value->NumericByRef().u4);
            break;
        case DATATYPE_I8:
            ret = (int)(int)
                snprintf(buffer, FORMAT_RESULT_BUFFER_SIZE, formatStr, (CLR_INT64_TEMP_CAST)value->NumericByRef().s8);
            break;
        case DATATYPE_U8:
            ret = (int)
                snprintf(buffer, FORMAT_RESULT_BUFFER_SIZE, formatStr, (CLR_UINT64_TEMP_CAST)value->NumericByRef().u8);
            break;
        case DATATYPE_R4:
            ret = (int)snprintf(buffer, FORMAT_RESULT_BUFFER_SIZE, formatStr, value->NumericByRef().r4);
            break;
        case DATATYPE_R8:
            ret = (int)
                snprintf(buffer, FORMAT_RESULT_BUFFER_SIZE, formatStr, (CLR_DOUBLE_TEMP_CAST)value->NumericByRef().r8);
            break;
        default:
            break;
    }

    // assure string valid even in cases when nothing was written
    if (ret >= 0)
    {
        buffer[ret] = 0;
    }

    return ret;
}

const char *Library_corlib_native_System_Number::GetPrintfLengthModifier(CLR_DataType dataType)
{
    const char *ret = (dataType == DATATYPE_I1 || dataType == DATATYPE_U1)   ? ""
                      : (dataType == DATATYPE_I2 || dataType == DATATYPE_U2) ? ""
                      : (dataType == DATATYPE_I4 || dataType == DATATYPE_U4) ? ""
                      : (dataType == DATATYPE_I8 || dataType == DATATYPE_U8) ? "ll"
                                                                             : "";
    return ret;
}

bool Library_corlib_native_System_Number::IsSignedIntegerDataType(CLR_DataType dataType)
{
    bool ret =
        (dataType == DATATYPE_I1 || dataType == DATATYPE_I2 || dataType == DATATYPE_I4 || dataType == DATATYPE_I8);
    return ret;
}

bool Library_corlib_native_System_Number::IsUnsignedIntegerDataType(CLR_DataType dataType)
{
    bool ret =
        (dataType == DATATYPE_U1 || dataType == DATATYPE_U2 || dataType == DATATYPE_U4 || dataType == DATATYPE_U8);
    return ret;
}

bool Library_corlib_native_System_Number::IsIntegerDataType(CLR_DataType dataType)
{
    bool ret = IsSignedIntegerDataType(dataType) || IsUnsignedIntegerDataType(dataType);
    return ret;
}

int Library_corlib_native_System_Number::GetStrLen(char *buffer)
{
    int ret = 0;
    for (;;)
    {
        if (buffer[ret] == 0)
        {
            break;
        }
        ret++;
    }
    return ret;
}

int Library_corlib_native_System_Number::GetDotIndex(char *buffer, int bufferContentLength)
{
    int ret = -1;

    for (int i = 0; i < bufferContentLength; i++)
    {
        if (buffer[i] == '.')
        {
            ret = i;
            break;
        }
    }

    return ret;
}

void Library_corlib_native_System_Number::RoundUpNumStr(char *buffer, int *bufferContentLength)
{
    char *c = &buffer[*bufferContentLength - 1];
    for (;;)
    {
        if (*c != '.' && *c != '-')
        {
            *c += 1;
            if (*c <= '9')
                break;
            *c = '0';
        }
        if (c == buffer)
        {
            if (*c == '-')
            {
                memmove(&buffer[2], &buffer[1], *bufferContentLength + 1);
                buffer[1] = '1';
            }
            else
            {
                memmove(&buffer[1], buffer, *bufferContentLength + 1);
                buffer[0] = '1';
            }
            (*bufferContentLength)++;
            break;
        }
        c--;
    }
}

int Library_corlib_native_System_Number::ReplaceNegativeSign(char *buffer, int bufferContentLength, char *negativeSign)
{
    int ret = bufferContentLength;

    if (buffer[0] == '-')
    {
        int negativeSignLength = GetStrLen(negativeSign);

        memmove(&buffer[negativeSignLength], &buffer[1], bufferContentLength);
        memcpy(buffer, negativeSign, negativeSignLength);
        ret += negativeSignLength - 1;
    }

    return ret;
}

int Library_corlib_native_System_Number::ReplaceDecimalSeparator(
    char *buffer,
    int bufferContentLength,
    char *decimalSeparator)
{
    int ret = bufferContentLength;

    int dotIndex = GetDotIndex(buffer, bufferContentLength);
    if (dotIndex != -1)
    {
        int decimalSeparatorLength = GetStrLen(decimalSeparator);

        memmove(&buffer[dotIndex + decimalSeparatorLength], &buffer[dotIndex + 1], bufferContentLength);
        memcpy(&buffer[dotIndex], decimalSeparator, decimalSeparatorLength);
        ret += decimalSeparatorLength - 1;
    }

    return ret;
}

int Library_corlib_native_System_Number::InsertGroupSeparators(
    char *buffer,
    int bufferContentLength,
    int groupSize,
    char *groupSep)
{
    int ret = bufferContentLength;

    int significantDigitsStartAtIndex = 0;
    int significantDigitCount = bufferContentLength - 1;
    int dotIndex = GetDotIndex(buffer, bufferContentLength);
    if (dotIndex != -1)
    {
        significantDigitCount = dotIndex - 1;
    }
    if (buffer[0] == '-')
    {
        significantDigitCount--;
        significantDigitsStartAtIndex++;
    }

    int groupSepLength = GetStrLen(groupSep);
    int groupCount = significantDigitCount / groupSize;
    int plusLength = groupCount * groupSepLength;

    if (plusLength > 0)
    {
        ret = bufferContentLength + plusLength;

        int srcIdx = bufferContentLength;
        int tgtIdx = ret;

        if (dotIndex != -1)
        {
            int fractionPostfixWithDotLength = bufferContentLength - dotIndex;
            memmove(&buffer[dotIndex + plusLength], &buffer[dotIndex], fractionPostfixWithDotLength);
            srcIdx -= fractionPostfixWithDotLength;
            tgtIdx -= fractionPostfixWithDotLength;
        }

        for (;;)
        {
            if ((srcIdx - significantDigitsStartAtIndex) <= groupSize)
                break;

            tgtIdx -= groupSize;
            srcIdx -= groupSize;
            memmove(&buffer[tgtIdx], &buffer[srcIdx], groupSize);
            tgtIdx -= groupSepLength;
            memcpy(&buffer[tgtIdx], groupSep, groupSepLength);
        }
    }

    buffer[ret] = 0;

    return ret;
}

int Library_corlib_native_System_Number::Format_G(
    char *buffer,
    CLR_RT_HeapBlock *value,
    char formatChar,
    int precision,
    char *negativeSign,
    char *decimalSeparator)
{
    int ret = -1;

    CLR_DataType dataType = value->DataType();

    bool isIntegerDataType = IsIntegerDataType(dataType);

    // set default precision for the conversion
    int defaultPrecision = 0;
    switch (dataType)
    {
        case DATATYPE_I1:
        case DATATYPE_U1:
            defaultPrecision = 3;
            break;
        case DATATYPE_I2:
        case DATATYPE_U2:
            defaultPrecision = 5;
            break;
        case DATATYPE_I4:
        case DATATYPE_U4:
            defaultPrecision = 10;
            break;
        case DATATYPE_I8:
            defaultPrecision = 19;
            break;
        case DATATYPE_U8:
            defaultPrecision = 20;
            break;
        case DATATYPE_R4:
            // from .NET documentation:
            // When used with a Single value, the "G9" format specifier ensures that the original Single value
            // successfully round-trips.
            defaultPrecision = 9;
            break;
        case DATATYPE_R8:
        {
            // from .NET documentation:
            // When used with a Double value, the "G17" format specifier ensures that the original Double value
            // successfully round-trips.
            defaultPrecision = 17;

            CLR_DOUBLE_TEMP_CAST number = (CLR_DOUBLE_TEMP_CAST)value->NumericByRef().r8;

            // check if number is an integer
            if (number == (CLR_INT64_TEMP_CAST)number)
            {
                // this is an integer, set precision to a value achievable by the library
                defaultPrecision = 15;
            }
            break;
        }
        default:
            break;
    }

    int requestedPrecision = precision;

    if (precision == -1)
    {
        // no precision specified, use default
        precision = defaultPrecision;
    }

    int precisionForConversion = precision;

    if (!isIntegerDataType)
    {
        precisionForConversion += 2;
    }

    if (precision > 0)
    {
        // compose format string
        char nonIntegerPrecStr[FORMAT_FMTSTR_BUFFER_SIZE];
        if (!isIntegerDataType)
        {
            snprintf(nonIntegerPrecStr, FORMAT_FMTSTR_BUFFER_SIZE, "0.%d", precisionForConversion);
        }

        char formatStr[FORMAT_FMTSTR_BUFFER_SIZE];
        snprintf(
            formatStr,
            FORMAT_FMTSTR_BUFFER_SIZE,
            "%%%s%s%c",
            (isIntegerDataType) ? "" : nonIntegerPrecStr,
            (isIntegerDataType) ? GetPrintfLengthModifier(dataType) : "",
            (!isIntegerDataType)                  ? formatChar
            : (IsSignedIntegerDataType(dataType)) ? 'd'
                                                  : 'u');

        ret = DoPrintfOnDataType(buffer, formatStr, value);

        if (ret > 0)
        {
            // printf and ToString differs on precision numbers:
            // printf("%.05d", 123.4567890) returns "123.45679"
            // (123.4567890).ToString("G5") returns "123.45"
            // that's why we ask printf for big precision then
            // post process the string here

            bool isNegative = (buffer[0] == '-');
            int offsetBecauseOfNegativeSign = (isNegative ? 1 : 0);
            int savedResultLength = ret;
            int exponent = 0;

            // find the exponent character, start with lower case
            char *e = strchr(buffer, 'e');

            if (!e)
            {
                // try upper case
                e = strchr(buffer, 'E');
            }

            if (e)
            {
                // move past the exponent character
                e++;

                // convert exponent to digits
                exponent = atoi(e);
            }

            if (ret > (precision + offsetBecauseOfNegativeSign))
            {
                int dotIndex = GetDotIndex(buffer, ret);

                int numDigits = 0;

                // leave just the required amount of digits
                if (requestedPrecision <= defaultPrecision)
                {
                    // find the first digit after the dot
                    for (int i = 0; i < ret; i++)
                    {
                        if (buffer[i] >= '0' && buffer[i] <= '9')
                        {
                            numDigits++;

                            if (numDigits == precision)
                            {
                                ret = i + 1;
                                char first_lost_digit = buffer[ret];

                                // handle various situation, like rounding, exponent, rounding errors
                                if (first_lost_digit == '.' && (ret + 1) < savedResultLength)
                                {
                                    first_lost_digit = buffer[ret + 1];
                                    buffer[ret] = 0;
                                }
                                else if (first_lost_digit == 'E' || first_lost_digit == 'e')
                                {
                                    first_lost_digit = buffer[ret - 1];
                                    buffer[ret] = 0;
                                    ret--;
                                }
                                else
                                {
                                    buffer[ret] = 0;
                                }

                                // drop last digit in case it's a rounding digit
                                // conditions are:
                                // - not an integer type
                                // - number of digits is at least 2
                                // - number of digits is greater than the default precision (otherwise we could be
                                // mistaking a valid last fraction digit for a rounding digit)
                                if (!isIntegerDataType && numDigits >= 2 && numDigits >= defaultPrecision &&
                                    buffer[ret - 2] == '0')
                                {
                                    buffer[ret - 1] = 0;
                                    ret--;

                                    break;
                                }

                                if (first_lost_digit >= '5')
                                {
                                    int savedRet = ret;
                                    RoundUpNumStr(buffer, &ret);

                                    if (savedRet < ret)
                                    {
                                        dotIndex++;
                                    }
                                }
                                break;
                            }
                        }
                    }
                }

                if (dotIndex != -1)
                {
                    // strip trailing zeros
                    for (int i = ret - 1; i >= dotIndex; i--)
                    {
                        if (buffer[i] != '0')
                        {
                            break;
                        }

                        buffer[i] = 0;
                        ret--;
                    }

                    // strip trailing dot too
                    if (ret == dotIndex + 1)
                    {
                        buffer[ret - 1] = 0;
                        ret--;
                    }
                }

                if ((dotIndex == -1) || exponent != 0 || (dotIndex > (precision + offsetBecauseOfNegativeSign)))
                {
                    // insert '.', only if request precision requires it
                    // this is: requestedPrecision is specified and is more than 1 (taking into account the sign)
                    if (requestedPrecision > 0 && (ret - offsetBecauseOfNegativeSign) > 1)
                    {
                        memmove(
                            &buffer[2 + offsetBecauseOfNegativeSign],
                            &buffer[1 + offsetBecauseOfNegativeSign],
                            ret - 1);
                        buffer[1 + offsetBecauseOfNegativeSign] = '.';

                        ret++;
                    }

                    // deal with 'E+exp'
                    if (exponent == 0)
                    {
                        exponent = (dotIndex == -1) ? savedResultLength - 1 : dotIndex - 1;
                        exponent -= offsetBecauseOfNegativeSign;
                    }

                    if (formatChar == 'g')
                    {
                        ret += (int)snprintf(&buffer[ret], FORMAT_RESULT_BUFFER_SIZE - ret, "e%+.2d", exponent);
                    }
                    else
                    {
                        ret += (int)snprintf(&buffer[ret], FORMAT_RESULT_BUFFER_SIZE - ret, "E%+.2d", exponent);
                    }
                }
            }

            ret = ReplaceNegativeSign(buffer, ret, negativeSign);
            ret = ReplaceDecimalSeparator(buffer, ret, decimalSeparator);
        }
    }

    return ret;
}

int Library_corlib_native_System_Number::Format_D(
    char *buffer,
    CLR_RT_HeapBlock *value,
    int precision,
    char *negativeSign,
    char *decimalSeparator)
{
    int ret = -1;

    CLR_DataType dataType = value->DataType();

    if (precision == -1)
    {
        precision = 0;
    }

    char formatStr[FORMAT_FMTSTR_BUFFER_SIZE];
    snprintf(
        formatStr,
        FORMAT_FMTSTR_BUFFER_SIZE,
        "%%0%d%s%c",
        precision,
        GetPrintfLengthModifier(dataType),
        (IsSignedIntegerDataType(dataType)) ? 'd' : 'u');

    ret = DoPrintfOnDataType(buffer, formatStr, value);
    if (ret > 0)
    {
        // printf and ToString differs on negative numbers:
        // printf("%05d", -123) returns "-0123"
        // -123.ToString("D5") returns "-00123"
        if (precision != 0 && ret > 1 && precision >= ret && buffer[0] == '-')
        {
            // our buffer defined bigger than the max number string, no worries
            memmove(&buffer[2], &buffer[1], ret);
            buffer[1] = '0';
            ret++;
        }

        ret = ReplaceNegativeSign(buffer, ret, negativeSign);
        ret = ReplaceDecimalSeparator(buffer, ret, decimalSeparator);
    }

    return ret;
}

int Library_corlib_native_System_Number::Format_X(char *buffer, CLR_RT_HeapBlock *value, char formatChar, int precision)
{
    int ret = -1;

    CLR_DataType dataType = value->DataType();

    // set max width for the conversion
    int maxWidth = 0;
    switch (dataType)
    {
        case DATATYPE_I1:
        case DATATYPE_U1:
            maxWidth = 2;
            break;
        case DATATYPE_I2:
        case DATATYPE_U2:
            maxWidth = 4;
            break;
        case DATATYPE_I4:
        case DATATYPE_U4:
            maxWidth = 8;
            break;
        case DATATYPE_I8:
        case DATATYPE_U8:
            maxWidth = 16;
            break;
        default:
            break;
    }

    if (precision == -1)
    {
        // no precision specified
        precision = 0;
    }

    char formatStr[FORMAT_FMTSTR_BUFFER_SIZE];
    snprintf(
        formatStr,
        FORMAT_FMTSTR_BUFFER_SIZE,
        "%%0%d%s%c",
        precision,
        GetPrintfLengthModifier(value->DataType()),
        formatChar); // x or X should return different results

    ret = DoPrintfOnDataType(buffer, formatStr, value);

    if (ret > maxWidth)
    {
        // we have more digits than the max width, so we need to strip the leading ones
        memmove(&buffer[0], &buffer[ret - maxWidth], maxWidth);

        buffer[maxWidth] = 0;
        ret = maxWidth;
    }

    return ret;
}

int Library_corlib_native_System_Number::Format_F(
    char *buffer,
    CLR_RT_HeapBlock *value,
    int precision,
    char *negativeSign,
    char *decimalSeparator)
{
    int ret = -1;

    if (precision == -1)
    {
        precision = 2; // should be equal to NumberFormatInfo.NumberDecimalDigits which isn't implemented in
                       // NF at the moment
    }

    CLR_DataType dataType = value->DataType();

    bool isIntegerDataType = IsIntegerDataType(dataType);

    char formatStr[FORMAT_FMTSTR_BUFFER_SIZE];
    snprintf(
        formatStr,
        FORMAT_FMTSTR_BUFFER_SIZE,
        "%%0.%d%s%c",
        precision,
        (isIntegerDataType) ? GetPrintfLengthModifier(dataType) : "",
        (!isIntegerDataType)                  ? 'f'
        : (IsSignedIntegerDataType(dataType)) ? 'd'
                                              : 'u');

    ret = DoPrintfOnDataType(buffer, formatStr, value);

    // this extra processing is only required for integer types
    if (isIntegerDataType && ret > 0)
    {
        bool isNegative = (buffer[0] == '-');
        int offsetBecauseOfNegativeSign = (isNegative ? 1 : 0);

        int dotIndex = GetDotIndex(buffer, ret);

        // if there is no dot, set the index to the end of the string
        if (dotIndex == -1)
        {
            dotIndex = ret;
        }

        // strip trailing zeros
        while (buffer[offsetBecauseOfNegativeSign] != '.' && buffer[offsetBecauseOfNegativeSign] == '0' && ret > 1)
        {
            memmove(&buffer[offsetBecauseOfNegativeSign], &buffer[offsetBecauseOfNegativeSign + 1], ret);

            ret--;
        };

        if (isIntegerDataType && (precision > 0))
        {
            // insert '.' and...
            buffer[ret++] = '.';

            // ...fill with zeros
            for (int i = 0; i < precision; i++)
            {
                buffer[ret++] = '0';
            }
            buffer[ret] = 0;
        }

        ret = ReplaceNegativeSign(buffer, ret, negativeSign);
        ret = ReplaceDecimalSeparator(buffer, ret, decimalSeparator);
    }

    return ret;
}

int Library_corlib_native_System_Number::Format_N(
    char *buffer,
    CLR_RT_HeapBlock *value,
    int precision,
    char *negativeSign,
    char *decimalSeparator,
    char *numberGroupSeparator,
    CLR_RT_HeapBlock_Array *numberGroupSizes)
{
    int ret = -1;

    if (precision == -1)
    {
        precision = 2; // should be equal to NumberFormatInfo.NumberDecimalDigits which isn't implemented in
                       // NF at the moment
    }

    if (numberGroupSizes->m_numOfElements != 1)
    {
        // TODO: notimplex
    }
    else
    {
        ret = Format_F(buffer, value, precision, negativeSign, decimalSeparator);
        if (ret > 0)
        {
            int groupSize = *((CLR_INT32 *)numberGroupSizes->GetElement(0));

            ret = InsertGroupSeparators(buffer, ret, groupSize, numberGroupSeparator);
        }
        else if (ret == 0)
        {
            buffer[ret++] = '0';
            buffer[ret] = 0;
        }
    }

    return ret;
}

int Library_corlib_native_System_Number::Format_E(char *buffer, CLR_RT_HeapBlock *value, int precision, char formatChar)
{
    int ret = -1;

    if (precision == -1)
    {
        // default to 6 digits after the decimal point
        precision = 6;
    }

    int requestedPrecision = precision;

    // force extra precision to account for rounding errors
    precision = requestedPrecision + 1;

    CLR_DataType dataType = value->DataType();

    char formatStr[FORMAT_FMTSTR_BUFFER_SIZE];
    double copyValue = 0.0;

    // need to convert to double
    switch (dataType)
    {
        case DATATYPE_I1:
            copyValue = (double)value->NumericByRef().s1;
            break;
        case DATATYPE_U1:
            copyValue = (double)value->NumericByRef().u1;
            break;
        case DATATYPE_I2:
            copyValue = (double)value->NumericByRef().s2;
            break;
        case DATATYPE_U2:
            copyValue = (double)value->NumericByRef().u2;
            break;
        case DATATYPE_I4:
            copyValue = (double)value->NumericByRef().s4;
            break;
        case DATATYPE_U4:
            copyValue = (double)value->NumericByRef().u4;
            break;
        case DATATYPE_I8:
            copyValue = (double)value->NumericByRef().s8;
            break;
        case DATATYPE_U8:
            copyValue = (double)value->NumericByRef().u8;
            break;
        case DATATYPE_R4:
            copyValue = (double)value->NumericByRef().r4;
            break;
        case DATATYPE_R8:
            copyValue = (double)value->NumericByRef().r8;
            break;
        default:
            break;
    }

    snprintf(formatStr, FORMAT_FMTSTR_BUFFER_SIZE, "%%.%d%c", precision, formatChar);

    ret = (int)snprintf(buffer, FORMAT_RESULT_BUFFER_SIZE, formatStr, copyValue);

    if (ret > 0)
    {
        int exponent = 0;

        // find the exponent character, start with lower case
        char *e = strchr(buffer, 'e');

        if (!e)
        {
            // try upper case
            e = strchr(buffer, 'E');
        }

        if (e)
        {
            // move past the exponent character
            e++;

            // convert exponent to digits
            exponent = atoi(e);
        }

        int numDigits = 0;
        int savedResultLength = ret;

        int dotIndex = GetDotIndex(buffer, ret);

        // if there is no dot, set the index to the end of the string
        if (dotIndex == -1)
        {
            dotIndex = ret;
        }

        // leave just the required amount of digits
        if (requestedPrecision <= precision)
        {
            // find the first digit after the dot
            for (int i = 0; i < ret; i++)
            {
                if (buffer[i] >= '0' && buffer[i] <= '9')
                {
                    numDigits++;

                    if (numDigits == precision)
                    {
                        ret = i + 1;
                        char first_lost_digit = buffer[ret];

                        if (first_lost_digit == '.' && (ret + 1) < savedResultLength)
                        {
                            first_lost_digit = buffer[ret + 1];
                        }

                        buffer[ret] = 0;

                        if (first_lost_digit >= '5')
                        {
                            int savedRet = ret;
                            RoundUpNumStr(buffer, &ret);

                            if (savedRet < ret)
                            {
                                dotIndex++;
                            }
                        }
                        break;
                    }
                }
            }
        }

        // now the exponent
        if (formatChar == 'e')
        {
            snprintf(&buffer[ret], FORMAT_RESULT_BUFFER_SIZE, "e%+.3d", exponent);
        }
        else
        {
            snprintf(&buffer[ret], FORMAT_RESULT_BUFFER_SIZE, "E%+.3d", exponent);
        }
    }

    return ret;
}

HRESULT Library_corlib_native_System_Number::
    FormatNative___STATIC__STRING__OBJECT__BOOLEAN__STRING__STRING__STRING__STRING__SZARRAY_I4(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *value;
    bool isInteger;
    char *format;
    char *numberDecimalSeparator;
    char *negativeSign;
    char *numberGroupSeparator;
    CLR_RT_HeapBlock_Array *numberGroupSizes;

    value = &(stack.Arg0());
    isInteger = (bool)stack.Arg1().NumericByRef().u1;
    format = (char *)stack.Arg2().RecoverString();
    numberDecimalSeparator = (char *)stack.Arg3().RecoverString();
    FAULT_ON_NULL(numberDecimalSeparator);
    negativeSign = (char *)stack.Arg4().RecoverString();
    FAULT_ON_NULL(negativeSign);
    numberGroupSeparator = (char *)stack.Arg5().RecoverString();
    FAULT_ON_NULL(numberGroupSeparator);
    numberGroupSizes = stack.Arg6().DereferenceArray();
    FAULT_ON_NULL(numberGroupSizes);

    {
        CLR_RT_TypeDescriptor desc{};
        NANOCLR_CHECK_HRESULT(desc.InitializeFromObject(*value));
        NANOCLR_CHECK_HRESULT(value->PerformUnboxing(desc.m_handlerCls));
    }

    char formatChar;
    int precision;

    if (!GetFormatSpec(format, isInteger, &formatChar, &precision))
    {
        NANOCLR_SET_AND_LEAVE(stack.SetResult_String(format));
    }
    else
    {
        char resultType[FORMAT_RESULT_BUFFER_SIZE];
        int resultLength;
        switch (formatChar)
        {
            case 'g':
            case 'G':
                resultLength = Format_G(resultType, value, formatChar, precision, negativeSign, numberDecimalSeparator);
                break;

            case 'x':
            case 'X':
                resultLength = Format_X(resultType, value, formatChar, precision);
                break;

            case 'f':
            case 'F':
                resultLength = Format_F(resultType, value, precision, negativeSign, numberDecimalSeparator);
                break;

            case 'n':
            case 'N':
                resultLength = Format_N(
                    resultType,
                    value,
                    precision,
                    negativeSign,
                    numberDecimalSeparator,
                    numberGroupSeparator,
                    numberGroupSizes);
                break;

            case 'd':
            case 'D':
                resultLength = Format_D(resultType, value, precision, negativeSign, numberDecimalSeparator);
                break;

            case 'e':
            case 'E':
                resultLength = Format_E(resultType, value, precision, formatChar);
                break;

            default:
                NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
        }

        if (resultLength > 0)
        {
            NANOCLR_SET_AND_LEAVE(stack.SetResult_String(resultType));
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
    }

    NANOCLR_NOCLEANUP();
}
