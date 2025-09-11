//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "CorLib.h"

typedef Library_corlib_native_System_Reflection_MethodBase MethodBase;

HRESULT Library_corlib_native_System_Type::get_DeclaringType___SystemType(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Instance td;
    CLR_RT_HeapBlock &top = stack.PushValueAndClear();
    CLR_RT_HeapBlock *hbType = stack.This();

    NANOCLR_CHECK_HRESULT(Library_corlib_native_System_RuntimeType::GetTypeDescriptor(*hbType, td));

    if (td.target->HasValidEnclosingType())
    {
        CLR_RT_HeapBlock *hbObj;
        td.Set(td.Assembly(), td.target->EnclosingTypeIndex());

        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.TypeStatic));
        hbObj = top.Dereference();
        hbObj->SetReflection(td);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::GetGenericTypeDefinition___SystemType(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDescriptor desc{};
    CLR_RT_TypeSpec_Instance genericInstance = {};

    CLR_RT_HeapBlock &top = stack.PushValueAndClear();

    // build a TypeDescriptor from "this"
    CLR_RT_HeapBlock *hbType = stack.This();

    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.TypeStatic));

    if (hbType->ReflectionDataConst().kind == CLR_ReflectionType::REFLECTION_TYPESPEC)
    {
        // instanciate the generic type
        genericInstance.InitializeFromIndex(hbType->ObjectGenericType());

        CLR_RT_SignatureParser parser;
        parser.Initialize_TypeSpec(genericInstance);

        CLR_RT_SignatureParser::Element element;

        // consume GENERICINST
        parser.Advance(element);

        // consume class | value type
        parser.Advance(element);

        top.Dereference()->SetReflection(element.Class);
    }
    else
    {
        NANOCLR_CHECK_HRESULT(desc.InitializeFromReflection(hbType->ReflectionDataConst()));
        top.Dereference()->SetReflection(desc.m_handlerCls);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::GetMethod___SystemReflectionMethodInfo__STRING__SystemReflectionBindingFlags(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    const char *szText = stack.Arg1().RecoverString();
    FAULT_ON_NULL(szText);

    NANOCLR_SET_AND_LEAVE(GetMethods(stack, szText, stack.Arg2().NumericByRef().s4, nullptr, 0, false));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::IsInstanceOfType___BOOLEAN__OBJECT(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDescriptor descTarget;
    CLR_RT_TypeDescriptor desc{};
    CLR_RT_HeapBlock *hbType = stack.This();

    if (hbType->DataType() != DATATYPE_REFLECTION)
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);

    NANOCLR_CHECK_HRESULT(descTarget.InitializeFromReflection(hbType->ReflectionDataConst()));
    NANOCLR_CHECK_HRESULT(desc.InitializeFromObject(stack.Arg1()));

    stack.SetResult_Boolean(CLR_RT_ExecutionEngine::IsInstanceOf(desc, descTarget, false));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::GetGenericArguments___SZARRAY_SystemType(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    int count;

    CLR_RT_SignatureParser parser{};
    CLR_RT_SignatureParser::Element elem;
    CLR_RT_TypeSpec_Instance genericInstance;
    CLR_RT_HeapBlock *hbType;
    CLR_RT_HeapBlock *pElems;

    CLR_RT_HeapBlock &top = stack.PushValueAndClear();

    // build a TypeDescriptor from "this"
    hbType = stack.This();

    // sanity check for type spec instance
    if (hbType->ReflectionDataConst().kind != CLR_ReflectionType::REFLECTION_TYPESPEC)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // instanciate the generic type
    if (genericInstance.InitializeFromIndex(hbType->ObjectGenericType()))
    {
        // parse the type spec to get the generic arguments
        parser.Initialize_TypeSpec(genericInstance);

        // consume GENERICINST
        NANOCLR_CHECK_HRESULT(parser.Advance(elem));

        // consume class | value type
        NANOCLR_CHECK_HRESULT(parser.Advance(elem));

        // get the number of generic parameters
        count = parser.GenParamCount;

        // allocate an array to hold the generic arguments
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, count, g_CLR_RT_WellKnownTypes.TypeStatic));

        pElems = (CLR_RT_HeapBlock *)top.DereferenceArray()->GetFirstElement();

        // fill the array with Type objects for each generic argument
        for (int i = 0; i < count; i++)
        {
            NANOCLR_CHECK_HRESULT(parser.Advance(elem));

            CLR_RT_HeapBlock &slot = pElems[i];
            NANOCLR_CHECK_HRESULT(
                g_CLR_RT_ExecutionEngine.NewObjectFromIndex(slot, g_CLR_RT_WellKnownTypes.TypeStatic));

            CLR_RT_HeapBlock *hbObj = slot.Dereference();
            hbObj->SetReflection(elem.Class);
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::
    InvokeMember___OBJECT__STRING__SystemReflectionBindingFlags__SystemReflectionBinder__OBJECT__SZARRAY_OBJECT(
        CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::GetConstructor___SystemReflectionConstructorInfo__SZARRAY_SystemType(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array *array = stack.Arg1().DereferenceArray();
    FAULT_ON_NULL(array);
    CLR_RT_HeapBlock *pParams;
    int iParams;

    pParams = (CLR_RT_HeapBlock *)array->GetFirstElement();
    iParams = array->m_numOfElements;

    NANOCLR_SET_AND_LEAVE(GetMethods(
        stack,
        nullptr,
        c_BindingFlags_CreateInstance | c_BindingFlags_Instance | c_BindingFlags_Public | c_BindingFlags_NonPublic,
        pParams,
        iParams,
        false));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::GetConstructors___SZARRAY_SystemReflectionConstructorInfo(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(GetMethods(
        stack,
        nullptr,
        c_BindingFlags_CreateInstance | c_BindingFlags_Instance | c_BindingFlags_Public,
        nullptr,
        0,
        true));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::GetMethod___SystemReflectionMethodInfo__STRING__SZARRAY_SystemType(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array *array = stack.Arg2().DereferenceArray();
    CLR_RT_HeapBlock *pParams;
    int iParams;

    if (array)
    {
        pParams = (CLR_RT_HeapBlock *)array->GetFirstElement();
        iParams = array->m_numOfElements;
    }
    else
    {
        pParams = nullptr;
        iParams = 0;
    }

    const char *szText = stack.Arg1().RecoverString();
    FAULT_ON_NULL(szText);

    NANOCLR_SET_AND_LEAVE(GetMethods(stack, szText, c_BindingFlags_DefaultLookup, pParams, iParams, false));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::GetMethod___SystemReflectionMethodInfo__STRING(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    const char *szText = stack.Arg1().RecoverString();
    FAULT_ON_NULL(szText);

    NANOCLR_SET_AND_LEAVE(GetMethods(stack, szText, c_BindingFlags_DefaultLookup, nullptr, 0, false));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::get_IsNotPublic___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(CheckFlags(stack, CLR_RECORD_TYPEDEF::TD_Scope_Mask, CLR_RECORD_TYPEDEF::TD_Scope_NotPublic));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::get_IsPublic___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(CheckFlags(stack, CLR_RECORD_TYPEDEF::TD_Scope_Mask, CLR_RECORD_TYPEDEF::TD_Scope_Public));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::get_IsClass___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(
        CheckFlags(stack, CLR_RECORD_TYPEDEF::TD_Semantics_Mask, CLR_RECORD_TYPEDEF::TD_Semantics_Class));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::get_IsInterface___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(
        CheckFlags(stack, CLR_RECORD_TYPEDEF::TD_Semantics_Mask, CLR_RECORD_TYPEDEF::TD_Semantics_Interface));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::get_IsValueType___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(
        CheckFlags(stack, CLR_RECORD_TYPEDEF::TD_Semantics_Mask, CLR_RECORD_TYPEDEF::TD_Semantics_ValueType));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::get_IsAbstract___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(CheckFlags(stack, CLR_RECORD_TYPEDEF::TD_Abstract, CLR_RECORD_TYPEDEF::TD_Abstract));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::get_IsEnum___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(
        CheckFlags(stack, CLR_RECORD_TYPEDEF::TD_Semantics_Mask, CLR_RECORD_TYPEDEF::TD_Semantics_Enum));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::get_IsSerializable___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(CheckFlags(stack, CLR_RECORD_TYPEDEF::TD_Serializable, CLR_RECORD_TYPEDEF::TD_Serializable));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::get_IsArray___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Instance td;
    CLR_RT_TypeSpec_Instance genericInstance = {};

    CLR_RT_HeapBlock *hbType = stack.This();

    if (hbType->ReflectionDataConst().kind == CLR_ReflectionType::REFLECTION_TYPESPEC)
    {
        // instanciate the generic type
        genericInstance.InitializeFromIndex(hbType->ObjectGenericType());

        CLR_RT_SignatureParser parser{};
        parser.Initialize_TypeSpec(genericInstance);

        CLR_RT_SignatureParser::Element element;

        // only need to consume the 1st element which has the level data we're looking for
        parser.Advance(element);

        // check if element has levels
        stack.SetResult_Boolean(element.Levels > 0);
    }
    else
    {
        NANOCLR_CHECK_HRESULT(Library_corlib_native_System_RuntimeType::GetTypeDescriptor(*hbType, td));

        stack.SetResult_Boolean(td.data == g_CLR_RT_WellKnownTypes.Array.data);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::get_IsGenericType___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    bool isGenericType;
    CLR_RT_TypeDescriptor desc{};

    CLR_RT_HeapBlock *hbType = stack.This();

    // turn the boxed System.Type ("this") into a TypeDescriptor
    NANOCLR_CHECK_HRESULT(desc.InitializeFromReflection(hbType->ReflectionDataConst()));

    // simple check: a generic type has generic parameters
    isGenericType = (desc.m_handlerCls.target->genericParamCount > 0);

    stack.SetResult_Boolean(isGenericType);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::get_IsGenericTypeDefinition___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    bool isTypeDefinition = false;
    CLR_RT_TypeDescriptor desc{};
    CLR_RT_TypeSpec_Instance genericInstance = {};

    CLR_RT_HeapBlock *hbType = stack.This();

    if (hbType->ReflectionDataConst().kind == CLR_ReflectionType::REFLECTION_TYPESPEC)
    {
        // instanciate the generic type
        genericInstance.InitializeFromIndex(hbType->ObjectGenericType());

        CLR_RT_SignatureParser parser;
        parser.Initialize_TypeSpec(genericInstance);

        CLR_RT_SignatureParser::Element element;

        // consume GENERICINST
        parser.Advance(element);

        // consume class | value type
        parser.Advance(element);

        // check if it has generic parameters
        if (parser.GenParamCount == 0)
        {
            isTypeDefinition = false;
        }
        else
        {
            // keep reading the generic parameters
            for (int i = 0; i < parser.GenParamCount; i++)
            {
                if (SUCCEEDED(parser.Advance(element)))
                {
                    if (element.DataType == DATATYPE_VAR || element.DataType == DATATYPE_MVAR)
                    {
                        // if we find a VAR or MVAR, then this is a generic type definition
                        isTypeDefinition = true;
                        break;
                    }
                    else
                    {
                        // anything else, this is not a generic type definition
                        isTypeDefinition = false;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        // turn the boxed System.Type ("this") into a TypeDescriptor
        NANOCLR_CHECK_HRESULT(desc.InitializeFromReflection(hbType->ReflectionDataConst()));

        isTypeDefinition = (desc.m_handlerCls.target->genericParamCount > 0);
    }

    stack.SetResult_Boolean(isTypeDefinition);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::GetTypeInternal___STATIC__SystemType__STRING__STRING__BOOLEAN__SZARRAY_I4(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    const char *szClass;
    const char *szAssembly;
    CLR_RT_HeapBlock_Array *verArray;
    CLR_RT_Assembly *assm;
    CLR_RT_TypeDef_Index td;
    CLR_INT32 *ver;
    bool fVersion;

    CLR_RECORD_VERSION version;

    CLR_RT_HeapBlock &top = stack.PushValueAndClear();
    CLR_RT_HeapBlock *hbObj;
    szClass = stack.Arg0().RecoverString();
    FAULT_ON_NULL(szClass);
    szAssembly = stack.Arg1().RecoverString();

    fVersion = stack.Arg2().NumericByRef().u1 != 0;

    if (fVersion)
    {
        verArray = stack.Arg3().DereferenceArray();
        FAULT_ON_NULL(verArray);
        ver = (CLR_INT32 *)verArray->GetFirstElement();

        version.majorVersion = ver[0];
        version.minorVersion = ver[1];
        version.buildNumber = ver[2];
        version.revisionNumber = ver[3];
    }
    else
    {
        memset(&version, 0, sizeof(CLR_RECORD_VERSION));
    }

    if (szAssembly)
    {
        assm = g_CLR_RT_TypeSystem.FindAssembly(szAssembly, fVersion ? &version : nullptr, fVersion);
    }
    else
    {
        assm = nullptr;
    }

    if (g_CLR_RT_TypeSystem.FindTypeDef(szClass, assm, td))
    {
#if defined(NANOCLR_APPDOMAINS)
        CLR_RT_TypeDef_Instance inst;

        inst.InitializeFromIndex(td);
        if (!g_CLR_RT_ExecutionEngine.GetCurrentAppDomain()->FindAppDomainAssembly(inst.m_assm))
            NANOCLR_SET_AND_LEAVE(S_OK);
#endif
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.TypeStatic));
        hbObj = top.Dereference();
        hbObj->SetReflection(td);
    }
    else
    {
        CLR_RT_ReflectionDef_Index reflex;
        if (g_CLR_RT_TypeSystem.FindTypeDef(szClass, assm, reflex))
        {
#if defined(NANOCLR_APPDOMAINS)
            CLR_RT_TypeDef_Instance inst;

            inst.InitializeFromIndex(reflex.m_data.m_type);
            if (!g_CLR_RT_ExecutionEngine.GetCurrentAppDomain()->FindAppDomainAssembly(inst.m_assm))
                NANOCLR_SET_AND_LEAVE(S_OK);
#endif
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.TypeStatic));
            hbObj = top.Dereference();
            hbObj->SetReflection(reflex);
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::GetTypeFromHandle___STATIC__SystemType__SystemRuntimeTypeHandle(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock &pThis = stack.Arg0();
    CLR_RT_HeapBlock &top = stack.PushValue();
    CLR_RT_HeapBlock *hbObj;

    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.TypeStatic));
    hbObj = top.Dereference();
    hbObj->Assign(pThis); // RuntimeTypeHandle and Type have the same representation.

    NANOCLR_NOCLEANUP();
}

//--//

HRESULT Library_corlib_native_System_Type::CheckFlags(CLR_RT_StackFrame &stack, CLR_UINT32 mask, CLR_UINT32 flag)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Instance td;
    bool fRes;
    CLR_RT_HeapBlock *hbType = stack.This();

    NANOCLR_CHECK_HRESULT(Library_corlib_native_System_RuntimeType::GetTypeDescriptor(*hbType, td));

    if ((td.target->flags & mask) == flag)
    {
        fRes = true;
    }
    //
    // Special case, an enum is a valuetype, so let's check for that one.
    //
    else if (mask == CLR_RECORD_TYPEDEF::TD_Semantics_Mask && flag == CLR_RECORD_TYPEDEF::TD_Semantics_ValueType)
    {
        NANOCLR_SET_AND_LEAVE(
            CheckFlags(stack, CLR_RECORD_TYPEDEF::TD_Semantics_Mask, CLR_RECORD_TYPEDEF::TD_Semantics_Enum));
    }
    else
    {
        fRes = false;
    }

    stack.SetResult_Boolean(fRes);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::GetFields(
    CLR_RT_StackFrame &stack,
    const char *szText,
    CLR_UINT32 bindingFlags,
    bool fAllMatches)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Instance td;
    CLR_RT_TypeDef_Instance tdArg;
    int iField;
    CLR_RT_HeapBlock *hbType = stack.This();

    if (bindingFlags == c_BindingFlags_Default)
        bindingFlags = c_BindingFlags_DefaultLookup;

    NANOCLR_CHECK_HRESULT(Library_corlib_native_System_RuntimeType::GetTypeDescriptor(*hbType, tdArg));

    {
        CLR_RT_HeapBlock &top = stack.PushValueAndClear();

        for (int pass = 0; pass < 2; pass++)
        {
            td = tdArg;
            iField = 0;

            do
            {
                CLR_RT_Assembly *assm = td.assembly;
                const CLR_RECORD_TYPEDEF *tdR = td.target;
                const CLR_RECORD_FIELDDEF *fd = td.assembly->GetFieldDef(tdR->firstStaticField);
                int iTot = tdR->instanceFieldsCount + tdR->staticFieldsCount;
                int i;
                CLR_RT_FieldDef_Index index;

                for (i = 0; i < iTot; i++, fd++)
                {
                    const char *fieldName = assm->GetString(fd->name);

                    if (fd->flags & CLR_RECORD_FIELDDEF::FD_NoReflection)
                    {
                        continue;
                    }

                    if (fd->flags & CLR_RECORD_FIELDDEF::FD_Static)
                    {
                        if ((bindingFlags & c_BindingFlags_Static) == 0)
                            continue;
                    }
                    else
                    {
                        if ((bindingFlags & c_BindingFlags_Instance) == 0)
                            continue;
                    }

                    if ((fd->flags & CLR_RECORD_FIELDDEF::FD_Scope_Mask) == CLR_RECORD_FIELDDEF::FD_Scope_Public)
                    {
                        if ((bindingFlags & c_BindingFlags_Public) == 0)
                            continue;
                    }
                    else
                    {
                        if ((bindingFlags & c_BindingFlags_NonPublic) == 0)
                            continue;
                    }

                    // In this block we check if requested name szText is the same as examined field name.
                    // We check if compare is case insensitive.
                    if (bindingFlags & c_BindingFlags_IgnoreCase)
                    {
                        // If strings are not eqaul - continue
                        if (szText != nullptr && hal_stricmp(fieldName, szText))
                        {
                            continue;
                        }
                    }
                    else // Case sensitive compare
                    {
                        // If strings are not eqaul - continue
                        if (szText != nullptr && strcmp(fieldName, szText))
                        {
                            continue;
                        }
                    }

                    index.Set(td.Assembly(), i + tdR->firstStaticField);

                    if (!fAllMatches)
                    {
                        CLR_RT_HeapBlock *hbObj;
                        NANOCLR_CHECK_HRESULT(
                            g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.FieldInfo));
                        hbObj = top.Dereference();
                        hbObj->SetReflection(index);

                        NANOCLR_SET_AND_LEAVE(S_OK);
                    }
                    else if (pass == 1)
                    {
                        CLR_RT_HeapBlock *elem = (CLR_RT_HeapBlock *)top.DereferenceArray()->GetElement(iField);
                        CLR_RT_HeapBlock *hbObj;
                        NANOCLR_CHECK_HRESULT(
                            g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*elem, g_CLR_RT_WellKnownTypes.FieldInfo));
                        hbObj = elem->Dereference();
                        NANOCLR_CHECK_HRESULT(hbObj->SetReflection(index));
                    }

                    iField++;
                }

                if (bindingFlags & c_BindingFlags_DeclaredOnly)
                    break;
            } while (td.SwitchToParent());

            if (pass == 0)
            {
                if (!fAllMatches)
                    NANOCLR_SET_AND_LEAVE(S_OK);

                NANOCLR_CHECK_HRESULT(
                    CLR_RT_HeapBlock_Array::CreateInstance(top, iField, g_CLR_RT_WellKnownTypes.FieldInfo));
            }
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Type::GetMethods(
    CLR_RT_StackFrame &stack,
    const char *szText,
    CLR_UINT32 bindingFlags,
    CLR_RT_HeapBlock *pParams,
    int iParams,
    bool fAllMatches)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_MethodDef_Instance inst{};
    inst.Clear();
    CLR_RT_TypeDef_Instance td;
    CLR_RT_TypeDef_Instance tdArg;
    int iMethod;
    CLR_RT_HeapBlock &top = stack.PushValueAndClear();
    CLR_RT_HeapBlock *hbType = stack.This();
    bool staticInstanceOnly = false;

    if (bindingFlags == c_BindingFlags_Default)
    {
        bindingFlags = c_BindingFlags_DefaultLookup;
    }

    // in default lookup mode we want the static methods only from the instance not from the base classes
    if (bindingFlags == c_BindingFlags_DefaultLookup)
    {
        staticInstanceOnly = true;
    }

    NANOCLR_CHECK_HRESULT(Library_corlib_native_System_RuntimeType::GetTypeDescriptor(*hbType, tdArg));

    for (int pass = 0; pass < 2; pass++)
    {
        td = tdArg;
        iMethod = 0;
        bool isInstance = true;

        do
        {
            CLR_RT_Assembly *assm = td.assembly;
            const CLR_RECORD_TYPEDEF *tdR = td.target;
            const CLR_RECORD_METHODDEF *md = assm->GetMethodDef(tdR->firstMethod);
            int iTot = tdR->staticMethodCount + tdR->instanceMethodCount + tdR->virtualMethodCount;
            int i;

            if (staticInstanceOnly)
            {
                if (isInstance)
                {
                    // for the instance we use DefaultLookup
                    bindingFlags = c_BindingFlags_DefaultLookup;
                    // goto the else block in the next round
                    isInstance = false;
                }
                else
                {
                    // otherwise (for the base classes) we use Instance+Public
                    bindingFlags = c_BindingFlags_Instance | c_BindingFlags_Public;
                }
            }

            for (i = 0; i < iTot; i++, md++)
            {
                if (md->flags & CLR_RECORD_METHODDEF::MD_Static)
                {
                    if ((bindingFlags & c_BindingFlags_Static) == 0)
                    {
                        continue;
                    }
                }
                else
                {
                    if ((bindingFlags & c_BindingFlags_Instance) == 0)
                    {
                        continue;
                    }
                }

                if ((md->flags & CLR_RECORD_METHODDEF::MD_Scope_Mask) == CLR_RECORD_METHODDEF::MD_Scope_Public)
                {
                    if ((bindingFlags & c_BindingFlags_Public) == 0)
                    {
                        continue;
                    }
                }
                else
                {
                    if ((bindingFlags & c_BindingFlags_NonPublic) == 0)
                    {
                        continue;
                    }
                }

                //--//

                if (md->flags & CLR_RECORD_METHODDEF::MD_Constructor)
                {
                    if ((bindingFlags & c_BindingFlags_CreateInstance) == 0)
                    {
                        continue;
                    }
                }
                else
                {
                    if ((bindingFlags & c_BindingFlags_CreateInstance) != 0)
                    {
                        continue;
                    }

                    if (szText != nullptr && !strcmp(assm->GetString(md->name), szText) == false)
                    {
                        continue;
                    }
                }

                if (pParams)
                {
                    CLR_RT_SignatureParser parserLeft{};
                    parserLeft.Initialize_MethodSignature(assm, md);
                    CLR_RT_SignatureParser parserRight{};
                    parserRight.Initialize_Objects(pParams, iParams, true);
                    CLR_RT_SignatureParser::Element res;

                    //
                    // Skip return value.
                    //
                    NANOCLR_CHECK_HRESULT(parserLeft.Advance(res));

                    if (CLR_RT_TypeSystem::MatchSignatureDirect(parserLeft, parserRight, true) == false)
                    {
                        continue;
                    }
                }

                CLR_RT_MethodDef_Index index{};
                index.Set(td.Assembly(), i + tdR->firstMethod);
                CLR_RT_MethodDef_Instance inst2{};
                inst2.InitializeFromIndex(index);

                if (fAllMatches)
                {
                    if (pass == 1)
                    {
                        CLR_RT_HeapBlock *elem = (CLR_RT_HeapBlock *)top.DereferenceArray()->GetElement(iMethod);
                        CLR_RT_HeapBlock *hbObj;
                        NANOCLR_CHECK_HRESULT(
                            g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*elem, g_CLR_RT_WellKnownTypes.MethodInfo));
                        hbObj = elem->Dereference();
                        NANOCLR_CHECK_HRESULT(hbObj->SetReflection(index));

                        // store token for type
                        hbObj[MethodBase::FIELD___token].NumericByRef().u4 = index.data;
                    }

                    iMethod++;
                }
                else
                {
                    CLR_RT_HeapBlock *hbObj;

                    if (NANOCLR_INDEX_IS_VALID(inst))
                    {
                        CLR_RT_SignatureParser parserLeft{};
                        parserLeft.Initialize_MethodSignature(&inst);
                        CLR_RT_SignatureParser parserRight{};
                        parserRight.Initialize_MethodSignature(&inst2);

                        CLR_RT_SignatureParser::Element resLeft;
                        CLR_RT_SignatureParser::Element resRight;

                        //
                        // Skip return value.
                        //
                        NANOCLR_CHECK_HRESULT(parserLeft.Advance(resLeft));
                        NANOCLR_CHECK_HRESULT(parserRight.Advance(resRight));

                        if (!pParams)
                        {
                            if (CLR_RT_TypeSystem::MatchSignatureDirect(parserLeft, parserRight, false) == false)
                            {
                                // Two methods with different signatures, we cannot distinguish between the two.
                                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                            }

                            // If they are identical signatures, the first one wins (subclass takes precendence)
                            continue;
                        }
                        else
                        {
                            bool fLeftBetterMatch = false;
                            bool fRightBetterMatch = false;

                            while (parserLeft.Available() > 0)
                            {
                                NANOCLR_CHECK_HRESULT(parserLeft.Advance(resLeft));
                                NANOCLR_CHECK_HRESULT(parserRight.Advance(resRight));

                                bool fRightBetterMatchT = CLR_RT_TypeSystem::MatchSignatureElement(
                                    resLeft,
                                    resRight,
                                    parserLeft,
                                    parserRight,
                                    true);
                                bool fLeftBetterMatchT = CLR_RT_TypeSystem::MatchSignatureElement(
                                    resRight,
                                    resLeft,
                                    parserLeft,
                                    parserRight,
                                    true);

                                // If fLeftBetterMatchT && fRightBetterMatchT, one is assignable from the other, they
                                // must be the same
                                //  !fLeftBetterMatchT && !fRightBetterMatchT cannot happen, since this signature
                                //  matches pParams

                                if (fLeftBetterMatchT && !fRightBetterMatchT)
                                {
                                    fLeftBetterMatch = true;
                                }

                                if (!fLeftBetterMatchT && fRightBetterMatchT)
                                {
                                    fRightBetterMatch = true;
                                }
                            }

                            if (fLeftBetterMatch && fRightBetterMatch)
                            {
                                // If the params match both Foo(Super, Sub) and Foo(Sub, Super)
                                // we cannot choose between the two
                                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                            }

                            // If they are identical signatures, the first one wins (subclass takes precendence)
                            // Only if Right is better do we have a strictly better match
                            if (!fRightBetterMatch)
                            {
                                continue;
                            }

                            // Found a better match
                        }
                    }

                    inst.InitializeFromIndex(index);

                    NANOCLR_CHECK_HRESULT(
                        g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.MethodInfo));
                    hbObj = top.Dereference();
                    hbObj->SetReflection(inst);

                    // store token for type
                    hbObj[MethodBase::FIELD___token].NumericByRef().u4 = inst.data;
                }
            }

            if (bindingFlags & (c_BindingFlags_DeclaredOnly | c_BindingFlags_CreateInstance))
            {
                break;
            }

        } while (td.SwitchToParent());

        if (pass == 0)
        {
            if (!fAllMatches)
            {
                NANOCLR_SET_AND_LEAVE(S_OK);
            }

            NANOCLR_CHECK_HRESULT(
                CLR_RT_HeapBlock_Array::CreateInstance(top, iMethod, g_CLR_RT_WellKnownTypes.MethodInfo));
        }
    }

    NANOCLR_NOCLEANUP();
}
