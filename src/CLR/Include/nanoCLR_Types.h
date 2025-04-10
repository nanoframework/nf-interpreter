//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef NANOCLR_TYPES_H
#define NANOCLR_TYPES_H

#include <nanoCLR_PlatformDef.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

#define FAULT_ON_NULL_HR(ptr, hr)                                                                                      \
    if (!(ptr))                                                                                                        \
    NANOCLR_SET_AND_LEAVE((hr))
#define CHECK_ALLOCATION(ptr)                                                                                          \
    if (!(ptr))                                                                                                        \
    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY)
#define CHECK_SIZE_RANGE(len)                                                                                          \
    if (len > CLR_RT_HeapBlock::HB_MaxSize)                                                                            \
    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE)

#define FIMPLIES(x, y)        ((!(x)) || (y))
#define FINRANGE(val, lo, hi) (((val) >= (lo)) && ((val) <= (hi)))

///////////////////////////////////////////////////////////////////////////////////////////////////

#define NANOCLR_INTEROP_STUB_RETURN(stack) return stack.NotImplementedStub()
#define NANOCLR_SYSTEM_STUB_RETURN()       return S_OK
#define NANOCLR_FEATURE_STUB_RETURN()      return CLR_E_NOTIMPL

//////////////////////////////////////////////////

#define NANOCLR_PARAMCHECK_BEGIN()                                                                                     \
    {                                                                                                                  \
        HRESULT hrInner = S_OK;

#define NANOCLR_PARAMCHECK_POINTER(ptr)                                                                                \
    {                                                                                                                  \
        if (ptr == nullptr)                                                                                            \
        {                                                                                                              \
            hrInner = CLR_E_NULL_REFERENCE;                                                                            \
        }                                                                                                              \
    }

#define NANOCLR_PARAMCHECK_POINTER_AND_SET(ptr, val)                                                                   \
    {                                                                                                                  \
        if (ptr == nullptr)                                                                                            \
        {                                                                                                              \
            hrInner = CLR_E_NULL_REFERENCE;                                                                            \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            *ptr = val;                                                                                                \
        }                                                                                                              \
    }

#define NANOCLR_PARAMCHECK_NOTNULL(ptr)                                                                                \
    {                                                                                                                  \
        if (ptr == nullptr)                                                                                            \
        {                                                                                                              \
            hrInner = CLR_E_INVALID_PARAMETER;                                                                         \
        }                                                                                                              \
    }

#define NANOCLR_PARAMCHECK_STRING_NOT_EMPTY(ptr)                                                                       \
    {                                                                                                                  \
        if (ptr == nullptr || ptr[0] == 0)                                                                             \
        {                                                                                                              \
            hrInner = CLR_E_INVALID_PARAMETER;                                                                         \
        }                                                                                                              \
    }

#define NANOCLR_PARAMCHECK_END()                                                                                       \
    {                                                                                                                  \
        NANOCLR_CHECK_HRESULT(hrInner);                                                                                \
    }                                                                                                                  \
    }

///////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma pack(push, __NANOCLR_TYPES_H__, 4)
#endif

enum CLR_OPCODE
{
#define OPDEF(c, s, pop, push, args, type, l, s1, s2, ctrl) c,
#include "opcode.def"
#undef OPDEF
    CEE_COUNT, /* number of instructions and macros pre-defined */
};

#if defined(__GNUC__)
#define __int64 long long
#endif

typedef unsigned char CLR_UINT8;
typedef unsigned short CLR_UINT16;
typedef unsigned int CLR_UINT32;
typedef unsigned __int64 CLR_UINT64;
typedef signed char CLR_INT8;
typedef signed short CLR_INT16;
typedef signed int CLR_INT32;
typedef signed __int64 CLR_INT64;

typedef CLR_UINT16 CLR_OFFSET;
typedef CLR_UINT32 CLR_OFFSET_LONG;
typedef CLR_UINT16 CLR_INDEX;
typedef CLR_UINT16 CLR_STRING;
typedef CLR_UINT16 CLR_SIG;
typedef const CLR_UINT8 *CLR_PMETADATA;
typedef CLR_UINT16 CLR_TYPEORMETHODDEF;
typedef CLR_UINT16 CLR_ENCODEDNANOTYPE;
typedef CLR_UINT16 CLR_EncodedMethodDefOrRef;
typedef CLR_UINT16 CLR_EncodedTypeDefOrRef;
typedef CLR_UINT16 CLR_EncodedTypeRefOrSpec;

//--//
// may need to change later
typedef CLR_INT64 CLR_INT64_TEMP_CAST;
typedef CLR_UINT64 CLR_UINT64_TEMP_CAST;

#if !defined(NANOCLR_EMULATED_FLOATINGPOINT)
typedef double CLR_DOUBLE_TEMP_CAST;
#else
typedef CLR_INT64 CLR_DOUBLE_TEMP_CAST;
#endif

#define CLR_SIG_INVALID 0xFFFF

enum CLR_LOGICAL_OPCODE
{
    LO_Not = 0x00,
    LO_And = 0x01,
    LO_Or = 0x02,
    LO_Xor = 0x03,
    LO_Shl = 0x04,
    LO_Shr = 0x05,

    LO_Neg = 0x06,
    LO_Add = 0x07,
    LO_Sub = 0x08,
    LO_Mul = 0x09,
    LO_Div = 0x0A,
    LO_Rem = 0x0B,

    LO_Box = 0x0C,
    LO_Unbox = 0x0D,

    LO_Branch = 0x0E,
    LO_Set = 0x0F,
    LO_Switch = 0x10,

    LO_LoadFunction = 0x11,
    LO_LoadVirtFunction = 0x12,

    LO_Call = 0x13,
    LO_CallVirt = 0x14,
    LO_Ret = 0x15,

    LO_NewObject = 0x16,
    LO_CastClass = 0x17,
    LO_IsInst = 0x18,

    LO_Dup = 0x19,
    LO_Pop = 0x1A,

    LO_Throw = 0x1B,
    LO_Rethrow = 0x1C,
    LO_Leave = 0x1D,
    LO_EndFinally = 0x1E,

    LO_Convert = 0x1F,

    LO_StoreArgument = 0x20,
    LO_LoadArgument = 0x21,
    LO_LoadArgumentAddress = 0x22,

    LO_StoreLocal = 0x23,
    LO_LoadLocal = 0x24,
    LO_LoadLocalAddress = 0x25,

    LO_LoadConstant_I4 = 0x26,
    LO_LoadConstant_I8 = 0x27,
    LO_LoadConstant_R4 = 0x28,
    LO_LoadConstant_R8 = 0x29,

    LO_LoadNull = 0x2A,
    LO_LoadString = 0x2B,
    LO_LoadToken = 0x2C,

    LO_NewArray = 0x2D,
    LO_LoadLength = 0x2E,

    LO_StoreElement = 0x2F,
    LO_LoadElement = 0x30,
    LO_LoadElementAddress = 0x31,

    LO_StoreField = 0x32,
    LO_LoadField = 0x33,
    LO_LoadFieldAddress = 0x34,

    LO_StoreStaticField = 0x35,
    LO_LoadStaticField = 0x36,
    LO_LoadStaticFieldAddress = 0x37,

    LO_StoreIndirect = 0x38,
    LO_LoadIndirect = 0x39,

    LO_InitObject = 0x3A,
    LO_LoadObject = 0x3B,
    LO_CopyObject = 0x3C,
    LO_StoreObject = 0x3D,

    LO_Nop = 0x3E,

    LO_EndFilter = 0x3F,

    LO_Unsupported = 0x40,

    LO_FIRST = LO_Not,
    LO_LAST = LO_EndFilter,
};

///////////////////////////////////////////////////////////////////////////////////////////////////

static const CLR_INDEX CLR_EmptyIndex = 0xFFFF;
static const CLR_UINT32 CLR_EmptyToken = 0xFFFFFFFF;
static const size_t CLR_MaxStreamSize_AssemblyRef = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_TypeRef = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_FieldRef = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_MethodRef = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_TypeDef = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_FieldDef = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_MethodDef = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_GenericParam = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_MethodSpec = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_Attributes = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_TypeSpec = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_Resources = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_ResourcesData = 0xFFFFFFFF;
static const size_t CLR_MaxStreamSize_Strings = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_Signatures = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_ByteCode = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_ResourcesFiles = 0x0000FFFF;

enum CLR_OpcodeParam
{
    CLR_OpcodeParam_Field = 0,
    CLR_OpcodeParam_Method = 1,
    CLR_OpcodeParam_Type = 2,
    CLR_OpcodeParam_String = 3,
    CLR_OpcodeParam_Tok = 4,
    CLR_OpcodeParam_Sig = 5,
    CLR_OpcodeParam_BrTarget = 6,
    CLR_OpcodeParam_ShortBrTarget = 7,
    CLR_OpcodeParam_I = 8,
    CLR_OpcodeParam_I8 = 9,
    CLR_OpcodeParam_None = 10,
    CLR_OpcodeParam_R = 11,
    CLR_OpcodeParam_Switch = 12,
    CLR_OpcodeParam_Var = 13,
    CLR_OpcodeParam_ShortI = 14,
    CLR_OpcodeParam_ShortR = 15,
    CLR_OpcodeParam_ShortVar = 16,
};

#define CanCompressOpParamToken(opParam) (opParam >= CLR_OpcodeParam_Field && opParam <= CLR_OpcodeParam_String)
#define IsOpParamToken(opParam)          (opParam >= CLR_OpcodeParam_Field && opParam <= CLR_OpcodeParam_Sig)

//--//

enum CLR_FlowControl
{
    CLR_FlowControl_NEXT = 0,
    CLR_FlowControl_CALL = 1,
    CLR_FlowControl_RETURN = 2,
    CLR_FlowControl_BRANCH = 3,
    CLR_FlowControl_COND_BRANCH = 4,
    CLR_FlowControl_THROW = 5,
    CLR_FlowControl_BREAK = 6,
    CLR_FlowControl_META = 7,
};

//--//

#define c_CLR_StringTable_Version 1

///////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH enum NanoCLRTable (in nanoCLR_TypeSystem VS extension) !!! //
// !!! KEEP IN SYNC WITH enum NanoCLRTable (in MDP)                             !!! //
//////////////////////////////////////////////////////////////////////////////////////
enum NanoCLRTable
{
    TBL_AssemblyRef = 0x00000000,
    TBL_TypeRef = 0x00000001,
    TBL_FieldRef = 0x00000002,
    TBL_MethodRef = 0x00000003,
    TBL_TypeDef = 0x00000004,
    TBL_FieldDef = 0x00000005,
    TBL_MethodDef = 0x00000006,
    TBL_GenericParam = 0x00000007,
    TBL_MethodSpec = 0x00000008,
    TBL_TypeSpec = 0x00000009,
    TBL_Attributes = 0x0000000A,
    TBL_Resources = 0x0000000B,
    TBL_ResourcesData = 0x0000000C,
    TBL_Strings = 0x0000000D,
    TBL_Signatures = 0x0000000E,
    TBL_ByteCode = 0x0000000F,
    TBL_ResourcesFiles = 0x00000010,
    TBL_EndOfAssembly = 0x000000011,
    TBL_Max = 0x00000012,
};

enum CLR_CorCallingConvention
{
    /////////////////////////////////////////////////////////////////////////////////////////////
    //
    // This is based on CorCallingConvention.
    //

    PIMAGE_CEE_CS_CALLCONV_DEFAULT = 0x0,

    PIMAGE_CEE_CS_CALLCONV_VARARG = 0x5,
    PIMAGE_CEE_CS_CALLCONV_FIELD = 0x6,
    PIMAGE_CEE_CS_CALLCONV_LOCAL_SIG = 0x7,
    PIMAGE_CEE_CS_CALLCONV_PROPERTY = 0x8,
    PIMAGE_CEE_CS_CALLCONV_UNMGD = 0x9,

    /// @brief generic method instantiation
    ///
    PIMAGE_CEE_CS_CALLCONV_GENERICINST = 0xA,

    /// @brief used ONLY for 64bit vararg PInvoke calls
    ///
    PIMAGE_CEE_CS_CALLCONV_NATIVEVARARG = 0xB,

    /// @brief first invalid calling convention
    ///
    PIMAGE_CEE_CS_CALLCONV_MAX = 0xC,

    // The high bits of the calling convention convey additional info

    /// @brief Calling convention is bottom 4 bits
    ///
    PIMAGE_CEE_CS_CALLCONV_MASK = 0x0F,

    /// @brief Top bit indicates a 'this' parameter
    ///
    PIMAGE_CEE_CS_CALLCONV_HASTHIS = 0x20,

    /// @brief This parameter is explicitly in the signature
    ///
    PIMAGE_CEE_CS_CALLCONV_EXPLICITTHIS = 0x40,

    /// @brief Generic method sig with explicit number of type arguments (precedes ordinary parameter count)
    ///
    PIMAGE_CEE_CS_CALLCONV_GENERIC = 0x10,

    //
    // End of overlap with CorCallingConvention.
    //
    /////////////////////////////////////////////////////////////////////////////////////////////
};

/////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH enum NanoCLRDataType (in nanoCLR_TypeSystem VS extension) !!! //
// !!! KEEP IN SYNC WITH enum NanoCLRDataType (in nanoCLR_TypeSystem Debugger)     !!! //
/////////////////////////////////////////////////////////////////////////////////////////

enum NanoCLRDataType // KEEP IN SYNC WITH nanoCLR_DataType enum in nanoFramework.Tools.MetadataProcessor!!
{
    /// @brief 0 bytes
    DATATYPE_VOID,

    /// @brief  1 byte
    DATATYPE_BOOLEAN,

    /// @brief 1 byte
    DATATYPE_I1,

    /// @brief 1 byte
    DATATYPE_U1,

    /// @brief 2 bytes
    DATATYPE_CHAR,

    /// @brief 2 bytes
    DATATYPE_I2,

    /// @brief 2 bytes
    DATATYPE_U2,

    /// @brief 4 bytes
    DATATYPE_I4,

    /// @brief 4 bytes
    DATATYPE_U4,

    /// @brief 4 bytes
    DATATYPE_R4,

    /// @brief 8 bytes
    DATATYPE_I8,

    /// @brief 8 bytes
    DATATYPE_U8,

    /// @brief 8 bytes
    DATATYPE_R8,

    /// @brief 8 bytes (Shortcut for System.DateTime)
    DATATYPE_DATETIME,

    /// @brief  8 bytes (Shortcut for System.TimeSpan)
    DATATYPE_TIMESPAN,
    DATATYPE_STRING,

    /// @brief  This is the last type that doesn't need to be relocated.
    DATATYPE_LAST_NONPOINTER = DATATYPE_TIMESPAN,

    //  All the above types don't need fix-up on assignment.
    DATATYPE_LAST_PRIMITIVE_TO_PRESERVE = DATATYPE_R8,

// All the above types can be marshaled by assignment.
#if defined(NANOCLR_NO_ASSEMBLY_STRINGS)
    DATATYPE_LAST_PRIMITIVE_TO_MARSHAL = DATATYPE_STRING, //
#else
    DATATYPE_LAST_PRIMITIVE_TO_MARSHAL = DATATYPE_TIMESPAN,
#endif

    // All the above types don't need fix-up on assignment.
    DATATYPE_LAST_PRIMITIVE = DATATYPE_STRING,

    /// @brief Shortcut for System.Object
    DATATYPE_OBJECT,

    /// @brief CLASS <class Token>
    DATATYPE_CLASS,

    /// @brief VALUETYPE <class Token>
    DATATYPE_VALUETYPE,

    /// @brief Shortcut for single dimension zero lower bound array SZARRAY <type>
    DATATYPE_SZARRAY,

    /// @brief BYREF <type>
    DATATYPE_BYREF,

    /// @brief Generic parameter in a generic type definition, represented as number
    DATATYPE_VAR,
    /// @brief Generic type instantiation
    DATATYPE_GENERICINST,
    /// @brief Generic parameter in a generic method definition, represented as number
    DATATYPE_MVAR,

    ////////////////////////////////////////

    DATATYPE_FREEBLOCK,
    DATATYPE_CACHEDBLOCK,
    DATATYPE_ASSEMBLY,
    DATATYPE_WEAKCLASS,
    DATATYPE_REFLECTION,
    DATATYPE_ARRAY_BYREF,
    DATATYPE_DELEGATE_HEAD,
    DATATYPE_DELEGATELIST_HEAD,
    DATATYPE_OBJECT_TO_EVENT,
    DATATYPE_BINARY_BLOB_HEAD,

    DATATYPE_THREAD,
    DATATYPE_SUBTHREAD,
    DATATYPE_STACK_FRAME,
    DATATYPE_TIMER_HEAD,
    DATATYPE_LOCK_HEAD,
    DATATYPE_LOCK_OWNER_HEAD,
    DATATYPE_LOCK_REQUEST_HEAD,
    DATATYPE_WAIT_FOR_OBJECT_HEAD,
    DATATYPE_FINALIZER_HEAD,
    DATATYPE_MEMORY_STREAM_HEAD, // SubDataType?
    DATATYPE_MEMORY_STREAM_DATA, // SubDataType?

    DATATYPE_SERIALIZER_HEAD,      // SubDataType?
    DATATYPE_SERIALIZER_DUPLICATE, // SubDataType?
    DATATYPE_SERIALIZER_STATE,     // SubDataType?

    DATATYPE_ENDPOINT_HEAD,

    // These constants are shared by Debugger.dll, and cannot be conditionally compiled away.
    // This adds a couple extra bytes to the lookup table.  But frankly, the lookup table should probably
    // be shrunk to begin with.  Most of the datatypes are used just to tag memory.
    // For those datatypes, perhaps we should use a subDataType instead (probably what the comments above are about).

    DATATYPE_RADIO_LAST = DATATYPE_ENDPOINT_HEAD + 3,

    DATATYPE_IO_PORT,
    DATATYPE_IO_PORT_LAST = DATATYPE_RADIO_LAST + 1,

    DATATYPE_VTU_PORT_LAST = DATATYPE_IO_PORT_LAST + 1,

#if defined(NANOCLR_APPDOMAINS)
    DATATYPE_APPDOMAIN_HEAD,
    DATATYPE_TRANSPARENT_PROXY,
    DATATYPE_APPDOMAIN_ASSEMBLY,
#endif
    DATATYPE_APPDOMAIN_LAST = DATATYPE_VTU_PORT_LAST + 3,

    DATATYPE_FIRST_INVALID,

    // Type modifies. This is exact copy of VALUES ELEMENT_TYPE_* from CorHdr.h
    //

    DATATYPE_TYPE_MODIFIER = 0x40,
    DATATYPE_TYPE_SENTINEL = 0x01 | DATATYPE_TYPE_MODIFIER, // sentinel for varargs
    DATATYPE_TYPE_PINNED = 0x05 | DATATYPE_TYPE_MODIFIER,
    DATATYPE_TYPE_R4_HFA = 0x06 | DATATYPE_TYPE_MODIFIER, // used only internally for R4 HFA types
    DATATYPE_TYPE_R8_HFA = 0x07 | DATATYPE_TYPE_MODIFIER, // used only internally for R8 HFA types
};

enum CLR_ReflectionType
{
    REFLECTION_INVALID = 0x00,
    REFLECTION_ASSEMBLY = 0x01,
    REFLECTION_TYPE = 0x02,
    REFLECTION_TYPE_DELAYED = 0x03,
    REFLECTION_CONSTRUCTOR = 0x04,
    REFLECTION_METHOD = 0x05,
    REFLECTION_FIELD = 0x06,
    REFLECTION_GENERICTYPE = 0x07,
};

////////////////////////////////////////////////////////////////////////////////////////////////////

inline CLR_UINT32 CLR_DataFromTk(CLR_UINT32 tk)
{
    return tk & 0x00FFFFFF;
}
inline NanoCLRTable CLR_TypeFromTk(CLR_UINT32 tk)
{
    return (NanoCLRTable)(tk >> 24);
}
inline CLR_UINT32 CLR_TkFromType(NanoCLRTable tbl, CLR_UINT32 data)
{
    return ((((CLR_UINT32)tbl) << 24) & 0xFF000000) | (data & 0x00FFFFFF);
}

//--//

inline CLR_UINT32 CLR_UncompressStringToken(CLR_UINT32 tk)
{
    return CLR_TkFromType(TBL_Strings, tk);
}

inline CLR_UINT32 CLR_UncompressTypeToken(CLR_UINT32 tk)
{
    static const NanoCLRTable c_lookup[] = {TBL_TypeDef, TBL_TypeRef, TBL_TypeSpec, TBL_GenericParam};
    return CLR_TkFromType(c_lookup[(tk >> 14)], 0x3fff & tk);
}

inline CLR_UINT32 CLR_UncompressFieldToken(CLR_UINT32 tk)
{
    static const NanoCLRTable c_lookup[2] = {TBL_FieldDef, TBL_FieldRef};
    return CLR_TkFromType(c_lookup[(tk >> 15)], 0x7fff & tk);
}

inline CLR_UINT32 CLR_UncompressMethodToken(CLR_UINT32 tk)
{
    static const NanoCLRTable c_lookup[4] = {TBL_MethodDef, TBL_MethodRef, TBL_TypeSpec, TBL_MethodSpec};

    return CLR_TkFromType(c_lookup[(tk >> 14)], 0x3fff & tk);
}

#if defined(VIRTUAL_DEVICE)

CLR_UINT32 CLR_ReadTokenCompressed(CLR_PMETADATA &ip, CLR_OPCODE opcode);

#endif

//--//

inline bool CLR_CompressData(CLR_UINT32 val, CLR_UINT8 *&p)
{
    CLR_UINT8 *ptr = p;

    if (val <= 0x7F)
    {
        *ptr++ = (CLR_UINT8)(val);
    }
    else if (val <= 0x3FFF)
    {
        *ptr++ = (CLR_UINT8)((val >> 8) | 0x80);
        *ptr++ = (CLR_UINT8)((val));
    }
    else if (val <= 0x1FFFFFFF)
    {
        *ptr++ = (CLR_UINT8)((val >> 24) | 0xC0);
        *ptr++ = (CLR_UINT8)((val >> 16));
        *ptr++ = (CLR_UINT8)((val >> 8));
        *ptr++ = (CLR_UINT8)((val));
    }
    else
    {
        return false;
    }

    p = ptr;

    return true;
}

inline CLR_UINT32 CLR_UncompressData(const CLR_UINT8 *&p)
{
    CLR_PMETADATA ptr = p;
    CLR_UINT32 val = *ptr++;
    // Handle smallest data inline.
    if ((val & 0x80) == 0x00) // 0??? ????
    {
    }
    else if ((val & 0xC0) == 0x80) // 10?? ????
    {
        val = (val & 0x3F) << 8;
        val |= (CLR_UINT32)*ptr++;
    }
    else // 110? ????
    {
        val = (val & 0x1F) << 24;
        val |= (CLR_UINT32)*ptr++ << 16;
        val |= (CLR_UINT32)*ptr++ << 8;
        val |= (CLR_UINT32)*ptr++ << 0;
    }

    p = ptr;

    return val;
}

inline NanoCLRDataType CLR_UncompressElementType(const CLR_UINT8 *&p)
{
    return (NanoCLRDataType)*p++;
}

inline CLR_UINT32 CLR_TkFromStream(const CLR_UINT8 *&p)
{
    static const NanoCLRTable c_lookup[4] = {TBL_TypeDef, TBL_TypeRef, TBL_TypeSpec, TBL_Max};

    CLR_UINT32 data = CLR_UncompressData(p);

    return CLR_TkFromType(c_lookup[data & 0x0003], data >> 2);
}

//--//--//--//

#if defined(__GNUC__)

#define NANOCLR_READ_UNALIGNED_UINT8(arg, ip)                                                                          \
    arg = *(const CLR_UINT8 *)ip;                                                                                      \
    ip += sizeof(CLR_UINT8)
template <typename T> __inline void NANOCLR_READ_UNALIGNED_UINT16(T &arg, CLR_PMETADATA &ip)
{
    arg = (CLR_UINT16)(*(const CLR_UINT8 *)ip);
    ip += sizeof(CLR_UINT8);
    arg |= (CLR_UINT16)(*(const CLR_UINT8 *)ip) << 8;
    ip += sizeof(CLR_UINT8);
}
template <typename T> __inline void NANOCLR_READ_UNALIGNED_UINT32(T &arg, CLR_PMETADATA &ip)
{
    arg = (CLR_UINT32)(*(const CLR_UINT8 *)ip);
    ip += sizeof(CLR_UINT8);
    arg |= (CLR_UINT32)(*(const CLR_UINT8 *)ip) << 8;
    ip += sizeof(CLR_UINT8);
    arg |= (CLR_UINT32)(*(const CLR_UINT8 *)ip) << 16;
    ip += sizeof(CLR_UINT8);
    arg |= (CLR_UINT32)(*(const CLR_UINT8 *)ip) << 24;
    ip += sizeof(CLR_UINT8);
}
template <typename T> __inline void NANOCLR_READ_UNALIGNED_UINT64(T &arg, CLR_PMETADATA &ip)
{
    arg = (CLR_UINT64)(*(const CLR_UINT8 *)ip);
    ip += sizeof(CLR_UINT8);
    arg |= (CLR_UINT64)(*(const CLR_UINT8 *)ip) << 8;
    ip += sizeof(CLR_UINT8);
    arg |= (CLR_UINT64)(*(const CLR_UINT8 *)ip) << 16;
    ip += sizeof(CLR_UINT8);
    arg |= (CLR_UINT64)(*(const CLR_UINT8 *)ip) << 24;
    ip += sizeof(CLR_UINT8);
    arg |= (CLR_UINT64)(*(const CLR_UINT8 *)ip) << 32;
    ip += sizeof(CLR_UINT8);
    arg |= (CLR_UINT64)(*(const CLR_UINT8 *)ip) << 40;
    ip += sizeof(CLR_UINT8);
    arg |= (CLR_UINT64)(*(const CLR_UINT8 *)ip) << 48;
    ip += sizeof(CLR_UINT8);
    arg |= (CLR_UINT64)(*(const CLR_UINT8 *)ip) << 56;
    ip += sizeof(CLR_UINT8);
}

#define NANOCLR_READ_UNALIGNED_INT8(arg, ip)                                                                           \
    arg = *(const CLR_INT8 *)ip;                                                                                       \
    ip += sizeof(CLR_INT8)
template <typename T> __inline void NANOCLR_READ_UNALIGNED_INT16(T &arg, CLR_PMETADATA &ip)
{
    arg = (CLR_UINT16)(*(const CLR_UINT8 *)ip);
    ip += sizeof(CLR_UINT8);
    arg |= (CLR_UINT16)(*(const CLR_UINT8 *)ip) << 8;
    ip += sizeof(CLR_UINT8);
    arg = (CLR_INT16)arg;
}
template <typename T> __inline void NANOCLR_READ_UNALIGNED_INT32(T &arg, CLR_PMETADATA &ip)
{
    arg = (CLR_UINT32)(*(const CLR_UINT8 *)ip);
    ip += sizeof(CLR_UINT8);
    arg |= (CLR_UINT32)(*(const CLR_UINT8 *)ip) << 8;
    ip += sizeof(CLR_UINT8);
    arg |= (CLR_UINT32)(*(const CLR_UINT8 *)ip) << 16;
    ip += sizeof(CLR_UINT8);
    arg |= (CLR_UINT32)(*(const CLR_UINT8 *)ip) << 24;
    ip += sizeof(CLR_UINT8);
    arg = (CLR_INT32)arg;
}
template <typename T> __inline void NANOCLR_READ_UNALIGNED_INT64(T &arg, CLR_PMETADATA &ip)
{
    arg = (CLR_UINT64)(*(const CLR_UINT8 *)ip);
    ip += sizeof(CLR_UINT8);
    arg |= (CLR_UINT64)(*(const CLR_UINT8 *)ip) << 8;
    ip += sizeof(CLR_UINT8);
    arg |= (CLR_UINT64)(*(const CLR_UINT8 *)ip) << 16;
    ip += sizeof(CLR_UINT8);
    arg |= (CLR_UINT64)(*(const CLR_UINT8 *)ip) << 24;
    ip += sizeof(CLR_UINT8);
    arg |= (CLR_UINT64)(*(const CLR_UINT8 *)ip) << 32;
    ip += sizeof(CLR_UINT8);
    arg |= (CLR_UINT64)(*(const CLR_UINT8 *)ip) << 40;
    ip += sizeof(CLR_UINT8);
    arg |= (CLR_UINT64)(*(const CLR_UINT8 *)ip) << 48;
    ip += sizeof(CLR_UINT8);
    arg |= (CLR_UINT64)(*(const CLR_UINT8 *)ip) << 56;
    ip += sizeof(CLR_UINT8);
    arg = (CLR_INT64)arg;
}

#elif defined(_MSC_VER)

#define NANOCLR_READ_UNALIGNED_UINT8(arg, ip)                                                                          \
    arg = *(__declspec(align(1)) const CLR_UINT8 *)ip;                                                                 \
    ip += sizeof(CLR_UINT8)
#define NANOCLR_READ_UNALIGNED_UINT16(arg, ip)                                                                         \
    arg = *(__declspec(align(1)) const CLR_UINT16 *)ip;                                                                \
    ip += sizeof(CLR_UINT16)
#define NANOCLR_READ_UNALIGNED_UINT32(arg, ip)                                                                         \
    arg = *(__declspec(align(1)) const CLR_UINT32 *)ip;                                                                \
    ip += sizeof(CLR_UINT32)
#define NANOCLR_READ_UNALIGNED_UINT64(arg, ip)                                                                         \
    arg = *(__declspec(align(1)) const CLR_UINT64 *)ip;                                                                \
    ip += sizeof(CLR_UINT64)

#define NANOCLR_READ_UNALIGNED_INT8(arg, ip)                                                                           \
    arg = *(__declspec(align(1)) const CLR_INT8 *)ip;                                                                  \
    ip += sizeof(CLR_INT8)
#define NANOCLR_READ_UNALIGNED_INT16(arg, ip)                                                                          \
    arg = *(__declspec(align(1)) const CLR_INT16 *)ip;                                                                 \
    ip += sizeof(CLR_INT16)
#define NANOCLR_READ_UNALIGNED_INT32(arg, ip)                                                                          \
    arg = *(__declspec(align(1)) const CLR_INT32 *)ip;                                                                 \
    ip += sizeof(CLR_INT32)
#define NANOCLR_READ_UNALIGNED_INT64(arg, ip)                                                                          \
    arg = *(__declspec(align(1)) const CLR_INT64 *)ip;                                                                 \
    ip += sizeof(CLR_INT64)

//--//

#define NANOCLR_WRITE_UNALIGNED_UINT8(ip, arg)                                                                         \
    *(__declspec(align(1)) CLR_UINT8 *)ip = arg;                                                                       \
    ip += sizeof(CLR_UINT8)
#define NANOCLR_WRITE_UNALIGNED_UINT16(ip, arg)                                                                        \
    *(__declspec(align(1)) CLR_UINT16 *)ip = arg;                                                                      \
    ip += sizeof(CLR_UINT16)
#define NANOCLR_WRITE_UNALIGNED_UINT32(ip, arg)                                                                        \
    *(__declspec(align(1)) CLR_UINT32 *)ip = arg;                                                                      \
    ip += sizeof(CLR_UINT32)
#define NANOCLR_WRITE_UNALIGNED_UINT64(ip, arg)                                                                        \
    *(__declspec(align(1)) CLR_UINT64 *)ip = arg;                                                                      \
    ip += sizeof(CLR_UINT64)

#define NANOCLR_WRITE_UNALIGNED_INT8(ip, arg)                                                                          \
    *(__declspec(align(1)) CLR_INT8 *)ip = arg;                                                                        \
    ip += sizeof(CLR_INT8)
#define NANOCLR_WRITE_UNALIGNED_INT16(ip, arg)                                                                         \
    *(__declspec(align(1)) CLR_INT16 *)ip = arg;                                                                       \
    ip += sizeof(CLR_INT16)
#define NANOCLR_WRITE_UNALIGNED_INT32(ip, arg)                                                                         \
    *(__declspec(align(1)) CLR_INT32 *)ip = arg;                                                                       \
    ip += sizeof(CLR_INT32)
#define NANOCLR_WRITE_UNALIGNED_INT64(ip, arg)                                                                         \
    *(__declspec(align(1)) CLR_INT64 *)ip = arg;                                                                       \
    ip += sizeof(CLR_INT64)

#else // TODO: __packed is compiler specific... Which compiler is this for?

#define NANOCLR_READ_UNALIGNED_UINT8(arg, ip)                                                                          \
    arg = *(__packed CLR_UINT8 *)ip;                                                                                   \
    ip += sizeof(CLR_UINT8)
#define NANOCLR_READ_UNALIGNED_UINT16(arg, ip)                                                                         \
    arg = *(__packed CLR_UINT16 *)ip;                                                                                  \
    ip += sizeof(CLR_UINT16)
#define NANOCLR_READ_UNALIGNED_UINT32(arg, ip)                                                                         \
    arg = *(__packed CLR_UINT32 *)ip;                                                                                  \
    ip += sizeof(CLR_UINT32)
#define NANOCLR_READ_UNALIGNED_UINT64(arg, ip)                                                                         \
    arg = *(__packed CLR_UINT64 *)ip;                                                                                  \
    ip += sizeof(CLR_UINT64)
#define NANOCLR_READ_UNALIGNED_INT8(arg, ip)                                                                           \
    arg = *(__packed CLR_INT8 *)ip;                                                                                    \
    ip += sizeof(CLR_INT8)
#define NANOCLR_READ_UNALIGNED_INT16(arg, ip)                                                                          \
    arg = *(__packed CLR_INT16 *)ip;                                                                                   \
    ip += sizeof(CLR_INT16)
#define NANOCLR_READ_UNALIGNED_INT32(arg, ip)                                                                          \
    arg = *(__packed CLR_INT32 *)ip;                                                                                   \
    ip += sizeof(CLR_INT32)
#define NANOCLR_READ_UNALIGNED_INT64(arg, ip)                                                                          \
    arg = *(__packed CLR_INT64 *)ip;                                                                                   \
    ip += sizeof(CLR_INT64)

//--//

#define NANOCLR_WRITE_UNALIGNED_UINT8(ip, arg)                                                                         \
    *(__packed CLR_UINT8 *)ip = arg;                                                                                   \
    ip += sizeof(CLR_UINT8)
#define NANOCLR_WRITE_UNALIGNED_UINT16(ip, arg)                                                                        \
    *(__packed CLR_UINT16 *)ip = arg;                                                                                  \
    ip += sizeof(CLR_UINT16)
#define NANOCLR_WRITE_UNALIGNED_UINT32(ip, arg)                                                                        \
    *(__packed CLR_UINT32 *)ip = arg;                                                                                  \
    ip += sizeof(CLR_UINT32)
#define NANOCLR_WRITE_UNALIGNED_UINT64(ip, arg)                                                                        \
    *(__packed CLR_UINT64 *)ip = arg;                                                                                  \
    ip += sizeof(CLR_UINT64)

#define NANOCLR_WRITE_UNALIGNED_INT8(ip, arg)                                                                          \
    *(__packed CLR_INT8 *)ip = arg;                                                                                    \
    ip += sizeof(CLR_INT8)
#define NANOCLR_WRITE_UNALIGNED_INT16(ip, arg)                                                                         \
    *(__packed CLR_INT16 *)ip = arg;                                                                                   \
    ip += sizeof(CLR_INT16)
#define NANOCLR_WRITE_UNALIGNED_INT32(ip, arg)                                                                         \
    *(__packed CLR_INT32 *)ip = arg;                                                                                   \
    ip += sizeof(CLR_INT32)
#define NANOCLR_WRITE_UNALIGNED_INT64(ip, arg)                                                                         \
    *(__packed CLR_INT64 *)ip = arg;                                                                                   \
    ip += sizeof(CLR_INT64)

#endif

//--//
#define NANOCLR_READ_UNALIGNED_OPCODE(op, ip)                                                                          \
    op = CLR_OPCODE(*ip++);                                                                                            \
    if (op == CEE_PREFIX1)                                                                                             \
    {                                                                                                                  \
        opcode = CLR_OPCODE(*ip++ + 256);                                                                              \
    }

#define NANOCLR_READ_UNALIGNED_COMPRESSED_FIELDTOKEN(arg, ip)                                                          \
    NANOCLR_READ_UNALIGNED_UINT16(arg, ip);                                                                            \
    arg = CLR_UncompressFieldToken(arg)
#define NANOCLR_READ_UNALIGNED_COMPRESSED_METHODTOKEN(arg, ip)                                                         \
    NANOCLR_READ_UNALIGNED_UINT16(arg, ip);                                                                            \
    arg = CLR_UncompressMethodToken(arg)
#define NANOCLR_READ_UNALIGNED_COMPRESSED_TYPETOKEN(arg, ip)                                                           \
    NANOCLR_READ_UNALIGNED_UINT16(arg, ip);                                                                            \
    arg = CLR_UncompressTypeToken(arg)
#define NANOCLR_READ_UNALIGNED_COMPRESSED_STRINGTOKEN(arg, ip)                                                         \
    NANOCLR_READ_UNALIGNED_UINT16(arg, ip);                                                                            \
    arg = CLR_UncompressStringToken(arg)

//--//

inline CLR_OPCODE CLR_ReadNextOpcode(CLR_PMETADATA &ip)
{
    CLR_PMETADATA ptr = ip;
    CLR_OPCODE opcode = CLR_OPCODE(*ptr++);

    if (opcode == CEE_PREFIX1)
    {
        opcode = CLR_OPCODE(*ptr++ + 256);
    }

    ip = ptr;

    return opcode;
}

inline CLR_OPCODE CLR_ReadNextOpcodeCompressed(CLR_PMETADATA &ip)
{
    CLR_PMETADATA ptr = ip;
    CLR_OPCODE opcode = CLR_OPCODE(*ptr++);

    if (opcode == CEE_PREFIX1)
    {
        opcode = CLR_OPCODE(*ptr++ + 256);
    }

    ip = ptr;

    return opcode;
}

//--//

#define FETCH_ARG_UINT8(arg, ip)                                                                                       \
    CLR_UINT32 arg;                                                                                                    \
    NANOCLR_READ_UNALIGNED_UINT8(arg, ip)
#define FETCH_ARG_UINT16(arg, ip)                                                                                      \
    CLR_UINT32 arg;                                                                                                    \
    NANOCLR_READ_UNALIGNED_UINT16(arg, ip)
#define FETCH_ARG_UINT32(arg, ip)                                                                                      \
    CLR_UINT32 arg;                                                                                                    \
    NANOCLR_READ_UNALIGNED_UINT32(arg, ip)
#define FETCH_ARG_UINT64(arg, ip)                                                                                      \
    CLR_UINT64 arg;                                                                                                    \
    NANOCLR_READ_UNALIGNED_UINT64(arg, ip)

#define FETCH_ARG_INT8(arg, ip)                                                                                        \
    CLR_INT32 arg;                                                                                                     \
    NANOCLR_READ_UNALIGNED_INT8(arg, ip)
#define FETCH_ARG_INT16(arg, ip)                                                                                       \
    CLR_INT32 arg;                                                                                                     \
    NANOCLR_READ_UNALIGNED_INT16(arg, ip)
#define FETCH_ARG_INT32(arg, ip)                                                                                       \
    CLR_INT32 arg;                                                                                                     \
    NANOCLR_READ_UNALIGNED_INT32(arg, ip)
#define FETCH_ARG_INT64(arg, ip)                                                                                       \
    CLR_INT64 arg;                                                                                                     \
    NANOCLR_READ_UNALIGNED_INT64(arg, ip)

#define FETCH_ARG_COMPRESSED_STRINGTOKEN(arg, ip)                                                                      \
    CLR_UINT32 arg;                                                                                                    \
    NANOCLR_READ_UNALIGNED_COMPRESSED_STRINGTOKEN(arg, ip)
#define FETCH_ARG_COMPRESSED_FIELDTOKEN(arg, ip)                                                                       \
    CLR_UINT32 arg;                                                                                                    \
    NANOCLR_READ_UNALIGNED_COMPRESSED_FIELDTOKEN(arg, ip)
#define FETCH_ARG_COMPRESSED_TYPETOKEN(arg, ip)                                                                        \
    CLR_UINT32 arg;                                                                                                    \
    NANOCLR_READ_UNALIGNED_COMPRESSED_TYPETOKEN(arg, ip)
#define FETCH_ARG_COMPRESSED_METHODTOKEN(arg, ip)                                                                      \
    CLR_UINT32 arg;                                                                                                    \
    NANOCLR_READ_UNALIGNED_COMPRESSED_METHODTOKEN(arg, ip)
#define FETCH_ARG_TOKEN(arg, ip)                                                                                       \
    CLR_UINT32 arg;                                                                                                    \
    NANOCLR_READ_UNALIGNED_UINT32(arg, ip)

//--//

#if defined(VIRTUAL_DEVICE)

CLR_PMETADATA CLR_SkipBodyOfOpcode(CLR_PMETADATA ip, CLR_OPCODE opcode);
CLR_PMETADATA CLR_SkipBodyOfOpcodeCompressed(CLR_PMETADATA ip, CLR_OPCODE opcode);

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

extern bool CLR_SafeSprintfV(char *&szBuffer, size_t &iBuffer, const char *format, va_list arg);
extern bool CLR_SafeSprintf(char *&szBuffer, size_t &iBuffer, const char *format, ...);

#if !defined(BUILD_RTM)

#define NANOCLR_DEBUG_STOP() _ASSERTE(false)

#else

#define NANOCLR_DEBUG_STOP() CPU_Reset()

#endif

//--//

struct CLR_Debug
{
    static int PrintfV(const char *format, va_list arg);
    static int Printf(const char *format, ...);
    static void Emit(const char *text, int len);
    static void Flush();

    //--//

    typedef int (*OutputHandler)(const char *format, ...);

#if defined(VIRTUAL_DEVICE)
    static void RedirectToString(std::string *str);
    static void SaveMessage(std::string str);
#endif
};

////////////////////////////////////////////////////////////////////////////////

struct CLR_RECORD_VERSION
{
    CLR_UINT16 majorVersion;
    CLR_UINT16 minorVersion;
    CLR_UINT16 buildNumber;
    CLR_UINT16 revisionNumber;
};

struct CLR_RECORD_ASSEMBLY
{
    CLR_UINT8 marker[8];
    //
    CLR_UINT32 headerCRC;
    CLR_UINT32 assemblyCRC;
    CLR_UINT32 flags;
    //
    CLR_UINT32 nativeMethodsChecksum;
    //
    CLR_RECORD_VERSION version;
    //
    CLR_STRING assemblyName; // TBL_Strings
    CLR_UINT16 stringTableVersion;
    //
    CLR_OFFSET_LONG startOfTables[TBL_Max];

    //
    // For every table, a number of bytes that were padded to the end of the table
    // to align to unsigned long.  Each table starts at a unsigned long boundary, and ends
    // at a unsigned long boundary.  Some of these tables will, by construction, have
    // no padding, and all will have values in the range [0-3].  This isn't the most
    // compact form to hold this information, but it only costs 16 bytes/assembly.
    // Trying to only align some of the tables is just much more hassle than it's worth.
    // And, of course, this field also has to be unsigned long-aligned.
    CLR_UINT8 paddingOfTables[((TBL_Max - 1) + 3) / 4 * 4];
    //--//

    bool GoodHeader() const;
    bool GoodAssembly() const;
    bool ValidateMarker() const;

#if defined(VIRTUAL_DEVICE)
    void ComputeCRC();
#endif

    CLR_OFFSET_LONG SizeOfTable(NanoCLRTable tbl) const
    {
        return startOfTables[tbl + 1] - startOfTables[tbl] - paddingOfTables[tbl];
    }

    CLR_OFFSET_LONG TotalSize() const
    {
        return startOfTables[TBL_EndOfAssembly];
    }

    //--//

    static CLR_UINT32 ComputeAssemblyHash(const char *name, const CLR_RECORD_VERSION &ver);
};

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// size of CLR_RECORDs
// these have to match the struct used in MDP
// when changing any of these structs need to update the constant in MDP
#define sizeOf_CLR_RECORD_ASSEMBLYREF  10
#define sizeOf_CLR_RECORD_TYPEREF      6
#define sizeOf_CLR_RECORD_FIELDREF     6
#define sizeOf_CLR_RECORD_METHODREF    6
#define sizeOf_CLR_RECORD_TYPEDEF      27
#define sizeOf_CLR_RECORD_FIELDDEF     8
#define sizeOf_CLR_RECORD_METHODDEF    19
#define sizeOf_CLR_RECORD_TYPESPEC     2
#define sizeOf_CLR_RECORD_GENERICPARAM 10
#define sizeOf_CLR_RECORD_METHODSPEC   6
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

// record structures are aligned to 1 byte boundary to minimize PE size
#pragma pack(push, 1)

/// @brief Assembly header
struct CLR_RECORD_ASSEMBLYREF
{
    /// @brief Index into TBL_Strings
    CLR_STRING name;

    /// @brief Assembly version
    CLR_RECORD_VERSION version;
};

CT_ASSERT(sizeof(CLR_RECORD_ASSEMBLYREF) == sizeOf_CLR_RECORD_ASSEMBLYREF)

struct CLR_RECORD_TYPEREF
{
    /// @brief Index into TBL_Strings
    ///
    CLR_STRING name;

    /// @brief Index into TBL_Strings
    ///
    CLR_STRING nameSpace;

    /// @brief TypeRefOrAssemblyRef -> Index into TBL_AssemblyRef (ORed with 0x0000) | TBL_TypeRef (ORed with 0x8000)
    ///
    CLR_INDEX scope;
};

CT_ASSERT(sizeof(CLR_RECORD_TYPEREF) == sizeOf_CLR_RECORD_TYPEREF)

struct CLR_RECORD_FIELDREF
{
    /// @brief Index into TBL_Strings
    ///
    CLR_STRING name;

    /// @brief Encoded index into TBL_TypeRef or TBL_TypeSpec for the type containing this field
    CLR_EncodedTypeRefOrSpec encodedOwner;

    /// @brief Index into TBL_Signatures
    ///
    CLR_SIG signature;

    /// @brief Index into owner table
    ///
    CLR_INDEX OwnerIndex() const
    {
        return (encodedOwner & 0x7FFF);
    }

    /// @brief Index into owner table
    ///
    NanoCLRTable Owner() const
    {
        static const NanoCLRTable c_lookup[2] = {TBL_TypeRef, TBL_TypeSpec};

        return c_lookup[(encodedOwner >> 15)];
    }
};

CT_ASSERT(sizeof(CLR_RECORD_FIELDREF) == sizeOf_CLR_RECORD_FIELDREF)

struct CLR_RECORD_METHODREF
{
    /// @brief Index into TBL_Strings
    ///
    CLR_STRING name;

    /// @brief Encoded index into TBL_TypeRef or TBL_TypeSpec for the type containing the method
    ///
    CLR_EncodedTypeRefOrSpec encodedOwner;

    /// @brief Index into TBL_Signatures
    ///
    CLR_SIG signature;

    //--//

    /// @brief Index into owner table
    ///
    CLR_INDEX OwnerIndex() const
    {
        return (encodedOwner & 0x7FFF);
    }

    /// @brief Owner table
    ///
    NanoCLRTable Owner() const
    {
        static const NanoCLRTable c_lookup[2] = {TBL_TypeRef, TBL_TypeSpec};

        return c_lookup[(encodedOwner >> 15)];
    }

    bool HasOwnerType() const
    {
        return encodedOwner != CLR_EmptyIndex;
    }
};

CT_ASSERT(sizeof(CLR_RECORD_METHODREF) == sizeOf_CLR_RECORD_METHODREF)

struct CLR_RECORD_TYPEDEF
{
    static const CLR_UINT16 TD_Scope_Mask = 0x0007;
    /// @brief Class is not public scope.
    ///
    static const CLR_UINT16 TD_Scope_NotPublic = 0x0000;
    /// @brief Class is public scope.
    ///
    static const CLR_UINT16 TD_Scope_Public = 0x0001;
    /// @brief Class is nested with public visibility.
    ///
    static const CLR_UINT16 TD_Scope_NestedPublic = 0x0002;
    /// @brief Class is nested with private visibility.
    ///
    static const CLR_UINT16 TD_Scope_NestedPrivate = 0x0003;

    /// @brief Class is nested with family visibility.
    ///
    static const CLR_UINT16 TD_Scope_NestedFamily = 0x0004;

    /// @brief Class is nested with assembly visibility.
    ///
    static const CLR_UINT16 TD_Scope_NestedAssembly = 0x0005;
    /// @brief Class is nested with family and assembly visibility.
    ///
    static const CLR_UINT16 TD_Scope_NestedFamANDAssem = 0x0006; //
    /// @brief Class is nested with family or assembly visibility.
    ///
    static const CLR_UINT16 TD_Scope_NestedFamORAssem = 0x0007;

    static const CLR_UINT16 TD_Serializable = 0x0008;

    static const CLR_UINT16 TD_Semantics_Mask = 0x0030;
    static const CLR_UINT16 TD_Semantics_Class = 0x0000;
    static const CLR_UINT16 TD_Semantics_ValueType = 0x0010;
    static const CLR_UINT16 TD_Semantics_Interface = 0x0020;
    static const CLR_UINT16 TD_Semantics_Enum = 0x0030;

    static const CLR_UINT16 TD_Abstract = 0x0040;
    static const CLR_UINT16 TD_Sealed = 0x0080;

    static const CLR_UINT16 TD_SpecialName = 0x0100;
    static const CLR_UINT16 TD_Delegate = 0x0200;
    static const CLR_UINT16 TD_MulticastDelegate = 0x0400;

    static const CLR_UINT16 TD_Patched = 0x0800;

    static const CLR_UINT16 TD_BeforeFieldInit = 0x1000;
    static const CLR_UINT16 TD_HasSecurity = 0x2000;
    static const CLR_UINT16 TD_HasFinalizer = 0x4000;
    static const CLR_UINT16 TD_HasAttributes = 0x8000;

    /// @brief Index into TBL_Strings with the name of the type
    ///
    CLR_STRING name;

    /// @brief Index into TBL_Strings  with the name of the namespace containing the type
    ///
    CLR_STRING nameSpace;

    /// @brief Encoded index for TypeDefOrRef -> Index into  TBL_TypeDef | TBL_TypeRef
    ///
    CLR_EncodedTypeDefOrRef encodedExtends;

    /// @brief Encoded index for TypeDefOrRef -> Index into  TBL_TypeDef | TBL_TypeRef
    ///
    CLR_EncodedTypeDefOrRef encodedEnclosingType;

    /// @brief Index into TBL_Signatures blob table for the set of interfaces implemented by this type
    ///
    CLR_SIG interfaces;

    /// @brief Index into TBL_MethodDef for the first method of the type
    ///
    CLR_INDEX firstMethod;

    /// @brief Count of virtual methods in the type
    ///
    CLR_UINT8 virtualMethodCount;

    /// @brief Count of instance methods in the type
    ///
    CLR_UINT8 instanceMethodCount;

    /// @brief Count of static methods in the type
    ///
    CLR_UINT8 staticMethodCount;

    /// @brief Data type identity for the type
    ///
    CLR_UINT8 dataType;

    /// @brief Index into  TBL_FieldDef for the first static field of the type
    ///
    CLR_INDEX firstStaticField;

    /// @brief Index into  TBL_FieldDef for the first instance field of the type
    ///
    CLR_INDEX firstInstanceField;

    /// @brief Count of static fields in the type
    ///
    CLR_UINT8 staticFieldsCount;

    /// @brief Count of instance fields in the type
    ///
    CLR_UINT8 instanceFieldsCount;

    /// @brief Index into TBL_GenericParam for the first generic parameter for the type
    ///
    CLR_INDEX firstGenericParam;

    /// @brief Count of generic parameters for the type
    ///
    CLR_UINT8 genericParamCount;

    /// @brief Flags defining intrinsic attributes and access modifiers for the type
    ///
    CLR_UINT16 flags;

    //--//

    bool IsEnum() const
    {
        return (flags & (TD_Semantics_Mask)) == TD_Semantics_Enum;
    }

    bool IsDelegate() const
    {
        return (flags & (TD_Delegate | TD_MulticastDelegate)) != 0;
    }

    bool HasValidExtendsType() const
    {
        return encodedExtends != CLR_EmptyIndex;
    }

    bool HasValidEnclosingType() const
    {
        return encodedEnclosingType != CLR_EmptyIndex;
    }

    /// @brief Index into Extends table
    ///
    CLR_INDEX ExtendsIndex() const
    {
        return (encodedExtends & 0x7FFF);
    }

    /// @brief Extends table
    ///
    NanoCLRTable Extends() const
    {
        static const NanoCLRTable c_lookup[2] = {TBL_TypeDef, TBL_TypeRef};

        return c_lookup[(encodedExtends >> 15)];
    }

    /// @brief Index into EnclosingType table
    ///
    CLR_INDEX EnclosingTypeIndex() const
    {
        return (encodedEnclosingType & 0x7FFF);
    }

    /// @brief EnclosingType table
    ///
    NanoCLRTable EnclosingType() const
    {
        static const NanoCLRTable c_lookup[2] = {TBL_TypeDef, TBL_TypeRef};

        return c_lookup[(encodedEnclosingType >> 15)];
    }
};

CT_ASSERT(sizeof(CLR_RECORD_TYPEDEF) == sizeOf_CLR_RECORD_TYPEDEF)

struct CLR_RECORD_FIELDDEF
{
    static const CLR_UINT16 FD_Scope_Mask = 0x0007;
    static const CLR_UINT16 FD_Scope_PrivateScope = 0x0000; // Member not referenceable.
    static const CLR_UINT16 FD_Scope_Private = 0x0001;      // Accessible only by the parent type.
    static const CLR_UINT16 FD_Scope_FamANDAssem = 0x0002;  // Accessible by sub-types only in this Assembly.
    static const CLR_UINT16 FD_Scope_Assembly = 0x0003;     // Accessibly by anyone in the Assembly.
    static const CLR_UINT16 FD_Scope_Family = 0x0004;       // Accessible only by type and sub-types.
    static const CLR_UINT16 FD_Scope_FamORAssem = 0x0005; // Accessibly by sub-types anywhere, plus anyone in assembly.
    static const CLR_UINT16 FD_Scope_Public = 0x0006;     // Accessibly by anyone who has visibility to this scope.

    static const CLR_UINT16 FD_NotSerialized = 0x0008; // Field does not have to be serialized when type is remoted.

    static const CLR_UINT16 FD_Static = 0x0010;   // Defined on type, else per instance.
    static const CLR_UINT16 FD_InitOnly = 0x0020; // Field may only be initialized, not written to after init.
    static const CLR_UINT16 FD_Literal = 0x0040;  // Value is compile time constant.

    static const CLR_UINT16 FD_SpecialName = 0x0100; // field is special.  Name describes how.
    static const CLR_UINT16 FD_HasDefault = 0x0200;  // Field has default.
    static const CLR_UINT16 FD_HasFieldRVA = 0x0400; // Field has RVA.

    static const CLR_UINT16 FD_NoReflection = 0x0800; // field does not allow reflection

    static const CLR_UINT16 FD_HasAttributes = 0x8000;

    /// @brief Index into TBL_Strings
    ///
    CLR_STRING name;

    /// @brief Index into TBL_Signatures
    ///
    CLR_SIG signature;

    /// @brief Index into TBL_Signatures
    ///
    CLR_SIG defaultValue;

    CLR_UINT16 flags;
};

CT_ASSERT(sizeof(CLR_RECORD_FIELDDEF) == sizeOf_CLR_RECORD_FIELDDEF)

struct CLR_RECORD_METHODDEF
{
    static const CLR_UINT32 MD_Scope_Mask = 0x00000007;

    /// @brief ember not referenceable.
    ///
    static const CLR_UINT32 MD_Scope_PrivateScope = 0x00000000;

    /// @brief Accessible only by the parent type.
    ///
    static const CLR_UINT32 MD_Scope_Private = 0x00000001;

    /// @brief Accessible by sub-types only in this Assembly.
    ///
    static const CLR_UINT32 MD_Scope_FamANDAssem = 0x00000002;

    /// @brief Accessibly by anyone in the Assembly.
    ///
    static const CLR_UINT32 MD_Scope_Assem = 0x00000003;

    /// @brief Accessible only by type and sub-types.
    ///
    static const CLR_UINT32 MD_Scope_Family = 0x00000004;

    /// @brief Accessibly by sub-types anywhere, plus anyone in assembly.
    ///
    static const CLR_UINT32 MD_Scope_FamORAssem = 0x00000005;

    /// @brief Accessibly by anyone who has visibility to this scope.
    ///
    static const CLR_UINT32 MD_Scope_Public = 0x00000006;

    /// @brief Defined on type, else per instance.
    ///
    static const CLR_UINT32 MD_Static = 0x00000010;

    /// @brief Method may not be overridden.
    ///
    static const CLR_UINT32 MD_Final = 0x00000020;

    /// @brief Method virtual.
    ///
    static const CLR_UINT32 MD_Virtual = 0x00000040;

    /// @brief Method hides by name+sig, else just by name.
    ///
    static const CLR_UINT32 MD_HideBySig = 0x00000080;

    static const CLR_UINT32 MD_VtableLayoutMask = 0x00000100;

    /// @brief The default.
    ///
    static const CLR_UINT32 MD_ReuseSlot = 0x00000000;

    /// @brief Method always gets a new slot in the vtable.
    ///
    static const CLR_UINT32 MD_NewSlot = 0x00000100;

    /// @brief  Method does not provide an implementation.
    ///
    static const CLR_UINT32 MD_Abstract = 0x00000200;

    /// @brief  Method is special.  Name describes how.
    ///
    static const CLR_UINT32 MD_SpecialName = 0x00000400;

    static const CLR_UINT32 MD_NativeProfiled = 0x00000800;

    static const CLR_UINT32 MD_Constructor = 0x00001000;
    static const CLR_UINT32 MD_StaticConstructor = 0x00002000;
    static const CLR_UINT32 MD_Finalizer = 0x00004000;

    static const CLR_UINT32 MD_DelegateConstructor = 0x00010000;
    static const CLR_UINT32 MD_DelegateInvoke = 0x00020000;
    static const CLR_UINT32 MD_DelegateBeginInvoke = 0x00040000;
    static const CLR_UINT32 MD_DelegateEndInvoke = 0x00080000;

    static const CLR_UINT32 MD_IsGenericInstance = 0x00100000;
    static const CLR_UINT32 MD_ContainsGenericParameter = 0x00200000;

    static const CLR_UINT32 MD_Synchronized = 0x01000000;
    static const CLR_UINT32 MD_GloballySynchronized = 0x02000000;
    static const CLR_UINT32 MD_Patched = 0x04000000;
    static const CLR_UINT32 MD_EntryPoint = 0x08000000;

    /// @brief Method calls another method containing security code.
    ///
    static const CLR_UINT32 MD_RequireSecObject = 0x10000000;

    /// @brief Method has security associate with it.
    ///
    static const CLR_UINT32 MD_HasSecurity = 0x20000000;
    static const CLR_UINT32 MD_HasExceptionHandlers = 0x40000000;
    static const CLR_UINT32 MD_HasAttributes = 0x80000000;

    /// @brief Index into TBL_Strings for the name of the method
    ///
    CLR_STRING name;

    /// @brief Offset into the IL byte code blob table for the opcodes of the method
    ///
    CLR_OFFSET rva;

    /// @brief Flags to indicate intrinsic attributes and semantics of the method
    ///
    CLR_UINT32 flags;

    /// @brief DataType of the return value for the method
    ///
    CLR_UINT8 retValDataType;

    /// @brief Count of arguments to the method
    ///
    CLR_UINT8 argumentsCount;

    /// @brief Count of local variables for the method
    ///
    CLR_UINT8 localsCount;

    /// @brief Length of the evaluation stack for the method
    ///
    CLR_UINT8 lengthEvalStack;

    /// @brief Index into TBL_Signatures to describe the locals for the method
    ///
    CLR_SIG locals;

    /// @brief Index into TBL_GenericParam for the first generic parameter for the method
    ///
    CLR_INDEX firstGenericParam;

    /// @brief Count of generic parameters for the method
    ///
    CLR_UINT8 genericParamCount;

    /// @brief Index into TBL_Signatures that describes the method itself
    ///
    CLR_SIG signature;
};

CT_ASSERT(sizeof(CLR_RECORD_METHODDEF) == sizeOf_CLR_RECORD_METHODDEF)

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
// pragma required here because compiler is not too happy with the cast to CLR_UINT32* from a CLR_UINT16 variable

struct CLR_RECORD_ATTRIBUTE
{
    /// @brief one of TBL_TypeDef, TBL_MethodDef, or TBL_FieldDef.
    ///
    CLR_UINT16 ownerType;

    /// @brief TBL_TypeDef | TBL_MethodDef | TBL_FielfDef
    ///
    CLR_UINT16 ownerIndex;
    CLR_UINT16 constructor;

    /// @brief Index into TBL_Signatures
    ///
    CLR_SIG data;

    CLR_UINT16 Key() const
    {
        return *(CLR_UINT16 *)&ownerType;
    }
};

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

struct CLR_RECORD_TYPESPEC
{
    /// @brief Index into TBL_Signatures
    ///
    CLR_SIG signature;
};

CT_ASSERT(sizeof(CLR_RECORD_TYPESPEC) == sizeOf_CLR_RECORD_TYPESPEC)

struct CLR_RECORD_GENERICPARAM
{
    /// @brief 2-byte index of the generic parameter, numbered left -to-right, from zero.
    ///
    CLR_UINT16 number;

    /// @brief 2-byte bitmask of type GenericParamAttributes
    ///
    CLR_UINT16 flags;

    /// @brief TypeOrMethodDef -> Index into TBL_TypeDef TBL_MethodDef
    ///
    CLR_TYPEORMETHODDEF owner;

    /// @brief Index into TBL_Signatures for parameter type signature
    ///
    CLR_SIG signature;

    /// @brief Index into TBL_Strings
    ///
    CLR_STRING name;
};

CT_ASSERT(sizeof(CLR_RECORD_GENERICPARAM) == sizeOf_CLR_RECORD_GENERICPARAM)

struct CLR_RECORD_METHODSPEC
{
    /// @brief Encoded index into TBL_MethodDef | TBL_MethodRef
    ///
    CLR_EncodedMethodDefOrRef encodedMethod;

    /// @brief Index into TBL_Signatures holding the signature of this instantiation
    ///
    CLR_SIG instantiation;

    /// @brief Index into TBL_Signatures for the type specification containing the method
    ///
    CLR_INDEX container;

    /// @brief Index into table pointed by  TBL_MethodDef | TBL_MethodRef
    ///
    CLR_INDEX MethodIndex() const
    {
        return (encodedMethod & 0x7FFF);
    }

    /// @brief EnclosingType table
    ///
    NanoCLRTable MethodKind() const
    {
        static const NanoCLRTable c_lookup[2] = {TBL_MethodDef, TBL_MethodRef};

        return c_lookup[(encodedMethod >> 15)];
    }
};

CT_ASSERT(sizeof(CLR_RECORD_METHODSPEC) == sizeOf_CLR_RECORD_METHODSPEC)

struct CLR_RECORD_EH
{
    static const CLR_UINT16 EH_Catch = 0x0000;
    static const CLR_UINT16 EH_CatchAll = 0x0001;
    static const CLR_UINT16 EH_Finally = 0x0002;
    static const CLR_UINT16 EH_Filter = 0x0003;

    //--//

    CLR_UINT16 mode;
    union {
        CLR_INDEX classToken; // TBL_TypeDef | TBL_TypeRef
        CLR_OFFSET filterStart;
    };
    CLR_OFFSET tryStart;
    CLR_OFFSET tryEnd;
    CLR_OFFSET handlerStart;
    CLR_OFFSET handlerEnd;

    //--//

    static CLR_PMETADATA ExtractEhFromByteCode(CLR_PMETADATA ipEnd, const CLR_RECORD_EH *&ptrEh, CLR_UINT32 &numEh);

    CLR_UINT32 GetToken() const;
};

CT_ASSERT_UNIQUE_NAME(sizeof(CLR_RECORD_EH) == 12, CLR_RECORD_EH)

struct CLR_RECORD_RESOURCE_FILE
{
    static const CLR_UINT32 CURRENT_VERSION = 2;

    CLR_UINT32 version;
    CLR_UINT32 sizeOfHeader;
    CLR_UINT32 sizeOfResourceHeader;
    CLR_UINT32 numberOfResources;
    CLR_STRING name; // TBL_Strings
    CLR_UINT16 pad;
    CLR_UINT32 offset; // TBL_Resource
};

struct CLR_RECORD_RESOURCE
{
    static const CLR_UINT8 RESOURCE_Invalid = 0x00;
    static const CLR_UINT8 RESOURCE_Bitmap = 0x01;
    static const CLR_UINT8 RESOURCE_Font = 0x02;
    static const CLR_UINT8 RESOURCE_String = 0x03;
    static const CLR_UINT8 RESOURCE_Binary = 0x04;

    static const CLR_UINT8 FLAGS_PaddingMask = 0x03;
    static const CLR_INT16 SENTINEL_ID = 0x7FFF;

    //
    // Sorted on id
    //
    CLR_INT16 id;
    CLR_UINT8 kind;
    CLR_UINT8 flags;
    CLR_UINT32 offset;
};

#if defined(_MSC_VER)
#pragma pack(pop, __NANOCLR_TYPES_H__)
#endif

#endif // NANOCLR_TYPES_H
