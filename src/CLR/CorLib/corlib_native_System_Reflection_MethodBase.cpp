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

        CLR_RT_TypeSpec_Index owner;

        if (CLR_RT_TypeSystem::GetGenericOwnerRow(hbMeth->ReflectionDataConst().genericOwner, cls, owner))
        {
            hbObj->SetReflection(owner);
        }
        else
        {
            hbObj->SetReflection(cls);
        }
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

        if (GetGenericFlags(md) & MethodBase_GenericMethodFlags_ContainsGenericParameters)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }

        if (pArray)
        {
            args = (CLR_RT_HeapBlock *)pArray->GetFirstElement();
            argumentsCount = pArray->m_numOfElements;
        }

        if ((mdR->flags & CLR_RECORD_METHODDEF::MD_Constructor) && obj.Dereference() == nullptr)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_TARGET);
        }

        // Preserve null-target invocation for instance methods that do not access this.
        if ((mdR->flags & CLR_RECORD_METHODDEF::MD_Static) == 0 && obj.Dereference() != nullptr)
        {
            CLR_RT_TypeDef_Instance declaringType{};

            if (declaringType.InitializeFromMethod(md) == false ||
                CLR_RT_ExecutionEngine::IsInstanceOf(obj, declaringType) == false)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_TARGET);
            }

            if (md.genericType != nullptr && NANOCLR_INDEX_IS_VALID(*md.genericType))
            {
                CLR_RT_HeapBlock *target = nullptr;

                if (SUCCEEDED(obj.EnsureObjectReference(target)) && target->IsAGenericInstance() &&
                    target->ObjectCls().data == declaringType.data &&
                    !CLR_RT_TypeSystem::IsSameGenericInstance(target->ObjectGenericType(), *md.genericType))
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_TARGET);
                }
            }
        }

        NANOCLR_CHECK_HRESULT(CheckInvokeArguments(md, args, argumentsCount));

        NANOCLR_CHECK_HRESULT(stack.MakeCall(md, &obj, args, argumentsCount));
    }
    else
    {
        if (mdR->retValDataType != DATATYPE_VOID)
        {
            NanoCLRDataType returnType = (NanoCLRDataType)mdR->retValDataType;

            if (returnType == DATATYPE_VAR || returnType == DATATYPE_MVAR)
            {
                CLR_RT_SignatureParser parser{};
                CLR_RT_SignatureParser argument;
                CLR_RT_SignatureParser::Element element;

                parser.Initialize_MethodSignature(md.assembly, md.target);

                if (SUCCEEDED(parser.AdvanceParameter(element)) && element.Levels == 0 &&
                    CLR_RT_TypeSystem::PositionAtParameterArgument(element, md.genericType, &md.methodSpec, argument) &&
                    SUCCEEDED(argument.AdvanceParameter(element)) && element.Levels == 0)
                {
                    returnType = element.DataType;
                }
            }

            // Only small primitives need ChangeDataType
            if (returnType < DATATYPE_I4)
            {
                stack.TopValue().ChangeDataType(returnType);
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
    if (!CLR_RT_ReflectionDef_Index::Convert(arg, inst))
    {
        return CLR_E_NULL_REFERENCE;
    }

    if (arg.ReflectionDataConst().GetGenericOwner(inst.m_typeSpecStorage))
    {
        inst.genericType = &inst.m_typeSpecStorage;
    }

    arg.ReflectionDataConst().GetGenericMethodSpec(inst.methodSpec);

    return S_OK;
}

HRESULT Library_corlib_native_System_Reflection_MethodBase::NewMethodInfo(
    CLR_RT_HeapBlock &ref,
    const CLR_RT_MethodDef_Index &md,
    CLR_UINT16 slot)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(ref, g_CLR_RT_WellKnownTypes.MethodInfo));
    NANOCLR_CHECK_HRESULT(ref.Dereference()->SetReflection(md));

    ref.Dereference()->ReflectionData().genericOwner = slot;

    NANOCLR_NOCLEANUP();
}

CLR_UINT32 Library_corlib_native_System_Reflection_MethodBase::GetGenericFlags(const CLR_RT_MethodDef_Instance &md)
{
    CLR_UINT32 flags = MethodBase_GenericMethodFlags_None;
    CLR_RT_TypeDef_Instance declaring{};

    if (md.target->genericParamCount > 0)
    {
        flags = NANOCLR_INDEX_IS_VALID(md.methodSpec) ? MethodBase_GenericMethodFlags_GenericMethod
                                                      : MethodBase_GenericMethodFlags_GenericMethod |
                                                            MethodBase_GenericMethodFlags_GenericMethodDefinition |
                                                            MethodBase_GenericMethodFlags_ContainsGenericParameters;
    }

    if (declaring.InitializeFromMethod(md) && declaring.target->genericParamCount > 0 &&
        (md.genericType == nullptr || !NANOCLR_INDEX_IS_VALID(*md.genericType) ||
         CLR_RT_TypeSystem::IsOpenInstance(*md.genericType)))
    {
        flags |= MethodBase_GenericMethodFlags_ContainsGenericParameters;
    }

    return flags;
}

HRESULT Library_corlib_native_System_Reflection_MethodBase::
    GetGenericFlags___SystemReflectionMethodBaseGenericMethodFlags(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_MethodDef_Instance md;

    NANOCLR_CHECK_HRESULT(GetMethodDescriptor(stack, *stack.This(), md));

    stack.SetResult_I4((CLR_INT32)GetGenericFlags(md));

    NANOCLR_NOCLEANUP();
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

    const CLR_RT_TypeSpec_Index *owner =
        md.genericType != nullptr && NANOCLR_INDEX_IS_VALID(*md.genericType) ? md.genericType : nullptr;
    const CLR_RT_MethodDef_Instance *context = NANOCLR_INDEX_IS_VALID(md.methodSpec) ? &md : nullptr;

    // Skip the return type; the remaining signature elements are the explicit parameters.
    NANOCLR_CHECK_HRESULT(parser.AdvanceParameter(element));

    for (int i = 0; i < numArgs && parser.Available() > 0; i++)
    {
        CLR_RT_SignatureParser parameter = parser;
        CLR_RT_GenericTarget target;
        const CLR_RT_TypeSpec_Index *targetOwner = owner;

        NANOCLR_CHECK_HRESULT(parser.AdvanceParameter(element));

        CLR_RT_SignatureParser argument;

        if (CLR_RT_TypeSystem::PositionAtParameterArgument(element, owner, &md.methodSpec, argument))
        {
            CLR_RT_SignatureParser::Element resolved;
            int levels = element.Levels;

            parameter = argument;
            targetOwner = nullptr;

            NANOCLR_CHECK_HRESULT(argument.AdvanceParameter(resolved));

            element.Class = resolved.Class;
            element.DataType = resolved.DataType;
            element.Levels = levels + resolved.Levels;
        }

        // MakeCall passes the value, not a reference, so the callee's writes would be lost.
        if (element.IsByRef)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }

        // Without a generic context the argument can't be checked or converted, not even a null one.
        if (!NANOCLR_INDEX_IS_VALID(element.Class))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }

        if (CLR_RT_TypeSystem::CollectGenericTarget(parameter, targetOwner, context, target) == S_FALSE)
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
            CLR_RT_ExecutionEngine::IsInstanceOf(sourceType, targetType) == false)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        if (target.m_count > 0)
        {
            CLR_RT_GenericMatch match = CLR_RT_TypeSystem::MatchGenericArguments(arg, target);

            if (match == GenericMatch_Different || match == GenericMatch_NoTypeSpec)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            }
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
    CLR_RT_SignatureParser sigParser{};
    CLR_RT_TypeDef_Index paramInfoTypeDef;
    CLR_RT_SignatureParser::Element paramElement;
    CLR_RT_HeapBlock *hbObj;
    CLR_RT_HeapBlock *paramInfoElement;
    CLR_RT_TypeSpec_Index owner;

    uint32_t paramCount = 0;

    CLR_RT_HeapBlock &top = stack.PushValueAndClear();

    NANOCLR_CHECK_HRESULT(GetMethodDescriptor(stack, *stack.This(), inst));

    owner.Clear();

    if (inst.genericType != nullptr)
    {
        owner = *inst.genericType;
    }

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
        CLR_RT_SignatureParser parameter = sigParser;

        NANOCLR_CHECK_HRESULT(sigParser.AdvanceParameter(paramElement));

        // create a new instance of <ParameterInfo>
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*paramInfoElement, paramInfoTypeDef));
        hbObj = paramInfoElement->Dereference();

        {
            CLR_RT_TypeDescriptor desc{};
            CLR_RT_TypeSpec_Index elementSpec;
            CLR_RT_HeapBlock &paraTypeHB = hbObj[ParameterInfo::FIELD___parameterType];

            CLR_RT_TypeDef_Index declaring;

            declaring.Set(inst.Assembly(), inst.CrossReference().GetOwner());

            NANOCLR_CHECK_HRESULT(
                desc.InitializeFromClosedSignature(parameter, &owner, &elementSpec, &declaring, &inst, &inst));
            NANOCLR_CHECK_HRESULT(
                g_CLR_RT_ExecutionEngine.NewObjectFromIndex(paraTypeHB, g_CLR_RT_WellKnownTypes.TypeStatic));
            NANOCLR_CHECK_HRESULT(paraTypeHB.Dereference()->SetReflection(desc.GetTypeReflection(elementSpec)));
        }

        // move pointer to the next element
        paramInfoElement++;
    }

    NANOCLR_NOCLEANUP();
}
