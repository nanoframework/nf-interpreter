//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation. All rights reserved.
// Portions Copyright (C) 2002-2019 Free Software Foundation, Inc. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "corlib_native.h"
#include <ctype.h>
#include <base64.h>

// when running with lwip the use of errno is affected by _REENT_ONLY - see below.  LWIP is included via corlib_native.h
// and lower (HAL).  Win32 does not use it
#include <cerrno>

// remap this namespace to have better code readability
typedef Library_corlib_native_System_DateTime DateTime;

HRESULT Library_corlib_native_System_Convert::
    NativeToInt64___STATIC__I8__STRING__BOOLEAN__I8__I8__I4__BOOLEAN__BYREF_BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    char *endptr;
    int64_t result = 0;

#if (SUPPORT_ANY_BASE_CONVERSION == TRUE)
    // convert via strtoll / strtoull
    int error_code;
#else
    uint64_t intPart = 0;
    uint64_t lastValue = 0;
#endif

    char *str = (char *)stack.Arg0().RecoverString();
    signed int radix = stack.Arg4().NumericByRef().s4;
    bool isUInt64 = false;

    bool isSigned = (bool)stack.Arg1().NumericByRef().u1;
    bool negReturnExpected = false;

    long long minValue = stack.Arg2().NumericByRef().s8;
    long long maxValue = stack.Arg3().NumericByRef().s8;
    if (minValue == 0 && maxValue == 0)
    {
        isUInt64 = true;
        isSigned = false;
    }

    // grab parameter with flag to throw on failure
    bool throwOnFailure = (bool)stack.Arg5().NumericByRefConst().u1;

    // check string parameter for null
    FAULT_ON_NULL_ARG(str);

    // allow spaces before digits
    while (*str == ' ')
    {
        str++;
    }
    endptr = NULL;

#if (SUPPORT_ANY_BASE_CONVERSION == TRUE)
    // support for conversion from any base

    if (*str == '-')
    {
        negReturnExpected = true;
    }

    // Have to use reentrant version of strtoll because lwip sets _REENT_ONLY to require all stdlib calls
    // to be reentrant
#ifdef _REENT_ONLY

    _reent reent_data;
    reent_data._errno = 0;
    result = isSigned ? _strtoll_r(&reent_data, str, &endptr, radix)
                      : (long long)_strtoull_r(&reent_data, str, &endptr, radix);
    error_code = (int)reent_data._errno;

#else

    errno = 0;
    result = isSigned ? strtoll(str, &endptr, radix) : (long long)strtoull(str, &endptr, radix);
    error_code = errno;

#endif //_REENT_ONLY

    // catch the case of exceeding signed/unsigned int64. Catch formatting errors in the next statement
    if (error_code == ERANGE)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    // if no valid conversion endptr is equal str
    if (str == endptr)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FORMAT_EXCEPTION);
    }

    // allow spaces after digits
    while (*endptr == ' ')
    {
        endptr++;
    }

    // should reach end of string no aditional chars
    if (*endptr != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FORMAT_EXCEPTION);
    }

    // the signed values for SByte, Int16 and Int32 are always positive for base 2, 8 or 16 conversions
    // because the 64-bit function strtoll is used; need the post process the value
    // if the result is greater max and smaller (max + 1) * 2 this value should be subtracted
    if (radix == 2 || radix == 8 || radix == 16)
    {
        if (isSigned && result > maxValue && result < (maxValue + 1) * 2)
        {
            result -= (maxValue + 1) * 2;
        }
    }

    if (negReturnExpected && isSigned == false && result != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    // Check min and max values for the smaller integers - the stroll and stroull will catch int64 excesses
    if (!isUInt64 && !isSigned && (uint64_t)result > (uint64_t)maxValue)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }
    else if (!isUInt64 && (result > maxValue || result < minValue))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }
    else
    {
        stack.SetResult_I8(result);
    }

#else

    // support for conversion from base 10 and 16 (partial)
    if (radix == 10)
    {
        // conversion from base 10

        // check for minus sign
        if (*str == '-')
        {
            negReturnExpected = true;
            str++;
        }
        else if (*str == '+')
        {
            str++;
        }

        intPart = 0;
        lastValue = 0;

        // guess at no more than 99 characters
        for (int i = 0; i < 99; i++)
        {
            if (*str < '0' || *str > '9')
            {
                endptr = str;

                // allow spaces after digits
                while (*endptr == ' ')
                {
                    endptr++;
                }

                // should reach end of string no aditional chars
                if (*endptr == 0)
                {
                    break;
                }

                // non-numeric (and not trailing space)
                NANOCLR_SET_AND_LEAVE(CLR_E_FORMAT_EXCEPTION);
            }

            // advance the digits and add the current number
            intPart = (intPart * 10) + (*str - '0');

            if (intPart < lastValue)
            {
                // the above operation overflowed the value
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
            }

            lastValue = intPart;

            str++;
            if (*str == '\0')
            {
                break;
            }
        }

        // intPart now holds a positive number from the string.
        if (negReturnExpected)
        {
            // it's ok to use -0 even for unsigned types, but otherwise - NO.
            if (isSigned == false && intPart > 0)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
            }

            // too big to make a negative value?
            if (intPart > (uint64_t)(minValue * -1))
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
            }

            result = intPart * -1;
        }
        else
        {
            // result will be negative for large uints, and we checked for overflow above
            if (isUInt64 == false && intPart > (uint64_t)maxValue)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
            }

            // this MAY have made the result negative by overflowing the buffer - which we do
            // for uint64 logic.  The c# code will cast the int64 to uint64 removing the sign
            result = (int64_t)intPart;
        }
    }
    else if (radix == 16)
    {
        // conversion from base 16
        result = GetIntegerFromHexString(str);
        //??? check against min/max?  Signed possible?
    }
    else
    {
        // all other bases are not supported
        return stack.NotImplementedStub();
    }

    stack.SetResult_I8(result);

#endif // defined(SUPPORT_ANY_BASE_CONVERSION)

    NANOCLR_CLEANUP();

    // set parameter reporting conversion success/failure
    stack.Arg6().Dereference()->NumericByRef().u1 = (hr == S_OK);

    // should we throw an exception?
    if (hr != S_OK && !throwOnFailure)
    {
        // nope! so clear the exception
        hr = S_OK;

        // set return value with minimum value
        stack.SetResult_I8(minValue);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_corlib_native_System_Convert::NativeToDouble___STATIC__R8__STRING__BOOLEAN__BYREF_BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    double returnValue = 0;

#if (SUPPORT_ANY_BASE_CONVERSION == TRUE)

    char *endptr;

#else

    int endOrExponentialPart;
    int exponent;
    double outExponent;
    int length = 0;
    bool hasMinusSign = false;
    bool hasPlusSign = false;
    int decimalPoint = -1;
    int exponentialSign = -1;
    bool hasMinusExponentialSign = false;
    bool hasPlusExponentialSign = false;
    char *temp;

#endif

    char *str = (char *)stack.Arg0().RecoverString();

    // grab parameter with flag to throw on failure
    bool throwOnFailure = (bool)stack.Arg1().NumericByRefConst().u1;

    // check string parameter for null
    FAULT_ON_NULL_ARG(str);

    // skip spaces before digits
    while (*str == ' ')
    {
        str++;
    }

#if (SUPPORT_ANY_BASE_CONVERSION == TRUE)
    // support for conversion from any base

    endptr = str;

    // notice we don't try to catch errno=ERANGE - IEEE574 says overflows should just convert to infinity values
    returnValue = strtod(str, &endptr);

    if (endptr == str)
    {
        // didn't parse the string completely
        NANOCLR_SET_AND_LEAVE(CLR_E_FORMAT_EXCEPTION);
    }

    // allow spaces after digits
    while (*endptr == ' ')
    {
        endptr++;
    }

    // should reach end of string no aditional chars
    if (*endptr != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FORMAT_EXCEPTION);
    }

    stack.SetResult_R8(returnValue);

#else

    // support for conversion from base 10 and 16 (partial)
    // in this particular function the base isn't relevant

    // first pass, get count of decimal places, integer part and check for valid chars
    temp = str;

    while (*temp != '\0')
    {
        switch (*temp)
        {
            case '-':
                if (exponentialSign == -1)
                {
                    if (length == 0 && hasMinusSign == false)
                    {
                        hasMinusSign = true;

                        // point past the leading sign
                        str++;

                        // don't count this in the length
                        length--;
                    }
                    else
                    {
                        // found a minus signal NOT at the start of the string
                        NANOCLR_SET_AND_LEAVE(CLR_E_FORMAT_EXCEPTION);
                    }
                }
                else
                {
                    if (length == exponentialSign + 1)
                    {
                        hasMinusExponentialSign = true;
                    }
                    else
                    {
                        // found a minus signal NOT at the start of the exponent
                        NANOCLR_SET_AND_LEAVE(CLR_E_FORMAT_EXCEPTION);
                    }
                }
                break;

            case '+':
                if (exponentialSign == -1)
                {
                    if (length == 0 && hasPlusSign == false)
                    {
                        hasPlusSign = true;

                        // point past the leading sign
                        str++;

                        // don't count this in the length
                        length--;
                    }
                    else
                    {
                        // found a plus signal NOT at the start of the string
                        NANOCLR_SET_AND_LEAVE(CLR_E_FORMAT_EXCEPTION);
                    }
                }
                else
                {
                    if (length == exponentialSign + 1)
                    {
                        hasPlusExponentialSign = true;
                    }
                    else
                    {
                        // found a plus signal NOT at the start of the exponent
                        NANOCLR_SET_AND_LEAVE(CLR_E_FORMAT_EXCEPTION);
                    }
                }
                break;

            case '.':
                if (decimalPoint == -1)
                {
                    decimalPoint = length;
                }
                else
                {
                    // already found a decimal point, can't have another
                    NANOCLR_SET_AND_LEAVE(CLR_E_FORMAT_EXCEPTION);
                }
                break;

            case 'e':
            case 'E':
                if (exponentialSign == -1)
                {
                    exponentialSign = length;
                }
                else
                {
                    // already found a exponential sign, can't have another
                    NANOCLR_SET_AND_LEAVE(CLR_E_FORMAT_EXCEPTION);
                }
                break;

            default:
                if (*temp < '0' || *temp > '9')
                {
                    // there is an invalid char in the string
                    NANOCLR_SET_AND_LEAVE(CLR_E_FORMAT_EXCEPTION);
                }
        }
        length++;
        temp++;
    }

    //  now parse the string according to it's format
    endOrExponentialPart = exponentialSign == -1 ? length : exponentialSign;
    if (decimalPoint == -1)
    {
        // string doesn't have fractional part, treat as integer
        returnValue = GetIntegerPart(str, endOrExponentialPart);
    }
    else if (decimalPoint == 0)
    {
        // string starts with the decimal point, only has fractional part
        returnValue = GetDoubleFractionalPart((str + decimalPoint + 1), (endOrExponentialPart - decimalPoint - 1));
    }
    else
    {
        // string has integer and fractional parts
        returnValue = GetIntegerPart(str, decimalPoint);
        returnValue =
            returnValue + GetDoubleFractionalPart((str + decimalPoint + 1), (endOrExponentialPart - decimalPoint - 1));
        if (hasMinusSign)
        {
            returnValue *= -1;
        }
    }

    // exponential part found?
    if (exponentialSign != -1)
    {
        // advance by one if a sign (+ or -) is after the exponential sign
        if (hasMinusExponentialSign || hasPlusExponentialSign)
        {
            exponentialSign++;
        }

        // get the exponential part
        exponent = GetIntegerPart((str + exponentialSign + 1), (length - exponentialSign - 1));
        outExponent = pow(10, exponent);

        if (hasMinusExponentialSign)
        {
            returnValue = returnValue / outExponent;
        }
        else
        {
            returnValue = returnValue * outExponent;
        }
    }

    stack.SetResult_R8(returnValue);

#endif // defined(SUPPORT_ANY_BASE_CONVERSION)

    NANOCLR_CLEANUP();

    // set parameter reporting conversion success/failure
    stack.Arg2().Dereference()->NumericByRef().u1 = (hr == S_OK);

    // should we throw an exception?
    if (hr != S_OK && !throwOnFailure)
    {
        // nope! so clear the exception
        hr = S_OK;

        // need to set result value to 0
        stack.SetResult_R8(returnValue);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_corlib_native_System_Convert::NativeToDateTime___STATIC__SystemDateTime__STRING__BOOLEAN__BYREF_BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_TypeDescriptor dtType;
    CLR_INT64 *pRes;

    char *str = (char *)stack.Arg0().RecoverString();
    char *conversionResult = NULL;
    // char *str = (char *)"1999-10-31 10:00:00Z";
    uint64_t ticks;

    // grab parameter with flag to throw on failure
    bool throwOnFailure = (bool)stack.Arg1().NumericByRefConst().u1;

    CLR_RT_HeapBlock &ref = stack.PushValue();

    // check string parameter for null
    FAULT_ON_NULL_ARG(str);

    // initialize <DateTime> type descriptor
    NANOCLR_CHECK_HRESULT(dtType.InitializeFromType(g_CLR_RT_WellKnownTypes.m_DateTime));

    // create an instance of <DateTime>
    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObject(ref, dtType.m_handlerCls));

    pRes = Library_corlib_native_System_DateTime::GetValuePtr(ref);

    // try 'u' Universal time with sortable format (yyyy-MM-dd' 'HH:mm:ss)
    conversionResult = Nano_strptime(str, "%Y-%m-%d %H:%M:%SZ", &ticks);
    if (conversionResult == NULL)
    {
        // try 'o/O' Round Trip ISO 8601 compatible (yyyy-MM-ddTHH:mm:ss.fffffff)
        conversionResult = Nano_strptime(str, "%Y-%m-%dT%H:%M:%S.%f", &ticks);
    }

    if (conversionResult == NULL)
    {
        // try 'r/R' RFC 1123 date (ddd, dd MMM yyyy HH:mm:ss)
        conversionResult = Nano_strptime(str, "%a, %d %b %Y %H:%M:%S", &ticks);
    }

    if (conversionResult == NULL)
    {
        // failed to parse string

        NANOCLR_SET_AND_LEAVE(CLR_E_FORMAT_EXCEPTION);
    }
    else
    {
        *pRes = ticks;
    }

    NANOCLR_CLEANUP();

    // set parameter reporting conversion success/failure
    stack.Arg2().Dereference()->NumericByRef().u1 = (hr == S_OK);

    // should we throw an exception?
    if (hr != S_OK && !throwOnFailure)
    {
        // nope! so clear the exception
        hr = S_OK;
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_corlib_native_System_Convert::ToBase64String___STATIC__STRING__SZARRAY_U1__I4__I4__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if (SUPPORT_ANY_BASE_CONVERSION == TRUE)

    size_t outputLength;
    char *outArray = NULL;
    char *outArrayWitLineBreak = NULL;
    uint8_t *inArrayPointer = NULL;
    uint8_t lineBreakCount;
    uint16_t offsetIndex = 0;
    uint8_t count = 0;
    uint16_t result;

    CLR_RT_HeapBlock_Array *inArray = stack.Arg0().DereferenceArray();
    size_t offset = (size_t)stack.Arg1().NumericByRef().s4;
    size_t length = (size_t)stack.Arg2().NumericByRef().s4;
    bool insertLineBreaks = (bool)stack.Arg3().NumericByRefConst().u1;

    FAULT_ON_NULL_ARG(inArray);

    inArrayPointer = (uint8_t *)inArray->GetFirstElement();
    inArrayPointer += (offset * sizeof(uint8_t));

    // compute base64 string length
    outputLength = 4 * ((length + 2) / 3);

    // need malloc with base64 string length plus string terminator (+1)
    outArray = (char *)platform_malloc(outputLength + 1);

    // check if have allocation
    if (outArray == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // perform the operation
    // need to tweak the parameter with the output length because it includes room for the terminator
    result =
        mbedtls_base64_encode((unsigned char *)outArray, (outputLength + 1), &outputLength, inArrayPointer, length);

    if (result != 0)
    {
        // internal error occurred
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    if (insertLineBreaks)
    {
        // get line break count (line break every 76 chars)
        lineBreakCount = outputLength / 76;

        // need malloc with base64 string length plus line breaks (line break is 2 char long: CR + LF) plus final line
        // break
        outArrayWitLineBreak = (char *)platform_malloc(outputLength + (lineBreakCount * 2) + 2);

        for (int i = 0; i <= lineBreakCount; i++)
        {
            // how many chars to copy
            if (outputLength > 76)
            {
                // first/next 76 chars
                count = 76;
            }
            else
            {
                // last outputLength chars
                count = outputLength;
            }

            // copy first/next count chars
            // because we are using same offset for both arrays, we need to discount line break for tmp array
            memcpy(outArrayWitLineBreak + offsetIndex, outArray + (offsetIndex - (i * 2)), count);

            // remove copied chars from original output length if more than 76 chars still to be copied
            if (outputLength >= 76)
            {
                // more chars

                // adjust output length
                outputLength -= 76;

                // add line break
                outArrayWitLineBreak[count + offsetIndex] = '\r';
                outArrayWitLineBreak[count + offsetIndex + 1] = '\n';

                // move offset for next copy (including line break +2)
                offsetIndex += count + 2;
            }
            else
            {
                // move offset for last position
                offsetIndex += count;
                // reached end of array, add terminator
                outArrayWitLineBreak[offsetIndex] = 0;
            }
        }
        // set a return result in the stack argument using the appropriate SetResult according to the variable type (a
        // string here)
        NANOCLR_CHECK_HRESULT(stack.SetResult_String(outArrayWitLineBreak));
    }
    else
    {
        // set a return result in the stack argument using the appropriate SetResult according to the variable type (a
        // string here)
        NANOCLR_CHECK_HRESULT(stack.SetResult_String(outArray));
    }

    // need to free memory from arrays
    platform_free((void *)outArray);

    if (outArrayWitLineBreak != NULL)
    {
        platform_free((void *)outArrayWitLineBreak);
    }

#else

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Convert::FromBase64String___STATIC__SZARRAY_U1__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if (SUPPORT_ANY_BASE_CONVERSION == TRUE)

    CLR_RT_HeapBlock_String *inString = NULL;
    size_t outputLength;
    char *outArray = NULL;
    CLR_UINT8 *returnArray;
    uint16_t result;
    size_t length;

    inString = stack.Arg0().DereferenceString();
    FAULT_ON_NULL(inString);

    FAULT_ON_NULL_ARG(inString->StringText());

    length = (size_t)hal_strlen_s(inString->StringText());

    // estimate output length
    outputLength = length / 4 * 3;

    // alloc output array
    outArray = (char *)platform_malloc(outputLength + 1);
    // check malloc success
    if (outArray == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // perform the operation
    // need to tweak the parameter with the output length because it includes room for the terminator
    result = mbedtls_base64_decode(
        (unsigned char *)outArray,
        (outputLength + 1),
        &outputLength,
        (const unsigned char *)inString->StringText(),
        length);

    if (result != 0)
    {
        // internal error occurred
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    // create heap block array instance with appropriate size (the length of the output array)
    // and type (byte which is uint8_t)
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
        stack.PushValueAndClear(),
        outputLength,
        g_CLR_RT_WellKnownTypes.m_UInt8));

    // get a pointer to the array in the heap block array just created
    returnArray = stack.TopValue().DereferenceArray()->GetFirstElement();

    // copy outArray to the returnArray
    memcpy(returnArray, outArray, outputLength);

    NANOCLR_CLEANUP();

    if (outArray)
    {
        // need to free memory from outArray
        platform_free(outArray);
    }

    NANOCLR_CLEANUP_END();

#else

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();

#endif
}

double Library_corlib_native_System_Convert::GetDoubleFractionalPart(char *str, int length)
{
    double place = 1;
    double returnValue = 0.0;

    for (int i = 0; i < length; i++)
    {
        // move decimal place to the right
        place /= 10.0;

        returnValue += ((int)(*str++) - '0') * place;
    }

    return returnValue;
}

int64_t Library_corlib_native_System_Convert::GetIntegerPart(char *str, int length)
{
    int64_t returnValue = 0;

    for (int i = 0; i < length; i++)
    {
        returnValue = returnValue * 10 + (*str - '0');
        str++;

        // check for terminator, in case this is being called in 'guess' mode
        if (*str == '\0')
        {
            break;
        }
    }

    return returnValue;
}

int64_t Library_corlib_native_System_Convert::GetIntegerFromHexString(char *str)
{
    int64_t returnValue = 0;

    if ((*str == '0') && (*(str + 1) == 'x'))
    {
        // there a 0x at the begining of the string, so move pointer forward 2 notches
        str += 2;
    }

    while (*str != '\0')
    {
        char c = toupper(*str++);

        if ((c < '0') || (c > 'F') || ((c > '9') && (c < 'A')))
        {
            // there is an invalid char in the string
            break;
        }

        c -= '0';

        if (c > 9)
        {
            c -= 7;
        }

        returnValue = (returnValue << 4) + c;
    }

    return returnValue;
}

/////////////////////////////////////////////////////////////
// support functions for string to date/time conversion    //
// heavily inspired in the strptime from the GNU C library //
/////////////////////////////////////////////////////////////

static const char *abday[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char *abmon[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

static int nano_strncasecmp(const char *s1, const char *s2, size_t n)
{
    if (n != 0)
    {
        const unsigned char *us1 = (const unsigned char *)s1;
        const unsigned char *us2 = (const unsigned char *)s2;

        do
        {
            if (tolower(*us1) != tolower(*us2++))
            {
                return tolower(*us1) - tolower(*--us2);
            }

            if (*us1++ == '\0')
            {
                break;
            }

        } while (--n != 0);
    }

    return 0;
}

static int nano_conv_num(const char **buf, int *dest, int lowerLimit, int upperLimit)
{
    int result = 0;

    // The limit also determines the number of valid digits
    int runningLimit = upperLimit;

    if (**buf < '0' || **buf > '9')
    {
        return 0;
    }

    do
    {
        result *= 10;
        result += *(*buf)++ - '0';
        runningLimit /= 10;
    } while ((result * 10 <= upperLimit) && runningLimit && **buf >= '0' && **buf <= '9');

    if (result < lowerLimit || result > upperLimit)
    {
        return 0;
    }

    *dest = result;

    return (1);
}

char *Library_corlib_native_System_Convert::Nano_strptime(const char *buf, const char *format, uint64_t *ticks)
{
    char c;
    const char *bufPointer;
    size_t len = 0;
    int i, value = 0;
    int extraTicks = 0;
    SYSTEMTIME st;

    memset(&st, 0, sizeof(SYSTEMTIME));

    // reset this so it doesn't return wrong values in case of error
    *ticks = 0;

    bufPointer = buf;

    while ((c = *format) != '\0')
    {
        // consume white-space
        if (isspace((int)c))
        {
            while (isspace((int)*bufPointer))
            {
                bufPointer++;
            }

            format++;
            continue;
        }

        if ((c = *format++) != '%')
        {
            goto literal;
        }

        switch (c = *format++)
        {
            // "%%" is converted to "%"
            case '%':
            literal:
                if (c != *bufPointer++)
                {
                    return NULL;
                }
                break;

            // "Elementary" conversion rules.
            // day of week (abbreviation)
            case 'a':
                for (i = 0; i < 7; i++)
                {
                    // Full name not implemented

                    // Abbreviated name
                    len = hal_strlen_s(abday[i]);
                    if (nano_strncasecmp(abday[i], bufPointer, len) == 0)
                    {
                        break;
                    }
                }

                // no match
                if (i == 7)
                {
                    return NULL;
                }

                st.wDayOfWeek = i;
                bufPointer += len;
                break;

            // month (2 digits)
            case 'b':
                for (i = 0; i < 12; i++)
                {
                    // Full name not implemented

                    // Abbreviated name
                    len = hal_strlen_s(abmon[i]);
                    if (nano_strncasecmp(abmon[i], bufPointer, len) == 0)
                    {
                        break;
                    }
                }

                // no match
                if (i == 12)
                {
                    return NULL;
                }

                st.wMonth = i + 1;
                bufPointer += len;
                break;

            // day of month (2 digits)
            case 'd':
                if (!(nano_conv_num(&bufPointer, &value, 1, 31)))
                {
                    return NULL;
                }
                st.wDay = value;
                break;

            // hour (24-hour)
            case 'H':
                if (!(nano_conv_num(&bufPointer, &value, 0, 23)))
                {
                    return NULL;
                }
                st.wHour = value;
                break;

            // minutes (2 digits)
            case 'M':
                if (!(nano_conv_num(&bufPointer, &value, 0, 59)))
                {
                    return NULL;
                }
                st.wMinute = value;
                break;

            // month (2 digits)
            case 'm':
                if (!(nano_conv_num(&bufPointer, &value, 1, 12)))
                {
                    return NULL;
                }
                st.wMonth = value;
                break;

            // seconds (2 digits)
            case 'S':
                if (!(nano_conv_num(&bufPointer, &value, 0, 59)))
                {
                    return NULL;
                }
                st.wSecond = value;
                break;

            // year (4 digits)
            case 'Y':
                if (!(nano_conv_num(&bufPointer, &value, 0, 9999)))
                {
                    return NULL;
                }

                st.wYear = value;
                break;

            // ticks (any length)
            case 'f':
                if (!(nano_conv_num(&bufPointer, &value, 0, 9999999)))
                {
                    return NULL;
                }

                extraTicks = value;
                break;

            // Miscellaneous conversions
            // Any kind of white-space
            case 'n':
            case 't':
                while (isspace((int)*bufPointer))
                {
                    bufPointer++;
                }
                break;

                // Unknown/unsupported conversion
            default:
                return NULL;
        }
    }

    // convert to .NET ticks
    *ticks = HAL_Time_ConvertFromSystemTimeWithTicks(&st, extraTicks);

    // return whatever is left on the buffer
    return ((char *)bufPointer);
}
