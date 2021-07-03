using System;
using System.IO.Ports;
using System.Linq;

namespace nanoFramework.nanoCLR.Host
{
    public class SerialPortSink : IDeviceSink
    {
        private readonly bool _traceWire;
        private const int MaxBaud = 921600;

        private Action<byte[]> _transmitToDevice;
        private readonly SerialPort _serialPort;

        public SerialPortSink(string comPort, int baudRate = MaxBaud, bool traceWire = false)
        {
            _traceWire = traceWire;
            _serialPort = new SerialPort(comPort, baudRate);

            _serialPort.DataReceived += SerialPortOnDataReceived;
        }

        private void SerialPortOnDataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            var bytes = _serialPort.ReadAllBytes();
            if (_traceWire)
                Console.WriteLine($"Received from host: {HexDump.HexDump.Format(bytes)}");
            if (bytes.Any())
                _transmitToDevice(bytes);
        }

        public void Initialize(Action<byte[]> transmitToDevice)
        {
            _transmitToDevice = transmitToDevice;
        }

        public void ReceivedFromDevice(byte[] bytes)
        {
            if (_traceWire)
                Console.WriteLine($"Received from device: {HexDump.HexDump.Format(bytes)}");
            _serialPort.Write(bytes, 0, bytes.Length);
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
