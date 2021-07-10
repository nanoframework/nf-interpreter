using System.Linq;

namespace nanoFramework.nanoCLR.Host.Port
{
    public class WireProtocolPort: IPort
    {
        public int BytesAvailable => Native.NanoClr_WireBytesAvailable();

        public byte[] ReceiveData()
        {
            var bytes = new byte[BytesAvailable];
            var received = Native.NanoClr_WireReceive(bytes, bytes.Length);
            return bytes.Take(received).ToArray();
        }

        public void TransmitData(byte[] data) => Native.NanoClr_WireTransmit(data, data.Length);

        public void Process() => Native.NanoClr_WireProcess();

        public void Open()
        {
            Native.NanoClr_WireOpen();
        }

        public void Close()
        {
            Native.NanoClr_WireOpen();
        }
    }
}
