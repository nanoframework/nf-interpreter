//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"

HRESULT Library_corlib_native_System_Reflection_RuntimeMethodInfo::get_ReturnType___SystemType(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_MethodDef_Instance md;
    CLR_RT_SignatureParser parser{};
    CLR_RT_TypeDescriptor desc{};
    CLR_RT_TypeSpec_Index owner;
    CLR_RT_TypeSpec_Index elementSpec;
    CLR_RT_HeapBlock *hbMeth = stack.This();

    NANOCLR_CHECK_HRESULT(Library_corlib_native_System_Reflection_MethodBase::GetMethodDescriptor(stack, *hbMeth, md));

    parser.Initialize_MethodSignature(md.assembly, md.target);

    owner.Clear();
    hbMeth->ReflectionDataConst().GetGenericOwner(owner);

    {
        CLR_RT_TypeDef_Index declaring;

        declaring.Set(md.Assembly(), md.CrossReference().GetOwner());

        NANOCLR_CHECK_HRESULT(desc.InitializeFromClosedSignature(parser, &owner, &elementSpec, &declaring, &md, &md));
    }

    {
        CLR_RT_HeapBlock &top = stack.PushValue();
        CLR_RT_HeapBlock *hbObj;

        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.TypeStatic));
        hbObj = top.Dereference();
        hbObj->SetReflection(desc.GetTypeReflection(elementSpec));
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_RuntimeMethodInfo::GetCustomAttributesNative___SZARRAY_OBJECT__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *returnArray = nullptr;
    CLR_RT_MethodDef_Instance methodDefinition;
    CLR_RT_HeapBlock *callerMethod;
    int count = 0;

    // put the return array on the stack
    CLR_RT_HeapBlock &top = stack.PushValueAndClear();

    // get the caller method
    callerMethod = stack.This();

    NANOCLR_CHECK_HRESULT(
        Library_corlib_native_System_Reflection_MethodBase::GetMethodDescriptor(
            stack,
            *callerMethod,
            methodDefinition));

    // setup attribute enumerator
    CLR_RT_AttributeEnumerator attributeEnumerator;
    attributeEnumerator.Initialize(methodDefinition);

    // the return array has two positions for each attribute:
    // 1st: the attribute type
    // 2nd: the constructor parameters or nullptr, if the attribute has no constructor

    // 1st pass: count attributes
    do
    {
        // move to the next attribute in the collection, if any
        if (attributeEnumerator.Advance())
        {
            count++;
        }
        else
        {
            // done sweeping attributes

            // create the result array
            // (2 positions for each attribute)
            NANOCLR_CHECK_HRESULT(
                CLR_RT_HeapBlock_Array::CreateInstance(top, (count * 2), g_CLR_RT_WellKnownTypes.Object));

            // use this to skip to the 2nd pass if no attribute was found
            if (count == 0)
            {
                break;
            }

            // get the pointer to the first element
            returnArray = (CLR_RT_HeapBlock *)top.DereferenceArray()->GetFirstElement();

            // reset attribute enumerator
            attributeEnumerator.Initialize(methodDefinition);

            break;
        }
    } while (true);

    // 2nd pass: fill the array with the attributes types, if any
    NANOCLR_CHECK_HRESULT(
        Library_corlib_native_System_RuntimeType::GetCustomAttributes(attributeEnumerator, returnArray, count));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_RuntimeMethodInfo::GetGenericArguments___SZARRAY_SystemType(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_MethodDef_Instance md;
    CLR_RT_HeapBlock *hbMeth = stack.This();
    CLR_RT_HeapBlock &top = stack.PushValueAndClear();
    const CLR_RT_GenericInstance *instance;
    CLR_RT_HeapBlock *elements;
    int count;

    NANOCLR_CHECK_HRESULT(Library_corlib_native_System_Reflection_MethodBase::GetMethodDescriptor(stack, *hbMeth, md));

    count = md.target->genericParamCount;
    instance = CLR_RT_TypeSystem::GetMemberRecord(hbMeth->ReflectionDataConst().genericOwner, true);

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, count, g_CLR_RT_WellKnownTypes.TypeStatic));

    elements = (CLR_RT_HeapBlock *)top.DereferenceArray()->GetFirstElement();

    for (int i = 0; i < count; i++)
    {
        CLR_RT_ReflectionDef_Index word;

        if (instance != nullptr)
        {
            NANOCLR_CHECK_HRESULT(CLR_RT_TypeSystem::GetArgumentWord(*instance, i, word));
        }
        else
        {
            CLR_RT_TypeSystem::SetGenericParamWord(word, md.Assembly(), md.target->firstGenericParam + i, 0);
        }

        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.NewObjectFromIndex(elements[i], g_CLR_RT_WellKnownTypes.TypeStatic));
        NANOCLR_CHECK_HRESULT(elements[i].Dereference()->SetReflection(word));
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_RuntimeMethodInfo::
    MakeGenericMethod___SystemReflectionMethodInfo__SZARRAY_SystemType(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_MethodDef_Instance md;
    CLR_RT_TypeDef_Instance declaring{};
    CLR_RT_GenericInstance *record;
    CLR_RT_HeapBlock *hbMeth = stack.This();
    CLR_UINT16 ownerSlot = 0;

    NANOCLR_CHECK_HRESULT(Library_corlib_native_System_Reflection_MethodBase::GetMethodDescriptor(stack, *hbMeth, md));

    if (md.target->genericParamCount == 0 || NANOCLR_INDEX_IS_VALID(md.methodSpec))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    if (!declaring.InitializeFromMethod(md))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    if (declaring.target->genericParamCount > 0)
    {
        if (md.genericType == nullptr || CLR_RT_TypeSystem::IsOpenInstance(*md.genericType))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }

        ownerSlot = hbMeth->ReflectionDataConst().genericOwner;
    }

    hr = CLR_RT_GenericInstantiation::MakeGenericInstance(
        declaring,
        &md,
        ownerSlot,
        stack.Arg1().DereferenceArray(),
        record);

    NANOCLR_CHECK_HRESULT(hr);

    {
        CLR_RT_HeapBlock &top = stack.PushValueAndClear();

        if (hr == S_FALSE)
        {
            top.SetObjectReference(hbMeth);
            NANOCLR_SET_AND_LEAVE(S_OK);
        }

        NANOCLR_CHECK_HRESULT(
            Library_corlib_native_System_Reflection_MethodBase::NewMethodInfo(
                top,
                md,
                (CLR_UINT16)(record->m_self.TypeSpec() + 1)));
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_RuntimeMethodInfo::
    GetGenericMethodDefinition___SystemReflectionMethodInfo(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_MethodDef_Instance md;
    CLR_RT_HeapBlock *hbMeth = stack.This();
    const CLR_RT_GenericInstance *instance;

    NANOCLR_CHECK_HRESULT(Library_corlib_native_System_Reflection_MethodBase::GetMethodDescriptor(stack, *hbMeth, md));

    if (md.target->genericParamCount == 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    instance = CLR_RT_TypeSystem::GetMemberRecord(hbMeth->ReflectionDataConst().genericOwner, true);

    {
        CLR_RT_HeapBlock &top = stack.PushValueAndClear();

        if (instance == nullptr)
        {
            top.SetObjectReference(hbMeth);
            NANOCLR_SET_AND_LEAVE(S_OK);
        }

        const CLR_RT_GenericInstanceNode &root = instance->m_nodes[0];

        NANOCLR_CHECK_HRESULT(
            Library_corlib_native_System_Reflection_MethodBase::NewMethodInfo(
                top,
                md,
                (CLR_UINT16)(root.Levels | (root.Position << 8))));
    }

    NANOCLR_NOCLEANUP();
}
