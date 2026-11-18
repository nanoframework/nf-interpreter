//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"

HRESULT Library_corlib_native_System_RuntimeType::get_Assembly___SystemReflectionAssembly(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Instance td;
    CLR_RT_HeapBlock *hbType = stack.This();

    if (hbType->ReflectionDataConst().kind == REFLECTION_GENERICPARAM)
    {
        td.Set(hbType->ReflectionDataConst().data.genericParam.Assembly(), 0);
    }
    else
    {
        NANOCLR_CHECK_HRESULT(GetTypeDescriptor(*hbType, td, nullptr));
    }

    {
        CLR_RT_Assembly_Index index{};
        index.Set(td.Assembly());
        CLR_RT_HeapBlock &top = stack.PushValue();
        CLR_RT_HeapBlock *hbObj;

        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.Assembly));
        hbObj = top.Dereference();
        hbObj->SetReflection(index);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_RuntimeType::get_Name___STRING(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();
    CLR_RT_HeapBlock *hbType = stack.This();

    NANOCLR_CHECK_HRESULT(GetName(*hbType, false, stack.PushValueAndClear(), false));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_RuntimeType::get_FullName___STRING(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();
    CLR_RT_HeapBlock *hbType = stack.This();

    NANOCLR_CHECK_HRESULT(GetName(*hbType, true, stack.PushValueAndClear(), false));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_RuntimeType::ToString___STRING(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();
    CLR_RT_HeapBlock *hbType = stack.This();

    NANOCLR_CHECK_HRESULT(GetName(*hbType, true, stack.PushValueAndClear(), true));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_RuntimeType::get_BaseType___SystemType(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_ReflectionDef_Index word;
    CLR_RT_HeapBlock &top = stack.PushValueAndClear();
    CLR_RT_HeapBlock *hbType = stack.This();

    hr = CLR_RT_TypeSystem::GetBaseTypeWord(hbType->ReflectionDataConst(), word);

    NANOCLR_CHECK_HRESULT(hr);

    if (hr == S_OK)
    {
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.TypeStatic));
        NANOCLR_CHECK_HRESULT(top.Dereference()->SetReflection(word));
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_RuntimeType::
    GetMethods___SZARRAY_SystemReflectionMethodInfo__SystemReflectionBindingFlags(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    return Library_corlib_native_System_Type::GetMethods(
        stack,
        nullptr,
        stack.Arg1().NumericByRef().s4,
        nullptr,
        0,
        true);
}

HRESULT Library_corlib_native_System_RuntimeType::
    GetField___SystemReflectionFieldInfo__STRING__SystemReflectionBindingFlags(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    return Library_corlib_native_System_Type::GetFields(
        stack,
        stack.Arg1().RecoverString(),
        stack.Arg2().NumericByRef().s4,
        false);
}

HRESULT Library_corlib_native_System_RuntimeType::
    GetFields___SZARRAY_SystemReflectionFieldInfo__SystemReflectionBindingFlags(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    return Library_corlib_native_System_Type::GetFields(stack, nullptr, stack.Arg1().NumericByRef().s4, true);
}

static const int c_MaxInterfaces = 32;

HRESULT Library_corlib_native_System_RuntimeType::GetInterfaces___SZARRAY_SystemType(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_ReflectionDef_Index words[c_MaxInterfaces];
    CLR_RT_HeapBlock &top = stack.PushValueAndClear();
    int count;

    NANOCLR_CHECK_HRESULT(
        CLR_RT_TypeSystem::CollectInterfaceWords(stack.This()->ReflectionDataConst(), words, c_MaxInterfaces, count));

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, count, g_CLR_RT_WellKnownTypes.TypeStatic));

    if (count > 0)
    {
        CLR_RT_HeapBlock *ptr = (CLR_RT_HeapBlock *)top.DereferenceArray()->GetFirstElement();

        for (int i = 0; i < count; i++, ptr++)
        {
            NANOCLR_CHECK_HRESULT(
                g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*ptr, g_CLR_RT_WellKnownTypes.TypeStatic));
            NANOCLR_CHECK_HRESULT(ptr->Dereference()->SetReflection(words[i]));
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_RuntimeType::GetElementType___SystemType(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDescriptor desc{};
    CLR_RT_TypeDescriptor descSub;
    CLR_RT_HeapBlock &top = stack.PushValueAndClear();
    CLR_RT_HeapBlock *hbType = stack.This();

    if (hbType->ReflectionDataConst().kind == REFLECTION_GENERICPARAM && hbType->ReflectionDataConst().levels == 0)
    {
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    if ((hbType->ReflectionDataConst().kind == REFLECTION_TYPESPEC ||
         hbType->ReflectionDataConst().kind == REFLECTION_GENERICPARAM) &&
        hbType->ReflectionDataConst().levels > 0)
    {
        CLR_RT_ReflectionDef_Index element = hbType->ReflectionDataConst();

        element.levels--;

        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.TypeStatic));
        top.Dereference()->SetReflection(element);

        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    NANOCLR_CHECK_HRESULT(desc.InitializeFromReflection(hbType->ReflectionDataConst()));

    if (desc.GetElementType(descSub))
    {
        CLR_RT_HeapBlock *hbObj;

        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.TypeStatic));
        hbObj = top.Dereference();
        hbObj->SetReflection(descSub.m_reflex);
    }

    NANOCLR_NOCLEANUP();
}

//--//

HRESULT Library_corlib_native_System_RuntimeType::AppendGenericArguments(
    const CLR_RT_GenericInstance &record,
    char *&sz,
    size_t &size)
{
    NANOCLR_HEADER();

    CLR_UINT8 pending[CLR_RT_GenericInstance::c_MaxNodes];
    CLR_UINT8 openLevels[CLR_RT_GenericInstance::c_MaxNodes];
    int depth = 0;
    bool comma = false;

    if (record.m_nodes[0].ArgCount == 0)
    {
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    pending[0] = record.m_nodes[0].ArgCount;
    NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.QueueStringToBuffer(sz, size, "["));

    for (int i = 1; i < record.m_nodeCount; i++)
    {
        const CLR_RT_GenericInstanceNode &node = record.m_nodes[i];

        if (comma)
        {
            NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.QueueStringToBuffer(sz, size, ","));
        }

        if (node.DataType == DATATYPE_VAR)
        {
            // the open parameter of the type that owns it, by its name
            CLR_RT_TypeDef_Instance owner{};

            if (!owner.InitializeFromIndex(node.Class) || node.Position >= owner.target->genericParamCount)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
            }

            NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.QueueStringToBuffer(
                sz,
                size,
                owner.assembly->GetString(
                    owner.assembly->GetGenericParam(owner.target->firstGenericParam + node.Position)->name)));
        }
        else
        {
            NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.BuildTypeName(node.Class, sz, size));
        }

        if (node.ArgCount > 0)
        {
            NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.QueueStringToBuffer(sz, size, "["));

            pending[++depth] = node.ArgCount;
            openLevels[depth] = (CLR_UINT8)node.Levels;
            comma = false;
            continue;
        }

        for (int level = 0; level < node.Levels; level++)
        {
            NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.QueueStringToBuffer(sz, size, "[]"));
        }

        comma = true;
        pending[depth]--;

        while (pending[depth] == 0)
        {
            NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.QueueStringToBuffer(sz, size, "]"));

            if (depth == 0)
            {
                break;
            }

            for (int level = 0; level < openLevels[depth]; level++)
            {
                NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.QueueStringToBuffer(sz, size, "[]"));
            }

            pending[--depth]--;
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_RuntimeType::GetTypeDescriptor(
    CLR_RT_HeapBlock &arg,
    CLR_RT_TypeDef_Instance &inst,
    CLR_UINT32 *levels)
{
    NATIVE_PROFILE_CLR_CORE();

    if (arg.DataType() == DATATYPE_REFLECTION && arg.ReflectionDataConst().kind == REFLECTION_GENERICPARAM)
    {
        if (levels != nullptr)
        {
            *levels = arg.ReflectionDataConst().levels;
        }

        return arg.ReflectionDataConst().levels > 0 && inst.InitializeFromIndex(g_CLR_RT_WellKnownTypes.Array)
                   ? S_OK
                   : CLR_E_NOT_SUPPORTED;
    }

    return CLR_RT_ReflectionDef_Index::Convert(arg, inst, levels) ? S_OK : CLR_E_NULL_REFERENCE;
}

HRESULT Library_corlib_native_System_RuntimeType::GetTypeDescriptor(
    CLR_RT_HeapBlock &arg,
    CLR_RT_TypeDef_Instance &inst)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_UINT32 levels;

    NANOCLR_CHECK_HRESULT(GetTypeDescriptor(arg, inst, &levels));

    if (levels > 0)
    {
        inst.InitializeFromIndex(g_CLR_RT_WellKnownTypes.Array);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_RuntimeType::GetName(
    CLR_RT_HeapBlock &arg,
    bool fFullName,
    CLR_RT_HeapBlock &res,
    bool fOpenAsString)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Instance td;
    CLR_UINT32 levels;
    char rgBuffer[256]{};
    char *szBuffer;
    size_t iBuffer;

    szBuffer = rgBuffer;
    iBuffer = MAXSTRLEN(rgBuffer);

    if (arg.ReflectionDataConst().kind == REFLECTION_GENERICPARAM)
    {
        CLR_RT_Assembly *assembly;
        const CLR_RECORD_GENERICPARAM *record;

        if (fFullName && !fOpenAsString)
        {
            res.SetObjectReference(nullptr);

            NANOCLR_SET_AND_LEAVE(S_OK);
        }

        if (!CLR_RT_TypeSystem::GetGenericParam(arg.ReflectionDataConst(), assembly, record))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }

        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_TypeSystem.QueueStringToBuffer(szBuffer, iBuffer, assembly->GetString(record->name)));

        for (int level = 0; level < arg.ReflectionDataConst().levels; level++)
        {
            NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.QueueStringToBuffer(szBuffer, iBuffer, "[]"));
        }

        NANOCLR_SET_AND_LEAVE(CLR_RT_HeapBlock_String::CreateInstance(res, rgBuffer));
    }

    NANOCLR_CHECK_HRESULT(GetTypeDescriptor(arg, td, &levels));

    // FullName of a type that contains generic parameters is null, ToString names the parameters
    if (fFullName && !fOpenAsString && arg.ReflectionDataConst().kind == REFLECTION_TYPESPEC &&
        CLR_RT_TypeSystem::IsOpenInstance(arg.ReflectionDataConst().data.typeSpec))
    {
        res.SetObjectReference(nullptr);

        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    if (fFullName && arg.ReflectionDataConst().kind == REFLECTION_TYPESPEC)
    {
        // closed generic: Ns.Name`1[Ns.Argument], the arguments follow the name and precede the array brackets
        CLR_RT_TypeSpec_Instance spec{};
        CLR_RT_GenericInstance *record = nullptr;
        char *argBuffer;
        size_t argSize;

        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_TypeSystem.BuildTypeName(td, szBuffer, iBuffer, CLR_RT_TypeSystem::TYPENAME_FLAGS_FULL, 0));

        if (spec.InitializeFromIndex(arg.ReflectionDataConst().data.typeSpec))
        {
            record = CLR_RT_TypeSystem::FindOrCreateGenericInstance(spec);
        }

        // too long for the buffer: the name stays without arguments
        argBuffer = szBuffer;
        argSize = iBuffer;

        if (record == nullptr || FAILED(AppendGenericArguments(*record, argBuffer, argSize)))
        {
            *szBuffer = 0;
        }
        else
        {
            szBuffer = argBuffer;
            iBuffer = argSize;
        }

        while (levels-- > 0)
        {
            NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.QueueStringToBuffer(szBuffer, iBuffer, "[]"));
        }
    }
    else
    {
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_TypeSystem
                .BuildTypeName(td, szBuffer, iBuffer, fFullName ? CLR_RT_TypeSystem::TYPENAME_FLAGS_FULL : 0, levels));
    }

    NANOCLR_SET_AND_LEAVE(CLR_RT_HeapBlock_String::CreateInstance(res, rgBuffer));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_RuntimeType::GetCustomAttributesNative___SZARRAY_OBJECT__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *returnArray = nullptr;
    CLR_RT_HeapBlock *callerType = nullptr;
    CLR_RT_TypeDef_Instance typeDefinition;
    int count = 0;

    // put the return array on the stack
    CLR_RT_HeapBlock &top = stack.PushValueAndClear();

    // get the caller type
    callerType = stack.This();

    NANOCLR_CHECK_HRESULT(GetTypeDescriptor(*callerType, typeDefinition));

    // setup attribute enumerator
    CLR_RT_AttributeEnumerator attributeEnumerator;
    attributeEnumerator.Initialize(typeDefinition);

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

            // use this to skip the 2nd pass if no attribute was found
            if (count == 0)
            {
                break;
            }

            // get the pointer to the first element
            returnArray = (CLR_RT_HeapBlock *)top.DereferenceArray()->GetFirstElement();

            // reset attribute enumerator
            attributeEnumerator.Initialize(typeDefinition);

            break;
        }
    } while (true);

    // 2nd pass: fill the array with the attributes types, if any
    NANOCLR_CHECK_HRESULT(GetCustomAttributes(attributeEnumerator, returnArray, count));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_RuntimeType::GetCustomAttributes(
    CLR_RT_AttributeEnumerator attributeEnumerator,
    CLR_RT_HeapBlock *&returnArray,
    int count)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    while (count > 0)
    {
        // move to the next attribute in the collection, if there is another
        if (attributeEnumerator.Advance())
        {
            CLR_RT_TypeDef_Instance instanceTypeDef;

            // get the type def for the current attribute
            attributeEnumerator.GetCurrent(&instanceTypeDef);

            // setup attribute parser
            CLR_RT_AttributeParser parser{};
            NANOCLR_CHECK_HRESULT(parser.Initialize(attributeEnumerator));

            while (true)
            {
                CLR_RT_AttributeParser::Value *val;

                // parse next attribute, if there is another
                NANOCLR_CHECK_HRESULT(parser.Next(val));

                if (val == nullptr)
                {
                    break;
                }
                else
                {
                    // check if this attribute has a default constructor or one with argument
                    if (val->m_mode == CLR_RT_AttributeParser::Value::c_DefaultConstructor)
                    {
                        // default constructor here

                        // get attribute type
                        returnArray->LoadFromReference(val->m_value);

                        // no parameters for the constructor
                        returnArray++;

                        // set next position to nullptr
                        returnArray->SetObjectReference(nullptr);
                    }
                    else if (val->m_mode == CLR_RT_AttributeParser::Value::c_ConstructorArgument)
                    {
                        // constructor with argument

                        // get the type for the class object
                        // the assembly has to be the instance type
                        CLR_RT_MethodDef_Index md{};
                        md.Set(instanceTypeDef.assembly->assemblyIndex, parser.m_mdIndex.Method());
                        CLR_RT_MethodDef_Instance mdInst{};
                        mdInst.InitializeFromIndex(md);

                        CLR_RT_TypeDef_Instance cls{};
                        if (cls.InitializeFromMethod(mdInst) == false)
                            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);

                        // create a new object for the attribute type and put it on the return array
                        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObject(*returnArray, cls));

                        // now the constructor parameter(s)
                        returnArray++;

                        // load the constructor parameter
                        returnArray->LoadFromReference(val->m_value);
                    }
                }
            }

            returnArray++;
            count--;
        }
        else
        {
            // no more attributes
            break;
        }
    }

    NANOCLR_NOCLEANUP();
}
