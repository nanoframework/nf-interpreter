//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include <nanoCLR_Runtime.h>

__nfweak bool CLR_RT_BinaryFormatter::SerializationEnabled()
{
    NATIVE_PROFILE_CLR_SERIALIZATION();
    return false;
}

__nfweak CLR_RT_HeapBlock* CLR_RT_BinaryFormatter::TypeHandler::FixDereference( CLR_RT_HeapBlock* v )
{
    (void)v;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    return NULL;
}

__nfweak CLR_RT_HeapBlock* CLR_RT_BinaryFormatter::TypeHandler::FixNull( CLR_RT_HeapBlock* v )
{
    (void)v;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    return NULL;
}

//--//

__nfweak HRESULT CLR_RT_BinaryFormatter::TypeHandler::TypeHandler_Initialize( CLR_RT_BinaryFormatter* bf, SerializationHintsAttribute* hints, CLR_RT_TypeDescriptor* expected )
{
    (void)bf;
    (void)hints;
    (void)expected;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

__nfweak HRESULT CLR_RT_BinaryFormatter::TypeHandler::SetValue( CLR_RT_HeapBlock* v )
{
    (void)v;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

//--//

__nfweak int CLR_RT_BinaryFormatter::TypeHandler::SignatureRequirements()
{
    NATIVE_PROFILE_CLR_SERIALIZATION();
    return 0;
}

__nfweak bool CLR_RT_BinaryFormatter::TypeHandler::CompareTypes( CLR_RT_TypeDescriptor* left, CLR_RT_TypeDescriptor* right )
{
    (void)left;
    (void)right;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    return true;
}

__nfweak CLR_DataType CLR_RT_BinaryFormatter::TypeHandler::GetDataType( CLR_RT_TypeDescriptor* type )
{
    (void)type;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    return (CLR_DataType)0;
}

__nfweak CLR_UINT32 CLR_RT_BinaryFormatter::TypeHandler::GetSizeOfType( CLR_RT_TypeDescriptor* type )
{
    (void)type;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    return 0;
}

__nfweak bool CLR_RT_BinaryFormatter::TypeHandler::GetSignOfType( CLR_RT_TypeDescriptor* type )
{
    (void)type;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    return true;
}

//--//

__nfweak HRESULT CLR_RT_BinaryFormatter::TypeHandler::EmitSignature( int& res )
{
    (void)res;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

__nfweak HRESULT CLR_RT_BinaryFormatter::TypeHandler::EmitSignature_Inner( int mask, CLR_RT_TypeDescriptor* type, CLR_RT_HeapBlock* value )
{
    (void)mask;
    (void)type;
    (void)value;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

//--//

__nfweak HRESULT CLR_RT_BinaryFormatter::TypeHandler::ReadSignature( int& res )
{
    (void)res;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

//--//

__nfweak HRESULT CLR_RT_BinaryFormatter::TypeHandler::EmitValue( int& res )
{
    (void)res;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

__nfweak HRESULT CLR_RT_BinaryFormatter::TypeHandler::ReadValue( int& res )
{
    (void)res;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

//--//

__nfweak HRESULT CLR_RT_BinaryFormatter::TypeHandler::TrackObject( int& res )
{
    (void)res;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

__nfweak HRESULT CLR_RT_BinaryFormatter::State::CreateInstance( CLR_RT_BinaryFormatter* parent, SerializationHintsAttribute* hints, CLR_RT_HeapBlock* type )
{
    (void)parent;
    (void)hints;
    (void)type;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

__nfweak HRESULT CLR_RT_BinaryFormatter::State::CreateInstance( CLR_RT_BinaryFormatter* parent, SerializationHintsAttribute* hints, CLR_RT_TypeDescriptor* type )
{
    (void)parent;
    (void)hints;
    (void)type;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

__nfweak void CLR_RT_BinaryFormatter::State::DestroyInstance()
{
    NATIVE_PROFILE_CLR_SERIALIZATION();
}

//--//

__nfweak HRESULT CLR_RT_BinaryFormatter::State::FindHints( SerializationHintsAttribute& hints, const CLR_RT_TypeDef_Instance& cls )
{
    (void)hints;
    (void)cls;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

__nfweak HRESULT CLR_RT_BinaryFormatter::State::FindHints( SerializationHintsAttribute& hints, const CLR_RT_FieldDef_Instance& fld )
{
    (void)hints;
    (void)fld;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

//--//

__nfweak HRESULT CLR_RT_BinaryFormatter::State::AssignAndFixBoxing( CLR_RT_HeapBlock& dst )
{
    (void)dst;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

__nfweak HRESULT CLR_RT_BinaryFormatter::State::GetValue()
{
    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

__nfweak HRESULT CLR_RT_BinaryFormatter::State::SetValueAndDestroyInstance()
{
    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

//////////////////////////////////////////////////////////////////////////////////////////

__nfweak HRESULT CLR_RT_BinaryFormatter::State::Advance()
{
    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

//--//

__nfweak HRESULT CLR_RT_BinaryFormatter::State::AdvanceToTheNextField()
{
    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

//--//

__nfweak HRESULT CLR_RT_BinaryFormatter::State::AdvanceToTheNextElement()
{
    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

__nfweak HRESULT CLR_RT_BinaryFormatter::CreateInstance( CLR_UINT8* buf, int len, CLR_RT_BinaryFormatter*& res )
{
    (void)buf;
    (void)len;
    (void)res;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

__nfweak void CLR_RT_BinaryFormatter::DestroyInstance()
{
    NATIVE_PROFILE_CLR_SERIALIZATION();
}

__nfweak HRESULT CLR_RT_BinaryFormatter::Advance()
{
    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

//--//

__nfweak void CLR_RT_BinaryFormatter::PrepareForGC( void* data )
{
    (void)data;

    NATIVE_PROFILE_CLR_SERIALIZATION();
}

__nfweak HRESULT CLR_RT_BinaryFormatter::Serialize( CLR_RT_HeapBlock& refData, CLR_RT_HeapBlock& object, CLR_RT_HeapBlock* cls, CLR_UINT32 flags )
{
    (void)refData;
    (void)object;
    (void)cls;
    (void)flags;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

__nfweak HRESULT CLR_RT_BinaryFormatter::Deserialize( CLR_RT_HeapBlock& refData, CLR_RT_HeapBlock& object, CLR_RT_HeapBlock* cls, CLR_UINT32* unknownType, CLR_UINT32 flags )
{
    (void)refData;
    (void)object;
    (void)cls;
    (void)unknownType;
    (void)flags;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

__nfweak HRESULT CLR_RT_BinaryFormatter::Deserialize( CLR_RT_HeapBlock& refData, CLR_UINT8* data, CLR_UINT32 size, CLR_RT_HeapBlock* cls, CLR_UINT32* unknownType, CLR_UINT32 flags )
{
    (void)refData;
    (void)data;
    (void)size;
    (void)cls;
    (void)unknownType;
    (void)flags;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

//--//

__nfweak HRESULT CLR_RT_BinaryFormatter::TrackDuplicate( CLR_RT_HeapBlock* object )
{
    (void)object;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

__nfweak CLR_UINT32 CLR_RT_BinaryFormatter::SearchDuplicate( CLR_RT_HeapBlock* object )
{
    (void)object;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    return (CLR_UINT32)-1;
}

__nfweak CLR_RT_HeapBlock* CLR_RT_BinaryFormatter::GetDuplicate( CLR_UINT32 idx )
{
    (void)idx;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    return NULL;
}

//--//--//

__nfweak int     CLR_RT_BinaryFormatter::BitsAvailable(                                  ) { return 0; }

__nfweak HRESULT CLR_RT_BinaryFormatter::ReadBits     (       CLR_UINT32& res, int bits  ) { (void)res; (void)bits; NANOCLR_FEATURE_STUB_RETURN(); }
__nfweak HRESULT CLR_RT_BinaryFormatter::WriteBits    (       CLR_UINT32  val, int bits  ) { (void)val; (void)bits; NANOCLR_FEATURE_STUB_RETURN(); }

__nfweak HRESULT CLR_RT_BinaryFormatter::ReadBits     (       CLR_UINT64& res, int bits  ) { (void)res; (void)bits; NANOCLR_FEATURE_STUB_RETURN(); }
__nfweak HRESULT CLR_RT_BinaryFormatter::WriteBits    (       CLR_UINT64  val, int bits  ) { (void)val; (void)bits; NANOCLR_FEATURE_STUB_RETURN(); }

__nfweak HRESULT CLR_RT_BinaryFormatter::ReadArray    (       CLR_UINT8*  buf, int bytes ) { (void)buf; (void)bytes; NANOCLR_FEATURE_STUB_RETURN(); }
__nfweak HRESULT CLR_RT_BinaryFormatter::WriteArray   ( const CLR_UINT8*  buf, int bytes ) { (void)buf; (void)bytes; NANOCLR_FEATURE_STUB_RETURN(); }

__nfweak HRESULT CLR_RT_BinaryFormatter::ReadCompressedUnsigned( CLR_UINT32& val )
{
    (void)val;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

__nfweak HRESULT CLR_RT_BinaryFormatter::WriteCompressedUnsigned( CLR_UINT32 val )
{
    (void)val;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

//--//

__nfweak HRESULT CLR_RT_BinaryFormatter::ReadType( CLR_RT_ReflectionDef_Index& val )
{
    (void)val;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

__nfweak HRESULT CLR_RT_BinaryFormatter::WriteType( const CLR_RT_ReflectionDef_Index& val )
{
    (void)val;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}

__nfweak HRESULT CLR_RT_BinaryFormatter::WriteType( CLR_UINT32 hash )
{
    (void)hash;

    NATIVE_PROFILE_CLR_SERIALIZATION();
    NANOCLR_FEATURE_STUB_RETURN();
}
