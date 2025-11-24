//-----------------------------------------------------------------------------
//
//                   ** WARNING! ** 
//    This file was generated automatically by a tool.
//    Re-running the tool will overwrite this file.
//    You should copy this file to a custom location
//    before adding any customization in the copy to
//    prevent loss of your changes when the tool is
//    re-run.
//
//-----------------------------------------------------------------------------

#ifndef COM_SKYWORKSINC_NANOFRAMEWORK_DEVICES_C1_COM_SKYWORKSINC_NANOFRAMEWORK_DEVICES_C1_COM_SKYWORKSINC_NANOFRAMEWORK_DEVICES_C1_C1BUS_H
#define COM_SKYWORKSINC_NANOFRAMEWORK_DEVICES_C1_COM_SKYWORKSINC_NANOFRAMEWORK_DEVICES_C1_COM_SKYWORKSINC_NANOFRAMEWORK_DEVICES_C1_C1BUS_H

namespace Com_SkyworksInc_NanoFramework_Devices_C1
{
    namespace Com_SkyworksInc_NanoFramework_Devices_C1
    {
        struct C1Bus
        {
            // Helper Functions to access fields of managed object
            // Declaration of stubs. These functions are implemented by Interop code developers

            static void NativeTransmitWriteWithAddress( uint16_t param0, uint8_t param1, uint8_t param2, CLR_RT_TypedArray_UINT8 param3, HRESULT &hr );

            static void NativeTransmitReadWithAddress( uint16_t param0, uint8_t param1, CLR_RT_TypedArray_UINT8 param2, HRESULT &hr );

            static void NativeTransmitRead( uint16_t param0, CLR_RT_TypedArray_UINT8 param1, HRESULT &hr );

            static void NativeTransmitWrite( uint16_t param0, uint8_t param1, CLR_RT_TypedArray_UINT8 param2, HRESULT &hr );

            static void NativeTransmitWriteAddress( uint16_t param0, uint8_t param1, CLR_RT_TypedArray_UINT8 param2, HRESULT &hr );

            static void NativeTransmitReadAddress( uint16_t param0, CLR_RT_TypedArray_UINT8 param1, HRESULT &hr );

        };
    }
}

#endif // COM_SKYWORKSINC_NANOFRAMEWORK_DEVICES_C1_COM_SKYWORKSINC_NANOFRAMEWORK_DEVICES_C1_COM_SKYWORKSINC_NANOFRAMEWORK_DEVICES_C1_C1BUS_H
