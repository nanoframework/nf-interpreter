//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"

HRESULT Library_corlib_native_System_Runtime_CompilerServices_RuntimeHelpers::
    InitializeArray___STATIC__VOID__SystemArray__SystemRuntimeFieldHandle(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_FieldDef_Instance inst;
    CLR_RT_HeapBlock_Array *array = stack.Arg0().DereferenceArray();
    FAULT_ON_NULL(array);

    if (array->m_fReference)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (CLR_RT_ReflectionDef_Index::Convert(stack.Arg1(), inst) == false)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    if ((inst.target->flags & CLR_RECORD_FIELDDEF::FD_HasFieldRVA) == 0 || inst.target->defaultValue == CLR_EmptyIndex)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    {
        CLR_PMETADATA ptrSrc = inst.assembly->GetSignature(inst.target->defaultValue);
        CLR_UINT32 lenSrc;
        NANOCLR_READ_UNALIGNED_UINT16(lenSrc, ptrSrc);

        CLR_UINT8 *ptrDst = array->GetFirstElement();
        CLR_UINT32 lenDst = array->m_numOfElements;
        CLR_UINT32 sizeDst = array->m_sizeOfElement;

        lenSrc /= sizeDst;
        if (lenSrc > lenDst)
            lenSrc = lenDst;
#if !defined(NETMF_TARGET_BIG_ENDIAN)
        memcpy(ptrDst, ptrSrc, lenSrc * sizeDst);

#if defined(NANOCLR_EMULATED_FLOATINGPOINT)
        switch (array->m_typeOfElement)
        {
            case DATATYPE_R4:
            {
                CLR_RT_HeapBlock tmp;
                CLR_UINT32 *ptr = (CLR_UINT32 *)ptrDst;

                for (; lenSrc; lenSrc--, ptr++)
                {
                    NANOCLR_CHECK_HRESULT(tmp.SetFloatIEEE754(*ptr));

                    *ptr = tmp.NumericByRef().u4;
                }
            }
            break;

            case DATATYPE_R8:
            {
                CLR_RT_HeapBlock tmp;
                CLR_UINT64 *ptr = (CLR_UINT64 *)ptrDst;

                for (; lenSrc; lenSrc--, ptr++)
                {
                    NANOCLR_CHECK_HRESULT(tmp.SetDoubleIEEE754(*ptr));

                    *ptr = tmp.NumericByRef().u8;
                }
            }
            break;
        }
#endif

#else
        // FIXME GJS - WOuld it be possible to move the endian swap to pe compile time to get rid of this?
        // If this is a numeric dataype of datatype size other than a byte then byteswap the entries
        // Unaligned reads handle endianess, just use them. FIXME GJS - this could be the subject of much optimization
        switch (array->m_typeOfElement)
        {
            case DATATYPE_CHAR:
            case DATATYPE_I2:
            case DATATYPE_U2:
            {
                CLR_UINT32 count = 0;
                CLR_UINT16 d;
                CLR_UINT16 *p16 = (CLR_UINT16 *)ptrDst;
                while (count < lenDst)
                {
                    NANOCLR_READ_UNALIGNED_UINT16(d, ptrSrc);
                    *p16++ = SwapEndian(d);
                    count++;
                }
            }
            break;

            case DATATYPE_I4:
            case DATATYPE_U4:
            case DATATYPE_R4:
            {
                CLR_UINT32 count = 0;
                CLR_UINT32 d;
                CLR_UINT32 *p32 = (CLR_UINT32 *)ptrDst;
                while (count < lenDst)
                {
                    NANOCLR_READ_UNALIGNED_UINT32(d, ptrSrc);
                    *p32++ = SwapEndian(d);
                    count++;
                }
            }
            break;
            case DATATYPE_I8:
            case DATATYPE_U8:
            case DATATYPE_R8:
            {
                CLR_UINT32 count = 0;
                CLR_UINT64 d;
                CLR_UINT64 *p64 = (CLR_UINT64 *)ptrDst;

                while (count < lenDst)
                {
                    NANOCLR_READ_UNALIGNED_UINT64(d, ptrSrc);
                    *p64++ = SwapEndian(d);
                    count++;
                }
            }
            break;
            default:
                memcpy(ptrDst, ptrSrc, lenSrc * sizeDst);
                break;
        }

#if defined(NANOCLR_EMULATED_FLOATINGPOINT)
        switch (array->m_typeOfElement)
        {
            case DATATYPE_R4:
            {
                CLR_RT_HeapBlock tmp;
                CLR_UINT32 *ptr = (CLR_UINT32 *)ptrDst;

                for (; lenSrc; lenSrc--, ptr++)
                {
                    NANOCLR_CHECK_HRESULT(tmp.SetFloatIEEE754(*ptr));

                    *ptr = tmp.NumericByRef().u4;
                }
            }
            break;

            case DATATYPE_R8:
            {
                CLR_RT_HeapBlock tmp;
                CLR_UINT64 *ptr = (CLR_UINT64 *)ptrDst;

                for (; lenSrc; lenSrc--, ptr++)
                {
                    NANOCLR_CHECK_HRESULT(tmp.SetDoubleIEEE754(*ptr));

                    *ptr = tmp.NumericByRef().u8;
                }
            }
            break;
        }
#endif
#endif // NETMF_TARGET_BIG_ENDIAN
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Runtime_CompilerServices_RuntimeHelpers::GetObjectValue___STATIC__OBJECT__OBJECT(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock &top = stack.PushValueAndClear();
    CLR_RT_HeapBlock *src = stack.Arg0().Dereference();

    if (src && src->DataType() == DATATYPE_VALUETYPE && src->IsBoxed())
    {
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.CloneObject(top, *src));
    }
    else
    {
        top.SetObjectReference(src);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Runtime_CompilerServices_RuntimeHelpers::
    RunClassConstructor___STATIC__VOID__SystemRuntimeTypeHandle(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Runtime_CompilerServices_RuntimeHelpers::get_OffsetToStringData___STATIC__I4(
    CLR_RT_StackFrame &stack)
{
    (void)stack;

    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

    NANOCLR_NOCLEANUP();
}

#if (NANOCLR_REFLECTION == TRUE)

HRESULT Library_corlib_native_System_Runtime_CompilerServices_RuntimeHelpers::
    IsReferenceOrContainsReferences___STATIC__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    bool isRefContainsRefs = false;

    // exceptionally need to push the return value
    CLR_RT_HeapBlock &top = stack.PushValueAndClear();

    // grab caller
    CLR_RT_MethodDef_Instance &caller = stack.MethodCall();

    // parse the method signature to grab the generic argument
    CLR_RT_MethodSpec_Instance thisMethodSpecInst;
    thisMethodSpecInst.InitializeFromIndex(caller.methodSpec);

    CLR_RT_SignatureParser parserCaller;
    parserCaller.Initialize_MethodSignature(&thisMethodSpecInst);

    CLR_RT_SignatureParser::Element element;

    // advance to the 1st (and only parameter)
    parserCaller.Advance(element);

    if (element.DataType == DATATYPE_GENERICINST)
    {
        parserCaller.Advance(element);
    }

    NANOCLR_CHECK_HRESULT(
        CheckReferenceOrContainsReferences(element.Class, element.DataType, &parserCaller, isRefContainsRefs));

    top.SetBoolean(isRefContainsRefs);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Runtime_CompilerServices_RuntimeHelpers::CheckReferenceOrContainsReferences(
    const CLR_RT_TypeDef_Index &cls,
    NanoCLRDataType dt,
    CLR_RT_SignatureParser *parserCaller,
    bool &isRefContainsRefs)
{
    NANOCLR_HEADER();

    if (dt <= DATATYPE_LAST_NONPOINTER)
    {
        // primitive types aren't and don't contain references
        isRefContainsRefs |= false;
    }
    else if (dt == DATATYPE_CLASS || dt == DATATYPE_VALUETYPE)
    {
        // for class and value types need to check the instance fields

        // get the type definition
        CLR_RT_TypeDef_Instance inst{};
        inst.InitializeFromIndex(cls);

        const CLR_RECORD_FIELDDEF *targetField = nullptr;
        CLR_RT_Assembly *assm = nullptr;
        CLR_RT_TypeDef_Instance instSub = inst;

        int clsFields = instSub.target->instanceFieldsCount;
        int totFields = inst.CrossReference().totalFields;

        if (totFields == 0)
        {
            // this class has no fields, therefore doesn't contain references
            isRefContainsRefs |= false;
        }

        // Crawl all instance fields from last to first
        while (--totFields >= 0)
        {
            while (clsFields == 0)
            {
                if (instSub.SwitchToParent() == false)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
                }

                clsFields = instSub.target->instanceFieldsCount;
                targetField = nullptr;
            }

            if (targetField == nullptr)
            {
                assm = instSub.assembly;
                targetField = assm->GetFieldDef(instSub.target->firstInstanceField + clsFields);
            }

            // move to the previous field (GetFieldDef returns a pointer one past the last when indexed this way)
            targetField--;
            clsFields--;

#if defined(NANOCLR_INSTANCE_NAMES)
            const char *typeName = assm->GetString(targetField->type);
            const char *fieldName = assm->GetString(targetField->name);
#endif
            // parse the field signature to discover its element type/levels
            CLR_RT_SignatureParser parser{};
            parser.Initialize_FieldDef(assm, targetField);

            CLR_RT_SignatureParser::Element element;
            NANOCLR_CHECK_HRESULT(parser.Advance(element));

            if (element.DataType == DATATYPE_MVAR)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
            }
            else if (element.DataType == DATATYPE_VAR)
            {
                CLR_RT_SignatureParser::Element typeElement;

                // get the type from the caller's generic type
                for (int paramIndex = 0; paramIndex <= element.GenericParamPosition; paramIndex++)
                {
                    parserCaller->Advance(typeElement);
                }

                NANOCLR_CHECK_HRESULT(CheckReferenceOrContainsReferences(
                    typeElement.Class,
                    typeElement.DataType,
                    parserCaller,
                    isRefContainsRefs));

                // has levels, therefore it's an array
                isRefContainsRefs |= true;
            }
            else
            {
                // recurse using the element information
                NANOCLR_CHECK_HRESULT(CheckReferenceOrContainsReferences(
                    element.Class,
                    element.DataType,
                    parserCaller,
                    isRefContainsRefs));
            }
        }
    }
    else
    {
        // everything else are or contain references
        isRefContainsRefs |= true;
    }

    NANOCLR_NOCLEANUP();
}

#endif
