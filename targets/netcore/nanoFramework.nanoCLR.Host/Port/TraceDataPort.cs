//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System;

namespace nanoFramework.nanoCLR.Host.Port
{
    internal class TraceDataPort : IPort
    {
        private readonly IPort _target;

        public PortStatus Status => _target.Status;
        public int BytesAvailable => _target.BytesAvailable;

        public TraceDataPort(IPort target)
        {
            _target = target;
        }

        public byte[] ReceiveData() => TraceData("Wire Data Received", _target.ReceiveData());

        public void TransmitData(byte[] data) => _target.TransmitData(TraceData("Wire Data Transmitted", data));

        public void Process() => _target.Process();

        public void Open() => _target.Open();

        public void Close() => _target.Open();

        private byte[] TraceData(string message, byte[] data)
        {
            if (data.Length == 0)
            {
                return data;
            }

            Console.WriteLine($"{message}:");
            Console.WriteLine(HexDump.HexDump.Format(data));
            return data;
        }
    }
}
