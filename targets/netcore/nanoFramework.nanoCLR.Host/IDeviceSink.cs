using System;

namespace nanoFramework.nanoCLR.Host
{
    public interface IDeviceSink
    {
        void Initialize(Action<byte[]> transmitToDevice);
        void ReceivedFromDevice(byte[] bytes);
        void Open();
        void Close();
    }
}
