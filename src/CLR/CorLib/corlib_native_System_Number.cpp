//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"

// must be big enough to fit the biggest number
// decorated with negative signs, group separators, etc.
#define FORMAT_RESULT_BUFFER_SIZE 128
#define FORMAT_FMTSTR_BUFFER_SIZE 10

bool nf_ParseFormat(char *format, char *formatChar, int *precision)
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

bool nf_ValidateFormatChar(char *formatChar, bool isInteger)
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

bool nf_GetFormatSpec(char *format, bool isInteger, char *formatChar, int *precision)
{
    bool ret = nf_ParseFormat(format, formatChar, precision);

    if (ret)
    {
        ret = nf_ValidateFormatChar(formatChar, isInteger);
    }

    return ret;
}

int nf_DoPrintfOnDataType(char *buffer, char *formatStr, CLR_RT_HeapBlock *value)
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

    return ret;
}

const char *nf_GetPrintfLengthModifier(CLR_DataType dataType)
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

bool nf_IsSignedIntegerDataType(CLR_DataType dataType)
{
    bool ret =
        (dataType == DATATYPE_I1 || dataType == DATATYPE_I2 || dataType == DATATYPE_I4 || dataType == DATATYPE_I8);
    return ret;
}

bool nf_IsUnsignedIntegerDataType(CLR_DataType dataType)
{
    bool ret =
        (dataType == DATATYPE_U1 || dataType == DATATYPE_U2 || dataType == DATATYPE_U4 || dataType == DATATYPE_U8);
    return ret;
}

bool nf_IsIntegerDataType(CLR_DataType dataType)
{
    bool ret = nf_IsSignedIntegerDataType(dataType) || nf_IsUnsignedIntegerDataType(dataType);
    return ret;
}

int nf_GetStrLen(char *buffer)
{
    int ret = 0;
    for (; buffer[ret++] != 0;)
        ;
    return ret;
}

int nf_GetDotIndex(char *buffer, int bufferContentLength)
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

void nf_roundUpNumStr(char *buffer, int *bufferContentLength)
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

int nf_ReplaceNegativeSign(char *buffer, int bufferContentLength, char *negativeSign)
{
    int ret = bufferContentLength;

    if (buffer[0] == '-')
    {
        int negativeSignLength = nf_GetStrLen(negativeSign);

        // we need it without trailing 0
        negativeSignLength--;

        memmove(&buffer[negativeSignLength], &buffer[1], bufferContentLength);
        memcpy(buffer, negativeSign, negativeSignLength);
        ret += negativeSignLength - 1;
    }

    return ret;
}

int nf_ReplaceDecimalSeparator(char *buffer, int bufferContentLength, char *decimalSeparator)
{
    int ret = bufferContentLength;

    int dotIndex = nf_GetDotIndex(buffer, bufferContentLength);
    if (dotIndex != -1)
    {
        int decimalSeparatorLength = nf_GetStrLen(decimalSeparator);

        // we need it without trailing 0
        decimalSeparatorLength--;

        memmove(&buffer[dotIndex + decimalSeparatorLength], &buffer[dotIndex + 1], bufferContentLength);
        memcpy(&buffer[dotIndex], decimalSeparator, decimalSeparatorLength);
        ret += decimalSeparatorLength - 1;
    }

    return ret;
}

int nf_Format_G(char *buffer, CLR_RT_HeapBlock *value, int precision, char *negativeSign, char *decimalSeparator)
{
    int ret = -1;

    CLR_DataType dataType = value->DataType();

    bool isIntegerDataType = nf_IsIntegerDataType(dataType);

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
            (isIntegerDataType) ? nf_GetPrintfLengthModifier(dataType) : "",
            (!isIntegerDataType) ? 'f' : (nf_IsSignedIntegerDataType(dataType)) ? 'd' : 'u');

        // {
        //     char temporaryStringBuffer[640];
        //     int realStringSize =
        //         snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "*** %d %s\r\n", dataType, formatStr);
        //     CLR_EE_DBG_EVENT_BROADCAST(
        //         CLR_DBG_Commands_c_Monitor_Message,
        //         realStringSize,
        //         temporaryStringBuffer,
        //         WP_Flags_c_NonCritical | WP_Flags_c_NoCaching);
        // }

        ret = nf_DoPrintfOnDataType(buffer, formatStr, value);
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
                int dotIndex = nf_GetDotIndex(buffer, ret);

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
                            buffer[ret] = 0;
                            if (first_lost_digit >= '5')
                            {
                                int savedRet = ret;
                                nf_roundUpNumStr(buffer, &ret);
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
                    ret += snprintf(&buffer[ret], FORMAT_RESULT_BUFFER_SIZE - ret, "E+%02d", exponent);
                }
            }

            ret = nf_ReplaceNegativeSign(buffer, ret, negativeSign);
            ret = nf_ReplaceDecimalSeparator(buffer, ret, decimalSeparator);
        }
    }

    return ret;
}

int nf_Format_D(char *buffer, CLR_RT_HeapBlock *value, int precision, char *negativeSign, char *decimalSeparator)
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
        nf_GetPrintfLengthModifier(dataType),
        (nf_IsSignedIntegerDataType(dataType)) ? 'd' : 'u');

    ret = nf_DoPrintfOnDataType(buffer, formatStr, value);
    if (ret > 0)
    {
        // printf and ToString differs on negative numbers:
        // printf("%05d", -123) returns "-0123"
        // -123.ToString("D5") returns "-00123"
        if (precision != 0 && ret > 1 && buffer[0] == '-')
        {
            // our buffer defined bigger than the max number string, no worries
            memmove(&buffer[2], &buffer[1], ret);
            buffer[1] = '0';
            ret++;
        }

        ret = nf_ReplaceNegativeSign(buffer, ret, negativeSign);
        ret = nf_ReplaceDecimalSeparator(buffer, ret, decimalSeparator);
    }

    return ret;
}

int nf_Format_X(char *buffer, CLR_RT_HeapBlock *value, char formatChar, int precision)
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
        nf_GetPrintfLengthModifier(value->DataType()),
        formatChar); // x or X should return different results

    ret = nf_DoPrintfOnDataType(buffer, formatStr, value);

    return ret;
}

int nf_Format_F(char *buffer, CLR_RT_HeapBlock *value, int precision, char *negativeSign, char *decimalSeparator)
{
    int ret = -1;

    if (precision == -1)
    {
        precision = 2; // should be equal to NumberFormatInfo.NumberDecimalDigits which isn't implemented in
                       // NF at the moment
    }

    CLR_DataType dataType = value->DataType();

    bool isIntegerDataType = nf_IsIntegerDataType(dataType);

    char formatStr[FORMAT_FMTSTR_BUFFER_SIZE];
    snprintf(
        formatStr,
        FORMAT_FMTSTR_BUFFER_SIZE,
        "%%0.%d%s%c",
        precision,
        (isIntegerDataType) ? nf_GetPrintfLengthModifier(dataType) : "",
        (!isIntegerDataType) ? 'f' : (nf_IsSignedIntegerDataType(dataType)) ? 'd' : 'u');

    ret = nf_DoPrintfOnDataType(buffer, formatStr, value);
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
        ret = nf_ReplaceNegativeSign(buffer, ret, negativeSign);
        ret = nf_ReplaceDecimalSeparator(buffer, ret, decimalSeparator);
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

    // {
    //     char temporaryStringBuffer[640];
    //     int realStringSize = snprintf(
    //         temporaryStringBuffer,
    //         sizeof(temporaryStringBuffer),
    //         "params:\r\n\tdt: %d\r\n\ti: %d\r\n\tf: [%s]\r\n\tnds: [%s]\r\n\tns: [%s]\r\n\tngs: [%s]\r\n\tdn: "
    //         "[%d]\r\n\tdn0: [%d]\r\n",
    //         value->DataType(),
    //         isInteger,
    //         (format) ? format : "NULL",
    //         numberDecimalSeparator,
    //         negativeSign,
    //         numberGroupSeparator,
    //         numberGroupSizes->m_numOfElements,
    //         *((CLR_INT32 *)numberGroupSizes->GetElement(0)));
    //     CLR_EE_DBG_EVENT_BROADCAST(
    //         CLR_DBG_Commands_c_Monitor_Message,
    //         realStringSize,
    //         temporaryStringBuffer,
    //         WP_Flags_c_NonCritical | WP_Flags_c_NoCaching);
    // }

    char formatChar;
    int precision;
    if (!nf_GetFormatSpec(format, isInteger, &formatChar, &precision))
    {
        ret = format;
    }
    else
    {

        // {
        //     char temporaryStringBuffer[640];
        //     int realStringSize = snprintf(
        //         temporaryStringBuffer,
        //         sizeof(temporaryStringBuffer),
        //         "formatChar: %c, precision: %d\r\n",
        //         formatChar,
        //         precision);
        //     CLR_EE_DBG_EVENT_BROADCAST(
        //         CLR_DBG_Commands_c_Monitor_Message,
        //         realStringSize,
        //         temporaryStringBuffer,
        //         WP_Flags_c_NonCritical | WP_Flags_c_NoCaching);
        // }

        char result[FORMAT_RESULT_BUFFER_SIZE];

        int resultLength;
        switch (formatChar)
        {
            case 'g':
            case 'G':
                resultLength = nf_Format_G(result, value, precision, negativeSign, numberDecimalSeparator);
                break;
            case 'x':
            case 'X':
                resultLength = nf_Format_X(result, value, formatChar, precision);
                break;
            case 'f':
            case 'F':
                resultLength = nf_Format_F(result, value, precision, negativeSign, numberDecimalSeparator);
                break;
            case 'n':
            case 'N':
            {
                if (precision < 0)
                    precision = 6; // should be equal to NumberFormatInfo.NumberDecimalDigits which isn't implemented in
                                   // NF at the moment
                snprintf(result, FORMAT_RESULT_BUFFER_SIZE, "NNN");
                resultLength = true;
            }
            break;
            case 'd':
            case 'D':
                resultLength = nf_Format_D(result, value, precision, negativeSign, numberDecimalSeparator);
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

// compose sprintf format string according to requested parameters
void nf_GetFormatString(
    char *formatStr,
    size_t formatStrLen,
    char formatCh,
    int precision,
    bool isLong,
    bool isFloat,
    bool isSigned)
{

    if (formatCh == 'X' || formatCh == 'x')
        snprintf(formatStr, formatStrLen, "%%0%d%sX", precision, isLong ? "ll" : "");
    else
        snprintf(
            formatStr,
            formatStrLen,
            "%%%s%d%s%s",
            isFloat ? "." : isSigned ? "-0" : "0",
            precision,
            isLong ? "ll" : isFloat ? "f" : "",
            isFloat ? "" : isSigned ? "d" : "u");
}

// remove the prepended zeros and (if possible) the decimal point in a float that's formated as string. e.g.
// "47.1100815000000" => "47.1100815" or "8.0000E-12" => "8E-12"
void nf_RemovePrependedZeros(char *floatStr)
{
    int length = hal_strlen_s(floatStr);
    // flag for finding the decimal point
    bool pointFound = false;
    // if not -1 we found the first zero after the decimal point
    int firstZero = -1;
    // if not -1 we found an "e" or "E" after the last zero of this is the string length
    int nextNonZero = length;

    // iterate thru all chars
    for (int i = 0; i < length; i++)
    {
        // no decimal point found until now?
        if (!pointFound)
        {
            // is it the decimal point?
            if (floatStr[i] == '.')
            {
                pointFound = true;
            }

            // next char
            continue;
        }

        // at this point we found the decimal point
        // no zero found until now?
        if (firstZero == -1)
        {
            // is it a zero?
            if (floatStr[i] == '0')
            {
                // store the position of the first zero after the decimal point
                firstZero = i;
            }

            // next char
            continue;
        }

        // at this point we found the decimal point and the first zero
        // an "e" or "E" char stops the sequence of zeros
        if (floatStr[i] == 'e' || floatStr[i] == 'E')
        {
            // store the position of the e/E char
            nextNonZero = i;
            // done! we found the positions for the prepended zeros
            break;
        }

        // at this point we found the decimal point and the first zero and the current char is not the e/E char
        // is this not a zero?
        if (floatStr[i] != '0')
        {
            // reset! we need to find another zero
            firstZero = -1;
        }
    }

    // something to remove?
    if (pointFound && firstZero != -1 && nextNonZero != -1)
    {
        // is the char before the first zero the decimal point? => Remove the decimal point
        int startIndex = floatStr[firstZero - 1] == '.' ? firstZero - 1 : firstZero;
        // no e/E char after the last trailing zero?
        if (nextNonZero == length)
        {
            // we can cut away the prepended zeros by terminating the string at first zero
            floatStr[startIndex] = 0;
        }
        else
        {
            // otherwise we copy the last part over the prepended zeros and terminate the string
            int lengthToCopy = length - nextNonZero;
            memcpy(&floatStr[startIndex], &floatStr[nextNonZero], lengthToCopy);
            floatStr[startIndex + lengthToCopy] = 0;
        }
    }
}

HRESULT Library_corlib_native_System_Number::FormatNative___STATIC__STRING__OBJECT__CHAR__I4(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pArgs = &(stack.Arg0());
    CLR_RT_HeapBlock *value = pArgs;
    CLR_DataType dt;
    CLR_RT_TypeDescriptor desc;
    char result[24];
    // This is temporary buffer to create proper format string.
    char formatStr[8];
    char formatCh = (char)pArgs[1].NumericByRef().u1;
    int precision = pArgs[2].NumericByRef().s4;
    bool shouldRemovePrependedZeros = false;

    NANOCLR_CHECK_HRESULT(desc.InitializeFromObject(*value));
    NANOCLR_CHECK_HRESULT(value->PerformUnboxing(desc.m_handlerCls));

    dt = value->DataType();

    if (formatCh == 'G' && precision == 0)
    {
        precision = 1;
        shouldRemovePrependedZeros = true;
    }

    switch (dt)
    {
        case DATATYPE_I1:
            // get format string
            nf_GetFormatString(formatStr, ARRAYSIZE(formatStr), formatCh, precision, false, false, true);
            snprintf(result, ARRAYSIZE(result), formatStr, value->NumericByRef().s1);
            break;

        case DATATYPE_U1:
            // get format string
            nf_GetFormatString(formatStr, ARRAYSIZE(formatStr), formatCh, precision, false, false, false);
            snprintf(result, ARRAYSIZE(result), formatStr, value->NumericByRef().u1);
            break;

        case DATATYPE_I2:
            // get format string
            nf_GetFormatString(formatStr, ARRAYSIZE(formatStr), formatCh, precision, false, false, true);
            snprintf(result, ARRAYSIZE(result), formatStr, value->NumericByRef().s2);
            break;

        case DATATYPE_U2:
            // get format string
            nf_GetFormatString(formatStr, ARRAYSIZE(formatStr), formatCh, precision, false, false, false);
            snprintf(result, ARRAYSIZE(result), formatStr, value->NumericByRef().u2);
            break;

        case DATATYPE_I4:
            // get format string
            nf_GetFormatString(formatStr, ARRAYSIZE(formatStr), formatCh, precision, false, false, true);
            snprintf(result, ARRAYSIZE(result), formatStr, value->NumericByRef().s4);
            break;

        case DATATYPE_U4:
            // get format string
            nf_GetFormatString(formatStr, ARRAYSIZE(formatStr), formatCh, precision, false, false, false);
            snprintf(result, ARRAYSIZE(result), formatStr, value->NumericByRef().u4);
            break;

        case DATATYPE_I8:
        {
#if defined(_WIN32)
            hal_snprintf(result, ARRAYSIZE(result), "%I64d", (CLR_INT64_TEMP_CAST)value->NumericByRef().s8);
#else
            // get format string
            nf_GetFormatString(formatStr, ARRAYSIZE(formatStr), formatCh, precision, true, false, true);
            snprintf(result, ARRAYSIZE(result), formatStr, (CLR_INT64_TEMP_CAST)value->NumericByRef().s8);
#endif // defined(_WIN32)
        }
        break;

        case DATATYPE_U8:
        {
#if defined(_WIN32)
            hal_snprintf(result, ARRAYSIZE(result), "%I64u", (CLR_UINT64_TEMP_CAST)value->NumericByRef().u8);
#else
            // get format string
            nf_GetFormatString(formatStr, ARRAYSIZE(formatStr), formatCh, precision, true, false, false);
            snprintf(result, ARRAYSIZE(result), formatStr, (CLR_UINT64_TEMP_CAST)value->NumericByRef().u8);
#endif // defined(_WIN32)
        }
        break;

        case DATATYPE_R4:
        {
            // All the formatCh have been converted to upper case in the managed layer
            _ASSERTE(formatCh == 'G' || formatCh == 'N' || formatCh == 'F' || formatCh == 'D');

            if (precision < 0 || precision > 99)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            }

            // DATATYPE_R4 is float and uses a precision of 9, standard sprintf format is "%.9g"

#if defined(_WIN32) || defined(WIN32) || defined(_WIN32_WCE)
            hal_snprintf(result, ARRAYSIZE(result), "%.9g", value->NumericByRef().r4);
#else

#if !defined(NANOCLR_EMULATED_FLOATINGPOINT)
            // use default precision is none is specfied AND if format in not generic
            precision = (formatCh == 'G' && precision == 1) ? 9 : precision;

            // get format string
            nf_GetFormatString(formatStr, ARRAYSIZE(formatStr), formatCh, precision, false, true, false);

            snprintf(result, ARRAYSIZE(result), formatStr, value->NumericByRef().r4);
            if (shouldRemovePrependedZeros)
            {
                nf_RemovePrependedZeros(result);
            }
#else
            CLR_INT32 f = value->NumericByRef().r4;
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
#endif // !defined(NANOCLR_EMULATED_FLOATINGPOINT)
#endif // defined(_WIN32)
        }
        break;

        case DATATYPE_R8:
        {
            // All the formatCh have been converted to upper case in the managed layer
            _ASSERTE(formatCh == 'G' || formatCh == 'N' || formatCh == 'F' || formatCh == 'D');

            if (precision < 0 || precision > 99)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            }

            // DATATYPE_R8 is double and uses a precision of 15, standard sprintf format is "%.15g"

#if defined(_WIN32) || defined(WIN32) || defined(_WIN32_WCE)
            hal_snprintf(result, ARRAYSIZE(result), "%.15g", (CLR_DOUBLE_TEMP_CAST)value->NumericByRef().r8);
#else
#if !defined(NANOCLR_EMULATED_FLOATINGPOINT)
            // use default precision is none is specfied AND format is not generic
            precision = (formatCh == 'G' && precision == 1) ? 15 : precision;

            // get format string
            nf_GetFormatString(formatStr, ARRAYSIZE(formatStr), formatCh, precision, false, true, false);

            snprintf(result, ARRAYSIZE(result), formatStr, (CLR_DOUBLE_TEMP_CAST)value->NumericByRef().r8);
            if (shouldRemovePrependedZeros)
            {
                nf_RemovePrependedZeros(result);
            }
#else
            CLR_INT64 d = (CLR_DOUBLE_TEMP_CAST)value->NumericByRef().r8;
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
#endif // !defined(NANOCLR_EMULATED_FLOATINGPOINT)
#endif // defined(_WIN32)
        }
        break;

        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    NANOCLR_SET_AND_LEAVE(stack.SetResult_String(result));

    NANOCLR_NOCLEANUP();
}
