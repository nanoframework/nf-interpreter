//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Graphics.h"
#include "TouchDevice.h"
#include "nanoFramework_Graphics.h"

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_TouchEventProcessor::ProcessEvent___nanoFrameworkRuntimeEventsBaseEvent__U4__U4__SystemDateTime(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();
    hr = S_OK;

    CLR_UINT32              data1 = stack.Arg1().NumericByRef().u4;
    CLR_UINT32              numTouches = data1 >> 16;
    TouchPoint* touchPoint = (TouchPoint*)stack.Arg2().NumericByRef().u4;
    CLR_RT_HeapBlock* touchEvent = NULL;
    CLR_RT_HeapBlock_Array* touchInputArray = NULL;
    CLR_RT_HeapBlock* touchInputObject = NULL;

    //Create a Microsoft.SPOT.Touch.TouchEvent object to return:
    CLR_RT_HeapBlock& resultObject = stack.PushValue();

    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(resultObject, g_CLR_RT_WellKnownTypes.m_TouchEvent));

    touchEvent = resultObject.Dereference(); if (!touchEvent) { NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY); }

    //Initialize the fields of the TouchEvent object.
    touchEvent[Library_nf_rt_events_native_nanoFramework_Runtime_Events_BaseEvent::FIELD__Source].SetInteger((CLR_UINT16)0);
    touchEvent[Library_nanoFramework_Graphics_nanoFramework_UI_Input_RawTouchInputReport::FIELD__EventMessage].SetInteger((CLR_UINT8)data1 & 0x00ff);
    touchEvent[Library_nanoFramework_Graphics_nanoFramework_UI_TouchEvent::FIELD__Time].SetInteger(stack.Arg3().NumericByRef().s8);
    touchEvent[Library_nanoFramework_Graphics_nanoFramework_UI_TouchEvent::FIELD__Time].ChangeDataType(DATATYPE_DATETIME);

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(touchEvent[Library_nanoFramework_Graphics_nanoFramework_UI_TouchEvent::FIELD__Touches],
        numTouches,
        g_CLR_RT_WellKnownTypes.m_TouchInput));

    touchInputArray = touchEvent[Library_nanoFramework_Graphics_nanoFramework_UI_TouchEvent::FIELD__Touches].DereferenceArray();
    if (!touchInputArray) { NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY); }

    //Marshall the TouchPoint array from the PAL into the TouchInput[] managed array.
    touchInputObject = (CLR_RT_HeapBlock*)touchInputArray->GetFirstElement();
    for (; numTouches > 0; --numTouches, touchInputObject++, touchPoint++)
    {
        g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*touchInputObject, g_CLR_RT_WellKnownTypes.m_TouchInput);
        CLR_RT_HeapBlock* touchInput = touchInputObject->Dereference(); if (!touchInput) { NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY); }

        CLR_UINT32 location = touchPoint->location;
        touchInput[Library_nanoFramework_Graphics_nanoFramework_UI_TouchInput::FIELD__X].SetInteger((CLR_INT32)(location & 0x3fff));
        touchInput[Library_nanoFramework_Graphics_nanoFramework_UI_TouchInput::FIELD__Y].SetInteger((CLR_INT32)((location >> 14) & 0x3fff));
        touchInput[Library_nanoFramework_Graphics_nanoFramework_UI_TouchInput::FIELD__SourceID].SetInteger((CLR_UINT8)(location >> 28));

        CLR_UINT32 contact = touchPoint->contact;
        CLR_UINT32 flags = Library_nanoFramework_Graphics_nanoFramework_UI_TouchInput::FLAG__None;

        if ((contact & TouchPointContactFlags_Primary) != 0) { flags |= Library_nanoFramework_Graphics_nanoFramework_UI_TouchInput::FLAG__Primary; }
        if ((contact & TouchPointContactFlags_Pen) != 0) { flags |= Library_nanoFramework_Graphics_nanoFramework_UI_TouchInput::FLAG__Pen; }
        if ((contact & TouchPointContactFlags_Palm) != 0) { flags |= Library_nanoFramework_Graphics_nanoFramework_UI_TouchInput::FLAG__Palm; }

        touchInput[Library_nanoFramework_Graphics_nanoFramework_UI_TouchInput::FIELD__Flags].SetInteger((CLR_UINT32)flags);
        touchInput[Library_nanoFramework_Graphics_nanoFramework_UI_TouchInput::FIELD__ContactWidth].SetInteger((CLR_UINT32)(contact & 0x3ff));
        touchInput[Library_nanoFramework_Graphics_nanoFramework_UI_TouchInput::FIELD__ContactHeight].SetInteger((CLR_UINT32)((contact >> 14) & 0x3ff));
    }

    NANOCLR_NOCLEANUP();
}
