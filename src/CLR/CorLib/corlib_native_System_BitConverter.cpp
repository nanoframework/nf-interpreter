//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"

char DigitalToHex(unsigned char x)
{
	return x < 10 ? x + '0' :  x - 10 + 'A';
}

char* ByteArrayToHex(unsigned char* pInput, int index, int length)
{
	char* pOutput = (char*) malloc(length * 3);
	char* p = pOutput;

	pInput += index;
	for(int i = 0; i < length; i++, pInput++)
	{
		*p++ = DigitalToHex(*pInput / 16);
		*p++ = DigitalToHex(*pInput % 16);
		*p++ = '-';
	}
	*(--p) = 0;

	return pOutput;
}


HRESULT Library_corlib_native_System_BitConverter::get_IsLittleEndian___STATIC__BOOLEAN( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	unsigned long x = 0x12345678;
	unsigned char* p = reinterpret_cast<unsigned char*>(&x);
	stack.SetResult_Boolean(*p == 0x78);

	NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_corlib_native_System_BitConverter::DoubleToInt64Bits___STATIC__I8__R8( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

#if !defined(NANOCLR_EMULATED_FLOATINGPOINT)
	double input = stack.Arg0().NumericByRefConst().r8;
#else
	CLR_INT64 input = stack.Arg0().NumericByRefConst().r8;
#endif
	__int64* p = reinterpret_cast<__int64*>(&input);
	stack.SetResult_I8(*p);

	NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_corlib_native_System_BitConverter::GetBytes___STATIC__SZARRAY_U1__BOOLEAN( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	bool input = stack.Arg0().NumericByRefConst().u1 != 0;

	NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(stack.PushValueAndClear(), 1, g_CLR_RT_WellKnownTypes.m_UInt8));
	{
		unsigned char* p = stack.TopValue().DereferenceArray()->GetFirstElement();
		*reinterpret_cast<bool*>(p) = input;
	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_BitConverter::GetBytes___STATIC__SZARRAY_U1__CHAR( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	wchar_t input = stack.Arg0().NumericByRefConst().u2;

	NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(stack.PushValueAndClear(), 2, g_CLR_RT_WellKnownTypes.m_UInt8));
	{
		unsigned char* p = stack.TopValue().DereferenceArray()->GetFirstElement();
		*reinterpret_cast<wchar_t*>(p) = input;
	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_BitConverter::GetBytes___STATIC__SZARRAY_U1__R8( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

#if !defined(NANOCLR_EMULATED_FLOATINGPOINT)
	double input = stack.Arg0().NumericByRefConst().r8;
#else
	CLR_INT64 input = stack.Arg0().NumericByRefConst().r8;
#endif

	NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(stack.PushValueAndClear(), 8, g_CLR_RT_WellKnownTypes.m_UInt8));
	{
		unsigned char* p = stack.TopValue().DereferenceArray()->GetFirstElement();
#if !defined(NANOCLR_EMULATED_FLOATINGPOINT)
		*reinterpret_cast<double*>(p) = input;
#else
		*reinterpret_cast<CLR_INT64*>(p) = input;
#endif
	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_BitConverter::GetBytes___STATIC__SZARRAY_U1__R4( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

#if !defined(NANOCLR_EMULATED_FLOATINGPOINT)
	float input = stack.Arg0().NumericByRefConst().r4;
#else
	CLR_INT32 input = stack.Arg0().NumericByRefConst().r4;
#endif

	NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(stack.PushValueAndClear(), 4, g_CLR_RT_WellKnownTypes.m_UInt8));
	{
		unsigned char* p = stack.TopValue().DereferenceArray()->GetFirstElement();
#if !defined(NANOCLR_EMULATED_FLOATINGPOINT)
		*reinterpret_cast<float*>(p) = input;
#else
		*reinterpret_cast<CLR_INT32*>(p) = input;
#endif
	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_BitConverter::GetBytes___STATIC__SZARRAY_U1__I4( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	int input = stack.Arg0().NumericByRefConst().s4;

	NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(stack.PushValueAndClear(), 4, g_CLR_RT_WellKnownTypes.m_UInt8));
	{
		unsigned char* p = stack.TopValue().DereferenceArray()->GetFirstElement();
		*reinterpret_cast<int*>(p) = input;
	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_BitConverter::GetBytes___STATIC__SZARRAY_U1__I8( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	__int64 input = stack.Arg0().NumericByRefConst().s8;

	NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(stack.PushValueAndClear(), 8, g_CLR_RT_WellKnownTypes.m_UInt8));
	{
		unsigned char* p = stack.TopValue().DereferenceArray()->GetFirstElement();
		*reinterpret_cast<__int64*>(p) = input;
	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_BitConverter::GetBytes___STATIC__SZARRAY_U1__I2( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	short input = stack.Arg0().NumericByRefConst().s2;

	NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(stack.PushValueAndClear(), 2, g_CLR_RT_WellKnownTypes.m_UInt8));
	{
		unsigned char* p = stack.TopValue().DereferenceArray()->GetFirstElement();
		*reinterpret_cast<short*>(p) = input;
	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_BitConverter::GetBytes___STATIC__SZARRAY_U1__U4( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	unsigned int input = stack.Arg0().NumericByRefConst().u4;

	NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(stack.PushValueAndClear(), 4, g_CLR_RT_WellKnownTypes.m_UInt8));
	{
		unsigned char* p = stack.TopValue().DereferenceArray()->GetFirstElement();
		*reinterpret_cast<unsigned int*>(p) = input;
	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_BitConverter::GetBytes___STATIC__SZARRAY_U1__U8( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	unsigned __int64 input = stack.Arg0().NumericByRefConst().u8;

	NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(stack.PushValueAndClear(), 8, g_CLR_RT_WellKnownTypes.m_UInt8));
	{
		unsigned char* p = stack.TopValue().DereferenceArray()->GetFirstElement();
		*reinterpret_cast<unsigned __int64*>(p) = input;
	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_BitConverter::GetBytes___STATIC__SZARRAY_U1__U2( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	unsigned short input = stack.Arg0().NumericByRefConst().u2;

	NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(stack.PushValueAndClear(), 2, g_CLR_RT_WellKnownTypes.m_UInt8));
	{
		unsigned char* p = stack.TopValue().DereferenceArray()->GetFirstElement();
		*reinterpret_cast<unsigned short*>(p) = input;
	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_BitConverter::Int64BitsToDouble___STATIC__R8__I8( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	__int64 input = stack.Arg0().NumericByRefConst().s8;
#if !defined(NANOCLR_EMULATED_FLOATINGPOINT)
	double* p = reinterpret_cast<double*>(&input);
#else
	CLR_INT64* p = reinterpret_cast<CLR_INT64*>(&input);
#endif
	stack.SetResult_R8(*p);

	NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_corlib_native_System_BitConverter::ToBoolean___STATIC__BOOLEAN__SZARRAY_U1__I4( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	unsigned char *p = NULL;
	unsigned char b=0;
	int index = 0;
	bool res = false;
	
	CLR_RT_HeapBlock_Array* pArray = stack.Arg0().DereferenceArray();
	FAULT_ON_NULL_ARG(pArray);

	index = stack.Arg1().NumericByRefConst().s4;
	if (index < 0 || (unsigned int)index >= pArray->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);

	p = pArray->GetFirstElement(); 
	b = *(p + index);
	res = (b != 0); // assume any value not zero is true. Zero is false. 
	stack.SetResult_Boolean(res);

	NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_BitConverter::ToChar___STATIC__CHAR__SZARRAY_U1__I4( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	unsigned char *p = NULL;
	int index = 0;         
	wchar_t value=0;
	unsigned char *pValue = (unsigned char*)(&value);

	CLR_RT_HeapBlock_Array* pArray = stack.Arg0().DereferenceArray();
	FAULT_ON_NULL_ARG(pArray);

	index = stack.Arg1().NumericByRefConst().s4;
	if (index < 0 || (unsigned int)index >= pArray->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
	if ((unsigned int)index + 2 > pArray->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

	p = pArray->GetFirstElement();              
	for (int i=0; i<2; ++i)
	{
		pValue[i] = p[index+i];
	}

	stack.SetResult(value, DATATYPE_CHAR);
	NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_BitConverter::ToDouble___STATIC__R8__SZARRAY_U1__I4( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	unsigned char *p = NULL;
	int index = 0;                       
	
	double value=0;
	unsigned char *pValue = (unsigned char*)(&value);

	CLR_RT_HeapBlock_Array* pArray = stack.Arg0().DereferenceArray();
	FAULT_ON_NULL_ARG(pArray);

	index = stack.Arg1().NumericByRefConst().s4;
	if (index < 0 || (unsigned int)index >= pArray->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
	if ((unsigned int)index + 8 > pArray->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
					
	// fetch unsigned char aligned values, and store in object that is correctly aligned for this system. 									 
	p = pArray->GetFirstElement();
	for (int i=0; i<8; ++i)
	{
		pValue[i] = p[index+i];
	}
	stack.SetResult_R8(value);	 
		 
	NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_BitConverter::ToSingle___STATIC__R4__SZARRAY_U1__I4( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	unsigned char *p = NULL;
	int index = 0;      
	float value=0;
	unsigned char *pValue = (unsigned char*)(&value);

	CLR_RT_HeapBlock_Array* pArray = stack.Arg0().DereferenceArray();
	FAULT_ON_NULL_ARG(pArray);

	index = stack.Arg1().NumericByRefConst().s4;
	if (index < 0 || (unsigned int)index >= pArray->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
	if ((unsigned int)index + 4 > pArray->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
	 
	// fetch unsigned char aligned values, and store in object that is correctly aligned for this system. 
	p = pArray->GetFirstElement();   
	
	for (int i=0; i<4; ++i)
	{
		pValue[i] = p[index+i];
	}   
	stack.SetResult_R4(value);

	NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_BitConverter::ToInt16___STATIC__I2__SZARRAY_U1__I4( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	unsigned char *p = NULL;
	int index = 0;       
	signed short value=0;
	unsigned char *pValue = (unsigned char*)(&value);

	CLR_RT_HeapBlock_Array* pArray = stack.Arg0().DereferenceArray();
	FAULT_ON_NULL_ARG(pArray);

	index = stack.Arg1().NumericByRefConst().s4;
	if (index < 0 || (unsigned int)index >= pArray->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
	if ((unsigned int)index + 2 > pArray->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

	p = pArray->GetFirstElement();     
	// big vs little endian?
	for (int i=0; i<2; ++i)
	{
		pValue[i] = p[index+i];
	}   
	stack.SetResult_I4(value);
	
	NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_BitConverter::ToInt32___STATIC__I4__SZARRAY_U1__I4( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	unsigned char *p = NULL;
	int index = 0;  
	
    signed int value=0;
	unsigned char *pValue = (unsigned char*)(&value);

	CLR_RT_HeapBlock_Array* pArray = stack.Arg0().DereferenceArray();
	FAULT_ON_NULL_ARG(pArray);

	index = stack.Arg1().NumericByRefConst().s4;
	if (index < 0 || (unsigned int)index >= pArray->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
	if ((unsigned int)index + 4 > pArray->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

	p = pArray->GetFirstElement();    
		// big vs little endian?
	for (int i=0; i<4; ++i)
	{
		pValue[i] = p[index+i];
	}   
	stack.SetResult_I4(value);

	NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_BitConverter::ToInt64___STATIC__I8__SZARRAY_U1__I4( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	unsigned char *p = NULL;
	int index = 0;
					 
	CLR_INT64 value=0;
	unsigned char *pValue = (unsigned char*)(&value);
	
	CLR_RT_HeapBlock_Array* pArray = stack.Arg0().DereferenceArray();
	FAULT_ON_NULL_ARG(pArray);

	index = stack.Arg1().NumericByRefConst().s4;
	if (index < 0 || (unsigned int)index >= pArray->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
	if ((unsigned int)index + 8 > pArray->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

	p = pArray->GetFirstElement();    
	
	for (int i=0; i<8; ++i)
	{
		pValue[i] = p[index+i];
	}   
	stack.SetResult_I8(value) ;
	

	NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_BitConverter::ToUInt16___STATIC__U2__SZARRAY_U1__I4( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	unsigned char *p = NULL;
	int index = 0;
	CLR_UINT16 value=0;
	unsigned char *pValue = (unsigned char*)(&value);

	CLR_RT_HeapBlock_Array* pArray = stack.Arg0().DereferenceArray();
	FAULT_ON_NULL_ARG(pArray);

	index = stack.Arg1().NumericByRefConst().s4;
	if (index < 0 || (unsigned int)index >= pArray->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
	if ((unsigned int)index + 2 > pArray->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

	p = pArray->GetFirstElement();
	for (int i=0; i<2; ++i)
	{
		pValue[i] = p[index+i];
	}   
	stack.SetResult_I4(value);

	NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_BitConverter::ToUInt32___STATIC__U4__SZARRAY_U1__I4( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	unsigned char *p = NULL;
	int index = 0;  
	CLR_UINT32 value=0;
	unsigned char *pValue = (unsigned char*)(&value);

	CLR_RT_HeapBlock_Array* pArray = stack.Arg0().DereferenceArray();
	FAULT_ON_NULL_ARG(pArray);

	index = stack.Arg1().NumericByRefConst().s4;
	if (index < 0 || (unsigned int)index >= pArray->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
	if ((unsigned int)index + 4 > pArray->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

	p = pArray->GetFirstElement();     
	for (int i=0; i<4; ++i)
	{
		pValue[i] = p[index+i];
	}   
	stack.SetResult_U4(value) ;

	NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_BitConverter::ToUInt64___STATIC__U8__SZARRAY_U1__I4( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	unsigned char *p = NULL;
	int index = 0;       
	CLR_UINT64 value=0;
	unsigned char *pValue = (unsigned char*)(&value);

	CLR_RT_HeapBlock_Array* pArray = stack.Arg0().DereferenceArray();
	FAULT_ON_NULL_ARG(pArray);

	index = stack.Arg1().NumericByRefConst().s4;
	if (index < 0 || (unsigned int)index >= pArray->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
	if ((unsigned int)index + 8 > pArray->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

	p = pArray->GetFirstElement();    
	  
	for (int i=0; i<8; ++i)
	{
		pValue[i] = p[index+i];
	}   
	stack.SetResult_U8(value)  ;
	

	NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_BitConverter::ToString___STATIC__STRING__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	CLR_RT_HeapBlock_Array* pArray = stack.Arg0().DereferenceArray();
	FAULT_ON_NULL_ARG(pArray);

	if (pArray->m_numOfElements == 0)
	{
		NANOCLR_CHECK_HRESULT(stack.SetResult_String(""));
	}
	else
	{
		unsigned char* p = pArray->GetFirstElement();
		char* pOutput = ByteArrayToHex(p, 0, pArray->m_numOfElements);
		NANOCLR_CHECK_HRESULT(stack.SetResult_String(pOutput));
		free(pOutput);
	}

	NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_BitConverter::ToString___STATIC__STRING__SZARRAY_U1__I4( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	int index = 0;

	CLR_RT_HeapBlock_Array* pArray = stack.Arg0().DereferenceArray();
	FAULT_ON_NULL_ARG(pArray);

	index = stack.Arg1().NumericByRefConst().s4;
	if (pArray->m_numOfElements == 0 && index == 0)
	{
		NANOCLR_CHECK_HRESULT(stack.SetResult_String(""));
	}
	else
	{
		if (index < 0 || (unsigned int)index >= pArray->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);

		unsigned char* p = pArray->GetFirstElement();
		char* pOutput = ByteArrayToHex(p, index, pArray->m_numOfElements - index);
		NANOCLR_CHECK_HRESULT(stack.SetResult_String(pOutput));
		free(pOutput);
	}

	NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_BitConverter::ToString___STATIC__STRING__SZARRAY_U1__I4__I4( CLR_RT_StackFrame& stack )
{
	NATIVE_PROFILE_CLR_CORE();
	NANOCLR_HEADER();

	int index = 0;
	int length = 0;

	CLR_RT_HeapBlock_Array* pArray = stack.Arg0().DereferenceArray();
	FAULT_ON_NULL_ARG(pArray);

	index = stack.Arg1().NumericByRefConst().s4;
	length = stack.Arg2().NumericByRefConst().s4;
	if (pArray->m_numOfElements == 0 && index == 0 && length == 0)
	{
		NANOCLR_CHECK_HRESULT(stack.SetResult_String(""));
	}
	else
	{
		if (index < 0 || length < 0 || (unsigned int)index >= pArray->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
		if ((unsigned int)index + length > pArray->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

		unsigned char* p = pArray->GetFirstElement();
		char* pOutput = ByteArrayToHex(p, index, length);
		NANOCLR_CHECK_HRESULT(stack.SetResult_String(pOutput));
		free(pOutput);
	}

	NANOCLR_NOCLEANUP();
}
