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
int s_CLR_RT_fTrace_Instructions = NANOCLR_TRACE_DEFAULT(c_CLR_RT_Trace_Verbose, c_CLR_RT_Trace_None);
#endif

#if defined(NANOCLR_GC_VERBOSE)
int s_CLR_RT_fTrace_Memory = NANOCLR_TRACE_DEFAULT(c_CLR_RT_Trace_Verbose, c_CLR_RT_Trace_None);
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

#if defined(NANOCLR_TRACE_GENERICS)
int s_CLR_RT_fTrace_GenericFields = NANOCLR_TRACE_DEFAULT(c_CLR_RT_Trace_Verbose, c_CLR_RT_Trace_Info);
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

bool CLR_RT_ReflectionDef_Index::GetGenericOwner(CLR_RT_TypeSpec_Index &typeSpec) const
{
    NATIVE_PROFILE_CLR_CORE();

    const CLR_RT_GenericInstance *record =
        (kind == REFLECTION_METHOD || kind == REFLECTION_CONSTRUCTOR || kind == REFLECTION_FIELD)
            ? CLR_RT_TypeSystem::GetMemberRecord(genericOwner, false)
            : nullptr;

    if (record == nullptr)
    {
        return false;
    }

    typeSpec = record->m_self;

    return true;
}

bool CLR_RT_ReflectionDef_Index::GetGenericMethodSpec(CLR_RT_MethodSpec_Index &methodSpec) const
{
    NATIVE_PROFILE_CLR_CORE();

    const CLR_RT_GenericInstance *record =
        kind == REFLECTION_METHOD ? CLR_RT_TypeSystem::GetMemberRecord(genericOwner, true) : nullptr;

    if (record == nullptr)
    {
        return false;
    }

    methodSpec.data = record->m_self.data;

    return true;
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
    m_pendingGenericInst = false;
}

void CLR_RT_SignatureParser::Initialize_TypeSpec(CLR_RT_TypeSpec_Instance tsInstance)
{
    NATIVE_PROFILE_CLR_CORE();

    Assembly = tsInstance.assembly;
    Type = CLR_RT_SignatureParser::c_TypeSpec;
    Flags = 0;
    ParamCount = 1;
    GenParamCount = 0;
    m_pendingGenericInst = false;

    if (tsInstance.IsRuntime())
    {
        CLR_RT_GenericInstance *instance = CLR_RT_TypeSystem::GetGenericInstance(tsInstance);

        Type = CLR_RT_SignatureParser::c_RuntimeTypeSpec;
        Signature = instance != nullptr ? (CLR_PMETADATA)instance->m_nodes : nullptr;
        m_runtimeNodesLeft = instance != nullptr ? instance->m_nodeCount : 0;

        return;
    }

    Signature = Assembly->GetSignature(tsInstance.target->signature);
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
    m_pendingGenericInst = false;
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
    m_pendingGenericInst = false;
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
    m_pendingGenericInst = false;
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
    m_pendingGenericInst = false;
}

void CLR_RT_SignatureParser::Initialize_MethodSignature(CLR_RT_MethodSpec_Instance *ms)
{
    NATIVE_PROFILE_CLR_CORE();

    if (ms->IsRuntime())
    {
        CLR_RT_TypeSpec_Index recordIndex;
        recordIndex.data = ms->data;
        CLR_RT_GenericInstance *instance = CLR_RT_TypeSystem::GetGenericInstance(recordIndex);

        Method = 0;
        Assembly = nullptr;
        Flags = PIMAGE_CEE_CS_CALLCONV_GENERICINST;
        Type = CLR_RT_SignatureParser::c_RuntimeTypeSpec;
        Signature = instance != nullptr ? (CLR_PMETADATA)(instance->m_nodes + 1) : nullptr;
        m_runtimeNodesLeft = instance != nullptr ? instance->m_nodeCount - 1 : 0;
        ParamCount = instance != nullptr ? instance->m_nodes[0].ArgCount : 0;
        GenParamCount = ParamCount;
        m_pendingGenericInst = false;

        return;
    }

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
    m_pendingGenericInst = false;
}

//--//

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
    m_pendingGenericInst = false;
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
    m_pendingGenericInst = false;
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
    m_pendingGenericInst = false;
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
    res.IsGenericInst = 0;
    res.GenParamCount = 0;

    switch (Type)
    {
        case c_RuntimeTypeSpec:
        {
            const CLR_RT_GenericInstanceNode *node = (const CLR_RT_GenericInstanceNode *)Signature;

            if (m_runtimeNodesLeft == 0)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
            }

            res.DataType = (NanoCLRDataType)node->DataType;
            res.Class = node->Class;

            if (m_pendingGenericInst)
            {
                m_pendingGenericInst = false;
                res.GenParamCount = node->ArgCount;
                ParamCount += node->ArgCount;
            }
            else if (node->ArgCount > 0)
            {
                res.DataType = DATATYPE_GENERICINST;
                res.Class.Clear();
                res.Levels = node->Levels;
                res.IsGenericInst = true;
                m_pendingGenericInst = true;
                ParamCount++;

                NANOCLR_SET_AND_LEAVE(S_OK);
            }
            else
            {
                res.Levels = node->Levels;

                if (res.DataType == DATATYPE_VAR || res.DataType == DATATYPE_MVAR)
                {
                    res.GenericParamPosition = node->Position;
                }
            }

            Signature = (CLR_PMETADATA)(node + 1);
            m_runtimeNodesLeft--;
        }
        break;

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
                    case ELEMENT_TYPE_CMOD_REQD:
                    case ELEMENT_TYPE_CMOD_OPT:
                        // Consume the modifier type token (TypeDefOrRef coded index)
                        // Format: CMOD_REQD/OPT + token + actual_type
                        // We don't need to do anything with the modifier itself,
                        // just skip it to get to the actual type
                        CLR_TkFromStream(Signature);
                        // Continue loop to read the actual element type
                        break;

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

                        // check if this type is a generic instance
                        CLR_RT_TypeDef_Instance cls{};
                        cls.InitializeFromIndex(res.Class);

                        // Read arg_count if this element follows a GENERICINST marker or the
                        // TypeDef declares its own generic params. m_pendingGenericInst is checked
                        // first so cls.target is not dereferenced when InitializeFromIndex failed.
                        // See CLAUDE.md "Signature parsing".
                        if (m_pendingGenericInst || (cls.target != nullptr && cls.target->genericParamCount > 0))
                        {
                            // reset the generic instance flag
                            res.IsGenericInst = false;

                            // get generic arguments count
                            res.GenParamCount = (int)*Signature++;

                            // update parser param counter
                            ParamCount += res.GenParamCount;
                        }

                        // consumed — clear for subsequent elements
                        m_pendingGenericInst = false;

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
                        res.IsGenericInst = true;

                        // Signal that the very next CLASS/VALUETYPE element must consume its
                        // arg-count byte unconditionally (even for nested types with 0 own params).
                        m_pendingGenericInst = true;

                        // update parser param counter
                        ParamCount++;

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

    NANOCLR_CLEANUP();
#if defined(NANOCLR_TRACE_GENERICS)
    if (s_CLR_RT_fTrace_GenericFields >= c_CLR_RT_Trace_Info)
    {
        CLR_Debug::Printf(
            "[DIAG] SigParser::Advance hr=%08X Type=%d DataType=%02X Levels=%d IsByRef=%d "
            "IsGenericInst=%d GenParamCount=%d GenParamPos=%04X Class.data=%08X\r\n",
            (unsigned)hr,
            (int)Type,
            (unsigned)res.DataType,
            (int)res.Levels,
            (int)res.IsByRef,
            (int)res.IsGenericInst,
            (int)res.GenParamCount,
            (unsigned)res.GenericParamPosition,
            (unsigned)res.Class.data);
    }
#endif
    NANOCLR_CLEANUP_END();
}

// Drain one full GENERICINST sub-tree from `parser` by advancing until Available()
// converges with `targetAvail`. Relies on the parser's ParamCount bookkeeping rather
// than counting GenParamCount by hand — see CLAUDE.md "Signature parsing".
static bool DrainGenericInstSubtree(CLR_RT_SignatureParser &parser, int targetAvail)
{
    while (parser.Available() > targetAvail)
    {
        CLR_RT_SignatureParser::Element drained{};

        if (FAILED(parser.Advance(drained)))
        {
            return false;
        }
    }

    return true;
}

HRESULT CLR_RT_SignatureParser::AdvanceParameter(Element &res)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    // See CLAUDE.md "Reflection invocation and casts".
    int end = Available() - 1;

    NANOCLR_CHECK_HRESULT(Advance(res));

    if (res.DataType == DATATYPE_GENERICINST)
    {
        bool isByRef = res.IsByRef;
        int levels = res.Levels;

        NANOCLR_CHECK_HRESULT(Advance(res));

        res.IsByRef = isByRef;
        res.Levels = levels;
    }

    if (!DrainGenericInstSubtree(*this, end))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    NANOCLR_NOCLEANUP();
}

static bool SkipParameters(CLR_RT_SignatureParser &parser, CLR_INT32 count)
{
    CLR_RT_SignatureParser::Element skipped;

    for (CLR_INT32 i = 0; i < count; i++)
    {
        if (FAILED(parser.AdvanceParameter(skipped)))
        {
            return false;
        }
    }

    return true;
}

static bool PositionAtGenericParam(
    CLR_RT_TypeSpec_Instance &typeSpec,
    CLR_INT32 parameterPosition,
    CLR_RT_SignatureParser &parser)
{
    CLR_RT_SignatureParser::Element element;

    parser.Initialize_TypeSpec(typeSpec);

    if (FAILED(parser.Advance(element)) || element.DataType != DATATYPE_GENERICINST)
    {
        return false;
    }

    if (FAILED(parser.Advance(element)) || parameterPosition >= element.GenParamCount)
    {
        return false;
    }

    return SkipParameters(parser, parameterPosition);
}

static bool PositionAtGenericArgument(
    CLR_RT_MethodSpec_Instance &methodSpec,
    CLR_INT32 argumentPosition,
    CLR_RT_SignatureParser &parser)
{
    parser.Initialize_MethodSignature(&methodSpec);

    if (argumentPosition >= parser.ParamCount)
    {
        return false;
    }

    return SkipParameters(parser, argumentPosition);
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
    if (index.IsRuntime())
    {
        CLR_RT_GenericInstance *instance = CLR_RT_TypeSystem::GetGenericInstance(index);

        if (instance == nullptr)
        {
            ClearInstance();

            return false;
        }

        data = index.data;
        genericTypeDef = instance->m_nodes[0].Class;
        assembly = g_CLR_RT_TypeSystem.m_assemblies[genericTypeDef.Assembly() - 1];
        target = nullptr;
        levels = 0;
        cachedElementType.Clear();

        return true;
    }

    if (NANOCLR_INDEX_IS_VALID(index))
    {
        data = index.data;
        assembly = g_CLR_RT_TypeSystem.m_assemblies[Assembly() - 1];
        target = assembly->GetTypeSpec(TypeSpec());

        CLR_RT_SignatureParser parser;
        parser.Initialize_TypeSpec(assembly, assembly->GetTypeSpec(index.TypeSpec()));

        CLR_RT_SignatureParser::Element element{};

        if (FAILED(parser.Advance(element)))
        {
            ClearInstance();

            return false;
        }

        levels = element.Levels;

        if (element.DataType == DATATYPE_GENERICINST)
        {
            // this is a generic instance, advance one more time to get the generic type definition
            if (FAILED(parser.Advance(element)))
            {
                ClearInstance();

                return false;
            }

            genericTypeDef = element.Class;
        }
        else
        {
            genericTypeDef.Clear();
        }

        return true;
    }

    ClearInstance();

    return false;
}

void CLR_RT_TypeSpec_Instance::ClearInstance()
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_TypeSpec_Index::Clear();

    assembly = nullptr;
    target = nullptr;
    levels = 0;
    genericTypeDef.Clear();
    data = 0;
}

bool CLR_RT_TypeSpec_Instance::ResolveToken(
    CLR_UINT32 token,
    CLR_RT_Assembly *assm,
    const CLR_RT_MethodDef_Instance *caller)
{
    NATIVE_PROFILE_CLR_CORE();
    if (assm && CLR_TypeFromTk(token) == TBL_TypeSpec)
    {
        CLR_UINT32 index = CLR_DataFromTk(token);

        Set(assm->assemblyIndex, index);

        assembly = assm;
        target = assm->GetTypeSpec(index);

        CLR_RT_SignatureParser parser;
        parser.Initialize_TypeSpec(assembly, assembly->GetTypeSpec(index));

        CLR_RT_SignatureParser::Element element;

        // if this is a generic, advance another one
        if (FAILED(parser.Advance(element)))
        {
            ClearInstance();

            return false;
        }

        levels = element.Levels;

        if (element.DataType == DATATYPE_GENERICINST)
        {
            // this is a generic instance, so we need to advance one more time
            if (FAILED(parser.Advance(element)))
            {
                ClearInstance();

                return false;
            }

            genericTypeDef = element.Class;
        }
        else if (element.DataType == DATATYPE_VAR)
        {
            // this is type‐generic slot (!T), resolve against the caller's closed generic

            int pos = element.GenericParamPosition;

            if (caller != nullptr && NANOCLR_INDEX_IS_VALID(caller->arrayElementType) && pos == 0)
            {
                cachedElementType = caller->arrayElementType;
                genericTypeDef.Clear();

                return true;
            }

            // Use the *caller's* bound genericType (Stack<Int32>, etc.)
            if (caller == nullptr || caller->genericType == nullptr)
            {
                ClearInstance();

                return false;
            }

            CLR_RT_TypeSpec_Instance callerTypeSpec;
            if (!callerTypeSpec.InitializeFromIndex(*caller->genericType))
            {
                ClearInstance();
                return false;
            }

            CLR_RT_SignatureParser::Element paramElement;
            if (!callerTypeSpec.GetGenericParam((CLR_UINT32)pos, paramElement))
            {
                ClearInstance();
                return false;
            }

            // Use the resolved parameter's type for this TypeSpec
            data = caller->genericType->data;
            assembly = callerTypeSpec.assembly;
            target = callerTypeSpec.target;
            cachedElementType = paramElement.Class;
        }
        else if (element.DataType == DATATYPE_MVAR)
        {
            if (caller != nullptr && NANOCLR_INDEX_IS_VALID(caller->arrayElementType) &&
                element.GenericParamPosition == 0)
            {
                cachedElementType = caller->arrayElementType;
                genericTypeDef.Clear();
            }
            else
            {
                ClearInstance();
                return false;
            }
        }
        else
        {
            cachedElementType = element.Class;

            genericTypeDef.Clear();
        }

        return true;
    }

    ClearInstance();

    return false;
}

bool CLR_RT_TypeSpec_Instance::IsClosedGenericType()
{
    if (!NANOCLR_INDEX_IS_VALID(genericTypeDef))
    {
        return false;
    }

    if (target == nullptr)
    {
        const CLR_RT_GenericInstance *record = CLR_RT_TypeSystem::GetGenericInstance(*this);

        return record != nullptr && (record->m_flags & CLR_RT_GenericInstance::c_Open) == 0;
    }

    return (target->flags & CLR_RECORD_TYPESPEC::TS_Closed) != 0;
}

bool CLR_RT_TypeSpec_Instance::GetGenericParam(CLR_INT32 parameterPosition, CLR_RT_SignatureParser::Element &element)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_SignatureParser parser;

    return ::PositionAtGenericParam(*this, parameterPosition, parser) && SUCCEEDED(parser.Advance(element));
}

bool CLR_RT_TypeSpec_Instance::PositionAtGenericParam(CLR_INT32 parameterPosition, CLR_RT_SignatureParser &parser)
{
    return ::PositionAtGenericParam(*this, parameterPosition, parser);
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

        case REFLECTION_TYPESPEC:
        {
            CLR_RT_TypeSpec_Instance tsInst{};

            if (!tsInst.InitializeFromIndex(reflex.data.typeSpec))
            {
                return false;
            }

            assembly = tsInst.assembly;

            CLR_RT_SignatureParser parser;
            parser.Initialize_TypeSpec(tsInst);

            CLR_RT_SignatureParser::Element element;
            if (FAILED(parser.Advance(element)))
            {
                return false;
            }

            if (levels)
            {
                *levels = element.Levels + reflex.levels;
            }
            else if (reflex.levels > 0)
            {
                return InitializeFromIndex(g_CLR_RT_WellKnownTypes.Array);
            }

            // if this is a generic type, need to advance to get type
            if (element.DataType == DATATYPE_GENERICINST)
            {
                parser.Advance(element);
            }

            return InitializeFromIndex(element.Class);
        }
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
    const CLR_RT_MethodDef_Instance *caller,
    const CLR_RT_TypeSpec_Index *contextTypeSpec)
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

                    CLR_INDEX genericPosition;

                    CLR_RT_SignatureParser::Element elem;
                    if (FAILED(parser.Advance(elem)))
                    {
                        return false;
                    }

                    if (elem.Levels > 0)
                    {
                        // this is an array, can't init like this
                        return false;
                    }
                    else
                    {
                        if (elem.DataType == DATATYPE_GENERICINST)
                        {
                            // Advance once past the open generic typedef. Advance already consumed
                            // the arg-count byte, so no further skip is needed for NEWARR / STELEM
                            // / ISINST / CASTCLASS on a GENERICINST token.
                            if (FAILED(parser.Advance(elem)))
                            {
                                return false;
                            }

                            if ((elem.DataType == DATATYPE_CLASS || elem.DataType == DATATYPE_VALUETYPE) &&
                                NANOCLR_INDEX_IS_VALID(elem.Class))
                            {
                                data = elem.Class.data;
                                assembly = g_CLR_RT_TypeSystem.m_assemblies[elem.Class.Assembly() - 1];
                                target = assembly->GetTypeDef(elem.Class.Type());
#if defined(NANOCLR_INSTANCE_NAMES)
                                name = assembly->GetString(target->name);
#endif
                                return true;
                            }

                            return false;
                        }

                        genericPosition = elem.GenericParamPosition;

                        // VAR (!T) resolution chain: contextTypeSpec → caller->genericType
                        // → arrayElementType. See CLAUDE.md "VAR / MVAR resolution".
                        if (elem.DataType == DATATYPE_VAR)
                        {
                            const CLR_RT_TypeSpec_Index *effectiveContext = nullptr;

                            if (contextTypeSpec != nullptr && NANOCLR_INDEX_IS_VALID(*contextTypeSpec))
                            {
                                effectiveContext = contextTypeSpec;
                            }
                            else if (
                                caller != nullptr && caller->genericType != nullptr &&
                                NANOCLR_INDEX_IS_VALID(*caller->genericType))
                            {
                                effectiveContext = caller->genericType;
                            }
                            else
                            {
                                effectiveContext = nullptr;
                            }

                            if (effectiveContext != nullptr)
                            {
                                CLR_RT_TypeSpec_Instance callerTypeSpec;
                                if (!callerTypeSpec.InitializeFromIndex(*effectiveContext))
                                {
                                    return false;
                                }

                                CLR_RT_SignatureParser::Element paramElement;
                                CLR_RT_SignatureParser paramParser;

                                // Try to map using the generic context (e.g. !T→Int32)
                                if (PositionAtGenericParam(callerTypeSpec, genericPosition, paramParser) &&
                                    SUCCEEDED(paramParser.AdvanceParameter(paramElement)) &&
                                    paramElement.DataType != DATATYPE_VAR)
                                {
                                    // Successfully resolved from generic context
                                    if (NANOCLR_INDEX_IS_VALID(paramElement.Class))
                                    {
                                        data = paramElement.Class.data;
                                        assembly = g_CLR_RT_TypeSystem.m_assemblies[paramElement.Class.Assembly() - 1];
                                        target = assembly->GetTypeDef(paramElement.Class.Type());
                                    }
                                    else if (paramElement.DataType == DATATYPE_MVAR)
                                    {
                                        // need to defer to generic method argument
                                        genericPosition = paramElement.GenericParamPosition;

                                        goto resolve_generic_argument;
                                    }
                                    else
                                    {
                                        return false;
                                    }
                                }
                                else if (
                                    caller != nullptr && NANOCLR_INDEX_IS_VALID(caller->arrayElementType) &&
                                    genericPosition == 0)
                                {
                                    // arrayElementType fallback (runtime-inferred / still-open context).
                                    data = caller->arrayElementType.data;
                                    assembly =
                                        g_CLR_RT_TypeSystem.m_assemblies[caller->arrayElementType.Assembly() - 1];
                                    target = assembly->GetTypeDef(caller->arrayElementType.Type());
                                }
                                else
                                {
                                    return false;
                                }
                            }
                            else if (
                                caller != nullptr && NANOCLR_INDEX_IS_VALID(caller->arrayElementType) &&
                                genericPosition == 0)
                            {
                                data = caller->arrayElementType.data;
                                assembly = g_CLR_RT_TypeSystem.m_assemblies[caller->arrayElementType.Assembly() - 1];
                                target = assembly->GetTypeDef(caller->arrayElementType.Type());
                            }
                            else
                            {
                                return false;
                            }
                        }
                        else if (elem.DataType == DATATYPE_MVAR)
                        {
                        resolve_generic_argument:

                            if (caller == nullptr)
                            {
                                return false;
                            }

                            // resolve from methodspec context (MVAR = method-level generic parameter)
                            if (NANOCLR_INDEX_IS_VALID(caller->methodSpec))
                            {
                                CLR_RT_MethodSpec_Instance methodSpecInstance;
                                if (methodSpecInstance.InitializeFromIndex(caller->methodSpec))
                                {
                                    CLR_RT_SignatureParser::Element element;
                                    CLR_RT_SignatureParser argParser;

                                    if (!PositionAtGenericArgument(methodSpecInstance, genericPosition, argParser) ||
                                        FAILED(argParser.AdvanceParameter(element)))
                                    {
                                        return false;
                                    }

                                    if (element.DataType == DATATYPE_VAR)
                                    {
                                        // need to defer to generic type parameter
                                        CLR_RT_TypeSpec_Instance contextTs;
                                        if (!contextTypeSpec || !contextTs.InitializeFromIndex(*contextTypeSpec))
                                        {
                                            return false;
                                        }

                                        if (!PositionAtGenericParam(
                                                contextTs,
                                                element.GenericParamPosition,
                                                argParser) ||
                                            FAILED(argParser.AdvanceParameter(element)))
                                        {
                                            return false;
                                        }
                                    }
                                    else if (element.DataType == DATATYPE_MVAR)
                                    {
                                        return false;
                                    }

                                    // the TypeDef of an array argument is System.Array
                                    if (element.Levels > 0)
                                    {
                                        element.Class = g_CLR_RT_WellKnownTypes.Array;
                                    }

                                    if (!NANOCLR_INDEX_IS_VALID(element.Class))
                                    {
                                        return false;
                                    }

                                    data = element.Class.data;
                                    assembly = g_CLR_RT_TypeSystem.m_assemblies[element.Class.Assembly() - 1];
                                    target = assembly->GetTypeDef(element.Class.Type());
                                }
                                else
                                {
                                    return false;
                                }
                            }
                            else if (NANOCLR_INDEX_IS_VALID(caller->arrayElementType) && genericPosition == 0)
                            {
                                // SZArrayHelper methods have no MethodSpec, T is the element type of the array.
                                data = caller->arrayElementType.data;
                                assembly = g_CLR_RT_TypeSystem.m_assemblies[caller->arrayElementType.Assembly() - 1];
                                target = assembly->GetTypeDef(caller->arrayElementType.Type());
                            }
                            else
                            {
                                return false;
                            }
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

            if (caller != nullptr && NANOCLR_INDEX_IS_VALID(caller->arrayElementType) && pos == 0)
            {
                data = caller->arrayElementType.data;
                assembly = g_CLR_RT_TypeSystem.m_assemblies[caller->arrayElementType.Assembly() - 1];
                target = assembly->GetTypeDef(caller->arrayElementType.Type());

                return true;
            }

            // Use the *caller's* bound genericType (Stack<Int32>, etc.)
            if (caller == nullptr || caller->genericType == nullptr)
            {
                return false;
            }

            CLR_RT_TypeSpec_Instance callerTypeSpec;
            if (!callerTypeSpec.InitializeFromIndex(*caller->genericType))
            {
                return false;
            }

            CLR_RT_SignatureParser::Element paramElement;
            CLR_RT_SignatureParser paramParser;
            if (!PositionAtGenericParam(callerTypeSpec, pos, paramParser) ||
                FAILED(paramParser.AdvanceParameter(paramElement)) || !NANOCLR_INDEX_IS_VALID(paramElement.Class))
            {
                return false;
            }

            // populate this instance
            data = paramElement.Class.data;
            assembly = g_CLR_RT_TypeSystem.m_assemblies[paramElement.Class.Assembly() - 1];
            target = assembly->GetTypeDef(paramElement.Class.Type());

            return true;
        }
        else if (elem.DataType == DATATYPE_MVAR)
        {
            // Some rebound generic methods bind their first method generic parameter from the
            // runtime element type.
            if (caller != nullptr && NANOCLR_INDEX_IS_VALID(caller->arrayElementType) && elem.GenericParamPosition == 0)
            {
                data = caller->arrayElementType.data;
                assembly = g_CLR_RT_TypeSystem.m_assemblies[caller->arrayElementType.Assembly() - 1];
                target = assembly->GetTypeDef(caller->arrayElementType.Type());

                return true;
            }

            if (caller != nullptr && NANOCLR_INDEX_IS_VALID(caller->methodSpec))
            {
                CLR_RT_MethodSpec_Instance methodSpecInstance;
                CLR_RT_SignatureParser::Element argument;
                CLR_RT_SignatureParser argumentParser;

                if (!methodSpecInstance.InitializeFromIndex(caller->methodSpec) ||
                    !PositionAtGenericArgument(methodSpecInstance, elem.GenericParamPosition, argumentParser) ||
                    FAILED(argumentParser.AdvanceParameter(argument)) || !NANOCLR_INDEX_IS_VALID(argument.Class))
                {
                    return false;
                }

                data = argument.Class.data;
                assembly = g_CLR_RT_TypeSystem.m_assemblies[argument.Class.Assembly() - 1];
                target = assembly->GetTypeDef(argument.Class.Type());

                return true;
            }

            return false;
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

                case TBL_TypeSpec:
                {
                    CLR_RT_TypeSpec_Index typeSpec;
                    CLR_RT_TypeSpec_Instance typeSpecInstance{};

                    typeSpec.Set(Assembly(), target->ExtendsIndex());

                    if (!typeSpecInstance.InitializeFromIndex(typeSpec))
                    {
                        return false;
                    }

                    tmp = typeSpecInstance.genericTypeDef;
                    cls = &tmp;
                }
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

bool CLR_RT_TypeDef_Instance::GetExtendsTypeSpec(CLR_RT_TypeSpec_Index &typeSpec) const
{
    NATIVE_PROFILE_CLR_CORE();

    if (!NANOCLR_INDEX_IS_VALID(*this) || !target->HasValidExtendsType() || !target->ExtendsIsTypeSpec())
    {
        return false;
    }

    typeSpec.Set(Assembly(), target->ExtendsIndex());

    return true;
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

static bool IsInstantiatedOverOwnParameters(const CLR_RT_TypeSpec_Instance &typeSpec)
{
    return typeSpec.target != nullptr && (typeSpec.target->flags & CLR_RECORD_TYPESPEC::TS_OverOwnParameters) != 0;
}

// Closes an open owner in the caller's context, see CLAUDE.md "A closed owner outranks the caller".
// nullptr when it stays open; overLimit when it can't be represented.
static const CLR_RT_TypeSpec_Index *FindClosedOwnerTypeSpec(
    CLR_RT_TypeSpec_Instance &openTsInst,
    const CLR_RT_MethodDef_Instance *caller,
    bool &overLimit)
{
    HRESULT hr;
    const CLR_RT_TypeDef_Index *mvarFallback =
        NANOCLR_INDEX_IS_VALID(caller->methodSpec) ? nullptr : &caller->arrayElementType;

    CLR_RT_GenericInstance *instance =
        CLR_RT_TypeSystem::FindOrCreateGenericInstance(openTsInst, caller->genericType, caller, &hr, mvarFallback);

    overLimit = hr == CLR_E_NOT_SUPPORTED;

    return instance != nullptr ? &instance->m_self : nullptr;
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
                    // Use the pre-resolved FieldDef from the cross-reference (set during ResolveFieldRef).
                    // FindFieldDef with assm (caller assembly) searches the wrong assembly field-def
                    // table when the owning type is in a different assembly, giving wrong results.
                    data = assm->crossReferenceFieldRef[index].target.data;

                    if (!NANOCLR_INDEX_IS_VALID(*this))
                    {
#if defined(NANOCLR_TRACE_GENERICS)
                        if (s_CLR_RT_fTrace_GenericFields >= c_CLR_RT_Trace_Info)
                        {
                            CLR_Debug::Printf(
                                "GenericFields: FieldDef ResolveToken TBL_TypeSpec invalid cross-ref idx=%u "
                                "assm='%s'\r\n",
                                index,
                                assm->name);
                        }
#endif
                        return false;
                    }

                    assembly = g_CLR_RT_TypeSystem.m_assemblies[Assembly() - 1];
                    target = assembly->GetFieldDef(Field());

                    // Resolve the best TypeSpec context for type-printing and generic validation.
                    const CLR_RT_TypeSpec_Index *mdTS = &assm->crossReferenceFieldRef[index].genericType;
                    const CLR_RT_TypeSpec_Index *effectiveTS = mdTS;

                    if (caller && caller->genericType && NANOCLR_INDEX_IS_VALID(*caller->genericType))
                    {
                        CLR_RT_TypeSpec_Instance instCaller, instMd;
                        if (instCaller.InitializeFromIndex(*caller->genericType) && instMd.InitializeFromIndex(*mdTS))
                        {
                            // See CLAUDE.md "A closed owner outranks the caller".
                            if (instCaller.genericTypeDef.data == instMd.genericTypeDef.data &&
                                !instMd.IsClosedGenericType())
                            {
                                bool overLimit = false;
                                const CLR_RT_TypeSpec_Index *closedOwner =
                                    IsInstantiatedOverOwnParameters(instMd)
                                        ? nullptr
                                        : FindClosedOwnerTypeSpec(instMd, caller, overLimit);

                                if (overLimit)
                                {
                                    return false;
                                }

                                effectiveTS = closedOwner != nullptr ? closedOwner : caller->genericType;
                            }
                        }
                    }

                    genericType = effectiveTS;

                    break;
                }
                default:
                    // should not happen
#if defined(NANOCLR_TRACE_GENERICS)
                    if (s_CLR_RT_fTrace_GenericFields >= c_CLR_RT_Trace_Info)
                    {
                        CLR_Debug::Printf(
                            "GenericFields: FieldDef ResolveToken unexpected owner table=%u idx=%u assm='%s'\r\n",
                            (unsigned)fr->Owner(),
                            index,
                            assm->name);
                    }
#endif
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
#if defined(NANOCLR_TRACE_GENERICS)
            if (s_CLR_RT_fTrace_GenericFields >= c_CLR_RT_Trace_Info)
            {
                CLR_Debug::Printf(
                    "GenericFields: FieldDef ResolveToken unrecognised token table=%u tk=%08X assm='%s'\r\n",
                    (unsigned)CLR_TypeFromTk(tk),
                    tk,
                    assm->name);
            }
#endif
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
        arrayElementType.Clear();
        methodSpec.Clear();

#if defined(NANOCLR_INSTANCE_NAMES)
        name = assembly->GetString(target->name);
#endif
        return true;
    }

    data = 0;
    assembly = nullptr;
    target = nullptr;
    genericType = nullptr;
    arrayElementType.Clear();

    return false;
}

bool CLR_RT_MethodDef_Instance::InitializeFromIndex(
    const CLR_RT_MethodDef_Index &md,
    const CLR_RT_TypeSpec_Index &typeSpec,
    const CLR_RT_MethodDef_Instance *caller)
{
    NATIVE_PROFILE_CLR_CORE();

    methodSpec.Clear();

    CLR_RT_TypeSpec_Instance tsInst;

    if (!tsInst.InitializeFromIndex(typeSpec))
    {
        return false;
    }

    // parse the signature to get to the GENERICINST and then the definition token
    CLR_RT_SignatureParser parser;
    parser.Initialize_TypeSpec(tsInst);

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
    CLR_RT_MethodDef_Index mdResolved;

    if (elem.DataType == DATATYPE_VAR)
    {
        CLR_RT_SignatureParser::Element paramElement;

        if (!tsInst.GetGenericParam(elem.GenericParamPosition, paramElement))
        {
            return false;
        }

        if (paramElement.DataType == DATATYPE_GENERICINST)
        {
            // The resolved parameter is itself a constructed generic instance.
            // Its Class field is not valid; re-parse the TypeSpec to get the underlying generic definition.
            CLR_RT_SignatureParser subParser{};
            subParser.Initialize_TypeSpec(tsInst);

            CLR_RT_SignatureParser::Element subElem{};
            if (FAILED(subParser.Advance(subElem)))
            {
                return false;
            }

            // skip GENERICINST marker
            if (subElem.DataType == DATATYPE_GENERICINST)
            {
                if (FAILED(subParser.Advance(subElem)))
                {
                    return false;
                }
            }

            ownerTypeIdx = subElem.Class;
        }
        else
        {
            ownerTypeIdx = paramElement.Class;
        }
    }
    else if (elem.DataType == DATATYPE_MVAR)
    {
        // Method-level generic parameter (!!U): resolve from the caller's MethodSpec context
        if (caller != nullptr && NANOCLR_INDEX_IS_VALID(caller->methodSpec))
        {
            CLR_RT_MethodSpec_Instance msInst;
            if (!msInst.InitializeFromIndex(caller->methodSpec))
            {
                return false;
            }

            CLR_RT_SignatureParser::Element argElement;
            if (!msInst.GetGenericArgument(elem.GenericParamPosition, argElement))
            {
                return false;
            }

            if (argElement.DataType == DATATYPE_VAR && caller->genericType &&
                NANOCLR_INDEX_IS_VALID(*caller->genericType))
            {
                // MVAR resolved to a VAR, chain-resolve from the caller's type context
                CLR_RT_TypeSpec_Instance callerTs;
                if (!callerTs.InitializeFromIndex(*caller->genericType))
                {
                    return false;
                }

                CLR_RT_SignatureParser::Element paramElement;
                if (!callerTs.GetGenericParam(argElement.GenericParamPosition, paramElement))
                {
                    return false;
                }

                if (paramElement.DataType == DATATYPE_GENERICINST)
                {
                    // The resolved parameter is a constructed generic instance.
                    // Extract the underlying generic definition from the caller's TypeSpec.
                    if (NANOCLR_INDEX_IS_VALID(callerTs.genericTypeDef))
                    {
                        ownerTypeIdx = callerTs.genericTypeDef;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    ownerTypeIdx = paramElement.Class;
                }
            }
            else if (argElement.DataType == DATATYPE_GENERICINST)
            {
                // The MVAR resolved directly to a constructed generic instance.
                // Re-resolve via the caller's MethodSpec to find the generic definition token.
                CLR_RT_SignatureParser reParser{};
                reParser.Initialize_MethodSignature(&msInst);

                // walk to the correct argument position
                CLR_RT_SignatureParser::Element reElem{};
                for (int gi = 0; gi <= elem.GenericParamPosition; gi++)
                {
                    if (FAILED(reParser.Advance(reElem)))
                    {
                        return false;
                    }
                }

                // reElem should be GENERICINST; advance once more for the type definition
                if (reElem.DataType == DATATYPE_GENERICINST)
                {
                    if (FAILED(reParser.Advance(reElem)))
                    {
                        return false;
                    }
                }

                ownerTypeIdx = reElem.Class;
            }
            else
            {
                ownerTypeIdx = argElement.Class;
            }
        }
        else
        {
            // No caller context available, cannot resolve MVAR
            return false;
        }
    }
    else
    {
        // elem.Class.data now contains the TypeDef/TypeRef token for the generic definition
        ownerTypeIdx.data = elem.Class.data;
    }

    // try to find the correct method reference
    if (ownerTypeIdx.Assembly() != md.Assembly())
    {
        // Cross-assembly case: need to find the corresponding MethodRef/MethodDef
        CLR_RT_Assembly *originalAssm = g_CLR_RT_TypeSystem.m_assemblies[md.Assembly() - 1];
        CLR_RT_Assembly *targetAssm = g_CLR_RT_TypeSystem.m_assemblies[ownerTypeIdx.Assembly() - 1];

        // Get the original method information
        const CLR_RECORD_METHODDEF *originalMD = originalAssm->GetMethodDef(md.Method());
        const char *methodName = originalAssm->GetString(originalMD->name);

        // Try to find the method in the target assembly by looking through MethodRefs
        bool found = false;
        for (int i = 0; i < targetAssm->tablesSize[TBL_MethodRef]; i++)
        {
            const CLR_RECORD_METHODREF *mr = targetAssm->GetMethodRef(i);
            const char *refName = targetAssm->GetString(mr->name);

            if (!strcmp(methodName, refName))
            {
                // Found a potential match, now check if it resolves to our original method
                CLR_RT_MethodRef_CrossReference &crossRef = targetAssm->crossReferenceMethodRef[i];

                if (crossRef.target.data == md.data)
                {
                    // This MethodRef points to our original method!
                    mdResolved.data = md.data;
                    found = true;
                    break;
                }
            }
        }

        if (!found)
        {
            // Fallback: keep the original method reference
            mdResolved.data = md.data;
        }
    }
    else
    {
        // Same assembly case: no rebinding needed
        mdResolved.data = md.data;
    }

    if (!InitializeFromIndex(mdResolved))
    {
        return false;
    }

    // Copy into member storage — genericType must outlive the caller's stack.
    m_typeSpecStorage = typeSpec;
    genericType = &m_typeSpecStorage;

    return true;
}

void CLR_RT_MethodDef_Instance::ClearInstance()
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_MethodDef_Index::Clear();
    methodSpec.Clear();
    m_typeSpecStorage.Clear();

    assembly = nullptr;
    target = nullptr;
    genericType = nullptr;
    arrayElementType.Clear();
}

bool CLR_RT_MethodDef_Instance::ResolveToken(
    CLR_UINT32 tk,
    CLR_RT_Assembly *assm,
    const CLR_RT_TypeSpec_Index *callerGeneric,
    const CLR_RT_MethodDef_Instance *caller)
{
    NATIVE_PROFILE_CLR_CORE();

    ClearInstance();

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
                    // Owner is a TypeSpec — possibly open. Prefer the caller's closed TypeSpec
                    // only when it points at the same TypeDef. See CLAUDE.md "Generic context
                    // propagation".
                    const CLR_RT_TypeSpec_Index *methodOwnerTS = &assm->crossReferenceMethodRef[index].genericType;

                    // check if MethodRef TypeDef token is the same TypeDef as in that caller
                    bool useCaller = false;
                    bool callerIsFallback = false;

                    if (callerGeneric != nullptr && NANOCLR_INDEX_IS_VALID(*callerGeneric))
                    {
                        CLR_RT_TypeSpec_Instance callerInst{};
                        if (callerInst.InitializeFromIndex(*callerGeneric))
                        {
                            CLR_RT_SignatureParser parserCaller;
                            parserCaller.Initialize_TypeSpec(callerInst);

                            CLR_RT_SignatureParser::Element elemCaller;

                            // advance to the generic instance which will point to the class
                            parserCaller.Advance(elemCaller);

                            if (elemCaller.DataType == DATATYPE_GENERICINST)
                            {
                                parserCaller.Advance(elemCaller);
                            }

                            CLR_UINT32 callerTypeDefToken = elemCaller.Class.data;

                            // parse the MethodRef declared TypeSpec
                            CLR_RT_TypeSpec_Instance ownerInst{};
                            if (ownerInst.InitializeFromIndex(*methodOwnerTS))
                            {
                                CLR_RT_SignatureParser parserOwner;
                                parserOwner.Initialize_TypeSpec(ownerInst);

                                CLR_RT_SignatureParser::Element elemOwner;

                                // advance to the generic instance which will point to the class
                                parserOwner.Advance(elemOwner);

                                if (elemOwner.DataType == DATATYPE_GENERICINST)
                                {
                                    parserOwner.Advance(elemOwner);
                                }

                                CLR_UINT32 ownerTypeDefToken = elemOwner.Class.data;

                                if ((callerTypeDefToken == ownerTypeDefToken) && callerTypeDefToken != 0x0 &&
                                    !ownerInst.IsClosedGenericType())
                                {
                                    // same TypeDef → bind to the caller's closed TypeSpec
                                    useCaller = IsInstantiatedOverOwnParameters(ownerInst);
                                    callerIsFallback = !useCaller;
                                }
                            }
                        }
                    }

                    genericType = useCaller ? callerGeneric : methodOwnerTS;

                    // Any other open owner is closed in the caller's context.
                    if (!useCaller && caller != nullptr)
                    {
                        CLR_RT_TypeSpec_Instance openTsInst{};
                        if (openTsInst.InitializeFromIndex(*methodOwnerTS) &&
                            NANOCLR_INDEX_IS_VALID(openTsInst.genericTypeDef) && !openTsInst.IsClosedGenericType())
                        {
                            bool overLimit = false;
                            const CLR_RT_TypeSpec_Index *closedOwner =
                                FindClosedOwnerTypeSpec(openTsInst, caller, overLimit);

                            if (overLimit)
                            {
                                return false;
                            }

                            if (closedOwner != nullptr)
                            {
                                genericType = closedOwner;
                                callerIsFallback = false;
                            }
                        }
                    }

                    // See CLAUDE.md "A closed owner outranks the caller".
                    if (callerIsFallback && (caller == nullptr || !NANOCLR_INDEX_IS_VALID(caller->methodSpec)))
                    {
                        genericType = callerGeneric;
                    }

                    // Use the pre-resolved method target from startup instead of a fresh
                    // signature-based FindMethodDef call.
                    data = assm->crossReferenceMethodRef[index].target.data;
                    if (!data)
                    {
                        return false;
                    }

                    assembly = g_CLR_RT_TypeSystem.m_assemblies[Assembly() - 1];
                    target = assembly->GetMethodDef(Method());
                }
                else
                {
                    // owner is TypeRef (e.g., interface method call)

                    // get data for MethodRef (from index)
                    data = assm->crossReferenceMethodRef[index].target.data;
                    if (!data)
                    {
                        return false;
                    }

                    // get assembly for this type ref
                    assembly = g_CLR_RT_TypeSystem.m_assemblies[Assembly() - 1];
                    // grab the MethodDef
                    target = assembly->GetMethodDef(Method());

                    // Preserve caller's generic context for interface method calls
                    // When calling a interface method (e.g. IList<T>.Remove() on a List<int> object) we need the closed
                    // generic context
                    genericType = callerGeneric;
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

                methodSpec.Set(assembly->assemblyIndex, index);

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

                // get generic type - guard against invalid container index (CLR_EmptyIndex == 65535)
                if (ms->container != CLR_EmptyIndex && ms->container < (CLR_UINT16)assm->tablesSize[TBL_TypeSpec])
                {
                    genericType = &assm->crossReferenceTypeSpec[ms->container].genericType;

                    CLR_RT_TypeSpec_Instance containerInst{};

                    if (caller != nullptr && containerInst.InitializeFromIndex(*genericType) &&
                        NANOCLR_INDEX_IS_VALID(containerInst.genericTypeDef) && !containerInst.IsClosedGenericType())
                    {
                        bool overLimit = false;
                        const CLR_RT_TypeSpec_Index *closedOwner =
                            FindClosedOwnerTypeSpec(containerInst, caller, overLimit);

                        if (overLimit)
                        {
                            return false;
                        }

                        if (closedOwner != nullptr)
                        {
                            genericType = closedOwner;
                        }
                    }
                }
                else
                {
                    genericType = nullptr;
                }

#if defined(NANOCLR_INSTANCE_NAMES)
                name = assembly->GetString(target->name);
#endif
                return true;
            }
            case TBL_TypeSpec:
            {
                CLR_RT_MethodSpec_Index newMethodSpec;
                assm->FindMethodSpecFromTypeSpec(index, newMethodSpec);

                const CLR_RECORD_METHODSPEC *ms = assm->GetMethodSpec(newMethodSpec.Method());

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

                // get generic type (dead code: all paths above return, but kept for clarity)
                if (ms->container != CLR_EmptyIndex && ms->container < (CLR_UINT16)assembly->tablesSize[TBL_TypeSpec])
                {
                    genericType = &assembly->crossReferenceTypeSpec[ms->container].genericType;
                }
                else
                {
                    genericType = nullptr;
                }

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

    // First, get the method's owner type to check if it's actually generic
    CLR_RT_TypeDef_Instance ownerType{};
    if (!ownerType.InitializeFromMethod(*this))
    {
        return false;
    }

    // Only use the generic type context if:
    // 1. We have a generic type context available AND
    // 2. The method's declaring type is actually generic
    if (genericType && NANOCLR_INDEX_IS_VALID(*genericType) && ownerType.target->genericParamCount > 0)
    {
        CLR_RT_TypeSpec_Instance tsInst{};

        if (!tsInst.InitializeFromIndex(*genericType))
        {
            return false;
        }

        // Parse the signature so we can peel off [GENERICINST CLASS <type> <args>].
        CLR_RT_SignatureParser parser;
        parser.Initialize_TypeSpec(tsInst);

        CLR_RT_SignatureParser::Element elem;
        parser.Advance(elem);

        if (elem.DataType == DATATYPE_GENERICINST)
        {
            // generic type, advance again to get the type
            parser.Advance(elem);

            return declType.InitializeFromIndex(elem.Class);
        }
        else if (elem.DataType == DATATYPE_VAR)
        {
            // generic type, advance to get the type
            int pos = elem.GenericParamPosition;
            // Use the *caller's* bound genericType (Stack<Int32>, etc.)
            CLR_RT_TypeSpec_Instance callerTypeSpec;
            if (!callerTypeSpec.InitializeFromIndex(*genericType))
            {
                return false;
            }

            CLR_RT_SignatureParser::Element paramElement;
            if (!callerTypeSpec.GetGenericParam((CLR_UINT32)pos, paramElement))
            {
                return false;
            }

            return declType.InitializeFromIndex(paramElement.Class);
        }
    }

    // For non-generic types or when no generic context is available,
    // just return the declaring type
    return declType.InitializeFromMethod(*this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool CLR_RT_MethodSpec_Instance::InitializeFromIndex(const CLR_RT_MethodSpec_Index &index)
{
    NATIVE_PROFILE_CLR_CORE();

    if (index.IsRuntime())
    {
        CLR_RT_TypeSpec_Index recordIndex;
        recordIndex.data = index.data;

        if (CLR_RT_TypeSystem::GetGenericInstance(recordIndex) != nullptr)
        {
            data = index.data;
            assembly = nullptr;
            target = nullptr;

            return true;
        }

        ClearInstance();

        return false;
    }

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

bool CLR_RT_MethodSpec_Instance::GetGenericArgument(
    CLR_INT32 argumentPosition,
    CLR_RT_SignatureParser::Element &element)
{
    CLR_RT_SignatureParser parser;

    return ::PositionAtGenericArgument(*this, argumentPosition, parser) && SUCCEEDED(parser.Advance(element));
}

bool CLR_RT_MethodSpec_Instance::PositionAtGenericArgument(CLR_INT32 argumentPosition, CLR_RT_SignatureParser &parser)
{
    return ::PositionAtGenericArgument(*this, argumentPosition, parser);
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

    if (reflex.kind == REFLECTION_GENERICPARAM)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }

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

HRESULT CLR_RT_TypeDescriptor::InitializeFromTypeSpec(
    const CLR_RT_TypeSpec_Index &sig,
    const CLR_RT_TypeSpec_Index *contextTypeSpec)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeSpec_Instance inst{};
    CLR_RT_SignatureParser parser{};

    if (inst.InitializeFromIndex(sig) == false)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    parser.Initialize_TypeSpec(inst);

    NANOCLR_SET_AND_LEAVE(InitializeFromSignatureParser(parser, contextTypeSpec));

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
        m_flags = CLR_RT_DataTypeLookup::c_ManagedType;

        m_reflex.kind = REFLECTION_TYPESPEC;
        m_reflex.levels = 0;
        m_reflex.data.typeSpec = m_handlerGenericType;
    }

    m_handlerCls.ClearInstance();

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_TypeDescriptor::InitializeFromFieldDefinition(
    const CLR_RT_FieldDef_Instance &fd,
    const CLR_RT_TypeSpec_Index *ownerTypeSpec,
    CLR_RT_TypeSpec_Index *elementSpec,
    const CLR_RT_TypeDef_Index *declaringType)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_SignatureParser parser{};
    parser.Initialize_FieldDef(fd.assembly, fd.target);

    if (ownerTypeSpec == nullptr)
    {
        NANOCLR_SET_AND_LEAVE(InitializeFromSignatureParser(parser));
    }

    NANOCLR_SET_AND_LEAVE(InitializeFromClosedSignature(parser, ownerTypeSpec, elementSpec, declaringType));

    NANOCLR_NOCLEANUP();
}

bool CLR_RT_TypeSystem::PositionAtParameterArgument(
    const CLR_RT_SignatureParser::Element &element,
    const CLR_RT_TypeSpec_Index *owner,
    const CLR_RT_MethodSpec_Index *methodSpec,
    CLR_RT_SignatureParser &argument)
{
    if (element.DataType == DATATYPE_VAR)
    {
        CLR_RT_TypeSpec_Instance instance{};

        return owner != nullptr && NANOCLR_INDEX_IS_VALID(*owner) && instance.InitializeFromIndex(*owner) &&
               instance.PositionAtGenericParam(element.GenericParamPosition, argument);
    }

    if (element.DataType == DATATYPE_MVAR)
    {
        CLR_RT_MethodSpec_Instance instance{};

        return methodSpec != nullptr && NANOCLR_INDEX_IS_VALID(*methodSpec) &&
               instance.InitializeFromIndex(*methodSpec) &&
               instance.PositionAtGenericArgument(element.GenericParamPosition, argument);
    }

    return false;
}

HRESULT CLR_RT_TypeDescriptor::InitializeFromClosedSignature(
    CLR_RT_SignatureParser &parser,
    const CLR_RT_TypeSpec_Index *ownerTypeSpec,
    CLR_RT_TypeSpec_Index *elementSpec,
    const CLR_RT_TypeDef_Index *declaringType,
    const CLR_RT_MethodDef_Index *declaringMethod,
    const CLR_RT_MethodDef_Instance *context)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_SignatureParser at = parser;
    CLR_RT_SignatureParser::Element res;
    CLR_RT_GenericTarget target;
    int levels;
    HRESULT collected;

    if (context != nullptr && !NANOCLR_INDEX_IS_VALID(context->methodSpec))
    {
        context = nullptr;
    }

    if (elementSpec != nullptr)
    {
        elementSpec->Clear();
    }

    if (declaringType != nullptr && context == nullptr)
    {
        bool noOwner = ownerTypeSpec == nullptr || !NANOCLR_INDEX_IS_VALID(*ownerTypeSpec);
        bool openOwner = !noOwner && CLR_RT_TypeSystem::IsOpenInstance(*ownerTypeSpec);
        CLR_RT_SignatureParser peek = parser;
        CLR_RT_ReflectionDef_Index openWord;

        NANOCLR_CHECK_HRESULT(peek.AdvanceParameter(res));

        if (noOwner || openOwner || res.DataType == DATATYPE_MVAR)
        {
            HRESULT open = CLR_RT_TypeSystem::GetOpenSignatureWord(
                parser,
                openOwner ? ownerTypeSpec : nullptr,
                *declaringType,
                declaringMethod,
                openWord);

            if (open != S_FALSE)
            {
                if (SUCCEEDED(open))
                {
                    m_reflex = openWord;
                    parser = peek;
                }

                NANOCLR_SET_AND_LEAVE(open);
            }
        }
    }

    NANOCLR_CHECK_HRESULT(parser.AdvanceParameter(res));

    // a type parameter without its owner has no Type
    if ((res.DataType == DATATYPE_MVAR && context == nullptr) ||
        ((ownerTypeSpec == nullptr || !NANOCLR_INDEX_IS_VALID(*ownerTypeSpec)) && res.DataType == DATATYPE_VAR))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }

    levels = res.Levels;

    if (res.DataType == DATATYPE_VAR || res.DataType == DATATYPE_MVAR)
    {
        CLR_RT_SignatureParser argument;

        if (!CLR_RT_TypeSystem::PositionAtParameterArgument(
                res,
                ownerTypeSpec,
                context != nullptr ? &context->methodSpec : nullptr,
                argument))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }

        at = argument;
        NANOCLR_CHECK_HRESULT(argument.AdvanceParameter(res));
        levels += res.Levels;
        ownerTypeSpec = nullptr;
    }

    collected = CLR_RT_TypeSystem::CollectGenericTarget(at, ownerTypeSpec, context, target);

    NANOCLR_CHECK_HRESULT(collected);

    // an open generic instance has no Type either
    if (collected == S_FALSE)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }

    if (collected == S_OK && target.m_count > 0 && levels == 0)
    {
        CLR_RT_GenericInstance *record =
            CLR_RT_TypeSystem::FindOrCreateGenericInstanceByNodes(target.m_nodes, target.m_count);

        if (record == nullptr)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }

        NANOCLR_CHECK_HRESULT(InitializeFromType(res.Class));

        m_reflex.kind = REFLECTION_TYPESPEC;
        m_reflex.levels = 0;
        m_reflex.data.typeSpec = record->m_self;

        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    NANOCLR_CHECK_HRESULT(InitializeFromType(res.Class));

    if (levels > 0)
    {
        m_reflex.levels = (CLR_UINT16)levels;

        ConvertToArray();

        if (elementSpec != nullptr &&
            CLR_RT_TypeSystem::CollectGenericTarget(at, ownerTypeSpec, context, target, true) == S_OK &&
            target.m_count > 0)
        {
            CLR_RT_GenericInstance *record =
                CLR_RT_TypeSystem::FindOrCreateGenericInstanceByNodes(target.m_nodes, target.m_count);

            if (record != nullptr)
            {
                *elementSpec = record->m_self;
            }
        }
    }

    NANOCLR_NOCLEANUP();
}

CLR_RT_ReflectionDef_Index CLR_RT_TypeDescriptor::GetTypeReflection(const CLR_RT_TypeSpec_Index &elementSpec) const
{
    CLR_RT_ReflectionDef_Index reflex = m_reflex;

    if (NANOCLR_INDEX_IS_VALID(elementSpec) && reflex.kind == REFLECTION_TYPE && reflex.levels > 0)
    {
        reflex.kind = REFLECTION_TYPESPEC;
        reflex.data.typeSpec = elementSpec;
    }

    return reflex;
}

HRESULT CLR_RT_TypeDescriptor::InitializeFromSignatureParser(
    CLR_RT_SignatureParser &parser,
    const CLR_RT_TypeSpec_Index *contextTypeSpec)
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
        // generic type, advance again to get the type; the array levels are on the marker
        int levels = res.Levels;

        NANOCLR_CHECK_HRESULT(parser.Advance(res));

        res.Levels = levels;
    }

    // Check if this is an unresolved generic parameter (VAR) and we have a context
    if (res.DataType == DATATYPE_VAR && contextTypeSpec && NANOCLR_INDEX_IS_VALID(*contextTypeSpec))
    {
        // Resolve VAR from context TypeSpec using existing helper
        CLR_RT_TypeSpec_Instance contextTs;
        if (!contextTs.InitializeFromIndex(*contextTypeSpec))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }

        CLR_RT_SignatureParser::Element paramElement;
        if (contextTs.GetGenericParam(res.GenericParamPosition, paramElement))
        {
            // Use the resolved type from context
            NANOCLR_CHECK_HRESULT(InitializeFromType(paramElement.Class));
        }
        else
        {
            // Couldn't resolve, fall back to original behavior
            NANOCLR_CHECK_HRESULT(InitializeFromType(res.Class));
        }
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

#if defined(NANOCLR_TRACE_GENERICS)
    if (s_CLR_RT_fTrace_GenericFields >= c_CLR_RT_Trace_Info)
    {
        CLR_Debug::Printf(
            "[DIAG] InitFromSigToken assm=%s token=%08X callerHasGenType=%d callerHasElemType=%d\r\n",
            assm ? assm->name : "(null)",
            (unsigned)token,
            (int)(caller && caller->genericType && NANOCLR_INDEX_IS_VALID(*caller->genericType)),
            (int)(caller && NANOCLR_INDEX_IS_VALID(caller->arrayElementType)));
    }
#endif

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

            if ((elem.DataType == DATATYPE_VAR || elem.DataType == DATATYPE_MVAR) && elem.Levels > 0 &&
                caller != nullptr)
            {
                bool handled;
                CLR_RT_TypeDef_Index rowInnermost;
                CLR_RT_TypeSpec_Index rowElement;
                int rowLevels;

                NANOCLR_CHECK_HRESULT(
                    CLR_RT_TypeSystem::ResolveArrayRow(
                        assm,
                        token,
                        caller,
                        caller->genericType,
                        handled,
                        rowInnermost,
                        rowLevels,
                        rowElement));

                if (handled)
                {
                    NANOCLR_CHECK_HRESULT(InitializeFromType(rowInnermost));

                    m_reflex.levels = rowLevels;
                    ConvertToArray();
                    break;
                }
            }

            if (elem.DataType == DATATYPE_VAR)
            {
                // !T: ask the CLR to map that slot into the *actual* argument

                // For runtime-inferred generic element scenarios, arrayElementType is authoritative for position 0
                if (caller && NANOCLR_INDEX_IS_VALID(caller->arrayElementType) && elem.GenericParamPosition == 0)
                {
                    this->InitializeFromTypeDef(caller->arrayElementType);
                }
                // Otherwise try to resolve from generic context
                else if (caller && caller->genericType && NANOCLR_INDEX_IS_VALID(*caller->genericType))
                {
                    CLR_RT_TypeSpec_Instance callerTypeSpec;
                    if (!callerTypeSpec.InitializeFromIndex(*caller->genericType))
                    {
                        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
                    }

                    CLR_RT_SignatureParser::Element paramElement;
                    CLR_RT_SignatureParser paramParser;
                    if (callerTypeSpec.PositionAtGenericParam(elem.GenericParamPosition, paramParser) &&
                        SUCCEEDED(paramParser.AdvanceParameter(paramElement)))
                    {
                        if (NANOCLR_INDEX_IS_VALID(paramElement.Class))
                        {
                            this->InitializeFromTypeDef(paramElement.Class);
                        }
                        else
                        {
                            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
                        }
                    }
                    else
                    {
                        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
                    }
                }
                else
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
                }
            }
            else if (elem.DataType == DATATYPE_MVAR)
            {
                // !!U: method-generic parameter — resolve from the caller's MethodSpec if available,
                // which gives the concrete closed type (e.g. String for BuildListAndCount<String>).
                if (caller != nullptr && NANOCLR_INDEX_IS_VALID(caller->arrayElementType) &&
                    elem.GenericParamPosition == 0)
                {
                    this->InitializeFromTypeDef(caller->arrayElementType);
                    if (elem.Levels > 0)
                    {
                        m_reflex.levels = elem.Levels;
                        ConvertToArray();
                    }
                    break;
                }

                if (caller != nullptr && NANOCLR_INDEX_IS_VALID(caller->methodSpec))
                {
                    CLR_RT_MethodSpec_Instance msInst;
                    if (msInst.InitializeFromIndex(caller->methodSpec))
                    {
                        CLR_RT_SignatureParser::Element msElem;
                        if (msInst.GetGenericArgument(elem.GenericParamPosition, msElem))
                        {
                            if (NANOCLR_INDEX_IS_VALID(msElem.Class))
                            {
                                // Concrete type argument: use it directly.
                                this->InitializeFromTypeDef(msElem.Class);
                                break;
                            }
                            else if (
                                msElem.DataType == DATATYPE_VAR && caller->genericType != nullptr &&
                                NANOCLR_INDEX_IS_VALID(*caller->genericType))
                            {
                                // !!U -> !T: the MethodSpec maps a method-generic to a type-generic;
                                // resolve the type-generic slot from the caller's closed TypeSpec.
                                CLR_RT_TypeSpec_Instance callerTypeSpec;
                                if (callerTypeSpec.InitializeFromIndex(*caller->genericType))
                                {
                                    CLR_RT_SignatureParser::Element paramElement;
                                    if (callerTypeSpec.GetGenericParam(msElem.GenericParamPosition, paramElement) &&
                                        NANOCLR_INDEX_IS_VALID(paramElement.Class))
                                    {
                                        this->InitializeFromTypeDef(paramElement.Class);
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }

                NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
            }
            else if (elem.DataType == DATATYPE_GENERICINST)
            {
                // full generic instantiation: parse it directly from the signature
                // Pass caller's generic type as context to resolve VAR parameters in the generic arguments
                const CLR_RT_TypeSpec_Index *contextTypeSpec =
                    (caller && caller->genericType && NANOCLR_INDEX_IS_VALID(*caller->genericType))
                        ? caller->genericType
                        : nullptr;
                NANOCLR_CHECK_HRESULT(this->InitializeFromSignatureParser(parser, contextTypeSpec));
            }
            else
            {
                // e.g. SZARRAY, VALUETYPE, CLASS
                // The parser has already been advanced above;
                // use the parsed elem directly rather than re-advancing an exhausted parser.
                if (NANOCLR_INDEX_IS_VALID(elem.Class))
                {
                    NANOCLR_CHECK_HRESULT(InitializeFromType(elem.Class));
                }
                else
                {
                    NANOCLR_CHECK_HRESULT(InitializeFromDataType(elem.DataType));
                }

                if (elem.Levels > 0)
                {
                    m_reflex.levels = elem.Levels;
                    ConvertToArray();
                }
            }

            if ((elem.DataType == DATATYPE_VAR || elem.DataType == DATATYPE_MVAR ||
                 elem.DataType == DATATYPE_GENERICINST) &&
                elem.Levels > 0)
            {
                m_reflex.levels = elem.Levels;
                ConvertToArray();
            }
            break;
        }

        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_NOTIMPL);
    }

    NANOCLR_CLEANUP();
#if defined(NANOCLR_TRACE_GENERICS)
    if (s_CLR_RT_fTrace_GenericFields >= c_CLR_RT_Trace_Info)
    {
        CLR_Debug::Printf(
            "[DIAG] InitFromSigToken EXIT hr=%08X DT=%d hCls=%08X hGT=%08X levels=%d\r\n",
            (unsigned)hr,
            (int)GetDataType(),
            (unsigned)m_handlerCls.data,
            (unsigned)m_handlerGenericType.data,
            (int)m_reflex.levels);
    }
#endif
    NANOCLR_CLEANUP_END();
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
                    case REFLECTION_GENERICTYPE:
                        cls = &g_CLR_RT_WellKnownTypes.Type;
                        break;
                    case REFLECTION_TYPESPEC:
                    case REFLECTION_GENERICPARAM:
                        cls = &g_CLR_RT_WellKnownTypes.Type;
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

                    // For reference arrays, if the element is null, we need to get the type from the array's element
                    // type rather than trying to dereference a null object
                    if (obj->Dereference() == nullptr)
                    {
                        // Use the array's element type.
                        // Keep 'reflex' null to avoid carrying array levels when returning an element type.
                        cls = &(array->ReflectionDataConst().data.type);
                        break;
                    }

                    NANOCLR_SET_AND_LEAVE(InitializeFromObject(*obj));
                }

                // For a byref to an element of a primitive/value array, report the element type only.
                // Carrying the array reflection data here would make the actual type look like T[]
                // instead of T during ldelem.any validation.
                cls = &array->ReflectionDataConst().data.type;
            }
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
            res.data = desc.m_handlerGenericType.genericTypeDef.data;
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

// Classify a candidate record: marker, header CRC and string table version.
// See CLAUDE.md.
CLR_RECORD_ASSEMBLY::HeaderStatus CLR_RECORD_ASSEMBLY::CheckHeader() const
{
    NATIVE_PROFILE_CLR_CORE();

    // an all-0xFF (erased NOR) or all-0x00 (zero-filled media, e.g. virtual device RAM) marker marks
    // the end of the deployment region
    bool allErased = true;
    bool allZero = true;
    for (size_t i = 0; i < sizeof(marker); i++)
    {
        if (marker[i] != 0xFF)
        {
            allErased = false;
        }
        if (marker[i] != 0x00)
        {
            allZero = false;
        }
    }
    if (allErased || allZero)
    {
        return HeaderStatus_Erased;
    }

    if (memcmp(marker, c_MARKER_ASSEMBLY_V2, sizeof(c_MARKER_ASSEMBLY_V2)) != 0)
    {
        // a recognized nanoFramework marker prefix but not V2 (e.g. the retired NFMRK1) is an
        // unsupported PE format; anything else is foreign data where a record was expected
        if (memcmp(marker, "NFMRK", 5) == 0)
        {
            return HeaderStatus_UnsupportedVersion;
        }

        return HeaderStatus_NotAnAssembly;
    }

    CLR_RECORD_ASSEMBLY header = *this;
    header.headerCRC = 0;

    if (SUPPORT_ComputeCRC(&header, sizeof(header), 0) != this->headerCRC)
    {
        return HeaderStatus_BadHeaderCrc;
    }

    if (this->stringTableVersion != c_CLR_StringTable_Version)
    {
        return HeaderStatus_BadStringTableVersion;
    }

    return HeaderStatus_Valid;
}

/// @brief Check for valid assembly header (CRC32 of header, string table version and marker)
///
/// @return Check result
bool CLR_RECORD_ASSEMBLY::GoodHeader() const
{
    NATIVE_PROFILE_CLR_CORE();

    return CheckHeader() == HeaderStatus_Valid;
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

    if (!owner.target->HasILBody())
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
            dst->genericInstance = nullptr;
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
                               offsets.methodSpec + offsets.typeSpec;

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
#if defined(NANOCLR_64BIT_POINTERS)
                CLR_Debug::Printf(" @ 0x%016" PRIxPTR "", (uintptr_t)assm);
#else
                CLR_Debug::Printf(" @ 0x%08" PRIxPTR "", (uintptr_t)assm);
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

                    if (target == nullptr)
                    {
                        // same name but major.minor mismatch: report what is deployed
                        const CLR_RT_Assembly *deployed = g_CLR_RT_TypeSystem.FindAssembly(szName, nullptr, false);

                        if (deployed != nullptr)
                        {
                            CLR_Debug::Printf(
                                "    deployed '%s' is (%d.%d.%d.%d), major.minor must match\r\n",
                                szName,
                                deployed->header->version.majorVersion,
                                deployed->header->version.minorVersion,
                                deployed->header->version.buildNumber,
                                deployed->header->version.revisionNumber);
                        }
                    }
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

            if (inst.assembly->FindTypeDef(szName, inst.Type(), dst->target, src->name) == false)
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

            if (assm->FindTypeDef(szName, szNameSpace, dst->target, src->name, src->nameSpace) == false)
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

            if (!typeSpecInstance.assembly
                     ->FindFieldDef(&typeSpec, fieldName, this, src->signature, dst->target, src->name))
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
                    ->FindFieldDef(typeDefInstance.target, fieldName, this, src->signature, dst->target, src->name) ==
                false)
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

            CLR_UINT32 dummyAssemblyIndex = 0xffffffff;

            if (typeSpecInstance.assembly->FindMethodDef(
                    typeSpecInstance.target,
                    methodName,
                    this,
                    src->signature,
                    dst->target,
                    dummyAssemblyIndex,
                    src->name))
            {
                fGot = true;

                // set TypeSpec
                dst->genericType.data = typeSpec.data;
            }

            if (!fGot && NANOCLR_INDEX_IS_VALID(typeSpecInstance.genericTypeDef))
            {
                CLR_RT_TypeDef_Instance typeDefInst{};
                if (typeDefInst.InitializeFromIndex(typeSpecInstance.genericTypeDef))
                {
                    while (NANOCLR_INDEX_IS_VALID(typeDefInst))
                    {
                        if (typeDefInst.assembly->FindMethodDef(
                                typeDefInst.target,
                                methodName,
                                this,
                                src->signature,
                                dst->target,
                                src->name))
                        {
                            fGot = true;
                            dst->genericType.data = typeSpec.data;
                            break;
                        }

                        typeDefInst.SwitchToParent();
                    }
                }
            }

            if (!fGot && NANOCLR_INDEX_IS_VALID(typeSpecInstance.genericTypeDef))
            {
                CLR_RT_TypeDef_Instance typeDefInst{};
                if (typeDefInst.InitializeFromIndex(typeSpecInstance.genericTypeDef))
                {
                    while (NANOCLR_INDEX_IS_VALID(typeDefInst))
                    {
                        if (typeDefInst.assembly->FindMethodDef(
                                typeDefInst.target,
                                methodName,
                                this,
                                CLR_SIG_INVALID,
                                dst->target,
                                src->name))
                        {
                            fGot = true;
                            dst->genericType.data = typeSpec.data;
                            break;
                        }

                        typeDefInst.SwitchToParent();
                    }
                }
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

            // walk the hierarchy on a copy: typeDefInstance must stay valid for the error message below
            CLR_RT_TypeDef_Instance searchInstance = typeDefInstance;

            while (NANOCLR_INDEX_IS_VALID(searchInstance))
            {
                if (searchInstance.assembly->FindMethodDef(
                        searchInstance.target,
                        methodName,
                        this,
                        src->signature,
                        dst->target,
                        src->name))
                {
                    fGot = true;

                    // invalidate GenericType
                    dst->genericType.data = CLR_EmptyToken;

                    break;
                }

                searchInstance.SwitchToParent();
            }

            if (fGot == false)
            {
#if !defined(BUILD_RTM)
                CLR_Debug::Printf(
                    "Unknown MethodRef: %s.%s.%s\r\n",
                    typeDefInstance.assembly->GetString(typeDefInstance.target->nameSpace),
                    typeDefInstance.assembly->GetString(typeDefInstance.target->name),
                    methodName);
#endif
#if defined(VIRTUAL_DEVICE)
                NANOCLR_CHARMSG_SET_AND_LEAVE(
                    CLR_E_FAIL,
                    "Unknown MethodRef: %s.%s.%s\r\n",
                    typeDefInstance.assembly->GetString(typeDefInstance.target->nameSpace),
                    typeDefInstance.assembly->GetString(typeDefInstance.target->name),
                    methodName);
#else
                NANOCLR_MSG1_SET_AND_LEAVE(
                    CLR_E_FAIL,
                    L"Unknown MethodRef: %s.%s.%s\r\n",
                    typeDefInstance.assembly->GetString(typeDefInstance.target->nameSpace),
                    typeDefInstance.assembly->GetString(typeDefInstance.target->name),
                    methodName);
#endif
            }
#ifdef NANOCLR_TRACE_TYPE_RESOLUTION
            else
            {
                char rgBuffer[512];
                char *szBuffer = rgBuffer;
                size_t iBuffer = MAXSTRLEN(rgBuffer);

                g_CLR_RT_TypeSystem.BuildTypeName(searchInstance, szBuffer, iBuffer);
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
    }
}

CLR_RT_TypeDef_Index CLR_RT_Assembly::GenericDefFromHash(CLR_UINT32 hash)
{
    NATIVE_PROFILE_CLR_CORE();

    // Search through all TypeDefs in the assembly to find one with a matching hash
    for (int i = 0; i < tablesSize[TBL_TypeDef]; i++)
    {
        CLR_RT_TypeDef_Index typeDef;
        typeDef.Set(assemblyIndex, i);

        CLR_RT_TypeDef_Instance inst;
        if (inst.InitializeFromIndex(typeDef))
        {
            if (inst.CrossReference().hash == hash)
            {
                return typeDef;
            }
        }
    }

    // No match found
    CLR_RT_TypeDef_Index emptyIndex;
    emptyIndex.Clear();
    return emptyIndex;
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

    int size = CONVERTFROMSIZETOHEAPBLOCKS(sizeof(CLR_RT_AppDomainAssembly)) + assm->m_iStaticFields +
               CONVERTFROMSIZETOHEAPBLOCKS(assm->tablesSize[TBL_TypeDef] * sizeof(CLR_UINT8));

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
    TIL("System",                                   "ArrayTypeMismatchException",       ArrayTypeMismatchException),
    TIL("System.Reflection",                        "TargetException",                  TargetException),
    TIL("System.Reflection",                        "TargetParameterCountException",    TargetParameterCountException),
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
    TIL(nullptr,                                    "SZArrayHelper",                    SZArrayHelper),
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


    TIL("System.Security.Cryptography",             "CryptographicException",           CryptographicException),
    TIL("System.Device.I2c",                        "I2cTransferResult",                I2cTransferResult),

    TIL("nanoFramework.Hardware.Esp32.Rmt",         "RmtSymbol",                        RmtSymbol),

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

            // Static fields declared on an open generic TypeDef live per-instantiation;
            // this assembly-wide slot is dead storage. Allow VAR fallback so the slot
            // is stamped as a null OBJECT rather than aborting startup.
            NANOCLR_CHECK_HRESULT(
                g_CLR_RT_ExecutionEngine.InitializeReference(pStaticFields[res.offset], fd, this, nullptr, true));
        }
    }

    NANOCLR_NOCLEANUP();
}

// The slots are indexed as an array, so the run has to stay contiguous and unmoved: see CLAUDE.md
// "Static fields on generic types".
static CLR_RT_HeapBlock *CLR_AllocateGenericStaticFieldStorage(CLR_UINT32 count)
{
    NATIVE_PROFILE_CLR_CORE();

    const CLR_UINT32 headerBlocks = CONVERTFROMSIZETOHEAPBLOCKS(sizeof(CLR_RT_HeapBlock_BinaryBlob));

    auto *blob = (CLR_RT_HeapBlock_BinaryBlob *)g_CLR_RT_ExecutionEngine.ExtractHeapBlocksForEvents(
        DATATYPE_BINARY_BLOB_HEAD,
        0,
        headerBlocks + count);

    if (blob == nullptr)
    {
        return nullptr;
    }

    // no handlers: the registry marks and relocates the payload, see CLR_RT_TypeSystem::Relocate
    blob->SetBinaryBlobHandlers(nullptr, nullptr);
    blob->m_assembly = nullptr;

    CLR_RT_HeapBlock *fields = (CLR_RT_HeapBlock *)blob + headerBlocks;

    // the record is registered before the slots are typed, so they have to be walkable right away
    for (CLR_UINT32 i = 0; i < count; i++)
    {
        fields[i].SetObjectReference(nullptr);
    }

    return fields;
}

// The blob is an event block, so only an explicit release returns it. See CLAUDE.md "Static fields on generic types".
static void CLR_ReleaseGenericStaticFieldStorage(CLR_RT_HeapBlock *fields)
{
    const CLR_UINT32 headerBlocks = CONVERTFROMSIZETOHEAPBLOCKS(sizeof(CLR_RT_HeapBlock_BinaryBlob));

    ((CLR_RT_HeapBlock_BinaryBlob *)(fields - headerBlocks))->Release(true);
}

HRESULT CLR_RT_Assembly::ResolveAllocateGenericTypeStaticFields()
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    // A failed Assembly.Load must leave no instantiation behind that a later load would reuse.
    CLR_UINT32 firstNew = g_CLR_RT_TypeSystem.m_genericInstancesCount;
    int numTypeSpec = this->tablesSize[TBL_TypeSpec];

    for (int iTs = 0; iTs < numTypeSpec; iTs++)
    {
        CLR_RT_TypeSpec_Instance genericTypeInstance{};
        CLR_RT_TypeSpec_Index tsIndex;
        CLR_RT_TypeDef_Instance owner{};
        tsIndex.Set(assemblyIndex, iTs);

        if (!genericTypeInstance.InitializeFromIndex(tsIndex) || !genericTypeInstance.IsClosedGenericType() ||
            !owner.InitializeFromIndex(genericTypeInstance.genericTypeDef) || owner.target->staticFieldsCount == 0)
        {
            continue;
        }

        CLR_RT_GenericInstance *instance =
            CLR_RT_TypeSystem::FindOrCreateGenericInstance(genericTypeInstance, nullptr, nullptr, &hr);

        if (instance == nullptr)
        {
            if (hr == CLR_E_OUT_OF_MEMORY)
            {
                NANOCLR_LEAVE();
            }

            // Over the limits or unresolvable: no storage, its static field accesses throw.
            hr = S_OK;
            continue;
        }

        NANOCLR_CHECK_HRESULT(CLR_RT_TypeSystem::AllocateGenericInstanceStatics(*instance));
    }

    NANOCLR_CLEANUP();

    if (FAILED(hr))
    {
        g_CLR_RT_TypeSystem.TruncateGenericInstances(firstNew);
    }

    NANOCLR_CLEANUP_END();
}

static void ScheduleGenericCctor(CLR_RT_GenericInstance &instance)
{
    CLR_RT_TypeDef_Index typeDef = instance.m_nodes[0].Class;
    CLR_RT_Assembly *ownerAsm = g_CLR_RT_TypeSystem.m_assemblies[typeDef.Assembly() - 1];
    const CLR_RECORD_TYPEDEF *ownerTd = ownerAsm->GetTypeDef(typeDef.Type());
    const CLR_RECORD_METHODDEF *md = ownerAsm->GetMethodDef(ownerTd->firstMethod);
    int methodCount = ownerTd->virtualMethodCount + ownerTd->instanceMethodCount + ownerTd->staticMethodCount;

    if (instance.m_flags & (CLR_RT_GenericInstance::c_Scheduled | CLR_RT_GenericInstance::c_Executed))
    {
        return;
    }

    for (int i = 0; i < methodCount; i++, md++)
    {
        if ((md->flags & CLR_RECORD_METHODDEF::MD_StaticConstructor) == 0)
        {
            continue;
        }

        CLR_RT_MethodDef_Index cctorIndex;
        cctorIndex.Set(ownerAsm->assemblyIndex, ownerTd->firstMethod + i);

        if (!g_CLR_RT_ExecutionEngine.EnsureSystemThread(
                g_CLR_RT_ExecutionEngine.m_cctorThread,
                ThreadPriority::System_Highest))
        {
            return;
        }

        CLR_RT_HeapBlock refDlg;
        refDlg.SetObjectReference(nullptr);
        CLR_RT_ProtectFromGC gc(refDlg);

        if (FAILED(CLR_RT_HeapBlock_Delegate::CreateInstance(refDlg, cctorIndex, nullptr)))
        {
            return;
        }

        CLR_RT_HeapBlock_Delegate *dlg = refDlg.DereferenceDelegate();

        // The instantiation is its own context. See CLAUDE.md "Generic .cctor lifecycle".
        dlg->m_genericTypeSpec = instance.m_self;
        dlg->m_genericMethodSpec.Clear();

        if (SUCCEEDED(g_CLR_RT_ExecutionEngine.m_cctorThread->PushThreadProcDelegate(dlg)))
        {
            instance.m_flags |= CLR_RT_GenericInstance::c_Scheduled;
            g_CLR_RT_ExecutionEngine.m_cctorThread->m_terminationCallback =
                CLR_RT_ExecutionEngine::StaticConstructorTerminationCallback;
        }

        return;
    }
}

CLR_RT_HeapBlock *CLR_RT_Assembly::GetStaticFieldByFieldDef(
    const CLR_RT_FieldDef_Index &fdIndex,
    const CLR_RT_TypeSpec_Index *genericType,
    const CLR_RT_TypeSpec_Index *contextTypeSpec,
    const CLR_RT_MethodDef_Instance *contextMethod)
{
    NATIVE_PROFILE_CLR_CORE();

    // Generic static fields live with their instantiation. See CLAUDE.md "Static fields on generic types".
    if (genericType != nullptr && NANOCLR_INDEX_IS_VALID(*genericType))
    {
        CLR_RT_TypeSpec_Instance tsInst;

        if (!tsInst.InitializeFromIndex(*genericType))
        {
            return nullptr;
        }

        // The owner has to be closed, with the same fallback as ResolveToken, before any storage is used.
        CLR_RT_GenericInstance *instance = CLR_RT_TypeSystem::FindOrCreateGenericInstance(
            tsInst,
            contextTypeSpec,
            contextMethod,
            nullptr,
            contextMethod != nullptr && !NANOCLR_INDEX_IS_VALID(contextMethod->methodSpec)
                ? &contextMethod->arrayElementType
                : nullptr);

        if (instance == nullptr)
        {
            return nullptr;
        }

        if (instance->m_fields == nullptr)
        {
            if (FAILED(CLR_RT_TypeSystem::AllocateGenericInstanceStatics(*instance)))
            {
                return nullptr;
            }

            if (instance->m_fields != nullptr)
            {
                ScheduleGenericCctor(*instance);
            }
        }

        for (CLR_UINT32 i = 0; i < instance->m_count; i++)
        {
            if (instance->m_fieldDefs[i].data == fdIndex.data)
            {
                return &instance->m_fields[i];
            }
        }

        return nullptr;
    }

    // fallback to assembly static fields (use offset stored on crossReferenceFieldDef)
    const CLR_RT_FieldDef_CrossReference &fdCross = crossReferenceFieldDef[fdIndex.Field()];

    _ASSERTE(fdCross.offset != CLR_EmptyIndex);

#if defined(NANOCLR_TRACE_GENERICS)
    if (s_CLR_RT_fTrace_GenericFields >= c_CLR_RT_Trace_Info)
    {
        CLR_Debug::Printf(
            "GenericStatic: GetStaticFieldByFieldDef fallback to assembly static offset=%u\r\n",
            fdCross.offset);
    }
#endif

#if defined(NANOCLR_APPDOMAINS)
    return GetStaticField(fdCross.offset); // existing method that uses current AppDomain's m_pStaticFields
#else
    return GetStaticField(fdCross.offset);
#endif
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

bool CLR_RT_Assembly::FindTypeDef(
    const char *typeName,
    const char *nameSpace,
    CLR_RT_TypeDef_Index &index,
    CLR_STRING typeNameId,
    CLR_STRING nameSpaceId)
{
    NATIVE_PROFILE_CLR_CORE();
    const CLR_RECORD_TYPEDEF *target = GetTypeDef(0);
    int tblSize = tablesSize[TBL_TypeDef];
    bool isNestedType = false;
    std::string extractedNamespace;
    std::string enclosedTypeName;

    // Check if typeName contains '/'
    const char *slashPos = strchr(typeName, '/');
    if (slashPos != nullptr)
    {
        // Extract the type name from the '/' to the end of the string
        const char *extractedTypeName = slashPos + 1;

        // Extract the enclosed type name from the '/' backwards to the '.' before
        const char *dotPos = strrchr(typeName, '.');

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

                if (!strcmp(szName, typeName))
                {
                    // Verify the enclosing type name when the path was "Outer/Nested" — same-named
                    // nested types in different generic outers must not collide.
                    if (!enclosedTypeName.empty() && target->EnclosingType() == TBL_TypeDef)
                    {
                        CLR_INDEX enclosingIdx = target->EnclosingTypeIndex();
                        const CLR_RECORD_TYPEDEF *enclosingTD = GetTypeDef(enclosingIdx);
                        const char *enclosingName = GetString(enclosingTD->name);
                        if (strcmp(enclosingName, enclosedTypeName.c_str()) != 0)
                        {
                            continue;
                        }
                    }

                    index.Set(assemblyIndex, i);
                    return true;
                }
            }
        }
        else if (!target->HasValidEnclosingType())
        {
            if (StringEquals(target->name, typeName, typeNameId) &&
                StringEquals(target->nameSpace, nameSpace, nameSpaceId))
            {
                index.Set(assemblyIndex, i);
                return true;
            }
        }
    }

    index.Clear();
    return false;
}

bool CLR_RT_Assembly::FindTypeDef(
    const char *typeName,
    CLR_INDEX scope,
    CLR_RT_TypeDef_Index &index,
    CLR_STRING typeNameId)
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

        // Filter to nested types whose EnclosingType is a TypeDef (matches the caller's
        // index space) — without this, a TypeRef row with a colliding index can match.
        if (target->EnclosingType() != TBL_TypeDef)
        {
            continue;
        }

        CLR_INDEX enclosingTypeIndex = target->EnclosingTypeIndex();

        if (enclosingTypeIndex == scope)
        {
            if (StringEquals(target->name, typeName, typeNameId))
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
    CLR_RT_FieldDef_Index &res,
    CLR_STRING textId)
{
    NATIVE_PROFILE_CLR_CORE();
    const CLR_RECORD_FIELDDEF *fd = assm->GetFieldDef(first);

    for (CLR_UINT32 i = 0; i < num; i++, fd++)
    {
        if (assm->StringEquals(fd->name, szText, textId))
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
    CLR_RT_FieldDef_Index &index,
    CLR_STRING fieldNameId)
{
    NATIVE_PROFILE_CLR_CORE();

    if (local_FindFieldDef(
            this,
            td->firstInstanceField,
            td->instanceFieldsCount,
            fieldName,
            base,
            sig,
            index,
            fieldNameId))
    {
        return true;
    }

    if (local_FindFieldDef(this, td->firstStaticField, td->staticFieldsCount, fieldName, base, sig, index, fieldNameId))
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
    CLR_RT_FieldDef_Index &index,
    CLR_STRING fieldNameId)
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
        if (StringEquals(fd->type, typeName, typeDefInstance.target->name) &&
            StringEquals(fd->name, fieldName, fieldNameId))
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
    CLR_RT_MethodDef_Index &index,
    CLR_STRING methodNameId)
{
    NATIVE_PROFILE_CLR_CORE();
    int i;
    int num = td->virtualMethodCount + td->instanceMethodCount + td->staticMethodCount;
    const CLR_RECORD_METHODDEF *md = GetMethodDef(td->firstMethod);

    for (i = 0; i < num; i++, md++)
    {
        if (StringEquals(md->name, methodName, methodNameId))
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
    CLR_UINT32 &assmIndex,
    CLR_STRING methodNameId)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_TypeSpec_Index tsIndex;
    if (!base->FindTypeSpec(base->GetSignature(ts->signature), tsIndex))
    {
        index.Clear();
        return false;
    }

    CLR_RT_TypeSpec_Instance tsInstance{};
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
        if (typeDefInstance.assembly->FindMethodDef(typeDefInstance.target, methodName, this, sig, index, methodNameId))
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

    if (!p->HasILBody())
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

        // native rows carry a slot index in rva, skip them
        if (p->HasILBody())
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

bool CLR_RT_Assembly::HasStaticConstructor(const CLR_RT_TypeDef_Index &typeDef) const
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_Assembly *ownerAsm = g_CLR_RT_TypeSystem.m_assemblies[typeDef.Assembly() - 1];
    const CLR_RECORD_TYPEDEF *ownerTd = ownerAsm->GetTypeDef(typeDef.Type());

    // Calculate total method count for this type
    int methodCount = ownerTd->virtualMethodCount + ownerTd->instanceMethodCount + ownerTd->staticMethodCount;

    // Iterate through all methods of the generic type definition
    const CLR_RECORD_METHODDEF *md = ownerAsm->GetMethodDef(ownerTd->firstMethod);
    for (int i = 0; i < methodCount; i++, md++)
    {
        // Check if this is a static constructor
        if (md->flags & CLR_RECORD_METHODDEF::MD_StaticConstructor)
        {
            return true;
        }
    }

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

void CLR_RT_TypeSystem::Relocate()
{
    NATIVE_PROFILE_CLR_CORE();

    // The registry is global, so this runs once per relocation pass, never per assembly:
    // see CLAUDE.md "Static fields on generic types".
    for (CLR_UINT32 i = 0; i < m_genericInstancesCount; i++)
    {
        CLR_RT_GarbageCollector::RelocateGenericStaticField(m_genericInstances[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void CLR_RT_TypeSystem::TypeSystem_Initialize()
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_CLEAR(g_CLR_RT_TypeSystem);
    NANOCLR_CLEAR(g_CLR_RT_WellKnownTypes);

    g_CLR_RT_TypeSystem.m_genericInstances = nullptr;
    g_CLR_RT_TypeSystem.m_genericInstancesCount = 0;
    g_CLR_RT_TypeSystem.m_genericInstancesMaxCount = 0;
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

    TruncateGenericInstances(0);

    if (m_genericInstances != nullptr)
    {
        platform_free(m_genericInstances);
        m_genericInstances = nullptr;
        m_genericInstancesMaxCount = 0;
    }
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
            // exact match was NOT required but there is version information: enforce major.minor only.
            // A minor bump is a deliberate managed binary break; native compatibility is checked by the
            // contract hash (nativeContractHash) at load, not by the version.
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
                    NANOCLR_CHECK_HRESULT(pASSM->ResolveAllocateGenericTypeStaticFields());
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
                            offsets.methodRef + offsets.typeDef + offsets.fieldDef + offsets.methodDef;

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

        if (!CLR_RT_ExecutionEngine::IsInstanceOf(descRight, descLeft))
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
                return true;
            }
        }

        if (resLeft.DataType == DATATYPE_GENERICINST && resRight.DataType == DATATYPE_GENERICINST)
        {
            // Advance past the GENERICINST marker to the generic typedef + arg count.
            if (FAILED(parserLeft.Advance(resLeft)) || FAILED(parserRight.Advance(resRight)))
            {
                return false;
            }

            // The generic type definitions and argument counts must match exactly.
            if (resLeft.Class.data != resRight.Class.data)
            {
                return false;
            }

            if (resLeft.GenParamCount != resRight.GenParamCount)
            {
                return false;
            }

            // CLASS vs VALUETYPE must also agree.
            if (resLeft.DataType != resRight.DataType)
            {
                return false;
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
    CLR_UINT32 levels,
    const CLR_RT_TypeSpec_Index *contextTypeSpec,
    const CLR_RT_MethodDef_Instance *contextMethodDef)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeSpec_Instance typeSpecInstance;
    bool closeGenericSignature = false;

    if (typeSpecInstance.InitializeFromIndex(typeIndex) == false)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    CLR_RT_SignatureParser parser;
    parser.Initialize_TypeSpec(typeSpecInstance);

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

    NANOCLR_CHECK_HRESULT(BuildTypeName(typeDef, szBuffer, iBuffer));

    if (element.GenParamCount > 0)
    {
        NANOCLR_CHECK_HRESULT(QueueStringToBuffer(szBuffer, iBuffer, "<"));

        closeGenericSignature = true;
    }

    for (int i = 0; i < element.GenParamCount; i++)
    {
        // read the next element (should be either VAR, MVAR, or a concrete type)
        parser.Advance(element);

        if (element.DataType == DATATYPE_VAR)
        {
            // resolve the !T against our *closed* typeIndex, if possible
            if (contextTypeSpec != nullptr && NANOCLR_INDEX_IS_VALID(*contextTypeSpec))
            {
                // generic type parameter

                CLR_RT_TypeSpec_Instance typeContextTs;
                if (!typeContextTs.InitializeFromIndex(*contextTypeSpec))
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
                }

                CLR_RT_SignatureParser::Element paramElement;
                // this will bind !T→System.Int32, etc.
                if (!typeContextTs.GetGenericParam(element.GenericParamPosition, paramElement))
                {
                    // couldn't be resolved, print encoded form (!N)
                    char encodedParam[6];
                    snprintf(encodedParam, ARRAYSIZE(encodedParam), "!%d", element.GenericParamPosition);
                    NANOCLR_CHECK_HRESULT(QueueStringToBuffer(szBuffer, iBuffer, encodedParam));
                }
                else if (paramElement.DataType == DATATYPE_VAR)
                {
                    // couldn't be resolved, print encoded form (!N)
                    char encodedParam[6];
                    snprintf(encodedParam, ARRAYSIZE(encodedParam), "!%d", element.GenericParamPosition);
                    NANOCLR_CHECK_HRESULT(QueueStringToBuffer(szBuffer, iBuffer, encodedParam));
                }
                else
                {
                    // now print the *actual* type name
                    BuildTypeName(paramElement.Class, szBuffer, iBuffer);
                }
            }
            else
            {
                // couldn't be resolved, print encoded form (!N)
                char encodedParam[6];
                snprintf(encodedParam, ARRAYSIZE(encodedParam), "!%d", element.GenericParamPosition);
                NANOCLR_CHECK_HRESULT(QueueStringToBuffer(szBuffer, iBuffer, encodedParam));
            }
        }
        else if (element.DataType == DATATYPE_MVAR)
        {
            // method generic parameter -- try to resolve via MethodSpec; fall back to !!N only on failure
            bool mvarResolved = false;

            if (contextMethodDef != nullptr && NANOCLR_INDEX_IS_VALID(*contextMethodDef) &&
                NANOCLR_INDEX_IS_VALID(contextMethodDef->methodSpec))
            {
                CLR_RT_MethodSpec_Instance methodSpec{};
                CLR_RT_SignatureParser::Element paramElement;
                methodSpec.InitializeFromIndex(contextMethodDef->methodSpec);

                if (methodSpec.GetGenericArgument(element.GenericParamPosition, paramElement))
                {
                    CLR_RT_TypeDef_Index paramTypeDef = paramElement.Class;

                    if (paramElement.DataType == DATATYPE_VAR)
                    {
                        // VAR inside a MethodSpec arg -- resolve against the declaring type's closed TypeSpec
                        const CLR_RT_TypeSpec_Index *effectiveContext = nullptr;
                        CLR_RT_TypeSpec_Instance contextTs;
                        CLR_RT_SignatureParser::Element argElement;

                        if (contextTypeSpec != nullptr && NANOCLR_INDEX_IS_VALID(*contextTypeSpec))
                        {
                            effectiveContext = contextTypeSpec;
                        }
                        else if (
                            contextMethodDef->genericType != nullptr &&
                            NANOCLR_INDEX_IS_VALID(*contextMethodDef->genericType))
                        {
                            effectiveContext = contextMethodDef->genericType;
                        }

                        if (effectiveContext != nullptr && contextTs.InitializeFromIndex(*effectiveContext) &&
                            contextTs.GetGenericParam(paramElement.GenericParamPosition, argElement))
                        {
                            paramTypeDef = argElement.Class;
                        }
                    }

                    if (paramTypeDef.data != 0)
                    {
                        NANOCLR_CHECK_HRESULT(BuildTypeName(paramTypeDef, szBuffer, iBuffer));
                        mvarResolved = true;
                    }
                }
            }

            if (!mvarResolved)
            {
                // resolution failed (no MethodSpec, GetGenericArgument returned false, etc.) -- print !!N
                char encodedParam[7];
                snprintf(encodedParam, ARRAYSIZE(encodedParam), "!!%d", element.GenericParamPosition);
                NANOCLR_CHECK_HRESULT(QueueStringToBuffer(szBuffer, iBuffer, encodedParam));
            }
        }
        else
        {
            // concrete type (e.g. a nested generic, a value type, another class...)
            CLR_RT_TypeDef_Index td;
            td.data = element.Class.data;

            NANOCLR_CHECK_HRESULT(BuildTypeName(td, szBuffer, iBuffer));
        }

        if (i + 1 < element.GenParamCount)
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

    if (genericType != nullptr && NANOCLR_INDEX_IS_VALID(*genericType) && genericType->data != CLR_EmptyToken)
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

HRESULT CLR_RT_TypeSystem::BuildMethodName(
    const CLR_RT_MethodDef_Instance &mdInst,
    const CLR_RT_TypeSpec_Index *genericType,
    char *&szBuffer,
    size_t &iBuffer)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Instance declTypeInst{};
    CLR_RT_TypeDef_Index declTypeIdx;
    CLR_RT_TypeDef_Instance instOwner{};
    bool useGeneric = false;

    if (!declTypeInst.InitializeFromMethod(mdInst))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    declTypeIdx.Set(mdInst.Assembly(), declTypeInst.assembly->crossReferenceMethodDef[mdInst.Method()].GetOwner());

    if (mdInst.genericType && NANOCLR_INDEX_IS_VALID(*mdInst.genericType))
    {
        useGeneric = true;
    }
    else if (genericType && NANOCLR_INDEX_IS_VALID(*genericType))
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
        if (instOwner.InitializeFromMethod(mdInst) == false)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }

        NANOCLR_CHECK_HRESULT(BuildTypeName(instOwner, szBuffer, iBuffer));

        CLR_SafeSprintf(szBuffer, iBuffer, "::%s", mdInst.assembly->GetString(mdInst.target->name));
    }
    else
    {
        // First, build the type name (either from genericType or from the method's declaring type)
        if (mdInst.genericType != nullptr && NANOCLR_INDEX_IS_VALID(*mdInst.genericType))
        {
            // Use the method's genericType and pass itself as context to resolve VAR parameters
            if (FAILED(BuildTypeName(*mdInst.genericType, szBuffer, iBuffer, 0, mdInst.genericType, &mdInst)))
            {
                // Fall back to the declaring type
                if (instOwner.InitializeFromMethod(mdInst) == false)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
                }
                NANOCLR_CHECK_HRESULT(BuildTypeName(instOwner, szBuffer, iBuffer));
            }
        }
        else
        {
            // Fall back to the declaring type
            if (instOwner.InitializeFromMethod(mdInst) == false)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
            }
            NANOCLR_CHECK_HRESULT(BuildTypeName(instOwner, szBuffer, iBuffer));
        }

        // Append the method name
        CLR_SafeSprintf(szBuffer, iBuffer, "::%s", mdInst.assembly->GetString(mdInst.target->name));

        // If this method itself is generic (has its own generic parameters) and has a methodSpec, append them.
        // Do NOT append when methodSpec was merely inherited from the caller for MVAR resolution (e.g. .ctor).
        if (NANOCLR_INDEX_IS_VALID(mdInst.methodSpec) && mdInst.target->genericParamCount > 0)
        {
            CLR_RT_MethodSpec_Instance msInst{};
            if (msInst.InitializeFromIndex(mdInst.methodSpec))
            {
                // Parse the methodSpec instantiation signature to get the generic arguments
                CLR_RT_SignatureParser parser{};
                parser.Initialize_MethodSignature(&msInst);

                CLR_SafeSprintf(szBuffer, iBuffer, "<");

                for (int i = 0; i < parser.ParamCount; i++)
                {
                    CLR_RT_SignatureParser::Element elem{};
                    if (FAILED(parser.Advance(elem)))
                    {
                        break;
                    }

                    if (i > 0)
                    {
                        CLR_SafeSprintf(szBuffer, iBuffer, ", ");
                    }

                    if (elem.DataType == DATATYPE_VAR)
                    {
                        // Build the type name for this generic argument
                        // Use the method's declaring type as context for VAR resolution

                        CLR_RT_TypeSpec_Instance contextTs;
                        CLR_RT_SignatureParser::Element paramElement;

                        // Compute effective generic context: prefer the explicit genericType parameter,
                        // fall back to the method instance own genericType to avoid null dereference.
                        CLR_RT_TypeSpec_Index effectiveGenericIndex;
                        if (genericType != nullptr && NANOCLR_INDEX_IS_VALID(*genericType))
                        {
                            effectiveGenericIndex = *genericType;
                        }
                        else if (mdInst.genericType != nullptr && NANOCLR_INDEX_IS_VALID(*mdInst.genericType))
                        {
                            effectiveGenericIndex = *mdInst.genericType;
                        }
                        else
                        {
                            CLR_SafeSprintf(szBuffer, iBuffer, "!%d", elem.GenericParamPosition);
                            continue;
                        }

                        // try to resolve from method context
                        if (!contextTs.InitializeFromIndex(effectiveGenericIndex))
                        {
                            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
                        }

                        if (!contextTs.GetGenericParam(elem.GenericParamPosition, paramElement))
                        {
                            // Couldn't resolve
                            CLR_SafeSprintf(szBuffer, iBuffer, "!%d", elem.GenericParamPosition);
                            continue;
                        }

                        NANOCLR_CHECK_HRESULT(BuildTypeName(paramElement.Class, szBuffer, iBuffer));
                    }
                    else if (elem.DataType == DATATYPE_MVAR)
                    {
                        CLR_SafeSprintf(szBuffer, iBuffer, "!!%d", elem.GenericParamPosition);
                    }
                    else if (NANOCLR_INDEX_IS_VALID(elem.Class))
                    {
                        // Concrete type
                        NANOCLR_CHECK_HRESULT(BuildTypeName(elem.Class, szBuffer, iBuffer));
                    }
                }

                CLR_SafeSprintf(szBuffer, iBuffer, ">");
            }
        }
    }

    NANOCLR_NOCLEANUP();
}

// Overload that accepts an explicit parent context TypeSpec for resolving open VAR parameters
// in mdInst.genericType.  When the method's declaring type has open generic params (e.g.
// List`1+Enumerator<!0>), parentCtx supplies the closed type (e.g. List<String>) so that
// !0 is printed as "String" rather than "!0".
HRESULT CLR_RT_TypeSystem::BuildMethodName(
    const CLR_RT_MethodDef_Instance &mdInst,
    const CLR_RT_TypeSpec_Index *genericType,
    const CLR_RT_TypeSpec_Index *parentCtx,
    char *&szBuffer,
    size_t &iBuffer)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Instance instOwner{};
    bool useGeneric = (mdInst.genericType != nullptr && NANOCLR_INDEX_IS_VALID(*mdInst.genericType));

    // If there is no useful parent context, fall back to the two-arg overload.
    if (parentCtx == nullptr || !NANOCLR_INDEX_IS_VALID(*parentCtx))
    {
        NANOCLR_CHECK_HRESULT(BuildMethodName(mdInst, genericType, szBuffer, iBuffer));
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    if (!useGeneric)
    {
        // Non-generic method: print owning type name + method name the standard way.
        if (!instOwner.InitializeFromMethod(mdInst))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }
        NANOCLR_CHECK_HRESULT(BuildTypeName(instOwner, szBuffer, iBuffer));
        CLR_SafeSprintf(szBuffer, iBuffer, "::%s", mdInst.assembly->GetString(mdInst.target->name));
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    // Generic method: build the declaring type name using parentCtx to resolve open VARs.
    // e.g. mdInst.genericType = List+Enumerator<VAR0>, parentCtx = List<String>
    //   → BuildTypeName resolves VAR0 via parentCtx → "List+Enumerator<String>"
    if (FAILED(BuildTypeName(*mdInst.genericType, szBuffer, iBuffer, 0, parentCtx, &mdInst)))
    {
        // Fall back: render without parent context.
        if (FAILED(BuildTypeName(*mdInst.genericType, szBuffer, iBuffer, 0, mdInst.genericType, &mdInst)))
        {
            if (!instOwner.InitializeFromMethod(mdInst))
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
            }
            NANOCLR_CHECK_HRESULT(BuildTypeName(instOwner, szBuffer, iBuffer));
        }
    }

    CLR_SafeSprintf(szBuffer, iBuffer, "::%s", mdInst.assembly->GetString(mdInst.target->name));

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

            if (i + 1 < element.GenParamCount)
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

        if (i + 1 < element.GenParamCount)
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

static const int c_MaxInheritanceDepth = 32;

static HRESULT CollectGenericNodesAt(
    CLR_RT_SignatureParser parser,
    const CLR_RT_TypeSpec_Index *contextTypeSpec,
    const CLR_RT_MethodDef_Instance *contextMethod,
    const CLR_RT_TypeDef_Index *mvarFallback,
    CLR_RT_GenericInstanceNode *nodes,
    int &nodeCount,
    bool &isClosed,
    bool &substituted);
static bool AppendGenericInstanceNode(
    CLR_RT_GenericInstanceNode *nodes,
    int &nodeCount,
    const CLR_RT_SignatureParser::Element &elem,
    int levels);
static CLR_RT_GenericInstance *RecordOfTypeSpec(const CLR_RT_TypeSpec_Index &index);

bool CLR_RT_GenericDispatch::CollectElementNodes(
    CLR_RT_SignatureParser &parser,
    CLR_RT_GenericTarget &out,
    bool &isByRef)
{
    const int end = parser.Available() - 1;
    CLR_RT_SignatureParser start = parser;
    CLR_RT_SignatureParser::Element elem;

    out.m_count = 0;

    if (FAILED(parser.Advance(elem)))
    {
        return false;
    }

    isByRef = elem.IsByRef;

    if (elem.DataType == DATATYPE_GENERICINST)
    {
        bool closed;
        bool substituted;

        if (FAILED(
                CollectGenericNodesAt(start, nullptr, nullptr, nullptr, out.m_nodes, out.m_count, closed, substituted)))
        {
            return false;
        }

        out.m_nodes[0].Levels = (CLR_UINT8)elem.Levels;
    }
    else if (elem.GenParamCount > 0 || !AppendGenericInstanceNode(out.m_nodes, out.m_count, elem, elem.Levels))
    {
        return false;
    }

    return DrainGenericInstSubtree(parser, end);
}

bool CLR_RT_GenericDispatch::SubstituteNodes(
    CLR_RT_GenericTarget &stream,
    const CLR_RT_GenericInstanceNode *binding,
    int bindingCount,
    const CLR_RT_GenericInstanceNode *methodBinding,
    int methodCount)
{
    CLR_RT_GenericTarget result;

    result.m_count = 0;

    for (int i = 0; i < stream.m_count; i++)
    {
        const CLR_RT_GenericInstanceNode &node = stream.m_nodes[i];
        const CLR_RT_GenericInstanceNode *nodes = node.DataType == DATATYPE_VAR    ? binding
                                                  : node.DataType == DATATYPE_MVAR ? methodBinding
                                                                                   : nullptr;
        int count = node.DataType == DATATYPE_VAR ? bindingCount : methodCount;

        if (nodes == nullptr)
        {
            if (result.m_count >= CLR_RT_GenericInstance::c_MaxNodes)
            {
                return false;
            }

            result.m_nodes[result.m_count++] = node;
            continue;
        }

        const CLR_RT_GenericInstanceNode *end = nodes + count;
        const CLR_RT_GenericInstanceNode *argument = nodes + 1;
        int size = 0;

        if (count == 0 || node.Position >= nodes[0].ArgCount)
        {
            return false;
        }

        for (int a = 0; a <= node.Position; a++)
        {
            argument += size;
            size = CLR_RT_GenericAssignability::SubtreeSize(argument, end);

            if (size == 0)
            {
                return false;
            }
        }

        if (result.m_count + size > CLR_RT_GenericInstance::c_MaxNodes || argument->Levels + node.Levels > 0xFF)
        {
            return false;
        }

        memcpy(&result.m_nodes[result.m_count], argument, sizeof(CLR_RT_GenericInstanceNode) * size);
        result.m_nodes[result.m_count].Levels = (CLR_UINT8)(result.m_nodes[result.m_count].Levels + node.Levels);
        result.m_count += size;
    }

    stream = result;

    return true;
}

bool CLR_RT_GenericDispatch::GetBaseContext(
    const CLR_RT_TypeDef_Instance &inst,
    const CLR_RT_GenericTarget *context,
    CLR_RT_GenericTarget &base)
{
    CLR_RT_TypeSpec_Index baseSpec;
    CLR_RT_TypeSpec_Instance baseInstance{};
    CLR_RT_SignatureParser parser;
    CLR_RT_GenericTarget stream;
    bool isByRef;

    base.m_count = 0;

    if (!inst.GetExtendsTypeSpec(baseSpec))
    {
        return true;
    }

    if (!baseInstance.InitializeFromIndex(baseSpec))
    {
        return false;
    }

    parser.Initialize_TypeSpec(baseInstance);

    if (!CollectElementNodes(parser, stream, isByRef) || !SubstituteNodes(stream, context))
    {
        return false;
    }

    base = stream;

    return true;
}

HRESULT CLR_RT_GenericDispatch::GetCalleeBinding(
    const CLR_RT_TypeDef_Index &cls,
    const CLR_RT_TypeDef_Instance &declaring,
    const CLR_RT_GenericInstance *site,
    CLR_RT_GenericTarget &binding,
    bool &ambiguous)
{
    CLR_RT_TypeDef_Instance inst{};
    CLR_RT_GenericTarget context;
    CLR_RT_GenericTarget candidate;
    const bool isInterface = declaring.target->IsInterface();
    bool haveContext = false;
    int best = 0;

    inst.InitializeFromIndex(cls);
    binding.m_count = 0;

    for (int level = 0; level < c_MaxInheritanceDepth && NANOCLR_INDEX_IS_VALID(inst); level++)
    {
        const CLR_RT_GenericTarget *outer = haveContext ? &context : nullptr;

        if (!isInterface)
        {
            if (inst.data == declaring.data)
            {
                if (haveContext)
                {
                    binding = context;
                }

                return S_OK;
            }
        }
        else
        {
            CLR_RT_SignatureParser parser{};

            parser.Initialize_Interfaces(inst.assembly, inst.target);

            while (parser.Available() > 0)
            {
                CLR_RT_SignatureParser peek = parser;
                CLR_RT_SignatureParser::Element elem;
                CLR_UINT32 tk = CLR_TkFromStream(peek.Signature);

                if (FAILED(parser.Advance(elem)))
                {
                    break;
                }

                if (elem.Class.data != declaring.data || CLR_TypeFromTk(tk) != TBL_TypeSpec)
                {
                    continue;
                }

                CLR_RT_TypeSpec_Index entryIndex;
                CLR_RT_TypeSpec_Instance entry{};
                CLR_RT_SignatureParser at{};
                bool isByRef;

                entryIndex.Set(inst.assembly->assemblyIndex, CLR_DataFromTk(tk));

                if (!entry.InitializeFromIndex(entryIndex))
                {
                    continue;
                }

                at.Initialize_TypeSpec(entry);

                if (!CollectElementNodes(at, candidate, isByRef) || !SubstituteNodes(candidate, outer))
                {
                    return CLR_E_NOT_SUPPORTED;
                }

                int kind = 1;

                if (site != nullptr)
                {
                    CLR_RT_GenericRef source = {candidate.m_nodes, candidate.m_nodes + candidate.m_count, 0};
                    CLR_RT_GenericRef target = {site->m_nodes, site->m_nodes + site->m_nodeCount, 0};

                    if (candidate.m_count == site->m_nodeCount &&
                        memcmp(
                            candidate.m_nodes,
                            site->m_nodes,
                            sizeof(CLR_RT_GenericInstanceNode) * candidate.m_count) == 0)
                    {
                        kind = 3;
                    }
                    else if (CLR_RT_GenericAssignability::SameClassAssignable(source, target, 0))
                    {
                        kind = 2;
                    }
                }

                if (best != 0 && (candidate.m_count != binding.m_count ||
                                  memcmp(
                                      candidate.m_nodes,
                                      binding.m_nodes,
                                      sizeof(CLR_RT_GenericInstanceNode) * candidate.m_count) != 0))
                {
                    ambiguous = true;
                }

                if (kind > best)
                {
                    binding = candidate;
                    best = kind;
                }
            }
        }

        if (!GetBaseContext(inst, outer, candidate))
        {
            return CLR_E_NOT_SUPPORTED;
        }

        context = candidate;
        haveContext = true;

        if (!inst.SwitchToParent())
        {
            break;
        }
    }

    return best > 0 ? S_OK : S_FALSE;
}

int CLR_RT_GenericDispatch::MatchSignatureInClass(
    CLR_RT_MethodDef_Instance &callee,
    const CLR_RT_GenericTarget *calleeBinding,
    CLR_RT_Assembly *assembly,
    const CLR_RECORD_METHODDEF *method,
    const CLR_RT_GenericTarget *context)
{
    CLR_RT_SignatureParser left{};
    CLR_RT_SignatureParser right{};
    CLR_RT_GenericTarget leftNodes;
    CLR_RT_GenericTarget rightNodes;
    bool leftByRef;
    bool rightByRef;

    left.Initialize_MethodSignature(&callee);
    right.Initialize_MethodSignature(assembly, method);

    if (left.Type != right.Type || left.Flags != right.Flags)
    {
        return 0;
    }

    while (left.Available() > 0 && right.Available() > 0)
    {
        if (!CollectElementNodes(left, leftNodes, leftByRef) || !CollectElementNodes(right, rightNodes, rightByRef))
        {
            return -1;
        }

        if (!SubstituteNodes(leftNodes, calleeBinding) || !SubstituteNodes(rightNodes, context))
        {
            return 0;
        }

        if (leftByRef != rightByRef || leftNodes.m_count != rightNodes.m_count ||
            memcmp(leftNodes.m_nodes, rightNodes.m_nodes, sizeof(CLR_RT_GenericInstanceNode) * leftNodes.m_count) != 0)
        {
            return 0;
        }
    }

    return left.Available() == right.Available() ? 1 : 0;
}

static bool MatchInterfaceQualifier(const char *pattern, const char *target, size_t targetLen)
{
    size_t t = 0;

    while (*pattern)
    {
        if (*pattern == '`')
        {
            int depth = 0;

            pattern++;

            while (*pattern >= '0' && *pattern <= '9')
            {
                pattern++;
            }

            if (t >= targetLen || target[t] != '<')
            {
                return false;
            }

            do
            {
                depth += (target[t] == '<') - (target[t] == '>');
                t++;
            } while (depth > 0 && t < targetLen);

            if (depth != 0)
            {
                return false;
            }
        }
        else
        {
            if (t >= targetLen || target[t] != *pattern)
            {
                return false;
            }

            pattern++;
            t++;
        }
    }

    return t == targetLen;
}

bool CLR_RT_GenericDispatch::FindVirtualMethodInChain(
    const CLR_RT_TypeDef_Index &cls,
    CLR_RT_MethodDef_Instance &calleeInst,
    const char *calleeName,
    bool suffixMatchOnly,
    bool useNodes,
    const CLR_RT_GenericTarget *calleeBinding,
    CLR_RT_MethodDef_Index &index)
{
    CLR_RT_TypeDef_Instance clsInst{};
    clsInst.InitializeFromIndex(cls);

    const CLR_RECORD_METHODDEF *calleeMDR = calleeInst.target;
    const CLR_UINT8 calleeArgumentsCount = calleeMDR->argumentsCount;
    const size_t calleeLen = hal_strlen_s(calleeName);
    const bool hasHelper =
        cls.data == g_CLR_RT_WellKnownTypes.Array.data && NANOCLR_INDEX_IS_VALID(g_CLR_RT_WellKnownTypes.SZArrayHelper);
    bool helper = false;
    bool haveContext = false;
    bool contextOk = true;
    CLR_RT_GenericTarget context;
    CLR_RT_GenericTarget next;
    char qualifier[256];
    bool qualifierValid = false;

    if (suffixMatchOnly)
    {
        CLR_RT_TypeDef_Instance declaringType{};
        char *szBuffer = qualifier;
        size_t iBuffer = MAXSTRLEN(qualifier);

        if (declaringType.InitializeFromMethod(calleeInst) &&
            SUCCEEDED(g_CLR_RT_TypeSystem.BuildTypeName(
                declaringType,
                szBuffer,
                iBuffer,
                CLR_RT_TypeSystem::TYPENAME_FLAGS_FULL | CLR_RT_TypeSystem::TYPENAME_NESTED_SEPARATOR_DOT,
                0)))
        {
            qualifierValid = qualifier[0] != 0;
        }
    }

    // A generic interface on an array goes to SZArrayHelper, not to the explicit non-generic members of Array.
    if (hasHelper)
    {
        CLR_RT_TypeDef_Instance owner{};

        helper = owner.InitializeFromMethod(calleeInst) && owner.target->genericParamCount > 0 &&
                 owner.target->IsInterface();

        if (helper)
        {
            clsInst.InitializeFromIndex(g_CLR_RT_WellKnownTypes.SZArrayHelper);
        }
    }

    while (NANOCLR_INDEX_IS_VALID(clsInst))
    {
        CLR_RT_Assembly *targetAssm = clsInst.assembly;
        const CLR_RECORD_TYPEDEF *targetTDR = clsInst.target;
        const CLR_RECORD_METHODDEF *targetMDR = targetAssm->GetMethodDef(targetTDR->firstMethod);
        int num = targetTDR->virtualMethodCount + targetTDR->instanceMethodCount;

        for (int i = 0; i < num; i++, targetMDR++)
        {
            if (!helper && targetMDR == calleeMDR)
            {
                index = calleeInst;
                return true;
            }

            if (targetMDR->argumentsCount != calleeArgumentsCount ||
                (targetMDR->flags & (helper ? CLR_RECORD_METHODDEF::MD_Static : CLR_RECORD_METHODDEF::MD_Abstract)) !=
                    0)
            {
                continue;
            }

            const char *targetName = targetAssm->GetString(targetMDR->name);
            size_t targetLen = hal_strlen_s(targetName);
            bool nameMatch = (helper || !suffixMatchOnly) && !strcmp(targetName, calleeName);

            if (!nameMatch && !helper && suffixMatchOnly && qualifierValid && targetLen > calleeLen + 1)
            {
                const char *suffix = targetName + targetLen - calleeLen;

                if (suffix[-1] == '.' && !strcmp(suffix, calleeName))
                {
                    nameMatch = MatchInterfaceQualifier(qualifier, targetName, targetLen - calleeLen - 1);
                }
            }

            if (!nameMatch)
            {
                continue;
            }

            int matched = 1;

            if (!helper)
            {
                if (useNodes)
                {
                    matched = contextOk ? MatchSignatureInClass(
                                              calleeInst,
                                              calleeBinding,
                                              targetAssm,
                                              targetMDR,
                                              haveContext ? &context : nullptr)
                                        : 0;
                }
                else
                {
                    matched = -1;
                }

                if (matched < 0)
                {
                    CLR_RT_SignatureParser parserLeft{};
                    CLR_RT_SignatureParser parserRight{};

                    parserLeft.Initialize_MethodSignature(&calleeInst);
                    parserRight.Initialize_MethodSignature(targetAssm, targetMDR);

                    matched = CLR_RT_TypeSystem::MatchSignature(parserLeft, parserRight) ? 1 : 0;
                }
            }

            if (matched == 1)
            {
                index.Set(targetAssm->assemblyIndex, i + targetTDR->firstMethod);

                return true;
            }
        }

        if (helper)
        {
            break;
        }

        if (useNodes && contextOk)
        {
            contextOk = GetBaseContext(clsInst, haveContext ? &context : nullptr, next);
            context = next;
            haveContext = true;
        }

        if (!clsInst.SwitchToParent() && hasHelper)
        {
            helper = true;
            clsInst.InitializeFromIndex(g_CLR_RT_WellKnownTypes.SZArrayHelper);
        }
    }

    index.Clear();

    return false;
}

bool CLR_RT_GenericDispatch::FindExplicitImplementation(
    const CLR_RT_TypeDef_Index &cls,
    CLR_RT_MethodDef_Instance &calleeInst,
    const CLR_RT_TypeDef_Instance &declaring,
    const char *calleeName,
    CLR_RT_MethodDef_Index &index)
{
    char rgBuffer[512];
    char *szBuffer = rgBuffer;
    size_t iBuffer = MAXSTRLEN(rgBuffer);

    g_CLR_RT_TypeSystem.BuildTypeName(
        declaring,
        szBuffer,
        iBuffer,
        CLR_RT_TypeSystem::TYPENAME_FLAGS_FULL | CLR_RT_TypeSystem::TYPENAME_NESTED_SEPARATOR_DOT,
        0);
    g_CLR_RT_TypeSystem.QueueStringToBuffer(szBuffer, iBuffer, ".");
    g_CLR_RT_TypeSystem.QueueStringToBuffer(szBuffer, iBuffer, calleeName);

    return FindVirtualMethodInChain(cls, calleeInst, rgBuffer, false, false, nullptr, index);
}

bool CLR_RT_TypeSystem::FindVirtualMethodDef(
    const CLR_RT_TypeDef_Index &cls,
    const CLR_RT_MethodDef_Index &calleeMD,
    CLR_RT_MethodDef_Index &index,
    const CLR_RT_TypeSpec_Index *callSite,
    bool *cacheable)
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_MethodDef_Instance calleeInst{};

    if (calleeInst.InitializeFromIndex(calleeMD))
    {
        const char *calleeName = calleeInst.assembly->GetString(calleeInst.target->name);
        CLR_RT_TypeDef_Instance declaring{};
        CLR_RT_GenericTarget binding;
        bool useNodes = false;
        bool ambiguous = false;

        declaring.InitializeFromMethod(calleeInst);

        if (declaring.target->genericParamCount > 0)
        {
            useNodes = CLR_RT_GenericDispatch::GetCalleeBinding(cls, declaring, nullptr, binding, ambiguous) == S_OK;

            if (useNodes && ambiguous && callSite != nullptr && NANOCLR_INDEX_IS_VALID(*callSite))
            {
                const CLR_RT_GenericInstance *site = RecordOfTypeSpec(*callSite);

                if (site != nullptr)
                {
                    useNodes =
                        CLR_RT_GenericDispatch::GetCalleeBinding(cls, declaring, site, binding, ambiguous) == S_OK;
                }
            }
        }

        const CLR_RT_GenericTarget *calleeBinding = (useNodes && binding.m_count > 0) ? &binding : nullptr;

        if ((declaring.target->IsInterface() &&
             ((declaring.target->genericParamCount == 0 &&
               CLR_RT_GenericDispatch::FindExplicitImplementation(cls, calleeInst, declaring, calleeName, index)) ||
              CLR_RT_GenericDispatch::
                  FindVirtualMethodInChain(cls, calleeInst, calleeName, true, useNodes, calleeBinding, index))) ||
            CLR_RT_GenericDispatch::FindVirtualMethodInChain(
                cls,
                calleeInst,
                calleeName,
                false,
                useNodes,
                calleeBinding,
                index))
        {
            if (cacheable != nullptr)
            {
                *cacheable = !ambiguous;
            }

            return true;
        }
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

// See CLAUDE.md "Generic type identity hash" and "Runtime-closed instantiations".
static const int c_MaxGenericNestingDepth = 4;

static bool AppendGenericInstanceNode(
    CLR_RT_GenericInstanceNode *nodes,
    int &nodeCount,
    const CLR_RT_SignatureParser::Element &elem,
    int levels)
{
    if (nodeCount >= CLR_RT_GenericInstance::c_MaxNodes || levels > 0xFF || elem.GenParamCount > 0xFF)
    {
        return false;
    }

    CLR_RT_GenericInstanceNode &node = nodes[nodeCount++];
    bool isParameter = elem.DataType == DATATYPE_VAR || elem.DataType == DATATYPE_MVAR;

    node.Class.data = isParameter ? 0 : elem.Class.data;
    node.DataType = (CLR_UINT8)elem.DataType;
    node.Levels = (CLR_UINT8)levels;
    node.ArgCount = (CLR_UINT8)elem.GenParamCount;
    node.Position = isParameter ? (CLR_UINT8)elem.GenericParamPosition : 0;

    return true;
}

// S_OK, CLR_E_NOT_SUPPORTED over the limits, CLR_E_WRONG_TYPE when a position doesn't resolve.
static HRESULT CollectGenericArguments(
    const CLR_RT_SignatureParser &rootParser,
    const CLR_RT_TypeSpec_Index *contextTypeSpec,
    const CLR_RT_MethodDef_Instance *contextMethod,
    int argCount,
    const CLR_RT_TypeDef_Index *mvarFallback,
    CLR_RT_GenericInstanceNode *nodes,
    int &nodeCount,
    bool &isClosed,
    bool &substituted)
{
    struct SubstitutionFrame
    {
        CLR_RT_SignatureParser Parser;
        int End;
        int OuterLevels;
        bool VarResolved;
        bool MvarResolved;
    };

    // The signature, plus one substitution each for VAR and MVAR.
    SubstitutionFrame frames[3];
    CLR_UINT8 remainingArgs[c_MaxGenericNestingDepth];
    CLR_RT_SignatureParser::Element elem;
    int top = 0;
    int depth = 1;

    frames[0].Parser = rootParser;
    frames[0].End = 0;
    frames[0].OuterLevels = 0;
    frames[0].VarResolved = false;
    frames[0].MvarResolved = false;
    remainingArgs[0] = (CLR_UINT8)argCount;

    while (top >= 0)
    {
        SubstitutionFrame &frame = frames[top];

        if (frame.Parser.Available() <= frame.End)
        {
            top--;
            continue;
        }

        if (FAILED(frame.Parser.Advance(elem)))
        {
            return CLR_E_WRONG_TYPE;
        }

        int levels = frame.OuterLevels + elem.Levels;
        frame.OuterLevels = 0;

        if (elem.DataType == DATATYPE_MVAR && frame.MvarResolved)
        {
            return CLR_E_WRONG_TYPE;
        }

        bool substituteVar = elem.DataType == DATATYPE_VAR && !frame.VarResolved && contextTypeSpec &&
                             NANOCLR_INDEX_IS_VALID(*contextTypeSpec);
        bool substituteMvar = elem.DataType == DATATYPE_MVAR && !frame.MvarResolved && contextMethod &&
                              NANOCLR_INDEX_IS_VALID(contextMethod->methodSpec);

        if (substituteVar || substituteMvar)
        {
            SubstitutionFrame &next = frames[top + 1];

            if (substituteVar)
            {
                CLR_RT_TypeSpec_Instance contextTs;

                if (!contextTs.InitializeFromIndex(*contextTypeSpec) ||
                    !PositionAtGenericParam(contextTs, elem.GenericParamPosition, next.Parser))
                {
                    return CLR_E_WRONG_TYPE;
                }
            }
            else
            {
                CLR_RT_MethodSpec_Instance methodSpecInst{};

                if (!methodSpecInst.InitializeFromIndex(contextMethod->methodSpec) ||
                    !PositionAtGenericArgument(methodSpecInst, elem.GenericParamPosition, next.Parser))
                {
                    return CLR_E_WRONG_TYPE;
                }
            }

            next.End = next.Parser.Available() - 1;
            next.OuterLevels = levels;
            next.VarResolved = frame.VarResolved || substituteVar;
            next.MvarResolved = frame.MvarResolved || substituteMvar;
            substituted = true;
            top++;
            continue;
        }

        // Runtime-inferred element type, only for a frame with no MethodSpec. See CLAUDE.md §5.
        if (elem.DataType == DATATYPE_MVAR && !frame.MvarResolved && mvarFallback != nullptr &&
            NANOCLR_INDEX_IS_VALID(*mvarFallback) && elem.GenericParamPosition == 0)
        {
            CLR_RT_TypeDef_Instance fallback{};

            if (!fallback.InitializeFromIndex(*mvarFallback))
            {
                return CLR_E_WRONG_TYPE;
            }

            // An enum is a VALUETYPE in signatures, while its TypeDef reports the underlying type.
            bool isEnum = (fallback.target->flags & CLR_RECORD_TYPEDEF::TD_Semantics_Mask) ==
                          CLR_RECORD_TYPEDEF::TD_Semantics_Enum;

            elem.DataType = mvarFallback->data == g_CLR_RT_WellKnownTypes.Object.data
                                ? DATATYPE_OBJECT
                                : (isEnum ? DATATYPE_VALUETYPE : (NanoCLRDataType)fallback.target->dataType);
            elem.Class = *mvarFallback;
            elem.GenParamCount = 0;
            substituted = true;
        }

        if (elem.DataType == DATATYPE_VAR || elem.DataType == DATATYPE_MVAR)
        {
            isClosed = false;
        }

        if (elem.DataType == DATATYPE_GENERICINST)
        {
            if (FAILED(frame.Parser.Advance(elem)) || !NANOCLR_INDEX_IS_VALID(elem.Class))
            {
                return CLR_E_WRONG_TYPE;
            }
        }
        else if (
            (elem.DataType == DATATYPE_CLASS || elem.DataType == DATATYPE_VALUETYPE) &&
            !NANOCLR_INDEX_IS_VALID(elem.Class))
        {
            return CLR_E_WRONG_TYPE;
        }

        if (!AppendGenericInstanceNode(nodes, nodeCount, elem, levels))
        {
            return CLR_E_NOT_SUPPORTED;
        }

        if (elem.GenParamCount > 0)
        {
            if (depth == c_MaxGenericNestingDepth)
            {
                return CLR_E_NOT_SUPPORTED;
            }

            remainingArgs[depth++] = (CLR_UINT8)elem.GenParamCount;
            continue;
        }

        while (depth > 0 && --remainingArgs[depth - 1] == 0)
        {
            depth--;
        }

        // The parser may continue with the next parameter of a method signature.
        if (depth == 0)
        {
            break;
        }
    }

    return S_OK;
}

// The parser is positioned at the GENERICINST marker.
static HRESULT CollectGenericNodesAt(
    CLR_RT_SignatureParser parser,
    const CLR_RT_TypeSpec_Index *contextTypeSpec,
    const CLR_RT_MethodDef_Instance *contextMethod,
    const CLR_RT_TypeDef_Index *mvarFallback,
    CLR_RT_GenericInstanceNode *nodes,
    int &nodeCount,
    bool &isClosed,
    bool &substituted)
{
    CLR_RT_SignatureParser::Element elem;

    nodeCount = 0;
    isClosed = true;
    substituted = false;

    if (FAILED(parser.Advance(elem)) || elem.DataType != DATATYPE_GENERICINST || FAILED(parser.Advance(elem)) ||
        !NANOCLR_INDEX_IS_VALID(elem.Class) || elem.GenParamCount <= 0)
    {
        return CLR_E_WRONG_TYPE;
    }

    // The outermost levels belong to an array of the type, not to the type.
    if (!AppendGenericInstanceNode(nodes, nodeCount, elem, 0))
    {
        return CLR_E_NOT_SUPPORTED;
    }

    return CollectGenericArguments(
        parser,
        contextTypeSpec,
        contextMethod,
        elem.GenParamCount,
        mvarFallback,
        nodes,
        nodeCount,
        isClosed,
        substituted);
}

static HRESULT CollectGenericInstanceNodes(
    CLR_RT_TypeSpec_Instance &typeInstance,
    const CLR_RT_TypeSpec_Index *contextTypeSpec,
    const CLR_RT_MethodDef_Instance *contextMethod,
    const CLR_RT_TypeDef_Index *mvarFallback,
    CLR_RT_GenericInstanceNode *nodes,
    int &nodeCount,
    bool &isClosed,
    bool &substituted)
{
    CLR_RT_SignatureParser parser;

    parser.Initialize_TypeSpec(typeInstance);

    return CollectGenericNodesAt(
        parser,
        contextTypeSpec,
        contextMethod,
        mvarFallback,
        nodes,
        nodeCount,
        isClosed,
        substituted);
}

static bool SameGenericInstanceNodes(
    const CLR_RT_GenericInstanceNode *left,
    const CLR_RT_GenericInstanceNode *right,
    int nodeCount)
{
    return memcmp(left, right, sizeof(CLR_RT_GenericInstanceNode) * nodeCount) == 0;
}

// A closed row of the same instantiation, so objects created at runtime get the same TypeSpec as typeof().
static bool FindClosedTypeSpecRow(const CLR_RT_GenericInstanceNode *nodes, int nodeCount, CLR_RT_TypeSpec_Index &row)
{
    CLR_RT_GenericInstanceNode candidateNodes[CLR_RT_GenericInstance::c_MaxNodes];

    NANOCLR_FOREACH_ASSEMBLY(g_CLR_RT_TypeSystem)
    {
        for (int i = 0; i < pASSM->tablesSize[TBL_TypeSpec]; i++)
        {
            CLR_RT_TypeSpec_Instance candidate{};
            CLR_RT_TypeSpec_Index candidateIndex;
            int candidateCount;
            bool closed;
            bool substituted;

            if (pASSM->crossReferenceTypeSpec[i].genericInstance != nullptr)
            {
                continue;
            }

            candidateIndex.Set(pASSM->assemblyIndex, i);

            if (!candidate.InitializeFromIndex(candidateIndex) || candidate.levels != 0 ||
                candidate.genericTypeDef.data != nodes[0].Class.data)
            {
                continue;
            }

            if (SUCCEEDED(CollectGenericInstanceNodes(
                    candidate,
                    nullptr,
                    nullptr,
                    nullptr,
                    candidateNodes,
                    candidateCount,
                    closed,
                    substituted)) &&
                closed && candidateCount == nodeCount && SameGenericInstanceNodes(nodes, candidateNodes, nodeCount))
            {
                row = candidateIndex;
                return true;
            }
        }
    }
    NANOCLR_FOREACH_ASSEMBLY_END();

    return false;
}

CLR_RT_GenericInstance *CLR_RT_TypeSystem::GetGenericInstance(const CLR_RT_TypeSpec_Index &index)
{
    CLR_UINT32 slot = index.TypeSpec();

    if (!index.IsRuntime() || slot >= g_CLR_RT_TypeSystem.m_genericInstancesCount)
    {
        return nullptr;
    }

    return g_CLR_RT_TypeSystem.m_genericInstances[slot];
}

CLR_UINT16 CLR_RT_TypeSystem::GetGenericOwnerSlot(const CLR_RT_TypeSpec_Index &typeSpec)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_TypeSpec_Instance inst{};
    CLR_RT_GenericInstance *record;

    if (!NANOCLR_INDEX_IS_VALID(typeSpec))
    {
        return 0;
    }

    if (IsOpenInstance(typeSpec))
    {
        record = GetGenericInstance(typeSpec);
    }
    else if (!inst.InitializeFromIndex(typeSpec) || inst.levels != 0)
    {
        return 0;
    }
    else
    {
        record = FindOrCreateGenericInstance(inst);
    }

    if (record == nullptr)
    {
        return 0;
    }

    for (CLR_UINT32 i = 0; i < g_CLR_RT_TypeSystem.m_genericInstancesCount; i++)
    {
        if (g_CLR_RT_TypeSystem.m_genericInstances[i] == record)
        {
            return (CLR_UINT16)(i + 1);
        }
    }

    return 0;
}

bool CLR_RT_TypeSystem::GetGenericOwnerRow(
    CLR_UINT16 slot,
    const CLR_RT_TypeDef_Index &declaring,
    CLR_RT_TypeSpec_Index &row)
{
    NATIVE_PROFILE_CLR_CORE();

    const CLR_RT_GenericInstance *record = GetMemberRecord(slot, false);

    if (record == nullptr || record->m_nodes[0].Class.data != declaring.data)
    {
        return false;
    }

    row = record->m_self;

    return true;
}

const CLR_RT_GenericInstance *CLR_RT_TypeSystem::GetMemberRecord(CLR_UINT16 slot, bool method)
{
    const CLR_RT_TypeSystem &ts = g_CLR_RT_TypeSystem;

    for (int hop = 0; hop < 2; hop++)
    {
        if (slot == 0 || slot > ts.m_genericInstancesCount)
        {
            return nullptr;
        }

        const CLR_RT_GenericInstance *record = ts.m_genericInstances[slot - 1];
        const CLR_RT_GenericInstanceNode &root = record->m_nodes[0];
        bool isMethod = root.Class.data == 0 && root.DataType == DATATYPE_VOID;

        if (isMethod == method)
        {
            return record;
        }

        if (!isMethod)
        {
            return nullptr;
        }

        slot = (CLR_UINT16)(root.Levels | (root.Position << 8));
    }

    return nullptr;
}

CLR_RT_GenericInstance *CLR_RT_TypeSystem::FindOrCreateGenericBase(
    const CLR_RT_TypeDef_Instance &child,
    const CLR_RT_TypeSpec_Index *childContext)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_TypeSpec_Index baseSpec;
    CLR_RT_TypeSpec_Instance baseInstance{};

    if (!child.GetExtendsTypeSpec(baseSpec) || !baseInstance.InitializeFromIndex(baseSpec))
    {
        return nullptr;
    }

    return FindOrCreateGenericInstance(
        baseInstance,
        (childContext != nullptr && NANOCLR_INDEX_IS_VALID(*childContext)) ? childContext : nullptr);
}

// The size of an integral primitive or of the underlying type of an enum, 0 for anything else.
int CLR_RT_TypeSystem::IntegralSize(CLR_UINT32 dataType)
{
    switch (dataType)
    {
        case DATATYPE_I1:
        case DATATYPE_U1:
            return 1;
        case DATATYPE_I2:
        case DATATYPE_U2:
            return 2;
        case DATATYPE_I4:
        case DATATYPE_U4:
            return 4;
        case DATATYPE_I8:
        case DATATYPE_U8:
            return 8;
        default:
            return 0;
    }
}

bool CLR_RT_TypeSystem::IsSameGenericInstance(const CLR_RT_TypeSpec_Index &left, const CLR_RT_TypeSpec_Index &right)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_TypeSpec_Instance leftInst{};
    CLR_RT_TypeSpec_Instance rightInst{};
    CLR_RT_GenericInstance *leftRecord;

    if (left.data == right.data)
    {
        return true;
    }

    if (!leftInst.InitializeFromIndex(left) || !rightInst.InitializeFromIndex(right))
    {
        return false;
    }

    leftRecord = FindOrCreateGenericInstance(leftInst);

    return leftRecord != nullptr && leftRecord == FindOrCreateGenericInstance(rightInst);
}

// The registry lookup shared by every way of reaching an instantiation. A closed node stream is found or created here.
static CLR_RT_GenericInstance *RegisterGenericInstance(
    const CLR_RT_GenericInstanceNode *nodes,
    int nodeCount,
    const CLR_RT_TypeSpec_Index &knownRow,
    const CLR_RT_TypeSpec_Index &origin,
    HRESULT &hr)
{
    CLR_RT_TypeSystem &ts = g_CLR_RT_TypeSystem;
    CLR_UINT32 hash = SUPPORT_ComputeCRC(nodes, (int)(sizeof(CLR_RT_GenericInstanceNode) * nodeCount), 0);

    hr = S_OK;

    for (CLR_UINT32 i = 0; i < ts.m_genericInstancesCount; i++)
    {
        CLR_RT_GenericInstance *candidate = ts.m_genericInstances[i];

        if (candidate->m_hash == hash && candidate->m_nodeCount == nodeCount &&
            SameGenericInstanceNodes(candidate->m_nodes, nodes, nodeCount))
        {
            return candidate;
        }
    }

    if (ts.m_genericInstancesCount >= 0xFFFF)
    {
        hr = CLR_E_OUT_OF_MEMORY;
        return nullptr;
    }

    if (ts.m_genericInstancesCount >= ts.m_genericInstancesMaxCount)
    {
        CLR_UINT32 newMax = ts.m_genericInstancesMaxCount == 0 ? 8 : ts.m_genericInstancesMaxCount * 2;
        CLR_RT_GenericInstance **newArray =
            (CLR_RT_GenericInstance **)platform_malloc(sizeof(CLR_RT_GenericInstance *) * newMax);

        if (newArray == nullptr)
        {
            hr = CLR_E_OUT_OF_MEMORY;
            return nullptr;
        }

        if (ts.m_genericInstances != nullptr)
        {
            memcpy(newArray, ts.m_genericInstances, sizeof(CLR_RT_GenericInstance *) * ts.m_genericInstancesCount);
            platform_free(ts.m_genericInstances);
        }

        ts.m_genericInstances = newArray;
        ts.m_genericInstancesMaxCount = newMax;
    }

    CLR_RT_GenericInstance *instance = (CLR_RT_GenericInstance *)platform_malloc(
        offsetof(CLR_RT_GenericInstance, m_nodes) + sizeof(CLR_RT_GenericInstanceNode) * nodeCount);

    if (instance == nullptr)
    {
        hr = CLR_E_OUT_OF_MEMORY;
        return nullptr;
    }

    instance->m_hash = hash;
    instance->m_self.Clear();
    instance->m_origin.data = origin.data;
    instance->m_fields = nullptr;
    instance->m_fieldDefs = nullptr;
    instance->m_count = 0;
    instance->m_flags = 0;
    instance->m_nodeCount = (CLR_UINT8)nodeCount;
    memcpy(instance->m_nodes, nodes, sizeof(CLR_RT_GenericInstanceNode) * nodeCount);

    if (NANOCLR_INDEX_IS_VALID(knownRow))
    {
        instance->m_self.data = knownRow.data;
    }
    else if (nodes[0].Class.data == 0 || !FindClosedTypeSpecRow(nodes, nodeCount, instance->m_self))
    {
        instance->m_self.Set(CLR_RT_TypeSpec_Index::c_RuntimeAssembly, ts.m_genericInstancesCount);
    }

    ts.m_genericInstances[ts.m_genericInstancesCount++] = instance;

    if (!instance->m_self.IsRuntime())
    {
        CLR_RT_Assembly *selfAssembly = ts.m_assemblies[instance->m_self.Assembly() - 1];
        selfAssembly->crossReferenceTypeSpec[instance->m_self.TypeSpec()].genericInstance = instance;
    }

    return instance;
}

CLR_RT_GenericInstance *CLR_RT_TypeSystem::FindOrCreateGenericInstance(
    CLR_RT_TypeSpec_Instance &typeInstance,
    const CLR_RT_TypeSpec_Index *contextTypeSpec,
    const CLR_RT_MethodDef_Instance *contextMethod,
    HRESULT *result,
    const CLR_RT_TypeDef_Index *mvarFallback)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_GenericInstanceNode nodes[CLR_RT_GenericInstance::c_MaxNodes];
    CLR_RT_TypeSpec_CrossReference *row = nullptr;
    CLR_RT_TypeSpec_Index knownRow;
    CLR_RT_GenericInstance *instance = nullptr;
    HRESULT hr;
    int nodeCount;
    bool closed;
    bool substituted;

    knownRow.Clear();

    if (typeInstance.IsRuntime())
    {
        instance = GetGenericInstance(typeInstance);
        hr = instance != nullptr ? S_OK : CLR_E_WRONG_TYPE;
        goto Done;
    }

    if (NANOCLR_INDEX_IS_VALID(typeInstance) && typeInstance.assembly != nullptr)
    {
        row = &typeInstance.assembly->crossReferenceTypeSpec[typeInstance.TypeSpec()];

        if (row->genericInstance != nullptr)
        {
            instance = row->genericInstance;
            hr = S_OK;
            goto Done;
        }
    }

    hr = CollectGenericInstanceNodes(
        typeInstance,
        contextTypeSpec,
        contextMethod,
        mvarFallback,
        nodes,
        nodeCount,
        closed,
        substituted);

    if (FAILED(hr))
    {
        goto Done;
    }

    // Still open after resolution: there's no instantiation yet, and nothing may use storage for it.
    if (!closed)
    {
        hr = S_FALSE;
        goto Done;
    }

    if (!substituted && row != nullptr && typeInstance.levels == 0)
    {
        knownRow.data = typeInstance.data;
    }

    instance = RegisterGenericInstance(nodes, nodeCount, knownRow, typeInstance, hr);

    if (instance != nullptr && row != nullptr && !substituted)
    {
        row->genericInstance = instance;
    }

Done:
    if (result != nullptr)
    {
        *result = hr;
    }

    return SUCCEEDED(hr) ? instance : nullptr;
}

CLR_RT_GenericInstance *CLR_RT_TypeSystem::FindOrCreateGenericInstanceByNodes(
    const CLR_RT_GenericInstanceNode *nodes,
    int nodeCount,
    HRESULT *result)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_TypeSpec_Index none;
    HRESULT hr = S_OK;

    none.Clear();

    for (int i = 0; i < nodeCount; i++)
    {
        if (nodes[i].DataType == DATATYPE_VAR || nodes[i].DataType == DATATYPE_MVAR)
        {
            hr = CLR_E_WRONG_TYPE;
        }
    }

    CLR_RT_GenericInstance *instance =
        SUCCEEDED(hr) ? RegisterGenericInstance(nodes, nodeCount, none, none, hr) : nullptr;

    if (result != nullptr)
    {
        *result = hr;
    }

    return instance;
}

#if CONFIG_NF_FEATURE_SUPPORT_REFLECTION

static bool GetArgumentRange(const CLR_RT_GenericInstance &record, int argument, int &first, int &end)
{
    int position = 1;

    if (argument >= record.m_nodes[0].ArgCount)
    {
        return false;
    }

    for (int arg = 0; arg <= argument; arg++)
    {
        int remaining = 1;

        first = position;

        while (remaining > 0)
        {
            if (position >= record.m_nodeCount)
            {
                return false;
            }

            remaining += record.m_nodes[position].ArgCount - 1;
            position++;
        }
    }

    end = position;

    return true;
}

HRESULT CLR_RT_TypeSystem::GetClosedGenericBase(
    const CLR_RT_TypeDef_Instance &child,
    const CLR_RT_TypeSpec_Index *childContext,
    CLR_RT_TypeSpec_Index &row)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_TypeSpec_Index baseSpec;
    CLR_RT_TypeSpec_Instance baseInstance{};
    CLR_RT_GenericInstance *record;
    HRESULT hr;

    if (!child.GetExtendsTypeSpec(baseSpec) || !baseInstance.InitializeFromIndex(baseSpec))
    {
        return S_FALSE;
    }

    record = FindOrCreateGenericInstance(
        baseInstance,
        (childContext != nullptr && NANOCLR_INDEX_IS_VALID(*childContext)) ? childContext : nullptr,
        nullptr,
        &hr);

    if (record == nullptr)
    {
        return hr;
    }

    row = (baseInstance.assembly->crossReferenceTypeSpec[baseSpec.TypeSpec()].genericInstance == record)
              ? baseSpec
              : record->m_self;

    return S_OK;
}

HRESULT CLR_RT_TypeSystem::GetClosedEnclosingType(
    const CLR_RT_TypeSpec_Index &nested,
    CLR_UINT32 enclosingArgCount,
    const CLR_RT_TypeDef_Index &enclosing,
    CLR_RT_TypeSpec_Index &row)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_TypeSpec_Instance nestedInstance{};
    CLR_RT_GenericInstance *record;
    CLR_RT_GenericInstanceNode nodes[CLR_RT_GenericInstance::c_MaxNodes];
    int count = 1;
    HRESULT hr;

    if (!nestedInstance.InitializeFromIndex(nested) || nestedInstance.levels != 0)
    {
        return CLR_E_WRONG_TYPE;
    }

    record = FindOrCreateGenericInstance(nestedInstance, nullptr, nullptr, &hr);

    if (record == nullptr)
    {
        return hr;
    }

    if (enclosingArgCount == 0 || enclosingArgCount > record->m_nodes[0].ArgCount)
    {
        return S_FALSE;
    }

    CLR_RT_TypeDef_Instance enclosingInstance{};

    if (!enclosingInstance.InitializeFromIndex(enclosing))
    {
        return CLR_E_WRONG_TYPE;
    }

    nodes[0] = record->m_nodes[0];
    nodes[0].Class = enclosing;
    nodes[0].DataType = enclosingInstance.target->dataType;
    nodes[0].ArgCount = (CLR_UINT8)enclosingArgCount;

    for (CLR_UINT32 arg = 0; arg < enclosingArgCount; arg++)
    {
        int first;
        int end;

        if (!GetArgumentRange(*record, (int)arg, first, end) ||
            count + (end - first) > CLR_RT_GenericInstance::c_MaxNodes)
        {
            return CLR_E_NOT_SUPPORTED;
        }

        memcpy(&nodes[count], &record->m_nodes[first], sizeof(CLR_RT_GenericInstanceNode) * (end - first));
        count += end - first;
    }

    record = FindOrCreateGenericInstanceByNodes(nodes, count, &hr);

    if (record == nullptr)
    {
        return hr;
    }

    row = record->m_self;

    return S_OK;
}

#endif // CONFIG_NF_FEATURE_SUPPORT_REFLECTION

bool CLR_RT_TypeSystem::CloseMethodSpec(
    const CLR_RT_MethodDef_Instance &caller,
    CLR_RT_MethodSpec_Index &methodSpec,
    bool registerAlways)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_MethodSpec_Instance spec{};
    CLR_RT_SignatureParser parser;
    CLR_RT_GenericInstanceNode nodes[CLR_RT_GenericInstance::c_MaxNodes];
    int nodeCount = 1;
    bool closed = true;
    bool substituted = false;

    if (!NANOCLR_INDEX_IS_VALID(methodSpec) || methodSpec.IsRuntime() || !spec.InitializeFromIndex(methodSpec))
    {
        return false;
    }

    parser.Initialize_MethodSignature(&spec);

    int argCount = parser.ParamCount;

    if (argCount <= 0 || argCount > 0xFF)
    {
        return false;
    }

    nodes[0].Class.Clear();
    nodes[0].DataType = DATATYPE_VOID;
    nodes[0].Levels = 0;
    nodes[0].ArgCount = (CLR_UINT8)argCount;
    nodes[0].Position = 0;

    if (FAILED(CollectGenericArguments(
            parser,
            caller.genericType,
            &caller,
            argCount,
            nullptr,
            nodes,
            nodeCount,
            closed,
            substituted)) ||
        !closed)
    {
        // a MethodSpec that is kept for later must not be left open
        if (registerAlways)
        {
            methodSpec.Clear();
        }

        return false;
    }

    if (!substituted && !registerAlways)
    {
        return false;
    }

    CLR_RT_GenericInstance *instance = FindOrCreateGenericInstanceByNodes(nodes, nodeCount);

    if (instance == nullptr)
    {
        return false;
    }

    methodSpec.data = instance->m_self.data;

    return true;
}

static CLR_RT_GenericInstance *RecordOfTypeSpec(const CLR_RT_TypeSpec_Index &index);

// The data type a signature element of this class has, so the node compares equal to one parsed from metadata.
static NanoCLRDataType SignatureDataTypeOfClass(const CLR_RT_TypeDef_Index &cls)
{
    CLR_RT_TypeDef_Instance inst{};

    if (!inst.InitializeFromIndex(cls))
    {
        return DATATYPE_VOID;
    }

    if (cls.data == g_CLR_RT_WellKnownTypes.Object.data)
    {
        return DATATYPE_OBJECT;
    }

    NanoCLRDataType dt = (NanoCLRDataType)inst.target->dataType;
    const CLR_RT_TypeDef_Index *primitive = c_CLR_RT_DataTypeLookup[dt].m_cls;

    if (((dt >= DATATYPE_BOOLEAN && dt <= DATATYPE_R8) || dt == DATATYPE_STRING) && primitive != nullptr &&
        primitive->data == cls.data)
    {
        return dt;
    }

    CLR_UINT32 semantics = inst.target->flags & CLR_RECORD_TYPEDEF::TD_Semantics_Mask;

    return (semantics == CLR_RECORD_TYPEDEF::TD_Semantics_ValueType ||
            semantics == CLR_RECORD_TYPEDEF::TD_Semantics_Enum)
               ? DATATYPE_VALUETYPE
               : DATATYPE_CLASS;
}

static const struct
{
    const char *assembly;
    const char *name;
} c_ArrayInterfaceDefinitions[] = {
    {"mscorlib", "IEnumerable`1"},
    {"mscorlib", "ICollection`1"},
    {"nanoFramework.System.Collections", "IList`1"},
    {"nanoFramework.System.Collections", "IReadOnlyCollection`1"},
    {"nanoFramework.System.Collections", "IReadOnlyList`1"}};

static const char c_ArrayInterfaceNamespace[] = "System.Collections.Generic";

int CLR_RT_TypeSystem::GetArrayInterfaceCount()
{
    return ARRAYSIZE(c_ArrayInterfaceDefinitions);
}

bool CLR_RT_TypeSystem::GetArrayInterfaceDefinition(int index, CLR_RT_TypeDef_Index &definition)
{
    definition.Clear();

    NANOCLR_FOREACH_ASSEMBLY(g_CLR_RT_TypeSystem)
    {
        if (strcmp(pASSM->name, c_ArrayInterfaceDefinitions[index].assembly) == 0)
        {
            return pASSM->FindTypeDef(c_ArrayInterfaceDefinitions[index].name, c_ArrayInterfaceNamespace, definition);
        }
    }
    NANOCLR_FOREACH_ASSEMBLY_END();

    return false;
}

bool CLR_RT_TypeSystem::IsArrayGenericInterface(const CLR_RT_TypeDef_Instance &definition)
{
    if (strcmp(definition.assembly->GetString(definition.target->nameSpace), c_ArrayInterfaceNamespace) != 0)
    {
        return false;
    }

    const char *name = definition.assembly->GetString(definition.target->name);

    for (const auto &candidate : c_ArrayInterfaceDefinitions)
    {
        if (strcmp(name, candidate.name) == 0 && strcmp(definition.assembly->name, candidate.assembly) == 0)
        {
            return true;
        }
    }

    return false;
}

// The array element as the only argument of a closed instance: the nodes from 1, node 0 is left to the caller.
static bool BuildArrayElementNodes(
    const CLR_RT_TypeDef_Index &innermost,
    const CLR_RT_TypeSpec_Index *elementSpec,
    CLR_UINT32 levels,
    CLR_RT_GenericInstanceNode *nodes,
    int &nodeCount)
{
    nodeCount = 2;

    if (levels == 0 || levels > 0x100)
    {
        return false;
    }

    if (elementSpec != nullptr)
    {
        CLR_RT_GenericInstance *element = RecordOfTypeSpec(*elementSpec);

        if (element == nullptr || (element->m_flags & CLR_RT_GenericInstance::c_Open) != 0 ||
            element->m_nodeCount + 1 > CLR_RT_GenericInstance::c_MaxNodes)
        {
            return false;
        }

        memcpy(&nodes[1], element->m_nodes, sizeof(CLR_RT_GenericInstanceNode) * element->m_nodeCount);
        nodeCount = 1 + element->m_nodeCount;
    }
    else
    {
        NanoCLRDataType dt = SignatureDataTypeOfClass(innermost);

        if (dt == DATATYPE_VOID)
        {
            return false;
        }

        nodes[1].Class = innermost;
        nodes[1].DataType = (CLR_UINT8)dt;
        nodes[1].ArgCount = 0;
        nodes[1].Position = 0;
    }

    nodes[1].Levels = (CLR_UINT8)(levels - 1);

    return true;
}

// A runtime MethodSpec with one argument, the element of the array (levels - 1 levels).
bool CLR_RT_TypeSystem::GetArrayElementMethodSpec(
    const CLR_RT_HeapBlock_Array &array,
    CLR_RT_MethodSpec_Index &methodSpec)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_GenericInstanceNode nodes[CLR_RT_GenericInstance::c_MaxNodes];
    const CLR_RT_ReflectionDef_Index &reflex = array.ReflectionDataConst();
    int nodeCount;

    methodSpec.Clear();

    if (reflex.kind != REFLECTION_TYPE ||
        !BuildArrayElementNodes(reflex.data.type, array.ElementTypeSpec(), reflex.levels, nodes, nodeCount))
    {
        return false;
    }

    nodes[0].Class.Clear();
    nodes[0].DataType = DATATYPE_VOID;
    nodes[0].Levels = 0;
    nodes[0].ArgCount = 1;
    nodes[0].Position = 0;

    CLR_RT_GenericInstance *instance = FindOrCreateGenericInstanceByNodes(nodes, nodeCount);

    if (instance == nullptr)
    {
        return false;
    }

    methodSpec.data = instance->m_self.data;

    return true;
}

#if CONFIG_NF_FEATURE_SUPPORT_REFLECTION

static bool GetTypeSpecNodes(
    const CLR_RT_TypeSpec_Index &row,
    CLR_RT_GenericInstanceNode *buffer,
    const CLR_RT_GenericInstanceNode *&nodes,
    int &count,
    CLR_UINT32 &hash)
{
    CLR_RT_TypeSpec_Instance inst{};
    CLR_RT_GenericInstance *record;
    bool closed;
    bool substituted;

    if (!inst.InitializeFromIndex(row) || inst.levels != 0)
    {
        return false;
    }

    record = row.IsRuntime() ? CLR_RT_TypeSystem::GetGenericInstance(row)
                             : inst.assembly->crossReferenceTypeSpec[row.TypeSpec()].genericInstance;

    if (record != nullptr)
    {
        nodes = record->m_nodes;
        count = record->m_nodeCount;
        hash = record->m_hash;

        return true;
    }

    if (FAILED(CollectGenericInstanceNodes(inst, nullptr, nullptr, nullptr, buffer, count, closed, substituted)) ||
        !closed)
    {
        return false;
    }

    nodes = buffer;
    hash = SUPPORT_ComputeCRC(buffer, (int)(sizeof(CLR_RT_GenericInstanceNode) * count), 0);

    return true;
}

bool CLR_RT_TypeSystem::IsSameTypeSpecIdentity(const CLR_RT_TypeSpec_Index &left, const CLR_RT_TypeSpec_Index &right)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_GenericInstanceNode leftBuffer[CLR_RT_GenericInstance::c_MaxNodes];
    CLR_RT_GenericInstanceNode rightBuffer[CLR_RT_GenericInstance::c_MaxNodes];
    const CLR_RT_GenericInstanceNode *leftNodes;
    const CLR_RT_GenericInstanceNode *rightNodes;
    int leftCount;
    int rightCount;
    CLR_UINT32 leftHash;
    CLR_UINT32 rightHash;

    if (left.data == right.data)
    {
        return true;
    }

    return GetTypeSpecNodes(left, leftBuffer, leftNodes, leftCount, leftHash) &&
           GetTypeSpecNodes(right, rightBuffer, rightNodes, rightCount, rightHash) && leftHash == rightHash &&
           leftCount == rightCount && SameGenericInstanceNodes(leftNodes, rightNodes, leftCount);
}

bool CLR_RT_TypeSystem::GetTypeSpecIdentityHash(const CLR_RT_TypeSpec_Index &row, CLR_UINT32 &hash)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_GenericInstanceNode buffer[CLR_RT_GenericInstance::c_MaxNodes];
    const CLR_RT_GenericInstanceNode *nodes;
    int count;

    return GetTypeSpecNodes(row, buffer, nodes, count, hash);
}

bool CLR_RT_TypeSystem::IsOpenInstance(const CLR_RT_TypeSpec_Index &row)
{
    const CLR_RT_GenericInstance *record = row.IsRuntime() ? GetGenericInstance(row) : nullptr;

    return record != nullptr && (record->m_flags & CLR_RT_GenericInstance::c_Open) != 0;
}

void CLR_RT_TypeSystem::SetGenericParamWord(
    CLR_RT_ReflectionDef_Index &word,
    CLR_UINT32 assembly,
    CLR_UINT32 row,
    CLR_UINT32 levels)
{
    word.Clear();
    word.kind = REFLECTION_GENERICPARAM;
    word.levels = (CLR_UINT16)levels;
    word.data.genericParam.Set(assembly, row);
}

bool CLR_RT_TypeSystem::GetGenericParam(
    const CLR_RT_ReflectionDef_Index &word,
    CLR_RT_Assembly *&assembly,
    const CLR_RECORD_GENERICPARAM *&record)
{
    CLR_UINT32 index = word.data.genericParam.Assembly();

    if (word.kind != REFLECTION_GENERICPARAM || index == 0 || index > g_CLR_RT_TypeSystem.m_assembliesMax)
    {
        return false;
    }

    assembly = g_CLR_RT_TypeSystem.m_assemblies[index - 1];

    if (assembly == nullptr || word.data.genericParam.Type() >= assembly->tablesSize[TBL_GenericParam])
    {
        return false;
    }

    record = assembly->GetGenericParam(word.data.genericParam.Type());

    return true;
}

HRESULT CLR_RT_TypeSystem::GetGenericParamOwner(
    const CLR_RT_ReflectionDef_Index &word,
    bool &isMethod,
    CLR_RT_TypeDef_Index &type,
    CLR_RT_MethodDef_Index &method)
{
    CLR_RT_Assembly *assembly;
    const CLR_RECORD_GENERICPARAM *record;

    if (!GetGenericParam(word, assembly, record))
    {
        return CLR_E_WRONG_TYPE;
    }

    isMethod = (record->owner & 0x8000) != 0;

    if (isMethod)
    {
        CLR_RT_MethodDef_Instance owner{};

        method.Set(word.data.genericParam.Assembly(), record->owner & 0x7FFF);

        if (!owner.InitializeFromIndex(method))
        {
            return CLR_E_WRONG_TYPE;
        }

        type.Set(word.data.genericParam.Assembly(), owner.CrossReference().GetOwner());
    }
    else
    {
        type.Set(word.data.genericParam.Assembly(), record->owner & 0x7FFF);
    }

    return S_OK;
}

HRESULT CLR_RT_TypeSystem::GetGenericParamConstraintWord(
    const CLR_RT_ReflectionDef_Index &param,
    int index,
    CLR_RT_ReflectionDef_Index &constraint)
{
    CLR_RT_Assembly *assembly;
    const CLR_RECORD_GENERICPARAM *record;
    CLR_RT_SignatureParser parser{};
    CLR_RT_TypeDef_Index declaring;
    CLR_RT_MethodDef_Index method;
    CLR_PMETADATA signature;
    bool isMethod;
    HRESULT hr;

    constraint.Clear();

    if (!GetGenericParam(param, assembly, record))
    {
        return CLR_E_WRONG_TYPE;
    }

    if (record->signature == CLR_EmptyIndex)
    {
        return S_FALSE;
    }

    signature = assembly->GetSignature(record->signature);

    if (index >= signature[0])
    {
        return S_FALSE;
    }

    hr = GetGenericParamOwner(param, isMethod, declaring, method);

    if (FAILED(hr))
    {
        return hr;
    }

    parser.Initialize_TypeSpec(assembly, signature + 1);
    parser.ParamCount = signature[0];

    for (int i = 0; i <= index; i++)
    {
        CLR_RT_TypeDescriptor desc{};
        CLR_RT_TypeSpec_Index element;

        hr = desc.InitializeFromClosedSignature(parser, nullptr, &element, &declaring, isMethod ? &method : nullptr);

        if (FAILED(hr))
        {
            return hr;
        }

        constraint = desc.m_reflex;
    }

    return S_OK;
}

static const int c_MaxParamConstraintChain = 4;

// The parameter and the parameters it is constrained by, directly or not.
static HRESULT GetGenericParamClosure(
    const CLR_RT_ReflectionDef_Index &param,
    CLR_RT_ReflectionDef_Index *list,
    int &count)
{
    list[0] = param;
    count = 1;

    for (int next = 0; next < count; next++)
    {
        for (int i = 0;; i++)
        {
            CLR_RT_ReflectionDef_Index constraint;
            bool known = false;
            HRESULT hr = CLR_RT_TypeSystem::GetGenericParamConstraintWord(list[next], i, constraint);

            if (hr != S_OK)
            {
                if (FAILED(hr))
                {
                    return hr;
                }

                break;
            }

            if (constraint.kind != REFLECTION_GENERICPARAM)
            {
                continue;
            }

            for (int j = 0; j < count; j++)
            {
                known = known || CLR_RT_TypeSystem::IsSameTypeWord(list[j], constraint);
            }

            if (!known)
            {
                if (count == c_MaxParamConstraintChain)
                {
                    return CLR_E_NOT_SUPPORTED;
                }

                list[count++] = constraint;
            }
        }
    }

    return S_OK;
}

// S_FALSE when the parameter has no class constraint.
static HRESULT GetGenericParamClassWord(const CLR_RT_ReflectionDef_Index &param, CLR_RT_ReflectionDef_Index &word)
{
    CLR_RT_Assembly *assembly;
    const CLR_RECORD_GENERICPARAM *record;

    if (!CLR_RT_TypeSystem::GetGenericParam(param, assembly, record))
    {
        return CLR_E_WRONG_TYPE;
    }

    for (int i = 0;; i++)
    {
        CLR_RT_TypeDef_Instance td{};
        HRESULT hr = CLR_RT_TypeSystem::GetGenericParamConstraintWord(param, i, word);

        if (hr != S_OK)
        {
            return hr;
        }

        if (word.kind == REFLECTION_GENERICPARAM)
        {
            continue;
        }

        if (!td.InitializeFromReflection(word, nullptr))
        {
            return CLR_E_WRONG_TYPE;
        }

        if (td.target->IsInterface())
        {
            continue;
        }

        if (td.data == g_CLR_RT_WellKnownTypes.Enum.data)
        {
            return CLR_E_NOT_SUPPORTED;
        }

        if ((record->flags & CLR_RECORD_GENERICPARAM::GP_NotNullableValueType) != 0 &&
            td.data == g_CLR_RT_WellKnownTypes.ValueType.data)
        {
            continue;
        }

        return S_OK;
    }
}

HRESULT CLR_RT_TypeSystem::GetDefinitionArgumentWord(
    const CLR_RT_TypeDef_Instance &definition,
    int position,
    CLR_RT_ReflectionDef_Index &word)
{
    if (position >= definition.target->genericParamCount)
    {
        return CLR_E_WRONG_TYPE;
    }

    SetGenericParamWord(word, definition.Assembly(), definition.target->firstGenericParam + position, 0);

    return S_OK;
}

HRESULT CLR_RT_TypeSystem::GetGenericArgumentWord(
    const CLR_RT_TypeSpec_Index &row,
    int argument,
    CLR_RT_ReflectionDef_Index &word)
{
    CLR_RT_GenericInstanceNode nodes[CLR_RT_GenericInstance::c_MaxNodes];
    CLR_RT_TypeSpec_Index none;
    const CLR_RT_GenericInstance *record = row.IsRuntime() ? GetGenericInstance(row) : nullptr;
    CLR_RT_GenericInstance *instance;
    bool hasParameter = false;
    int first;
    int end;
    HRESULT hr;

    word.Clear();

    if (record == nullptr || !GetArgumentRange(*record, argument, first, end))
    {
        return CLR_E_WRONG_TYPE;
    }

    const CLR_RT_GenericInstanceNode &head = record->m_nodes[first];

    if (head.DataType == DATATYPE_VAR)
    {
        CLR_RT_TypeDef_Instance owner{};

        if (!owner.InitializeFromIndex(head.Class) || head.Position >= owner.target->genericParamCount)
        {
            return CLR_E_WRONG_TYPE;
        }

        SetGenericParamWord(word, owner.Assembly(), owner.target->firstGenericParam + head.Position, head.Levels);

        return S_OK;
    }

    if (head.DataType == DATATYPE_MVAR)
    {
        CLR_RT_MethodDef_Index owner;
        CLR_RT_MethodDef_Instance method{};

        owner.data = head.Class.data;

        if (!method.InitializeFromIndex(owner) || head.Position >= method.target->genericParamCount)
        {
            return CLR_E_WRONG_TYPE;
        }

        SetGenericParamWord(word, method.Assembly(), method.target->firstGenericParam + head.Position, head.Levels);

        return S_OK;
    }

    if (end - first == 1)
    {
        word.kind = REFLECTION_TYPE;
        word.levels = head.Levels;
        word.data.type = head.Class;

        return S_OK;
    }

    memcpy(nodes, &record->m_nodes[first], sizeof(CLR_RT_GenericInstanceNode) * (end - first));
    nodes[0].Levels = 0;

    for (int i = 0; i < end - first; i++)
    {
        hasParameter = hasParameter || nodes[i].DataType == DATATYPE_VAR || nodes[i].DataType == DATATYPE_MVAR;
    }

    none.Clear();
    instance = RegisterGenericInstance(nodes, end - first, none, none, hr);

    if (instance == nullptr)
    {
        return hr;
    }

    if (hasParameter)
    {
        instance->m_flags |= CLR_RT_GenericInstance::c_Open;
    }

    word.kind = REFLECTION_TYPESPEC;
    word.levels = head.Levels;
    word.data.typeSpec = instance->m_self;

    return S_OK;
}

HRESULT CLR_RT_TypeSystem::GetOpenGenericRecord(
    const CLR_RT_GenericInstanceNode *rowNodes,
    int rowCount,
    const CLR_RT_TypeSpec_Index &origin,
    const CLR_RT_TypeDef_Index &level,
    const CLR_RT_TypeSpec_Index *openContext,
    CLR_RT_TypeSpec_Index &row,
    const CLR_RT_MethodDef_Index *method)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_GenericInstanceNode nodes[CLR_RT_GenericInstance::c_MaxNodes];
    CLR_RT_GenericInstance *context = nullptr;
    CLR_RT_GenericInstance *record;
    CLR_RT_TypeSpec_Index none;
    int count = 0;
    bool hasParameter = false;
    HRESULT hr;

    if (openContext != nullptr)
    {
        context = GetGenericInstance(*openContext);

        if (context == nullptr || (context->m_flags & CLR_RT_GenericInstance::c_Open) == 0)
        {
            return CLR_E_NOT_SUPPORTED;
        }
    }

    for (int i = 0; i < rowCount; i++)
    {
        const CLR_RT_GenericInstanceNode &node = rowNodes[i];

        if (node.DataType == DATATYPE_MVAR && method == nullptr)
        {
            return CLR_E_NOT_SUPPORTED;
        }

        if (node.DataType == DATATYPE_VAR && context != nullptr)
        {
            int first;
            int end;

            if (!GetArgumentRange(*context, node.Position, first, end))
            {
                return CLR_E_WRONG_TYPE;
            }

            if (count + (end - first) > CLR_RT_GenericInstance::c_MaxNodes ||
                context->m_nodes[first].Levels + node.Levels > 0xFF)
            {
                return CLR_E_NOT_SUPPORTED;
            }

            for (int j = first; j < end; j++)
            {
                nodes[count++] = context->m_nodes[j];
            }

            nodes[count - (end - first)].Levels = (CLR_UINT8)(nodes[count - (end - first)].Levels + node.Levels);
        }
        else
        {
            if (count >= CLR_RT_GenericInstance::c_MaxNodes)
            {
                return CLR_E_NOT_SUPPORTED;
            }

            nodes[count] = node;

            if (node.DataType == DATATYPE_VAR)
            {
                nodes[count].Class = level;
            }
            else if (node.DataType == DATATYPE_MVAR)
            {
                nodes[count].Class.data = method->data;
            }

            count++;
        }
    }

    {
        CLR_UINT8 remainingArgs[c_MaxGenericNestingDepth + 1];
        int depth = 0;

        for (int i = 0; i < count; i++)
        {
            hasParameter = hasParameter || nodes[i].DataType == DATATYPE_VAR || nodes[i].DataType == DATATYPE_MVAR;

            if (nodes[i].ArgCount > 0)
            {
                if (depth == c_MaxGenericNestingDepth)
                {
                    return CLR_E_NOT_SUPPORTED;
                }

                remainingArgs[depth++] = nodes[i].ArgCount;
            }
            else
            {
                while (depth > 0 && --remainingArgs[depth - 1] == 0)
                {
                    depth--;
                }
            }
        }
    }

    none.Clear();
    record = RegisterGenericInstance(nodes, count, none, origin, hr);

    if (record == nullptr)
    {
        return hr;
    }

    if (hasParameter)
    {
        record->m_flags |= CLR_RT_GenericInstance::c_Open;
    }

    row = record->m_self;

    return S_OK;
}

HRESULT CLR_RT_TypeSystem::GetOpenGenericRow(
    const CLR_RT_TypeSpec_Index &sourceRow,
    const CLR_RT_TypeDef_Index &level,
    const CLR_RT_TypeSpec_Index *openContext,
    CLR_RT_TypeSpec_Index &row)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_TypeSpec_Instance source{};
    CLR_RT_GenericInstanceNode rowNodes[CLR_RT_GenericInstance::c_MaxNodes];
    int rowCount;
    bool closed;
    bool substituted;
    HRESULT hr;

    if (!source.InitializeFromIndex(sourceRow))
    {
        return CLR_E_WRONG_TYPE;
    }

    hr = CollectGenericInstanceNodes(source, nullptr, nullptr, nullptr, rowNodes, rowCount, closed, substituted);

    if (FAILED(hr))
    {
        return hr;
    }

    if (closed)
    {
        return S_FALSE;
    }

    return GetOpenGenericRecord(rowNodes, rowCount, sourceRow, level, openContext, row);
}

HRESULT CLR_RT_TypeSystem::GetOpenSignatureWord(
    const CLR_RT_SignatureParser &at,
    const CLR_RT_TypeSpec_Index *openOwner,
    const CLR_RT_TypeDef_Index &declaring,
    const CLR_RT_MethodDef_Index *method,
    CLR_RT_ReflectionDef_Index &word)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_SignatureParser peek = at;
    CLR_RT_SignatureParser::Element elem;
    CLR_RT_GenericInstanceNode nodes[CLR_RT_GenericInstance::c_MaxNodes];
    CLR_RT_TypeSpec_Index none;
    CLR_RT_TypeSpec_Index row;
    int count;
    bool closed;
    bool substituted;
    HRESULT hr;

    word.Clear();

    hr = peek.Advance(elem);

    if (FAILED(hr))
    {
        return hr;
    }

    if (elem.DataType == DATATYPE_MVAR)
    {
        CLR_RT_MethodDef_Instance md{};

        if (method == nullptr || !md.InitializeFromIndex(*method) ||
            elem.GenericParamPosition >= md.target->genericParamCount)
        {
            return CLR_E_NOT_SUPPORTED;
        }

        SetGenericParamWord(word, md.Assembly(), md.target->firstGenericParam + elem.GenericParamPosition, elem.Levels);

        return S_OK;
    }

    if (elem.DataType == DATATYPE_VAR)
    {
        if (openOwner != nullptr)
        {
            hr = GetGenericArgumentWord(*openOwner, elem.GenericParamPosition, word);

            if (SUCCEEDED(hr))
            {
                word.levels = (CLR_UINT16)(word.levels + elem.Levels);
            }

            return hr;
        }

        CLR_RT_TypeDef_Instance td{};

        if (!td.InitializeFromIndex(declaring))
        {
            return CLR_E_WRONG_TYPE;
        }

        hr = GetDefinitionArgumentWord(td, elem.GenericParamPosition, word);
        word.levels = elem.Levels;

        return hr;
    }

    if (elem.DataType != DATATYPE_GENERICINST)
    {
        return S_FALSE;
    }

    hr = CollectGenericNodesAt(at, nullptr, nullptr, nullptr, nodes, count, closed, substituted);

    if (FAILED(hr) || closed)
    {
        return FAILED(hr) ? hr : S_FALSE;
    }

    word.levels = elem.Levels;
    nodes[0].Levels = 0;
    none.Clear();

    hr = GetOpenGenericRecord(nodes, count, none, declaring, openOwner, row, method);

    if (FAILED(hr))
    {
        word.Clear();

        return hr;
    }

    word.kind = REFLECTION_TYPESPEC;
    word.data.typeSpec = row;

    return S_OK;
}

HRESULT CLR_RT_TypeSystem::GetTypeBase(
    const CLR_RT_TypeDef_Instance &child,
    const CLR_RT_TypeSpec_Index *context,
    bool isDefinition,
    CLR_RT_TypeSpec_Index &row)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_TypeSpec_Index extends;
    bool contextIsOpen = context != nullptr && IsOpenInstance(*context);
    HRESULT hr;

    if (!child.GetExtendsTypeSpec(extends))
    {
        return S_FALSE;
    }

    if (isDefinition || contextIsOpen)
    {
        hr = GetOpenGenericRow(extends, child, contextIsOpen ? context : nullptr, row);

        if (hr != S_FALSE)
        {
            return hr;
        }

        context = nullptr;
    }

    hr = GetClosedGenericBase(child, context, row);

    return hr == S_FALSE ? CLR_E_NOT_SUPPORTED : hr;
}

HRESULT CLR_RT_TypeSystem::GetInterfaceRow(
    const CLR_RT_TypeSpec_Index &entry,
    const CLR_RT_TypeDef_Index &level,
    const CLR_RT_TypeSpec_Index *context,
    bool isDefinition,
    CLR_RT_TypeSpec_Index &row)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_TypeSpec_Instance inst{};
    CLR_RT_GenericInstance *record;
    bool contextIsOpen = context != nullptr && IsOpenInstance(*context);
    HRESULT hr;

    if (!inst.InitializeFromIndex(entry))
    {
        return CLR_E_WRONG_TYPE;
    }

    if (inst.IsClosedGenericType())
    {
        row = entry;

        return S_OK;
    }

    if (isDefinition || contextIsOpen)
    {
        hr = GetOpenGenericRow(entry, level, contextIsOpen ? context : nullptr, row);

        return hr == S_FALSE ? CLR_E_NOT_SUPPORTED : hr;
    }

    if (context == nullptr)
    {
        return CLR_E_NOT_SUPPORTED;
    }

    record = FindOrCreateGenericInstance(inst, context, nullptr, &hr);

    if (record == nullptr)
    {
        return FAILED(hr) ? hr : CLR_E_NOT_SUPPORTED;
    }

    row = record->m_self;

    return S_OK;
}

HRESULT CLR_RT_TypeSystem::GetArrayInterfaceRow(
    const CLR_RT_ReflectionDef_Index &array,
    int index,
    CLR_RT_TypeSpec_Index &row)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_GenericInstanceNode nodes[CLR_RT_GenericInstance::c_MaxNodes];
    CLR_RT_TypeDef_Index definition;
    CLR_RT_GenericInstance *instance;
    int nodeCount;
    HRESULT hr;

    if (!GetArrayInterfaceDefinition(index, definition))
    {
        return S_FALSE;
    }

    if (!BuildArrayElementNodes(
            array.kind == REFLECTION_TYPE ? array.data.type : CLR_RT_TypeDef_Index(),
            array.kind == REFLECTION_TYPESPEC ? &array.data.typeSpec : nullptr,
            array.levels,
            nodes,
            nodeCount))
    {
        return CLR_E_NOT_SUPPORTED;
    }

    nodes[0].Class = definition;
    nodes[0].DataType = DATATYPE_CLASS;
    nodes[0].Levels = 0;
    nodes[0].ArgCount = 1;
    nodes[0].Position = 0;

    instance = FindOrCreateGenericInstanceByNodes(nodes, nodeCount, &hr);

    if (instance == nullptr)
    {
        return hr;
    }

    row = instance->m_self;

    return S_OK;
}

bool CLR_RT_TypeSystem::IsSameTypeSpecWord(
    const CLR_RT_ReflectionDef_Index &left,
    const CLR_RT_ReflectionDef_Index &right)
{
    return left.kind == REFLECTION_TYPESPEC && right.kind == REFLECTION_TYPESPEC && left.levels == right.levels &&
           IsSameTypeSpecIdentity(left.data.typeSpec, right.data.typeSpec);
}

bool CLR_RT_TypeSystem::IsSameTypeWord(const CLR_RT_ReflectionDef_Index &left, const CLR_RT_ReflectionDef_Index &right)
{
    return (left.kind == right.kind && left.levels == right.levels && left.data.raw == right.data.raw) ||
           IsSameTypeSpecWord(left, right);
}

HRESULT CLR_RT_TypeSystem::GetBaseTypeWord(const CLR_RT_ReflectionDef_Index &reflex, CLR_RT_ReflectionDef_Index &word)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_TypeDef_Instance td{};
    CLR_RT_TypeSpec_Index extends;
    CLR_UINT32 levels;

    word.Clear();

    if (reflex.kind == REFLECTION_GENERICPARAM)
    {
        CLR_RT_Assembly *assembly;
        const CLR_RECORD_GENERICPARAM *record;

        if (reflex.levels == 0)
        {
            HRESULT hr = GetGenericParamClassWord(reflex, word);

            if (hr != S_FALSE)
            {
                return hr;
            }
        }

        if (!GetGenericParam(reflex, assembly, record))
        {
            return CLR_E_WRONG_TYPE;
        }

        word.Clear();
        word.kind = REFLECTION_TYPE;

        if (reflex.levels > 0)
        {
            word.data.type = g_CLR_RT_WellKnownTypes.Array;
        }
        else
        {
            word.data.type = (record->flags & CLR_RECORD_GENERICPARAM::GP_NotNullableValueType) != 0
                                 ? g_CLR_RT_WellKnownTypes.ValueType
                                 : g_CLR_RT_WellKnownTypes.Object;
        }

        return S_OK;
    }

    if (!td.InitializeFromReflection(reflex, &levels))
    {
        return CLR_E_WRONG_TYPE;
    }

    if (levels > 0)
    {
        word.kind = REFLECTION_TYPE;
        word.data.type = g_CLR_RT_WellKnownTypes.Array;

        return S_OK;
    }

    if (td.GetExtendsTypeSpec(extends))
    {
        CLR_RT_TypeSpec_Index row;
        HRESULT hr = GetTypeBase(
            td,
            reflex.kind == REFLECTION_TYPESPEC ? &reflex.data.typeSpec : nullptr,
            reflex.kind == REFLECTION_TYPE && td.target->genericParamCount > 0,
            row);

        if (FAILED(hr))
        {
            return hr;
        }

        word.kind = REFLECTION_TYPESPEC;
        word.data.typeSpec = row;

        return S_OK;
    }

    if (td.SwitchToParent())
    {
        word.kind = REFLECTION_TYPE;
        word.data.type = td;

        return S_OK;
    }

    return S_FALSE;
}

static HRESULT AddInterfaceWord(
    CLR_RT_ReflectionDef_Index *words,
    int max,
    int &count,
    const CLR_RT_ReflectionDef_Index &word)
{
    for (int i = 0; i < count; i++)
    {
        if (CLR_RT_TypeSystem::IsSameTypeWord(words[i], word))
        {
            return S_OK;
        }
    }

    if (count == max)
    {
        return CLR_E_NOT_SUPPORTED;
    }

    words[count++] = word;

    return S_OK;
}

static HRESULT AppendGenericParamInterfaceWords(
    const CLR_RT_ReflectionDef_Index &param,
    CLR_RT_ReflectionDef_Index *words,
    int max,
    int &count)
{
    CLR_RT_ReflectionDef_Index closure[c_MaxParamConstraintChain];
    int members;
    HRESULT hr = GetGenericParamClosure(param, closure, members);

    for (int m = 0; SUCCEEDED(hr) && m < members; m++)
    {
        for (int i = 0;; i++)
        {
            CLR_RT_ReflectionDef_Index constraint;
            CLR_RT_TypeDef_Instance td{};

            hr = CLR_RT_TypeSystem::GetGenericParamConstraintWord(closure[m], i, constraint);

            if (hr != S_OK)
            {
                hr = SUCCEEDED(hr) ? S_OK : hr;

                break;
            }

            if (constraint.kind == REFLECTION_GENERICPARAM)
            {
                continue;
            }

            if (!td.InitializeFromReflection(constraint, nullptr) || td.data == g_CLR_RT_WellKnownTypes.Enum.data)
            {
                return CLR_E_NOT_SUPPORTED;
            }

            hr = td.target->IsInterface() ? AddInterfaceWord(words, max, count, constraint) : S_OK;

            if (SUCCEEDED(hr))
            {
                hr = CLR_RT_TypeSystem::AppendInterfaceWords(constraint, words, max, count);
            }

            if (FAILED(hr))
            {
                break;
            }
        }
    }

    return hr;
}

HRESULT CLR_RT_TypeSystem::CollectInterfaceWords(
    const CLR_RT_ReflectionDef_Index &reflex,
    CLR_RT_ReflectionDef_Index *words,
    int max,
    int &count)
{
    count = 0;

    return AppendInterfaceWords(reflex, words, max, count);
}

HRESULT CLR_RT_TypeSystem::AppendInterfaceWords(
    const CLR_RT_ReflectionDef_Index &reflex,
    CLR_RT_ReflectionDef_Index *words,
    int max,
    int &count)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_TypeDef_Instance td{};
    CLR_RT_ReflectionDef_Index word;
    CLR_RT_TypeSpec_Index context;
    bool hasContext = reflex.kind == REFLECTION_TYPESPEC && reflex.levels == 0;
    bool isDefinition;
    HRESULT hr;

    context.Clear();
    word.Clear();

    if (reflex.kind == REFLECTION_GENERICPARAM)
    {
        return reflex.levels == 0 ? AppendGenericParamInterfaceWords(reflex, words, max, count) : CLR_E_NOT_SUPPORTED;
    }

    // an array has the interfaces of System.Array
    if (!td.InitializeFromReflection(reflex, nullptr))
    {
        return CLR_E_WRONG_TYPE;
    }

    if (hasContext)
    {
        context = reflex.data.typeSpec;
    }

    isDefinition = reflex.kind == REFLECTION_TYPE && reflex.levels == 0 && td.target->genericParamCount > 0;

    do
    {
        CLR_RT_SignatureParser parser{};
        CLR_RT_TypeSpec_Index extends;

        parser.Initialize_Interfaces(td.assembly, td.target);

        while (parser.Available() > 0)
        {
            CLR_RT_SignatureParser peek = parser;
            CLR_RT_SignatureParser::Element res;
            CLR_UINT32 tk = CLR_TkFromStream(peek.Signature);

            hr = parser.Advance(res);

            if (FAILED(hr))
            {
                return hr;
            }

            word.Clear();

            if (CLR_TypeFromTk(tk) == TBL_TypeSpec)
            {
                CLR_RT_TypeSpec_Index entry;
                CLR_RT_TypeSpec_Index row;

                entry.Set(td.assembly->assemblyIndex, CLR_DataFromTk(tk));

                hr = GetInterfaceRow(entry, td, hasContext ? &context : nullptr, isDefinition, row);

                if (FAILED(hr))
                {
                    return hr;
                }

                word.kind = REFLECTION_TYPESPEC;
                word.data.typeSpec = row;
            }
            else
            {
                word.kind = REFLECTION_TYPE;
                word.data.type = res.Class;
            }

            hr = AddInterfaceWord(words, max, count, word);

            if (FAILED(hr))
            {
                return hr;
            }
        }

        if (td.GetExtendsTypeSpec(extends))
        {
            CLR_RT_TypeSpec_Index parentContext;

            hr = GetTypeBase(td, hasContext ? &context : nullptr, isDefinition, parentContext);

            if (FAILED(hr))
            {
                return hr;
            }

            context = parentContext;
            hasContext = true;
        }
        else
        {
            hasContext = false;
        }

        isDefinition = false;
    } while (td.SwitchToParent());

    if (reflex.levels > 0)
    {
        for (int i = 0; i < GetArrayInterfaceCount(); i++)
        {
            CLR_RT_TypeSpec_Index row;

            hr = GetArrayInterfaceRow(reflex, i, row);

            if (FAILED(hr))
            {
                return hr;
            }

            if (hr == S_OK)
            {
                word.Clear();
                word.kind = REFLECTION_TYPESPEC;
                word.data.typeSpec = row;

                hr = AddInterfaceWord(words, max, count, word);

                if (FAILED(hr))
                {
                    return hr;
                }
            }
        }
    }

    return S_OK;
}

// The type word of an argument of a closed instance.
HRESULT CLR_RT_TypeSystem::GetArgumentWord(
    const CLR_RT_GenericInstance &record,
    int argument,
    CLR_RT_ReflectionDef_Index &word)
{
    CLR_RT_GenericInstanceNode nodes[CLR_RT_GenericInstance::c_MaxNodes];
    CLR_RT_GenericInstance *instance;
    int first;
    int end;
    HRESULT hr;

    word.Clear();

    if (!GetArgumentRange(record, argument, first, end) || record.m_nodes[first].DataType == DATATYPE_VAR ||
        record.m_nodes[first].DataType == DATATYPE_MVAR)
    {
        return CLR_E_NOT_SUPPORTED;
    }

    if (end - first == 1)
    {
        word.kind = REFLECTION_TYPE;
        word.levels = record.m_nodes[first].Levels;
        word.data.type = record.m_nodes[first].Class;

        return S_OK;
    }

    memcpy(nodes, &record.m_nodes[first], sizeof(CLR_RT_GenericInstanceNode) * (end - first));
    nodes[0].Levels = 0;

    instance = CLR_RT_TypeSystem::FindOrCreateGenericInstanceByNodes(nodes, end - first, &hr);

    if (instance == nullptr)
    {
        return hr;
    }

    word.kind = REFLECTION_TYPESPEC;
    word.levels = record.m_nodes[first].Levels;
    word.data.typeSpec = instance->m_self;

    return S_OK;
}

static HRESULT GetWordDefinition(const CLR_RT_ReflectionDef_Index &word, CLR_RT_TypeDef_Instance &td)
{
    if (word.kind == REFLECTION_GENERICPARAM)
    {
        return td.InitializeFromIndex(g_CLR_RT_WellKnownTypes.Object) ? S_OK : CLR_E_WRONG_TYPE;
    }

    return td.InitializeFromReflection(word, nullptr) ? S_OK : CLR_E_WRONG_TYPE;
}

static HRESULT IsAssignableGenericParam(
    const CLR_RT_ReflectionDef_Index &target,
    const CLR_RT_ReflectionDef_Index &source,
    bool inArray,
    bool &result)
{
    if (!inArray && target.kind == REFLECTION_TYPE && target.levels == 0 &&
        target.data.type.data == g_CLR_RT_WellKnownTypes.Object.data)
    {
        result = true;

        return S_OK;
    }

    if (inArray || target.levels > 0 || source.levels > 0)
    {
        return CLR_E_NOT_SUPPORTED;
    }

    if (target.kind != REFLECTION_GENERICPARAM)
    {
        return S_FALSE;
    }

    result = false;

    if (source.kind == REFLECTION_GENERICPARAM)
    {
        CLR_RT_ReflectionDef_Index closure[c_MaxParamConstraintChain];
        int members;
        HRESULT hr = GetGenericParamClosure(source, closure, members);

        for (int i = 1; SUCCEEDED(hr) && i < members; i++)
        {
            result = result || CLR_RT_TypeSystem::IsSameTypeWord(closure[i], target);
        }

        return hr;
    }

    return S_OK;
}

HRESULT CLR_RT_TypeSystem::IsAssignableFrom(
    const CLR_RT_ReflectionDef_Index &targetType,
    const CLR_RT_ReflectionDef_Index &sourceType,
    bool &result)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_ReflectionDef_Index target = targetType;
    CLR_RT_ReflectionDef_Index source = sourceType;
    CLR_RT_ReflectionDef_Index words[32];
    bool inArray = false;
    HRESULT hr;

    result = false;

    for (int step = 0; step < 4 * c_MaxInheritanceDepth; step++)
    {
        CLR_RT_TypeDef_Instance targetDef{};
        CLR_RT_TypeDef_Instance sourceDef{};

        if (IsSameTypeWord(target, source))
        {
            result = true;

            return S_OK;
        }

        if (target.kind == REFLECTION_GENERICPARAM || source.kind == REFLECTION_GENERICPARAM)
        {
            hr = IsAssignableGenericParam(target, source, inArray, result);

            if (hr != S_FALSE)
            {
                return hr;
            }
        }

        if ((target.kind != REFLECTION_TYPE && target.kind != REFLECTION_TYPESPEC) ||
            (source.kind != REFLECTION_TYPE && source.kind != REFLECTION_TYPESPEC &&
             source.kind != REFLECTION_GENERICPARAM))
        {
            return CLR_E_WRONG_TYPE;
        }

        if (target.levels > 0)
        {
            CLR_UINT16 peel = target.levels < source.levels ? target.levels : source.levels;

            if (source.levels == 0)
            {
                return S_OK;
            }

            target.levels -= peel;
            source.levels -= peel;
            inArray = true;

            if (target.levels > 0)
            {
                return S_OK;
            }

            continue;
        }

        hr = GetWordDefinition(target, targetDef);

        if (FAILED(hr))
        {
            return hr;
        }

        hr = GetWordDefinition(source, sourceDef);

        if (FAILED(hr))
        {
            return hr;
        }

        if (inArray && source.levels == 0)
        {
            // the element types of arrays: a value type is only assignable to itself
            if (sourceDef.target->IsValueTypeOrEnum())
            {
                int size = IntegralSize(sourceDef.target->dataType);

                result = size != 0 && size == IntegralSize(targetDef.target->dataType) &&
                         targetDef.target->IsValueTypeOrEnum();

                return S_OK;
            }

            if (targetDef.target->IsValueTypeOrEnum())
            {
                return S_OK;
            }

            inArray = false;
        }

        if (targetDef.data == g_CLR_RT_WellKnownTypes.Object.data)
        {
            result = true;

            return S_OK;
        }

        if (source.levels > 0)
        {
            int count;

            if (targetDef.data == g_CLR_RT_WellKnownTypes.Array.data)
            {
                result = true;

                return S_OK;
            }

            if (!targetDef.target->IsInterface())
            {
                return S_OK;
            }

            hr = CollectInterfaceWords(source, words, ARRAYSIZE(words), count);

            if (FAILED(hr))
            {
                return hr;
            }

            for (int i = 0; i < count; i++)
            {
                if (IsSameTypeWord(words[i], target))
                {
                    result = true;

                    return S_OK;
                }
            }

            if (target.kind == REFLECTION_TYPESPEC && IsArrayGenericInterface(targetDef))
            {
                CLR_RT_GenericInstance *record = RecordOfTypeSpec(target.data.typeSpec);

                if (record == nullptr)
                {
                    return S_OK;
                }

                hr = GetArgumentWord(*record, 0, target);

                if (FAILED(hr))
                {
                    return hr;
                }

                source.levels--;
                inArray = true;

                continue;
            }

            return S_OK;
        }

        if (sourceDef.data == targetDef.data && CLR_RT_GenericAssignability::IsVariantWord(source, target))
        {
            result = true;

            return S_OK;
        }

        if (target.kind == REFLECTION_TYPESPEC && targetDef.data == g_CLR_RT_WellKnownTypes.Nullable.data)
        {
            CLR_RT_GenericInstance *record = RecordOfTypeSpec(target.data.typeSpec);
            CLR_RT_ReflectionDef_Index argument;

            if (record != nullptr && SUCCEEDED(GetArgumentWord(*record, 0, argument)) &&
                IsSameTypeWord(argument, source))
            {
                result = true;

                return S_OK;
            }
        }

        if (targetDef.target->IsInterface())
        {
            int count;

            hr = CollectInterfaceWords(source, words, ARRAYSIZE(words), count);

            if (FAILED(hr))
            {
                return hr;
            }

            for (int i = 0; i < count; i++)
            {
                if (IsSameTypeWord(words[i], target) || CLR_RT_GenericAssignability::IsVariantWord(words[i], target))
                {
                    result = true;

                    return S_OK;
                }
            }

            return S_OK;
        }

        // a class or a struct: the base chain of the source, of each parameter that constrains it
        {
            CLR_RT_ReflectionDef_Index starts[c_MaxParamConstraintChain];
            int startCount = 1;

            starts[0] = source;

            if (source.kind == REFLECTION_GENERICPARAM)
            {
                hr = GetGenericParamClosure(source, starts, startCount);

                if (FAILED(hr))
                {
                    return hr;
                }
            }

            for (int start = 0; start < startCount; start++)
            {
                CLR_RT_ReflectionDef_Index current = starts[start];
                bool ended = false;

                for (int depth = 0; depth < c_MaxInheritanceDepth && !ended; depth++)
                {
                    CLR_RT_ReflectionDef_Index parent;

                    hr = GetBaseTypeWord(current, parent);

                    if (FAILED(hr))
                    {
                        return hr;
                    }

                    ended = hr == S_FALSE;

                    if (ended)
                    {
                        break;
                    }

                    if (IsSameTypeWord(parent, target) || CLR_RT_GenericAssignability::IsVariantWord(parent, target))
                    {
                        result = true;

                        return S_OK;
                    }

                    current = parent;
                }

                if (!ended)
                {
                    return CLR_E_NOT_SUPPORTED;
                }
            }

            return S_OK;
        }
    }

    return CLR_E_NOT_SUPPORTED;
}

int CLR_RT_GenericInstantiation::GetNestingDepth(const CLR_RT_GenericInstanceNode *nodes, int count)
{
    CLR_UINT8 remaining[CLR_RT_GenericInstance::c_MaxNodes];
    int top = 0;
    int depth = 0;

    for (int i = 0; i < count; i++)
    {
        if (top > 0)
        {
            remaining[top - 1]--;
        }

        if (nodes[i].ArgCount > 0)
        {
            remaining[top++] = nodes[i].ArgCount;
            depth = top > depth ? top : depth;
        }

        while (top > 0 && remaining[top - 1] == 0)
        {
            top--;
        }
    }

    return depth;
}

HRESULT CLR_RT_GenericInstantiation::AppendArgumentNodes(
    CLR_RT_HeapBlock_Array *arguments,
    CLR_UINT32 assembly,
    CLR_UINT32 first,
    int arity,
    CLR_RT_GenericTarget &built,
    CLR_RT_ReflectionDef_Index *words)
{
    CLR_RT_GenericInstanceNode buffer[CLR_RT_GenericInstance::c_MaxNodes];
    CLR_RT_HeapBlock *elements;
    int own = 0;

    if (arguments == nullptr)
    {
        return CLR_E_ARGUMENT_NULL;
    }

    if ((int)arguments->m_numOfElements != arity)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    elements = (CLR_RT_HeapBlock *)arguments->GetFirstElement();

    for (int i = 0; i < arity; i++)
    {
        if (elements[i].Dereference() == nullptr)
        {
            return CLR_E_ARGUMENT_NULL;
        }
    }

    if (arity >= CLR_RT_GenericInstance::c_MaxNodes)
    {
        return CLR_E_NOT_SUPPORTED;
    }

    for (int i = 0; i < arity; i++)
    {
        const CLR_RT_HeapBlock *type = elements[i].Dereference();
        const CLR_RT_GenericInstanceNode *nodes = buffer;
        CLR_RT_ReflectionDef_Index param;
        int count = 1;
        CLR_UINT32 hash;

        if (type->DataType() != DATATYPE_REFLECTION)
        {
            return CLR_E_INVALID_PARAMETER;
        }

        words[i] = type->ReflectionDataConst();
        CLR_RT_TypeSystem::SetGenericParamWord(param, assembly, first + i, 0);

        if (CLR_RT_TypeSystem::IsSameTypeWord(words[i], param))
        {
            own++;
            continue;
        }

        if (words[i].kind == REFLECTION_TYPE)
        {
            CLR_RT_TypeDef_Instance td{};

            if (!td.InitializeFromIndex(words[i].data.type))
            {
                return CLR_E_WRONG_TYPE;
            }

            if (words[i].data.type.data == g_CLR_RT_WellKnownTypes.n_Void.data)
            {
                return CLR_E_INVALID_PARAMETER;
            }

            if (td.target->genericParamCount > 0)
            {
                return CLR_E_NOT_SUPPORTED;
            }

            buffer[0].Class = words[i].data.type;
            buffer[0].DataType = (CLR_UINT8)SignatureDataTypeOfClass(words[i].data.type);
            buffer[0].Levels = 0;
            buffer[0].ArgCount = 0;
            buffer[0].Position = 0;
        }
        else if (
            words[i].kind != REFLECTION_TYPESPEC || CLR_RT_TypeSystem::IsOpenInstance(words[i].data.typeSpec) ||
            !GetTypeSpecNodes(words[i].data.typeSpec, buffer, nodes, count, hash))
        {
            return CLR_E_NOT_SUPPORTED;
        }

        if (built.m_count + count > CLR_RT_GenericInstance::c_MaxNodes || nodes[0].Levels + words[i].levels > 0xFF)
        {
            return CLR_E_NOT_SUPPORTED;
        }

        memcpy(&built.m_nodes[built.m_count], nodes, sizeof(CLR_RT_GenericInstanceNode) * count);
        built.m_nodes[built.m_count].Levels = (CLR_UINT8)(nodes[0].Levels + words[i].levels);
        built.m_count += count;
    }

    if (own == arity)
    {
        return S_FALSE;
    }

    if (own > 0 || GetNestingDepth(built.m_nodes, built.m_count) > c_MaxGenericNestingDepth)
    {
        return CLR_E_NOT_SUPPORTED;
    }

    return S_OK;
}

bool CLR_RT_GenericInstantiation::HasPublicDefaultConstructor(const CLR_RT_TypeDef_Instance &td)
{
    const CLR_RECORD_TYPEDEF *target = td.target;
    int count = target->virtualMethodCount + target->instanceMethodCount + target->staticMethodCount;

    for (int i = 0; i < count; i++)
    {
        const CLR_RECORD_METHODDEF *md = td.assembly->GetMethodDef(target->firstMethod + i);

        if ((md->flags & (CLR_RECORD_METHODDEF::MD_Constructor | CLR_RECORD_METHODDEF::MD_Static |
                          CLR_RECORD_METHODDEF::MD_Scope_Mask)) ==
                (CLR_RECORD_METHODDEF::MD_Constructor | CLR_RECORD_METHODDEF::MD_Scope_Public) &&
            md->argumentsCount == 1)
        {
            return true;
        }
    }

    return false;
}

bool CLR_RT_GenericInstantiation::IsByRefLike(const CLR_RT_TypeDef_Instance &td)
{
    CLR_RT_AttributeEnumerator attributes;

    if (!td.target->IsValueTypeOrEnum() || (td.target->flags & CLR_RECORD_TYPEDEF::TD_HasAttributes) == 0)
    {
        return false;
    }

    attributes.Initialize(td);

    while (attributes.Advance())
    {
        CLR_RT_TypeDef_Instance attribute{};

        attributes.GetCurrent(&attribute);

        if (strcmp(attribute.assembly->GetString(attribute.target->name), "IsByRefLikeAttribute") == 0 &&
            strcmp(attribute.assembly->GetString(attribute.target->nameSpace), "System.Runtime.CompilerServices") == 0)
        {
            return true;
        }
    }

    return false;
}

HRESULT CLR_RT_GenericInstantiation::CloseConstraintWord(
    CLR_RT_ReflectionDef_Index &constraint,
    const CLR_RT_ReflectionDef_Index *words,
    const CLR_RT_GenericTarget &built,
    const CLR_RT_GenericInstance *owner,
    bool method)
{
    if (constraint.kind == REFLECTION_GENERICPARAM)
    {
        CLR_RT_Assembly *assembly;
        const CLR_RECORD_GENERICPARAM *record;
        CLR_RT_TypeDef_Index type;
        CLR_RT_MethodDef_Index declaringMethod;
        bool isMethod;
        HRESULT hr = CLR_RT_TypeSystem::GetGenericParamOwner(constraint, isMethod, type, declaringMethod);

        if (FAILED(hr) || !CLR_RT_TypeSystem::GetGenericParam(constraint, assembly, record) || constraint.levels != 0)
        {
            return CLR_E_NOT_SUPPORTED;
        }

        if (isMethod == method)
        {
            constraint = words[record->number];

            return S_OK;
        }

        return owner != nullptr ? CLR_RT_TypeSystem::GetArgumentWord(*owner, record->number, constraint)
                                : CLR_E_NOT_SUPPORTED;
    }

    if (constraint.kind == REFLECTION_TYPESPEC && CLR_RT_TypeSystem::IsOpenInstance(constraint.data.typeSpec))
    {
        const CLR_RT_GenericInstance *open = CLR_RT_TypeSystem::GetGenericInstance(constraint.data.typeSpec);
        CLR_RT_GenericInstance *closed = nullptr;
        CLR_RT_GenericTarget stream;
        bool substituted;

        stream.m_count = open->m_nodeCount;
        memcpy(stream.m_nodes, open->m_nodes, sizeof(CLR_RT_GenericInstanceNode) * open->m_nodeCount);

        substituted = method
                          ? CLR_RT_GenericDispatch::SubstituteNodes(
                                stream,
                                owner != nullptr ? owner->m_nodes : nullptr,
                                owner != nullptr ? owner->m_nodeCount : 0,
                                built.m_nodes,
                                built.m_count)
                          : CLR_RT_GenericDispatch::SubstituteNodes(stream, built.m_nodes, built.m_count, nullptr, 0);

        if (substituted)
        {
            closed = CLR_RT_TypeSystem::FindOrCreateGenericInstanceByNodes(stream.m_nodes, stream.m_count);
        }

        if (closed == nullptr)
        {
            return CLR_E_NOT_SUPPORTED;
        }

        constraint.data.typeSpec = closed->m_self;
    }

    return S_OK;
}

HRESULT CLR_RT_GenericInstantiation::CheckGenericConstraints(
    CLR_UINT32 assembly,
    CLR_UINT32 first,
    int arity,
    const CLR_RT_ReflectionDef_Index *words,
    const CLR_RT_GenericTarget &built,
    const CLR_RT_GenericInstance *owner,
    bool method)
{
    for (int i = 0; i < arity; i++)
    {
        CLR_RT_ReflectionDef_Index param;
        CLR_RT_Assembly *paramAssembly;
        const CLR_RECORD_GENERICPARAM *record;
        CLR_RT_TypeDef_Instance td{};
        const CLR_RT_ReflectionDef_Index &argument = words[i];
        bool valueType;

        CLR_RT_TypeSystem::SetGenericParamWord(param, assembly, first + i, 0);

        if (!CLR_RT_TypeSystem::GetGenericParam(param, paramAssembly, record) ||
            !td.InitializeFromReflection(argument, nullptr))
        {
            return CLR_E_WRONG_TYPE;
        }

        valueType = argument.levels == 0 && td.target->IsValueTypeOrEnum();

        if (((record->flags & CLR_RECORD_GENERICPARAM::GP_ReferenceType) != 0 && valueType) ||
            ((record->flags & CLR_RECORD_GENERICPARAM::GP_NotNullableValueType) != 0 &&
             (!valueType || td.data == g_CLR_RT_WellKnownTypes.Nullable.data)) ||
            ((record->flags & CLR_RECORD_GENERICPARAM::GP_DefaultConstructor) != 0 && !valueType &&
             (argument.levels > 0 || (td.target->flags & CLR_RECORD_TYPEDEF::TD_Abstract) != 0 ||
              td.target->IsInterface() || !HasPublicDefaultConstructor(td))) ||
            (valueType && (record->flags & CLR_RECORD_GENERICPARAM::GP_AllowByRefLike) == 0 && IsByRefLike(td)))
        {
            return CLR_E_INVALID_PARAMETER;
        }

        for (int c = 0;; c++)
        {
            CLR_RT_ReflectionDef_Index constraint;
            bool satisfied;
            HRESULT hr = CLR_RT_TypeSystem::GetGenericParamConstraintWord(param, c, constraint);

            if (hr == S_FALSE)
            {
                break;
            }

            if (FAILED(hr) || FAILED(hr = CloseConstraintWord(constraint, words, built, owner, method)) ||
                FAILED(hr = CLR_RT_TypeSystem::IsAssignableFrom(constraint, argument, satisfied)))
            {
                return hr;
            }

            if (!satisfied)
            {
                return CLR_E_INVALID_PARAMETER;
            }
        }
    }

    return S_OK;
}

HRESULT CLR_RT_GenericInstantiation::MakeGenericInstance(
    const CLR_RT_TypeDef_Instance &type,
    const CLR_RT_MethodDef_Instance *method,
    CLR_UINT16 ownerSlot,
    CLR_RT_HeapBlock_Array *arguments,
    CLR_RT_GenericInstance *&record)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_GenericTarget built;
    CLR_RT_ReflectionDef_Index words[CLR_RT_GenericInstance::c_MaxNodes - 1];
    CLR_RT_GenericInstanceNode &root = built.m_nodes[0];
    CLR_UINT32 assembly = method != nullptr ? method->Assembly() : type.Assembly();
    CLR_UINT32 first = method != nullptr ? method->target->firstGenericParam : type.target->firstGenericParam;
    int arity = method != nullptr ? method->target->genericParamCount : type.target->genericParamCount;
    HRESULT hr;

    record = nullptr;
    built.m_count = 1;

    if (method != nullptr)
    {
        root.Class.Clear();
        root.DataType = DATATYPE_VOID;
        root.Levels = (CLR_UINT8)ownerSlot;
        root.Position = (CLR_UINT8)(ownerSlot >> 8);
    }
    else
    {
        root.Class = type;
        root.DataType = (CLR_UINT8)SignatureDataTypeOfClass(type);
        root.Levels = 0;
        root.Position = 0;
    }

    root.ArgCount = (CLR_UINT8)arity;

    hr = AppendArgumentNodes(arguments, assembly, first, arity, built, words);

    if (hr != S_OK)
    {
        return hr;
    }

    hr = CheckGenericConstraints(
        assembly,
        first,
        arity,
        words,
        built,
        method != nullptr ? CLR_RT_TypeSystem::GetMemberRecord(ownerSlot, false) : nullptr,
        method != nullptr);

    if (FAILED(hr))
    {
        return hr;
    }

    record = CLR_RT_TypeSystem::FindOrCreateGenericInstanceByNodes(built.m_nodes, built.m_count, &hr);

    return record != nullptr ? S_OK : hr;
}

CLR_UINT16 CLR_RT_GenericInstantiation::GetMethodInstanceSlot(
    const CLR_RT_MethodSpec_Index &methodSpec,
    CLR_UINT16 ownerSlot)
{
    CLR_RT_GenericInstanceNode nodes[CLR_RT_GenericInstance::c_MaxNodes];
    CLR_RT_TypeSpec_Index index;
    const CLR_RT_GenericInstance *spec;
    CLR_RT_GenericInstance *record;

    index.data = methodSpec.data;
    spec = methodSpec.IsRuntime() ? CLR_RT_TypeSystem::GetGenericInstance(index) : nullptr;

    if (spec == nullptr || spec->m_nodes[0].Class.data != 0 || spec->m_nodes[0].DataType != DATATYPE_VOID)
    {
        return 0;
    }

    memcpy(nodes, spec->m_nodes, sizeof(CLR_RT_GenericInstanceNode) * spec->m_nodeCount);
    nodes[0].Levels = (CLR_UINT8)ownerSlot;
    nodes[0].Position = (CLR_UINT8)(ownerSlot >> 8);

    record = CLR_RT_TypeSystem::FindOrCreateGenericInstanceByNodes(nodes, spec->m_nodeCount);

    return record != nullptr ? (CLR_UINT16)(record->m_self.TypeSpec() + 1) : 0;
}

#endif // CONFIG_NF_FEATURE_SUPPORT_REFLECTION

// S_OK with a count of 0 when the signature isn't a generic instance or is an array of one, S_FALSE when it's open.
HRESULT CLR_RT_TypeSystem::CollectGenericTarget(
    const CLR_RT_SignatureParser &at,
    const CLR_RT_TypeSpec_Index *contextTypeSpec,
    const CLR_RT_MethodDef_Instance *contextMethod,
    CLR_RT_GenericTarget &target,
    bool allowLevels)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_SignatureParser peek = at;
    CLR_RT_SignatureParser::Element elem;
    bool closed;
    bool substituted;
    HRESULT hr;

    target.m_count = 0;

    if (SUCCEEDED(peek.Advance(elem)) && elem.DataType == DATATYPE_VAR && elem.Levels == 0 &&
        contextTypeSpec != nullptr && NANOCLR_INDEX_IS_VALID(*contextTypeSpec))
    {
        CLR_RT_TypeSpec_Instance owner{};
        CLR_RT_SignatureParser argument;

        if (owner.InitializeFromIndex(*contextTypeSpec) &&
            owner.PositionAtGenericParam(elem.GenericParamPosition, argument))
        {
            return CollectGenericTarget(argument, nullptr, nullptr, target);
        }
    }

    peek = at;

    if (FAILED(peek.Advance(elem)) || elem.DataType != DATATYPE_GENERICINST || (elem.Levels > 0 && !allowLevels))
    {
        return S_OK;
    }

    hr = CollectGenericNodesAt(
        at,
        contextTypeSpec,
        contextMethod,
        nullptr,
        target.m_nodes,
        target.m_count,
        closed,
        substituted);

    if (FAILED(hr))
    {
        target.m_count = 0;
        return hr;
    }

    if (!closed)
    {
        target.m_count = 0;
        return S_FALSE;
    }

    return S_OK;
}

static CLR_RT_GenericInstance *RecordOfTypeSpec(const CLR_RT_TypeSpec_Index &index)
{
    CLR_RT_GenericInstance *record = CLR_RT_TypeSystem::GetGenericInstance(index);

    if (record == nullptr)
    {
        CLR_RT_TypeSpec_Instance inst{};

        if (inst.InitializeFromIndex(index))
        {
            record = CLR_RT_TypeSystem::FindOrCreateGenericInstance(inst);
        }
    }

    return record;
}

HRESULT CLR_RT_TypeSystem::GetElementGenericInstance(
    CLR_RT_Assembly *assm,
    CLR_UINT32 tk,
    const CLR_RT_MethodDef_Instance *contextMethod,
    const CLR_RT_TypeSpec_Index *contextTypeSpec,
    CLR_RT_TypeSpec_Index &element)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_GenericInstance *instance = nullptr;
    CLR_RT_TypeSpec_Index row;
    CLR_RT_TypeSpec_Instance inst{};
    CLR_RT_SignatureParser parser;
    CLR_RT_SignatureParser peek;
    CLR_RT_SignatureParser::Element first;
    HRESULT hr = S_OK;

    element.Clear();

    if (assm == nullptr || CLR_TypeFromTk(tk) != TBL_TypeSpec)
    {
        return S_OK;
    }

    // the row itself, not what the token resolves to in a generic context
    row.Set(assm->assemblyIndex, CLR_DataFromTk(tk));

    if (!inst.InitializeFromIndex(row))
    {
        return S_OK;
    }

    parser.Initialize_TypeSpec(inst);
    peek = parser;

    if (SUCCEEDED(peek.Advance(first)))
    {
        if (first.DataType == DATATYPE_GENERICINST)
        {
            instance = FindOrCreateGenericInstance(inst, contextTypeSpec, contextMethod, &hr);
        }
        else
        {
            CLR_RT_GenericTarget target;
            CLR_RT_MethodSpec_Instance methodSpec{};
            CLR_RT_SignatureParser argument;

            if (first.DataType == DATATYPE_MVAR && first.Levels == 0 && contextMethod != nullptr &&
                NANOCLR_INDEX_IS_VALID(contextMethod->methodSpec) &&
                methodSpec.InitializeFromIndex(contextMethod->methodSpec) &&
                methodSpec.PositionAtGenericArgument(first.GenericParamPosition, argument))
            {
                hr = CollectGenericTarget(argument, contextTypeSpec, nullptr, target);
            }
            else
            {
                hr = CollectGenericTarget(parser, contextTypeSpec, contextMethod, target);
            }

            if (hr == S_OK && target.m_count > 0)
            {
                instance = FindOrCreateGenericInstanceByNodes(target.m_nodes, target.m_count, &hr);
            }
        }
    }

    if (instance != nullptr)
    {
        element = instance->m_self;
    }

    // an element that has no closed instance (open context, not a generic) is an array without it
    if (hr != CLR_E_OUT_OF_MEMORY && hr != CLR_E_NOT_SUPPORTED)
    {
        hr = S_OK;
    }

    return hr;
}

HRESULT CLR_RT_TypeSystem::ResolveArrayRow(
    CLR_RT_Assembly *assm,
    CLR_UINT32 tk,
    const CLR_RT_MethodDef_Instance *contextMethod,
    const CLR_RT_TypeSpec_Index *contextTypeSpec,
    bool &handled,
    CLR_RT_TypeDef_Index &innermost,
    int &levels,
    CLR_RT_TypeSpec_Index &element,
    bool claimWithoutLevels)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_TypeSpec_Index row;
    CLR_RT_TypeSpec_Instance inst{};
    CLR_RT_SignatureParser parser;
    CLR_RT_SignatureParser::Element first;

    handled = false;
    levels = 0;
    innermost.Clear();
    element.Clear();

    if (assm == nullptr || CLR_TypeFromTk(tk) != TBL_TypeSpec)
    {
        return S_OK;
    }

    row.Set(assm->assemblyIndex, CLR_DataFromTk(tk));

    if (!inst.InitializeFromIndex(row))
    {
        return S_OK;
    }

    parser.Initialize_TypeSpec(inst);

    if (FAILED(parser.Advance(first)) || (first.DataType != DATATYPE_VAR && first.DataType != DATATYPE_MVAR))
    {
        return S_OK;
    }

    return ResolveParameterArgument(
        first,
        contextMethod,
        contextTypeSpec,
        handled,
        innermost,
        levels,
        element,
        claimWithoutLevels);
}

HRESULT CLR_RT_TypeSystem::ResolveParameterArgument(
    const CLR_RT_SignatureParser::Element &start,
    const CLR_RT_MethodDef_Instance *contextMethod,
    const CLR_RT_TypeSpec_Index *contextTypeSpec,
    bool &handled,
    CLR_RT_TypeDef_Index &innermost,
    int &levels,
    CLR_RT_TypeSpec_Index &element,
    bool claimWithoutLevels)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_SignatureParser argument;
    CLR_RT_SignatureParser::Element first = start;
    CLR_RT_SignatureParser::Element resolved;
    HRESULT hr = S_OK;

    handled = false;
    levels = 0;
    innermost.Clear();
    element.Clear();

    bool fromMethodSpec = false;

    levels = first.Levels;

    for (int hop = 0;; hop++)
    {
        CLR_RT_MethodSpec_Instance methodSpec{};
        CLR_RT_TypeSpec_Instance owner{};
        CLR_RT_SignatureParser::Element &position = hop == 0 ? first : resolved;

        if (position.DataType == DATATYPE_MVAR)
        {
            if ((hop > 0 && first.DataType == DATATYPE_MVAR) || contextMethod == nullptr ||
                NANOCLR_INDEX_IS_INVALID(contextMethod->methodSpec) ||
                !methodSpec.InitializeFromIndex(contextMethod->methodSpec) ||
                !methodSpec.PositionAtGenericArgument(position.GenericParamPosition, argument))
            {
                levels = 0;
                return S_OK;
            }
        }
        else if (
            contextTypeSpec == nullptr || NANOCLR_INDEX_IS_INVALID(*contextTypeSpec) ||
            !owner.InitializeFromIndex(*contextTypeSpec) ||
            !owner.PositionAtGenericParam(position.GenericParamPosition, argument))
        {
            levels = 0;
            return S_OK;
        }

        fromMethodSpec = position.DataType == DATATYPE_MVAR;

        CLR_RT_SignatureParser peek = argument;

        if (FAILED(peek.Advance(resolved)))
        {
            levels = 0;
            return S_OK;
        }

        levels += resolved.Levels;

        if (resolved.DataType == DATATYPE_VAR && hop == 0)
        {
            continue;
        }

        break;
    }

    // a plain class parameter with a plain argument stays on the TypeDef path
    bool claimed = claimWithoutLevels && (first.DataType == DATATYPE_MVAR || resolved.DataType == DATATYPE_GENERICINST);

    if ((levels == 0 && !claimed) || resolved.DataType == DATATYPE_VAR || resolved.DataType == DATATYPE_MVAR ||
        levels > 0xFF)
    {
        levels = 0;
        return S_OK;
    }

    if (resolved.DataType == DATATYPE_GENERICINST)
    {
        CLR_RT_GenericTarget target;
        CLR_RT_GenericInstance *instance = nullptr;

        hr = CollectGenericTarget(argument, fromMethodSpec ? contextTypeSpec : nullptr, nullptr, target, true);

        if (hr == S_OK && target.m_count > 0)
        {
            instance = FindOrCreateGenericInstanceByNodes(target.m_nodes, target.m_count, &hr);
        }

        if (instance == nullptr)
        {
            levels = 0;

            return (hr == CLR_E_OUT_OF_MEMORY || hr == CLR_E_NOT_SUPPORTED) ? hr : S_OK;
        }

        innermost = target.m_nodes[0].Class;
        element = instance->m_self;
    }
    else if (NANOCLR_INDEX_IS_VALID(resolved.Class))
    {
        innermost = resolved.Class;
    }
    else
    {
        levels = 0;
        return S_OK;
    }

    handled = true;

    return S_OK;
}

// The number of nodes of the subtree rooted at n, 0 when the stream ends inside it.
int CLR_RT_GenericAssignability::SubtreeSize(const CLR_RT_GenericInstanceNode *n, const CLR_RT_GenericInstanceNode *end)
{
    int need = 1;
    int count = 0;

    while (need > 0 && n + count < end)
    {
        need += n[count].ArgCount - 1;
        count++;
    }

    return need == 0 ? count : 0;
}

int CLR_RT_GenericAssignability::VarianceOf(const CLR_RT_TypeDef_Instance &def, int index)
{
    return index < def.target->genericParamCount
               ? (def.assembly->GetGenericParam(def.target->firstGenericParam + index)->flags & 0x3)
               : 0;
}

CLR_RT_GenericInstance *CLR_RT_GenericAssignability::RecordOfSubtree(const CLR_RT_GenericInstanceNode *n, int size)
{
    CLR_RT_GenericInstanceNode copy[CLR_RT_GenericInstance::c_MaxNodes];

    memcpy(copy, n, sizeof(CLR_RT_GenericInstanceNode) * size);
    copy[0].Levels = 0;

    return CLR_RT_TypeSystem::FindOrCreateGenericInstanceByNodes(copy, size);
}

// Two instances of the same generic definition: each argument identical, or compatible through its variance.
bool CLR_RT_GenericAssignability::SameClassAssignable(CLR_RT_GenericRef s, CLR_RT_GenericRef t, int depth)
{
    CLR_RT_TypeDef_Instance def{};
    const CLR_RT_GenericInstanceNode *sa = s.node + 1;
    const CLR_RT_GenericInstanceNode *ta = t.node + 1;

    if (s.node->Class.data != t.node->Class.data || s.node->ArgCount != t.node->ArgCount ||
        !def.InitializeFromIndex(t.node->Class))
    {
        return false;
    }

    for (int i = 0; i < t.node->ArgCount; i++)
    {
        int sSize = SubtreeSize(sa, s.end);
        int tSize = SubtreeSize(ta, t.end);

        if (sSize == 0 || tSize == 0)
        {
            return false;
        }

        if (sSize != tSize || memcmp(sa, ta, sizeof(CLR_RT_GenericInstanceNode) * tSize) != 0)
        {
            int variance = VarianceOf(def, i);
            CLR_RT_GenericRef sArg = {sa, s.end, sa->Levels};
            CLR_RT_GenericRef tArg = {ta, t.end, ta->Levels};

            if (variance == 0 || (variance == 1 ? Assignable(sArg, tArg, false, depth + 1)
                                                : Assignable(tArg, sArg, false, depth + 1)) != GenericAssign_Pass)
            {
                return false;
            }
        }

        sa += sSize;
        ta += tSize;
    }

    return true;
}

// The interfaces and the generic bases of a class, closed level by level, against the target. Pass on the first one
// that is compatible, Fail when some had the target's class and none was, Unknown when none could be read or closed.
CLR_RT_GenericAssign CLR_RT_GenericAssignability::AncestorSearch(
    const CLR_RT_TypeDef_Instance &start,
    const CLR_RT_GenericInstance *context,
    CLR_RT_GenericRef t,
    int depth,
    bool &open)
{
    const CLR_RT_TypeDef_Index &targetClass = t.node->Class;
    CLR_RT_TypeDef_Instance inst = start;
    CLR_RT_TypeDef_Instance targetDef{};
    bool found = false;

    if (!targetDef.InitializeFromIndex(targetClass))
    {
        return GenericAssign_Unknown;
    }

    const bool wantInterface = targetDef.target->IsInterface();

    for (int level = 0; level < c_MaxInheritanceDepth; level++)
    {
        const CLR_RT_TypeSpec_Index *self = context != nullptr ? &context->m_self : nullptr;

        if (wantInterface)
        {
            CLR_RT_SignatureParser parser{};

            parser.Initialize_Interfaces(inst.assembly, inst.target);

            while (parser.Available() > 0)
            {
                CLR_RT_SignatureParser peek = parser;
                CLR_RT_SignatureParser::Element elem;
                CLR_UINT32 tk = CLR_TkFromStream(peek.Signature);

                if (FAILED(parser.Advance(elem)))
                {
                    break;
                }

                if (elem.Class.data != targetClass.data || CLR_TypeFromTk(tk) != TBL_TypeSpec)
                {
                    continue;
                }

                CLR_RT_TypeSpec_Index entryIndex;
                CLR_RT_TypeSpec_Instance entry{};
                CLR_RT_SignatureParser at{};
                CLR_RT_GenericTarget entryTarget;

                entryIndex.Set(inst.assembly->assemblyIndex, CLR_DataFromTk(tk));

                if (!entry.InitializeFromIndex(entryIndex))
                {
                    continue;
                }

                at.Initialize_TypeSpec(entry);

                if (CLR_RT_TypeSystem::CollectGenericTarget(at, self, nullptr, entryTarget) != S_OK ||
                    entryTarget.m_count == 0)
                {
                    open = open || level == 0;
                    continue;
                }

                CLR_RT_GenericRef candidate = {entryTarget.m_nodes, entryTarget.m_nodes + entryTarget.m_count, 0};

                if (SameClassAssignable(candidate, t, depth))
                {
                    return GenericAssign_Pass;
                }

                found = true;
            }
        }

        const bool hasTypeSpecBase = inst.target->HasValidExtendsType() && inst.target->ExtendsIsTypeSpec();
        CLR_RT_GenericInstance *closedBase = CLR_RT_TypeSystem::FindOrCreateGenericBase(inst, self);

        if (!inst.SwitchToParent())
        {
            break;
        }

        if (!wantInterface && inst.data == targetClass.data)
        {
            if (closedBase == nullptr)
            {
                open = open || hasTypeSpecBase;

                return GenericAssign_Unknown;
            }

            CLR_RT_GenericRef candidate = {closedBase->m_nodes, closedBase->m_nodes + closedBase->m_nodeCount, 0};

            return SameClassAssignable(candidate, t, depth) ? GenericAssign_Pass : GenericAssign_Fail;
        }

        context = closedBase;
    }

    return found ? GenericAssign_Fail : GenericAssign_Unknown;
}

// A value of type s stored in a location of type t, both reference types or, for the elements of arrays, value types
// of the same size. Depth is bounded by the nesting of the registry.
CLR_RT_GenericAssign CLR_RT_GenericAssignability::Assignable(
    CLR_RT_GenericRef s,
    CLR_RT_GenericRef t,
    bool element,
    int depth)
{
    CLR_RT_TypeDef_Instance sDef{};
    CLR_RT_TypeDef_Instance tDef{};
    int sSize = SubtreeSize(s.node, s.end);
    int tSize = SubtreeSize(t.node, t.end);
    bool open = false;

    if (sSize == 0 || tSize == 0 || depth > c_MaxGenericNestingDepth + 1 || s.node->Class.data == 0 ||
        t.node->Class.data == 0)
    {
        return GenericAssign_Fail;
    }

    if (s.levels == t.levels && sSize == tSize && s.node->Class.data == t.node->Class.data &&
        (sSize == 1 || memcmp(s.node + 1, t.node + 1, sizeof(CLR_RT_GenericInstanceNode) * (sSize - 1)) == 0))
    {
        return GenericAssign_Pass;
    }

    while (s.levels > 0 && t.levels > 0)
    {
        s.levels--;
        t.levels--;
        element = true;
    }

    if (t.levels > 0 || !tDef.InitializeFromIndex(t.node->Class))
    {
        return GenericAssign_Fail;
    }

    if (s.levels > 0)
    {
        if (tDef.data == g_CLR_RT_WellKnownTypes.Object.data || tDef.data == g_CLR_RT_WellKnownTypes.Array.data)
        {
            return GenericAssign_Pass;
        }

        if (t.node->ArgCount == 0)
        {
            return CLR_RT_ExecutionEngine::IsInstanceOf(g_CLR_RT_WellKnownTypes.Array, t.node->Class)
                       ? GenericAssign_Pass
                       : GenericAssign_Fail;
        }

        if (t.node->ArgCount == 1 && CLR_RT_TypeSystem::IsArrayGenericInterface(tDef))
        {
            CLR_RT_GenericRef elementRef = {t.node + 1, t.end, t.node[1].Levels};

            s.levels--;

            return Assignable(s, elementRef, true, depth + 1);
        }

        return GenericAssign_Fail;
    }

    if (!sDef.InitializeFromIndex(s.node->Class))
    {
        return GenericAssign_Fail;
    }

    if (sDef.target->IsValueTypeOrEnum() || tDef.target->IsValueTypeOrEnum())
    {
        int size = CLR_RT_TypeSystem::IntegralSize(sDef.target->dataType);

        return (element && size != 0 && size == CLR_RT_TypeSystem::IntegralSize(tDef.target->dataType))
                   ? GenericAssign_Pass
                   : GenericAssign_Fail;
    }

    if (tDef.data == g_CLR_RT_WellKnownTypes.Object.data)
    {
        return GenericAssign_Pass;
    }

    if (s.node->Class.data == t.node->Class.data)
    {
        return SameClassAssignable(s, t, depth) ? GenericAssign_Pass : GenericAssign_Fail;
    }

    if (t.node->ArgCount == 0)
    {
        return CLR_RT_ExecutionEngine::IsInstanceOf(s.node->Class, t.node->Class) ? GenericAssign_Pass
                                                                                  : GenericAssign_Fail;
    }

    CLR_RT_GenericInstance *record = nullptr;

    if (s.node->ArgCount > 0)
    {
        record = RecordOfSubtree(s.node, sSize);

        if (record == nullptr)
        {
            return GenericAssign_Fail;
        }
    }

    CLR_RT_GenericAssign result = AncestorSearch(sDef, record, t, depth, open);

    // a hierarchy that was read completely and doesn't hold the target is a definite answer
    return (result == GenericAssign_Unknown && !open) ? GenericAssign_Fail : result;
}

#if CONFIG_NF_FEATURE_SUPPORT_REFLECTION
bool CLR_RT_GenericAssignability::IsVariantAssignable(
    const CLR_RT_GenericInstance &source,
    const CLR_RT_GenericInstance &target)
{
    CLR_RT_GenericRef s = {source.m_nodes, source.m_nodes + source.m_nodeCount, 0};
    CLR_RT_GenericRef t = {target.m_nodes, target.m_nodes + target.m_nodeCount, 0};

    return ((source.m_flags | target.m_flags) & CLR_RT_GenericInstance::c_Open) == 0 && SameClassAssignable(s, t, 0);
}

bool CLR_RT_GenericAssignability::IsVariantWord(
    const CLR_RT_ReflectionDef_Index &source,
    const CLR_RT_ReflectionDef_Index &target)
{
    if (source.kind != REFLECTION_TYPESPEC || target.kind != REFLECTION_TYPESPEC || source.levels != 0 ||
        target.levels != 0)
    {
        return false;
    }

    CLR_RT_GenericInstance *s = RecordOfTypeSpec(source.data.typeSpec);
    CLR_RT_GenericInstance *t = RecordOfTypeSpec(target.data.typeSpec);

    return s != nullptr && t != nullptr && IsVariantAssignable(*s, *t);
}
#endif

bool CLR_RT_TypeSystem::IsElementInstanceAssignable(
    const CLR_RT_TypeSpec_Index &actual,
    const CLR_RT_TypeSpec_Index &requested)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_GenericInstance *current = RecordOfTypeSpec(actual);
    CLR_RT_GenericInstance *wanted = RecordOfTypeSpec(requested);

    if (IsSameGenericInstance(actual, requested) || current == nullptr || wanted == nullptr)
    {
        return true;
    }

    CLR_RT_GenericRef s = {current->m_nodes, current->m_nodes + current->m_nodeCount, 0};
    CLR_RT_GenericRef t = {wanted->m_nodes, wanted->m_nodes + wanted->m_nodeCount, 0};

    return CLR_RT_GenericAssignability::Assignable(s, t, true, 0) != GenericAssign_Fail;
}

CLR_RT_GenericMatch CLR_RT_GenericAssignability::MatchObjectNodes(
    const CLR_RT_HeapBlock &ref,
    const CLR_RT_GenericInstanceNode *nodes,
    int count)
{
    const CLR_RT_HeapBlock *obj = &ref;
    CLR_RT_GenericInstance *own = nullptr;
    CLR_RT_TypeDef_Instance inst{};
    CLR_RT_GenericRef t = {nodes, nodes + count, 0};
    bool open = false;
    CLR_RT_GenericAssign result;

    while (obj->DataType() == DATATYPE_OBJECT || obj->DataType() == DATATYPE_BYREF)
    {
        obj = obj->Dereference();

        if (obj == nullptr)
        {
            return GenericMatch_Unresolved;
        }
    }

    if (count == 0)
    {
        return GenericMatch_Unresolved;
    }

    if (obj->DataType() == DATATYPE_SZARRAY)
    {
        const CLR_RT_ReflectionDef_Index &reflex = obj->ReflectionDataConst();
        const CLR_RT_TypeSpec_Index *elementSpec = ((const CLR_RT_HeapBlock_Array *)obj)->ElementTypeSpec();
        CLR_RT_GenericInstance *element = elementSpec != nullptr ? RecordOfTypeSpec(*elementSpec) : nullptr;
        CLR_RT_GenericInstanceNode single{};
        CLR_RT_GenericRef s;

        if (nodes[0].ArgCount != 1 || count < 2 || reflex.levels == 0)
        {
            return GenericMatch_Unresolved;
        }

        if (element != nullptr)
        {
            s = {element->m_nodes, element->m_nodes + element->m_nodeCount, reflex.levels};
        }
        else
        {
            if (!inst.InitializeFromIndex(reflex.data.type))
            {
                return GenericMatch_Unresolved;
            }

            if (inst.target->genericParamCount > 0)
            {
                return GenericMatch_NoTypeSpec;
            }

            single.Class = reflex.data.type;
            s = {&single, &single + 1, reflex.levels};
        }

        result = Assignable(s, t, false, 0);

        return result == GenericAssign_Pass
                   ? GenericMatch_Same
                   : (result == GenericAssign_Fail ? GenericMatch_Different : GenericMatch_Unresolved);
    }

    if ((obj->DataType() != DATATYPE_CLASS && obj->DataType() != DATATYPE_VALUETYPE) ||
        !inst.InitializeFromIndex(obj->ObjectCls()))
    {
        return GenericMatch_Unresolved;
    }

    // A stored TypeSpec of another class is the context of whoever created the object, not its own.
    if (obj->IsAGenericInstance())
    {
        CLR_RT_TypeSpec_Instance stored{};

        if (stored.InitializeFromIndex(obj->ObjectGenericType()))
        {
            own = CLR_RT_TypeSystem::FindOrCreateGenericInstance(stored);

            if (own != nullptr && own->m_nodes[0].Class.data != inst.data)
            {
                own = nullptr;
            }
        }
    }

    if (inst.data == nodes[0].Class.data)
    {
        if (own == nullptr)
        {
            return GenericMatch_NoTypeSpec;
        }

        if (own->m_nodeCount == count && memcmp(own->m_nodes, nodes, sizeof(CLR_RT_GenericInstanceNode) * count) == 0)
        {
            return GenericMatch_Same;
        }

        CLR_RT_GenericRef s = {own->m_nodes, own->m_nodes + own->m_nodeCount, 0};

        return SameClassAssignable(s, t, 0) ? GenericMatch_Same : GenericMatch_Different;
    }

    result = AncestorSearch(inst, own, t, 0, open);

    if (result == GenericAssign_Unknown)
    {
        return open ? GenericMatch_NoTypeSpec : GenericMatch_Unresolved;
    }

    return result == GenericAssign_Pass ? GenericMatch_Same : GenericMatch_Different;
}

bool CLR_RT_TypeSystem::ElementMatchesArraySpec(const CLR_RT_HeapBlock &object, const CLR_RT_TypeSpec_Index &element)
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_GenericInstance *record = RecordOfTypeSpec(element);
    const CLR_RT_HeapBlock *obj = &object;

    if (record == nullptr)
    {
        return true;
    }

    while (obj != nullptr && (obj->DataType() == DATATYPE_OBJECT || obj->DataType() == DATATYPE_BYREF))
    {
        obj = obj->Dereference();
    }

    // an element that is an array itself: its own element instance is what counts
    if (obj != nullptr && obj->DataType() == DATATYPE_SZARRAY)
    {
        const CLR_RT_TypeSpec_Index *actual = ((const CLR_RT_HeapBlock_Array *)obj)->ElementTypeSpec();

        return actual == nullptr || IsElementInstanceAssignable(*actual, element);
    }

    return CLR_RT_GenericAssignability::MatchObjectNodes(object, record->m_nodes, record->m_nodeCount) !=
           GenericMatch_Different;
}

CLR_RT_GenericMatch CLR_RT_TypeSystem::MatchGenericArguments(
    const CLR_RT_HeapBlock &ref,
    const CLR_RT_GenericTarget &target)
{
    NATIVE_PROFILE_CLR_CORE();

    return CLR_RT_GenericAssignability::MatchObjectNodes(ref, target.m_nodes, target.m_count);
}

HRESULT CLR_RT_TypeSystem::AllocateGenericInstanceStatics(CLR_RT_GenericInstance &instance)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Instance owner{};
    CLR_RT_TypeSpec_Instance self{};
    CLR_RT_FieldDef_Index *fieldDefs;
    CLR_RT_HeapBlock *fields;
    CLR_UINT32 count;

    if (instance.m_fields != nullptr)
    {
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    if (!owner.InitializeFromIndex(instance.m_nodes[0].Class))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    count = owner.target->staticFieldsCount;

    if (count == 0)
    {
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    if (!self.InitializeFromIndex(instance.m_self))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    // platform_malloc first: nothing to unwind if it fails, unlike the GC heap blob.
    fieldDefs = (CLR_RT_FieldDef_Index *)platform_malloc(sizeof(CLR_RT_FieldDef_Index) * count);

    if (fieldDefs == nullptr)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    fields = CLR_AllocateGenericStaticFieldStorage(count);

    if (fields == nullptr)
    {
        platform_free(fieldDefs);
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    for (CLR_UINT32 i = 0; i < count; i++)
    {
        fieldDefs[i].Set(owner.assembly->assemblyIndex, owner.target->firstStaticField + i);
    }

    // Published before stamping, so the GC marks the blob if stamping allocates.
    instance.m_fields = fields;
    instance.m_fieldDefs = fieldDefs;
    instance.m_count = (CLR_UINT16)count;

    for (CLR_UINT32 i = 0; i < count; i++)
    {
        const CLR_RECORD_FIELDDEF *pFd = owner.assembly->GetFieldDef(owner.target->firstStaticField + i);

        hr = g_CLR_RT_ExecutionEngine.InitializeReference(fields[i], pFd, owner.assembly, &self);

        if (FAILED(hr))
        {
            // An event block is never collected, so the blob is released here.
            instance.m_fields = nullptr;
            instance.m_fieldDefs = nullptr;
            instance.m_count = 0;
            CLR_ReleaseGenericStaticFieldStorage(fields);
            platform_free(fieldDefs);
            NANOCLR_LEAVE();
        }
    }

    NANOCLR_NOCLEANUP();
}

void CLR_RT_TypeSystem::TruncateGenericInstances(CLR_UINT32 count)
{
    if (m_genericInstancesCount <= count)
    {
        return;
    }

    NANOCLR_FOREACH_ASSEMBLY(*this)
    {
        for (int i = 0; i < pASSM->tablesSize[TBL_TypeSpec]; i++)
        {
            CLR_RT_GenericInstance *&linked = pASSM->crossReferenceTypeSpec[i].genericInstance;

            for (CLR_UINT32 j = count; linked != nullptr && j < m_genericInstancesCount; j++)
            {
                if (linked == m_genericInstances[j])
                {
                    linked = nullptr;
                }
            }
        }
    }
    NANOCLR_FOREACH_ASSEMBLY_END();

    while (m_genericInstancesCount > count)
    {
        CLR_RT_GenericInstance *instance = m_genericInstances[--m_genericInstancesCount];

        if (instance->m_fields != nullptr)
        {
            CLR_ReleaseGenericStaticFieldStorage(instance->m_fields);
        }

        if (instance->m_fieldDefs != nullptr)
        {
            platform_free(instance->m_fieldDefs);
        }

        platform_free(instance);
    }
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

                if (dt >= DATATYPE_FIRST_INVALID)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
                }

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
                else if (dt == DATATYPE_SZARRAY)
                {
                    NANOCLR_CHECK_HRESULT(ReadArrayValue(currentParam));
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
        else if (m_res.DataType == DATATYPE_SZARRAY)
        {
            // single parameter of array type
            // OK to skip element type
            m_blob += sizeof(CLR_UINT8);

            CLR_RT_HeapBlock *currentParam = &m_lastValue.m_value;
            NANOCLR_CHECK_HRESULT(ReadArrayValue(currentParam));
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
        else if (m_res.DataType == DATATYPE_SZARRAY)
        {
            CLR_RT_HeapBlock *currentParam = &m_lastValue.m_value;
            NANOCLR_CHECK_HRESULT(ReadArrayValue(currentParam));
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }
    }

    m_lastValue.m_pos = m_currentPos++;

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_AttributeParser::ReadArrayValue(CLR_RT_HeapBlock *&value)
{
    NANOCLR_HEADER();

    CLR_UINT8 elementType;
    CLR_UINT8 elementCount;
    const CLR_RT_DataTypeLookup *dtl;

    NANOCLR_READ_UNALIGNED_UINT8(elementType, m_blob);
    NANOCLR_READ_UNALIGNED_UINT8(elementCount, m_blob);

    NanoCLRDataType dt = CLR_RT_TypeSystem::MapElementTypeToDataType(elementType);

    if (dt >= DATATYPE_FIRST_INVALID)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    dtl = &c_CLR_RT_DataTypeLookup[dt];

    if (dtl->m_flags & CLR_RT_DataTypeLookup::c_Numeric)
    {
        CLR_UINT32 size = dtl->m_sizeInBytes;

        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(*value, elementCount, *dtl->m_cls));

        CLR_UINT8 *arrayData = value->DereferenceArray()->GetFirstElement();

        if (dt == DATATYPE_BOOLEAN)
        {
            for (CLR_UINT8 i = 0; i < elementCount; i++)
            {
                CLR_UINT8 boolValue;
                NANOCLR_READ_UNALIGNED_UINT8(boolValue, m_blob);

                arrayData[i] = boolValue ? 1 : 0;
            }
        }
        else
        {
            memcpy(arrayData, m_blob, size * elementCount);
            m_blob += size * elementCount;
        }
    }
    else if (dt == DATATYPE_STRING)
    {
        NANOCLR_CHECK_HRESULT(
            CLR_RT_HeapBlock_Array::CreateInstance(*value, elementCount, g_CLR_RT_WellKnownTypes.String));

        auto *currentElement = (CLR_RT_HeapBlock *)value->DereferenceArray()->GetFirstElement();

        for (CLR_UINT8 i = 0; i < elementCount; i++)
        {
            NANOCLR_CHECK_HRESULT(ReadString(currentElement));
            currentElement++;
        }
    }
    else
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

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
