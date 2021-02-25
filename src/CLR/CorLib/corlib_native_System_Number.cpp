//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"

bool nf_ParseFormat(char *format, char *formatChar, int *precision)
{
    bool ret = true;

    // parse received format
    if (format != NULL)
    {
        *formatChar = format[0];
        *precision = 0;

        char *cur = format;
        while (*++cur != 0)
        {
            if ((*cur) < '0' || (*cur) > '9')
            {
                ret = false;
                break;
            }
            *precision = (*precision) * 10 + ((*cur) - '0');
        }
    }
    else
    {
        // defaults
        *formatChar = 'G';
        *precision = 0;
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

bool nf_AdjustPrecision(CLR_DataType dataType, char *formatChar, int *precision)
{
    bool ret = true;

    if (*precision == 0)
    {
        switch (*formatChar)
        {
            case 'g':
            case 'G':
                switch (dataType)
                {
                    case DATATYPE_I1:
                    case DATATYPE_U1:
                        *precision = 3;
                        break;
                    case DATATYPE_I2:
                    case DATATYPE_U2:
                        *precision = 5;
                        break;
                    case DATATYPE_I4:
                    case DATATYPE_U4:
                        *precision = 10;
                        break;
                    case DATATYPE_I8:
                        *precision = 19;
                        break;
                    case DATATYPE_U8:
                        *precision = 20;
                        break;
                    case DATATYPE_R4:
                        *precision = 7;
                        break;
                    case DATATYPE_R8:
                        *precision = 15;
                        break;
                    default:
                        ret = false;
                        break;
                }
                break;
            case 'f':
            case 'F':
            case 'n':
            case 'N':
                *precision = 6; // should be equal to NumberFormatInfo.NumberDecimalDigits which isn't implemented in NF
                                // at the moment
                break;
            case 'x':
            case 'X':
            case 'd':
            case 'D':
                *precision = -1;
                break;
            default:
                ret = false;
        }
    }

    return ret;
}

bool nf_GetFormatSpec(char *format, bool isInteger, CLR_DataType dataType, char *formatChar, int *precision)
{
    bool ret = nf_ParseFormat(format, formatChar, precision);

    if (ret)
    {
        ret = nf_ValidateFormatChar(formatChar, isInteger);
        if (ret)
        {
            ret = nf_AdjustPrecision(dataType, formatChar, precision);
        }
    }

    return ret;
}

int nf_PrintfOnDataType(char *buffer, size_t bufferSize, char *formatStr, CLR_RT_HeapBlock *value)
{
    int ret = -1;

    CLR_DataType dataType = value->DataType();

    switch (dataType)
    {
        case DATATYPE_I1:
            ret = snprintf(buffer, bufferSize, formatStr, value->NumericByRef().s1);
            break;
        case DATATYPE_U1:
            ret = snprintf(buffer, bufferSize, formatStr, value->NumericByRef().u1);
            break;
        case DATATYPE_I2:
            ret = snprintf(buffer, bufferSize, formatStr, value->NumericByRef().s2);
            break;
        case DATATYPE_U2:
            ret = snprintf(buffer, bufferSize, formatStr, value->NumericByRef().u2);
            break;
        case DATATYPE_I4:
            ret = snprintf(buffer, bufferSize, formatStr, value->NumericByRef().s4);
            break;
        case DATATYPE_U4:
            ret = snprintf(buffer, bufferSize, formatStr, value->NumericByRef().u4);
            break;
        case DATATYPE_I8:
            ret = snprintf(buffer, bufferSize, formatStr, (CLR_INT64_TEMP_CAST)value->NumericByRef().s8);
            break;
        case DATATYPE_U8:
            ret = snprintf(buffer, bufferSize, formatStr, (CLR_INT64_TEMP_CAST)value->NumericByRef().s8);
            break;
        default:
            break;
    }

    return ret;
}

bool nf_Format_G(char *buffer, size_t bufferSize, CLR_RT_HeapBlock *value, char formatChar, int precision)
{
    bool ret = true;

    snprintf(buffer, bufferSize, "GGG");

    return ret;
}

bool nf_Format_D(char *buffer, size_t bufferSize, CLR_RT_HeapBlock *value, char formatChar, int precision)
{
    bool ret = true;

    if (precision == -1)
    {
        precision = 0;
    }

    CLR_DataType dataType = value->DataType();

    char formatStr[10];
    snprintf(
        formatStr,
        ARRAYSIZE(formatStr),
        "%%0%d%s%c",
        precision,
        (dataType == DATATYPE_I1 || dataType == DATATYPE_U1)
            ? "hh"
            : (dataType == DATATYPE_I2 || dataType == DATATYPE_U2)
                  ? "h"
                  : (dataType == DATATYPE_I4 || dataType == DATATYPE_U4)
                        ? ""
                        : (dataType == DATATYPE_I8 || dataType == DATATYPE_U8) ? "ll" : "???",
        (dataType == DATATYPE_I1 || dataType == DATATYPE_I2 || dataType == DATATYPE_I4 || dataType == DATATYPE_I8)
            ? 'd'
            : 'u');

    int resultLength = nf_PrintfOnDataType(buffer, bufferSize, formatStr, value);
    if (resultLength < 0)
    {
        ret = false;
    }
    else
    {
        // printf and ToString differs on negative numbers:
        // printf("%05d", -123) returns "-0123"
        // -123.ToString("D5") returns "-00123"
        if (precision != 0 && resultLength > 1 && buffer[0] == '-')
        {
            // our buffer defined bigger than the max number string
            memmove(&buffer[2], &buffer[1], resultLength);
            buffer[1] = '0';
        }
    }

    return ret;
}

bool nf_Format_X(char *buffer, size_t bufferSize, CLR_RT_HeapBlock *value, char formatChar, int precision)
{
    bool ret = true;

    if (precision == -1)
    {
        precision = 0;
    }

    CLR_DataType dataType = value->DataType();

    char formatStr[10];
    snprintf(
        formatStr,
        ARRAYSIZE(formatStr),
        "%%0%d%s%c",
        precision,
        (dataType == DATATYPE_I1 || dataType == DATATYPE_U1)
            ? "hh"
            : (dataType == DATATYPE_I2 || dataType == DATATYPE_U2)
                  ? "h"
                  : (dataType == DATATYPE_I4 || dataType == DATATYPE_U4)
                        ? ""
                        : (dataType == DATATYPE_I8 || dataType == DATATYPE_U8) ? "ll" : "???",
        formatChar); // x or X should return different results

    if (nf_PrintfOnDataType(buffer, bufferSize, formatStr, value) < 0)
    {
        ret = false;
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
    if (!nf_GetFormatSpec(format, isInteger, value->DataType(), &formatChar, &precision))
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

        char result[128];

        bool successfullyFormatted;
        switch (formatChar)
        {
            case 'g':
            case 'G':
                successfullyFormatted = nf_Format_G(result, ARRAYSIZE(result), value, formatChar, precision);
                break;
            case 'x':
            case 'X':
                successfullyFormatted = nf_Format_X(result, ARRAYSIZE(result), value, formatChar, precision);
                break;
            case 'n':
            case 'N':
            case 'f':
            case 'F':
            {
                snprintf(result, ARRAYSIZE(result), "XXX");
                successfullyFormatted = true;
            }
            break;
            case 'd':
            case 'D':
                successfullyFormatted = nf_Format_D(result, ARRAYSIZE(result), value, formatChar, precision);
                break;
            default:
                NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
        }

        if (successfullyFormatted)
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
