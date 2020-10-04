//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "corlib_native.h"
#include <ctype.h>
#include <base64.h>

HRESULT Library_corlib_native_System_Convert::NativeToInt64___STATIC__I8__STRING__BOOLEAN__I8__I8__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        int64_t result = 0;

		char* str = (char*)stack.Arg0().RecoverString();
        signed int radix = stack.Arg4().NumericByRef().s4;

#if (SUPPORT_ANY_BASE_CONVERSION == TRUE)
        // suport for conversion from any base
        char* endptr = NULL;

        bool isUInt64 = false;

        bool isSigned = (bool)stack.Arg1().NumericByRef().u1;
        long long minValue = stack.Arg2().NumericByRef().s8;
        long long maxValue = stack.Arg3().NumericByRef().s8;
        
        // UInt64? => use also strtoull the result will be casted to Int64
        if (minValue == 0 && maxValue == 0) {
            isUInt64 = true;
            isSigned = false;

            //allow spaces before digits
            while (*str == ' ') {
                str++;
            }

            //UInt64 can't begin with minus
            if (*str == '-' ) {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
            }
        }

        // convert via strtoll / strtoull
        result = isSigned ? strtoll(str, &endptr, radix) : (long long) strtoull(str, &endptr, radix);

        // TODO: 
        // If the value in input string is out of the range of representable values
        // by a long long int / unsigned long int, the function returns 
        // LLONG_MAX or LLONG_MIN for signed conversion 
        // and ULONG_MAX for unsigned conversion
        // It is necessary to add a check

        // if no valid conversion endptr is equal str
        if (str == endptr) {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // allow spaces after digits
        while (*endptr == ' ') {
            endptr++;
        }

        // should reach end of string no aditional chars
        if (*endptr != 0) {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        
        // the signed values for SByte, Int16 and Int32 are always positive for base 2, 8 or 16 conversions
        // because the 64-bit function strtoll is used; need the post process the value
        // if the result is greater max and smaller (max + 1) * 2 this value should be subtracted
        if (radix == 2 || radix == 8 || radix == 16) {
            if (isSigned && result > maxValue && result < (maxValue + 1) * 2) result -= (maxValue + 1) * 2;
        }

        if (!isUInt64 && !isSigned && (uint64_t)result > (uint64_t)maxValue) {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
        } else if (!isUInt64 && (result > maxValue || result < minValue)) {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
        } else {
            stack.SetResult_I8(result);
        }
    }
    NANOCLR_NOCLEANUP();
#else
        // support for conversion from base 10 and 16 (partial)

        if(radix == 10)
        {
            // conversion from base 10

            // check for minus sign
            if(*str == '-')
            {
                // call GetIntegerPart() in 'guess' mode by throwing a crazy number for the length
                result = GetIntegerPart((str + 1), 99);

                // is negative
                result *= -1;
            }
            // check for plus sign
            else if(*str == '+')
            {
                // call GetIntegerPart() in 'guess' mode by throwing a crazy number for the length
                result = GetIntegerPart((str + 1), 99);
            }
            else
            {
                // call GetIntegerPart() in 'guess' mode by throwing a crazy number for the length
                result = GetIntegerPart(str, 99);
            }

        }
        else if(radix == 16)
        {
            // conversion from base 16
            result = GetIntegerFromHexString(str);
        }
        else
        {
            // all other bases are not supported
            return stack.NotImplementedStub();
        }

        stack.SetResult_I8(result);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
#endif // defined(SUPPORT_ANY_BASE_CONVERSION)
    
}

HRESULT Library_corlib_native_System_Convert::NativeToDouble___STATIC__R8__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        char* str = (char*)stack.Arg0().RecoverString();

  #if (SUPPORT_ANY_BASE_CONVERSION == TRUE)
        // suport for conversion from any base

        stack.SetResult_R8 (strtod(str, nullptr));
    }
    NANOCLR_NOCLEANUP_NOLABEL();

  #else
        // support for conversion from base 10 and 16 (partial)
        // in this particular function the base isn't relevant

        int length = 0;
        bool hasMinusSign = false;
        bool hasPlusSign = false;
        int decimalPoint = -1;
        int exponentialSign = -1;
        bool hasMinusExponentialSign = false;
        bool hasPlusExponentialSign = false;
        double returnValue = 0.0;

        // first pass, get count of decimal places, integer part and check for valid chars
        char* temp  = str;
        while (*temp != '\0')
        {
            switch (*temp)
            {
                case '-':
                    if (exponentialSign == -1)
                    {
                        if (length == 0)
                        {
                            hasMinusSign = true;
                        }
                        else 
                        {
                            // found a minus signal NOT at the start of the string
                            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
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
                            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                        }
                    }
                    break;

                case '+':
                    if (exponentialSign == -1)
                    {
                        if (length == 0)
                        {
                            hasPlusSign = true;
                        }
                        else 
                        {
                            // found a plus signal NOT at the start of the string
                            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
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
                            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
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
                        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
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
                        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                    }
                    break;

                default:
                    if (*temp < '0' && *temp > '9')
                    {
                        // there is an invalid char in the string
                        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                    }
            }
            length++;
            temp++;
        }

        //  now parse the string according to it's format
        int endOrExponentialPart = exponentialSign == -1 ? length : exponentialSign;
        if (decimalPoint == -1)
        {
            // string doesn't have fractional part, treat as integer
            returnValue = GetIntegerPart(str, endOrExponentialPart);
        }
        else if (decimalPoint == 0)
        {
            // string starts with the decimal point, only has fractional part
            returnValue = GetDoubleFractionalPart((str+decimalPoint+1), (endOrExponentialPart-decimalPoint-1));
        }
        else if (hasMinusSign || hasPlusSign)
        {
            // string starts with sign and...
            
            if(decimalPoint == 1)
            {
                // ... is followed by a decimal point, only has fractional part
                returnValue = GetDoubleFractionalPart((str+decimalPoint+1), (endOrExponentialPart-decimalPoint-1));
            }
            else
            {
                // ... has integer and fractional parts
                returnValue = GetIntegerPart(str+1, decimalPoint-1);
                returnValue = (returnValue + GetDoubleFractionalPart((str+decimalPoint+1), (endOrExponentialPart-decimalPoint-1)));
            }

            if (hasMinusSign)
            {
                returnValue *= -1;
            }
        }
        else
        {
            // string has integer and fractional parts
            returnValue = GetIntegerPart(str, decimalPoint);
            returnValue = returnValue + GetDoubleFractionalPart((str+decimalPoint+1), (endOrExponentialPart-decimalPoint-1));
        }

        // exponential part found?
        if (exponentialSign != -1)
        {
            // advance by one if a sign (+ or -) is after the exponential sign
            if (hasMinusExponentialSign || hasPlusExponentialSign) exponentialSign++;
            // get the exponential part
            int exponent = GetIntegerPart((str+exponentialSign+1), (length-exponentialSign-1));
            // each time multiply or divide by 10
            for (int i = 0; i < exponent; i++)
            {
                if (hasMinusExponentialSign)
                {
                    returnValue /= 10;
                }
                else
                {
                    returnValue *= 10;
                }
            }
        }

        stack.SetResult_R8(returnValue);
	}
    NANOCLR_NOCLEANUP();

  #endif // defined(SUPPORT_ANY_BASE_CONVERSION)        

}

HRESULT Library_corlib_native_System_Convert::ToBase64String___STATIC__STRING__SZARRAY_U1__I4__I4__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    size_t outputLength;
    char* outArray = NULL;
    char* outArrayWitLineBreak = NULL;
    uint8_t* inArrayPointer = NULL;
    uint8_t lineBreakCount;
    uint16_t offsetIndex = 0;
    uint8_t count = 0;
    uint16_t result;

    CLR_RT_HeapBlock_Array* inArray = stack.Arg0().DereferenceArray(); 
    size_t offset = (size_t)stack.Arg1().NumericByRef().s4;
    size_t length = (size_t)stack.Arg2().NumericByRef().s4;
    bool insertLineBreaks = (bool)stack.Arg3().NumericByRefConst().u1;

    if(inArray == NULL) NANOCLR_SET_AND_LEAVE(CLR_E_ARGUMENT_NULL);

    inArrayPointer = (uint8_t*)inArray->GetFirstElement();
    inArrayPointer += (offset * sizeof(uint8_t));

    // compute base64 string length
    outputLength = 4 * ((length + 2) / 3);

    // need malloc with base64 string length plus string terminator (+1)
    outArray = (char*)platform_malloc(outputLength + 1);

    // check if have allocation
    if (outArray == NULL) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);

    // perform the operation
    // need to tweak the parameter with the output length because it includes room for the terminator
    result = mbedtls_base64_encode( 
        (unsigned char*)outArray, 
        (outputLength + 1), 
        &outputLength, 
        inArrayPointer, 
        length );

    if(result != 0)
    {
        // internal error occurred
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    if(insertLineBreaks)
    {
        // get line break count (line break every 76 chars)
        lineBreakCount = outputLength / 76;

        // need malloc with base64 string length plus line breaks (line break is 2 char long: CR + LF) plus final line break
        outArrayWitLineBreak = (char*)platform_malloc(outputLength + (lineBreakCount * 2) + 2);

        for(int i = 0; i <= lineBreakCount; i++)
        {
            // how many chars to copy
            if(outputLength > 76)
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
            if(outputLength >= 76)
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
        // set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
        NANOCLR_CHECK_HRESULT(stack.SetResult_String(outArrayWitLineBreak));
    }
    else
    {
        // set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
        NANOCLR_CHECK_HRESULT(stack.SetResult_String(outArray));
    }

    // need to free memory from arrays
    platform_free((void*)outArray);

    if(outArrayWitLineBreak != NULL)
    {
        platform_free((void*)outArrayWitLineBreak);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Convert::FromBase64CharArray___STATIC__SZARRAY_U1__SZARRAY_CHAR__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    size_t outputLength;
    char* outArray = NULL;
    uint16_t* inArrayPointerTmp = NULL;
    uint8_t* inArrayPointer = NULL;
    uint8_t charValue;
    CLR_UINT8* returnArray;
    int16_t i = 0;
    uint16_t result;

    CLR_RT_HeapBlock_Array* inArray = stack.Arg0().DereferenceArray();
    size_t length = (size_t)stack.Arg1().NumericByRef().s4;

    if(inArray == NULL) NANOCLR_SET_AND_LEAVE(CLR_E_ARGUMENT_NULL);

    outputLength = length / 4 * 3;

    // transform the 16 bits inArray to a 8 bits array so mbed knows how to convert it
    inArrayPointerTmp = (uint16_t*)inArray->GetFirstElementUInt16();
    inArrayPointer = (uint8_t*)inArray->GetFirstElement();
    for(i = 0; i < (int16_t)length; i++)
    {
        *inArrayPointer = *inArrayPointerTmp;
        inArrayPointer++;
        inArrayPointerTmp++;
    }
    
    // pointer is pointing to the end 
    // point to last char in array and get it
    inArrayPointer--;
    charValue = *inArrayPointer;
    // adjust output length
    if (charValue == '=')
    {
        outputLength--;
    }

    // point to before last char and get it
    inArrayPointer--;
    charValue = *inArrayPointer;
    // adjust output length
    if (charValue == '=') 
    {
        outputLength--;
    }

    // reset pointer position (-2 because above we already went back two positions)
    inArrayPointer -= length - 2;

    outArray = (char*)platform_malloc(outputLength + 1);
    // check malloc success
    if (outArray == NULL) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);

    // perform the operation
    // need to tweak the parameter with the output length because it includes room for the terminator
    result = mbedtls_base64_decode( 
        (unsigned char*)outArray, 
        (outputLength + 1), 
        &outputLength, 
        inArrayPointer, 
        length );

    if(result != 0)
    {
        // internal error occurred
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    // create heap block array instance with appropriate size (the length of the output array) and type (byte which is uint8_t)
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( stack.PushValueAndClear() , outputLength, g_CLR_RT_WellKnownTypes.m_UInt8 ));

    // get a pointer to the array in the heap block array just created
    returnArray = stack.TopValue().DereferenceArray()->GetFirstElement();

    // copy outArray to the returnArray
    memcpy(returnArray, outArray, outputLength);

    // need to free memory from outArray
    platform_free(outArray);

    NANOCLR_NOCLEANUP();
}

double Library_corlib_native_System_Convert::GetDoubleFractionalPart(char* str, int length)
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

int64_t Library_corlib_native_System_Convert::GetIntegerPart(char* str, int length)
{
    int64_t returnValue = 0;

    for (int i = 0; i < length; i++)
    {
        returnValue = returnValue * 10 + (*str - '0');
        str++;

        // check for terminator, in case this is being called in 'guess' mode
        if(*str == '\0')
        {
            break;
        }
    }

    return returnValue;
}

int64_t Library_corlib_native_System_Convert::GetIntegerFromHexString(char* str)
{
    int64_t returnValue = 0;

	if ((*str == '0') && (*(str+1) == 'x'))
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
