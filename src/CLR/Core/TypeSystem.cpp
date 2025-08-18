//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"
#include <string>

#include "Core.h"
#include "corhdr_private.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

#define ITERATE_THROUGH_RECORDS(assm, i, tblName, tblNameUC)                                                           \
    const CLR_RECORD_##tblNameUC *src = (const CLR_RECORD_##tblNameUC *)assm->GetTable(TBL_##tblName);                 \
    CLR_RT_##tblName##_CrossReference *dst = assm->crossReference##tblName;                                            \
    for (i = 0; i < assm->tablesSize[TBL_##tblName]; i++, src++, dst++)

////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(VIRTUAL_DEVICE)
#define NANOCLR_TRACE_DEFAULT(win, arm) (win)
#else
#define NANOCLR_TRACE_DEFAULT(win, arm) (arm)
#endif

#if defined(NANOCLR_TRACE_ERRORS)
int s_CLR_RT_fTrace_Errors = NANOCLR_TRACE_DEFAULT(c_CLR_RT_Trace_Info, c_CLR_RT_Trace_Info);
#endif

#if defined(NANOCLR_TRACE_EXCEPTIONS)
int s_CLR_RT_fTrace_Exceptions = NANOCLR_TRACE_DEFAULT(c_CLR_RT_Trace_Info, c_CLR_RT_Trace_Info);
#endif

#if defined(NANOCLR_TRACE_INSTRUCTIONS)
int s_CLR_RT_fTrace_Instructions = NANOCLR_TRACE_DEFAULT(c_CLR_RT_Trace_Info, c_CLR_RT_Trace_None);
#endif

#if defined(NANOCLR_GC_VERBOSE)
int s_CLR_RT_fTrace_Memory = NANOCLR_TRACE_DEFAULT(c_CLR_RT_Trace_Info, c_CLR_RT_Trace_None);
#endif

#if defined(NANOCLR_TRACE_MEMORY_STATS)
int s_CLR_RT_fTrace_MemoryStats = NANOCLR_TRACE_DEFAULT(c_CLR_RT_Trace_Info, c_CLR_RT_Trace_Info);
#endif

#if defined(NANOCLR_GC_VERBOSE)
int s_CLR_RT_fTrace_GC = NANOCLR_TRACE_DEFAULT(c_CLR_RT_Trace_Info, c_CLR_RT_Trace_None);
#endif

#if defined(VIRTUAL_DEVICE)
int s_CLR_RT_fTrace_SimulateSpeed = NANOCLR_TRACE_DEFAULT(c_CLR_RT_Trace_Info, c_CLR_RT_Trace_None);
#endif

#if !defined(BUILD_RTM)
int s_CLR_RT_fTrace_AssemblyOverhead = NANOCLR_TRACE_DEFAULT(c_CLR_RT_Trace_Info, c_CLR_RT_Trace_Info);
#endif

#if defined(VIRTUAL_DEVICE)
int s_CLR_RT_fTrace_StopOnFAILED = NANOCLR_TRACE_DEFAULT(c_CLR_RT_Trace_None, c_CLR_RT_Trace_None);
#endif

#if defined(VIRTUAL_DEVICE)
int s_CLR_RT_fTrace_ARM_Execution = 0;

int s_CLR_RT_fTrace_RedirectLinesPerFile;
std::wstring s_CLR_RT_fTrace_RedirectOutput;
std::wstring s_CLR_RT_fTrace_RedirectCallChain;

std::wstring s_CLR_RT_fTrace_HeapDump_FilePrefix;
bool s_CLR_RT_fTrace_HeapDump_IncludeCreators = false;

bool s_CLR_RT_fTimeWarp = false;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

void CLR_RT_ReflectionDef_Index::Clear()
{
    NATIVE_PROFILE_CLR_CORE();
    kind = REFLECTION_INVALID;
    levels = 0;
    data.raw = 0;
}

CLR_UINT32 CLR_RT_ReflectionDef_Index::GetTypeHash() const
{
    NATIVE_PROFILE_CLR_CORE();
    switch (kind)
    {
        case REFLECTION_TYPE:
        {
            CLR_RT_TypeDef_Instance inst{};

            if (levels != 0)
                return 0;

            if (!inst.InitializeFromIndex(data.type))
                return 0;

            return inst.CrossReference().hash;
        }

        case REFLECTION_TYPE_DELAYED:
            return data.raw;
    }

    return 0;
}

void CLR_RT_ReflectionDef_Index::InitializeFromHash(CLR_UINT32 hash)
{
    NATIVE_PROFILE_CLR_CORE();
    kind = REFLECTION_TYPE_DELAYED;
    levels = 0;
    data.raw = hash;
}

CLR_UINT64 CLR_RT_ReflectionDef_Index::GetRawData() const
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_UINT64 dataRaw;
    _ASSERTE(sizeof(dataRaw) == sizeof(*this));

    memcpy(&dataRaw, this, sizeof(dataRaw));

    return dataRaw;
}

void CLR_RT_ReflectionDef_Index::SetRawData(CLR_UINT64 dataRaw)
{
    NATIVE_PROFILE_CLR_CORE();
    _ASSERTE(sizeof(dataRaw) == sizeof(*this));

    memcpy(this, &dataRaw, sizeof(dataRaw));
}

bool CLR_RT_ReflectionDef_Index::Convert(CLR_RT_HeapBlock &ref, CLR_RT_Assembly_Instance &inst)
{
    NATIVE_PROFILE_CLR_CORE();
    if (ref.DataType() == DATATYPE_REFLECTION)
    {
        return inst.InitializeFromIndex(ref.ReflectionDataConst().data.assembly);
    }

    return false;
}

bool CLR_RT_ReflectionDef_Index::Convert(CLR_RT_HeapBlock &ref, CLR_RT_TypeDef_Instance &inst, CLR_UINT32 *levels)
{
    NATIVE_PROFILE_CLR_CORE();
    if (ref.DataType() == DATATYPE_REFLECTION)
    {
        return inst.InitializeFromReflection(ref.ReflectionDataConst(), levels);
    }

    return false;
}

bool CLR_RT_ReflectionDef_Index::Convert(CLR_RT_HeapBlock &ref, CLR_RT_MethodDef_Instance &inst)
{
    NATIVE_PROFILE_CLR_CORE();
    if (ref.DataType() == DATATYPE_REFLECTION)
    {
        switch (ref.ReflectionData().kind)
        {
            case REFLECTION_CONSTRUCTOR:
            case REFLECTION_METHOD:
                return inst.InitializeFromIndex(ref.ReflectionDataConst().data.method);
        }
    }

    return false;
}

bool CLR_RT_ReflectionDef_Index::Convert(CLR_RT_HeapBlock &ref, CLR_RT_FieldDef_Instance &inst)
{
    NATIVE_PROFILE_CLR_CORE();
    if (ref.DataType() == DATATYPE_REFLECTION && ref.ReflectionData().kind == REFLECTION_FIELD)
    {
        return inst.InitializeFromIndex(ref.ReflectionDataConst().data.field);
    }

    return false;
}

bool CLR_RT_ReflectionDef_Index::Convert(CLR_RT_HeapBlock &ref, CLR_UINT32 &hash)
{
    NATIVE_PROFILE_CLR_CORE();
    if (ref.DataType() != DATATYPE_REFLECTION)
        return false;

    hash = ref.ReflectionData().GetTypeHash();

    return hash != 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void CLR_RT_SignatureParser::Initialize_TypeSpec(CLR_RT_Assembly *assm, const CLR_RECORD_TYPESPEC *ts)
{
    NATIVE_PROFILE_CLR_CORE();
    Initialize_TypeSpec(assm, assm->GetSignature(ts->signature));
}

void CLR_RT_SignatureParser::Initialize_TypeSpec(CLR_RT_Assembly *assm, CLR_PMETADATA ts)
{
    NATIVE_PROFILE_CLR_CORE();
    Assembly = assm;
    Signature = ts;

    Type = CLR_RT_SignatureParser::c_TypeSpec;
    Flags = 0;
    ParamCount = 1;
    GenParamCount = 0;
    IsGenericInst = false;
}

//--//

void CLR_RT_SignatureParser::Initialize_Interfaces(CLR_RT_Assembly *assm, const CLR_RECORD_TYPEDEF *td)
{
    NATIVE_PROFILE_CLR_CORE();
    if (td->interfaces != CLR_EmptyIndex)
    {
        CLR_PMETADATA sig = assm->GetSignature(td->interfaces);

        ParamCount = (*sig++);
        Signature = sig;
    }
    else
    {
        ParamCount = 0;
        Signature = nullptr;
    }

    Type = CLR_RT_SignatureParser::c_Interfaces;
    Flags = 0;

    Assembly = assm;

    GenParamCount = 0;
    IsGenericInst = false;
}

//--//

void CLR_RT_SignatureParser::Initialize_FieldSignature(CLR_RT_Assembly *assm, CLR_PMETADATA fd)
{
    NATIVE_PROFILE_CLR_CORE();

    Type = CLR_RT_SignatureParser::c_Field;

    Method = 0xFFFF;

    Flags = (*fd++);

    ParamCount = 1;

    Assembly = assm;
    Signature = fd;

    GenParamCount = 0;
    IsGenericInst = false;
}

void CLR_RT_SignatureParser::Initialize_FieldDef(CLR_RT_Assembly *assm, const CLR_RECORD_FIELDDEF *fd)
{
    NATIVE_PROFILE_CLR_CORE();

    Initialize_FieldDef(assm, assm->GetSignature(fd->signature));
}

void CLR_RT_SignatureParser::Initialize_FieldDef(CLR_RT_Assembly *assm, CLR_PMETADATA fd)
{
    NATIVE_PROFILE_CLR_CORE();
    Type = CLR_RT_SignatureParser::c_Field;
    Flags = (*fd++);
    ParamCount = 1;

    Assembly = assm;
    Signature = fd;

    GenParamCount = 0;
    IsGenericInst = false;
}

//--//
void CLR_RT_SignatureParser::Initialize_MethodSignature(CLR_RT_MethodDef_Instance *md)
{
    NATIVE_PROFILE_CLR_CORE();

    Method = md->Method();

    Initialize_MethodSignature(md->assembly, md->assembly->GetSignature(md->target->signature));
}

void CLR_RT_SignatureParser::Initialize_MethodSignature(CLR_RT_Assembly *assm, const CLR_RECORD_METHODDEF *md)
{
    NATIVE_PROFILE_CLR_CORE();

    Method = 0xFFFF;

    Initialize_MethodSignature(assm, assm->GetSignature(md->signature));
}

void CLR_RT_SignatureParser::Initialize_MethodSignature(CLR_RT_Assembly *assm, CLR_PMETADATA md)
{
    NATIVE_PROFILE_CLR_CORE();

    Type = CLR_RT_SignatureParser::c_Method;

    Flags = (*md++);

    if ((Flags & PIMAGE_CEE_CS_CALLCONV_GENERIC) == PIMAGE_CEE_CS_CALLCONV_GENERIC)
    {
        // is generic instance, has generic parameters count
        GenParamCount = (*md++);
    }
    else
    {
        GenParamCount = 0;
    }

    ParamCount = (*md++) + 1;

    Assembly = assm;
    Signature = md;

    IsGenericInst = false;
}

void CLR_RT_SignatureParser::Initialize_MethodSignature(CLR_RT_MethodSpec_Instance *ms)
{
    NATIVE_PROFILE_CLR_CORE();

    Method = ms->Method();

    Signature = ms->assembly->GetSignature(ms->target->instantiation);

    Type = CLR_RT_SignatureParser::c_MethodSpec;

    Flags = (*Signature++);

    if (Flags != PIMAGE_CEE_CS_CALLCONV_GENERICINST)
    {
        // call is wrong
        return;
    }

    ParamCount = (*Signature++);

    Assembly = ms->assembly;

    GenParamCount = ParamCount;

    IsGenericInst = false;
}

//--//

bool CLR_RT_SignatureParser::Initialize_GenericParamTypeSignature(
    CLR_RT_Assembly *assm,
    const CLR_RECORD_GENERICPARAM *gp)
{
    NATIVE_PROFILE_CLR_CORE();

    Type = CLR_RT_SignatureParser::c_GenericParamType;

    Assembly = assm;

    // need to check for valid signature
    if (gp->signature != 0xFFFF)
    {
        Signature = assm->GetSignature(gp->signature);
        ParamCount = 1;
    }
    else
    {
        // can't process...
        return false;
    }

    Flags = 0;

    GenParamCount = 0;
    IsGenericInst = false;

    // done here
    return true;
}

//--//

void CLR_RT_SignatureParser::Initialize_MethodLocals(CLR_RT_Assembly *assm, const CLR_RECORD_METHODDEF *md)
{
    NATIVE_PROFILE_CLR_CORE();
    //
    // WARNING!!!
    //
    // If you change this method, change "CLR_RT_ExecutionEngine::InitializeLocals" too.
    //

    Assembly = assm;
    Signature = assm->GetSignature(md->locals);

    Type = CLR_RT_SignatureParser::c_Locals;
    Flags = 0;
    ParamCount = md->localsCount;

    GenParamCount = 0;
    IsGenericInst = false;
}

void CLR_RT_SignatureParser::Initialize_LocalVar(CLR_RT_Assembly *assm, const CLR_PMETADATA sig)
{
    NATIVE_PROFILE_CLR_CORE();

    Assembly = assm;
    Signature = sig;

    Type = CLR_RT_SignatureParser::c_Locals;
    Flags = 0;
    ParamCount = 1;

    GenParamCount = 0;
    IsGenericInst = false;
}

//--//

void CLR_RT_SignatureParser::Initialize_Objects(CLR_RT_HeapBlock *lst, int count, bool fTypes)
{
    NATIVE_PROFILE_CLR_CORE();
    ObjectList = lst;

    Type = CLR_RT_SignatureParser::c_Object;
    Flags = fTypes ? 1 : 0;
    ParamCount = count;

    GenParamCount = 0;
    IsGenericInst = false;
}

//--//

HRESULT CLR_RT_SignatureParser::Advance(Element &res)
{
    NATIVE_PROFILE_CLR_CORE();

    ///////////////////////////////////////////////////////////////////////////////
    //                                                                           //
    //                            ***** WARNING *****                            //
    //                                                                           //
    // Changes here must be ported to "CLR_RT_ExecutionEngine::InitializeLocals" //
    ///////////////////////////////////////////////////////////////////////////////

    NANOCLR_HEADER();

    ParamCount--;

    res.IsByRef = false;
    res.Levels = 0;
    res.GenericParamPosition = 0xFFFF;
    res.Class.Clear();

    switch (Type)
    {
        case c_Interfaces:
        {
            CLR_RT_TypeDef_Instance cls{};

            res.DataType = DATATYPE_CLASS;

            if (cls.ResolveToken(CLR_TkFromStream(Signature), Assembly) == false)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
            }

            res.Class = cls;
        }
        break;

        case c_Object:
        {
            CLR_RT_TypeDescriptor desc{};
            CLR_RT_HeapBlock *ptr = ObjectList++;

            if (Flags)
            {
                // Reflection types are now boxed, so unbox first
                if (ptr->DataType() == DATATYPE_OBJECT)
                {
                    ptr = ptr->Dereference();
                }
                if (ptr->DataType() != DATATYPE_REFLECTION)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
                }

                NANOCLR_CHECK_HRESULT(desc.InitializeFromReflection(ptr->ReflectionDataConst()));
            }
            else
            {
                switch (ptr->DataType())
                {
                    case DATATYPE_BYREF:
                    case DATATYPE_ARRAY_BYREF:
                        res.IsByRef = true;
                        break;

                    default:
                        // the remaining data types aren't to be handled
                        break;
                }

                NANOCLR_CHECK_HRESULT(desc.InitializeFromObject(*ptr));
            }

            desc.m_handlerCls.InitializeFromIndex(desc.m_reflex.data.type);

            res.Levels = desc.m_reflex.levels;
            res.DataType = (NanoCLRDataType)desc.m_handlerCls.target->dataType;
            res.Class = desc.m_reflex.data.type;

            //
            // Special case for Object types.
            //
            if (res.Class.data == g_CLR_RT_WellKnownTypes.Object.data)
            {
                res.DataType = DATATYPE_OBJECT;
            }
        }
        break;

        default:
            while (true)
            {
                res.DataType = CLR_UncompressElementType(Signature);

                switch (res.DataType)
                {
                    case DATATYPE_BYREF:
                        if (res.IsByRef)
                        {
                            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
                        }

                        res.IsByRef = true;
                        break;

                    case DATATYPE_SZARRAY:
                        res.Levels++;
                        break;

                    case DATATYPE_CLASS:
                    case DATATYPE_VALUETYPE:
                    {
                        CLR_UINT32 tk = CLR_TkFromStream(Signature);
                        CLR_UINT32 index = CLR_DataFromTk(tk);

                        switch (CLR_TypeFromTk(tk))
                        {
                            case TBL_TypeSpec:
                            {
                                CLR_RT_SignatureParser sub{};
                                sub.Initialize_TypeSpec(Assembly, Assembly->GetTypeSpec(index));
                                CLR_RT_SignatureParser::Element dummyElement;
                                int extraLevels = res.Levels;

                                NANOCLR_CHECK_HRESULT(sub.Advance(dummyElement));

                                res.Levels += extraLevels;
                            }
                            break;

                            case TBL_TypeRef:
                                res.Class = Assembly->crossReferenceTypeRef[index].target;
                                break;

                            case TBL_TypeDef:
                            {
                                CLR_RT_TypeDef_Instance cls;

                                if (cls.ResolveToken(tk, Assembly) == false)
                                {
                                    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
                                }

                                res.Class = cls;
                                break;
                            }

                            default:
                                NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
                        }

                        if (IsGenericInst)
                        {
                            // get generic arguments count
                            GenParamCount = (int)*Signature++;

                            // need to update the parser counter too
                            ParamCount = GenParamCount;
                        }

                        NANOCLR_SET_AND_LEAVE(S_OK);
                    }

                    case DATATYPE_OBJECT:
                        res.Class = g_CLR_RT_WellKnownTypes.Object;
                        NANOCLR_SET_AND_LEAVE(S_OK);

                    case DATATYPE_VOID:
                        res.Class = g_CLR_RT_WellKnownTypes.n_Void;
                        NANOCLR_SET_AND_LEAVE(S_OK);

                    case DATATYPE_GENERICINST:
                    {
                        // set flag for GENERICINST
                        IsGenericInst = true;
                        NANOCLR_SET_AND_LEAVE(S_OK);
                    }

                    case DATATYPE_VAR:
                        res.GenericParamPosition = (int)*Signature++;
                        NANOCLR_SET_AND_LEAVE(S_OK);

                    case DATATYPE_MVAR:
                        res.GenericParamPosition = (int)*Signature++;
                        NANOCLR_SET_AND_LEAVE(S_OK);

                    default:
                    {
                        const CLR_RT_TypeDef_Index *cls = c_CLR_RT_DataTypeLookup[res.DataType].m_cls;

                        if (cls)
                        {
                            res.Class = *cls;
                            NANOCLR_SET_AND_LEAVE(S_OK);
                        }
                        else
                        {
                            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
                        }
                    }
                }
            }
            break;
    }

    NANOCLR_NOCLEANUP();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool CLR_RT_Assembly_Instance::InitializeFromIndex(const CLR_RT_Assembly_Index &index)
{
    NATIVE_PROFILE_CLR_CORE();
    if (NANOCLR_INDEX_IS_VALID(index))
    {
        data = index.data;
        assembly = g_CLR_RT_TypeSystem.m_assemblies[Assembly() - 1];

        return true;
    }

    data = 0;
    assembly = nullptr;

    return false;
}

void CLR_RT_Assembly_Instance::ClearInstance()
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_Assembly_Index::Clear();

    assembly = nullptr;
}

//////////////////////////////

bool CLR_RT_TypeSpec_Instance::InitializeFromIndex(const CLR_RT_TypeSpec_Index &index)
{
    NATIVE_PROFILE_CLR_CORE();
    if (NANOCLR_INDEX_IS_VALID(index))
    {
        data = index.data;
        assembly = g_CLR_RT_TypeSystem.m_assemblies[Assembly() - 1];
        target = assembly->GetTypeSpec(TypeSpec());

        CLR_RT_SignatureParser parser;
        parser.Initialize_TypeSpec(assembly, assembly->GetTypeSpec(index.TypeSpec()));

        CLR_RT_SignatureParser::Element element;

        // if this is a generic, advance another one
        parser.Advance(element);

        if (element.DataType == DATATYPE_GENERICINST)
        {
            // this is a generic instance, so we need to advance one more time
            parser.Advance(element);

            genericTypeDef = element.Class;
        }

        return true;
    }

    data = 0;
    assembly = nullptr;
    target = nullptr;

    return false;
}

void CLR_RT_TypeSpec_Instance::ClearInstance()
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_TypeSpec_Index::Clear();

    assembly = nullptr;
    target = nullptr;
}

bool CLR_RT_TypeSpec_Instance::ResolveToken(CLR_UINT32 token, CLR_RT_Assembly *assm)
{
    NATIVE_PROFILE_CLR_CORE();
    if (assm && CLR_TypeFromTk(token) == TBL_TypeSpec)
    {
        CLR_UINT32 index = CLR_DataFromTk(token);

        Set(assm->assemblyIndex, index);

        assembly = assm;
        target = assm->GetTypeSpec(index);

        return true;
    }

    ClearInstance();

    return false;
}

//////////////////////////////

bool CLR_RT_TypeDef_Instance::InitializeFromReflection(const CLR_RT_ReflectionDef_Index &reflex, CLR_UINT32 *levels)
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_TypeDef_Index cls;
    const CLR_RT_TypeDef_Index *ptr = nullptr;

    if (levels)
        *levels = reflex.levels;

    switch (reflex.kind)
    {
        case REFLECTION_TYPE:
            if (reflex.levels > 0 && levels == nullptr)
            {
                ptr = &g_CLR_RT_WellKnownTypes.Array;
            }
            else
            {
                ptr = &reflex.data.type;
            }
            break;

        case REFLECTION_TYPE_DELAYED:
            if (g_CLR_RT_TypeSystem.FindTypeDef(reflex.data.raw, cls))
            {
                ptr = &cls;
            }
            break;
    }

    return ptr ? InitializeFromIndex(*ptr) : false;
}

bool CLR_RT_TypeDef_Instance::InitializeFromIndex(const CLR_RT_TypeDef_Index &index)
{
    NATIVE_PROFILE_CLR_CORE();
    if (NANOCLR_INDEX_IS_VALID(index))
    {
        data = index.data;
        assembly = g_CLR_RT_TypeSystem.m_assemblies[Assembly() - 1];
        target = assembly->GetTypeDef(Type());

#if defined(NANOCLR_INSTANCE_NAMES)
        name = assembly->GetString(target->name);
#endif

        return true;
    }

    data = 0;
    assembly = nullptr;
    target = nullptr;

    return false;
}

bool CLR_RT_TypeDef_Instance::InitializeFromMethod(const CLR_RT_MethodDef_Instance &md)
{
    NATIVE_PROFILE_CLR_CORE();
    if (NANOCLR_INDEX_IS_VALID(md))
    {
        CLR_INDEX indexAssm = md.Assembly();
        CLR_INDEX indexType = md.CrossReference().GetOwner();

        Set(indexAssm, indexType);

        assembly = g_CLR_RT_TypeSystem.m_assemblies[indexAssm - 1];
        target = assembly->GetTypeDef(indexType);

#if defined(NANOCLR_INSTANCE_NAMES)
        name = assembly->GetString(target->name);
#endif
        return true;
    }

    ClearInstance();

    return false;
}

bool CLR_RT_TypeDef_Instance::InitializeFromMethod(const CLR_RT_MethodSpec_Instance &ms)
{
    NATIVE_PROFILE_CLR_CORE();
    if (NANOCLR_INDEX_IS_VALID(ms))
    {
        CLR_INDEX indexAssm = ms.Assembly();
        // CLR_INDEX indexType = ms.CrossReference().GetOwner();

        // Set(indexAssm, indexType);

        assembly = g_CLR_RT_TypeSystem.m_assemblies[indexAssm - 1];
        // m_target = m_assm->GetTypeDef(indexType);

#if defined(NANOCLR_INSTANCE_NAMES)
        name = assembly->GetString(target->name);
#endif
        return true;
    }

    ClearInstance();

    return false;
}

bool CLR_RT_TypeDef_Instance::InitializeFromField(const CLR_RT_FieldDef_Instance &fd)
{
    NATIVE_PROFILE_CLR_CORE();
    if (NANOCLR_INDEX_IS_VALID(fd))
    {
        CLR_RT_Assembly const *assm = fd.assembly;
        auto *td = (const CLR_RECORD_TYPEDEF *)assm->GetTable(TBL_TypeDef);
        CLR_INDEX indexField = fd.Field();
        int i = assm->tablesSize[TBL_TypeDef];

        if (fd.target->flags & CLR_RECORD_FIELDDEF::FD_Static)
        {
            for (; i; i--, td++)
            {
                if (td->firstStaticField <= indexField && indexField < td->firstStaticField + td->staticFieldsCount)
                {
                    break;
                }
            }
        }
        else
        {
            for (; i; i--, td++)
            {
                if (td->firstInstanceField <= indexField &&
                    indexField < td->firstInstanceField + td->instanceFieldsCount)
                {
                    break;
                }
            }
        }

        if (i)
        {
            CLR_INDEX indexAssm = fd.Assembly();
            CLR_INDEX indexType = assm->tablesSize[TBL_TypeDef] - i;

            Set(indexAssm, indexType);

            assembly = g_CLR_RT_TypeSystem.m_assemblies[indexAssm - 1];
            target = assembly->GetTypeDef(indexType);

#if defined(NANOCLR_INSTANCE_NAMES)
            name = assembly->GetString(target->name);
#endif
            return true;
        }
    }

    ClearInstance();

    return false;
}

bool CLR_RT_TypeDef_Instance::IsATypeHandler()
{
    NATIVE_PROFILE_CLR_CORE();
    return (data == g_CLR_RT_WellKnownTypes.Type.data || data == g_CLR_RT_WellKnownTypes.TypeStatic.data);
}

void CLR_RT_TypeDef_Instance::ClearInstance()
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_TypeDef_Index::Clear();

    assembly = nullptr;
    target = nullptr;
}

// if type token is not generic, we are going to resolve from the assembly else from the heapblock that may contains
// generic parameter
bool CLR_RT_TypeDef_Instance::ResolveToken(
    CLR_UINT32 tk,
    CLR_RT_Assembly *assm,
    const CLR_RT_MethodDef_Instance *caller)
{
    NATIVE_PROFILE_CLR_CORE();
    if (assm)
    {
        CLR_UINT32 index = CLR_DataFromTk(tk);

        switch (CLR_TypeFromTk(tk))
        {
            case TBL_TypeRef:
                data = assm->crossReferenceTypeRef[index].target.data;
                assembly = g_CLR_RT_TypeSystem.m_assemblies[Assembly() - 1];
                target = assembly->GetTypeDef(Type());

#if defined(NANOCLR_INSTANCE_NAMES)
                name = assembly->GetString(target->name);
#endif

                return true;

            case TBL_TypeDef:
                Set(assm->assemblyIndex, index);

                assembly = assm;
                target = assm->GetTypeDef(index);

#if defined(NANOCLR_INSTANCE_NAMES)
                name = assembly->GetString(target->name);
#endif

                return true;

            case TBL_GenericParam:
            {
                CLR_RT_GenericParam_CrossReference gp = assm->crossReferenceGenericParam[index];

                Set(gp.classTypeDef.Assembly(), gp.classTypeDef.Type());

                assembly = g_CLR_RT_TypeSystem.m_assemblies[gp.classTypeDef.Assembly() - 1];
                target = assembly->GetTypeDef(gp.classTypeDef.Type());

#if defined(NANOCLR_INSTANCE_NAMES)
                name = assembly->GetString(target->name);
#endif

                return true;
            }

            case TBL_TypeSpec:
            {
                const CLR_RT_TypeSpec_Index typeSpecIdx = assm->crossReferenceTypeSpec[index].genericType;
                CLR_RT_TypeSpec_Instance typeSpecInstance{};
                typeSpecInstance.InitializeFromIndex(typeSpecIdx);

                if (NANOCLR_INDEX_IS_VALID(typeSpecInstance.genericTypeDef))
                {
                    // this is a generic type definition
                    data = typeSpecInstance.genericTypeDef.data;
                    assembly = g_CLR_RT_TypeSystem.m_assemblies[typeSpecInstance.genericTypeDef.Assembly() - 1];
                    target = assembly->GetTypeDef(typeSpecInstance.genericTypeDef.Type());
                }
                else
                {
                    // this a generic parameter
                    const CLR_RECORD_TYPESPEC *ts = assm->GetTypeSpec(index);
                    CLR_RT_SignatureParser parser;
                    parser.Initialize_TypeSpec(assm, ts);

                    CLR_RT_SignatureParser::Element elem;
                    if (FAILED(parser.Advance(elem)))
                    {
                        return false;
                    }

                    if (elem.Levels > 0)
                    {
                        // this is an array
                        data = elem.Class.data;
                        assembly = g_CLR_RT_TypeSystem.m_assemblies[elem.Class.Assembly() - 1];
                        target = assembly->GetTypeDef(elem.Class.Type());
                    }
                    else
                    {
                        if (elem.DataType == DATATYPE_GENERICINST)
                        {
                            if (FAILED(parser.Advance(elem)))
                            {
                                return false;
                            }

                            if (elem.DataType == DATATYPE_CLASS || elem.DataType == DATATYPE_VALUETYPE)
                            {
                                // consume the CLASS/VALUETYPE marker
                                if (FAILED(parser.Advance(elem)))
                                {
                                    return false;
                                }
                                // consume the generic‐definition token itself
                                if (FAILED(parser.Advance(elem)))
                                {
                                    return false;
                                }
                                // consume the count of generic arguments
                                if (FAILED(parser.Advance(elem)))
                                {
                                    return false;
                                }
                            }

                            // walk forward until a VAR (type‐generic) or MVAR (method‐generic) is hit
                            while (elem.DataType != DATATYPE_VAR && elem.DataType != DATATYPE_MVAR)
                            {
                                if (FAILED(parser.Advance(elem)))
                                {
                                    return false;
                                }
                            }
                        }

                        // If it's a type‐generic slot (!T), resolve against the caller's closed generic
                        if (elem.DataType == DATATYPE_VAR)
                        {
                            int pos = elem.GenericParamPosition;

                            // Use the *caller's* bound genericType (Stack<Int32>, etc.)
                            if (caller == nullptr || caller->genericType == nullptr)
                            {
                                return false;
                            }

                            auto &tsi = *caller->genericType;
                            CLR_UINT32 closedTsRow = tsi.TypeSpec();

                            CLR_RT_TypeDef_Index realTypeDef;
                            NanoCLRDataType realDataType;

                            // Only call this once to map (e.g. !T→Int32)
                            caller->assembly
                                ->FindGenericParamAtTypeSpec(closedTsRow, (CLR_UINT32)pos, realTypeDef, realDataType);

                            // populate this instance
                            data = realTypeDef.data;
                            assembly = g_CLR_RT_TypeSystem.m_assemblies[realTypeDef.Assembly() - 1];
                            target = assembly->GetTypeDef(realTypeDef.Type());
                        }

                        else if (elem.DataType == DATATYPE_MVAR)
                        {
                            // Use the caller bound genericType (Stack<Int32>, etc.)
                            if (caller == nullptr || caller->genericType == nullptr)
                            {
                                return false;
                            }

                            CLR_RT_GenericParam_Index gpIdx;
                            caller->assembly->FindGenericParamAtMethodDef(*caller, elem.GenericParamPosition, gpIdx);

                            auto &gp = caller->assembly->crossReferenceGenericParam[gpIdx.GenericParam()];

                            data = gp.classTypeDef.data;
                            assembly = g_CLR_RT_TypeSystem.m_assemblies[gp.classTypeDef.Assembly() - 1];
                            target = assembly->GetTypeDef(gp.classTypeDef.Type());
                        }
                    }
                }

#if defined(NANOCLR_INSTANCE_NAMES)
                name = assembly->GetString(target->name);
#endif

                return true;
            }

            default:

#if defined(NANOCLR_INSTANCE_NAMES)
                name = assembly->GetString(target->name);
#endif

                return true;
                // the remaining data types aren't to be handled
                break;
        }
    }

    ClearInstance();

    return false;
}

bool CLR_RT_TypeDef_Instance::ResolveNullableType(
    CLR_UINT32 tk,
    CLR_RT_Assembly *assm,
    const CLR_RT_MethodDef_Instance *caller)
{
    NATIVE_PROFILE_CLR_CORE();

    if (assm)
    {
        CLR_UINT32 index = CLR_DataFromTk(tk);

        if (CLR_TypeFromTk(tk) != TBL_TypeSpec)
        {
            // not a TypeSpec, so return false
            ClearInstance();
            return false;
        }

        // Grab the raw signature for the IL token (e.g. !T[], List`1<T>, etc.)
        const CLR_RECORD_TYPESPEC *ts = assm->GetTypeSpec(index);
        CLR_RT_SignatureParser parser;
        parser.Initialize_TypeSpec(assm, ts);

        CLR_RT_SignatureParser::Element elem;
        if (FAILED(parser.Advance(elem)))
        {
            return false;
        }

        if (elem.DataType == DATATYPE_GENERICINST)
        {
            if (FAILED(parser.Advance(elem)))
            {
                return false;
            }
        }

        if (elem.DataType != DATATYPE_VALUETYPE)
        {
            // If it's not a value type, we can't resolve it as a nullable type
            ClearInstance();
            return false;
        }

        // move to the next element in the signature
        if (FAILED(parser.Advance(elem)))
        {
            return false;
        }

        // If it's a type‐generic slot (!T), resolve against the caller's closed generic
        if (elem.DataType == DATATYPE_VAR)
        {
            int pos = elem.GenericParamPosition;

            // Use the *caller's* bound genericType (Stack<Int32>, etc.)
            if (caller == nullptr || caller->genericType == nullptr)
            {
                return false;
            }

            auto &tsi = *caller->genericType;
            CLR_UINT32 closedTsRow = tsi.TypeSpec();

            CLR_RT_TypeDef_Index realTypeDef;
            NanoCLRDataType realDataType;

            // Only call this once to map (e.g. !T→Int32)
            caller->assembly->FindGenericParamAtTypeSpec(closedTsRow, (CLR_UINT32)pos, realTypeDef, realDataType);

            // populate this instance
            data = realTypeDef.data;
            assembly = g_CLR_RT_TypeSystem.m_assemblies[realTypeDef.Assembly() - 1];
            target = assembly->GetTypeDef(realTypeDef.Type());

            return true;
        }
        else if (elem.DataType == DATATYPE_MVAR)
        {
            // Use the *caller's* bound genericType (Stack<Int32>, etc.)
            if (caller == nullptr || caller->genericType == nullptr)
            {
                return false;
            }

            CLR_RT_GenericParam_Index gpIdx;
            caller->assembly->FindGenericParamAtMethodDef(*caller, elem.GenericParamPosition, gpIdx);
            auto &gp = caller->assembly->crossReferenceGenericParam[gpIdx.GenericParam()];

            data = gp.classTypeDef.data;
            assembly = g_CLR_RT_TypeSystem.m_assemblies[gp.classTypeDef.Assembly() - 1];
            target = assembly->GetTypeDef(gp.classTypeDef.Type());

            return true;
        }
        else
        {
            // If it's a class or value type, resolve the type
            data = elem.Class.data;
            assembly = g_CLR_RT_TypeSystem.m_assemblies[elem.Class.Assembly() - 1];
            target = assembly->GetTypeDef(elem.Class.Type());

            return true;
        }
    }

    ClearInstance();

    return false;
}

//--//

bool CLR_RT_TypeDef_Instance::SwitchToParent()
{
    NATIVE_PROFILE_CLR_CORE();
    if (NANOCLR_INDEX_IS_VALID(*this))
    {
        if (target->HasValidExtendsType())
        {
            CLR_RT_TypeDef_Index tmp;
            const CLR_RT_TypeDef_Index *cls;

            switch (target->Extends())
            {
                case TBL_TypeDef:
                    tmp.Set(Assembly(), target->ExtendsIndex());
                    cls = &tmp;
                    break;

                case TBL_TypeRef:
                    cls = &assembly->crossReferenceTypeRef[target->ExtendsIndex()].target;
                    break;

                // all others are not supported
                default:
                    return false;
            }

            return InitializeFromIndex(*cls);
        }
    }

    ClearInstance();

    return false;
}

bool CLR_RT_TypeDef_Instance::HasFinalizer() const
{
    NATIVE_PROFILE_CLR_CORE();
    return NANOCLR_INDEX_IS_VALID(*this) &&
           (CrossReference().flags & CLR_RT_TypeDef_CrossReference::TD_CR_HasFinalizer);
}

//////////////////////////////

bool CLR_RT_FieldDef_Instance::InitializeFromIndex(const CLR_RT_FieldDef_Index &index)
{
    NATIVE_PROFILE_CLR_CORE();
    if (NANOCLR_INDEX_IS_VALID(index))
    {
        data = index.data;
        assembly = g_CLR_RT_TypeSystem.m_assemblies[Assembly() - 1];
        target = assembly->GetFieldDef(Field());
        genericType = nullptr;

#if defined(NANOCLR_INSTANCE_NAMES)
        name = assembly->GetString(target->name);
#endif
        return true;
    }

    data = 0;
    assembly = nullptr;
    target = nullptr;
    genericType = nullptr;

    return false;
}

void CLR_RT_FieldDef_Instance::ClearInstance()
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_FieldDef_Index::Clear();

    assembly = nullptr;
    target = nullptr;
    genericType = nullptr;
}

bool CLR_RT_FieldDef_Instance::ResolveToken(
    CLR_UINT32 tk,
    CLR_RT_Assembly *assm,
    const CLR_RT_MethodDef_Instance *caller)
{
    NATIVE_PROFILE_CLR_CORE();

    if (!assm)
    {
        return false;
    }

    CLR_UINT32 index = CLR_DataFromTk(tk);

    switch (CLR_TypeFromTk(tk))
    {
        case TBL_FieldRef:
        {
            // Find the raw FIELDREF record
            const CLR_RECORD_FIELDREF *fr = assm->GetFieldRef(index);

            switch (fr->Owner())
            {
                case TBL_TypeRef:
                    // Simple (non‐generic) field on a TypeRef
                    data = assm->crossReferenceFieldRef[index].target.data;
                    assembly = g_CLR_RT_TypeSystem.m_assemblies[Assembly() - 1];
                    target = assembly->GetFieldDef(Field());
                    genericType = nullptr; // no TypeSpec
                    break;

                case TBL_TypeSpec:
                {
                    // the metadata own (possibly open) TypeSpec...
                    const CLR_RT_TypeSpec_Index *mdTS = &assm->crossReferenceFieldRef[index].genericType;

                    // decide whether to prefer the caller’s closed-generic
                    const CLR_RT_TypeSpec_Index *effectiveTS = mdTS;
                    if (caller && caller->genericType && NANOCLR_INDEX_IS_VALID(*caller->genericType))
                    {
                        CLR_RT_TypeSpec_Instance instCaller, instMd;
                        if (instCaller.InitializeFromIndex(*caller->genericType) && instMd.InitializeFromIndex(*mdTS))
                        {
                            // check if generic definition token is the same...
                            if (instCaller.genericTypeDef.data == instMd.genericTypeDef.data)
                            {
                                // it is, therefore use the caller closed TypeSpec
                                effectiveTS = caller->genericType;
                            }
                        }
                    }

                    // now bind against effectiveTS
                    genericType = effectiveTS;

                    CLR_RT_FieldDef_Index resolved;

                    if (!assm->FindFieldDef(genericType, assm->GetString(fr->name), assm, fr->signature, resolved))
                    {
                        return false;
                    }

                    data = resolved.data;
                    assembly = assm;
                    target = assembly->GetFieldDef(Field());

                    break;
                }
                default:
                    // should not happen
                    return false;
            }

#if defined(NANOCLR_INSTANCE_NAMES)
            name = assembly->GetString(target->name);
#endif
            return true;
        }

        case TBL_FieldDef:
            // Direct FieldDef token
            Set(assm->assemblyIndex, CLR_DataFromTk(tk));
            assembly = assm;
            target = assembly->GetFieldDef(Field());
            genericType = nullptr;

#if defined(NANOCLR_INSTANCE_NAMES)
            name = assembly->GetString(target->name);
#endif
            return true;

        default:
            // Not a field token
            return false;
    }
}

//////////////////////////////

bool CLR_RT_MethodDef_Instance::InitializeFromIndex(const CLR_RT_MethodDef_Index &index)
{
    NATIVE_PROFILE_CLR_CORE();
    if (NANOCLR_INDEX_IS_VALID(index))
    {
        data = index.data;
        assembly = g_CLR_RT_TypeSystem.m_assemblies[Assembly() - 1];
        target = assembly->GetMethodDef(Method());
        genericType = nullptr;

#if defined(NANOCLR_INSTANCE_NAMES)
        name = assembly->GetString(target->name);
#endif
        return true;
    }

    data = 0;
    assembly = nullptr;
    target = nullptr;
    genericType = nullptr;

    return false;
}

bool CLR_RT_MethodDef_Instance::InitializeFromIndex(
    const CLR_RT_MethodDef_Index &md,
    const CLR_RT_TypeSpec_Index &typeSpec)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_TypeSpec_Instance tsInst;

    if (!tsInst.InitializeFromIndex(typeSpec))
    {
        return false;
    }

    // parse the signature to get to the GENERICINST and then the definition token
    CLR_RT_Assembly *tsAsm = tsInst.assembly;
    auto tsRec = tsAsm->GetTypeSpec(typeSpec.TypeSpec());
    CLR_RT_SignatureParser parser;
    parser.Initialize_TypeSpec(tsAsm, tsRec);

    CLR_RT_SignatureParser::Element elem;
    if (FAILED(parser.Advance(elem)))
    {
        return false;
    }

    // if this is a generic type, need to advance to the next
    if (elem.DataType == DATATYPE_GENERICINST)
    {
        if (FAILED(parser.Advance(elem)))
        {
            return false;
        }
    }

    CLR_RT_TypeDef_Index ownerTypeIdx;
    CLR_RT_MethodDef_Index mdRebound;

    if (elem.DataType == DATATYPE_VAR)
    {
        CLR_RT_TypeDef_Index realOwner;
        NanoCLRDataType dummyDT;

        if (!tsAsm->FindGenericParamAtTypeSpec(typeSpec.TypeSpec(), elem.GenericParamPosition, realOwner, dummyDT))
        {
            return false;
        }

        ownerTypeIdx = realOwner;
    }
    else
    {
        // elem.Class.data now contains the TypeDef/TypeRef token for the generic definition
        ownerTypeIdx.data = elem.Class.data;
    }

    // rebind the method onto the *declaring* assembly of the generic
    mdRebound.data = (ownerTypeIdx.Assembly() << 24) | md.Method();

    if (!InitializeFromIndex(mdRebound))
    {
        return false;
    }

    // remember the TypeSpec so this is available when needed
    this->genericType = &typeSpec;

    return true;
}

void CLR_RT_MethodDef_Instance::ClearInstance()
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_MethodDef_Index::Clear();

    assembly = nullptr;
    target = nullptr;
    genericType = nullptr;
}

bool CLR_RT_MethodDef_Instance::ResolveToken(
    CLR_UINT32 tk,
    CLR_RT_Assembly *assm,
    const CLR_RT_TypeSpec_Index *callerGeneric)
{
    NATIVE_PROFILE_CLR_CORE();
    if (assm)
    {
        CLR_UINT32 index = CLR_DataFromTk(tk);

        switch (CLR_TypeFromTk(tk))
        {
            case TBL_MethodRef:
            {
                const CLR_RECORD_METHODREF *mr = assm->GetMethodRef(index);

                if (mr->Owner() == TBL_TypeSpec)
                {
                    // owner is TypeSpec

                    // The raw MethodRef* says "Owner = a TypeSpec row".
                    // That TypeSpec row might be either the *open* generic or a purely nested flavor.
                    // Even if we know we are inside a closed instantiation of that same generic.
                    // We want to prefer the calling method's closed TypeSpec (callerGeneric) ONLY if they refer to the
                    // same TypeDef token.
                    //

                    // grab the MethodRef *declared* owner:
                    const CLR_RT_TypeSpec_Index *methodOwnerTS = &assm->crossReferenceMethodRef[index].genericType;

                    // check if MethodRef TypeDef token is the same TypeDef as in that caller
                    bool useCaller = false;

                    if (callerGeneric != nullptr && NANOCLR_INDEX_IS_VALID(*callerGeneric))
                    {
                        CLR_RT_TypeSpec_Instance callerInst{};
                        if (callerInst.InitializeFromIndex(*callerGeneric))
                        {
                            CLR_RT_SignatureParser parserCaller;
                            parserCaller.Initialize_TypeSpec(callerInst.assembly, callerInst.target);

                            CLR_RT_SignatureParser::Element elemCaller;

                            // advance to the generic instance which will point to the class
                            parserCaller.Advance(elemCaller);

                            CLR_UINT32 callerTypeDefToken = elemCaller.Class.data;

                            // parse the MethodRef declared TypeSpec
                            CLR_RT_TypeSpec_Instance ownerInst{};
                            if (ownerInst.InitializeFromIndex(*methodOwnerTS))
                            {
                                CLR_RT_SignatureParser parserOwner;
                                parserOwner.Initialize_TypeSpec(ownerInst.assembly, ownerInst.target);

                                CLR_RT_SignatureParser::Element elemOwner;

                                // advance to the generic instance which will point to the class
                                parserOwner.Advance(elemOwner);

                                CLR_UINT32 ownerTypeDefToken = elemOwner.Class.data;

                                if (callerTypeDefToken == ownerTypeDefToken)
                                {
                                    // we have a match on the typeDef, so they refer to the same type
                                    // lets bind using the closed generic
                                    useCaller = true;
                                }
                            }
                        }
                    }

                    // Pick the “winner” between methodOwnerTS or callerGeneric:
                    const CLR_RT_TypeSpec_Index *definitiveTypeSpec = useCaller ? callerGeneric : methodOwnerTS;
                    genericType = (CLR_RT_TypeSpec_Index *)definitiveTypeSpec;

                    CLR_INDEX tsAsmIdx = genericType->Assembly();
                    CLR_RT_Assembly *genericTypeAssembly = g_CLR_RT_TypeSystem.m_assemblies[tsAsmIdx - 1];

                    const CLR_RECORD_TYPESPEC *ts = genericTypeAssembly->GetTypeSpec(definitiveTypeSpec->TypeSpec());
                    CLR_UINT32 assemblyIndex = 0xFFFF;
                    CLR_RT_MethodDef_Index methodIndex;

                    if (!genericTypeAssembly->FindMethodDef(
                            ts,
                            genericTypeAssembly->GetString(mr->name),
                            genericTypeAssembly,
                            mr->signature,
                            methodIndex,
                            assemblyIndex))
                    {
                        return false;
                    }

                    Set(assemblyIndex, methodIndex.Method());
                    assembly = g_CLR_RT_TypeSystem.m_assemblies[assemblyIndex - 1];
                    target = assembly->GetMethodDef(methodIndex.Method());
                }
                else
                {
                    // owner is TypeRef

                    // get data for MethodRef (from index)
                    data = assm->crossReferenceMethodRef[index].target.data;
                    // get assembly for this type ref
                    assembly = g_CLR_RT_TypeSystem.m_assemblies[Assembly() - 1];
                    // grab the MethodDef
                    target = assembly->GetMethodDef(Method());

                    // invalidate GenericType
                    genericType = nullptr;
                }

#if defined(NANOCLR_INSTANCE_NAMES)
                name = assembly->GetString(target->name);
#endif
                return true;
            }

            case TBL_MethodDef:
                Set(assm->assemblyIndex, index);

                assembly = assm;
                target = assembly->GetMethodDef(index);

                // invalidate generic type
                genericType = nullptr;

#if defined(NANOCLR_INSTANCE_NAMES)
                name = assembly->GetString(target->name);
#endif
                return true;

            case TBL_MethodSpec:
            {
                assembly = assm;

                const CLR_RECORD_METHODSPEC *ms = assembly->GetMethodSpec(index);

                CLR_RT_MethodSpec_Index msIndex;
                msIndex.Set(assembly->assemblyIndex, index);

                switch (ms->MethodKind())
                {
                    case TBL_MethodDef:
                        Set(assembly->assemblyIndex, ms->MethodIndex());
                        assembly = assm;
                        target = assembly->GetMethodDef(ms->MethodIndex());
                        break;

                    case TBL_MethodRef:
                        data = assm->crossReferenceMethodRef[ms->MethodIndex()].target.data;
                        assembly = g_CLR_RT_TypeSystem.m_assemblies[Assembly() - 1];
                        target = assembly->GetMethodDef(Method());

                        break;

                    default:
                        // shouldn't be here
                        return false;
                }

                // get generic type
                genericType = &assembly->crossReferenceTypeSpec[ms->container].genericType;

#if defined(NANOCLR_INSTANCE_NAMES)
                name = assembly->GetString(target->name);
#endif
                return true;
            }
            case TBL_TypeSpec:
            {
                CLR_RT_MethodSpec_Index methodSpec;
                assm->FindMethodSpecFromTypeSpec(index, methodSpec);

                const CLR_RECORD_METHODSPEC *ms = assm->GetMethodSpec(index);

                switch (ms->MethodKind())
                {
                    case TBL_MethodDef:
                        Set(assm->assemblyIndex, ms->MethodIndex());

                        assembly = assm;
                        target = assembly->GetMethodDef(ms->MethodIndex());

#if defined(NANOCLR_INSTANCE_NAMES)
                        name = assembly->GetString(target->name);
#endif
                        return true;

                    case TBL_MethodRef:
                        data = assm->crossReferenceMethodRef[ms->MethodIndex()].target.data;
                        assembly = g_CLR_RT_TypeSystem.m_assemblies[Assembly() - 1];
                        target = assembly->GetMethodDef(Method());

#if defined(NANOCLR_INSTANCE_NAMES)
                        name = assembly->GetString(target->name);
#endif

                        return true;

                    default:
                        // shouldn't be here
                        return false;
                }
                break;

                // get generic type
                genericType = &assembly->crossReferenceTypeSpec[ms->container].genericType;

#if defined(NANOCLR_INSTANCE_NAMES)
                name = assembly->GetString(target->name);
#endif
                return true;
            }
            default:
                // the remaining data types aren't to be handled
                break;
        }
    }

    ClearInstance();

    return false;
}

bool CLR_RT_MethodDef_Instance::GetDeclaringType(CLR_RT_TypeDef_Instance &declType) const
{
    NATIVE_PROFILE_CLR_CORE();

    if (genericType && NANOCLR_INDEX_IS_VALID(*genericType))
    {
        // Look up the assembly that actually owns that TypeSpec
        auto tsAsm = g_CLR_RT_TypeSystem.m_assemblies[genericType->Assembly() - 1];
        auto tsRec = tsAsm->GetTypeSpec(genericType->TypeSpec());

        // Parse the signature so we can peel off [GENERICINST CLASS <type> <args>].
        CLR_RT_SignatureParser parser;
        parser.Initialize_TypeSpec(tsAsm, tsRec);

        CLR_RT_SignatureParser::Element elem;
        parser.Advance(elem);

        // if this a generic, advance again to get the type
        if (elem.DataType == DATATYPE_GENERICINST)
        {
            parser.Advance(elem);
        }

        return declType.InitializeFromIndex(elem.Class);
    }
    else
    {
        // Normal (non‐generic or open‐generic)
        return declType.InitializeFromMethod(*this);
    }
}

//////////////////////////////

bool CLR_RT_GenericParam_Instance::InitializeFromIndex(const CLR_RT_GenericParam_Index &index)
{
    NATIVE_PROFILE_CLR_CORE();

    if (NANOCLR_INDEX_IS_VALID(index))
    {
        data = index.data;
        assembly = g_CLR_RT_TypeSystem.m_assemblies[Assembly() - 1];
        target = assembly->GetGenericParam(GenericParam());

#if defined(NANOCLR_INSTANCE_NAMES)
        name = assembly->GetString(target->name);
#endif

        return true;
    }

    data = 0;
    assembly = nullptr;
    target = nullptr;

    return false;
}

void CLR_RT_GenericParam_Instance::ClearInstance()
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_GenericParam_Index::Clear();

    assembly = nullptr;
    target = nullptr;
}

bool CLR_RT_GenericParam_Instance::ResolveToken(CLR_UINT32 tk, CLR_RT_Assembly *assm)
{
    NATIVE_PROFILE_CLR_CORE();
    if (assm)
    {
        CLR_UINT32 index = CLR_DataFromTk(tk);

        CLR_RT_GenericParam_Index genericParamIndex;
        genericParamIndex.Set(assm->assemblyIndex, index);

        // get generic type
        data = genericParamIndex.data;
        assembly = assm;
        target = assembly->GetGenericParam(index);

#if defined(NANOCLR_INSTANCE_NAMES)
        name = assembly->GetString(target->name);
#endif
        return true;
    }

    ClearInstance();

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool CLR_RT_MethodSpec_Instance::InitializeFromIndex(const CLR_RT_MethodSpec_Index &index)
{
    NATIVE_PROFILE_CLR_CORE();

    if (NANOCLR_INDEX_IS_VALID(index))
    {
        data = index.data;
        assembly = g_CLR_RT_TypeSystem.m_assemblies[Assembly() - 1];
        target = assembly->GetMethodSpec(Method());

        return true;
    }

    data = 0;
    assembly = nullptr;
    target = nullptr;

    return false;
}

void CLR_RT_MethodSpec_Instance::ClearInstance()
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_MethodSpec_Index::Clear();

    assembly = nullptr;
    target = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void CLR_RT_TypeDescriptor::TypeDescriptor_Initialize()
{
    NATIVE_PROFILE_CLR_CORE();
    m_flags = 0;
    m_handlerCls.ClearInstance();
    m_handlerGenericType.ClearInstance();
    m_reflex.Clear();
}

HRESULT CLR_RT_TypeDescriptor::InitializeFromDataType(NanoCLRDataType dt)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    if (dt >= DATATYPE_FIRST_INVALID)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }
    else
    {
        const CLR_RT_DataTypeLookup &dtl = c_CLR_RT_DataTypeLookup[dt];

        m_flags = dtl.m_flags & CLR_RT_DataTypeLookup::c_SemanticMask2;

        if (dtl.m_cls)
        {
            if (m_handlerCls.InitializeFromIndex(*dtl.m_cls) == false)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
            }

            m_reflex.kind = REFLECTION_TYPE;
            m_reflex.levels = 0;
            m_reflex.data.type = *dtl.m_cls;
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }
    }

    m_handlerGenericType.ClearInstance();

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_TypeDescriptor::InitializeFromReflection(const CLR_RT_ReflectionDef_Index &reflex)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Instance inst{};
    CLR_UINT32 levels;

    if (inst.InitializeFromReflection(reflex, &levels) == false)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    NANOCLR_CHECK_HRESULT(InitializeFromType(inst));

    if (levels)
    {
        m_reflex.levels = levels;

        ConvertToArray();
    }

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_TypeDescriptor::InitializeFromTypeSpec(const CLR_RT_TypeSpec_Index &sig)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeSpec_Instance inst{};
    CLR_RT_SignatureParser parser{};

    if (inst.InitializeFromIndex(sig) == false)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    parser.Initialize_TypeSpec(inst.assembly, inst.target);

    NANOCLR_SET_AND_LEAVE(InitializeFromSignatureParser(parser));

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_TypeDescriptor::InitializeFromType(const CLR_RT_TypeDef_Index &cls)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    if (m_handlerCls.InitializeFromIndex(cls) == false)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }
    else
    {
        const CLR_RT_DataTypeLookup &dtl = c_CLR_RT_DataTypeLookup[m_handlerCls.target->dataType];

        m_flags = dtl.m_flags & CLR_RT_DataTypeLookup::c_SemanticMask;

        m_reflex.kind = REFLECTION_TYPE;
        m_reflex.levels = 0;
        m_reflex.data.type = m_handlerCls;

        if (m_flags == CLR_RT_DataTypeLookup::c_Primitive)
        {
            if ((m_handlerCls.target->flags & CLR_RECORD_TYPEDEF::TD_Semantics_Mask) ==
                CLR_RECORD_TYPEDEF::TD_Semantics_Enum)
            {
                m_flags = CLR_RT_DataTypeLookup::c_Enum;
            }
        }
        else
        {
            switch (m_handlerCls.target->flags & CLR_RECORD_TYPEDEF::TD_Semantics_Mask)
            {
                case CLR_RECORD_TYPEDEF::TD_Semantics_ValueType:
                    m_flags = CLR_RT_DataTypeLookup::c_ValueType;
                    break;
                case CLR_RECORD_TYPEDEF::TD_Semantics_Class:
                    m_flags = CLR_RT_DataTypeLookup::c_Class;
                    break;
                case CLR_RECORD_TYPEDEF::TD_Semantics_Interface:
                    m_flags = CLR_RT_DataTypeLookup::c_Interface;
                    break;
                case CLR_RECORD_TYPEDEF::TD_Semantics_Enum:
                    m_flags = CLR_RT_DataTypeLookup::c_Enum;
                    break;
            }
        }

        if (m_handlerCls.data == g_CLR_RT_WellKnownTypes.Array.data)
        {
            m_flags |= CLR_RT_DataTypeLookup::c_Array;
        }

        if (m_handlerCls.data == g_CLR_RT_WellKnownTypes.ArrayList.data)
        {
            m_flags |= CLR_RT_DataTypeLookup::c_ArrayList;
        }
    }

    m_handlerGenericType.ClearInstance();

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_TypeDescriptor::InitializeFromTypeDef(const CLR_RT_TypeDef_Index &cls)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();
    if (m_handlerCls.InitializeFromIndex(cls) == false)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }
    else
    {
        m_flags = CLR_RT_DataTypeLookup::c_ManagedType;
        m_reflex.kind = REFLECTION_TYPE;
        m_reflex.levels = 0;
        m_reflex.data.type = m_handlerCls;
    }
    m_handlerGenericType.ClearInstance();
    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_TypeDescriptor::InitializeFromGenericType(const CLR_RT_TypeSpec_Index &genericType)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    if (m_handlerGenericType.InitializeFromIndex(genericType) == false)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }
    else
    {
        m_flags = CLR_RT_DataTypeLookup::c_ManagedType | CLR_RT_DataTypeLookup::c_GenericInstance;

        m_reflex.kind = REFLECTION_GENERICTYPE;
        m_reflex.data.genericType = m_handlerGenericType;
    }

    m_handlerCls.ClearInstance();

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_TypeDescriptor::InitializeFromFieldDefinition(const CLR_RT_FieldDef_Instance &fd)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_SignatureParser parser{};
    parser.Initialize_FieldDef(fd.assembly, fd.target);

    NANOCLR_SET_AND_LEAVE(InitializeFromSignatureParser(parser));

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_TypeDescriptor::InitializeFromSignatureParser(CLR_RT_SignatureParser &parser)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_SignatureParser::Element res;

    if (parser.Available() <= 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    NANOCLR_CHECK_HRESULT(parser.Advance(res));

    if (res.DataType == DATATYPE_GENERICINST)
    {
        NANOCLR_CHECK_HRESULT(InitializeFromGenericType(res.TypeSpec));
    }
    else
    {
        NANOCLR_CHECK_HRESULT(InitializeFromType(res.Class));
    }

    if (res.Levels)
    {
        m_reflex.levels = res.Levels;

        ConvertToArray();
    }

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_TypeDescriptor::InitializeFromSignatureToken(
    CLR_RT_Assembly *assm,
    CLR_UINT32 token,
    const CLR_RT_MethodDef_Instance *caller)
{
    NANOCLR_HEADER();

    // Peel the compressed token to find out which table and index we're in
    NanoCLRTable dataTable = CLR_TypeFromTk(token);
    CLR_UINT32 idx = CLR_DataFromTk(token);

    switch (dataTable)
    {
        case TBL_TypeRef:
        case TBL_TypeDef:
        {
            CLR_RT_TypeDef_Instance tdInst;
            if (!tdInst.ResolveToken(token, assm, caller))
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
            }
            this->InitializeFromTypeDef(tdInst);
            break;
        }

        case TBL_TypeSpec:
        {
            // could be SZARRAY, VAR, MVAR or full GENERICINST
            const CLR_RECORD_TYPESPEC *ts = assm->GetTypeSpec(idx);
            CLR_RT_SignatureParser parser;
            parser.Initialize_TypeSpec(assm, ts);

            CLR_RT_SignatureParser::Element elem;
            parser.Advance(elem);

            if (elem.DataType == DATATYPE_VAR)
            {
                // !T: ask the CLR to map that slot into the *actual* argument
                CLR_RT_TypeDef_Index td;
                NanoCLRDataType dt;
                assm->FindGenericParamAtTypeSpec(caller->genericType->TypeSpec(), elem.GenericParamPosition, td, dt);
                this->InitializeFromTypeDef(td);
            }
            else if (elem.DataType == DATATYPE_MVAR)
            {
                // !!U: method-generic
                CLR_RT_GenericParam_Index gp;
                assm->FindGenericParamAtMethodDef(*caller, elem.GenericParamPosition, gp);
                auto &info = assm->crossReferenceGenericParam[gp.GenericParam()];
                this->InitializeFromTypeDef(info.classTypeDef);
            }
            else if (elem.DataType == DATATYPE_GENERICINST)
            {
                // full generic instantiation: read it out
                // CLASS/VALUETYPE
                parser.Advance(elem);
                // generic-definition token
                parser.Advance(elem);

                CLR_RT_TypeSpec_Index tsInst{};
                tsInst.Set(elem.Class.Assembly(), elem.Class.Type());

                // argument count
                parser.Advance(elem);

                // now read each argument and record in tsInst.m_data.GenericArguments
                this->InitializeFromTypeSpec(tsInst);
            }
            else
            {
                // e.g. SZARRAY, VALUETYPE, CLASS
                this->InitializeFromSignatureParser(parser);
            }
            break;
        }

        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_NOTIMPL);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_TypeDescriptor::InitializeFromObject(const CLR_RT_HeapBlock &ref)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    const CLR_RT_HeapBlock *obj = &ref;
    NanoCLRDataType dt;

    while (true)
    {
        dt = (NanoCLRDataType)obj->DataType();

        if (dt == DATATYPE_BYREF || dt == DATATYPE_OBJECT)
        {
            obj = obj->Dereference();
            FAULT_ON_NULL(obj);
        }
#if defined(NANOCLR_APPDOMAINS)
        else if (dt == DATATYPE_TRANSPARENT_PROXY)
        {
            NANOCLR_CHECK_HRESULT(obj->TransparentProxyValidate());
            obj = obj->TransparentProxyDereference();
        }
#endif
        else
        {
            break;
        }
    }

    {
        const CLR_RT_TypeDef_Index *cls = nullptr;
        const CLR_RT_ReflectionDef_Index *reflex = nullptr;
        const CLR_RT_TypeSpec_Index *genericType = nullptr;

        switch (dt)
        {
            case DATATYPE_SZARRAY:
                reflex = &obj->ReflectionDataConst();
                cls = &reflex->data.type;
                break;

            case DATATYPE_VALUETYPE:
            case DATATYPE_CLASS:
                cls = &obj->ObjectCls();
                break;

            case DATATYPE_DELEGATE_HEAD:
            {
                auto *dlg = (const CLR_RT_HeapBlock_Delegate *)obj;

                cls = NANOCLR_INDEX_IS_VALID(dlg->m_cls) ? &dlg->m_cls : &g_CLR_RT_WellKnownTypes.Delegate;
            }
            break;

            case DATATYPE_DELEGATELIST_HEAD:
            {
                auto *dlgLst = (const CLR_RT_HeapBlock_Delegate_List *)obj;

                cls =
                    NANOCLR_INDEX_IS_VALID(dlgLst->m_cls) ? &dlgLst->m_cls : &g_CLR_RT_WellKnownTypes.MulticastDelegate;
            }
            break;

                //--//

            case DATATYPE_WEAKCLASS:
            {
                cls = &g_CLR_RT_WellKnownTypes.WeakReference;
            }
            break;

                //--//

            case DATATYPE_REFLECTION:
                reflex = &(obj->ReflectionDataConst());

                switch (reflex->kind)
                {
                    case REFLECTION_ASSEMBLY:
                        cls = &g_CLR_RT_WellKnownTypes.Assembly;
                        break;
                    case REFLECTION_TYPE:
                        cls = &g_CLR_RT_WellKnownTypes.Type;
                        break;
                    case REFLECTION_TYPE_DELAYED:
                        cls = &g_CLR_RT_WellKnownTypes.Type;
                        break;
                    case REFLECTION_CONSTRUCTOR:
                        cls = &g_CLR_RT_WellKnownTypes.ConstructorInfo;
                        break;
                    case REFLECTION_METHOD:
                        cls = &g_CLR_RT_WellKnownTypes.MethodInfo;
                        break;
                    case REFLECTION_FIELD:
                        cls = &g_CLR_RT_WellKnownTypes.FieldInfo;
                        break;
                }

                break;

                //--//

            case DATATYPE_ARRAY_BYREF:
            {
                CLR_RT_HeapBlock_Array *array = obj->Array();
                FAULT_ON_NULL(array);

                if (array->m_fReference)
                {
                    obj = (CLR_RT_HeapBlock *)array->GetElement(obj->ArrayIndex());

                    NANOCLR_SET_AND_LEAVE(InitializeFromObject(*obj));
                }

                reflex = &array->ReflectionDataConst();
                cls = &reflex->data.type;
            }
            break;

            case DATATYPE_GENERICINST:
                genericType = &obj->ObjectGenericType();
                break;

                //--//

            default:
                NANOCLR_SET_AND_LEAVE(InitializeFromDataType(dt));
        }

        if (cls)
        {
            NANOCLR_CHECK_HRESULT(InitializeFromType(*cls));
        }

        if (reflex)
        {
            m_reflex = *reflex;
        }

        if (genericType)
        {
            NANOCLR_CHECK_HRESULT(InitializeFromGenericType(*genericType));
        }

        if (dt == DATATYPE_SZARRAY)
        {
            ConvertToArray();
        }
    }

    NANOCLR_NOCLEANUP();
}

////////////////////////////////////////

void CLR_RT_TypeDescriptor::ConvertToArray()
{
    NATIVE_PROFILE_CLR_CORE();
    m_flags &= CLR_RT_DataTypeLookup::c_SemanticMask;
    m_flags |= CLR_RT_DataTypeLookup::c_Array;

    m_handlerCls.InitializeFromIndex(g_CLR_RT_WellKnownTypes.Array);
}

bool CLR_RT_TypeDescriptor::ShouldEmitHash()
{
    NATIVE_PROFILE_CLR_CORE();
    if (m_flags & (CLR_RT_DataTypeLookup::c_Array | CLR_RT_DataTypeLookup::c_ArrayList))
    {
        return true;
    }

    if (m_flags &
        (CLR_RT_DataTypeLookup::c_Primitive | CLR_RT_DataTypeLookup::c_ValueType | CLR_RT_DataTypeLookup::c_Enum))
    {
        return false;
    }

    if (m_handlerCls.CrossReference().hash != 0)
    {
        return true;
    }

    return false;
}

bool CLR_RT_TypeDescriptor::GetElementType(CLR_RT_TypeDescriptor &sub)
{
    NATIVE_PROFILE_CLR_CORE();
    switch (m_reflex.levels)
    {
        case 0:
            return false;

        case 1:
            sub.InitializeFromType(m_reflex.data.type);
            break;

        default:
            sub = *this;
            sub.m_reflex.levels--;
            break;
    }

    return true;
}

////////////////////////////////////////

HRESULT CLR_RT_TypeDescriptor::ExtractObjectAndDataType(CLR_RT_HeapBlock *&ref, NanoCLRDataType &dt)
{
    NATIVE_PROFILE_CLR_CORE();

    NANOCLR_HEADER();

    while (true)
    {
        dt = (NanoCLRDataType)ref->DataType();

        if (dt == DATATYPE_BYREF || dt == DATATYPE_OBJECT)
        {
            ref = ref->Dereference();
            FAULT_ON_NULL(ref);
        }
        else
        {
            break;
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_TypeDescriptor::ExtractTypeIndexFromObject(const CLR_RT_HeapBlock &ref, CLR_RT_TypeDef_Index &res)
{
    NATIVE_PROFILE_CLR_CORE();

    NANOCLR_HEADER();

    auto *obj = (CLR_RT_HeapBlock *)&ref;
    NanoCLRDataType dt;

    NANOCLR_CHECK_HRESULT(CLR_RT_TypeDescriptor::ExtractObjectAndDataType(obj, dt));

    if (dt == DATATYPE_VALUETYPE || dt == DATATYPE_CLASS)
    {
        res = obj->ObjectCls();
    }
    else
    {
        const CLR_RT_DataTypeLookup &dtl = c_CLR_RT_DataTypeLookup[dt];

        if (dtl.m_cls)
        {
            res = *dtl.m_cls;
        }
        else
        {
            res.Clear();
        }
    }

    if (NANOCLR_INDEX_IS_INVALID(res))
    {
        CLR_RT_TypeDescriptor desc{};

        NANOCLR_CHECK_HRESULT(desc.InitializeFromObject(ref))

        // If desc.InitializeFromObject( ref ) call succeed, then we need to find out what to call

        if (desc.GetDataType() == DATATYPE_GENERICINST)
        {
            res.Set(desc.m_handlerGenericType.Assembly(), desc.m_handlerGenericType.genericTypeDef.Type());
        }
        else
        {
            // use m_handlerCls for res
            res = desc.m_handlerCls;
        }

        if (NANOCLR_INDEX_IS_INVALID(res))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }
    }

    NANOCLR_NOCLEANUP();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////

// Keep these strings less than 8-character long (including terminator) because it's stuffed into an 8-byte structure.
// static const char c_MARKER_ASSEMBLY_V1[] = "NFMRK1";
static const char c_MARKER_ASSEMBLY_V2[] = "NFMRK2";

bool CLR_RECORD_ASSEMBLY::ValidateMarker() const
{
    NATIVE_PROFILE_CLR_CORE();

    // compare the marker
    return memcmp(marker, c_MARKER_ASSEMBLY_V2, sizeof(c_MARKER_ASSEMBLY_V2)) == 0;
}

/// @brief Check for valid assembly header (CRC32 of header, string table version and marker)
///
/// @return Check result
bool CLR_RECORD_ASSEMBLY::GoodHeader() const
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RECORD_ASSEMBLY header = *this;
    header.headerCRC = 0;

    if (SUPPORT_ComputeCRC(&header, sizeof(header), 0) != this->headerCRC)
    {
        return false;
    }

    if (this->stringTableVersion != c_CLR_StringTable_Version)
    {
        return false;
    }

    return ValidateMarker();
}

/// @brief Check for valid assembly (header and CRC32 of assembly content)
///
/// @return bool Check result
bool CLR_RECORD_ASSEMBLY::GoodAssembly() const
{
    NATIVE_PROFILE_CLR_CORE();
    if (!GoodHeader())
    {
        return false;
    }

    return SUPPORT_ComputeCRC(&this[1], this->TotalSize() - sizeof(*this), 0) == this->assemblyCRC;
}

#if defined(VIRTUAL_DEVICE)

void CLR_RECORD_ASSEMBLY::ComputeCRC()
{
    NATIVE_PROFILE_CLR_CORE();
    memcpy(marker, c_MARKER_ASSEMBLY_V2, sizeof(c_MARKER_ASSEMBLY_V2));

    headerCRC = 0;
    assemblyCRC = SUPPORT_ComputeCRC(&this[1], this->TotalSize() - sizeof(*this), 0);
    headerCRC = SUPPORT_ComputeCRC(this, sizeof(*this), 0);
}

#endif

CLR_UINT32 CLR_RECORD_ASSEMBLY::ComputeAssemblyHash(const char *name, const CLR_RECORD_VERSION &ver)
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_UINT32 assemblyHASH;

    assemblyHASH = SUPPORT_ComputeCRC(name, (int)hal_strlen_s(name), 0);
    assemblyHASH = SUPPORT_ComputeCRC(&ver, sizeof(ver), assemblyHASH);

    return assemblyHASH;
}

//--//

CLR_PMETADATA CLR_RECORD_EH::ExtractEhFromByteCode(CLR_PMETADATA ipEnd, const CLR_RECORD_EH *&ptrEh, CLR_UINT32 &numEh)
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_UINT32 num = *(--ipEnd);
    ipEnd -= sizeof(CLR_RECORD_EH) * num;

    numEh = num;
    ptrEh = (const CLR_RECORD_EH *)ipEnd;

    return ipEnd;
}

CLR_UINT32 CLR_RECORD_EH::GetToken() const
{
    NATIVE_PROFILE_CLR_CORE();

    return CLR_UncompressTypeToken(classToken);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool CLR_RT_ExceptionHandler::ConvertFromEH(
    const CLR_RT_MethodDef_Instance &owner,
    CLR_PMETADATA ipStart,
    const CLR_RECORD_EH *ehPtr)
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RECORD_EH eh;
    memcpy(&eh, ehPtr, sizeof(eh));

    switch (eh.mode)
    {
        case CLR_RECORD_EH::EH_Finally:
            m_typeFilter.Clear();
            break;

        case CLR_RECORD_EH::EH_Filter:
            m_userFilterStart = ipStart + eh.filterStart;
            break;

        case CLR_RECORD_EH::EH_CatchAll:
            m_typeFilter = g_CLR_RT_WellKnownTypes.Object;
            break;

        case CLR_RECORD_EH::EH_Catch:
        {
            CLR_RT_TypeDef_Instance cls{};
            if (cls.ResolveToken(eh.GetToken(), owner.assembly) == false)
                return false;
            m_typeFilter = cls;
        }
        break;

        default:
            return false;
    }

    if (owner.target->rva == CLR_EmptyIndex)
        return false;

    m_ehType = eh.mode;
    m_tryStart = ipStart + eh.tryStart;
    m_tryEnd = ipStart + eh.tryEnd;
    m_handlerStart = ipStart + eh.handlerStart;
    m_handlerEnd = ipStart + eh.handlerEnd;

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool CLR_RT_Assembly::IsSameAssembly(const CLR_RT_Assembly &assm) const
{
    if (header->headerCRC == assm.header->headerCRC && header->assemblyCRC == assm.header->assemblyCRC)
    {
        return true;
    }

    return false;
}

void CLR_RT_Assembly::AssemblyInitialize(CLR_RT_Assembly::Offsets &offsets)
{
    NATIVE_PROFILE_CLR_CORE();
    auto *buffer = (CLR_UINT8 *)this;
    int i;

    name = GetString(header->assemblyName);

    //--//
    buffer += offsets.base;
    crossReferenceAssemblyRef = (CLR_RT_AssemblyRef_CrossReference *)buffer;
    buffer += offsets.assemblyRef;
    crossReferenceTypeRef = (CLR_RT_TypeRef_CrossReference *)buffer;
    buffer += offsets.typeRef;
    crossReferenceFieldRef = (CLR_RT_FieldRef_CrossReference *)buffer;
    buffer += offsets.fieldRef;
    crossReferenceMethodRef = (CLR_RT_MethodRef_CrossReference *)buffer;
    buffer += offsets.methodRef;
    crossReferenceTypeDef = (CLR_RT_TypeDef_CrossReference *)buffer;
    buffer += offsets.typeDef;
    crossReferenceFieldDef = (CLR_RT_FieldDef_CrossReference *)buffer;
    buffer += offsets.fieldDef;
    crossReferenceMethodDef = (CLR_RT_MethodDef_CrossReference *)buffer;
    buffer += offsets.methodDef;
    crossReferenceGenericParam = (CLR_RT_GenericParam_CrossReference *)buffer;
    buffer += offsets.genericParam;
    crossReferenceMethodSpec = (CLR_RT_MethodSpec_CrossReference *)buffer;
    buffer += offsets.methodSpec;
    crossReferenceTypeSpec = (CLR_RT_TypeSpec_CrossReference *)buffer;
    buffer += offsets.typeSpec;

#if !defined(NANOCLR_APPDOMAINS)
    staticFields = (CLR_RT_HeapBlock *)buffer;
    buffer += offsets.staticFieldsCount;

    memset(staticFields, 0, offsets.staticFieldsCount);
#endif

    //--//

    {
        const auto *src = (const CLR_RECORD_TYPEDEF *)this->GetTable(TBL_TypeDef);
        CLR_RT_TypeDef_CrossReference *dst = this->crossReferenceTypeDef;
        for (i = 0; i < this->tablesSize[TBL_TypeDef]; i++, src++, dst++)
        {
            dst->flags = 0;
            dst->totalFields = 0;
            dst->hash = 0;
        }
    }

    {
        auto *src = (const CLR_RECORD_FIELDDEF *)this->GetTable(TBL_FieldDef);
        CLR_RT_FieldDef_CrossReference *dst = this->crossReferenceFieldDef;
        for (i = 0; i < this->tablesSize[TBL_FieldDef]; i++, src++, dst++)
        {
            dst->offset = CLR_EmptyIndex;
        }
    }

    {
        auto *src = (const CLR_RECORD_METHODDEF *)this->GetTable(TBL_MethodDef);
        CLR_RT_MethodDef_CrossReference *dst = this->crossReferenceMethodDef;
        for (i = 0; i < this->tablesSize[TBL_MethodDef]; i++, src++, dst++)
        {
            dst->data = CLR_EmptyIndex;
        }
    }

    {
        auto *src = (const CLR_RECORD_GENERICPARAM *)this->GetTable(TBL_GenericParam);
        CLR_RT_GenericParam_CrossReference *dst = this->crossReferenceGenericParam;
        for (i = 0; i < this->tablesSize[TBL_GenericParam]; i++, src++, dst++)
        {
            dst->data = CLR_EmptyIndex;
        }
    }

    {
        auto *src = (const CLR_RECORD_METHODSPEC *)this->GetTable(TBL_MethodSpec);
        CLR_RT_MethodSpec_CrossReference *dst = this->crossReferenceMethodSpec;
        for (i = 0; i < this->tablesSize[TBL_MethodSpec]; i++, src++, dst++)
        {
            dst->data = CLR_EmptyIndex;
        }
    }

    {
        auto *src = (const CLR_RECORD_TYPESPEC *)this->GetTable(TBL_TypeSpec);
        CLR_RT_TypeSpec_CrossReference *dst = this->crossReferenceTypeSpec;
        for (i = 0; i < this->tablesSize[TBL_TypeSpec]; i++, src++, dst++)
        {
            dst->genericType.data = 0;
        }
    }

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
    {
        debuggingInfoMethodDef = (CLR_RT_MethodDef_DebuggingInfo *)buffer;
        buffer += offsets.debuggingInfoMethods;

        memset(debuggingInfoMethodDef, 0, offsets.debuggingInfoMethods);
    }
#endif // #if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
}

HRESULT CLR_RT_Assembly::CreateInstance(const CLR_RECORD_ASSEMBLY *header, CLR_RT_Assembly *&assm)
{
    NATIVE_PROFILE_CLR_CORE();
    //
    // We have to use this trick, otherwise the C++ compiler will try to all the constructor for Assembly.
    //
    NANOCLR_HEADER();

    CLR_UINT8 buf[sizeof(CLR_RT_Assembly)];
    auto *skeleton = (CLR_RT_Assembly *)buf;

    NANOCLR_CLEAR(*skeleton);

    if (header->GoodAssembly() == false)
    {
        NANOCLR_MSG_SET_AND_LEAVE(CLR_E_FAIL, L"Failed in type system: assembly is not good.\n");
    }

    skeleton->header = header;

    // Compute overall size for assembly data structure.
    {
        for (uint32_t i = 0; i < ARRAYSIZE(skeleton->tablesSize) - 1; i++)
        {
            skeleton->tablesSize[i] = header->SizeOfTable((NanoCLRTable)i);
        }

        skeleton->tablesSize[TBL_AssemblyRef] /= sizeof(CLR_RECORD_ASSEMBLYREF);
        skeleton->tablesSize[TBL_TypeRef] /= sizeof(CLR_RECORD_TYPEREF);
        skeleton->tablesSize[TBL_FieldRef] /= sizeof(CLR_RECORD_FIELDREF);
        skeleton->tablesSize[TBL_MethodRef] /= sizeof(CLR_RECORD_METHODREF);
        skeleton->tablesSize[TBL_TypeDef] /= sizeof(CLR_RECORD_TYPEDEF);
        skeleton->tablesSize[TBL_FieldDef] /= sizeof(CLR_RECORD_FIELDDEF);
        skeleton->tablesSize[TBL_MethodDef] /= sizeof(CLR_RECORD_METHODDEF);
        skeleton->tablesSize[TBL_GenericParam] /= sizeof(CLR_RECORD_GENERICPARAM);
        skeleton->tablesSize[TBL_MethodSpec] /= sizeof(CLR_RECORD_METHODSPEC);
        skeleton->tablesSize[TBL_TypeSpec] /= sizeof(CLR_RECORD_TYPESPEC);
        skeleton->tablesSize[TBL_Attributes] /= sizeof(CLR_RECORD_ATTRIBUTE);
        skeleton->tablesSize[TBL_Resources] /= sizeof(CLR_RECORD_RESOURCE);
        skeleton->tablesSize[TBL_ResourcesFiles] /= sizeof(CLR_RECORD_RESOURCE_FILE);
    }

    //--//

    // Count static fields.
    {
        auto *src = (const CLR_RECORD_TYPEDEF *)skeleton->GetTable(TBL_TypeDef);

        for (int i = 0; i < skeleton->tablesSize[TBL_TypeDef]; i++, src++)
        {
            skeleton->staticFieldsCount += src->staticFieldsCount;
        }
    }

    //--//

    {
        CLR_RT_Assembly::Offsets offsets;

        offsets.base = ROUNDTOMULTIPLE(sizeof(CLR_RT_Assembly), CLR_UINT32);

        offsets.assemblyRef = ROUNDTOMULTIPLE(
            skeleton->tablesSize[TBL_AssemblyRef] * sizeof(CLR_RT_AssemblyRef_CrossReference),
            CLR_UINT32);

        offsets.typeRef =
            ROUNDTOMULTIPLE(skeleton->tablesSize[TBL_TypeRef] * sizeof(CLR_RT_TypeRef_CrossReference), CLR_UINT32);

        offsets.fieldRef =
            ROUNDTOMULTIPLE(skeleton->tablesSize[TBL_FieldRef] * sizeof(CLR_RT_FieldRef_CrossReference), CLR_UINT32);

        offsets.methodRef =
            ROUNDTOMULTIPLE(skeleton->tablesSize[TBL_MethodRef] * sizeof(CLR_RT_MethodRef_CrossReference), CLR_UINT32);

        offsets.typeDef =
            ROUNDTOMULTIPLE(skeleton->tablesSize[TBL_TypeDef] * sizeof(CLR_RT_TypeDef_CrossReference), CLR_UINT32);

        offsets.fieldDef =
            ROUNDTOMULTIPLE(skeleton->tablesSize[TBL_FieldDef] * sizeof(CLR_RT_FieldDef_CrossReference), CLR_UINT32);

        offsets.methodDef =
            ROUNDTOMULTIPLE(skeleton->tablesSize[TBL_MethodDef] * sizeof(CLR_RT_MethodDef_CrossReference), CLR_UINT32);

        offsets.genericParam = ROUNDTOMULTIPLE(
            skeleton->tablesSize[TBL_GenericParam] * sizeof(CLR_RT_GenericParam_CrossReference),
            CLR_UINT32);

        offsets.methodSpec = ROUNDTOMULTIPLE(
            skeleton->tablesSize[TBL_MethodSpec] * sizeof(CLR_RT_MethodSpec_CrossReference),
            CLR_UINT32);

        offsets.typeSpec =
            ROUNDTOMULTIPLE(skeleton->tablesSize[TBL_TypeSpec] * sizeof(CLR_RT_TypeSpec_CrossReference), CLR_UINT32);

#if !defined(NANOCLR_APPDOMAINS)
        offsets.staticFieldsCount = ROUNDTOMULTIPLE(skeleton->staticFieldsCount * sizeof(CLR_RT_HeapBlock), CLR_UINT32);
#endif

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
        offsets.debuggingInfoMethods =
            ROUNDTOMULTIPLE(skeleton->tablesSize[TBL_MethodDef] * sizeof(CLR_RT_MethodDef_DebuggingInfo), CLR_UINT32);
#endif // #if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

        size_t iTotalRamSize = offsets.base + offsets.assemblyRef + offsets.typeRef + offsets.fieldRef +
                               offsets.methodRef + offsets.typeDef + offsets.fieldDef + offsets.methodDef +
                               offsets.genericParam + offsets.methodSpec + offsets.typeSpec;

#if !defined(NANOCLR_APPDOMAINS)
        iTotalRamSize += offsets.staticFieldsCount;
#endif

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
        iTotalRamSize += offsets.debuggingInfoMethods;
#endif // #if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

        //--//

        assm = EVENTCACHE_EXTRACT_NODE_AS_BYTES(
            g_CLR_RT_EventCache,
            CLR_RT_Assembly,
            DATATYPE_ASSEMBLY,
            0,
            (CLR_UINT32)iTotalRamSize);
        CHECK_ALLOCATION(assm);

        {
            //
            // We don't want to blow away the block header...
            //
            CLR_RT_HeapBlock *src = skeleton;
            CLR_RT_HeapBlock *dst = assm;

            memset(&dst[1], 0, iTotalRamSize - sizeof(struct CLR_RT_HeapBlock));
            memcpy(&dst[1], &src[1], sizeof(*assm) - sizeof(struct CLR_RT_HeapBlock));
        }

        assm->AssemblyInitialize(offsets);

#if !defined(BUILD_RTM)
        CLR_Debug::Printf(
            "   Assembly: %s (%d.%d.%d.%d)  ",
            assm->name,
            header->version.majorVersion,
            header->version.minorVersion,
            header->version.buildNumber,
            header->version.revisionNumber);

        if (s_CLR_RT_fTrace_AssemblyOverhead >= c_CLR_RT_Trace_Info)
        {
            size_t iMetaData = header->SizeOfTable(TBL_AssemblyRef) + header->SizeOfTable(TBL_TypeRef) +
                               header->SizeOfTable(TBL_FieldRef) + header->SizeOfTable(TBL_MethodRef) +
                               header->SizeOfTable(TBL_TypeDef) + header->SizeOfTable(TBL_FieldDef) +
                               header->SizeOfTable(TBL_MethodDef) + header->SizeOfTable(TBL_GenericParam) +
                               header->SizeOfTable(TBL_MethodSpec) + header->SizeOfTable(TBL_TypeSpec) +
                               header->SizeOfTable(TBL_Attributes) + header->SizeOfTable(TBL_Signatures);

            CLR_Debug::Printf(" (%d RAM - %d ROM - %d METADATA)", iTotalRamSize, header->TotalSize(), iMetaData);

#if defined(NANOCLR_GC_VERBOSE)
            if (s_CLR_RT_fTrace_Memory >= c_CLR_RT_Trace_Info)
            {
#ifdef _WIN64

                CLR_Debug::Printf(" @ 0x%016" PRIxPTR "", (uintptr_t)assm);
#else
                CLR_Debug::Printf(" @ 0x%08 PRIxPTR ", (uintptr_t)assm);
#endif
            }
#endif
            CLR_Debug::Printf("\r\n\r\n");

            CLR_Debug::Printf(" (%d RAM - %d ROM - %d METADATA)", iTotalRamSize, header->TotalSize(), iMetaData);

#if defined(NANOCLR_GC_VERBOSE)
            if (s_CLR_RT_fTrace_Memory >= c_CLR_RT_Trace_Info)
            {
#ifdef _WIN64

                CLR_Debug::Printf(" @ 0x%016" PRIxPTR "", (uintptr_t)assm);
#else
                CLR_Debug::Printf(" @ 0x%08 PRIxPTR ", (uintptr_t)assm);
#endif
            }
#endif
            CLR_Debug::Printf("\r\n\r\n");

            CLR_Debug::Printf(
                "   AssemblyRef     = %6d bytes (%5d elements)\r\n",
                offsets.assemblyRef,
                skeleton->tablesSize[TBL_AssemblyRef]);
            CLR_Debug::Printf(
                "   TypeRef         = %6d bytes (%5d elements)\r\n",
                offsets.typeRef,
                skeleton->tablesSize[TBL_TypeRef]);
            CLR_Debug::Printf(
                "   FieldRef        = %6d bytes (%5d elements)\r\n",
                offsets.fieldRef,
                skeleton->tablesSize[TBL_FieldRef]);
            CLR_Debug::Printf(
                "   MethodRef       = %6d bytes (%5d elements)\r\n",
                offsets.methodRef,
                skeleton->tablesSize[TBL_MethodRef]);
            CLR_Debug::Printf(
                "   TypeDef         = %6d bytes (%5d elements)\r\n",
                offsets.typeDef,
                skeleton->tablesSize[TBL_TypeDef]);
            CLR_Debug::Printf(
                "   FieldDef        = %6d bytes (%5d elements)\r\n",
                offsets.fieldDef,
                skeleton->tablesSize[TBL_FieldDef]);
            CLR_Debug::Printf(
                "   MethodDef       = %6d bytes (%5d elements)\r\n",
                offsets.methodDef,
                skeleton->tablesSize[TBL_MethodDef]);
            CLR_Debug::Printf(
                "   GenericParam    = %6d bytes (%5d elements)\r\n",
                offsets.genericParam,
                skeleton->tablesSize[TBL_GenericParam]);
            CLR_Debug::Printf(
                "   MethodSpec      = %6d bytes (%5d elements)\r\n",
                offsets.methodSpec,
                skeleton->tablesSize[TBL_MethodSpec]);

#if !defined(NANOCLR_APPDOMAINS)
            CLR_Debug::Printf(
                "   StaticFields    = %6d bytes (%5d elements)\r\n",
                offsets.staticFieldsCount,
                skeleton->staticFieldsCount);
#endif
            CLR_Debug::Printf("\r\n");

            CLR_Debug::Printf(
                "   Attributes      = %6d bytes (%5d elements)\r\n",
                skeleton->tablesSize[TBL_Attributes] * sizeof(CLR_RECORD_ATTRIBUTE),
                skeleton->tablesSize[TBL_Attributes]);
            CLR_Debug::Printf(
                "   TypeSpec        = %6d bytes (%5d elements)\r\n",
                skeleton->tablesSize[TBL_TypeSpec] * sizeof(CLR_RECORD_TYPESPEC),
                skeleton->tablesSize[TBL_TypeSpec]);
            CLR_Debug::Printf(
                "   Resources       = %6d bytes (%5d elements)\r\n",
                skeleton->tablesSize[TBL_Resources] * sizeof(CLR_RECORD_RESOURCE),
                skeleton->tablesSize[TBL_Resources]);
            CLR_Debug::Printf(
                "   Resources Files = %6d bytes (%5d elements)\r\n",
                skeleton->tablesSize[TBL_ResourcesFiles] * sizeof(CLR_RECORD_RESOURCE),
                skeleton->tablesSize[TBL_ResourcesFiles]);
            CLR_Debug::Printf("   Resources Data  = %6d bytes\r\n", skeleton->tablesSize[TBL_ResourcesData]);
            CLR_Debug::Printf("   Strings         = %6d bytes\r\n", skeleton->tablesSize[TBL_Strings]);
            CLR_Debug::Printf("   Signatures      = %6d bytes\r\n", skeleton->tablesSize[TBL_Signatures]);
            CLR_Debug::Printf("   ByteCode        = %6d bytes\r\n", skeleton->tablesSize[TBL_ByteCode]);
            CLR_Debug::Printf("\r\n\r\n");
        }
#endif
    }

    NANOCLR_NOCLEANUP();
}

#if defined(VIRTUAL_DEVICE)
HRESULT CLR_RT_Assembly::CreateInstance(
    const CLR_RECORD_ASSEMBLY *header,
    CLR_RT_Assembly *&assm,
    const wchar_t *szName)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    std::string strPath;

    NANOCLR_CHECK_HRESULT(CLR_RT_Assembly::CreateInstance(header, assm));

    if (szName != nullptr)
    {
        CLR_RT_UnicodeHelper::ConvertToUTF8(szName, strPath);

        assm->path = new std::string(strPath);
    }

    NANOCLR_NOCLEANUP();
}
#endif

bool CLR_RT_Assembly::ResolveAssemblyRef(bool fOutput)
{
#ifdef BUILD_RTM
    (void)fOutput;
#endif

    NATIVE_PROFILE_CLR_CORE();
    bool fGot = true;
    int i;

    ITERATE_THROUGH_RECORDS(this, i, AssemblyRef, ASSEMBLYREF)
    {
        const char *szName = GetString(src->name);

        if (dst->target == nullptr)
        {
            CLR_RT_Assembly *target = g_CLR_RT_TypeSystem.FindAssembly(szName, &src->version, false);

            if (target == nullptr || (target->flags & CLR_RT_Assembly::Resolved) == 0)
            {
#if !defined(BUILD_RTM)
                if (fOutput)
                {
                    CLR_Debug::Printf(
                        "Assembly: %s (%d.%d.%d.%d)",
                        name,
                        header->version.majorVersion,
                        header->version.minorVersion,
                        header->version.buildNumber,
                        header->version.revisionNumber);

                    CLR_Debug::Printf(
                        " needs assembly '%s' (%d.%d.%d.%d)\r\n",
                        szName,
                        src->version.majorVersion,
                        src->version.minorVersion,
                        src->version.buildNumber,
                        src->version.revisionNumber);
                }
#endif

                fGot = false;
            }
            else
            {
                dst->target = target;

#ifdef NANOCLR_TRACE_TYPE_RESOLUTION
                CLR_Debug::Printf(
                    "\r\nResolved assembly: %s (%d.%d.%d.%d) [%04X]\r\n",
                    name,
                    header->version.majorVersion,
                    header->version.minorVersion,
                    header->version.buildNumber,
                    header->version.revisionNumber,
                    target->assemblyIndex - 1);
#endif
            }
        }
    }

    return fGot;
}

void CLR_RT_Assembly::DestroyInstance()
{
    NATIVE_PROFILE_CLR_CORE();
    if (assemblyIndex)
    {
        g_CLR_RT_TypeSystem.m_assemblies[assemblyIndex - 1] = nullptr;
    }

#if defined(VIRTUAL_DEVICE)
    if (this->path != nullptr)
    {
        delete this->path;
        this->path = nullptr;
    }
#endif

    // check if header has to be freed (in case the assembly lives in RAM)
    if ((this->flags & CLR_RT_Assembly::FreeOnDestroy) != 0)
    {
        platform_free((void *)this->header);
    }

    //--//

    g_CLR_RT_EventCache.Append_Node(this);
}

//--//
HRESULT CLR_RT_Assembly::ResolveTypeRef()
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

#ifdef NANOCLR_TRACE_TYPE_RESOLUTION
    CLR_Debug::Printf("    Resolving TypeRef...\r\n");
#endif

    int i;

    ITERATE_THROUGH_RECORDS(this, i, TypeRef, TYPEREF)
    {
        // TODO check typedef
        if (src->scope & 0x8000) // Flag for TypeRef
        {
            CLR_RT_TypeDef_Instance inst{};

            if (inst.InitializeFromIndex(crossReferenceTypeRef[src->scope & 0x7FFF].target) == false)
            {
#if !defined(BUILD_RTM)
                CLR_Debug::Printf("Resolve: unknown scope: %08x\r\n", src->scope);
#endif

#if defined(VIRTUAL_DEVICE)
                NANOCLR_CHARMSG_SET_AND_LEAVE(CLR_E_FAIL, "Resolve: unknown scope: %08x\r\n", src->scope);
#else
                NANOCLR_MSG1_SET_AND_LEAVE(CLR_E_FAIL, L"Resolve: unknown scope: %08x\r\n", src->Scope);
#endif
            }

            const char *szName = GetString(src->name);

            if (inst.assembly->FindTypeDef(szName, inst.Type(), dst->target) == false)
            {
#if !defined(BUILD_RTM)
                CLR_Debug::Printf("Resolve: unknown type: %s\r\n", szName);
#endif

#if defined(VIRTUAL_DEVICE)
                NANOCLR_CHARMSG_SET_AND_LEAVE(CLR_E_FAIL, "Resolve: unknown type: %s\r\n", szName);
#else
                NANOCLR_MSG1_SET_AND_LEAVE(CLR_E_FAIL, L"Resolve: unknown type: %s\r\n", szName);
#endif
            }
#ifdef NANOCLR_TRACE_TYPE_RESOLUTION
            else
            {
                CLR_Debug::Printf("        [%04X] '%s' in current assembly\r\n", i, szName);
            }
#endif
        }
        else
        {
            CLR_RT_Assembly *assm = crossReferenceAssemblyRef[src->scope].target;
            if (assm == nullptr)
            {
                NANOCLR_MSG_SET_AND_LEAVE(CLR_E_FAIL, L"Resolve: assm is null\n");
            }

            const char *szNameSpace = GetString(src->nameSpace);
            const char *szName = GetString(src->name);

            if (assm->FindTypeDef(szName, szNameSpace, dst->target) == false)
            {
#if !defined(BUILD_RTM)
                CLR_Debug::Printf("Resolve: unknown type: %s.%s\r\n", szNameSpace, szName);
#endif

#if defined(VIRTUAL_DEVICE)
                NANOCLR_CHARMSG_SET_AND_LEAVE(CLR_E_FAIL, "Resolve: unknown type: %s\r\n", szName);
#else
                NANOCLR_MSG1_SET_AND_LEAVE(CLR_E_FAIL, L"Resolve: unknown type: %s\r\n", szName);
#endif
            }
#ifdef NANOCLR_TRACE_TYPE_RESOLUTION
            else
            {
                CLR_Debug::Printf("        [%04X] '%s.%s' from '%s'\r\n", i, szNameSpace, szName, assm->name);
            }
#endif
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_Assembly::ResolveFieldRef()
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

#ifdef NANOCLR_TRACE_TYPE_RESOLUTION
    bool outputHint = false;
#endif

    int i;

    ITERATE_THROUGH_RECORDS(this, i, FieldRef, FIELDREF)
    {
#ifdef NANOCLR_TRACE_TYPE_RESOLUTION
        if (!outputHint)
        {
            CLR_Debug::Printf("    Resolving FieldRef...\r\n");
            outputHint = true;
        }
#endif
        CLR_RT_TypeDef_Index typeDef;
        typeDef.Clear();

        CLR_RT_TypeDef_Instance typeDefInstance;

        CLR_RT_TypeSpec_Index typeSpec;
        typeSpec.Clear();

        CLR_RT_TypeSpec_Instance typeSpecInstance;

        const char *fieldName = GetString(src->name);

        switch (src->Owner())
        {

            case TBL_TypeRef:
                typeDef = crossReferenceTypeRef[src->OwnerIndex()].target;
                break;

                // case CLR_MemberRefParent::MRP_TypeDef:
                //     typeDef.Set(this->m_index, CLR_GetIndexFromMemberRefParent(src->container));
                //     break;
                //
                // case CLR_MemberRefParent::MRP_MethodDef:
                //     dst->m_target.Set(this->m_index, CLR_GetIndexFromMemberRefParent(src->container));
                //     fGot = true;
                //     break;

            case TBL_TypeSpec:
                typeSpec.Set(this->assemblyIndex, src->OwnerIndex());
                break;

            default:
#if !defined(BUILD_RTM)
                CLR_Debug::Printf(
                    "Unknown or unsupported TypeRefOrSpec when resolving FieldRef %08x '%s'\r\n",
                    src->encodedOwner,
                    fieldName);
#endif

#if defined(VIRTUAL_DEVICE)
                NANOCLR_CHARMSG_SET_AND_LEAVE(
                    CLR_E_FAIL,
                    "Unknown or unsupported TypeRefOrSpec when resolving FieldRef %08x '%s'\r\n",
                    src->encodedOwner,
                    fieldName);
#else
                NANOCLR_MSG1_SET_AND_LEAVE(
                    CLR_E_FAIL,
                    L"Unknown or unsupported TypeRefOrSpec when resolving FieldRef %08x '%s'\r\n",
                    src->encodedOwner,
                    fieldName);
#endif
        }

        if (NANOCLR_INDEX_IS_VALID(typeSpec))
        {
            if (typeSpecInstance.InitializeFromIndex(typeSpec) == false)
            {
#if !defined(BUILD_RTM)
                CLR_Debug::Printf("Unknown scope when resolving FieldRef: %08x '%s'\r\n", src->encodedOwner, fieldName);
#endif

#if defined(VIRTUAL_DEVICE)
                NANOCLR_CHARMSG_SET_AND_LEAVE(
                    CLR_E_FAIL,
                    "Unknown scope when resolving FieldRef: %08x '%s'\r\n",
                    src->encodedOwner,
                    fieldName);
#else
                NANOCLR_MSG1_SET_AND_LEAVE(
                    CLR_E_FAIL,
                    L"Unknown scope when resolving FieldRef: %08x '%s'\r\n",
                    src->encodedOwner,
                    fieldName);
#endif
            }

            if (!typeSpecInstance.assembly->FindFieldDef(&typeSpec, fieldName, this, src->signature, dst->target))
            {
#if !defined(BUILD_RTM)
                CLR_Debug::Printf("Unknown FieldRef: %s.%s.%s\r\n", "???", "???", fieldName);
#endif

#if defined(VIRTUAL_DEVICE)
                NANOCLR_CHARMSG_SET_AND_LEAVE(CLR_E_FAIL, "Unknown FieldRef: %s.%s.%s\r\n", "???", "???", fieldName);
#else
                NANOCLR_MSG1_SET_AND_LEAVE(CLR_E_FAIL, L"Unknown FieldRef: %s.%s.%s\r\n", "???", "???", fieldName);
#endif
            }
#ifdef NANOCLR_TRACE_TYPE_RESOLUTION
            else
            {
                char rgBuffer[512];
                char *szBuffer = rgBuffer;
                size_t iBuffer = MAXSTRLEN(rgBuffer);

                g_CLR_RT_TypeSystem.BuildTypeName(typeSpecInstance, szBuffer, iBuffer);
                rgBuffer[MAXSTRLEN(rgBuffer)] = 0;

                CLR_Debug::Printf("        [%04X] Resolving '%s' from '%s'\r\n", i, fieldName, rgBuffer);
            }
#endif

            // set TypeSpec
            dst->genericType.data = typeSpec.data;
        }
        else if (NANOCLR_INDEX_IS_VALID(typeDef))
        {
            if (typeDefInstance.InitializeFromIndex(crossReferenceTypeRef[src->OwnerIndex()].target) == false)
            {
#if !defined(BUILD_RTM)
                CLR_Debug::Printf("Unknown scope when resolving FieldRef: %08x\r\n", src->encodedOwner);
#endif

                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            }

#if defined(VIRTUAL_DEVICE) && defined(DEBUG_RESOLUTION)
            const CLR_RECORD_TYPEDEF *qTD = typeDefInstance.m_target;
            CLR_RT_Assembly *qASSM = typeDefInstance.m_assm;

            CLR_Debug::Printf(
                "Unknown scope when resolving FieldRef: %s.%s.%s\r\n",
                qASSM->GetString(qTD->NameSpace),
                qASSM->GetString(qTD->Name),
                name);
#endif

            if (typeDefInstance.assembly
                    ->FindFieldDef(typeDefInstance.target, fieldName, this, src->signature, dst->target) == false)
            {
#if !defined(BUILD_RTM)
                CLR_Debug::Printf("Unknown FieldRef: %s\r\n", fieldName);
#endif

#if defined(VIRTUAL_DEVICE)
                NANOCLR_CHARMSG_SET_AND_LEAVE(CLR_E_FAIL, "Unknown FieldRef: %s\r\n", fieldName);
#else
                NANOCLR_MSG1_SET_AND_LEAVE(CLR_E_FAIL, L"Unknown FieldRef: %s\r\n", fieldName);
#endif
            }
#ifdef NANOCLR_TRACE_TYPE_RESOLUTION
            else
            {
                char rgBuffer[512];
                char *szBuffer = rgBuffer;
                size_t iBuffer = MAXSTRLEN(rgBuffer);

                g_CLR_RT_TypeSystem.BuildTypeName(typeDefInstance, szBuffer, iBuffer);
                rgBuffer[MAXSTRLEN(rgBuffer)] = 0;

                CLR_Debug::Printf("        [%04X] Resolving '%s' from '%s'\r\n", i, fieldName, rgBuffer);
            }
#endif

            // invalidate GenericType
            dst->genericType.data = CLR_EmptyToken;
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_Assembly::ResolveMethodRef()
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

#ifdef NANOCLR_TRACE_TYPE_RESOLUTION
    bool outputHint = false;
#endif

    int i;

    ITERATE_THROUGH_RECORDS(this, i, MethodRef, METHODREF)
    {
#ifdef NANOCLR_TRACE_TYPE_RESOLUTION
        if (!outputHint)
        {
            CLR_Debug::Printf("    Resolving MethodRef...\r\n");
            outputHint = true;
        }
#endif

        CLR_RT_TypeDef_Index typeDef;
        typeDef.Clear();

        CLR_RT_TypeDef_Instance typeDefInstance;

        CLR_RT_TypeSpec_Index typeSpec;
        typeSpec.Clear();

        CLR_RT_TypeSpec_Instance typeSpecInstance;

        bool fGot = false;
        const char *methodName = GetString(src->name);

        switch (src->Owner())
        {
            case TBL_TypeRef:
                typeDef = crossReferenceTypeRef[src->OwnerIndex()].target;
                break;

                // case CLR_MemberRefParent::MRP_TypeDef:
                //     typeDef.Set(this->m_index, CLR_GetIndexFromMemberRefParent(src->container));
                //     break;
                //
                // case CLR_MemberRefParent::MRP_MethodDef:
                //     dst->m_target.Set(this->m_index, CLR_GetIndexFromMemberRefParent(src->container));
                //     fGot = true;
                //     break;

            case TBL_TypeSpec:
                typeSpec.Set(this->assemblyIndex, src->OwnerIndex());
                break;

            default:
#if !defined(BUILD_RTM)
                CLR_Debug::Printf(
                    "Unknown or unsupported TypeRefOrSpec when resolving MethodRef %08x '%s'\r\n",
                    src->encodedOwner,
                    methodName);
#endif

#if defined(VIRTUAL_DEVICE)
                NANOCLR_CHARMSG_SET_AND_LEAVE(
                    CLR_E_FAIL,
                    "Unknown or unsupported TypeRefOrSpec when resolving MethodRef %08x '%s'\r\n",
                    src->encodedOwner,
                    methodName);
#else
                NANOCLR_MSG1_SET_AND_LEAVE(
                    CLR_E_FAIL,
                    L"Unknown or unsupported TypeRefOrSpec when resolving MethodRef %08x '%s'\r\n",
                    src->encodedOwner,
                    methodName);
#endif
        }

        if (NANOCLR_INDEX_IS_VALID(typeSpec))
        {
            if (typeSpecInstance.InitializeFromIndex(typeSpec) == false)
            {
#if !defined(BUILD_RTM)
                CLR_Debug::Printf("Unknown scope when resolving MethodRef: %08x '%s'\r\n", src->encodedOwner);
#endif

#if defined(VIRTUAL_DEVICE)
                NANOCLR_CHARMSG_SET_AND_LEAVE(
                    CLR_E_FAIL,
                    "Unknown scope when resolving MethodRef: %08x\r\n",
                    src->encodedOwner,
                    methodName);
#else
                NANOCLR_MSG1_SET_AND_LEAVE(
                    CLR_E_FAIL,
                    L"Unknown scope when resolving MethodRef: %08x\r\n",
                    src->encodedOwner,
                    methodName);
#endif
            }

            CLR_UINT32 dummyAssemblyIndex = 0xffffffff;

            if (typeSpecInstance.assembly->FindMethodDef(
                    typeSpecInstance.target,
                    methodName,
                    this,
                    src->signature,
                    dst->target,
                    dummyAssemblyIndex))
            {
                fGot = true;

                // set TypeSpec
                dst->genericType.data = typeSpec.data;
            }

            if (fGot == false)
            {
#if !defined(BUILD_RTM)
                CLR_Debug::Printf("Unknown MethodRef: %s.%s.%s\r\n", "???", "???", methodName);
#endif

#if defined(VIRTUAL_DEVICE)
                NANOCLR_CHARMSG_SET_AND_LEAVE(CLR_E_FAIL, "Unknown MethodRef: %s.%s.%s\r\n", "???", "???", methodName);
#else
                NANOCLR_MSG1_SET_AND_LEAVE(CLR_E_FAIL, L"Unknown MethodRef: %s.%s.%s\r\n", "???", "???", methodName);
#endif
            }
#ifdef NANOCLR_TRACE_TYPE_RESOLUTION
            else
            {
                char rgBuffer[512];
                char *szBuffer = rgBuffer;
                size_t iBuffer = MAXSTRLEN(rgBuffer);

                g_CLR_RT_TypeSystem.BuildTypeName(typeSpecInstance, szBuffer, iBuffer);
                rgBuffer[MAXSTRLEN(rgBuffer)] = 0;

                CLR_Debug::Printf("        [%04X] Resolving '%s' from '%s'\r\n", i, methodName, rgBuffer);
            }
#endif
        }
        else if (NANOCLR_INDEX_IS_VALID(typeDef))
        {
            if (typeDefInstance.InitializeFromIndex(typeDef) == false)
            {
#if !defined(BUILD_RTM)
                CLR_Debug::Printf(
                    "Unknown scope when resolving MethodRef: %08x '%s'\r\n",
                    src->encodedOwner,
                    methodName);
#endif

#if defined(VIRTUAL_DEVICE)
                NANOCLR_CHARMSG_SET_AND_LEAVE(
                    CLR_E_FAIL,
                    "Unknown scope when resolving MethodRef: %08x '%s'\r\n",
                    src->encodedOwner,
                    methodName);
#else
                NANOCLR_MSG1_SET_AND_LEAVE(
                    CLR_E_FAIL,
                    L"Unknown scope when resolving MethodRef: %08x '%s'\r\n",
                    src->encodedOwner,
                    methodName);
#endif
            }

#if defined(VIRTUAL_DEVICE) && defined(DEBUG_RESOLUTION)
            const CLR_RECORD_TYPEDEF *qTD = typeDefInstance.m_target;
            CLR_RT_Assembly *qASSM = typeDefInstance.m_assm;

            CLR_Debug::Printf(
                "Unknown scope when resolving MethodRef: %s.%s.%s\r\n",
                qASSM->GetString(qTD->NameSpace),
                qASSM->GetString(qTD->Name),
                methodName);
#endif

            while (NANOCLR_INDEX_IS_VALID(typeDefInstance))
            {
                if (typeDefInstance.assembly
                        ->FindMethodDef(typeDefInstance.target, methodName, this, src->signature, dst->target))
                {
                    fGot = true;

                    // invalidate GenericType
                    dst->genericType.data = CLR_EmptyToken;

                    break;
                }

                typeDefInstance.SwitchToParent();
            }

            if (fGot == false)
            {
#if !defined(BUILD_RTM)
                const CLR_RECORD_TYPEDEF *qTD = typeDefInstance.target;
                CLR_RT_Assembly *qASSM = typeDefInstance.assembly;

                CLR_Debug::Printf(
                    "Unknown MethodRef: %s.%s.%s\r\n",
                    qASSM->GetString(qTD->nameSpace),
                    qASSM->GetString(qTD->name),
                    methodName);
#endif
#if defined(VIRTUAL_DEVICE)
                NANOCLR_CHARMSG_SET_AND_LEAVE(
                    CLR_E_FAIL,
                    "Unknown MethodRef: %s.%s.%s\r\n",
                    qASSM->GetString(qTD->nameSpace),
                    qASSM->GetString(qTD->name),
                    methodName);
#else
                NANOCLR_MSG1_SET_AND_LEAVE(
                    CLR_E_FAIL,
                    L"Unknown MethodRef: %s.%s.%s\r\n",
                    qASSM->GetString(qTD->NameSpace),
                    qASSM->GetString(qTD->Name),
                    methodName);
#endif
            }
#ifdef NANOCLR_TRACE_TYPE_RESOLUTION
            else
            {
                char rgBuffer[512];
                char *szBuffer = rgBuffer;
                size_t iBuffer = MAXSTRLEN(rgBuffer);

                g_CLR_RT_TypeSystem.BuildTypeName(typeDefInstance, szBuffer, iBuffer);
                rgBuffer[MAXSTRLEN(rgBuffer)] = 0;

                CLR_Debug::Printf("        [%04X] Resolving '%s' from '%s'\r\n", i, methodName, rgBuffer);
            }
#endif
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_Assembly::ResolveTypeSpec()
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

#ifdef NANOCLR_TRACE_TYPE_RESOLUTION
    bool outputHint = false;
#endif

    int i;

    ITERATE_THROUGH_RECORDS(this, i, TypeSpec, TYPESPEC)
    {
#ifdef NANOCLR_TRACE_TYPE_RESOLUTION
        if (!outputHint)
        {
            CLR_Debug::Printf("    Resolving TypeSpec...\r\n");
            outputHint = true;
        }
#endif

        dst->genericType.Set(assemblyIndex, i);

        CLR_RT_TypeSpec_Instance inst;

        if (inst.InitializeFromIndex(dst->genericType) == false)
        {
#if !defined(BUILD_RTM)
            CLR_Debug::Printf("Resolve TypeSpec: can't create TypeSpec instance: %08x\r\n", src->signature);
#endif
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
#ifdef NANOCLR_TRACE_TYPE_RESOLUTION
        else
        {
            CLR_Debug::Printf("        [%04X]\r\n", i);
        }
#endif

        // parse the signature to find the defining type token
        CLR_RT_SignatureParser parser;
        parser.Initialize_TypeSpec(this, GetTypeSpec(i));

        CLR_RT_SignatureParser::Element elem;

        parser.Advance(elem);

        if (elem.DataType == DATATYPE_VALUETYPE)
        {
            // now `elem.Class` is exactly the open Span`1 or List`1
            // store the open‐generic’s token
            dst->ownerType = elem.Class;
        }
        else
        {
            // if the first element is not a generic instantiation, then it must be a type token
            // no open‐generic, so clear the token
            dst->ownerType.Clear();
        }
    }

    NANOCLR_NOCLEANUP();
}

void CLR_RT_Assembly::ResolveLink()
{
    NATIVE_PROFILE_CLR_CORE();
    int iStaticFields = 0;
    int indexType;

    ITERATE_THROUGH_RECORDS(this, indexType, TypeDef, TYPEDEF)
    {
        int num;
        int i;

        //
        // Link static fields.
        //
        {
            CLR_RT_FieldDef_CrossReference *fd = &crossReferenceFieldDef[src->firstStaticField];

            num = src->staticFieldsCount;

            for (; num; num--, fd++)
            {
                fd->offset = iStaticFields++;
            }
        }

        //
        // Link instance fields.
        //
        {
            CLR_RT_TypeDef_Index index{};
            index.Set(assemblyIndex, indexType);
            CLR_RT_TypeDef_Instance inst{};
            inst.InitializeFromIndex(index);
            CLR_INDEX tot = 0;

            do
            {
                if (inst.target->flags & CLR_RECORD_TYPEDEF::TD_HasFinalizer)
                {
                    dst->flags |= CLR_RT_TypeDef_CrossReference::TD_CR_HasFinalizer;
                }

#if defined(NANOCLR_APPDOMAINS)
                if (inst.m_data == g_CLR_RT_WellKnownTypes.MarshalByRefObject.m_data)
                {
                    dst->m_flags |= CLR_RT_TypeDef_CrossReference::TD_CR_IsMarshalByRefObject;
                }
#endif

                tot += inst.target->instanceFieldsCount;
            } while (inst.SwitchToParent());

            dst->totalFields = tot;

            //--//

            CLR_RT_FieldDef_CrossReference *fd = &crossReferenceFieldDef[src->firstInstanceField];

            num = src->instanceFieldsCount;
            i = tot - num + CLR_RT_HeapBlock::HB_Object_Fields_Offset; // Take into account the offset from the
                                                                       // beginning of the object.

            for (; num; num--, i++, fd++)
            {
                fd->offset = i;
            }
        }

        //
        // Link methods.
        //
        {
            CLR_RT_MethodDef_CrossReference *md = &crossReferenceMethodDef[src->firstMethod];

            num = src->virtualMethodCount + src->instanceMethodCount + src->staticMethodCount;

            for (; num; num--, md++)
            {
                md->data = indexType;
            }
        }

        //
        // Link generic parameters, if any
        //
        {
            if (src->genericParamCount)
            {
                CLR_RT_GenericParam_CrossReference *gp = &crossReferenceGenericParam[src->firstGenericParam];

                // get generic parameter count for stop condition
                int count = src->genericParamCount;
                CLR_UINT16 indexGenericParam = src->firstGenericParam;

                for (; count; count--, gp++, indexGenericParam++)
                {
                    gp->m_target.Set(assemblyIndex, indexGenericParam);

                    gp->data = indexType;
                    gp->typeOrMethodDef = TBL_TypeDef;

                    CLR_RT_SignatureParser sub;
                    if (sub.Initialize_GenericParamTypeSignature(this, GetGenericParam(indexGenericParam)))
                    {
                        CLR_RT_SignatureParser::Element res;

                        // get generic param type
                        sub.Advance(res);

                        gp->dataType = res.DataType;
                        gp->classTypeDef = res.Class;
                    }
                    else
                    {
                        gp->dataType = DATATYPE_VOID;

                        CLR_RT_TypeDef_Index td;
                        td.Clear();

                        gp->classTypeDef = td;
                    }
                }
            }
        }
    }
}

//--//

#if defined(NANOCLR_APPDOMAINS)

HRESULT CLR_RT_AppDomain::CreateInstance(const char *szName, CLR_RT_AppDomain *&appDomain)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();
    CLR_RT_HeapBlock name;
    name.SetObjectReference(nullptr);
    CLR_RT_ProtectFromGC gc(name);

    if (!szName || szName[0] == '\0')
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

    // Check to see that the name does not already exist.
    NANOCLR_FOREACH_NODE(CLR_RT_AppDomain, appDomain, g_CLR_RT_ExecutionEngine.m_appDomains)
    {
        if (!strcmp(appDomain->m_strName->StringText(), szName))
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }
    NANOCLR_FOREACH_NODE_END();

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(name, szName));

    appDomain = EVENTCACHE_EXTRACT_NODE(g_CLR_RT_EventCache, CLR_RT_AppDomain, DATATYPE_APPDOMAIN_HEAD);
    CHECK_ALLOCATION(appDomain);

    appDomain->AppDomain_Initialize();

    appDomain->m_strName = name.DereferenceString();

    g_CLR_RT_ExecutionEngine.m_appDomains.LinkAtBack(appDomain);

    NANOCLR_NOCLEANUP();
}

void CLR_RT_AppDomain::RecoverFromGC()
{
    NATIVE_PROFILE_CLR_CORE();
    CheckAll();

    /*
        AppDomains can be zombied and stay around forever.  It is worth looking into cleaning up dead AppDomains, but
       this needs to be done with great care.  First, enumerations of AppDomains must not be allowed.  Second, the
       AppDomain must really be dead. This is much harder to ensure.  Everything that is needed to be checked for
       explicit AD unloading (stack frames, finalizable objects) must be checked, but also other things that can cause
       the AppDomain to be alive, like timers, manaaged drivers, transparent proxies, etc..
    */

    if (m_state == CLR_RT_AppDomain::AppDomainState_Unloaded)
    {
        // We could actually clean up here.  Since the AppDomain is now officially loaded,
        // we can remove all object_to_event_dst references, and null out the
        // pointers in the managed AppDomain class, provided that calling any method on the
        // AppDomain will result in a AppDomainDisposed exception.  However, it's probably
        // not worth the effort, the majority of the resources have been already cleaned
        // up, from AppDomain_Uninitialize

        if (IsReadyForRelease())
        {
            DestroyInstance();
        }
    }
}

void CLR_RT_AppDomain::AppDomain_Initialize()
{
    NATIVE_PROFILE_CLR_CORE();
    Initialize();

    m_appDomainAssemblies.DblLinkedList_Initialize();

    m_state = CLR_RT_AppDomain::AppDomainState_Loaded;
    m_id = g_CLR_RT_ExecutionEngine.m_appDomainIdNext++;
    m_globalLock = nullptr;
    m_strName = nullptr;
    m_outOfMemoryException = nullptr;
    m_appDomainAssemblyLastAccess = nullptr;
}

void CLR_RT_AppDomain::AppDomain_Uninitialize()
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_FOREACH_NODE(CLR_RT_AppDomainAssembly, appDomainAssembly, m_appDomainAssemblies)
    {
        appDomainAssembly->DestroyInstance();
    }
    NANOCLR_FOREACH_NODE_END();
}

void CLR_RT_AppDomain::DestroyInstance()
{
    NATIVE_PROFILE_CLR_CORE();
    AppDomain_Uninitialize();

    Unlink();

    g_CLR_RT_EventCache.Append_Node(this);
}

HRESULT CLR_RT_AppDomain::LoadAssembly(CLR_RT_Assembly *assm)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_AppDomain *appDomainSav = g_CLR_RT_ExecutionEngine.SetCurrentAppDomain(this);
    CLR_RT_AppDomainAssembly *appDomainAssembly = nullptr;
    int i;

    FAULT_ON_NULL(assm);

    // check to make sure the assembly is not already loaded
    if (FindAppDomainAssembly(assm) != nullptr)
        NANOCLR_SET_AND_LEAVE(S_OK);

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
    g_CLR_RT_ExecutionEngine.Breakpoint_Assemblies_Loaded();
#endif

    // Next, make sure that all dependent assemblies are loaded.
    {
        ITERATE_THROUGH_RECORDS(assm, i, AssemblyRef, ASSEMBLYREF)
        {
            NANOCLR_CHECK_HRESULT(LoadAssembly(dst->m_target));
        }
    }

    NANOCLR_CHECK_HRESULT(CLR_RT_AppDomainAssembly::CreateInstance(this, assm, appDomainAssembly));

    // Preemptively allocate an out of memory exception.
    // We can never get into a case where an out of memory exception cannot be thrown.
    if (m_outOfMemoryException == nullptr)
    {
        _ASSERTE(!strcmp(assm->m_szName, "mscorlib")); // always the first assembly to be loaded

        CLR_RT_HeapBlock exception;

        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.NewObjectFromIndex(exception, g_CLR_RT_WellKnownTypes.OutOfMemoryException));

        m_outOfMemoryException = exception.Dereference();
    }

    NANOCLR_CLEANUP();

    if (FAILED(hr))
    {
        if (appDomainAssembly)
        {
            appDomainAssembly->DestroyInstance();
        }
    }

    (void)g_CLR_RT_ExecutionEngine.SetCurrentAppDomain(appDomainSav);

    NANOCLR_CLEANUP_END();
}

HRESULT CLR_RT_AppDomain::GetManagedObject(CLR_RT_HeapBlock &res)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_AppDomain *appDomainSav = g_CLR_RT_ExecutionEngine.SetCurrentAppDomain(this);

    res.SetObjectReference(nullptr);

    // Check if a managed object is already present, and use it
    NANOCLR_FOREACH_NODE(CLR_RT_ObjectToEvent_Source, ref, m_references)
    {
        CLR_RT_HeapBlock *obj = ref->m_objectPtr;

        _ASSERTE(FIMPLIES(obj, obj->DataType() == DATATYPE_CLASS || obj->DataType() == DATATYPE_VALUETYPE));

        if (obj && obj->ObjectCls().m_data == g_CLR_RT_WellKnownTypes.AppDomain.m_data)
        {
            // managed appDomain is found.  Use it.
            res.SetObjectReference(ref->m_objectPtr);

            NANOCLR_SET_AND_LEAVE(S_OK);
        }
    }
    NANOCLR_FOREACH_NODE_END();

    {
        // Create the managed AppDomain in the destination AppDomain
        CLR_RT_HeapBlock *pRes;
        CLR_RT_ProtectFromGC gc(res);

        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(res, g_CLR_RT_WellKnownTypes.AppDomain));

        pRes = res.Dereference();

        NANOCLR_CHECK_HRESULT(
            CLR_RT_ObjectToEvent_Source::CreateInstance(
                this,
                *pRes,
                pRes[Library_corlib_native_System_AppDomain::FIELD___appDomain]));

        pRes[Library_corlib_native_System_AppDomain::FIELD___friendlyName].SetObjectReference(m_strName);
    }

    NANOCLR_CLEANUP();

    g_CLR_RT_ExecutionEngine.SetCurrentAppDomain(appDomainSav);

    NANOCLR_CLEANUP_END();
}

CLR_RT_AppDomainAssembly *CLR_RT_AppDomain::FindAppDomainAssembly(CLR_RT_Assembly *assm)
{
    NATIVE_PROFILE_CLR_CORE();
    if (m_appDomainAssemblyLastAccess != nullptr && m_appDomainAssemblyLastAccess->m_assembly == assm)
    {
        return m_appDomainAssemblyLastAccess;
    }

    NANOCLR_FOREACH_NODE(CLR_RT_AppDomainAssembly, appDomainAssembly, m_appDomainAssemblies)
    {
        if (appDomainAssembly->m_assembly == assm)
        {
            m_appDomainAssemblyLastAccess = appDomainAssembly;

            return appDomainAssembly;
        }
    }
    NANOCLR_FOREACH_NODE_END();

    return nullptr;
}

void CLR_RT_AppDomain::Relocate()
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_globalLock);
    CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_strName);
    CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_outOfMemoryException);
}

HRESULT CLR_RT_AppDomain::VerifyTypeIsLoaded(const CLR_RT_TypeDef_Index &index)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Instance inst;

    if (!inst.InitializeFromIndex(index))
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    if (!FindAppDomainAssembly(inst.m_assm))
        NANOCLR_SET_AND_LEAVE(CLR_E_APPDOMAIN_MARSHAL_EXCEPTION);

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_AppDomain::MarshalObject(CLR_RT_HeapBlock &src, CLR_RT_HeapBlock &dst, CLR_RT_AppDomain *appDomainSrc)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    // This function marshals an object from appDomainSrc to 'this' AppDomain
    // If appDomainSrc == nullptr, this uses the current AppDomain

    CLR_RT_AppDomain *appDomainDst = this;
    CLR_RT_HeapBlock *proxySrc = nullptr;
    CLR_RT_HeapBlock *mbroSrc = nullptr;
    bool fSimpleAssign = false;
    CLR_RT_TypeDef_Index indexVerify = g_CLR_RT_WellKnownTypes.Object;
    NanoCLRDataType dtSrc = src.DataType();
    CLR_RT_AppDomain *appDomainSav = g_CLR_RT_ExecutionEngine.GetCurrentAppDomain();

    if (!appDomainSrc)
    {
        appDomainSrc = appDomainSav;
    }

    //
    // DATATYPE_LAST_PRIMITIVE_TO_MARSHAL note
    // We should think about allowing STRINGS to be shared across AD boundaries
    // Strings are read-only and it is safe to do this with some small restrictions
    // First, as with the Assembly unloading, some strings can actually point within an
    // assembly.  If we allow assemblies to be unloaded (and why shouldn't we, if they are
    // not in any AppDomain, than we need to deal with this case).  If we just
    // get copy the string on constructor, then we do not need to marshal strings
    // across AD boundaries.
    //
    fSimpleAssign = (appDomainSrc == appDomainDst);
    fSimpleAssign = fSimpleAssign || (dtSrc <= DATATYPE_LAST_PRIMITIVE_TO_MARSHAL);
    fSimpleAssign = fSimpleAssign || (dtSrc == DATATYPE_OBJECT && src.Dereference() == nullptr);

#if !defined(NANOCLR_NO_ASSEMBLY_STRINGS)
    fSimpleAssign = fSimpleAssign || (dtSrc == DATATYPE_STRING && !src.StringAssembly());
#endif

    if (!fSimpleAssign)
    {
        if (dtSrc == DATATYPE_OBJECT)
        {
            CLR_RT_HeapBlock *ptr = src.Dereference();

            switch (ptr->DataType())
            {
                case DATATYPE_TRANSPARENT_PROXY:
                {
                    proxySrc = ptr;

                    NANOCLR_CHECK_HRESULT(proxySrc->TransparentProxyValidate());

                    indexVerify = proxySrc->TransparentProxyDereference()->ObjectCls();

                    if (proxySrc->TransparentProxyAppDomain() != appDomainDst)
                    {
                        // marshalling a transparent proxy object to a third AppDomain
                        // This makes the marshaling a simple assign of the DATATYPE_TRANSPARENT_PROXY heapblock
                        fSimpleAssign = true;
                    }
                }
                break;
                case DATATYPE_CLASS:
                {
                    CLR_RT_TypeDef_Instance inst;

                    if (inst.InitializeFromIndex(ptr->ObjectCls()))
                    {
                        if ((inst.CrossReference().m_flags &
                             CLR_RT_TypeDef_CrossReference::TD_CR_IsMarshalByRefObject) != 0)
                        {
                            indexVerify = inst;

                            mbroSrc = ptr;
                        }
                    }
                }
                break;
            }
        }
    }

    NANOCLR_CHECK_HRESULT(appDomainDst->VerifyTypeIsLoaded(indexVerify));

    if (fSimpleAssign)
    {
        dst.Assign(src);
    }
    else if (proxySrc != nullptr)
    {
        // src is OBJECT->TRANSPARENT_PROXY->CLASS, and we are marshalling into 'this' appDomain
        // dst is OBJECT->CLASS
        _ASSERTE(proxySrc->TransparentProxyAppDomain() == appDomainDst);

        dst.SetObjectReference(proxySrc->TransparentProxyDereference());
    }
    else if (mbroSrc != nullptr)
    {
        // src is a MarshalByRefObject that we are marshalling outside of its AppDomain
        // src is OBJECT->CLASS
        // dst is OBJECT->TRANSPARENT_PROXY->CLASS

        (void)g_CLR_RT_ExecutionEngine.SetCurrentAppDomain(appDomainDst);
        CLR_RT_HeapBlock *proxyDst =
            g_CLR_RT_ExecutionEngine.ExtractHeapBlocksForObjects(DATATYPE_TRANSPARENT_PROXY, 0, 1);
        CHECK_ALLOCATION(proxyDst);

        proxyDst->SetTransparentProxyReference(appDomainSrc, mbroSrc);
        dst.SetObjectReference(proxyDst);
    }
    else
    {
        CLR_RT_HeapBlock blk;
        blk.SetObjectReference(nullptr);
        CLR_RT_ProtectFromGC gc(blk);
        bool fNoCompaction = CLR_EE_DBG_IS(NoCompaction);

        // Need to prevent compaction between serialization/deserialization.
        // Doesn't seem that currently compaction can actually occur during this time,
        // but just to be safe, we should prevent it.

        CLR_EE_DBG_SET(NoCompaction);
        NANOCLR_CHECK_HRESULT(
            CLR_RT_BinaryFormatter::Serialize(blk, src, nullptr, CLR_RT_BinaryFormatter::c_Flags_Marshal));

        (void)g_CLR_RT_ExecutionEngine.SetCurrentAppDomain(appDomainDst);
        hr = CLR_RT_BinaryFormatter::Deserialize(dst, blk, nullptr, CLR_RT_BinaryFormatter::c_Flags_Marshal);

        CLR_EE_DBG_RESTORE(NoCompaction, fNoCompaction);
    }

    NANOCLR_CLEANUP();

    (void)g_CLR_RT_ExecutionEngine.SetCurrentAppDomain(appDomainSav);

    NANOCLR_CLEANUP_END();
}

HRESULT CLR_RT_AppDomain::MarshalParameters(
    CLR_RT_HeapBlock *callerArgs,
    CLR_RT_HeapBlock *calleeArgs,
    int count,
    bool fOnReturn,
    CLR_RT_AppDomain *appDomainSrc)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *src = fOnReturn ? calleeArgs : callerArgs;
    CLR_RT_HeapBlock *dst = fOnReturn ? callerArgs : calleeArgs;

    while (count-- > 0)
    {
        NanoCLRDataType dtSrc = src->DataType();
        NanoCLRDataType dtDst = dst->DataType();

        if (dtSrc == DATATYPE_BYREF || dtSrc == DATATYPE_ARRAY_BYREF)
        {
            CLR_RT_HeapBlock srcObj;
            CLR_RT_HeapBlock dstObj;

            if (fOnReturn)
            {
                srcObj.Assign(*src->Dereference());

                NANOCLR_CHECK_HRESULT(MarshalObject(srcObj, dstObj, appDomainSrc));

                // Move the marshaled object back into dst heapblock
                if (dtDst == DATATYPE_BYREF)
                {
                    dst->Dereference()->Assign(dstObj);
                }
                else
                {
                    dstObj.StoreToReference(*dst, 0);
                }
            }
            else //! fOnReturn
            {
                CLR_RT_HeapBlock *dstPtr = nullptr;

                if (dtSrc == DATATYPE_BYREF)
                {
                    srcObj.Assign(*src->Dereference());
                }
                else
                {
                    srcObj.LoadFromReference(*src);
                }

                NANOCLR_CHECK_HRESULT(MarshalObject(srcObj, dstObj, appDomainSrc));

                // Need to copy DstObj onto the heap.
                dstPtr = g_CLR_RT_ExecutionEngine.ExtractHeapBlocksForObjects(DATATYPE_OBJECT, 0, 1);
                FAULT_ON_NULL(dstPtr);

                _ASSERTE(c_CLR_RT_DataTypeLookup[dstObj.DataType()].m_sizeInBytes != CLR_RT_DataTypeLookup::c_NA);

                dstPtr->Assign(dstObj);

                // Turn the OBJECT back into a BYREF
                dst->SetReference(*dstPtr);
            }
        }
        else // Not BYREF
        {
            if (!fOnReturn)
            {
                NANOCLR_CHECK_HRESULT(MarshalObject(*src, *dst, appDomainSrc));
            }
        }

        src++;
        dst++;
    }

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_AppDomain::GetAssemblies(CLR_RT_HeapBlock &ref)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    int count = 0;
    CLR_RT_HeapBlock *pArray = nullptr;

    for (int pass = 0; pass < 2; pass++)
    {
        NANOCLR_FOREACH_ASSEMBLY_IN_APPDOMAIN(this)
        {
            if (pass == 0)
            {
                count++;
            }
            else
            {
                CLR_RT_HeapBlock *hbObj;
                CLR_RT_Assembly_Index index;
                index.Set(pASSM->m_index);

                NANOCLR_CHECK_HRESULT(
                    g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*pArray, g_CLR_RT_WellKnownTypes.Assembly));
                hbObj = pArray->Dereference();

                hbObj->SetReflection(index);

                pArray++;
            }
        }
        NANOCLR_FOREACH_ASSEMBLY_IN_APPDOMAIN_END();

        if (pass == 0)
        {
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(ref, count, g_CLR_RT_WellKnownTypes.Assembly));

            pArray = (CLR_RT_HeapBlock *)ref.DereferenceArray()->GetFirstElement();
        }
    }

    NANOCLR_NOCLEANUP();
}

bool CLR_RT_AppDomain::IsLoaded()
{
    NATIVE_PROFILE_CLR_CORE();
    return m_state == CLR_RT_AppDomain::AppDomainState_Loaded;
}

//--//

HRESULT CLR_RT_AppDomainAssembly::CreateInstance(
    CLR_RT_AppDomain *appDomain,
    CLR_RT_Assembly *assm,
    CLR_RT_AppDomainAssembly *&appDomainAssembly)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    int size = CONVERTFROMSIZETOHEAPBLOCKS(sizeof(CLR_RT_AppDomainAssembly)) + assm->m_iStaticFields;

    appDomainAssembly = EVENTCACHE_EXTRACT_NODE_AS_BLOCKS(
        g_CLR_RT_EventCache,
        CLR_RT_AppDomainAssembly,
        DATATYPE_APPDOMAIN_ASSEMBLY,
        CLR_RT_HeapBlock::HB_InitializeToZero,
        size);
    CHECK_ALLOCATION(appDomainAssembly);

    NANOCLR_CHECK_HRESULT(appDomainAssembly->AppDomainAssembly_Initialize(appDomain, assm));

    NANOCLR_CLEANUP();

    if (FAILED(hr))
    {
        if (appDomainAssembly)
        {
            appDomainAssembly->DestroyInstance();
        }
    }

    NANOCLR_CLEANUP_END();
}

HRESULT CLR_RT_AppDomainAssembly::AppDomainAssembly_Initialize(CLR_RT_AppDomain *appDomain, CLR_RT_Assembly *assm)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_AppDomain *appDomainSav = g_CLR_RT_ExecutionEngine.SetCurrentAppDomain(appDomain);

    m_appDomain = appDomain;
    m_assembly = assm;
    m_flags = 0;
    m_pStaticFields = (CLR_RT_HeapBlock *)&this[1];

    /*
        The AppDomainAssembly gets linked before it is actually initialized, for two reasons.  First, it needs to be
        attached to the AppDomain in case a GC runs during the allocation of fields.  Second, this assembly needs to be
        loaded in the AppDomain when the static constructors are run.
    */

    appDomain->m_appDomainAssemblies.LinkAtBack(this);

    NANOCLR_CHECK_HRESULT(assm->Resolve_AllocateStaticFields(m_pStaticFields));

    if (!CLR_EE_DBG_IS_MASK(StateInitialize, StateMask))
    {
        // Only in the non-boot case should we do this. Otherwise, debug events can occur out of order (thread creation
        // of the static constructor before thread creation of the main thread.
        g_CLR_RT_ExecutionEngine.SpawnStaticConstructor(g_CLR_RT_ExecutionEngine.m_cctorThread);
    }

    NANOCLR_CLEANUP();

    if (FAILED(hr))
    {
        Unlink();
    }

    g_CLR_RT_ExecutionEngine.SetCurrentAppDomain(appDomainSav);

    NANOCLR_CLEANUP_END();
}

void CLR_RT_AppDomainAssembly::DestroyInstance()
{
    NATIVE_PROFILE_CLR_CORE();
    Unlink();

    g_CLR_RT_EventCache.Append_Node(this);
}

void CLR_RT_AppDomainAssembly::Relocate()
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_GarbageCollector::Heap_Relocate(m_pStaticFields, m_assembly->m_iStaticFields);
}

#endif // NANOCLR_APPDOMAINS

//--//

struct TypeIndexLookup
{
    const char *nameSpace;
    const char *name;
    CLR_RT_TypeDef_Index *ptr;
};

// clang-format off

static const TypeIndexLookup c_TypeIndexLookup[] = {
#define TIL(ns, nm, fld)                                                                                               \
    {                                                                                                                  \
        ns, nm, &g_CLR_RT_WellKnownTypes.fld                                                                           \
    }
    TIL("System",                                   "Boolean",                          Boolean),
    TIL("System",                                   "Char",                             Char),
    TIL("System",                                   "SByte",                            Int8),
    TIL("System",                                   "Byte",                             UInt8),
    TIL("System",                                   "Int16",                            Int16),
    TIL("System",                                   "UInt16",                           UInt16),
    TIL("System",                                   "Int32",                            Int32),
    TIL("System",                                   "UInt32",                           UInt32),
    TIL("System",                                   "Int64",                            Int64),
    TIL("System",                                   "UInt64",                           UInt64),
    TIL("System",                                   "Single",                           Single),
    TIL("System",                                   "Double",                           Double),
    TIL("System",                                   "DateTime",                         DateTime),
    TIL("System",                                   "TimeSpan",                         TimeSpan),
    TIL("System",                                   "String",                           String),

    TIL("System",                                   "Void",                             n_Void),
    TIL("System",                                   "Object",                           Object),
    TIL("System",                                   "ValueType",                        ValueType),
    TIL("System",                                   "Enum",                             Enum),
    TIL("System",                                   "Nullable`1",                       Nullable),

    TIL("System",                                   "AppDomainUnloadedException",       AppDomainUnloadedException),
    TIL("System",                                   "ArgumentNullException",            ArgumentNullException),
    TIL("System",                                   "ArgumentException",                ArgumentException),
    TIL("System",                                   "ArgumentOutOfRangeException",      ArgumentOutOfRangeException),
    TIL("System",                                   "Exception",                        Exception),
    TIL("System",                                   "IndexOutOfRangeException",         IndexOutOfRangeException),
    TIL("System",                                   "InvalidCastException",             InvalidCastException),
    TIL("System",                                   "FormatException",                  FormatException),
    TIL("System",                                   "InvalidOperationException",        InvalidOperationException),
    TIL("System",                                   "NotSupportedException",            NotSupportedException),
    TIL("System",                                   "NotImplementedException",          NotImplementedException),
    TIL("System",                                   "NullReferenceException",           NullReferenceException),
    TIL("System",                                   "OutOfMemoryException",             OutOfMemoryException),
    TIL("System",                                   "TimeoutException",                 TimeoutException),
    TIL("System",                                   "ObjectDisposedException",          ObjectDisposedException),
    TIL("System.Threading",                         "ThreadAbortException",             ThreadAbortException),
    TIL("nanoFramework.Runtime.Native",             "ConstraintException",              ConstraintException),

    TIL("System",                                   "Delegate",                         Delegate),
    TIL("System",                                   "MulticastDelegate",                MulticastDelegate),

    TIL("System",                                   "Array",                            Array),
    TIL("System.Collections",                       "ArrayList",                        ArrayList),
    TIL("System",                                   "ICloneable",                       ICloneable),
    TIL("System.Collections",                       "IList",                            IList),

    TIL("System.Reflection",                        "Assembly",                         Assembly),
    TIL("System",                                   "Type",                             TypeStatic),
    TIL("System",                                   "RuntimeType",                      Type),
    TIL("System.Reflection",                        "RuntimeConstructorInfo",           ConstructorInfo),
    TIL("System.Reflection",                        "RuntimeMethodInfo",                MethodInfo),
    TIL("System.Reflection",                        "RuntimeFieldInfo",                 FieldInfo),

    TIL("System",                                   "WeakReference",                    WeakReference),

    TIL("System",                                   "Guid",                             Guid),

    TIL("nanoFramework.UI",                         "Bitmap",                           Bitmap),
    TIL("nanoFramework.UI",                         "Font",                             Font),
    TIL("nanoFramework.Touch",                      "TouchEvent",                       TouchEvent),
    TIL("nanoFramework.Touch",                      "TouchInput",                       TouchInput),

    TIL("System.Net.NetworkInformation",            "NetworkInterface",                 NetworkInterface),
    TIL("System.Net.NetworkInformation",            "Wireless80211Configuration",       Wireless80211Configuration),
    TIL("System.Net.NetworkInformation",            "WirelessAPConfiguration",          WirelessAPConfiguration),
    TIL("System.Net.NetworkInformation",            "WirelessAPStation",                WirelessAPStation),

#if defined(NANOCLR_APPDOMAINS)
    TIL("System",                                   "AppDomain",                        m_AppDomain),
    TIL("System",                                   "MarshalByRefObject",               m_MarshalByRefObject),
#endif

    TIL("System.Threading",                         "Thread",                           Thread),
    TIL("System.Resources",                         "ResourceManager",                  ResourceManager),

    TIL("System.Net.Sockets",                       "SocketException",                  SocketException),

    TIL("System.Device.I2c",                        "I2cTransferResult",                I2cTransferResult),

    TIL("nanoFramework.Hardware.Esp32.Rmt",         "RmtCommand",                       RmtCommand),

#undef TIL
};

// clang-format on

//--//

struct MethodIndexLookup
{
    const char *name;
    CLR_RT_TypeDef_Index *type;
    CLR_RT_MethodDef_Index *method;
};

static const MethodIndexLookup c_MethodIndexLookup[] = {
#define MIL(nm, type, method) {nm, &g_CLR_RT_WellKnownTypes.type, &g_CLR_RT_WellKnownMethods.method}

    // clang-format off

    MIL("GetObjectFromId", ResourceManager, m_ResourceManager_GetObjectFromId),
    MIL("GetObjectChunkFromId", ResourceManager, m_ResourceManager_GetObjectChunkFromId),

// clang-format on

#undef MIL
};

void CLR_RT_Assembly::ResolveTypeDef()
{
    NATIVE_PROFILE_CLR_CORE();
    const TypeIndexLookup *tilOuterClass = nullptr;
    const TypeIndexLookup *til = c_TypeIndexLookup;

    for (size_t i = 0; i < ARRAYSIZE(c_TypeIndexLookup); i++, til++)
    {
        CLR_RT_TypeDef_Index &dst = *til->ptr;

        if (NANOCLR_INDEX_IS_INVALID(dst))
        {
            if (til->nameSpace == nullptr)
            {
                if (tilOuterClass)
                {
                    FindTypeDef(til->name, tilOuterClass->ptr->Type(), dst);
                }
            }
            else
            {
                FindTypeDef(til->name, til->nameSpace, dst);
            }
        }

        if (til->nameSpace != nullptr)
        {
            tilOuterClass = til;
        }

#ifdef NANOCLR_TRACE_TYPE_RESOLUTION
        else
        {
            CLR_Debug::Printf("        [%04X] '%s.%s'\r\n", i, til->nameSpace, til->name);
        }
#endif
    }
}

void CLR_RT_Assembly::ResolveMethodDef()
{
    NATIVE_PROFILE_CLR_CORE();
    const CLR_RECORD_METHODDEF *md = GetMethodDef(0);

#ifdef NANOCLR_TRACE_TYPE_RESOLUTION
    bool outputHint = false;
#endif

    for (int indexMethod = 0; indexMethod < tablesSize[TBL_MethodDef]; indexMethod++, md++)
    {
#ifdef NANOCLR_TRACE_TYPE_RESOLUTION
        if (!outputHint)
        {
            CLR_Debug::Printf("    Resolving MethodDef...\r\n");
            outputHint = true;
        }
#endif

        const MethodIndexLookup *mil = c_MethodIndexLookup;

        CLR_RT_MethodDef_Index index;
        index.Set(assemblyIndex, indexMethod);

        const char *methodName = GetString(md->name);

        // Check for wellKnownMethods
        for (size_t ii = 0; ii < ARRAYSIZE(c_MethodIndexLookup); ii++, mil++)
        {
            CLR_RT_TypeDef_Index &indexType = *mil->type;
            CLR_RT_MethodDef_Index &mIndex = *mil->method;

            if (NANOCLR_INDEX_IS_VALID(indexType) && NANOCLR_INDEX_IS_INVALID(mIndex))
            {
                CLR_RT_TypeDef_Instance instType{};

                _SIDE_ASSERTE(instType.InitializeFromIndex(indexType));

                if (instType.assembly == this)
                {
                    if (!strcmp(methodName, mil->name))
                    {
                        mIndex.data = index.data;
                    }
                }
            }
        }

        if (md->flags & CLR_RECORD_METHODDEF::MD_EntryPoint)
        {
            g_CLR_RT_TypeSystem.m_entryPoint = index;
        }

        // link generic parameters
        if (md->genericParamCount)
        {
            CLR_RT_GenericParam_CrossReference *gp = &crossReferenceGenericParam[md->firstGenericParam];

            // get generic parameter count for stop condition
            int num = md->genericParamCount;
            CLR_UINT16 indexGenericParam = md->firstGenericParam;

            for (; num; num--, gp++, indexGenericParam++)
            {
                gp->m_target.Set(assemblyIndex, indexGenericParam);

                gp->data = indexMethod;
                gp->typeOrMethodDef = TBL_MethodDef;

                CLR_RT_SignatureParser sub;
                if (sub.Initialize_GenericParamTypeSignature(this, GetGenericParam(indexGenericParam)))
                {
                    CLR_RT_SignatureParser::Element res;

                    // get generic param type
                    sub.Advance(res);

                    gp->dataType = res.DataType;
                    gp->classTypeDef = res.Class;
                }
                else
                {
                    gp->dataType = DATATYPE_VOID;

                    CLR_RT_TypeDef_Index td;
                    td.Clear();

                    gp->classTypeDef = td;
                }
            }
        }

#ifdef NANOCLR_TRACE_TYPE_RESOLUTION
        CLR_Debug::Printf("        [%04X] Resolving '%s'\r\n", indexMethod, methodName);
#endif
    }
}

HRESULT CLR_RT_Assembly::ResolveAllocateStaticFields(CLR_RT_HeapBlock *pStaticFields)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    const CLR_RECORD_FIELDDEF *fd = GetFieldDef(0);

    for (int i = 0; i < tablesSize[TBL_FieldDef]; i++, fd++)
    {
        if (fd->flags & CLR_RECORD_FIELDDEF::FD_Static)
        {
            CLR_RT_FieldDef_CrossReference &res = crossReferenceFieldDef[i];

            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.InitializeReference(pStaticFields[res.offset], fd, this));
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_Assembly::PrepareForExecution()
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    if ((flags & CLR_RT_Assembly::PreparingForExecution) != 0)
    {
        // Circular dependency
        _ASSERTE(false);

        NANOCLR_MSG_SET_AND_LEAVE(CLR_E_FAIL, L"Failed to prepare type system for execution\n");
    }

    if ((flags & CLR_RT_Assembly::PreparedForExecution) == 0)
    {
        int i;

        flags |= CLR_RT_Assembly::PreparingForExecution;

        ITERATE_THROUGH_RECORDS(this, i, AssemblyRef, ASSEMBLYREF)
        {
            _ASSERTE(dst->target != nullptr);

            if (dst->target != nullptr)
            {
                NANOCLR_CHECK_HRESULT(dst->target->PrepareForExecution());
            }
        }

#if defined(NANOCLR_APPDOMAINS)
        // Temporary solution.  All Assemblies get added to the current AppDomain
        // Which assemblies get loaded at boot, and when assemblies get added to AppDomain at runtime is
        // not yet determined/implemented

        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.GetCurrentAppDomain()->LoadAssembly(this));
#endif
    }

    NANOCLR_CLEANUP();

    // Only try once.  If this fails, then what?
    flags |= CLR_RT_Assembly::PreparedForExecution;
    flags &= ~CLR_RT_Assembly::PreparingForExecution;

    NANOCLR_CLEANUP_END();
}

//--//

CLR_UINT32 CLR_RT_Assembly::ComputeAssemblyHash()
{
    NATIVE_PROFILE_CLR_CORE();
    return header->ComputeAssemblyHash(name, header->version);
}

CLR_UINT32 CLR_RT_Assembly::ComputeAssemblyHash(const CLR_RECORD_ASSEMBLYREF *ar)
{
    NATIVE_PROFILE_CLR_CORE();
    return header->ComputeAssemblyHash(GetString(ar->name), ar->version);
}

//--//

bool CLR_RT_Assembly::FindTypeDef(const char *typeName, const char *nameSpace, CLR_RT_TypeDef_Index &index)
{
    NATIVE_PROFILE_CLR_CORE();
    const CLR_RECORD_TYPEDEF *target = GetTypeDef(0);
    int tblSize = tablesSize[TBL_TypeDef];
    bool isNestedType = false;
    std::string extractedNamespace;

    // Check if typeName contains '/'
    const char *slashPos = strchr(typeName, '/');
    if (slashPos != nullptr)
    {
        // Extract the type name from the '/' to the end of the string
        const char *extractedTypeName = slashPos + 1;

        // Extract the enclosed type name from the '/' backwards to the '.' before
        const char *dotPos = strrchr(typeName, '.');
        std::string enclosedTypeName;

        if (dotPos != nullptr)
        {
            enclosedTypeName.assign(dotPos + 1, slashPos);
            // Extract the namespace from the beginning of the string to that '.'
            extractedNamespace.assign(typeName, dotPos - typeName);
        }
        else
        {
            enclosedTypeName.assign(typeName, slashPos);
            extractedNamespace.clear();
        }

        // Use the extracted values for further processing
        typeName = extractedTypeName;
        nameSpace = extractedNamespace.c_str();

        // set flag to indicate that this is a nested type
        isNestedType = true;
    }

    for (int i = 0; i < tblSize; i++, target++)
    {
        if (isNestedType)
        {
            // check if this is a nested type
            if (target->HasValidEnclosingType())
            {
                const char *szName = GetString(target->name);

                // for nested types, there is no namespace encoded in the type
                // looking at the type name only, does look a bit flaky but it will have to work for now
                if (!strcmp(szName, typeName))
                {
                    index.Set(assemblyIndex, i);
                    return true;
                }
            }
        }
        else if (!target->HasValidEnclosingType())
        {
            const char *szNameSpace = GetString(target->nameSpace);
            const char *szName = GetString(target->name);

            if (!strcmp(szName, typeName) && !strcmp(szNameSpace, nameSpace))
            {
                index.Set(assemblyIndex, i);
                return true;
            }
        }
    }

    index.Clear();
    return false;
}

bool CLR_RT_Assembly::FindTypeDef(const char *typeName, CLR_INDEX scope, CLR_RT_TypeDef_Index &index)
{
    NATIVE_PROFILE_CLR_CORE();
    const CLR_RECORD_TYPEDEF *target = GetTypeDef(0);
    int tblSize = tablesSize[TBL_TypeDef];

    for (int i = 0; i < tblSize; i++, target++)
    {
        if (!target->HasValidEnclosingType())
        {
            continue;
        }

        CLR_INDEX enclosingTypeIndex = target->EnclosingTypeIndex();

        if (enclosingTypeIndex == scope)
        {
            const char *szName = GetString(target->name);

            if (!strcmp(szName, typeName))
            {
                index.Set(assemblyIndex, i);

                return true;
            }
        }
    }

    index.Clear();

    return false;
}

bool CLR_RT_Assembly::FindTypeDef(CLR_UINT32 hash, CLR_RT_TypeDef_Index &index)
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_TypeDef_CrossReference *p = crossReferenceTypeDef;
    CLR_UINT32 tblSize = tablesSize[TBL_TypeDef];
    CLR_UINT32 i;

    for (i = 0; i < tblSize; i++, p++)
    {
        if (p->hash == hash)
            break;
    }

    if (i != tblSize)
    {
        index.Set(assemblyIndex, i);

        return true;
    }
    else
    {
        index.Clear();

        return false;
    }
}

bool CLR_RT_Assembly::FindGenericParam(CLR_INDEX typeSpecIndex, CLR_RT_GenericParam_Index &index)
{
    NATIVE_PROFILE_CLR_CORE();
    const CLR_RECORD_GENERICPARAM *gp = this->GetGenericParam(0);
    int tblSize = this->tablesSize[TBL_GenericParam];

    for (int i = 0; i < tblSize; i++, gp++)
    {
        CLR_RT_SignatureParser parserLeft;
        parserLeft.Initialize_GenericParamTypeSignature(this, gp);

        CLR_RT_SignatureParser parserRight;
        parserRight.Initialize_TypeSpec(this, this->GetTypeSpec(typeSpecIndex));

        if (CLR_RT_TypeSystem::MatchSignature(parserLeft, parserRight))
        {

            return true;
        }
    }

    index.Clear();

    return false;
}

bool CLR_RT_Assembly::FindGenericParamAtTypeSpec(
    CLR_UINT32 typeSpecIndex,
    CLR_INT32 genericParameterPosition,
    CLR_RT_TypeDef_Index &typeDef,
    NanoCLRDataType &dataType)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_SignatureParser parser;
    parser.Initialize_TypeSpec(this, GetTypeSpec(typeSpecIndex));

    CLR_RT_SignatureParser::Element element;

    // get into the GENERICINST
    if (FAILED(parser.Advance(element)))
    {
        return false;
    }

    // move to type
    if (FAILED(parser.Advance(element)))
    {
        return false;
    }

    // sanity check for invalid parameter position
    if (genericParameterPosition > parser.GenParamCount)
    {
        // not enough parameters!!
        return false;
    }

    // walk to the requested parameter position
    for (int32_t i = 0; i <= genericParameterPosition; i++)
    {
        if (FAILED(parser.Advance(element)))
        {
            return false;
        }
    }

    // element.Class was filled from the VAR position
    typeDef = element.Class;
    dataType = element.DataType;

    return true;
}

bool CLR_RT_Assembly::FindGenericParamAtMethodDef(
    CLR_RT_MethodDef_Instance md,
    CLR_INT32 genericParameterPosition,
    CLR_RT_GenericParam_Index &index)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_INDEX paramIndex = GetMethodDef(md.Method())->firstGenericParam;

    // sanity check for valid parameter index
    if (paramIndex != CLR_EmptyIndex)
    {
        paramIndex += genericParameterPosition;

        index.Set(assemblyIndex, paramIndex);

        return true;
    }
    else
    {
        index.Clear();

        return false;
    }
}

bool CLR_RT_Assembly::FindMethodSpecFromTypeSpec(CLR_INDEX typeSpecIndex, CLR_RT_MethodSpec_Index &index)
{
    NATIVE_PROFILE_CLR_CORE();
    const CLR_RECORD_METHODSPEC *ms = this->GetMethodSpec(0);
    int tblSize = this->tablesSize[TBL_MethodSpec];

    for (int i = 0; i < tblSize; i++, ms++)
    {
        if (ms->container == typeSpecIndex)
        {
            index.Set(assemblyIndex, i);

            return true;
        }
    }

    index.Clear();

    return false;
}

//--//

static bool local_FindFieldDef(
    CLR_RT_Assembly *assm,
    CLR_UINT32 first,
    CLR_UINT32 num,
    const char *szText,
    CLR_RT_Assembly *base,
    CLR_INDEX sig,
    CLR_RT_FieldDef_Index &res)
{
    NATIVE_PROFILE_CLR_CORE();
    const CLR_RECORD_FIELDDEF *fd = assm->GetFieldDef(first);

    for (CLR_UINT32 i = 0; i < num; i++, fd++)
    {
        const char *fieldName = assm->GetString(fd->name);

        if (!strcmp(fieldName, szText))
        {
            if (base)
            {
                CLR_RT_SignatureParser parserLeft{};
                parserLeft.Initialize_FieldDef(assm, fd);
                CLR_RT_SignatureParser parserRight{};
                parserRight.Initialize_FieldDef(base, base->GetSignature(sig));

                if (CLR_RT_TypeSystem::MatchSignature(parserLeft, parserRight) == false)
                {
                    continue;
                }
            }

            res.Set(assm->assemblyIndex, first + i);

            return true;
        }
    }

    res.Clear();

    return false;
}

bool CLR_RT_Assembly::FindFieldDef(
    const CLR_RECORD_TYPEDEF *td,
    const char *fieldName,
    CLR_RT_Assembly *base,
    CLR_INDEX sig,
    CLR_RT_FieldDef_Index &index)
{
    NATIVE_PROFILE_CLR_CORE();

    if (local_FindFieldDef(this, td->firstInstanceField, td->instanceFieldsCount, fieldName, base, sig, index))
    {
        return true;
    }

    if (local_FindFieldDef(this, td->firstStaticField, td->staticFieldsCount, fieldName, base, sig, index))
    {
        return true;
    }

    index.Clear();

    return false;
}

bool CLR_RT_Assembly::FindFieldDef(
    const CLR_RT_TypeSpec_Index *tsIndex,
    const char *fieldName,
    CLR_RT_Assembly *base,
    CLR_SIG sig,
    CLR_RT_FieldDef_Index &index)
{
    NATIVE_PROFILE_CLR_CORE();

    // find assembly of TypeSpec
    CLR_RT_Assembly *assm = g_CLR_RT_TypeSystem.m_assemblies[tsIndex->Assembly() - 1];

    CLR_RT_SignatureParser parser;
    parser.Initialize_TypeSpec(assm, assm->GetTypeSpec(tsIndex->TypeSpec()));

    CLR_RT_SignatureParser::Element element;

    // get type
    parser.Advance(element);

    CLR_RT_TypeDef_Index typeDef{};
    CLR_RT_TypeDef_Instance typeDefInstance{};
    const char *typeName;

    // if this is a generic type, need to advance to get type
    if (element.DataType == DATATYPE_GENERICINST)
    {
        parser.Advance(element);

        // OK to set the data directly
        typeDef.data = element.Class.data;
    }
    else if (element.DataType == DATATYPE_VAR)
    {
        // resolve the !T against the *closed* typeIndex
        CLR_RT_TypeSpec_Instance typeSpecInstance;
        typeSpecInstance.InitializeFromIndex(*tsIndex);

        typeDef.data = typeSpecInstance.genericTypeDef.data;
    }

    typeDefInstance.InitializeFromIndex(typeDef);
    typeName = GetString(typeDefInstance.target->name);

    const CLR_RECORD_FIELDDEF *fd = GetFieldDef(0);

    for (int i = 0; i < tablesSize[TBL_FieldDef]; i++, fd++)
    {
        const char *tempTypeName = GetString(fd->type);
        const char *tempFieldName = GetString(fd->name);

        if (!strcmp(typeName, tempTypeName) && !strcmp(fieldName, tempFieldName))
        {
            if (base)
            {
                CLR_RT_SignatureParser parserLeft;
                parserLeft.Initialize_FieldDef(this, fd);

                CLR_RT_SignatureParser parserRight;
                parserRight.Initialize_FieldSignature(base, base->GetSignature(sig));

                if (CLR_RT_TypeSystem::MatchSignature(parserLeft, parserRight) == false)
                {
                    continue;
                }
            }

            index.Set(assemblyIndex, i);

            return true;
        }
    }

    index.Clear();

    return false;
}

bool CLR_RT_Assembly::FindMethodDef(
    const CLR_RECORD_TYPEDEF *td,
    const char *methodName,
    CLR_RT_Assembly *base,
    CLR_SIG sig,
    CLR_RT_MethodDef_Index &index)
{
    NATIVE_PROFILE_CLR_CORE();
    int i;
    int num = td->virtualMethodCount + td->instanceMethodCount + td->staticMethodCount;
    const CLR_RECORD_METHODDEF *md = GetMethodDef(td->firstMethod);

    for (i = 0; i < num; i++, md++)
    {
        const char *tempMethodName = GetString(md->name);

        if (!strcmp(methodName, tempMethodName))
        {
            bool fMatch = true;

            if (CLR_SIG_INVALID != sig)
            {
                CLR_RT_SignatureParser parserLeft{};
                parserLeft.Initialize_MethodSignature(this, md);
                CLR_RT_SignatureParser parserRight{};
                parserRight.Initialize_MethodSignature(base, base->GetSignature(sig));

                fMatch = CLR_RT_TypeSystem::MatchSignature(parserLeft, parserRight);
            }

            if (fMatch)
            {
                index.Set(assemblyIndex, i + td->firstMethod);

                return true;
            }
        }
    }

    index.Clear();

    return false;
}

bool CLR_RT_Assembly::FindMethodDef(
    const CLR_RECORD_TYPESPEC *ts,
    const char *methodName,
    CLR_RT_Assembly *base,
    CLR_SIG sig,
    CLR_RT_MethodDef_Index &index,
    CLR_UINT32 &assmIndex)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_TypeSpec_Index tsIndex;
    if (!base->FindTypeSpec(base->GetSignature(ts->signature), tsIndex))
    {
        index.Clear();
        return false;
    }

    CLR_RT_TypeSpec_Instance tsInstance;
    if (!tsInstance.InitializeFromIndex(tsIndex))
    {
        index.Clear();
        return false;
    }

    // parse the TypeSpec signature to get the *definition* token of the generic type:
    CLR_RT_SignatureParser parser{};
    parser.Initialize_TypeSpec(this, ts);
    CLR_RT_SignatureParser::Element elem{};

    // advance to get the actual CLASS/VALUETYPE token for the generic definition.
    if (FAILED(parser.Advance(elem)))
    {
        return false;
    }

    // if this is a generic type, need the type
    if (elem.DataType == DATATYPE_GENERICINST)
    {
        if (FAILED(parser.Advance(elem)))
        {
            return false;
        }
    }

    CLR_UINT32 genericDefToken = elem.Class.data;

    CLR_RT_TypeDef_Index typeDef;
    typeDef.Clear();
    typeDef.data = genericDefToken;

    CLR_RT_TypeDef_Instance typeDefInstance;
    if (typeDefInstance.InitializeFromIndex(typeDef) == false)
    {
        return false;
    }

    while (NANOCLR_INDEX_IS_VALID(typeDefInstance))
    {
        if (typeDefInstance.assembly->FindMethodDef(typeDefInstance.target, methodName, this, sig, index))
        {
            assmIndex = typeDefInstance.assembly->assemblyIndex;

            return true;
        }

        typeDefInstance.SwitchToParent();
    }

    return false;
}

bool CLR_RT_Assembly::FindTypeSpec(CLR_PMETADATA sig, CLR_RT_TypeSpec_Index &index)
{
    NATIVE_PROFILE_CLR_CORE();

    for (int i = 0; i < tablesSize[TBL_TypeSpec]; i++)
    {
        const CLR_RECORD_TYPESPEC *ts = GetTypeSpec(i);

        CLR_RT_SignatureParser parserLeft;
        parserLeft.Initialize_TypeSpec(this, sig);
        CLR_RT_SignatureParser parserRight;
        parserRight.Initialize_TypeSpec(this, ts);

        if (CLR_RT_TypeSystem::MatchSignature(parserLeft, parserRight))
        {
            // found it!
            // set TypeSpec index
            index.Set(this->assemblyIndex, i);

            // need to advance the signature to consume it
            while (parserLeft.Signature != sig)
            {
                sig++;
            }

            return true;
        }
    }

    index.Clear();
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool CLR_RT_Assembly::FindMethodBoundaries(CLR_INDEX i, CLR_OFFSET &start, CLR_OFFSET &end)
{
    NATIVE_PROFILE_CLR_CORE();
    const CLR_RECORD_METHODDEF *p = GetMethodDef(i);

    if (p->rva == CLR_EmptyIndex)
        return false;

    start = p->rva;

    while (true)
    {
        p++;
        i++;

        if (i == tablesSize[TBL_MethodDef])
        {
            end = tablesSize[TBL_ByteCode];
            break;
        }

        if (p->rva != CLR_EmptyIndex)
        {
            end = p->rva;
            break;
        }
    }

    return true;
}

bool CLR_RT_Assembly::FindNextStaticConstructor(CLR_RT_MethodDef_Index &index)
{
    NATIVE_PROFILE_CLR_CORE();
    _ASSERTE(assemblyIndex == index.Assembly());

    for (int i = index.Method(); i < tablesSize[TBL_MethodDef]; i++)
    {
        const CLR_RECORD_METHODDEF *md = GetMethodDef(i);

        index.Set(assemblyIndex, i);

        // turn the index into a MethodDef_Instance
        CLR_RT_MethodDef_Instance methodDefInst;
        methodDefInst.InitializeFromIndex(index);

        CLR_RT_TypeDef_Instance typeDefInst;
        typeDefInst.InitializeFromMethod(methodDefInst);

        // check if this is a static constructor
        // but skip it if:
        // - references generic parameters
        // - it lives on a generic type definition
        if ((md->flags & CLR_RECORD_METHODDEF::MD_StaticConstructor) &&
            ((md->flags & CLR_RECORD_METHODDEF::MD_ContainsGenericParameter) == 0) &&
            typeDefInst.target->genericParamCount == 0)
        {
            return true;
        }
    }

    index.Clear();
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CLR_RT_Assembly::ResolveComputeHashes()
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    const CLR_RECORD_TYPEDEF *src = GetTypeDef(0);
    CLR_RT_TypeDef_CrossReference *dst = crossReferenceTypeDef;

    for (int i = 0; i < tablesSize[TBL_TypeDef]; i++, src++, dst++)
    {
        CLR_RT_TypeDef_Index index{};
        index.Set(assemblyIndex, i);
        CLR_RT_TypeDef_Instance inst{};
        inst.InitializeFromIndex(index);
        CLR_UINT32 hash = ComputeHashForName(index, 0);

        while (NANOCLR_INDEX_IS_VALID(inst))
        {
            const CLR_RECORD_TYPEDEF *target = inst.target;
            const CLR_RECORD_FIELDDEF *fd = inst.assembly->GetFieldDef(target->firstInstanceField);

            for (int j = 0; j < target->instanceFieldsCount; j++, fd++)
            {
                if ((fd->flags & CLR_RECORD_FIELDDEF::FD_NotSerialized) == 0)
                {
                    CLR_RT_SignatureParser parser{};
                    parser.Initialize_FieldDef(inst.assembly, fd);
                    CLR_RT_SignatureParser::Element res;

                    NANOCLR_CHECK_HRESULT(parser.Advance(res));

                    while (res.Levels-- > 0)
                    {
                        hash = ComputeHashForType(DATATYPE_SZARRAY, hash);
                    }

                    hash = ComputeHashForType(res.DataType, hash);

                    if ((res.DataType == DATATYPE_VALUETYPE) || (res.DataType == DATATYPE_CLASS))
                    {
                        hash = ComputeHashForName(res.Class, hash);
                    }

                    const char *fieldName = inst.assembly->GetString(fd->name);

                    hash = SUPPORT_ComputeCRC(fieldName, (CLR_UINT32)hal_strlen_s(fieldName), hash);
                }
            }

            inst.SwitchToParent();
        }

        dst->hash = hash ? hash : 0xFFFFFFFF; // Don't allow zero as an hash value!!
    }

    NANOCLR_NOCLEANUP();
}

CLR_UINT32 CLR_RT_Assembly::ComputeHashForName(const CLR_RT_TypeDef_Index &td, CLR_UINT32 hash)
{
    NATIVE_PROFILE_CLR_CORE();
    char rgBuffer[512];
    char *szBuffer = rgBuffer;
    size_t iBuffer = MAXSTRLEN(rgBuffer);

    g_CLR_RT_TypeSystem.BuildTypeName(td, szBuffer, iBuffer);

    CLR_UINT32 hashPost = SUPPORT_ComputeCRC(rgBuffer, (int)(MAXSTRLEN(rgBuffer) - iBuffer), hash);

    return hashPost;
}

CLR_UINT32 CLR_RT_Assembly::ComputeHashForType(NanoCLRDataType et, CLR_UINT32 hash)
{
    NATIVE_PROFILE_CLR_CORE();
    auto val = (CLR_UINT8)CLR_RT_TypeSystem::MapDataTypeToElementType(et);

    CLR_UINT32 hashPost = SUPPORT_ComputeCRC(&val, sizeof(val), hash);

    return hashPost;
}

//--//

CLR_RT_HeapBlock *CLR_RT_Assembly::GetStaticField(const int index)
{
    NATIVE_PROFILE_CLR_CORE();

#if defined(NANOCLR_APPDOMAINS)

    CLR_RT_AppDomainAssembly *adAssm = g_CLR_RT_ExecutionEngine.GetCurrentAppDomain()->FindAppDomainAssembly(this);

    _ASSERTE(adAssm);

    return &adAssm->m_pStaticFields[index];

#else

    return &staticFields[index];

#endif
}

//--//

void CLR_RT_Assembly::Relocate()
{
    NATIVE_PROFILE_CLR_CORE();

#if !defined(NANOCLR_APPDOMAINS)
    CLR_RT_GarbageCollector::Heap_Relocate(staticFields, staticFieldsCount);
#endif

    CLR_RT_GarbageCollector::Heap_Relocate((void **)&header);
    CLR_RT_GarbageCollector::Heap_Relocate((void **)&name);
    CLR_RT_GarbageCollector::Heap_Relocate((void **)&file);
    CLR_RT_GarbageCollector::Heap_Relocate((void **)&nativeCode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void CLR_RT_TypeSystem::TypeSystem_Initialize()
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_CLEAR(g_CLR_RT_TypeSystem);
    NANOCLR_CLEAR(g_CLR_RT_WellKnownTypes);
}

void CLR_RT_TypeSystem::TypeSystem_Cleanup()
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_FOREACH_ASSEMBLY(*this)
    {
        pASSM->DestroyInstance();

        *ppASSM = nullptr;
    }
    NANOCLR_FOREACH_ASSEMBLY_END();

    m_assembliesMax = 0;
}

//--//

void CLR_RT_TypeSystem::Link(CLR_RT_Assembly *assm)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_FOREACH_ASSEMBLY_NULL(*this)
    {
        *ppASSM = assm;

        assm->assemblyIndex = index;

        PostLinkageProcessing(assm);

        if (m_assembliesMax < index)
            m_assembliesMax = index;

        return;
    }
    NANOCLR_FOREACH_ASSEMBLY_NULL_END();
}

void CLR_RT_TypeSystem::PostLinkageProcessing(CLR_RT_Assembly *assm)
{
    NATIVE_PROFILE_CLR_CORE();
    if (!strcmp(assm->name, "mscorlib"))
    {
        m_assemblyMscorlib = assm;
    }
    if (!strcmp(assm->name, "nanoFramework.Runtime.Native"))
    {
        m_assemblyNative = assm;
    }
}

CLR_RT_Assembly *CLR_RT_TypeSystem::FindAssembly(const char *szName, const CLR_RECORD_VERSION *ver, bool fExact)
{
    NATIVE_PROFILE_CLR_CORE();

    NANOCLR_FOREACH_ASSEMBLY(*this)
    {
        if (!strcmp(pASSM->name, szName))
        {
            // if there is no version information, anything goes
            if (nullptr == ver)
            {
                return pASSM;
            }
            // exact match requested: must take into accoutn all numbers in the version
            else if (fExact)
            {
                if (0 == memcmp(&pASSM->header->version, ver, sizeof(*ver)))
                {
                    return pASSM;
                }
            }
            // exact match was NOT required but still there version information,
            // we will enforce only the first two number because (by convention)
            // only the minor field is required to be bumped when native assemblies change CRC
            else if (
                ver->majorVersion == pASSM->header->version.majorVersion &&
                ver->minorVersion == pASSM->header->version.minorVersion)
            {
                return pASSM;
            }
        }
    }
    NANOCLR_FOREACH_ASSEMBLY_END();

    return nullptr;
}

bool CLR_RT_TypeSystem::FindTypeDef(
    const char *name,
    const char *nameSpace,
    CLR_RT_Assembly *assm,
    CLR_RT_TypeDef_Index &res)
{
    NATIVE_PROFILE_CLR_CORE();

    if (assm)
    {
        NANOCLR_FOREACH_ASSEMBLY_IN_CURRENT_APPDOMAIN(*this)
        {
            if (pASSM->IsSameAssembly(*assm) && pASSM->FindTypeDef(name, nameSpace, res))
                return true;
        }
        NANOCLR_FOREACH_ASSEMBLY_IN_CURRENT_APPDOMAIN_END();

        res.Clear();

        return false;
    }

    return FindTypeDef(name, nameSpace, res);
}

bool CLR_RT_TypeSystem::FindTypeDef(const char *name, const char *nameSpace, CLR_RT_TypeDef_Index &res)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_FOREACH_ASSEMBLY_IN_CURRENT_APPDOMAIN(*this)
    {
        if (pASSM->FindTypeDef(name, nameSpace, res))
            return true;
    }
    NANOCLR_FOREACH_ASSEMBLY_IN_CURRENT_APPDOMAIN_END();

    res.Clear();
    return false;
}

bool CLR_RT_TypeSystem::FindTypeDef(CLR_UINT32 hash, CLR_RT_TypeDef_Index &res)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_FOREACH_ASSEMBLY_IN_CURRENT_APPDOMAIN(*this)
    {
        if (pASSM->FindTypeDef(hash, res))
            return true;
    }
    NANOCLR_FOREACH_ASSEMBLY_IN_CURRENT_APPDOMAIN_END();

    res.Clear();
    return false;
}

bool CLR_RT_TypeSystem::FindTypeDef(const char *szClass, CLR_RT_Assembly *assm, CLR_RT_TypeDef_Index &res)
{
    (void)szClass;
    (void)assm;
    (void)res;

    NATIVE_PROFILE_CLR_CORE();

    char rgName[MAXTYPENAMELEN];
    char rgNamespace[MAXTYPENAMELEN];

    if (hal_strlen_s(szClass) < ARRAYSIZE(rgNamespace))
    {
        const char *szPtr = szClass;
        const char *szPtr_LastDot = nullptr;
        const char *szPtr_FirstSubType = nullptr;
        char c;
        size_t len;

        while (true)
        {
            c = szPtr[0];
            if (!c)
                break;

            if (c == '.')
            {
                szPtr_LastDot = szPtr;
            }
            else if (c == '+')
            {
                szPtr_FirstSubType = szPtr;
                break;
            }

            szPtr++;
        }

        if (szPtr_LastDot)
        {
            len = szPtr_LastDot++ - szClass;
            hal_strncpy_s(rgNamespace, ARRAYSIZE(rgNamespace), szClass, len);
            len = szPtr - szPtr_LastDot;
            hal_strncpy_s(rgName, ARRAYSIZE(rgName), szPtr_LastDot, len);
        }
        else
        {
            rgNamespace[0] = 0;
            hal_strcpy_s(rgName, ARRAYSIZE(rgName), szClass);
        }

        if (FindTypeDef(rgName, rgNamespace, assm, res))
        {
            //
            // Found the containing type, let's look for the nested type.
            //
            if (szPtr_FirstSubType)
            {
                CLR_RT_TypeDef_Instance inst{};

                do
                {
                    szPtr = ++szPtr_FirstSubType;

                    while (true)
                    {
                        c = szPtr_FirstSubType[0];
                        if (!c)
                        {
                            break;
                        }

                        if (c == '+')
                        {
                            break;
                        }

                        szPtr_FirstSubType++;
                    }

                    len = szPtr_FirstSubType - szPtr;
                    hal_strncpy_s(rgName, ARRAYSIZE(rgName), szPtr, len);

                    inst.InitializeFromIndex(res);

                    if (inst.assembly->FindTypeDef(rgName, res.Type(), res) == false)
                    {
                        return false;
                    }

                } while (c == '+');
            }

            return true;
        }
    }

    res.Clear();

    return false;
}

bool CLR_RT_TypeSystem::FindTypeDef(const char *szClass, CLR_RT_Assembly *assm, CLR_RT_ReflectionDef_Index &reflex)
{
    (void)szClass;
    (void)assm;
    (void)reflex;

    NATIVE_PROFILE_CLR_CORE();

    char rgName[MAXTYPENAMELEN];
    char rgNamespace[MAXTYPENAMELEN];
    CLR_RT_TypeDef_Index res;

    if (hal_strlen_s(szClass) < ARRAYSIZE(rgNamespace))
    {
        const char *szPtr = szClass;
        const char *szPtr_LastDot = nullptr;
        const char *szPtr_FirstSubType = nullptr;
        char c;
        size_t len;
        bool arrayType = false;

        while (true)
        {
            c = szPtr[0];
            if (!c)
                break;

            if (c == '.')
            {
                szPtr_LastDot = szPtr;
            }
            else if (c == '+')
            {
                szPtr_FirstSubType = szPtr;
                break;
            }
            else if (c == '[')
            {
                char ch = szPtr[1];
                if (ch == ']')
                {
                    arrayType = true;
                    break;
                }
            }

            szPtr++;
        }

        if (szPtr_LastDot)
        {
            len = szPtr_LastDot++ - szClass;
            hal_strncpy_s(rgNamespace, ARRAYSIZE(rgNamespace), szClass, len);
            len = szPtr - szPtr_LastDot;
            hal_strncpy_s(rgName, ARRAYSIZE(rgName), szPtr_LastDot, len);
        }
        else
        {
            rgNamespace[0] = 0;
            hal_strcpy_s(rgName, ARRAYSIZE(rgName), szClass);
        }

        if (FindTypeDef(rgName, rgNamespace, assm, res))
        {
            //
            // Found the containing type, let's look for the nested type.
            //
            if (szPtr_FirstSubType)
            {
                CLR_RT_TypeDef_Instance inst{};

                do
                {
                    szPtr = ++szPtr_FirstSubType;

                    while (true)
                    {
                        c = szPtr_FirstSubType[0];
                        if (!c)
                            break;

                        if (c == '+')
                            break;

                        szPtr_FirstSubType++;
                    }

                    len = szPtr_FirstSubType - szPtr;
                    hal_strncpy_s(rgName, ARRAYSIZE(rgName), szPtr, len);

                    inst.InitializeFromIndex(res);

                    if (inst.assembly->FindTypeDef(rgName, res.Type(), res) == false)
                    {
                        return false;
                    }

                } while (c == '+');
            }

            reflex.kind = REFLECTION_TYPE;
            // make sure this works for multidimensional arrays.
            reflex.levels = arrayType ? 1 : 0;
            reflex.data.type = res;
            return true;
        }
    }

    res.Clear();

    return false;
}

//--//

int
#if defined(_MSC_VER)
    __cdecl
#endif
    CompareResource(const void *p1, const void *p2)
{
    NATIVE_PROFILE_CLR_CORE();
    auto *resource1 = (const CLR_RECORD_RESOURCE *)p1;
    auto *resource2 = (const CLR_RECORD_RESOURCE *)p2;

    return (int)resource1->id - (int)resource2->id;
}

HRESULT CLR_RT_TypeSystem::LocateResourceFile(
    CLR_RT_Assembly_Instance assm,
    const char *name,
    CLR_INT32 &indexResourceFile)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_Assembly *pAssm = assm.assembly;

    for (indexResourceFile = 0; indexResourceFile < pAssm->tablesSize[TBL_ResourcesFiles]; indexResourceFile++)
    {
        const CLR_RECORD_RESOURCE_FILE *resourceFile = pAssm->GetResourceFile(indexResourceFile);

        if (!strcmp(pAssm->GetString(resourceFile->name), name))
        {
            NANOCLR_SET_AND_LEAVE(S_OK);
        }
    }

    indexResourceFile = -1;

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_TypeSystem::LocateResource(
    CLR_RT_Assembly_Instance assm,
    CLR_INT32 indexResourceFile,
    CLR_INT16 id,
    const CLR_RECORD_RESOURCE *&res,
    CLR_UINT32 &size)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_Assembly *pAssm = assm.assembly;
    const CLR_RECORD_RESOURCE_FILE *resourceFile;
    CLR_RECORD_RESOURCE resourceT;
    const CLR_RECORD_RESOURCE *resNext;
    const CLR_RECORD_RESOURCE *resZero;

    res = nullptr;
    size = 0;

    if (indexResourceFile < 0 || indexResourceFile >= pAssm->tablesSize[TBL_ResourcesFiles])
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

    resourceFile = pAssm->GetResourceFile(indexResourceFile);

    _ASSERTE(resourceFile->numberOfResources > 0);

    resZero = pAssm->GetResource(resourceFile->offset);

    resourceT.id = id;

    res = (const CLR_RECORD_RESOURCE *)
        bsearch(&resourceT, resZero, resourceFile->numberOfResources, sizeof(CLR_RECORD_RESOURCE), CompareResource);

    if (res != nullptr)
    {
        // compute size here...
        // assert not the last resource
        _ASSERTE(res + 1 <= pAssm->GetResource(pAssm->tablesSize[TBL_Resources] - 1));
        resNext = res + 1;

        size = resNext->offset - res->offset;

        // deal with alignment.
        size -= (resNext->flags & CLR_RECORD_RESOURCE::FLAGS_PaddingMask);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_TypeSystem::ResolveAll()
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    bool fOutput = false;

    while (true)
    {
        bool fGot = false;
        bool fNeedResolution = false;

        NANOCLR_FOREACH_ASSEMBLY(*this)
        {
            if ((pASSM->flags & CLR_RT_Assembly::Resolved) == 0)
            {
                fNeedResolution = true;

                if (pASSM->ResolveAssemblyRef(fOutput))
                {
                    fGot = true;

                    pASSM->flags |= CLR_RT_Assembly::Resolved;

                    NANOCLR_CHECK_HRESULT(pASSM->ResolveTypeRef());
                    NANOCLR_CHECK_HRESULT(pASSM->ResolveFieldRef());
                    NANOCLR_CHECK_HRESULT(pASSM->ResolveMethodRef());
                    /********************/ pASSM->ResolveTypeDef();
                    /********************/ pASSM->ResolveMethodDef();
                    NANOCLR_CHECK_HRESULT(pASSM->ResolveTypeSpec());
                    /********************/ pASSM->ResolveLink();
                    NANOCLR_CHECK_HRESULT(pASSM->ResolveComputeHashes());

#if !defined(NANOCLR_APPDOMAINS)
                    NANOCLR_CHECK_HRESULT(pASSM->ResolveAllocateStaticFields(pASSM->staticFields));
#endif

                    pASSM->flags |= CLR_RT_Assembly::ResolutionCompleted;
                }
            }
        }
        NANOCLR_FOREACH_ASSEMBLY_END();

        if (fOutput == true)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_TYPE_UNAVAILABLE);
        }

        if (fGot == false)
        {
            if (fNeedResolution)
            {
#if !defined(BUILD_RTM)
                CLR_Debug::Printf("Link failure: some assembly references cannot be resolved!!\r\n\r\n");
#endif

                fOutput = true;
            }
            else
            {
                break;
            }
        }
    }

#if !defined(BUILD_RTM)

    if (s_CLR_RT_fTrace_AssemblyOverhead >= c_CLR_RT_Trace_Info)
    {
        {
            int pTablesSize[TBL_Max];
            memset(pTablesSize, 0, sizeof(pTablesSize));
            CLR_RT_Assembly::Offsets offsets;
            memset(&offsets, 0, sizeof(offsets));

            size_t iStaticFields = 0;
            size_t iTotalRamSize = 0;
            size_t iTotalRomSize = 0;
            size_t iMetaData = 0;

            NANOCLR_FOREACH_ASSEMBLY(*this)
            {
                offsets.base += ROUNDTOMULTIPLE(sizeof(CLR_RT_Assembly), CLR_UINT32);
                offsets.assemblyRef += ROUNDTOMULTIPLE(
                    pASSM->tablesSize[TBL_AssemblyRef] * sizeof(CLR_RT_AssemblyRef_CrossReference),
                    CLR_UINT32);
                offsets.typeRef +=
                    ROUNDTOMULTIPLE(pASSM->tablesSize[TBL_TypeRef] * sizeof(CLR_RT_TypeRef_CrossReference), CLR_UINT32);
                offsets.fieldRef += ROUNDTOMULTIPLE(
                    pASSM->tablesSize[TBL_FieldRef] * sizeof(CLR_RT_FieldRef_CrossReference),
                    CLR_UINT32);
                offsets.methodRef += ROUNDTOMULTIPLE(
                    pASSM->tablesSize[TBL_MethodRef] * sizeof(CLR_RT_MethodRef_CrossReference),
                    CLR_UINT32);
                offsets.typeDef +=
                    ROUNDTOMULTIPLE(pASSM->tablesSize[TBL_TypeDef] * sizeof(CLR_RT_TypeDef_CrossReference), CLR_UINT32);
                offsets.fieldDef += ROUNDTOMULTIPLE(
                    pASSM->tablesSize[TBL_FieldDef] * sizeof(CLR_RT_FieldDef_CrossReference),
                    CLR_UINT32);
                offsets.methodDef += ROUNDTOMULTIPLE(
                    pASSM->tablesSize[TBL_MethodDef] * sizeof(CLR_RT_MethodDef_CrossReference),
                    CLR_UINT32);
                offsets.genericParam += ROUNDTOMULTIPLE(
                    pASSM->tablesSize[TBL_GenericParam] * sizeof(CLR_RT_GenericParam_CrossReference),
                    CLR_UINT32);

#if !defined(NANOCLR_APPDOMAINS)
                offsets.staticFieldsCount +=
                    ROUNDTOMULTIPLE(pASSM->staticFieldsCount * sizeof(CLR_RT_HeapBlock), CLR_UINT32);
#endif

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
                offsets.debuggingInfoMethods += ROUNDTOMULTIPLE(
                    pASSM->tablesSize[TBL_MethodDef] * sizeof(CLR_RT_MethodDef_DebuggingInfo),
                    CLR_UINT32);
#endif // #if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

                iMetaData += pASSM->header->SizeOfTable(TBL_AssemblyRef) + pASSM->header->SizeOfTable(TBL_TypeRef) +
                             pASSM->header->SizeOfTable(TBL_FieldRef) + pASSM->header->SizeOfTable(TBL_MethodRef) +
                             pASSM->header->SizeOfTable(TBL_TypeDef) + pASSM->header->SizeOfTable(TBL_FieldDef) +
                             pASSM->header->SizeOfTable(TBL_MethodDef) + pASSM->header->SizeOfTable(TBL_GenericParam) +
                             pASSM->header->SizeOfTable(TBL_TypeSpec) + pASSM->header->SizeOfTable(TBL_Attributes) +
                             pASSM->header->SizeOfTable(TBL_Signatures);

                for (int tbl = 0; tbl < TBL_Max; tbl++)
                {
                    pTablesSize[tbl] += pASSM->tablesSize[tbl];
                }

                iTotalRomSize += pASSM->header->TotalSize();

                iStaticFields += pASSM->staticFieldsCount;
            }
            NANOCLR_FOREACH_ASSEMBLY_END();

            iTotalRamSize = offsets.base + offsets.assemblyRef + offsets.typeRef + offsets.fieldRef +
                            offsets.methodRef + offsets.typeDef + offsets.fieldDef + offsets.methodDef +
                            offsets.genericParam;

#if !defined(NANOCLR_APPDOMAINS)
            iTotalRamSize += offsets.staticFieldsCount;
#endif

            CLR_Debug::Printf(
                "\r\nTotal: (%d RAM - %d ROM - %d METADATA)\r\n\r\n",
                iTotalRamSize,
                iTotalRomSize,
                iMetaData);

            CLR_Debug::Printf(
                "   AssemblyRef     = %6d bytes (%5d elements)\r\n",
                offsets.assemblyRef,
                pTablesSize[TBL_AssemblyRef]);
            CLR_Debug::Printf(
                "   TypeRef         = %6d bytes (%5d elements)\r\n",
                offsets.typeRef,
                pTablesSize[TBL_TypeRef]);
            CLR_Debug::Printf(
                "   FieldRef        = %6d bytes (%5d elements)\r\n",
                offsets.fieldRef,
                pTablesSize[TBL_FieldRef]);
            CLR_Debug::Printf(
                "   MethodRef       = %6d bytes (%5d elements)\r\n",
                offsets.methodRef,
                pTablesSize[TBL_MethodRef]);
            CLR_Debug::Printf(
                "   TypeDef         = %6d bytes (%5d elements)\r\n",
                offsets.typeDef,
                pTablesSize[TBL_TypeDef]);
            CLR_Debug::Printf(
                "   FieldDef        = %6d bytes (%5d elements)\r\n",
                offsets.fieldDef,
                pTablesSize[TBL_FieldDef]);
            CLR_Debug::Printf(
                "   MethodDef       = %6d bytes (%5d elements)\r\n",
                offsets.methodDef,
                pTablesSize[TBL_MethodDef]);
            CLR_Debug::Printf(
                "   GenericParam    = %6d bytes (%5d elements)\r\n",
                offsets.genericParam,
                pTablesSize[TBL_GenericParam]);
            CLR_Debug::Printf(
                "   MethodSpec      = %6d bytes (%5d elements)\r\n",
                offsets.methodSpec,
                pTablesSize[TBL_MethodSpec]);

#if !defined(NANOCLR_APPDOMAINS)
            CLR_Debug::Printf(
                "   StaticFields    = %6d bytes (%5d elements)\r\n",
                offsets.staticFieldsCount,
                iStaticFields);
#endif

            CLR_Debug::Printf("\r\n");

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
            CLR_Debug::Printf("   DebuggingInfo   = %6d bytes\r\n", offsets.debuggingInfoMethods);
            CLR_Debug::Printf("\r\n");
#endif // #if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

            CLR_Debug::Printf(
                "   Attributes      = %6d bytes (%5d elements)\r\n",
                pTablesSize[TBL_Attributes] * sizeof(CLR_RECORD_ATTRIBUTE),
                pTablesSize[TBL_Attributes]);
            CLR_Debug::Printf(
                "   TypeSpec        = %6d bytes (%5d elements)\r\n",
                pTablesSize[TBL_TypeSpec] * sizeof(CLR_RECORD_TYPESPEC),
                pTablesSize[TBL_TypeSpec]);
            CLR_Debug::Printf(
                "   Resources Files = %6d bytes (%5d elements)\r\n",
                pTablesSize[TBL_ResourcesFiles] * sizeof(CLR_RECORD_RESOURCE_FILE),
                pTablesSize[TBL_ResourcesFiles]);
            CLR_Debug::Printf(
                "   Resources       = %6d bytes (%5d elements)\r\n",
                pTablesSize[TBL_Resources] * sizeof(CLR_RECORD_RESOURCE),
                pTablesSize[TBL_Resources]);
            CLR_Debug::Printf("   Resources Data  = %6d bytes\r\n", pTablesSize[TBL_ResourcesData]);
            CLR_Debug::Printf("   Strings         = %6d bytes\r\n", pTablesSize[TBL_Strings]);
            CLR_Debug::Printf("   Signatures      = %6d bytes\r\n", pTablesSize[TBL_Signatures]);
            CLR_Debug::Printf("   ByteCode        = %6d bytes\r\n", pTablesSize[TBL_ByteCode]);
            CLR_Debug::Printf("\r\n\r\n");
        }
    }

#endif

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_TypeSystem::PrepareForExecutionHelper(const char *szAssembly)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_FOREACH_ASSEMBLY(*this)
    {
        if (!strcmp(szAssembly, pASSM->name))
        {
            NANOCLR_CHECK_HRESULT(pASSM->PrepareForExecution());
        }
    }

    NANOCLR_FOREACH_ASSEMBLY_END();

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_TypeSystem::PrepareForExecution()
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
    CLR_EE_DBG_SET(BreakpointsDisabled);
#endif // #if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

#if !defined(NANOCLR_APPDOMAINS)

    // Preemptively create an out of memory exception.
    // We can never get into a case where an out of memory exception cannot be thrown.

    if (g_CLR_RT_ExecutionEngine.m_outOfMemoryException == nullptr)
    {
        CLR_RT_HeapBlock exception;

        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.NewObjectFromIndex(exception, g_CLR_RT_WellKnownTypes.OutOfMemoryException));

        g_CLR_RT_ExecutionEngine.m_outOfMemoryException = exception.Dereference();
    }
#endif

    // Load Runtime.Events to setup EventSink for other assemblies using it
    NANOCLR_CHECK_HRESULT(PrepareForExecutionHelper("nanoFramework.Runtime.Events"));

    // Load Runtime.Native for other assemblies using it
    NANOCLR_CHECK_HRESULT(PrepareForExecutionHelper("nanoFramework.Runtime.Native"));

    NANOCLR_FOREACH_ASSEMBLY(*this)
    {
        NANOCLR_CHECK_HRESULT(pASSM->PrepareForExecution());
    }
    NANOCLR_FOREACH_ASSEMBLY_END();

    NANOCLR_CLEANUP();

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
    CLR_EE_DBG_CLR(BreakpointsDisabled);

    g_CLR_RT_ExecutionEngine.Breakpoint_Assemblies_Loaded();
#endif // #if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

    NANOCLR_CLEANUP_END();
}

//--//

bool CLR_RT_TypeSystem::MatchSignature(CLR_RT_SignatureParser &parserLeft, CLR_RT_SignatureParser &parserRight)
{
    NATIVE_PROFILE_CLR_CORE();

    if (parserLeft.Type != parserRight.Type)
    {
        return false;
    }

    if (parserLeft.Flags != parserRight.Flags)
    {
        return false;
    }

    return MatchSignatureDirect(parserLeft, parserRight, false);
}

bool CLR_RT_TypeSystem::MatchSignatureDirect(
    CLR_RT_SignatureParser &parserLeft,
    CLR_RT_SignatureParser &parserRight,
    bool fIsInstanceOfOK)
{
    NATIVE_PROFILE_CLR_CORE();
    while (true)
    {
        int iAvailLeft = parserLeft.Available();
        int iAvailRight = parserRight.Available();

        if (iAvailLeft != iAvailRight)
        {
            return false;
        }

        if (!iAvailLeft)
        {
            return true;
        }

        CLR_RT_SignatureParser::Element resLeft;
        if (FAILED(parserLeft.Advance(resLeft)))
        {
            return false;
        }

        CLR_RT_SignatureParser::Element resRight;
        if (FAILED(parserRight.Advance(resRight)))
        {
            return false;
        }

        if (!MatchSignatureElement(resLeft, resRight, parserLeft, parserRight, fIsInstanceOfOK))
        {
            return false;
        }
    }

    return true;
}

bool CLR_RT_TypeSystem::MatchSignatureElement(
    CLR_RT_SignatureParser::Element &resLeft,
    CLR_RT_SignatureParser::Element &resRight,
    CLR_RT_SignatureParser &parserLeft,
    CLR_RT_SignatureParser &parserRight,
    bool fIsInstanceOfOK)
{
    NATIVE_PROFILE_CLR_CORE();
    if (fIsInstanceOfOK)
    {
        CLR_RT_ReflectionDef_Index indexLeft;
        CLR_RT_TypeDescriptor descLeft;
        CLR_RT_ReflectionDef_Index indexRight;
        CLR_RT_TypeDescriptor descRight;

        indexLeft.kind = REFLECTION_TYPE;
        indexLeft.levels = resLeft.Levels;
        indexLeft.data.type = resLeft.Class;

        indexRight.kind = REFLECTION_TYPE;
        indexRight.levels = resRight.Levels;
        indexRight.data.type = resRight.Class;

        if (FAILED(descLeft.InitializeFromReflection(indexLeft)))
        {
            return false;
        }

        if (FAILED(descRight.InitializeFromReflection(indexRight)))
        {
            return false;
        }

        if (!CLR_RT_ExecutionEngine::IsInstanceOf(descRight, descLeft, false))
        {
            return false;
        }
    }
    else
    {
        if (resLeft.IsByRef != resRight.IsByRef)
        {
            return false;
        }

        if (resLeft.Levels != resRight.Levels)
        {
            return false;
        }

        if (resLeft.DataType != resRight.DataType)
        {
            return false;
        }

        if (resLeft.Class.data != resRight.Class.data)
        {
            return false;
        }

        if ((resLeft.DataType == DATATYPE_MVAR && resRight.DataType == DATATYPE_MVAR) &&
            (resLeft.GenericParamPosition != resRight.GenericParamPosition))
        {
            return false;
        }

        if (resLeft.DataType == DATATYPE_VAR && resRight.DataType == DATATYPE_VAR)
        {
            if (resLeft.GenericParamPosition != resRight.GenericParamPosition)
            {
                return false;
            }
            else
            {
                if (parserLeft.IsGenericInst && parserRight.IsGenericInst)
                {
                    // TODO: we can do better here by checking the actual type of the generic parameters
                    // CLR_RT_TypeDef_Index leftTypeDef;
                    // NanoCLRDataType leftDT;
                    // CLR_RT_TypeDef_Index rightTypeDef;
                    // NanoCLRDataType rightDT;

                    // parserLeft.Assembly->FindGenericParamAtTypeSpec(
                    //     resLeft.TypeSpec.TypeSpec(),
                    //     resLeft.GenericParamPosition,
                    //     leftTypeDef,
                    //     leftDT);

                    // parserRight.Assembly->FindGenericParamAtTypeSpec(
                    //     resRight.TypeSpec.TypeSpec(),
                    //     resRight.GenericParamPosition,
                    //     rightTypeDef,
                    //     rightDT);

                    // if (leftTypeDef.data != rightTypeDef.data || leftDT != rightDT)
                    //{
                    //     return false;
                    // }
                }
            }
        }

        if (parserLeft.IsGenericInst != parserRight.IsGenericInst)
        {
            return false;
        }

        if (parserLeft.IsGenericInst || parserRight.IsGenericInst)
        {
            if (resLeft.DataType == DATATYPE_GENERICINST && resRight.DataType == DATATYPE_GENERICINST)
            {
                // processing generic instance signature
                // need to advance to get generic type and param count
                if (FAILED(parserLeft.Advance(resLeft)) || FAILED(parserRight.Advance(resRight)))
                {
                    return false;
                }

                // need to check if type of generic parameters match, if there are more
                if (parserLeft.ParamCount > 0 && parserRight.ParamCount > 0)
                {
                    if (parserLeft.ParamCount != parserRight.ParamCount)
                    {
                        return false;
                    }

                    if (resLeft.DataType != resRight.DataType)
                    {
                        return false;
                    }
                }
            }
            else
            {
                if (parserLeft.GenParamCount != parserRight.GenParamCount)
                {
                    return false;
                }
                else if (resLeft.GenericParamPosition != resRight.GenericParamPosition)
                {
                    return false;
                }
            }
        }

        // if (parserLeft.IsGenericInst || parserRight.IsGenericInst)
        //{
        //     if (resLeft.GenericParamPosition == 0xFFFF && resRight.GenericParamPosition == 0xFFFF)
        //     {
        //         // need to check if type of generic parameters match, if there are more
        //         if (parserLeft.ParamCount > 0 && parserRight.ParamCount > 0)
        //         {
        //             if (FAILED(parserLeft.Advance(resLeft)) || FAILED(parserRight.Advance(resRight)))
        //             {
        //                 return false;
        //             }
        //         }

        //        if (resLeft.DataType != resRight.DataType)
        //        {
        //            return false;
        //        }
        //    }
        //    else if (resLeft.GenericParamPosition != resRight.GenericParamPosition)
        //    {
        //        return false;
        //    }
        //}
    }

    return true;
}

//--//

HRESULT CLR_RT_TypeSystem::QueueStringToBuffer(char *&szBuffer, size_t &iBuffer, const char *szText)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    if (szText)
    {
        if (CLR_SafeSprintf(szBuffer, iBuffer, "%s", szText) == false)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }
    }
    else
    {
        szBuffer[0] = 0;
    }

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_TypeSystem::BuildTypeName(
    const CLR_RT_TypeSpec_Index &typeIndex,
    char *&szBuffer,
    size_t &iBuffer,
    CLR_UINT32 levels)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeSpec_Instance instance;
    bool closeGenericSignature = false;

    if (instance.InitializeFromIndex(typeIndex) == false)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    CLR_RT_SignatureParser parser;
    parser.Initialize_TypeSpec(instance.assembly, instance.assembly->GetTypeSpec(typeIndex.TypeSpec()));

    CLR_RT_SignatureParser::Element element;

    // get type
    parser.Advance(element);

    // if this is a generic type, need to advance to get type
    if (element.DataType == DATATYPE_GENERICINST)
    {
        parser.Advance(element);
    }

    CLR_RT_TypeDef_Index typeDef;
    typeDef.data = element.Class.data;

    BuildTypeName(typeDef, szBuffer, iBuffer);

    if (parser.GenParamCount > 0)
    {
        NANOCLR_CHECK_HRESULT(QueueStringToBuffer(szBuffer, iBuffer, "<"));

        closeGenericSignature = true;
    }

    for (int i = 0; i < parser.GenParamCount; i++)
    {
        // read the next element (should be either VAR, MVAR, or a concrete type)
        parser.Advance(element);

        if (element.DataType == DATATYPE_VAR)
        {
            // resolve the !T against our *closed* typeIndex
            CLR_RT_TypeDef_Index realTd;
            NanoCLRDataType realDt;

            // this will bind !T→System.Int32, etc.
            instance.assembly->FindGenericParamAtTypeSpec(
                typeIndex.TypeSpec(),         // closed instantiation row
                element.GenericParamPosition, // the !N slot
                realTd,
                realDt);

            // now print the *actual* type name
            BuildTypeName(realTd, szBuffer, iBuffer);
        }
        else
        {
            // concrete type (e.g. a nested generic, a value type, another class...)
            CLR_RT_TypeDef_Index td;
            td.data = element.Class.data;

            BuildTypeName(td, szBuffer, iBuffer);
        }

        if (i + 1 < parser.GenParamCount)
        {
            NANOCLR_CHECK_HRESULT(QueueStringToBuffer(szBuffer, iBuffer, ","));
        }
    }

    if (closeGenericSignature)
    {
        NANOCLR_CHECK_HRESULT(QueueStringToBuffer(szBuffer, iBuffer, ">"));
    }

    while (levels-- > 0)
    {
        NANOCLR_CHECK_HRESULT(QueueStringToBuffer(szBuffer, iBuffer, "[]"));
    }

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_TypeSystem::BuildTypeName(const CLR_RT_TypeDef_Index &cls, char *&szBuffer, size_t &iBuffer)
{
    NATIVE_PROFILE_CLR_CORE();
    return BuildTypeName(cls, szBuffer, iBuffer, CLR_RT_TypeSystem::TYPENAME_FLAGS_FULL, 0);
}

HRESULT CLR_RT_TypeSystem::BuildTypeName(
    const CLR_RT_TypeDef_Index &cls,
    char *&szBuffer,
    size_t &iBuffer,
    CLR_UINT32 flags,
    CLR_UINT32 levels)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Instance inst{};
    CLR_RT_Assembly *assm;
    const CLR_RECORD_TYPEDEF *td;
    bool fFullName;

    if (inst.InitializeFromIndex(cls) == false)
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);

    assm = inst.assembly;
    td = inst.target;
    fFullName = flags & CLR_RT_TypeSystem::TYPENAME_FLAGS_FULL;

    if (fFullName && td->HasValidEnclosingType())
    {
        CLR_RT_TypeDef_Index clsSub;
        clsSub.Set(inst.Assembly(), td->EnclosingTypeIndex());

        NANOCLR_CHECK_HRESULT(BuildTypeName(clsSub, szBuffer, iBuffer, flags, 0));

        NANOCLR_CHECK_HRESULT(QueueStringToBuffer(
            szBuffer,
            iBuffer,
            (flags & CLR_RT_TypeSystem::TYPENAME_NESTED_SEPARATOR_DOT) ? "." : "+"));
    }

    if (fFullName && td->nameSpace != CLR_EmptyIndex)
    {
        const char *szNameSpace = assm->GetString(td->nameSpace);

        if (szNameSpace[0])
        {
            NANOCLR_CHECK_HRESULT(QueueStringToBuffer(szBuffer, iBuffer, szNameSpace));
            NANOCLR_CHECK_HRESULT(QueueStringToBuffer(szBuffer, iBuffer, "."));
        }
    }

    NANOCLR_CHECK_HRESULT(QueueStringToBuffer(szBuffer, iBuffer, assm->GetString(td->name)));

    while (levels-- > 0)
    {
        NANOCLR_CHECK_HRESULT(QueueStringToBuffer(szBuffer, iBuffer, "[]"));
    }

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_TypeSystem::BuildMethodName(
    const CLR_RT_MethodDef_Index &md,
    const CLR_RT_TypeSpec_Index *genericType,
    char *&szBuffer,
    size_t &iBuffer)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Instance declTypeInst{};
    CLR_RT_MethodDef_Instance tempMD{};
    CLR_RT_MethodDef_Instance inst{};
    CLR_RT_TypeDef_Index declTypeIdx;
    CLR_RT_TypeDef_Instance instOwner{};
    bool useGeneric = false;

    // find out which type declares this method
    if (!tempMD.InitializeFromIndex(md))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    if (!declTypeInst.InitializeFromMethod(tempMD))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    declTypeIdx.Set(md.Assembly(), declTypeInst.assembly->crossReferenceMethodDef[md.Method()].GetOwner());

    if (genericType != nullptr && genericType->data != 0xffffffff)
    {
        // parse TypeSpec to get its TypeDef
        CLR_RT_TypeSpec_Instance tsInst = {};

        if (tsInst.InitializeFromIndex(*genericType))
        {
            if (tsInst.genericTypeDef.Type() == declTypeIdx.Type())
            {
                useGeneric = true;
            }
        }
    }

    if (!useGeneric)
    {
        // fallback: non generic method or a different type
        if (!inst.InitializeFromIndex(md))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }

        if (instOwner.InitializeFromMethod(inst) == false)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }

        NANOCLR_CHECK_HRESULT(BuildTypeName(instOwner, szBuffer, iBuffer));
    }
    else
    {
        // method belong to a closed generic type, so bind it to the appropriate TypeSpec
        if (!inst.InitializeFromIndex(md, *genericType))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }

        NANOCLR_CHECK_HRESULT(BuildTypeName(*genericType, szBuffer, iBuffer, 0));
    }

    CLR_SafeSprintf(szBuffer, iBuffer, "::%s", inst.assembly->GetString(inst.target->name));

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_TypeSystem::BuildFieldName(const CLR_RT_FieldDef_Index &fd, char *&szBuffer, size_t &iBuffer)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_FieldDef_Instance inst{};
    CLR_RT_TypeDef_Instance instOwner{};

    if (inst.InitializeFromIndex(fd) == false)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    if (instOwner.InitializeFromField(inst) == false)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    NANOCLR_CHECK_HRESULT(BuildTypeName(instOwner, szBuffer, iBuffer));

    CLR_SafeSprintf(szBuffer, iBuffer, "::%s", inst.assembly->GetString(inst.target->name));

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_TypeSystem::BuildMethodRefName(const CLR_RT_MethodRef_Index &method, char *&szBuffer, size_t &iBuffer)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_Assembly *assembly = g_CLR_RT_TypeSystem.m_assemblies[method.Assembly() - 1];

    const CLR_RT_MethodRef_CrossReference memberCrossRef = assembly->crossReferenceMethodRef[method.Method()];
    const CLR_RECORD_METHODREF *methodRef = assembly->GetMethodRef(method.Method());

    if (memberCrossRef.genericType.data == CLR_EmptyToken)
    {
        // this is a MethodRef belonging to another assembly

        CLR_RT_MethodDef_Instance mdInstance;
        mdInstance.data = memberCrossRef.target.data;
        mdInstance.assembly = g_CLR_RT_TypeSystem.m_assemblies[mdInstance.Assembly() - 1];
        mdInstance.target = mdInstance.assembly->GetMethodDef(mdInstance.Method());

        CLR_RT_TypeDef_Index typeOwner;
        typeOwner.Set(
            mdInstance.Assembly(),
            mdInstance.assembly->crossReferenceMethodDef[mdInstance.Method()].GetOwner());

        NANOCLR_CHECK_HRESULT(BuildTypeName(typeOwner, szBuffer, iBuffer));

        CLR_SafeSprintf(szBuffer, iBuffer, "::%s", mdInstance.assembly->GetString(mdInstance.target->name));
    }
    else
    {
        // this is a MethodRef for a generic type

        CLR_RT_SignatureParser parser;
        parser.Initialize_TypeSpec(assembly, assembly->GetTypeSpec(memberCrossRef.genericType.TypeSpec()));

        CLR_RT_SignatureParser::Element element;

        // get type
        parser.Advance(element);

        CLR_RT_TypeDef_Index typeDef;
        typeDef.data = element.Class.data;

        BuildTypeName(typeDef, szBuffer, iBuffer);

        CLR_SafeSprintf(szBuffer, iBuffer, "<");

        for (int i = 0; i < parser.GenParamCount; i++)
        {
            parser.Advance(element);

#if defined(VIRTUAL_DEVICE)
            CLR_SafeSprintf(szBuffer, iBuffer, c_CLR_RT_DataTypeLookup[element.DataType].m_name);
#endif

            if (i + 1 < parser.GenParamCount)
            {
                CLR_SafeSprintf(szBuffer, iBuffer, ",");
            }
        }

        CLR_SafeSprintf(szBuffer, iBuffer, ">::%s", assembly->GetString(methodRef->name));
    }

    NANOCLR_NOCLEANUP();
}
HRESULT CLR_RT_TypeSystem::BuildMethodRefName(
    const CLR_RT_MethodRef_Index &mri,
    const CLR_RT_TypeSpec_Index *callerGeneric, // may be nullptr if none
    char *&szBuffer,
    size_t &iBuffer)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    // 1) Grab the assembly that owns this MethodRef
    CLR_RT_Assembly *assembly = g_CLR_RT_TypeSystem.m_assemblies[mri.Assembly() - 1];

    // 2) Pull the raw CLR_RECORD_METHODREF
    const CLR_RECORD_METHODREF *mr = assembly->GetMethodRef(mri.Method());

    // 3) Build the corresponding MethodDef_Index
    //    (MethodRef.method is the metadata token for the definition)
    CLR_RT_MethodDef_Index md;
    md.Set(mri.Assembly(), mr->OwnerIndex());

    // 4) Delegate to your existing BuildMethodName, passing the *declaring type’s* TypeSpec
    NANOCLR_CHECK_HRESULT(BuildMethodName(md, callerGeneric, szBuffer, iBuffer));

    NANOCLR_NOCLEANUP();
}
HRESULT CLR_RT_TypeSystem::BuildMethodSpecName(const CLR_RT_MethodSpec_Index &ms, char *&szBuffer, size_t &iBuffer)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_MethodSpec_Instance inst;
    const CLR_RECORD_METHODSPEC *msRecord;
    CLR_RT_SignatureParser parser;
    CLR_RT_SignatureParser::Element element;
    CLR_RT_TypeDef_Index typeDef;
    CLR_RT_TypeDef_Instance instOwner;
    CLR_RT_Assembly *assembly;

    if (inst.InitializeFromIndex(ms) == false)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    assembly = g_CLR_RT_TypeSystem.m_assemblies[ms.Assembly() - 1];

    msRecord = inst.assembly->GetMethodSpec(ms.Method());

    parser.Initialize_TypeSpec(inst.assembly, inst.assembly->GetTypeSpec(msRecord->container));

    // get type
    parser.Advance(element);

    typeDef.data = element.Class.data;

    if (instOwner.InitializeFromIndex(typeDef) == false)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    BuildTypeName(typeDef, szBuffer, iBuffer);

    CLR_SafeSprintf(szBuffer, iBuffer, "<");

    parser.Initialize_MethodSignature(&inst);

    for (int i = 0; i < parser.GenParamCount; i++)
    {
        parser.Advance(element);

#if defined(VIRTUAL_DEVICE)
        NANOCLR_CHECK_HRESULT(QueueStringToBuffer(szBuffer, iBuffer, c_CLR_RT_DataTypeLookup[element.DataType].m_name));
#endif

        if (i + 1 < parser.GenParamCount)
        {
            NANOCLR_CHECK_HRESULT(QueueStringToBuffer(szBuffer, iBuffer, ","));
        }
    }

    CLR_SafeSprintf(szBuffer, iBuffer, ">::");

    switch (msRecord->MethodKind())
    {
        case TBL_MethodDef:
        {
            const CLR_RECORD_METHODDEF *md = assembly->GetMethodDef(msRecord->MethodIndex());
            CLR_SafeSprintf(szBuffer, iBuffer, "%s", assembly->GetString(md->name));
            break;
        }
        case TBL_MethodRef:
        {
            const CLR_RT_MethodRef_CrossReference memberCrossRef =
                assembly->crossReferenceMethodRef[msRecord->MethodIndex()];

            CLR_RT_MethodDef_Instance mdInstance;
            mdInstance.data = memberCrossRef.target.data;
            mdInstance.assembly = g_CLR_RT_TypeSystem.m_assemblies[mdInstance.Assembly() - 1];
            mdInstance.target = mdInstance.assembly->GetMethodDef(mdInstance.Method());

            CLR_SafeSprintf(
                szBuffer,
                iBuffer,
                "%s",
                mdInstance.assembly->GetString(mdInstance.CrossReference().GetOwner()));
            break;
        }
        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    NANOCLR_NOCLEANUP();
}
HRESULT CLR_RT_TypeSystem::BuildMethodSpecName(
    const CLR_RT_MethodSpec_Index &msi,
    const CLR_RT_TypeSpec_Index *callerGeneric, // may be nullptr if none
    char *&szBuffer,
    size_t &iBuffer)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    // 1) Grab the assembly that owns this MethodSpec
    CLR_RT_Assembly *assembly = g_CLR_RT_TypeSystem.m_assemblies[msi.Assembly() - 1];

    // 2) Pull the raw CLR_RECORD_METHODSPEC
    const CLR_RECORD_METHODSPEC *ms = assembly->GetMethodSpec(msi.Method());

    // 3) Build the corresponding MethodDef_Index
    //    (MethodSpec.method is the metadata token for the definition)
    CLR_RT_MethodDef_Index md;
    md.Set(msi.Assembly(), ms->MethodIndex());

    // 4) Delegate to BuildMethodName, again passing the closed declaring‐type
    NANOCLR_CHECK_HRESULT(BuildMethodName(md, callerGeneric, szBuffer, iBuffer));

    NANOCLR_NOCLEANUP();
}
//--//

bool CLR_RT_TypeSystem::FindVirtualMethodDef(
    const CLR_RT_TypeDef_Index &cls,
    const CLR_RT_MethodDef_Index &calleeMD,
    CLR_RT_MethodDef_Index &index)
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_MethodDef_Instance calleeInst{};

    if (calleeInst.InitializeFromIndex(calleeMD))
    {
        const char *calleeName = calleeInst.assembly->GetString(calleeInst.target->name);

        CLR_RT_TypeDef_Instance inst{};
        inst.InitializeFromMethod(calleeInst);

        if ((inst.target->flags & CLR_RECORD_TYPEDEF::TD_Semantics_Mask) == CLR_RECORD_TYPEDEF::TD_Semantics_Interface)
        {
            //
            // It's an interface method, it could be that the class is implementing explicitly the method.
            // Prepend the Interface name to the method name and try again.
            //
            char rgBuffer[512];
            char *szBuffer = rgBuffer;
            size_t iBuffer = MAXSTRLEN(rgBuffer);

            BuildTypeName(
                inst,
                szBuffer,
                iBuffer,
                CLR_RT_TypeSystem::TYPENAME_FLAGS_FULL | CLR_RT_TypeSystem::TYPENAME_NESTED_SEPARATOR_DOT,
                0);
            QueueStringToBuffer(szBuffer, iBuffer, ".");
            QueueStringToBuffer(szBuffer, iBuffer, calleeName);

            if (FindVirtualMethodDef(cls, calleeMD, rgBuffer, index))
                return true;
        }

        if (FindVirtualMethodDef(cls, calleeMD, calleeName, index))
            return true;
    }

    index.Clear();

    return false;
}

bool CLR_RT_TypeSystem::FindVirtualMethodDef(
    const CLR_RT_TypeDef_Index &cls,
    const CLR_RT_MethodDef_Index &calleeMD,
    const char *calleeName,
    CLR_RT_MethodDef_Index &index)
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_TypeDef_Instance clsInst{};
    clsInst.InitializeFromIndex(cls);
    CLR_RT_MethodDef_Instance calleeInst{};
    calleeInst.InitializeFromIndex(calleeMD);

    const CLR_RECORD_METHODDEF *calleeMDR = calleeInst.target;
    CLR_UINT8 calleeArgumentsCount = calleeMDR->argumentsCount;

    while (NANOCLR_INDEX_IS_VALID(clsInst))
    {
        CLR_RT_Assembly *targetAssm = clsInst.assembly;
        const CLR_RECORD_TYPEDEF *targetTDR = clsInst.target;
        const CLR_RECORD_METHODDEF *targetMDR = targetAssm->GetMethodDef(targetTDR->firstMethod);
        int num = targetTDR->virtualMethodCount + targetTDR->instanceMethodCount;

        for (int i = 0; i < num; i++, targetMDR++)
        {
            if (targetMDR == calleeMDR)
            {
                // Shortcut for identity.
                index = calleeInst;
                return true;
            }

            if (targetMDR->argumentsCount == calleeArgumentsCount &&
                (targetMDR->flags & CLR_RECORD_METHODDEF::MD_Abstract) == 0)
            {
                const char *targetName = targetAssm->GetString(targetMDR->name);

                if (!strcmp(targetName, calleeName))
                {
                    CLR_RT_SignatureParser parserLeft{};
                    parserLeft.Initialize_MethodSignature(&calleeInst);
                    CLR_RT_SignatureParser parserRight{};
                    parserRight.Initialize_MethodSignature(targetAssm, targetMDR);

                    if (CLR_RT_TypeSystem::MatchSignature(parserLeft, parserRight))
                    {
                        index.Set(targetAssm->assemblyIndex, i + targetTDR->firstMethod);

                        return true;
                    }
                }
            }
        }

        clsInst.SwitchToParent();
    }

    index.Clear();

    return false;
}

NanoCLRDataType CLR_RT_TypeSystem::MapElementTypeToDataType(CLR_UINT32 et)
{
    NATIVE_PROFILE_CLR_CORE();
    const CLR_RT_DataTypeLookup *ptr = c_CLR_RT_DataTypeLookup;

    for (CLR_UINT32 num = 0; num < DATATYPE_FIRST_INVALID; num++, ptr++)
    {
        if (ptr->m_convertToElementType == et)
            return (NanoCLRDataType)num;
    }

    if (et == ELEMENT_TYPE_I)
        return DATATYPE_I4;
    if (et == ELEMENT_TYPE_U)
        return DATATYPE_U4;

    return DATATYPE_FIRST_INVALID;
}

CLR_UINT32 CLR_RT_TypeSystem::MapDataTypeToElementType(NanoCLRDataType dt)
{
    NATIVE_PROFILE_CLR_CORE();
    return c_CLR_RT_DataTypeLookup[dt].m_convertToElementType;
}

//--//

void CLR_RT_AttributeEnumerator::Initialize(CLR_RT_Assembly *assm)
{
    NATIVE_PROFILE_CLR_CORE();
    m_assm = assm;   // CLR_RT_Assembly*            m_assm;
    m_ptr = nullptr; // const CLR_RECORD_ATTRIBUTE* m_ptr;
    m_num = 0;       // int                         m_num;
                     // CLR_RECORD_ATTRIBUTE        m_data;
    m_match.Clear(); // CLR_RT_MethodDef_Index      m_match;
}

void CLR_RT_AttributeEnumerator::Initialize(const CLR_RT_TypeDef_Instance &inst)
{
    NATIVE_PROFILE_CLR_CORE();
    m_data.ownerType = TBL_TypeDef;
    m_data.ownerIndex = inst.Type();

    Initialize(inst.assembly);
}

void CLR_RT_AttributeEnumerator::Initialize(const CLR_RT_FieldDef_Instance &inst)
{
    NATIVE_PROFILE_CLR_CORE();
    m_data.ownerType = TBL_FieldDef;
    m_data.ownerIndex = inst.Field();

    Initialize(inst.assembly);
}

void CLR_RT_AttributeEnumerator::Initialize(const CLR_RT_MethodDef_Instance &inst)
{
    NATIVE_PROFILE_CLR_CORE();
    m_data.ownerType = TBL_MethodDef;
    m_data.ownerIndex = inst.Method();

    Initialize(inst.assembly);
}

bool CLR_RT_AttributeEnumerator::Advance()
{
    NATIVE_PROFILE_CLR_CORE();
    const CLR_RECORD_ATTRIBUTE *ptr = m_ptr;
    int num = m_num;
    CLR_UINT32 key = m_data.Key();
    bool fRes = false;

    if (ptr == nullptr)
    {
        ptr = m_assm->GetAttribute(0) - 1;
        num = m_assm->tablesSize[TBL_Attributes];
    }

    while (num)
    {
        ptr++;
        num--;

        if (ptr->Key() == key && ptr->ownerIndex == m_data.ownerIndex)
        {
            // get the token of the constructor (compressed format)
            CLR_UINT32 token = CLR_UncompressMethodToken(ptr->constructor);

            // get the method definition, by resolving the token
            CLR_RT_MethodDef_Instance method{};
            if (method.ResolveToken(token, m_assm, nullptr) == false)
            {
                ASSERT(0);
            }

            // get the method definition index
            m_match.data = method.data;

            m_blob = m_assm->GetSignature(ptr->data);

            fRes = true;
            break;
        }
    }

    m_ptr = ptr;
    m_num = num;

    return fRes;
}

void CLR_RT_AttributeEnumerator::GetCurrent(CLR_RT_TypeDef_Instance *instTD)
{
    CLR_RT_MethodDef_Instance md{};

    md.InitializeFromIndex(m_match);
    instTD->InitializeFromMethod(md);
}

bool CLR_RT_AttributeEnumerator::MatchNext(
    const CLR_RT_TypeDef_Instance *instTD,
    const CLR_RT_MethodDef_Instance *instMD)
{
    NATIVE_PROFILE_CLR_CORE();
    while (Advance())
    {
        if (instMD)
        {
            if (m_match.data != instMD->data)
                continue;
        }

        if (instTD)
        {
            CLR_RT_MethodDef_Instance md{};
            CLR_RT_TypeDef_Instance td{};

            md.InitializeFromIndex(m_match);
            td.InitializeFromMethod(md);

            if (td.data != instTD->data)
                continue;
        }

        return true;
    }

    return false;
}

//--//

HRESULT CLR_RT_AttributeParser::Initialize(const CLR_RT_AttributeEnumerator &en)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    if (m_md.InitializeFromIndex(en.m_match) == false || m_td.InitializeFromMethod(m_md) == false)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    m_parser.Initialize_MethodSignature(&m_md);
    m_parser.Advance(m_res); // Skip return value.

    m_assm = en.m_assm;
    m_blob = en.m_blob;

    m_currentPos = 0;
    m_fixed_Count = m_md.target->argumentsCount - 1;
    m_named_Count = -1;
    m_constructorParsed = false;
    m_mdIndex = en.m_match;

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_AttributeParser::Next(Value *&res)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    if (m_currentPos == m_fixed_Count)
    {
        NANOCLR_READ_UNALIGNED_UINT16(m_named_Count, m_blob);
    }

    if (m_fixed_Count == 0 && m_named_Count == 0 && !m_constructorParsed)
    {
        // Attribute class has no fields, no properties and only default constructor

        m_lastValue.m_mode = Value::c_DefaultConstructor;
        m_lastValue.m_name = nullptr;

        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObject(m_lastValue.m_value, m_td));

        res = &m_lastValue;

        m_constructorParsed = true;

        NANOCLR_SET_AND_LEAVE(S_OK);
    }
    else if ((m_currentPos < m_fixed_Count) && !m_constructorParsed)
    {
        // Attribute class has a constructor with parameter(s)

        m_lastValue.m_mode = Value::c_ConstructorArgument;
        m_lastValue.m_name = nullptr;

        // get type
        NANOCLR_CHECK_HRESULT(m_parser.Advance(m_res));

        CLR_RT_DataTypeLookup dtl = c_CLR_RT_DataTypeLookup[m_res.DataType];

        ////////////////////////////////////////////////
        // now read the arguments from the blob

        if ((m_res.DataType == DATATYPE_OBJECT && m_res.Levels == 1) || m_fixed_Count > 1)
        {
            // this is either an array of objects or a constructor with multiple parameters

            uint8_t paramCount;

            if (m_res.DataType == DATATYPE_OBJECT && m_res.Levels == 1)
            {
                // get element count of array (from blob)
                NANOCLR_READ_UNALIGNED_UINT8(paramCount, m_blob);
            }
            else
            {
                // load parameter count from constructor info because this is not an array of objects
                paramCount = m_fixed_Count;
            }

            // instantiate array to hold parameters values
            NANOCLR_CHECK_HRESULT(
                CLR_RT_HeapBlock_Array::CreateInstance(
                    m_lastValue.m_value,
                    paramCount,
                    g_CLR_RT_WellKnownTypes.Object));

            // get a pointer to the first element
            auto *currentParam = (CLR_RT_HeapBlock *)m_lastValue.m_value.DereferenceArray()->GetFirstElement();

            do
            {
                // read element type
                uint8_t elementType;
                NANOCLR_READ_UNALIGNED_UINT8(elementType, m_blob);

                NanoCLRDataType dt = CLR_RT_TypeSystem::MapElementTypeToDataType(elementType);

                dtl = c_CLR_RT_DataTypeLookup[dt];

                if (dtl.m_flags & CLR_RT_DataTypeLookup::c_Numeric)
                {
                    CLR_UINT32 size = dtl.m_sizeInBytes;

                    NANOCLR_CHECK_HRESULT(ReadNumericValue(currentParam, dt, dtl.m_cls, size));
                }
                else if (dt == DATATYPE_STRING)
                {
                    NANOCLR_CHECK_HRESULT(ReadString(currentParam));
                }
                else
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
                }

                // move pointer to next array element
                currentParam++;

            } while (--paramCount > 0);
        }
        else if (dtl.m_flags & CLR_RT_DataTypeLookup::c_Numeric)
        {
            // single parameter of numeric type
            // OK to skip element type
            m_blob += sizeof(CLR_UINT8);

            // size of value
            CLR_UINT32 size = dtl.m_sizeInBytes;

            CLR_RT_HeapBlock *currentParam = &m_lastValue.m_value;
            NANOCLR_CHECK_HRESULT(ReadNumericValue(currentParam, m_res.DataType, dtl.m_cls, size));
        }
        else if (m_res.DataType == DATATYPE_STRING)
        {
            // single parameter of string type
            // OK to skip element type
            m_blob += sizeof(CLR_UINT8);

            CLR_RT_HeapBlock *currentParam = &m_lastValue.m_value;
            NANOCLR_CHECK_HRESULT(ReadString(currentParam));
        }
        else
        {
            // whatever this parameter is we don't have support for it
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }

        res = &m_lastValue;

        m_constructorParsed = true;

        NANOCLR_SET_AND_LEAVE(S_OK);
    }
    else if (m_currentPos < m_fixed_Count + m_named_Count && !m_constructorParsed)
    {
        // Attribute class has named fields

        CLR_UINT32 kind;
        NANOCLR_READ_UNALIGNED_UINT8(kind, m_blob);

        m_lastValue.m_name = GetString();

        if (kind == SERIALIZATION_TYPE_FIELD)
        {
            CLR_RT_FieldDef_Index fd;
            CLR_RT_FieldDef_Instance inst;

            m_lastValue.m_mode = Value::c_NamedField;

            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.FindFieldDef(m_td, m_lastValue.m_name, fd));

            inst.InitializeFromIndex(fd);

            m_parser.Initialize_FieldDef(inst.assembly, inst.target);
        }
        else
        {
            m_lastValue.m_mode = Value::c_NamedProperty;

            //
            // it's supposed to reach here when there is an attribute contructor
            // but that is already handled upwards
            // leaving this here waiting for a special case that hits here (if there is one...)
            //
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }
    }
    else
    {
        res = nullptr;
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    NANOCLR_CHECK_HRESULT(m_parser.Advance(m_res));

    res = &m_lastValue;

    //
    // Check for Enums.
    //
    if (m_res.DataType == DATATYPE_VALUETYPE)
    {
        CLR_RT_TypeDef_Instance td{};
        td.InitializeFromIndex(m_res.Class);

        if ((td.target->flags & CLR_RECORD_TYPEDEF::TD_Semantics_Mask) == CLR_RECORD_TYPEDEF::TD_Semantics_Enum)
        {
            m_res.DataType = (NanoCLRDataType)td.target->dataType;
        }
    }

    //
    // Skip value info.
    //
    m_blob += sizeof(CLR_UINT8);

    {
        const CLR_RT_DataTypeLookup &dtl = c_CLR_RT_DataTypeLookup[m_res.DataType];

        if (dtl.m_flags & CLR_RT_DataTypeLookup::c_Numeric)
        {
            CLR_UINT32 size = dtl.m_sizeInBytes;

            CLR_RT_HeapBlock *currentParam = &m_lastValue.m_value;
            NANOCLR_CHECK_HRESULT(ReadNumericValue(currentParam, m_res.DataType, dtl.m_cls, size));
        }
        else if (m_res.DataType == DATATYPE_STRING)
        {
            CLR_RT_HeapBlock *currentParam = &m_lastValue.m_value;
            NANOCLR_CHECK_HRESULT(ReadString(currentParam));
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }
    }

    m_lastValue.m_pos = m_currentPos++;

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_AttributeParser::ReadString(CLR_RT_HeapBlock *&value)
{
    NANOCLR_HEADER();

    CLR_UINT32 tk;

    CLR_RT_TypeDescriptor desc{};
    NANOCLR_CHECK_HRESULT(desc.InitializeFromType(g_CLR_RT_WellKnownTypes.String));

    NANOCLR_READ_UNALIGNED_UINT16(tk, m_blob);

    // check for invalid string ID (0xFFFF) meaning "null"
    if (tk == 0xFFFF)
    {
        // create a string
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(*value, nullptr, m_assm));

        // null the object so it's a real null
        value->SetObjectReference(nullptr);
    }
    else
    {
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(*value, CLR_TkFromType(TBL_Strings, tk), m_assm));

        // need to box this
        value->PerformBoxing(desc.m_handlerCls);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_AttributeParser::ReadNumericValue(
    CLR_RT_HeapBlock *&value,
    const NanoCLRDataType dt,
    const CLR_RT_TypeDef_Index *m_cls,
    const CLR_UINT32 size)
{
    NANOCLR_HEADER();

    CLR_RT_TypeDescriptor desc{};
    NANOCLR_CHECK_HRESULT(desc.InitializeFromType(*m_cls));

    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*value, g_CLR_RT_WellKnownTypes.TypeStatic));

    // need to setup reflection and data type Id to properly setup the object
    value->SetReflection(*m_cls);

    value->SetDataId(CLR_RT_HEAPBLOCK_RAW_ID(dt, 0, 1));

    if (dt == DATATYPE_BOOLEAN)
    {
        uint8_t boolValue;
        NANOCLR_READ_UNALIGNED_UINT8(boolValue, m_blob);

        value->SetBoolean((bool)boolValue);
    }
    else
    {
        // because this is a numeric object, perform a a raw copy of the numeric value data from the
        // blob to the return value
        memcpy((CLR_UINT8 *)&value->NumericByRef(), m_blob, size);

        m_blob += size;
    }

    // need to box this
    value->PerformBoxing(desc.m_handlerCls);

    NANOCLR_NOCLEANUP();
}

const char *CLR_RT_AttributeParser::GetString()
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_UINT32 tk;
    NANOCLR_READ_UNALIGNED_UINT16(tk, m_blob);

    return m_assm->GetString(tk);
}
