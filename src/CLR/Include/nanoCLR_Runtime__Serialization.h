//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOCLR_RUNTIME__SERIALIZATION_H_
#define _NANOCLR_RUNTIME__SERIALIZATION_H_

struct CLR_RT_BinaryFormatter : public CLR_RT_HeapBlock_Node // EVENT HEAP - NO RELOCATION -
{
    //////////////////////////////////////////////////////////////////
    //
    // Keep in sync with definitions in Reflection.cs!!!!
    // Keep in sync with definitions in Reflection.cs!!!!
    // Keep in sync with definitions in Reflection.cs!!!!
    //
    static const int        TE_L1           = 2;
    static const CLR_UINT32 TE_L1_Null      = 0x00000000;
    static const CLR_UINT32 TE_L1_Duplicate = 0x00000001; // N bits for the duplicate id.
    static const CLR_UINT32 TE_L1_Reference = 0x00000002; // 32 bits for the type.
    static const CLR_UINT32 TE_L1_Other     = 0x00000003;

    static const int        TE_L2           = 2;
    static const CLR_UINT32 TE_L2_Primitive = 0x00000000; // 4 bits for the type.
    static const CLR_UINT32 TE_L2_Array     = 0x00000001;
    static const CLR_UINT32 TE_L2_ArrayList = 0x00000002;
    static const CLR_UINT32 TE_L2_Other     = 0x00000003;

    static const int        TE_L3            = 4;
    static const CLR_UINT32 TE_L3_Type       = 0x00000000; // 32 bits for the type.
    static const CLR_UINT32 TE_L3_Reflection = 0x00000001; // CLR_RT_ReflectionDef_Index.  Valid in marshaling only.
    static const CLR_UINT32 TE_L3_MBRO       = 0x00000002; // 32 bits for the pointer to the MBRO class, 32 bits for the pointer to the AppDomain    

    static const int        TE_ElementType  = 4;
    static const int        TE_ArrayDepth   = 4;

    static const CLR_UINT32 c_Flags_None    = 0x00000000;
    static const CLR_UINT32 c_Flags_Marshal = 0x00000001;

    //
    //--//
    //
    enum SerializationFlags
    {
        SF_Encrypted         = 0x00000001,
        SF_Compressed        = 0x00000002, // Value uses range compression (max 2^30 values).
        SF_Optional          = 0x00000004, // If the value cannot be deserialized, skip it.

        SF_PointerNeverNull  = 0x00000010,
        SF_ElementsNeverNull = 0x00000020,

        SF_FixedType         = 0x00000100,
    };
    //
    struct SerializationHintsAttribute
    {
        SerializationFlags m_flags;

        int                m_arraySize;     // -1 == extend to the end of the stream.

        int                m_bitPacked;     // In bits.
        CLR_INT64          m_rangeBias;
        CLR_UINT64         m_scale;         // For time, it's in ticks.
    };
    //
    //////////////////////////////////////////////////////////////////

    struct TypeHandler
    {
        //
        // Type of signatures:
        //
        // 1) NULL
        //
        //      Invalid for NeverNull
        //
        // 2) DUPLICATE <num>
        //
        //      Invalid for Sealed/FixedType + NeverNull
        //
        // 3) PRIMITIVE <et>
        //
        //      <et>      optional for FixedType
        //      PRIMITIVE optional for FixedType + NeverNull
        //
        // 4) REFERENCE <type>
        //
        //      <type>    optional for Sealed/FixedType
        //      REFERENCE optional for Sealed/FixedType + NeverNull
        //
        // 5) ARRAYLIST <length>
        //
        //      <length>  optional for FixedSize
        //      ARRAYLIST optional for FixedType + NeverNull
        //
        // 6) ARRAY <depth> <baseType> <length>
        //
        //      <length>           optional for FixedSize
        //      <depth> <baseType> optional for FixedType
        //      ARRAY              optional for FixedType + NeverNull
        //
        // 7) REFLECTION <CLR_RT_ReflectionDef_Index>
        //
        //      <CLR_RT_ReflectionDef_Index> only valid for marshaling
        //
        // 8) MBRO <CLR_RT_HeapBlock*> <CLR_RT_AppDomain*>
        //      <CLR_RT_HeapBlock*> pointer to the DATATYPE_CLASS of the MarshalByRefObject on the object heap
        //                          It is the responsibility of the caller (CLR_RT_AppDomain::MarshalObject) to ensure that 
        //                          this pointer is kept alive, and not moved during the marshaling process
        //      <CLR_RT_AppDomain*> pointer to the AppDomain that this MBRO object lives in.
        //                          It is the responsiblity of the caller (CLR_RT_AppDomain::MarshalObject) to ensure that
        //                          the AppDomain does not get unloaded during the marshaling process.
        //

        // Always match type if FixedTyped is specified.
        //
        static const int c_Signature_Header = 0x01;
        static const int c_Signature_Type   = 0x02;
        static const int c_Signature_Length = 0x04;

        static const int c_Action_None           = 0;
        static const int c_Action_ObjectData     = 1;
        static const int c_Action_ObjectFields   = 2;
        static const int c_Action_ObjectElements = 3;

        //--//

        CLR_RT_HeapBlock*           m_value;
        SerializationHintsAttribute m_hints;
        CLR_RT_TypeDescriptor*      m_type;

        CLR_RT_TypeDescriptor*      m_typeExpected;
        CLR_RT_TypeDescriptor*      m_typeForced;

        CLR_RT_BinaryFormatter*     m_bf;

        CLR_RT_HeapBlock            m_value_tmp;
        CLR_RT_TypeDescriptor       m_type_tmp;
        CLR_RT_TypeDescriptor       m_typeExpected_tmp;
        CLR_RT_TypeDescriptor       m_typeForced_tmp;

#if defined(NANOCLR_APPDOMAINS)
        bool                        m_fIsMarshalByRefObject;
#endif

        //--//

        __nfweak HRESULT TypeHandler_Initialize( CLR_RT_BinaryFormatter* bf, SerializationHintsAttribute* hints, CLR_RT_TypeDescriptor* expected );

        __nfweak HRESULT SetValue( CLR_RT_HeapBlock* v );

        //--//

        __nfweak int SignatureRequirements();

        __nfweak bool CompareTypes( CLR_RT_TypeDescriptor* left, CLR_RT_TypeDescriptor* right );

        __nfweak static CLR_DataType GetDataType  ( CLR_RT_TypeDescriptor* type );
        __nfweak static CLR_UINT32   GetSizeOfType( CLR_RT_TypeDescriptor* type );
        __nfweak static bool         GetSignOfType( CLR_RT_TypeDescriptor* type );

        __nfweak static CLR_RT_HeapBlock* FixDereference( CLR_RT_HeapBlock* v );
        __nfweak static CLR_RT_HeapBlock* FixNull       ( CLR_RT_HeapBlock* v );

        //--//

        __nfweak HRESULT EmitSignature      ( int& res                                                        );
        __nfweak HRESULT EmitSignature_Inner( int  mask, CLR_RT_TypeDescriptor* type, CLR_RT_HeapBlock* value );
        __nfweak HRESULT ReadSignature      ( int& res                                                        );
        __nfweak HRESULT EmitValue          ( int& res                                                        );
        __nfweak HRESULT ReadValue          ( int& res                                                        );

        __nfweak HRESULT TrackObject        ( int& res                                                        );
    };

    struct State : public CLR_RT_HeapBlock_Node // EVENT HEAP - NO RELOCATION -
    {
        CLR_RT_BinaryFormatter*      m_parent;

        bool                         m_value_NeedProcessing;
        TypeHandler                  m_value;

        bool                         m_fields_NeedProcessing;
        CLR_RT_TypeDef_Instance      m_fields_CurrentClass;
        int                          m_fields_CurrentField;
        CLR_RT_HeapBlock*            m_fields_Pointer;

        bool                         m_array_NeedProcessing;
        CLR_RT_TypeDescriptor*       m_array_ExpectedType;
        CLR_RT_TypeDescriptor        m_array_ExpectedType_Tmp;
        CLR_RT_HeapBlock_Array*      m_array;
        int                          m_array_CurrentPos;
        int                          m_array_LastPos;

        //--//

        __nfweak static HRESULT CreateInstance( CLR_RT_BinaryFormatter* parent, SerializationHintsAttribute* hints, CLR_RT_HeapBlock*      type );
        __nfweak static HRESULT CreateInstance( CLR_RT_BinaryFormatter* parent, SerializationHintsAttribute* hints, CLR_RT_TypeDescriptor* type );

        __nfweak void DestroyInstance();

        __nfweak HRESULT FindHints( SerializationHintsAttribute& hints, const CLR_RT_TypeDef_Instance&  cls );
        __nfweak HRESULT FindHints( SerializationHintsAttribute& hints, const CLR_RT_FieldDef_Instance& fld );

        //--//

        __nfweak HRESULT AssignAndFixBoxing( CLR_RT_HeapBlock& dst );

        __nfweak HRESULT GetValue                  ();
        __nfweak HRESULT SetValueAndDestroyInstance();

        //--//

        __nfweak HRESULT Advance                ();
        __nfweak HRESULT AdvanceToTheNextField  ();
        __nfweak HRESULT AdvanceToTheNextElement();
    };


    struct DuplicateTracker : public CLR_RT_HeapBlock_Node // EVENT HEAP - NO RELOCATION -
    {
        CLR_RT_HeapBlock* m_ptr;
        CLR_UINT32        m_idx;
    };

    //--//

    CLR_RT_HeapBlock_MemoryStream* m_stream;
    CLR_UINT32                     m_idx;
    CLR_UINT32                     m_lastTypeRead;
    CLR_RT_DblLinkedList           m_duplicates;            // EVENT HEAP - NO RELOCATION - list of CLR_RT_BinaryFormatter::DuplicateTracker
    CLR_RT_DblLinkedList           m_states;                // EVENT HEAP - NO RELOCATION - list of CLR_RT_BinaryFormatter::State
    
    bool                           m_fDeserialize;
    CLR_RT_HeapBlock               m_value;
    CLR_RT_TypeDescriptor          m_value_desc;

    CLR_UINT32                     m_flags;

    //--//

    __nfweak static bool SerializationEnabled();

    __nfweak static HRESULT CreateInstance( CLR_UINT8* buf, int len, CLR_RT_BinaryFormatter*& res );

    __nfweak void    DestroyInstance();
    __nfweak HRESULT Advance        ();

    __nfweak static HRESULT Serialize  ( CLR_RT_HeapBlock& refData, CLR_RT_HeapBlock& object        , CLR_RT_HeapBlock* cls                         , CLR_UINT32 flags );
    __nfweak static HRESULT Deserialize( CLR_RT_HeapBlock& refData, CLR_RT_HeapBlock& object        , CLR_RT_HeapBlock* cls, CLR_UINT32* unknownType, CLR_UINT32 flags );
    __nfweak static HRESULT Deserialize( CLR_RT_HeapBlock& refData, CLR_UINT8* data, CLR_UINT32 size, CLR_RT_HeapBlock* cls, CLR_UINT32* unknownType, CLR_UINT32 flags );

    __nfweak HRESULT           TrackDuplicate ( CLR_RT_HeapBlock* object );
    __nfweak CLR_UINT32        SearchDuplicate( CLR_RT_HeapBlock* object );
    __nfweak CLR_RT_HeapBlock* GetDuplicate   ( CLR_UINT32        idx    );

    //--//

    __nfweak int     BitsAvailable          (                                                  );

    __nfweak HRESULT ReadBits               (       CLR_UINT32&                 val, int bits  );
    __nfweak HRESULT WriteBits              (       CLR_UINT32                  val, int bits  );

    __nfweak HRESULT ReadBits               (       CLR_UINT64&                 val, int bits  );
    __nfweak HRESULT WriteBits              (       CLR_UINT64                  val, int bits  );

    __nfweak HRESULT ReadArray              (       CLR_UINT8*                  buf, int bytes );
    __nfweak HRESULT WriteArray             ( const CLR_UINT8*                  buf, int bytes );

    __nfweak HRESULT ReadCompressedUnsigned (       CLR_UINT32&                 val            );
    __nfweak HRESULT WriteCompressedUnsigned(       CLR_UINT32                  val            );

    __nfweak HRESULT ReadType               (       CLR_RT_ReflectionDef_Index& val            );
    __nfweak HRESULT WriteType              ( const CLR_RT_ReflectionDef_Index& val            );
    __nfweak HRESULT WriteType              (       CLR_UINT32                  val            );

    //--//

private:

    __nfweak static void PrepareForGC( void* data );
};

#endif // _NANOCLR_RUNTIME__SERIALIZATION_H_

