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
    bool ret = true;

    switch (*formatChar)
    {
        case 'g':
        case 'G':
        case 'n':
        case 'N':
        case 'f':
        case 'F':
            break;
        case 'x':
        case 'X':
        case 'd':
        case 'D':
            if (!isInteger)
            {
                ret = false;
            }
            break;
        default:
            ret = false;
    }

    return ret;
}

bool Library_corlib_native_System_Number::GetFormatSpec(char *format, bool isInteger, char *formatChar, int *precision)
{
    bool ret = ParseFormat(format, formatChar, precision);

    if (ret)
    {
        ret = ValidateFormatChar(formatChar, isInteger);
    }

    return ret;
}

int Library_corlib_native_System_Number::DoPrintfOnDataType(char *buffer, char *formatStr, CLR_RT_HeapBlock *value)
{
    int ret = -1;

    CLR_DataType dataType = value->DataType();

    switch (dataType)
    {
        case DATATYPE_I1:
            ret = snprintf(buffer, FORMAT_RESULT_BUFFER_SIZE, formatStr, value->NumericByRef().s1);
            break;
        case DATATYPE_U1:
            ret = snprintf(buffer, FORMAT_RESULT_BUFFER_SIZE, formatStr, value->NumericByRef().u1);
            break;
        case DATATYPE_I2:
            ret = snprintf(buffer, FORMAT_RESULT_BUFFER_SIZE, formatStr, value->NumericByRef().s2);
            break;
        case DATATYPE_U2:
            ret = snprintf(buffer, FORMAT_RESULT_BUFFER_SIZE, formatStr, value->NumericByRef().u2);
            break;
        case DATATYPE_I4:
            ret = snprintf(buffer, FORMAT_RESULT_BUFFER_SIZE, formatStr, value->NumericByRef().s4);
            break;
        case DATATYPE_U4:
            ret = snprintf(buffer, FORMAT_RESULT_BUFFER_SIZE, formatStr, value->NumericByRef().u4);
            break;
        case DATATYPE_I8:
            ret = snprintf(buffer, FORMAT_RESULT_BUFFER_SIZE, formatStr, (CLR_INT64_TEMP_CAST)value->NumericByRef().s8);
            break;
        case DATATYPE_U8:
            ret = snprintf(buffer, FORMAT_RESULT_BUFFER_SIZE, formatStr, (CLR_INT64_TEMP_CAST)value->NumericByRef().s8);
            break;
        case DATATYPE_R4:
            ret = snprintf(buffer, FORMAT_RESULT_BUFFER_SIZE, formatStr, value->NumericByRef().r4);
            break;
        case DATATYPE_R8:
            ret =
                snprintf(buffer, FORMAT_RESULT_BUFFER_SIZE, formatStr, (CLR_DOUBLE_TEMP_CAST)value->NumericByRef().r8);
            break;
        default:
            break;
    }

    // assure string valid even in cases when nothing was written
    if (ret >= 0)
        buffer[ret] = 0;

    return ret;
}

const char *Library_corlib_native_System_Number::GetPrintfLengthModifier(CLR_DataType dataType)
{
    const char *ret = (dataType == DATATYPE_I1 || dataType == DATATYPE_U1)
                          ? "hh"
                          : (dataType == DATATYPE_I2 || dataType == DATATYPE_U2)
                                ? "h"
                                : (dataType == DATATYPE_I4 || dataType == DATATYPE_U4)
                                      ? ""
                                      : (dataType == DATATYPE_I8 || dataType == DATATYPE_U8) ? "ll" : "";
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

    if (precision == -1)
    {
        switch (dataType)
        {
            case DATATYPE_I1:
            case DATATYPE_U1:
                precision = 3;
                break;
            case DATATYPE_I2:
            case DATATYPE_U2:
                precision = 5;
                break;
            case DATATYPE_I4:
            case DATATYPE_U4:
                precision = 10;
                break;
            case DATATYPE_I8:
                precision = 19;
                break;
            case DATATYPE_U8:
                precision = 20;
                break;
            case DATATYPE_R4:
                precision = 7;
                break;
            case DATATYPE_R8:
                precision = 15;
                break;
            default:
                break;
        }
    }

    if (precision > 0)
    {
        char nonIntegerPrecStr[FORMAT_FMTSTR_BUFFER_SIZE];
        if (!isIntegerDataType)
        {
            // value of incoming precision would be more than enough
            // see diff between printf and ToString precision meaning below
            snprintf(nonIntegerPrecStr, FORMAT_FMTSTR_BUFFER_SIZE, "0.%d", precision);
        }

        char formatStr[FORMAT_FMTSTR_BUFFER_SIZE];
        snprintf(
            formatStr,
            FORMAT_FMTSTR_BUFFER_SIZE,
            "%%%s%s%c",
            (isIntegerDataType) ? "" : nonIntegerPrecStr,
            (isIntegerDataType) ? GetPrintfLengthModifier(dataType) : "",
            (!isIntegerDataType) ? 'f' : (IsSignedIntegerDataType(dataType)) ? 'd' : 'u');

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

            if (ret > (precision + offsetBecauseOfNegativeSign))
            {
                int dotIndex = GetDotIndex(buffer, ret);

                int numDigits = 0;

                // leave just the required amount of digits
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
                                    dotIndex++;
                            }
                            break;
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

                if ((dotIndex == -1) || (dotIndex > (precision + offsetBecauseOfNegativeSign)))
                {
                    // insert '.'
                    memmove(
                        &buffer[2 + offsetBecauseOfNegativeSign],
                        &buffer[1 + offsetBecauseOfNegativeSign],
                        ret - 1);
                    buffer[1 + offsetBecauseOfNegativeSign] = '.';
                    ret++;

                    // append 'E+exp'
                    int exponent = (dotIndex == -1) ? savedResultLength - 1 : dotIndex - 1;
                    exponent -= offsetBecauseOfNegativeSign;
                    if (formatChar == 'g')
                    {
                        ret += snprintf(&buffer[ret], FORMAT_RESULT_BUFFER_SIZE - ret, "e+%02d", exponent);
                    }
                    else
                    {
                        ret += snprintf(&buffer[ret], FORMAT_RESULT_BUFFER_SIZE - ret, "E+%02d", exponent);
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
        GetPrintfLengthModifier(value->DataType()),
        formatChar); // x or X should return different results

    ret = DoPrintfOnDataType(buffer, formatStr, value);

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
        (!isIntegerDataType) ? 'f' : (IsSignedIntegerDataType(dataType)) ? 'd' : 'u');

    ret = DoPrintfOnDataType(buffer, formatStr, value);
    if (ret > 0)
    {
        if (isIntegerDataType && (precision > 0))
        {
            buffer[ret++] = '.';
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

HRESULT Library_corlib_native_System_Number::
    FormatNative___STATIC__STRING__OBJECT__BOOLEAN__STRING__STRING__STRING__STRING__SZARRAY_I4(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    char *ret;

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
        CLR_RT_TypeDescriptor desc;
        NANOCLR_CHECK_HRESULT(desc.InitializeFromObject(*value));
        NANOCLR_CHECK_HRESULT(value->PerformUnboxing(desc.m_handlerCls));
    }

    char formatChar;
    int precision;
    if (!GetFormatSpec(format, isInteger, &formatChar, &precision))
    {
        ret = format;
    }
    else
    {

        /*
          It looks this could be the most appropriate place where everybody looks when wants to track the current
          implementation. So I put here my toughts how this code could be modified without regression :)

          Create an nf project in VS.
          Add the following class to it:

          public static class ToStringAsserts
          {
              public static void Run()
              {
                  //AssertToConsole("(123.456).ToString(\"C\"))","",(123.456).ToString("C"));
                  //AssertToConsole("(-123.456).ToString(\"C\"))","",(-123.456).ToString("C"));
                  //AssertToConsole("(123.456).ToString(\"C3\"))","",(123.456).ToString("C3"));
                  //AssertToConsole("(-123.456).ToString(\"C3\"))","",(-123.456).ToString("C3"));
                  AssertToConsole("(1).ToString(\"Da1x\")", "Da1x", (1).ToString("Da1x"));
                  AssertToConsole("(1).ToString(\"D\")", "1", (1).ToString("D"));
                  AssertToConsole("(-1).ToString(\"D\")", "-1", (-1).ToString("D"));
                  AssertToConsole("(1234).ToString(\"D\")", "1234", (1234).ToString("D"));
                  AssertToConsole("(-1234).ToString(\"D\")", "-1234", (-1234).ToString("D"));
                  AssertToConsole("(1234).ToString(\"D6\")", "001234", (1234).ToString("D6"));
                  AssertToConsole("(-1234).ToString(\"D6\")", "-001234", (-1234).ToString("D6"));
                  AssertToConsole("(Int32.MaxValue).ToString(\"D\")", "2147483647", (Int32.MaxValue).ToString("D"));
                  AssertToConsole("(Int32.MinValue).ToString(\"D\")", "-2147483648", (Int32.MinValue).ToString("D"));
                  AssertToConsole("(Int32.MaxValue).ToString(\"D21\")", "000000000002147483647",
          (Int32.MaxValue).ToString("D21")); AssertToConsole("(Int32.MinValue).ToString(\"D21\")",
          "-000000000002147483648", (Int32.MinValue).ToString("D21"));
                  AssertToConsole("(Int64.MaxValue).ToString(\"D\")", "9223372036854775807",
          (Int64.MaxValue).ToString("D")); AssertToConsole("(Int64.MinValue).ToString(\"D\")", "-9223372036854775808",
          (Int64.MinValue).ToString("D")); AssertToConsole("(Int64.MaxValue).ToString(\"D21\")",
          "009223372036854775807", (Int64.MaxValue).ToString("D21"));
                  AssertToConsole("(Int64.MinValue).ToString(\"D21\")", "-009223372036854775808",
          (Int64.MinValue).ToString("D21")); AssertToConsole("(Int32.MaxValue+100).ToString(\"D\")", "2147483747",
          ((Int64)Int32.MaxValue + 100).ToString("D")); AssertToConsole("(Int32.MinValue-100).ToString(\"D\")",
          "-2147483748", ((Int64)Int32.MinValue - 100).ToString("D"));
                  AssertToConsole("(Int32.MaxValue+100).ToString(\"D21\")", "000000000002147483747",
          ((Int64)Int32.MaxValue + 100).ToString("D21")); AssertToConsole("(Int32.MinValue-100).ToString(\"D21\")",
          "-000000000002147483748", ((Int64)Int32.MinValue - 100).ToString("D21"));
                  //AssertToConsole("(1052.0329112756).ToString(\"E\"))","",(1052.0329112756).ToString("E"));
                  //AssertToConsole("(-1052.0329112756).ToString(\"E\"))","",(-1052.0329112756).ToString("E"));
                  //AssertToConsole("(1052.0329112756).ToString(\"E2\"))","",(1052.0329112756).ToString("E2"));
                  //AssertToConsole("(-1052.0329112756).ToString(\"E2\"))","",(-1052.0329112756).ToString("E2"));
                  //AssertToConsole("(1052.0329112756).ToString(\"e\"))","",(1052.0329112756).ToString("e"));
                  //AssertToConsole("(-1052.0329112756).ToString(\"e\"))","",(-1052.0329112756).ToString("e"));
                  //AssertToConsole("(1052.0329112756).ToString(\"e2\"))","",(1052.0329112756).ToString("e2"));
                  //AssertToConsole("(-1052.0329112756).ToString(\"e2\"))","",(-1052.0329112756).ToString("e2"));
                  AssertToConsole("(1234.567).ToString(\"F\"))", "1234.57", (1234.567).ToString("F"));
                  AssertToConsole("(-1234.567).ToString(\"F\"))", "-1234.57", (-1234.567).ToString("F"));
                  AssertToConsole("(1234.567).ToString(\"F0\"))", "1235", (1234.567).ToString("F0"));
                  AssertToConsole("(-1234.567).ToString(\"F0\"))", "-1235", (-1234.567).ToString("F0"));
                  AssertToConsole("(1234).ToString(\"F1\"))", "1234.0", (1234).ToString("F1"));
                  AssertToConsole("(-1234).ToString(\"F1\"))", "-1234.0", (-1234).ToString("F1"));
                  AssertToConsole("(1234.56).ToString(\"F4\"))", "1234.5600", (1234.56).ToString("F4"));
                  AssertToConsole("(-1234.56).ToString(\"F4\"))", "-1234.5600", (-1234.56).ToString("F4"));
                  AssertToConsole("(1234.56).ToString(\"F1\"))", "1234.6", (1234.56).ToString("F1"));
                  AssertToConsole("(-1234.56).ToString(\"F1\"))", "-1234.6", (-1234.56).ToString("F1"));
                  AssertToConsole("(1234.0056).ToString(\"F1\"))", "1234.0", (1234.0056).ToString("F1"));
                  AssertToConsole("(-1234.0056).ToString(\"F1\"))", "-1234.0", (-1234.0056).ToString("F1"));
                  AssertToConsole("(1).ToString()", "1", (1).ToString());
                  AssertToConsole("(1).ToString(null)", "1", (1).ToString((string)null));
                  AssertToConsole("(1).ToString(String.Empty)", "1", (1).ToString(String.Empty));
                  AssertToConsole("(1234.567).ToString())", "1234.567", (1234.567).ToString());
                  AssertToConsole("(123.456).ToString(\"G\"))", "123.456", (123.456).ToString("G"));
                  AssertToConsole("(-123.456).ToString(\"G\"))", "-123.456", (-123.456).ToString("G"));
                  AssertToConsole("(123.4546).ToString(\"G2\"))", "1.2E+02", (123.4546).ToString("G2"));
                  AssertToConsole("(-123.4546).ToString(\"G2\"))", "-1.2E+02", (-123.4546).ToString("G2"));
                  AssertToConsole("(123.4546).ToString(\"G4\"))", "123.5", (123.4546).ToString("G4"));
                  AssertToConsole("(-123.4546).ToString(\"G4\"))", "-123.5", (-123.4546).ToString("G4"));
                  AssertToConsole("(123.4546).ToString(\"G10\"))", "123.4546", (123.4546).ToString("G10"));
                  AssertToConsole("(-123.4546).ToString(\"G10\"))", "-123.4546", (-123.4546).ToString("G10"));
                  AssertToConsole("(99.9999).ToString(\"G4\"))", "100", (99.9999).ToString("G4"));
                  AssertToConsole("(-99.9999).ToString(\"G4\"))", "-100", (-99.9999).ToString("G4"));
                  AssertToConsole("(1234567890).ToString(\"G\"))", "1234567890", (1234567890).ToString("G"));
                  AssertToConsole("(-1234567890).ToString(\"G\"))", "-1234567890", (-1234567890).ToString("G"));
                  AssertToConsole("(1234567890).ToString(\"G4\"))", "1.235E+09", (1234567890).ToString("G4"));
                  AssertToConsole("(-1234567890).ToString(\"G4\"))", "-1.235E+09", (-1234567890).ToString("G4"));
                  AssertToConsole("(9876543210).ToString(\"G4\"))", "9.877E+09", (9876543210).ToString("G4"));
                  AssertToConsole("(-9876543210).ToString(\"G4\"))", "-9.877E+09", (-9876543210).ToString("G4"));
                  AssertToConsole("(1234567890).ToString(\"G15\"))", "1234567890", (1234567890).ToString("G15"));
                  AssertToConsole("(-1234567890).ToString(\"G15\"))", "-1234567890", (-1234567890).ToString("G15"));
                  AssertToConsole("(1234.567).ToString(\"N\"))", "1,234.57", (1234.567).ToString("N"));
                  AssertToConsole("(-1234.567).ToString(\"N\"))", "-1,234.57", (-1234.567).ToString("N"));
                  AssertToConsole("(0).ToString(\"N0\"))", "0", (0).ToString("N0"));
                  AssertToConsole("(1234).ToString(\"N0\"))", "1,234", (1234).ToString("N0"));
                  AssertToConsole("(-1234).ToString(\"N0\"))", "-1,234", (-1234).ToString("N0"));
                  AssertToConsole("(12).ToString(\"N1\"))", "12.0", (12).ToString("N1"));
                  AssertToConsole("(-12).ToString(\"N1\"))", "-12.0", (-12).ToString("N1"));
                  AssertToConsole("(1234).ToString(\"N1\"))", "1,234.0", (1234).ToString("N1"));
                  AssertToConsole("(-1234).ToString(\"N1\"))", "-1,234.0", (-1234).ToString("N1"));
                  AssertToConsole("(1234.56).ToString(\"N3\"))", "1,234.560", (1234.56).ToString("N3"));
                  AssertToConsole("(-1234.56).ToString(\"N3\"))", "-1,234.560", (-1234.56).ToString("N3"));
                  AssertToConsole("(34561234.56).ToString(\"N3\"))", "34,561,234.560", (34561234.56).ToString("N3"));
                  AssertToConsole("(-34561234.56).ToString(\"N3\"))", "-34,561,234.560", (-34561234.56).ToString("N3"));
                  //AssertToConsole("(1).ToString(\"P\"))","",(1).ToString("P"));
                  //AssertToConsole("(-0.39678).ToString(\"P\"))","",(-0.39678).ToString("P"));
                  //AssertToConsole("(123456789.12345678).ToString(\"R\"))","",(123456789.12345678).ToString("R"));
                  //AssertToConsole("(-123456789.12345678).ToString(\"R\"))","",(-123456789.12345678).ToString("R"));
                  AssertToConsole("(255).ToString(\"X\"))", "FF", (255).ToString("X"));
                  AssertToConsole("((Int32)1).ToString(\"X\"))", "1", ((Int32)1).ToString("X"));
                  AssertToConsole("((Int32)(-1)).ToString(\"x\"))", "ffffffff", ((Int32)(-1)).ToString("x"));
                  AssertToConsole("((byte)1).ToString(\"x\"))", "1", (unchecked((byte)1)).ToString("x"));
                  AssertToConsole("((byte)(-1)).ToString(\"x\"))", "ff", (unchecked((byte)(-1))).ToString("x"));
                  AssertToConsole("((short)1).ToString(\"x\"))", "1", ((short)1).ToString("x"));
                  AssertToConsole("((short)1).ToString(\"x2\"))", "01", ((short)1).ToString("x2"));
                  AssertToConsole("((short)(-1)).ToString(\"x\"))", "ffff", ((short)(-1)).ToString("x"));
                  AssertToConsole("(255).ToString(\"x4\"))", "00ff", (255).ToString("x4"));
                  AssertToConsole("((Int32)(-1)).ToString(\"X4\"))", "FFFFFFFF", ((Int32)(-1)).ToString("X4"));
                  AssertToConsole("(Int64.MaxValue-10).ToString(\"x\"))", "7ffffffffffffff5", (Int64.MaxValue -
          10).ToString("x")); AssertToConsole("((Int64)10).ToString(\"x\"))", "a", ((Int64)10).ToString("x"));
                  AssertToConsole("((Int64)10).ToString(\"x5\"))", "0000a", ((Int64)10).ToString("x5"));
              }

              private static void AssertToConsole(string lbl, string expected, string str)
              {
                  if (str != expected)
                  {
                      Debug.WriteLine($"BAD: {lbl} expected: {expected} found: {str}");
                  }
                  else
                  {
                      Debug.WriteLine($" OK: {lbl} expected: {expected}");
                  }
              }
          }

          As you can see all the implemented formatters and some of their interesting deviances have a test case there.
          Feel free to add others which contain a bug case, etc.
          DONT FORGET TO MODIFY THIS COMMENT WITH UP-TO-DATE VERSION!

          From you Main() call this class:
            ToStringAsserts.Run();

          Add a second old-school (running on "official" .net environment, like a Windows Console app) project to your
          solution. Call from it's main() the test class as above. Execute this second project. You will see how the
          official implementation formats the various things. Adjust the "expected" strings to reflect the received
          output. E.g. if you want to check how we format (255).ToString("X") then add the following line:

            AssertToConsole("(255).ToString(\"X\"))", "dunnoyet", (255).ToString("X"));

          Execute the old-school project and you will see this line:
            BAD: (255).ToString("X")) expected: dunnoyet found: FF

          Now replace the "dunnoyet" with "FF" and the test case ready.
          AGAIN: DONT FORGET TO MODIFY THIS COMMENT WITH UP-TO-DATE VERSION!

          Now execute the nf project and check what nanoCLR formats for you.
          Implement, fix, etc.
          But always check your test project results in OK on all lines!

          Thats all folks :)
        */

        char result[FORMAT_RESULT_BUFFER_SIZE];

        int resultLength;
        switch (formatChar)
        {
            case 'g':
            case 'G':
                resultLength = Format_G(result, value, formatChar, precision, negativeSign, numberDecimalSeparator);
                break;
            case 'x':
            case 'X':
                resultLength = Format_X(result, value, formatChar, precision);
                break;
            case 'f':
            case 'F':
                resultLength = Format_F(result, value, precision, negativeSign, numberDecimalSeparator);
                break;
            case 'n':
            case 'N':
                resultLength = Format_N(
                    result,
                    value,
                    precision,
                    negativeSign,
                    numberDecimalSeparator,
                    numberGroupSeparator,
                    numberGroupSizes);
                break;
            case 'd':
            case 'D':
                resultLength = Format_D(result, value, precision, negativeSign, numberDecimalSeparator);
                break;
            default:
                NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
        }

        if (resultLength > 0)
        {
            ret = result;
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
    }

    NANOCLR_SET_AND_LEAVE(stack.SetResult_String(ret));
    NANOCLR_NOCLEANUP();
}
