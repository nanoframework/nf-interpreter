namespace nanoFramework.nanoCLR.Host.Port
{
    public class SerialPort : IPort
    {
        private const int MaxBaud = 921600;

        private readonly System.IO.Ports.SerialPort _serialPort;

        public SerialPort(string name, int baudRate = MaxBaud)
        {
            _serialPort = new System.IO.Ports.SerialPort(name, baudRate);
        }

        public int BytesAvailable => _serialPort.BytesToRead;

        public byte[] ReceiveData() => _serialPort.ReadAllBytes();

        public void TransmitData(byte[] data) => _serialPort.Write(data, 0, data.Length);

        public void Process()
        {
        }

        public void Open()
        {
            _serialPort.Open();
        }

        public void Close()
        {
            _serialPort.Close();
        }
    }
}
