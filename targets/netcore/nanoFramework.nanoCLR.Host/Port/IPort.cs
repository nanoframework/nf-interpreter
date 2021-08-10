//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

namespace nanoFramework.nanoCLR.Host.Port
{
    public interface IPort
    {
        public PortStatus Status { get; }
        int BytesAvailable { get; }
        byte[] ReceiveData();
        void TransmitData(byte[] data);
        void Process();
        void Open();
        void Close();
    }

    public enum PortStatus
    {
        Idle,
        Opened,
        Closed
    }
}
