//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "CorLib.h"

typedef Library_corlib_native_System_Reflection_ParameterInfo ParameterInfo;

HRESULT Library_corlib_native_System_Reflection_MethodBase::get_Name___STRING(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_MethodDef_Instance md;
    CLR_RT_HeapBlock *hbMeth = stack.This();

    NANOCLR_CHECK_HRESULT(GetMethodDescriptor(stack, *hbMeth, md));

    NANOCLR_SET_AND_LEAVE(CLR_RT_HeapBlock_String::CreateInstance(stack.PushValue(), md.target->name, md.assembly));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_MethodBase::get_DeclaringType___SystemType(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_MethodDef_Instance md;
    CLR_RT_TypeDef_Instance cls{};
    CLR_RT_HeapBlock *hbMeth = stack.This();

    NANOCLR_CHECK_HRESULT(GetMethodDescriptor(stack, *hbMeth, md));

    if (cls.InitializeFromMethod(md) == false)
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);

    {
        CLR_RT_HeapBlock &top = stack.PushValue();
        CLR_RT_HeapBlock *hbObj;

        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.TypeStatic));

        hbObj = top.Dereference();
        hbObj->SetReflection(cls);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_MethodBase::get_IsPublic___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(
        CheckFlags(stack, CLR_RECORD_METHODDEF::MD_Scope_Mask, CLR_RECORD_METHODDEF::MD_Scope_Public));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_MethodBase::get_IsStatic___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(CheckFlags(stack, CLR_RECORD_METHODDEF::MD_Static, CLR_RECORD_METHODDEF::MD_Static));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_MethodBase::get_IsFinal___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(CheckFlags(stack, CLR_RECORD_METHODDEF::MD_Final, CLR_RECORD_METHODDEF::MD_Final));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_MethodBase::get_IsVirtual___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(CheckFlags(stack, CLR_RECORD_METHODDEF::MD_Virtual, CLR_RECORD_METHODDEF::MD_Virtual));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_MethodBase::get_IsAbstract___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(CheckFlags(stack, CLR_RECORD_METHODDEF::MD_Abstract, CLR_RECORD_METHODDEF::MD_Abstract));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_MethodBase::Invoke___OBJECT__OBJECT__SZARRAY_OBJECT(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock &obj = stack.Arg1();
    CLR_RT_MethodDef_Instance md;
    const CLR_RECORD_METHODDEF *mdR;
    CLR_RT_HeapBlock_Array *pArray = stack.Arg2().DereferenceArray();
    CLR_RT_HeapBlock *args = nullptr;
    int argumentsCount = 0;
    CLR_RT_HeapBlock *hbMeth = stack.This();

    NANOCLR_CHECK_HRESULT(GetMethodDescriptor(stack, *hbMeth, md));

    mdR = md.target;

    if (stack.m_customState == 0)
    {
        stack.m_customState = 1;

        if (pArray)
        {
            args = (CLR_RT_HeapBlock *)pArray->GetFirstElement();
            argumentsCount = pArray->m_numOfElements;
        }

        // Preserve null-target invocation for instance methods that do not access this.
        if ((mdR->flags & CLR_RECORD_METHODDEF::MD_Static) == 0 && obj.Dereference() != nullptr)
        {
            CLR_RT_TypeDef_Instance declaringType{};

            if (declaringType.InitializeFromMethod(md) == false ||
                CLR_RT_ExecutionEngine::IsInstanceOf(obj, declaringType) == false)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_CAST);
            }
        }

        NANOCLR_CHECK_HRESULT(CheckInvokeArguments(md, args, argumentsCount));

        NANOCLR_CHECK_HRESULT(stack.MakeCall(md, &obj, args, argumentsCount));
    }
    else
    {
        if (mdR->retValDataType != DATATYPE_VOID)
        {
            // Only small primitives need ChangeDataType
            bool needsTypeChange = (mdR->retValDataType < DATATYPE_I4);

            if (needsTypeChange)
            {
                stack.TopValue().ChangeDataType(mdR->retValDataType);
            }

            NANOCLR_CHECK_HRESULT(stack.TopValue().PerformBoxingIfNeeded());
        }
        else
        {
            stack.SetResult_Object(nullptr);
        }
    }

    NANOCLR_NOCLEANUP();
}

//--//

HRESULT Library_corlib_native_System_Reflection_MethodBase::GetMethodDescriptor(
    CLR_RT_StackFrame &stack,
    CLR_RT_HeapBlock &arg,
    CLR_RT_MethodDef_Instance &inst)
{
    (void)stack;

    NATIVE_PROFILE_CLR_CORE();
    return CLR_RT_ReflectionDef_Index::Convert(arg, inst) ? S_OK : CLR_E_NULL_REFERENCE;
}

HRESULT Library_corlib_native_System_Reflection_MethodBase::CheckInvokeArguments(
    const CLR_RT_MethodDef_Instance &md,
    CLR_RT_HeapBlock *args,
    int numArgs)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_SignatureParser parser{};
    CLR_RT_SignatureParser::Element element{};
    parser.Initialize_MethodSignature(md.assembly, md.target);

    // Skip the return type; the remaining signature elements are the explicit parameters.
    NANOCLR_CHECK_HRESULT(parser.AdvanceParameter(element));

    for (int i = 0; i < numArgs && parser.Available() > 0; i++)
    {
        NANOCLR_CHECK_HRESULT(parser.AdvanceParameter(element));

        // MakeCall passes the value, not a reference, so the callee's writes would be lost.
        if (element.IsByRef)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }

        bool isUnresolved = !NANOCLR_INDEX_IS_VALID(element.Class);

        // Without a generic context FixCall can't convert a VAR or MVAR argument, not even a null one.
        if (isUnresolved && element.Levels == 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }

        CLR_RT_HeapBlock &arg = args[i];

        if (arg.DataType() == DATATYPE_OBJECT && arg.Dereference() == nullptr)
        {
            continue;
        }

        CLR_RT_ReflectionDef_Index expectedType{};
        CLR_RT_TypeDescriptor targetType{};
        CLR_RT_TypeDescriptor sourceType{};

        if (isUnresolved)
        {
            // Only the array shape of T[] can be checked.
            if (FAILED(sourceType.InitializeFromObject(arg)) ||
                (sourceType.m_flags & CLR_RT_DataTypeLookup::c_Array) == 0 ||
                sourceType.m_reflex.levels < element.Levels)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_CAST);
            }

            continue;
        }

        expectedType.kind = REFLECTION_TYPE;
        expectedType.levels = (CLR_UINT16)element.Levels;
        expectedType.data.type = element.Class;

        NANOCLR_CHECK_HRESULT(targetType.InitializeFromReflection(expectedType));

        // FixCall checks and converts these, including enums from their underlying type.
        if (c_CLR_RT_DataTypeLookup[targetType.m_handlerCls.target->dataType].m_flags &
            CLR_RT_DataTypeLookup::c_OptimizedValueType)
        {
            continue;
        }

        if (FAILED(sourceType.InitializeFromObject(arg)) ||
            CLR_RT_ExecutionEngine::IsCastableTo(sourceType, targetType) == false)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_CAST);
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_MethodBase::CheckFlags(
    CLR_RT_StackFrame &stack,
    CLR_UINT32 mask,
    CLR_UINT32 flag)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_MethodDef_Instance md;
    bool fRes;
    CLR_RT_HeapBlock *hbMeth = stack.This();

    NANOCLR_CHECK_HRESULT(GetMethodDescriptor(stack, *hbMeth, md));

    if ((md.target->flags & mask) == flag)
    {
        fRes = true;
    }
    else
    {
        fRes = false;
    }

    stack.SetResult_Boolean(fRes);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_MethodBase::GetParametersNative___SZARRAY_SystemReflectionParameterInfo(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_MethodDef_Instance inst{};
    CLR_RT_MethodDef_Index idx{};
    CLR_RT_SignatureParser sigParser{};
    CLR_RT_TypeDef_Index paramInfoTypeDef;
    CLR_RT_SignatureParser::Element paramElement;
    CLR_RT_HeapBlock *hbObj;
    CLR_RT_HeapBlock *paramInfoElement;

    uint32_t paramCount = 0;

    CLR_RT_HeapBlock &top = stack.PushValueAndClear();

    CLR_RT_HeapBlock *hbMethodInfo = stack.This();

    idx.data = hbMethodInfo[FIELD___token].NumericByRef().u4;
    inst.InitializeFromIndex(idx);

    // 1st pass: get the number of parameters
    sigParser.Initialize_MethodSignature(inst.assembly, inst.target);

    // discard return value
    NANOCLR_CHECK_HRESULT(sigParser.AdvanceParameter(paramElement));

    // loop through all the parameters
    while (sigParser.Available() > 0)
    {
        paramCount++;

        NANOCLR_CHECK_HRESULT(sigParser.AdvanceParameter(paramElement));
    }

    // find <ParameterInfo> type definition, don't bother checking the result as it exists for sure
    g_CLR_RT_TypeSystem.FindTypeDef("ParameterInfo", "System.Reflection", paramInfoTypeDef);

    // create return array with the appropriate type
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, paramCount, paramInfoTypeDef));
    paramInfoElement = (CLR_RT_HeapBlock *)top.DereferenceArray()->GetFirstElement();

    // 2nd pass: get the actual type of each parameter
    sigParser.Initialize_MethodSignature(inst.assembly, inst.target);

    // discard return value
    NANOCLR_CHECK_HRESULT(sigParser.AdvanceParameter(paramElement));

    while (sigParser.Available() > 0)
    {
        NANOCLR_CHECK_HRESULT(sigParser.AdvanceParameter(paramElement));

        // create a new instance of <ParameterInfo>
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*paramInfoElement, paramInfoTypeDef));
        hbObj = paramInfoElement->Dereference();

        // VAR, MVAR and arrays of them have no generic context to resolve, parameter type stays null
        if (NANOCLR_INDEX_IS_VALID(paramElement.Class))
        {
            // get reference to the <ParameterInfo> instance
            CLR_RT_HeapBlock &paraTypeHB = hbObj[ParameterInfo::FIELD___parameterType];

            // create a new instance of the parameter type
            NANOCLR_CHECK_HRESULT(
                g_CLR_RT_ExecutionEngine.NewObjectFromIndex(paraTypeHB, g_CLR_RT_WellKnownTypes.TypeStatic));
            hbObj = paraTypeHB.Dereference();
            hbObj->SetReflection(paramElement.Class);

            // deal with array types
            if (paramElement.Levels > 0)
            {
                hbObj->ReflectionData().levels = (CLR_UINT16)paramElement.Levels;
            }
        }

        // move pointer to the next element
        paramInfoElement++;
    }

    NANOCLR_NOCLEANUP();
}
