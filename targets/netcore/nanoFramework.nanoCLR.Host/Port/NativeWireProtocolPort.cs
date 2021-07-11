//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

using System.Linq;
using nanoFramework.nanoCLR.Host.Interop;

namespace nanoFramework.nanoCLR.Host.Port
{
    public class NativeWireProtocolPort : IPort
    {
        public PortStatus Status { get; private set; }

        public int BytesAvailable => Native.NanoClr_WireBytesAvailable();

        public byte[] ReceiveData()
        {
            var bytes = new byte[BytesAvailable];
            var received = Native.NanoClr_WireReceive(bytes, bytes.Length);
            return bytes.Take(received).ToArray();
        }

        public void TransmitData(byte[] data)
        {
            if(data.Length >0)
                Native.NanoClr_WireTransmit(data, data.Length);
        }

        public void Process() => Native.NanoClr_WireProcess();

        public void Open()
        {
            Native.NanoClr_WireOpen();
            Status = PortStatus.Opened;
        }

        public void Close()
        {
            Native.NanoClr_WireOpen();
            Status = PortStatus.Closed;
        }
    }
}
