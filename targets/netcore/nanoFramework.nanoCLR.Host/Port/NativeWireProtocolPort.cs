//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System.Collections.Generic;
using System.Threading;

namespace nanoFramework.nanoCLR.Host.Port
{
    internal class NativeWireProtocolPort : IPort
    {
        private readonly object _syncRoot = new object();
        private readonly List<byte> _receiveBuffer = new List<byte>();
        private readonly List<byte> _transmitBuffer = new List<byte>();
        private Thread _processingThread;

        private readonly Interop.nanoCLR.WireReceiveDelegate _wireReceiveCallback;
        private readonly Interop.nanoCLR.WireTransmitDelegate _wireTransmitCallback;

        public NativeWireProtocolPort()
        {
            _wireReceiveCallback = WireProtocolReceiveCallback;
            _wireTransmitCallback = WireProtocolTransmitCallback;
        }

        public PortStatus Status { get; private set; }

        public int BytesAvailable => _syncRoot.Sync(() => _transmitBuffer.Count);

        public byte[] ReceiveData()
        {
            lock (_syncRoot)
            {
                var data = _transmitBuffer.ToArray();
                _transmitBuffer.Clear();

                return data;
            }
        }

        public void TransmitData(byte[] data)
        {
            lock (_syncRoot)
            {
                _receiveBuffer.AddRange(data);

                Monitor.PulseAll(_syncRoot);
            }
        }

        public void Process()
        {
            // empty on purpose, nothing to process here
        }

        public void Open()
        {
            Interop.nanoCLR.nanoCLR_SetWireProtocolReceiveCallback(_wireReceiveCallback);
            Interop.nanoCLR.nanoCLR_SetWireProtocolTransmitCallback(_wireTransmitCallback);
            Interop.nanoCLR.nanoCLR_WireProtocolOpen();

            Status = PortStatus.Opened;

            _processingThread = new Thread(ProcessMessages);
            _processingThread.Start();
        }

        public void Close()
        {
            lock (_syncRoot)
            {
                Status = PortStatus.Closed;

                Interop.nanoCLR.nanoCLR_WireProtocolClose();
                Interop.nanoCLR.nanoCLR_SetWireProtocolReceiveCallback(null);
                Interop.nanoCLR.nanoCLR_SetWireProtocolTransmitCallback(null);
            }
        }

        private int WireProtocolTransmitCallback(byte[] data, int length)
        {
            lock (_syncRoot)
            {
                _transmitBuffer.AddRange(data);

                return length;
            }
        }


        private int WireProtocolReceiveCallback(byte[] data, int length)
        {
            lock (_syncRoot)
            {
                if (_receiveBuffer.Count == 0
                    || Status == PortStatus.Closed)
                {
                    Thread.Yield();

                    return 0;
                }

                var size = _receiveBuffer.Count < length ? _receiveBuffer.Count : length;

                _receiveBuffer.CopyTo(0, data, 0, size);
                _receiveBuffer.RemoveRange(0, size);

                return size;
            }
        }

        private void ProcessMessages()
        {
            while (Status == PortStatus.Opened)
            {
                Interop.nanoCLR.nanoCLR_WireProtocolProcess();
                Thread.Yield();
            }
        }
    }
}
