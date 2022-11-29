//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.IO;
using System.Linq;

namespace nanoFramework.nanoCLR.Host.Port.Common
{
    internal class StreamedConnection : IPort
    {
        private readonly Stream _stream;
        private readonly Func<int> _bytesAvailable;
        private readonly Action _releaseResources;

        public int BytesAvailable => Status == PortStatus.Opened ? _bytesAvailable() : 0;
        public PortStatus Status { get; private set; }

        public StreamedConnection(Stream stream, Func<int> bytesAvailable, Action releaseResources)
        {
            _stream = stream;
            _bytesAvailable = bytesAvailable;
            _releaseResources = releaseResources;
        }

        public byte[] ReceiveData()
        {
            var data = new byte[BytesAvailable];
            var actualRead = _stream.Read(data, 0, data.Length);
            return data.Take(actualRead).ToArray();
        }

        public void TransmitData(byte[] data)
        {
            _stream.Write(data);
        }

        public void Process()
        {
        }

        public void Open()
        {
            Status = PortStatus.Opened;
        }

        public void Close()
        {
            _stream.Close();
            _stream.Dispose();
            _releaseResources();
            Status = PortStatus.Closed;
        }
    }
}
