//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System.Linq;
using nanoFramework.nanoCLR.Host.Interop;

namespace nanoFramework.nanoCLR.Host.Port
{
    public class NativeWireProtocolPort : IPort
    {
        public PortStatus Status { get; private set; }

        public int BytesAvailable => Interop.nanoCLR.nanoCLR_WireBytesAvailable();

        public byte[] ReceiveData()
        {
            var bytes = new byte[BytesAvailable];
            var received = Interop.nanoCLR.nanoCLR_WireReceive(bytes, bytes.Length);
            return bytes.Take(received).ToArray();
        }

        public void TransmitData(byte[] data)
        {
            if (data.Length > 0)
            {
                Interop.nanoCLR.nanoCLR_WireTransmit(data, data.Length);
            }
        }

        public void Process() => Interop.nanoCLR.nanoCLR_WireProcess();

        public void Open()
        {
            Interop.nanoCLR.nanoCLR_WireOpen();
            Status = PortStatus.Opened;
        }

        public void Close()
        {
            Interop.nanoCLR.nanoCLR_WireOpen();
            Status = PortStatus.Closed;
        }
    }
}
