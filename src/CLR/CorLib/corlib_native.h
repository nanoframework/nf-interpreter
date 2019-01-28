//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#ifndef _CORLIB_NATIVE_H_
#define _CORLIB_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>

struct Library_corlib_native_System_Object
{
    NANOCLR_NATIVE_DECLARE(Equals___BOOLEAN__OBJECT);
    NANOCLR_NATIVE_DECLARE(GetHashCode___I4);
    NANOCLR_NATIVE_DECLARE(GetType___SystemType);
    NANOCLR_NATIVE_DECLARE(MemberwiseClone___OBJECT);
    NANOCLR_NATIVE_DECLARE(ReferenceEquals___STATIC__BOOLEAN__OBJECT__OBJECT);

    //--//

};

// // THIS CLASS IS BUILT INTO mscorlib and we need to offset static fields accordingly
// struct Library_corlib_native__<PrivateImplementationDetails>
// {
//     static const int FIELD_STATIC__DD3AEFEADB1CD615F3017763F1568179FEE640B0 = 0;
//     static const int FIELD_STATIC__E92B39D8233061927D9ACDE54665E68E7535635A = 1;


//     //--//

// };
//

struct Library_corlib_native_System_ValueType
{
    NANOCLR_NATIVE_DECLARE(Equals___BOOLEAN__OBJECT);

    //--//

};

struct Library_corlib_native_System_Collections_Hashtable
{
    static const int FIELD___buckets = 1;
    static const int FIELD___numberOfBuckets = 2;
    static const int FIELD___count = 3;
    static const int FIELD___loadFactor = 4;
    static const int FIELD___maxLoadFactor = 5;
    static const int FIELD___growthFactor = 6;


    //--//

};

struct Library_corlib_native_System_Collections_Hashtable__Entry
{
    static const int FIELD__key = 1;
    static const int FIELD__value = 2;
    static const int FIELD__next = 3;


    //--//

};

struct Library_corlib_native_System_Collections_Hashtable__HashtableEnumerator
{
    static const int FIELD__ht = 1;
    static const int FIELD__temp = 2;
    static const int FIELD__index = 3;
    static const int FIELD__returnType = 4;


    //--//

};

struct Library_corlib_native_System_Exception
{
    static const int FIELD___message = 1;
    static const int FIELD___innerException = 2;
    static const int FIELD___stackTrace = 3;
    static const int FIELD__HResult = 4;

    NANOCLR_NATIVE_DECLARE(get_StackTrace___STRING);

    //--//

    struct StackTrace
    {
        CLR_RT_MethodDef_Index m_md;
        CLR_UINT32             m_IP;
    };

    static HRESULT CreateInstance( CLR_RT_HeapBlock& ref,                                    HRESULT hr, CLR_RT_StackFrame* stack );
    static HRESULT CreateInstance( CLR_RT_HeapBlock& ref, const CLR_RT_TypeDef_Index& cls  , HRESULT hr, CLR_RT_StackFrame* stack );
    static HRESULT SetStackTrace ( CLR_RT_HeapBlock& ref,                                                CLR_RT_StackFrame* stack );

    static CLR_RT_HeapBlock* GetTarget    ( CLR_RT_HeapBlock& ref                    );
    static StackTrace*       GetStackTrace( CLR_RT_HeapBlock* obj, CLR_UINT32& depth );

    //--//

    static const char*     GetMessage( CLR_RT_HeapBlock* obj ) { return obj[ Library_corlib_native_System_Exception::FIELD___message  ].RecoverString()  ; }
    static CLR_UINT32 GetHResult( CLR_RT_HeapBlock* obj ) { return obj[ Library_corlib_native_System_Exception::FIELD__HResult ].NumericByRef().u4; }
};

struct Library_corlib_native_System_Collections_Hashtable__KeyCollection
{
    static const int FIELD__ht = 1;


    //--//

};

struct Library_corlib_native_System_AppDomain
{
    static const int FIELD___appDomain = 1;
    static const int FIELD___friendlyName = 2;

    NANOCLR_NATIVE_DECLARE(GetAssemblies___SZARRAY_SystemReflectionAssembly);
    NANOCLR_NATIVE_DECLARE(LoadInternal___SystemReflectionAssembly__STRING__BOOLEAN__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(CreateDomain___STATIC__SystemAppDomain__STRING);
    NANOCLR_NATIVE_DECLARE(Unload___STATIC__VOID__SystemAppDomain);

    //--//
#if defined(NANOCLR_APPDOMAINS)
    static HRESULT GetAppDomain( CLR_RT_HeapBlock& ref, CLR_RT_AppDomain*& appDomain, CLR_RT_AppDomain*& appDomainSav, bool fCheckForUnloadingAppDomain );    
#endif
};

struct Library_corlib_native_System_ArgumentException
{
    static const int FIELD___paramName = 5;


    //--//

};

struct Library_corlib_native_System_Array
{
    NANOCLR_NATIVE_DECLARE(System_Collections_IList_get_Item___OBJECT__I4);
    NANOCLR_NATIVE_DECLARE(System_Collections_IList_set_Item___VOID__I4__OBJECT);
    NANOCLR_NATIVE_DECLARE(get_Length___I4);
    NANOCLR_NATIVE_DECLARE(CreateInstance___STATIC__SystemArray__SystemType__I4);
    NANOCLR_NATIVE_DECLARE(Copy___STATIC__VOID__SystemArray__I4__SystemArray__I4__I4);
    NANOCLR_NATIVE_DECLARE(Clear___STATIC__VOID__SystemArray__I4__I4);
    NANOCLR_NATIVE_DECLARE(TrySzIndexOf___STATIC__BOOLEAN__SystemArray__I4__I4__OBJECT__BYREF_I4);

    //--//

    static HRESULT Clear       ( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock& arg   , int index   ,                                         int length                                            );
    static HRESULT Copy        ( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock& argSrc, int indexSrc, CLR_RT_HeapBlock& argDst, int indexDst, int length                                            );
    static HRESULT TrySzIndexOf( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock& argSrc,               CLR_RT_HeapBlock& match , int start   , int stop  , bool fForward, CLR_RT_HeapBlock& retValue );
};

struct Library_corlib_native_System_Delegate
{
    NANOCLR_NATIVE_DECLARE(Equals___BOOLEAN__OBJECT);
    NANOCLR_NATIVE_DECLARE(get_Method___SystemReflectionMethodInfo);
    NANOCLR_NATIVE_DECLARE(get_Target___OBJECT);
    NANOCLR_NATIVE_DECLARE(Combine___STATIC__SystemDelegate__SystemDelegate__SystemDelegate);
    NANOCLR_NATIVE_DECLARE(Remove___STATIC__SystemDelegate__SystemDelegate__SystemDelegate);
    NANOCLR_NATIVE_DECLARE(op_Equality___STATIC__BOOLEAN__SystemDelegate__SystemDelegate);
    NANOCLR_NATIVE_DECLARE(op_Inequality___STATIC__BOOLEAN__SystemDelegate__SystemDelegate);

    //--//

    static CLR_RT_HeapBlock_Delegate* GetLastDelegate( CLR_RT_HeapBlock_Delegate* dlg );
};

struct Library_corlib_native_System_MulticastDelegate
{
    NANOCLR_NATIVE_DECLARE(op_Equality___STATIC__BOOLEAN__SystemMulticastDelegate__SystemMulticastDelegate);
    NANOCLR_NATIVE_DECLARE(op_Inequality___STATIC__BOOLEAN__SystemMulticastDelegate__SystemMulticastDelegate);

    //--//

};

struct Library_corlib_native_System_BitConverter
{
    NANOCLR_NATIVE_DECLARE(get_IsLittleEndian___STATIC__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(DoubleToInt64Bits___STATIC__I8__R8);
    NANOCLR_NATIVE_DECLARE(GetBytes___STATIC__SZARRAY_U1__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(GetBytes___STATIC__SZARRAY_U1__CHAR);
    NANOCLR_NATIVE_DECLARE(GetBytes___STATIC__SZARRAY_U1__R8);
    NANOCLR_NATIVE_DECLARE(GetBytes___STATIC__SZARRAY_U1__R4);
    NANOCLR_NATIVE_DECLARE(GetBytes___STATIC__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(GetBytes___STATIC__SZARRAY_U1__I8);
    NANOCLR_NATIVE_DECLARE(GetBytes___STATIC__SZARRAY_U1__I2);
    NANOCLR_NATIVE_DECLARE(GetBytes___STATIC__SZARRAY_U1__U4);
    NANOCLR_NATIVE_DECLARE(GetBytes___STATIC__SZARRAY_U1__U8);
    NANOCLR_NATIVE_DECLARE(GetBytes___STATIC__SZARRAY_U1__U2);
    NANOCLR_NATIVE_DECLARE(Int64BitsToDouble___STATIC__R8__I8);
    NANOCLR_NATIVE_DECLARE(ToBoolean___STATIC__BOOLEAN__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(ToChar___STATIC__CHAR__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(ToDouble___STATIC__R8__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(ToInt16___STATIC__I2__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(ToInt32___STATIC__I4__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(ToInt64___STATIC__I8__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(ToSingle___STATIC__R4__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(ToString___STATIC__STRING__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(ToString___STATIC__STRING__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(ToString___STATIC__STRING__SZARRAY_U1__I4__I4);
    NANOCLR_NATIVE_DECLARE(ToUInt16___STATIC__U2__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(ToUInt32___STATIC__U4__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(ToUInt64___STATIC__U8__SZARRAY_U1__I4);

    //--//

};

struct Library_corlib_native_System_Boolean
{
    static const int FIELD_STATIC__FalseString = 0;
    static const int FIELD_STATIC__TrueString = 1;

    static const int FIELD___value = 1;


    //--//

};

struct Library_corlib_native_System_Byte
{
    static const int FIELD___value = 1;


    //--//

};

struct Library_corlib_native_System_Char
{
    static const int FIELD___value = 1;


    //--//

};

struct Library_corlib_native_System_Collections_ArrayList
{
    static const int FIELD___items = 1;
    static const int FIELD___size = 2;

    NANOCLR_NATIVE_DECLARE(get_Item___OBJECT__I4);
    NANOCLR_NATIVE_DECLARE(set_Item___VOID__I4__OBJECT);
    NANOCLR_NATIVE_DECLARE(Add___I4__OBJECT);
    NANOCLR_NATIVE_DECLARE(Clear___VOID);
    NANOCLR_NATIVE_DECLARE(Insert___VOID__I4__OBJECT);
    NANOCLR_NATIVE_DECLARE(RemoveAt___VOID__I4);
    NANOCLR_NATIVE_DECLARE(SetCapacity___VOID__I4);

    //--//

};

struct Library_corlib_native_System_Collections_DictionaryEntry
{
    static const int FIELD__Key = 1;
    static const int FIELD__Value = 2;


    //--//

};

struct Library_corlib_native_System_Collections_Queue
{
    static const int FIELD___array = 1;
    static const int FIELD___head = 2;
    static const int FIELD___tail = 3;
    static const int FIELD___size = 4;

    NANOCLR_NATIVE_DECLARE(Clear___VOID);
    NANOCLR_NATIVE_DECLARE(CopyTo___VOID__SystemArray__I4);
    NANOCLR_NATIVE_DECLARE(Enqueue___VOID__OBJECT);
    NANOCLR_NATIVE_DECLARE(Dequeue___OBJECT);
    NANOCLR_NATIVE_DECLARE(Peek___OBJECT);

    //--//

};

struct Library_corlib_native_System_Collections_Stack
{
    static const int FIELD___array = 1;
    static const int FIELD___size = 2;

    NANOCLR_NATIVE_DECLARE(Clear___VOID);
    NANOCLR_NATIVE_DECLARE(Peek___OBJECT);
    NANOCLR_NATIVE_DECLARE(Pop___OBJECT);
    NANOCLR_NATIVE_DECLARE(Push___VOID__OBJECT);

    //--//

};

struct Library_corlib_native_System_Console
{
    NANOCLR_NATIVE_DECLARE(OutNative___STATIC__VOID__STRING__BOOLEAN);

    //--//

};

struct Library_corlib_native_System_Convert
{
    NANOCLR_NATIVE_DECLARE(NativeToInt64___STATIC__I8__STRING__BOOLEAN__I8__I8__I4);
    NANOCLR_NATIVE_DECLARE(NativeToDouble___STATIC__R8__STRING);
    NANOCLR_NATIVE_DECLARE(ToBase64String___STATIC__STRING__SZARRAY_U1__I4__I4__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(FromBase64CharArray___STATIC__SZARRAY_U1__SZARRAY_CHAR__I4);

    //--//

    static double GetDoubleFractionalPart(char* str, int length);
    static int64_t GetIntegerPart(char* str, int length);
    static int64_t GetIntegerFromHexString(char* str);
};

struct Library_corlib_native_System_DateTime
{
    static const int FIELD_STATIC__MinValue = 2;
    static const int FIELD_STATIC__MaxValue = 3;

    static const int FIELD___ticks = 1;

    NANOCLR_NATIVE_DECLARE(_ctor___VOID__I4__I4__I4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(get_Day___I4);
    NANOCLR_NATIVE_DECLARE(get_DayOfWeek___SystemDayOfWeek);
    NANOCLR_NATIVE_DECLARE(get_DayOfYear___I4);
    NANOCLR_NATIVE_DECLARE(get_Hour___I4);
    NANOCLR_NATIVE_DECLARE(get_Millisecond___I4);
    NANOCLR_NATIVE_DECLARE(get_Minute___I4);
    NANOCLR_NATIVE_DECLARE(get_Month___I4);
    NANOCLR_NATIVE_DECLARE(get_Second___I4);
    NANOCLR_NATIVE_DECLARE(get_Year___I4);
    NANOCLR_NATIVE_DECLARE(DaysInMonth___STATIC__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(get_UtcNow___STATIC__SystemDateTime);
    NANOCLR_NATIVE_DECLARE(get_Today___STATIC__SystemDateTime);

    //--//

    static CLR_INT64* NewObject  ( CLR_RT_StackFrame& stack );
    static CLR_INT64* GetValuePtr( CLR_RT_StackFrame& stack );
    static CLR_INT64* GetValuePtr( CLR_RT_HeapBlock&  ref   );

    static void Expand  ( CLR_RT_StackFrame& stack,       SYSTEMTIME& st );
    static void Compress( CLR_RT_StackFrame& stack, const SYSTEMTIME& st );
};

struct Library_corlib_native_System_Diagnostics_Debugger
{
    NANOCLR_NATIVE_DECLARE(get_IsAttached___STATIC__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(Break___STATIC__VOID);

    //--//

};

struct Library_corlib_native_System_Diagnostics_DebuggerBrowsableAttribute
{
    static const int FIELD___state = 1;


    //--//

};

struct Library_corlib_native_System_Double
{
    static const int FIELD___value = 1;

    NANOCLR_NATIVE_DECLARE(CompareTo___STATIC__I4__R8__R8);
    NANOCLR_NATIVE_DECLARE(IsInfinity___STATIC__BOOLEAN__R8);
    NANOCLR_NATIVE_DECLARE(IsNaN___STATIC__BOOLEAN__R8);
    NANOCLR_NATIVE_DECLARE(IsNegativeInfinity___STATIC__BOOLEAN__R8);
    NANOCLR_NATIVE_DECLARE(IsPositiveInfinity___STATIC__BOOLEAN__R8);

    //--//

};

struct Library_corlib_native_System_GC
{
    NANOCLR_NATIVE_DECLARE(AnyPendingFinalizers___STATIC__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(SuppressFinalize___STATIC__VOID__OBJECT);
    NANOCLR_NATIVE_DECLARE(ReRegisterForFinalize___STATIC__VOID__OBJECT);

    //--//

};

struct Library_corlib_native_System_Globalization_CultureInfo
{
    static const int FIELD___numInfo = 1;
    static const int FIELD___dateTimeInfo = 2;
    static const int FIELD___cultureInfoName = 3;
    static const int FIELD___name = 4;
    static const int FIELD___parent = 5;

    NANOCLR_NATIVE_DECLARE(get_CurrentUICultureInternal___STATIC__SystemGlobalizationCultureInfo);
    NANOCLR_NATIVE_DECLARE(set_CurrentUICultureInternal___STATIC__VOID__SystemGlobalizationCultureInfo);

    //--//

};

struct Library_corlib_native_System_Globalization_DateTimeFormat
{
    NANOCLR_NATIVE_DECLARE(FormatDigits___STATIC__STRING__I4__I4);

    //--//

};

struct Library_corlib_native_System_Globalization_DateTimeFormatInfo
{
    static const int FIELD___generalShortTimePattern = 1;
    static const int FIELD___generalLongTimePattern = 2;
    static const int FIELD___fullDateTimePattern = 3;
    static const int FIELD__CultureInfo = 4;


    //--//

};

struct Library_corlib_native_System_Globalization_NumberFormatInfo
{
    static const int FIELD___numberGroupSizes = 1;
    static const int FIELD___positiveSign = 2;
    static const int FIELD___negativeSign = 3;
    static const int FIELD___numberDecimalSeparator = 4;
    static const int FIELD___numberGroupSeparator = 5;
    static const int FIELD___cultureInfo = 6;


    //--//

};

struct Library_corlib_native_System_Guid
{
    static const int FIELD_STATIC__Empty = 4;

    static const int FIELD___a = 1;
    static const int FIELD___b = 2;
    static const int FIELD___c = 3;
    static const int FIELD___d = 4;
    static const int FIELD___e = 5;
    static const int FIELD___f = 6;
    static const int FIELD___g = 7;
    static const int FIELD___h = 8;
    static const int FIELD___i = 9;
    static const int FIELD___j = 10;
    static const int FIELD___k = 11;

    NANOCLR_NATIVE_DECLARE(GenerateNewGuid___STATIC__SZARRAY_U1);

    //--//

};

struct Library_corlib_native_System_Int16
{
    static const int FIELD___value = 1;


    //--//

};

struct Library_corlib_native_System_Int32
{
    static const int FIELD___value = 1;


    //--//

};

struct Library_corlib_native_System_Int64
{
    static const int FIELD___value = 1;


    //--//

};

struct Library_corlib_native_System_MathInternal
{
    NANOCLR_NATIVE_DECLARE(Abs___STATIC__I4__I4);
    NANOCLR_NATIVE_DECLARE(Min___STATIC__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(Max___STATIC__I4__I4__I4);

    //--//

};

struct Library_corlib_native_System_Number
{
    NANOCLR_NATIVE_DECLARE(FormatNative___STATIC__STRING__OBJECT__CHAR__I4);

    //--//

};

struct Library_corlib_native_System_Random
{
    static const int FIELD___random = 1;

    NANOCLR_NATIVE_DECLARE(Next___I4);
    NANOCLR_NATIVE_DECLARE(Next___I4__I4);
    NANOCLR_NATIVE_DECLARE(NextDouble___R8);
    NANOCLR_NATIVE_DECLARE(NextBytes___VOID__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__I4);

    //--//

    static HRESULT GetRandom( CLR_RT_StackFrame& stack, CLR_RT_Random*& rand, bool create = false );
};

struct Library_corlib_native_System_Reflection_Assembly
{
    NANOCLR_NATIVE_DECLARE(get_FullName___STRING);
    NANOCLR_NATIVE_DECLARE(GetType___SystemType__STRING);
    NANOCLR_NATIVE_DECLARE(GetTypes___SZARRAY_SystemType);
    NANOCLR_NATIVE_DECLARE(GetVersion___VOID__BYREF_I4__BYREF_I4__BYREF_I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(GetManifestResourceNames___SZARRAY_STRING);
    NANOCLR_NATIVE_DECLARE(GetExecutingAssembly___STATIC__SystemReflectionAssembly);
    NANOCLR_NATIVE_DECLARE(LoadInternal___STATIC__SystemReflectionAssembly__STRING__BOOLEAN__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(Load___STATIC__SystemReflectionAssembly__SZARRAY_U1);

    //--//

    static HRESULT GetTypeDescriptor( CLR_RT_HeapBlock& arg, CLR_RT_Assembly_Instance& inst );
};

struct Library_corlib_native_System_Reflection_AssemblyName
{
    static const int FIELD___assembly = 1;


    //--//

};

struct Library_corlib_native_System_Reflection_MethodBase
{
    NANOCLR_NATIVE_DECLARE(get_Name___STRING);
    NANOCLR_NATIVE_DECLARE(get_DeclaringType___SystemType);
    NANOCLR_NATIVE_DECLARE(get_IsPublic___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(get_IsStatic___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(get_IsFinal___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(get_IsVirtual___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(get_IsAbstract___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(Invoke___OBJECT__OBJECT__SZARRAY_OBJECT);

    //--//

    static HRESULT GetMethodDescriptor( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock& arg, CLR_RT_MethodDef_Instance& inst );

    static HRESULT CheckFlags( CLR_RT_StackFrame& stack, CLR_UINT32 mask, CLR_UINT32 flag );
};

struct Library_corlib_native_System_Reflection_ConstructorInfo
{
    NANOCLR_NATIVE_DECLARE(GetCustomAttributes___SZARRAY_OBJECT__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(Invoke___OBJECT__SZARRAY_OBJECT);

    //--//

};

struct Library_corlib_native_System_Reflection_FieldInfo
{
    NANOCLR_NATIVE_DECLARE(SetValue___VOID__OBJECT__OBJECT);
    NANOCLR_NATIVE_DECLARE(GetCustomAttributes___SZARRAY_OBJECT__BOOLEAN);

    //--//

    static HRESULT Initialize( CLR_RT_StackFrame& stack, CLR_RT_FieldDef_Instance& instFD, CLR_RT_TypeDef_Instance& instTD, CLR_RT_HeapBlock*& obj );
};

struct Library_corlib_native_System_Reflection_PropertyInfo
{
    NANOCLR_NATIVE_DECLARE(GetValue___OBJECT__OBJECT__SZARRAY_OBJECT);
    NANOCLR_NATIVE_DECLARE(SetValue___VOID__OBJECT__OBJECT__SZARRAY_OBJECT);

    //--//

};

struct Library_corlib_native_System_Reflection_RuntimeFieldInfo
{
    NANOCLR_NATIVE_DECLARE(get_Name___STRING);
    NANOCLR_NATIVE_DECLARE(get_DeclaringType___SystemType);
    NANOCLR_NATIVE_DECLARE(get_FieldType___SystemType);
    NANOCLR_NATIVE_DECLARE(GetValue___OBJECT__OBJECT);

    //--//

    static bool GetFieldDescriptor( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock& arg, CLR_RT_FieldDef_Instance& inst );
};

struct Library_corlib_native_System_Reflection_RuntimeMethodInfo
{
    NANOCLR_NATIVE_DECLARE(get_ReturnType___SystemType);
    NANOCLR_NATIVE_DECLARE(GetCustomAttributes___SZARRAY_OBJECT__BOOLEAN);

    //--//

};

struct Library_corlib_native_System_Resources_ResourceManager
{
    static const int FIELD___resourceFileId = 1;
    static const int FIELD___assembly = 2;
    static const int FIELD___baseAssembly = 3;
    static const int FIELD___baseName = 4;
    static const int FIELD___cultureName = 5;
    static const int FIELD___rmFallback = 6;

    NANOCLR_NATIVE_DECLARE(GetObjectInternal___OBJECT__I2);
    NANOCLR_NATIVE_DECLARE(GetObjectInternal___OBJECT__I2__I4__I4);
    NANOCLR_NATIVE_DECLARE(FindResource___STATIC__I4__STRING__SystemReflectionAssembly);
    NANOCLR_NATIVE_DECLARE(GetObject___STATIC__OBJECT__SystemResourcesResourceManager__SystemEnum);

    //--//

    NANOCLR_NATIVE_DECLARE(GetObject___STATIC__OBJECT__SystemResourcesResourceManager__SystemEnum__I4__I4);    

};

struct Library_corlib_native_System_Runtime_CompilerServices_AccessedThroughPropertyAttribute
{
    static const int FIELD___propertyName = 1;


    //--//

};

struct Library_corlib_native_System_Runtime_CompilerServices_RuntimeHelpers
{
    NANOCLR_NATIVE_DECLARE(InitializeArray___STATIC__VOID__SystemArray__SystemRuntimeFieldHandle);
    NANOCLR_NATIVE_DECLARE(GetObjectValue___STATIC__OBJECT__OBJECT);
    NANOCLR_NATIVE_DECLARE(RunClassConstructor___STATIC__VOID__SystemRuntimeTypeHandle);
    NANOCLR_NATIVE_DECLARE(get_OffsetToStringData___STATIC__I4);

    //--//

};

struct Library_corlib_native_System_Runtime_Remoting_RemotingServices
{
    NANOCLR_NATIVE_DECLARE(IsTransparentProxy___STATIC__BOOLEAN__OBJECT);

    //--//

};

struct Library_corlib_native_System_Runtime_Versioning_TargetFrameworkAttribute
{
    static const int FIELD___frameworkName = 1;
    static const int FIELD___frameworkDisplayName = 2;


    //--//

};

struct Library_corlib_native_System_Type
{
    NANOCLR_NATIVE_DECLARE(get_DeclaringType___SystemType);
    NANOCLR_NATIVE_DECLARE(GetMethod___SystemReflectionMethodInfo__STRING__SystemReflectionBindingFlags);
    NANOCLR_NATIVE_DECLARE(IsInstanceOfType___BOOLEAN__OBJECT);
    NANOCLR_NATIVE_DECLARE(InvokeMember___OBJECT__STRING__SystemReflectionBindingFlags__SystemReflectionBinder__OBJECT__SZARRAY_OBJECT);
    NANOCLR_NATIVE_DECLARE(GetConstructor___SystemReflectionConstructorInfo__SZARRAY_SystemType);
    NANOCLR_NATIVE_DECLARE(GetMethod___SystemReflectionMethodInfo__STRING__SZARRAY_SystemType);
    NANOCLR_NATIVE_DECLARE(GetMethod___SystemReflectionMethodInfo__STRING);
    NANOCLR_NATIVE_DECLARE(get_IsNotPublic___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(get_IsPublic___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(get_IsClass___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(get_IsInterface___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(get_IsValueType___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(get_IsAbstract___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(get_IsEnum___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(get_IsSerializable___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(get_IsArray___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(GetTypeInternal___STATIC__SystemType__STRING__STRING__BOOLEAN__SZARRAY_I4);
    NANOCLR_NATIVE_DECLARE(GetTypeFromHandle___STATIC__SystemType__SystemRuntimeTypeHandle);

    //--//

    //
    static const int c_BindingFlags_Default              = 0x00000000; // a place holder for no flag specifed

    static const int c_BindingFlags_IgnoreCase           = 0x00000001; // Ignore the case of Names while searching
    static const int c_BindingFlags_DeclaredOnly         = 0x00000002; // Only look at the members declared on the Type
    static const int c_BindingFlags_Instance             = 0x00000004; // Include Instance members in search
    static const int c_BindingFlags_Static               = 0x00000008; // Include Static members in search
    static const int c_BindingFlags_Public               = 0x00000010; // Include Public members in search
    static const int c_BindingFlags_NonPublic            = 0x00000020; // Include Non-Public members in search
    static const int c_BindingFlags_FlattenHierarchy     = 0x00000040; // Rollup the statics into the class.

    static const int c_BindingFlags_InvokeMethod         = 0x00000100;
    static const int c_BindingFlags_CreateInstance       = 0x00000200;
    static const int c_BindingFlags_GetField             = 0x00000400;
    static const int c_BindingFlags_SetField             = 0x00000800;
    static const int c_BindingFlags_GetProperty          = 0x00001000;
    static const int c_BindingFlags_SetProperty          = 0x00002000;

    static const int c_BindingFlags_PutDispProperty      = 0x00004000;
    static const int c_BindingFlags_PutRefDispProperty   = 0x00008000;

    static const int c_BindingFlags_ExactBinding         = 0x00010000; // Bind with Exact Type matching, No Change type
    static const int c_BindingFlags_SuppressChangeType   = 0x00020000;

    static const int c_BindingFlags_OptionalParamBinding = 0x00040000;
    static const int c_BindingFlags_IgnoreReturn         = 0x01000000;  // This is used in COM Interop

    //--//

    static const int c_BindingFlags_DefaultLookup = c_BindingFlags_Instance | c_BindingFlags_Static | c_BindingFlags_Public;

    //--//

    static HRESULT CheckFlags( CLR_RT_StackFrame& stack, CLR_UINT32 mask, CLR_UINT32 flag );

    static HRESULT GetFields ( CLR_RT_StackFrame& stack, const char* szText, CLR_UINT32 attr                                        , bool fAllMatches );
    static HRESULT GetMethods( CLR_RT_StackFrame& stack, const char* szText, CLR_UINT32 attr, CLR_RT_HeapBlock* pParams, int iParams, bool fAllMatches );
};

struct Library_corlib_native_System_RuntimeType
{
    NANOCLR_NATIVE_DECLARE(get_Assembly___SystemReflectionAssembly);
    NANOCLR_NATIVE_DECLARE(get_Name___STRING);
    NANOCLR_NATIVE_DECLARE(get_FullName___STRING);
    NANOCLR_NATIVE_DECLARE(get_BaseType___SystemType);
    NANOCLR_NATIVE_DECLARE(GetMethods___SZARRAY_SystemReflectionMethodInfo__SystemReflectionBindingFlags);
    NANOCLR_NATIVE_DECLARE(GetField___SystemReflectionFieldInfo__STRING__SystemReflectionBindingFlags);
    NANOCLR_NATIVE_DECLARE(GetFields___SZARRAY_SystemReflectionFieldInfo__SystemReflectionBindingFlags);
    NANOCLR_NATIVE_DECLARE(GetInterfaces___SZARRAY_SystemType);
    NANOCLR_NATIVE_DECLARE(GetElementType___SystemType);
    NANOCLR_NATIVE_DECLARE(GetCustomAttributes___SZARRAY_OBJECT__BOOLEAN);

    //--//

    static HRESULT GetTypeDescriptor( CLR_RT_HeapBlock& arg, CLR_RT_TypeDef_Instance& inst, CLR_UINT32* levels );
    static HRESULT GetTypeDescriptor( CLR_RT_HeapBlock& arg, CLR_RT_TypeDef_Instance& inst );
    static HRESULT GetName( CLR_RT_HeapBlock& arg, bool fFullName, CLR_RT_HeapBlock& res );
};

struct Library_corlib_native_System_SByte
{
    static const int FIELD___value = 1;


    //--//

};

struct Library_corlib_native_System_Single
{
    static const int FIELD___value = 1;


    //--//

};

struct Library_corlib_native_System_String
{
    static const int FIELD_STATIC__Empty = 5;

    NANOCLR_NATIVE_DECLARE(CompareTo___I4__OBJECT);
    NANOCLR_NATIVE_DECLARE(get_Chars___CHAR__I4);
    NANOCLR_NATIVE_DECLARE(ToCharArray___SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(ToCharArray___SZARRAY_CHAR__I4__I4);
    NANOCLR_NATIVE_DECLARE(get_Length___I4);
    NANOCLR_NATIVE_DECLARE(Split___SZARRAY_STRING__SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(Split___SZARRAY_STRING__SZARRAY_CHAR__I4);
    NANOCLR_NATIVE_DECLARE(Substring___STRING__I4);
    NANOCLR_NATIVE_DECLARE(Substring___STRING__I4__I4);
    NANOCLR_NATIVE_DECLARE(Trim___STRING__SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(TrimStart___STRING__SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(TrimEnd___STRING__SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__SZARRAY_CHAR__I4__I4);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__CHAR__I4);
    NANOCLR_NATIVE_DECLARE(CompareTo___I4__STRING);
    NANOCLR_NATIVE_DECLARE(IndexOf___I4__CHAR);
    NANOCLR_NATIVE_DECLARE(IndexOf___I4__CHAR__I4);
    NANOCLR_NATIVE_DECLARE(IndexOf___I4__CHAR__I4__I4);
    NANOCLR_NATIVE_DECLARE(IndexOfAny___I4__SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(IndexOfAny___I4__SZARRAY_CHAR__I4);
    NANOCLR_NATIVE_DECLARE(IndexOfAny___I4__SZARRAY_CHAR__I4__I4);
    NANOCLR_NATIVE_DECLARE(IndexOf___I4__STRING);
    NANOCLR_NATIVE_DECLARE(IndexOf___I4__STRING__I4);
    NANOCLR_NATIVE_DECLARE(IndexOf___I4__STRING__I4__I4);
    NANOCLR_NATIVE_DECLARE(LastIndexOf___I4__CHAR);
    NANOCLR_NATIVE_DECLARE(LastIndexOf___I4__CHAR__I4);
    NANOCLR_NATIVE_DECLARE(LastIndexOf___I4__CHAR__I4__I4);
    NANOCLR_NATIVE_DECLARE(LastIndexOfAny___I4__SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(LastIndexOfAny___I4__SZARRAY_CHAR__I4);
    NANOCLR_NATIVE_DECLARE(LastIndexOfAny___I4__SZARRAY_CHAR__I4__I4);
    NANOCLR_NATIVE_DECLARE(LastIndexOf___I4__STRING);
    NANOCLR_NATIVE_DECLARE(LastIndexOf___I4__STRING__I4);
    NANOCLR_NATIVE_DECLARE(LastIndexOf___I4__STRING__I4__I4);
    NANOCLR_NATIVE_DECLARE(ToLower___STRING);
    NANOCLR_NATIVE_DECLARE(ToUpper___STRING);
    NANOCLR_NATIVE_DECLARE(Trim___STRING);
    NANOCLR_NATIVE_DECLARE(Equals___STATIC__BOOLEAN__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(op_Equality___STATIC__BOOLEAN__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(op_Inequality___STATIC__BOOLEAN__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(Compare___STATIC__I4__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(Concat___STATIC__STRING__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(Concat___STATIC__STRING__STRING__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(Concat___STATIC__STRING__STRING__STRING__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(Concat___STATIC__STRING__SZARRAY_STRING);

    //--//

    static const int c_IndexOf__SingleChar    = 0x00000001;
    static const int c_IndexOf__MultipleChars = 0x00000002;
    static const int c_IndexOf__String        = 0x00000004;
    static const int c_IndexOf__StartIndex    = 0x00000010;
    static const int c_IndexOf__Count         = 0x00000020;
    static const int c_IndexOf__Last          = 0x00000040;
    static const int c_IndexOf__String_Last   = 0x00000044;

    static HRESULT FromCharArray( CLR_RT_StackFrame& stack, int startIndex, int count );
    static HRESULT ToCharArray  ( CLR_RT_StackFrame& stack, int startIndex, int count );
    static HRESULT IndexOf      ( CLR_RT_StackFrame& stack, int mode                  );
    static HRESULT ChangeCase   ( CLR_RT_StackFrame& stack, bool fToUpper             );
    static HRESULT Substring    ( CLR_RT_StackFrame& stack, int startIndex, int count );


    static HRESULT Trim( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock_Array* arrayTrimChars, bool fStart, bool fEnd );

    static HRESULT Split( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock& chars, int maxStrings );

    //--//

    static HRESULT Concat( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock* array, int num );

    static HRESULT ConvertToCharArray( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock& ref, CLR_RT_HeapBlock_Array*& array, int startIndex, int length );
    static HRESULT ConvertToCharArray( const char* szText           , CLR_RT_HeapBlock& ref, CLR_RT_HeapBlock_Array*& array, int startIndex, int length );
};

struct Library_corlib_native_System_Text_StringBuilder
{
    static const int FIELD___maxCapacity = 1;
    static const int FIELD___chunkChars = 2;
    static const int FIELD___chunkLength = 3;
    static const int FIELD___chunkPrevious = 4;
    static const int FIELD___chunkOffset = 5;


    //--//

};

struct Library_corlib_native_System_Text_UTF8Decoder
{
    NANOCLR_NATIVE_DECLARE(Convert___VOID__SZARRAY_U1__I4__I4__SZARRAY_CHAR__I4__I4__BOOLEAN__BYREF_I4__BYREF_I4__BYREF_BOOLEAN);

    //--//

};

struct Library_corlib_native_System_Text_UTF8Encoding
{
    NANOCLR_NATIVE_DECLARE(GetBytes___SZARRAY_U1__STRING);
    NANOCLR_NATIVE_DECLARE(GetBytes___I4__STRING__I4__I4__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(GetChars___SZARRAY_CHAR__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(GetChars___SZARRAY_CHAR__SZARRAY_U1__I4__I4);

    //--//

    static HRESULT Helper__GetChars(CLR_RT_StackFrame& stack, bool fIndexed);

};

struct Library_corlib_native_System_Threading_WaitHandle
{
    NANOCLR_NATIVE_DECLARE(WaitOne___BOOLEAN__I4__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(WaitMultiple___STATIC__I4__SZARRAY_SystemThreadingWaitHandle__I4__BOOLEAN__BOOLEAN);

    //--//

    static void Set  ( CLR_RT_StackFrame& stack );
    static void Reset( CLR_RT_StackFrame& stack );

    static HRESULT Wait( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock& blkTimeout, CLR_RT_HeapBlock& blkExitContext, CLR_RT_HeapBlock* objects, int cObjects, bool fWaitAll );
};

struct Library_corlib_native_System_Threading_AutoResetEvent
{
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(Reset___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(Set___BOOLEAN);

    //--//

};

struct Library_corlib_native_System_Threading_Interlocked
{
    NANOCLR_NATIVE_DECLARE(Increment___STATIC__I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(Decrement___STATIC__I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(Exchange___STATIC__I4__BYREF_I4__I4);
    NANOCLR_NATIVE_DECLARE(CompareExchange___STATIC__I4__BYREF_I4__I4__I4);

    //--//

};

struct Library_corlib_native_System_Threading_ManualResetEvent
{
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(Reset___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(Set___BOOLEAN);

    //--//

};

struct Library_corlib_native_System_Threading_Monitor
{
    NANOCLR_NATIVE_DECLARE(Enter___STATIC__VOID__OBJECT);
    NANOCLR_NATIVE_DECLARE(Exit___STATIC__VOID__OBJECT);

    //--//

};

struct Library_corlib_native_System_Threading_Thread
{
    static const int FIELD___delegate = 1;
    static const int FIELD___priority = 2;
    static const int FIELD___thread = 3;
    static const int FIELD___appDomain = 4;
    static const int FIELD___id = 5;

    NANOCLR_NATIVE_DECLARE(_ctor___VOID__SystemThreadingThreadStart);
    NANOCLR_NATIVE_DECLARE(Start___VOID);
    NANOCLR_NATIVE_DECLARE(Abort___VOID);
    NANOCLR_NATIVE_DECLARE(Suspend___VOID);
    NANOCLR_NATIVE_DECLARE(Resume___VOID);
    NANOCLR_NATIVE_DECLARE(get_Priority___SystemThreadingThreadPriority);
    NANOCLR_NATIVE_DECLARE(set_Priority___VOID__SystemThreadingThreadPriority);
    NANOCLR_NATIVE_DECLARE(get_ManagedThreadId___I4);
    NANOCLR_NATIVE_DECLARE(get_IsAlive___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(Join___VOID);
    NANOCLR_NATIVE_DECLARE(Join___BOOLEAN__I4);
    NANOCLR_NATIVE_DECLARE(Join___BOOLEAN__SystemTimeSpan);
    NANOCLR_NATIVE_DECLARE(get_ThreadState___SystemThreadingThreadState);
    NANOCLR_NATIVE_DECLARE(Sleep___STATIC__VOID__I4);
    NANOCLR_NATIVE_DECLARE(get_CurrentThread___STATIC__SystemThreadingThread);
    NANOCLR_NATIVE_DECLARE(GetDomain___STATIC__SystemAppDomain);

    //--//

    static CLR_RT_ObjectToEvent_Source* GetThreadReference  ( CLR_RT_StackFrame& stack                                                               );
    static void                         ResetThreadReference( CLR_RT_StackFrame& stack                                                               );
    static HRESULT                      SetThread           ( CLR_RT_StackFrame& stack, CLR_RT_Thread*  th                                           );
    static HRESULT                      GetThread           ( CLR_RT_StackFrame& stack, CLR_RT_Thread*& th, bool mustBeStarted, bool noSystemThreads );

    static HRESULT Join( CLR_RT_StackFrame& stack, const CLR_INT64& timeExpire );
};

struct Library_corlib_native_System_Threading_Timeout
{
    static const int FIELD_STATIC__InfiniteTimeSpan = 6;


    //--//

};

struct Library_corlib_native_System_Threading_Timer
{
    static const int FIELD___timer = 1;
    static const int FIELD___state = 2;
    static const int FIELD___callback = 3;

    NANOCLR_NATIVE_DECLARE(Dispose___VOID);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__SystemThreadingTimerCallback__OBJECT__I4__I4);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__SystemThreadingTimerCallback__OBJECT__SystemTimeSpan__SystemTimeSpan);
    NANOCLR_NATIVE_DECLARE(Change___BOOLEAN__I4__I4);
    NANOCLR_NATIVE_DECLARE(Change___BOOLEAN__SystemTimeSpan__SystemTimeSpan);

    //--//

    static HRESULT SetValues    ( CLR_RT_StackFrame& stack, CLR_UINT32 flags );
    static bool    CheckDisposed( CLR_RT_StackFrame& stack                   );
};

struct Library_corlib_native_System_TimeSpan
{
    static const int FIELD_STATIC__Zero = 7;
    static const int FIELD_STATIC__MaxValue = 8;
    static const int FIELD_STATIC__MinValue = 9;

    static const int FIELD___ticks = 1;

    NANOCLR_NATIVE_DECLARE(Equals___BOOLEAN__OBJECT);
    NANOCLR_NATIVE_DECLARE(ToString___STRING);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__I4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(CompareTo___I4__OBJECT);
    NANOCLR_NATIVE_DECLARE(Compare___STATIC__I4__SystemTimeSpan__SystemTimeSpan);
    NANOCLR_NATIVE_DECLARE(Equals___STATIC__BOOLEAN__SystemTimeSpan__SystemTimeSpan);

    //--//

    static CLR_INT64* NewObject  ( CLR_RT_StackFrame& stack );
    static CLR_INT64* GetValuePtr( CLR_RT_StackFrame& stack );
    static CLR_INT64* GetValuePtr( CLR_RT_HeapBlock&  ref   );
    
    static void ConstructTimeSpan( CLR_INT64* val, CLR_INT32 days, CLR_INT32 hours, CLR_INT32 minutes, CLR_INT32 seconds, CLR_INT32 ms );
};

struct Library_corlib_native_System_UInt16
{
    static const int FIELD___value = 1;


    //--//

};

struct Library_corlib_native_System_UInt32
{
    static const int FIELD___value = 1;


    //--//

};

struct Library_corlib_native_System_UInt64
{
    static const int FIELD___value = 1;


    //--//

};

struct Library_corlib_native_System_Version
{
    static const int FIELD___Major = 1;
    static const int FIELD___Minor = 2;
    static const int FIELD___Build = 3;
    static const int FIELD___Revision = 4;


    //--//

};

struct Library_corlib_native_System_WeakReference
{
    NANOCLR_NATIVE_DECLARE(get_IsAlive___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(get_Target___OBJECT);
    NANOCLR_NATIVE_DECLARE(set_Target___VOID__OBJECT);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__OBJECT);

    //--//

};

struct Library_corlib_native_System_Array__SzArrayEnumerator
{
    static const int FIELD___array = 1;
    static const int FIELD___index = 2;
    static const int FIELD___endIndex = 3;
    static const int FIELD___startIndex = 4;
    static const int FIELD___arrayLength = 5;


    //--//

};

struct Library_corlib_native_System_Collections_Hashtable__ValueCollection
{
    static const int FIELD__ht = 1;


    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_mscorlib;

#endif  //_CORLIB_NATIVE_H_
