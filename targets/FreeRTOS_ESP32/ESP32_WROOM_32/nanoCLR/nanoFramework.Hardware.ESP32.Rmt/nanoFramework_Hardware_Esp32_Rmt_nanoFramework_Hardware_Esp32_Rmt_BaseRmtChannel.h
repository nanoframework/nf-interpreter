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
#ifndef _NANOFRAMEWORK_HARDWARE_ESP32_RMT_NANOFRAMEWORK_HARDWARE_ESP32_RMT_BASERMTCHANNEL_H_
#define _NANOFRAMEWORK_HARDWARE_ESP32_RMT_NANOFRAMEWORK_HARDWARE_ESP32_RMT_BASERMTCHANNEL_H_

namespace nanoFramework
{
    namespace Hardware
    {
        namespace Esp32
        {
            namespace Rmt
            {
                struct BaseRmtChannel
                {
                    // Helper Functions to access fields of managed object
                    static signed int& Get__channel( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_INT32( pMngObj, Library_nanoFramework_Hardware_Esp32_Rmt_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::FIELD___channel ); }

                    // Declaration of stubs. These functions are implemented by Interop code developers
                    static void NativeDispose( signed int param0, HRESULT &hr );
                    static signed int NativeGetChannelStatus( signed int param0, HRESULT &hr );
                    static unsigned char NativeGetClockDivider( signed int param0, HRESULT &hr );
                    static unsigned char NativeGetMemoryBlockNumber( signed int param0, HRESULT &hr );
                    static bool NativeGetMemoryLowPower( signed int param0, HRESULT &hr );
                    static signed int NativeGetSourceClock( signed int param0, HRESULT &hr );
                    static void NativeSetClockDivider( signed int param0, unsigned char param1, HRESULT &hr );
                    static void NativeSetMemoryBlockNumber( signed int param0, unsigned char param1, HRESULT &hr );
                    static void NativeSetMemoryLowPower( signed int param0, bool param1, HRESULT &hr );
                    static void NativeSetSourceClock( signed int param0, signed int param1, HRESULT &hr );
                };
            }
        }
    }
}
#endif  //_NANOFRAMEWORK_HARDWARE_ESP32_RMT_NANOFRAMEWORK_HARDWARE_ESP32_RMT_BASERMTCHANNEL_H_
