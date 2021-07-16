using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text.RegularExpressions;
using hhdvspkit;

namespace nanoFramework.nanoCLR.VirtualCom
{
    public class VirtualComManager
    {
        private const string LicenseResourceName = "nanoFramework.nanoCLR.VirtualCom.License.vspt.vsptlic";
        private static Regex s_PairRegex = new(@"COM(\d+):COM(\d+)", RegexOptions.IgnoreCase);

        public bool IsSupported { get; private set; }
        private SerialPortLibrary _serialPortLibrary = null;

        public VirtualComManager()
        {
        }

        public bool Initialize()
        {
            try
            {
                _serialPortLibrary = new SerialPortLibrary();
                InstallLicense();
                IsSupported = true;
            }
            catch (Exception e)
            {
                IsSupported = false;
                return false;
            }

            return true;
        }

        IBridgePortDevice[] GetBridgePorts() =>
            _serialPortLibrary.getPorts(SerialPortType.Bridge).Cast<IBridgePortDevice>().ToArray();

        IBridgePortDevice GetBridgePort(int port) => GetBridgePorts().FirstOrDefault(p => p.port == port);

        public IEnumerable<PortPair> GetPortPairs()
        {
            IBridgePortDevice[] bridgePorts = GetBridgePorts();
            var pairs = new List<PortPair>();

            foreach (var port in bridgePorts)
            {
                if (port.bridgePort > 0 && pairs.All(b => b.PortA.port != port.bridgePort))
                {
                    pairs.Add(new PortPair {PortA = port, PortB = bridgePorts.First(p => p.port == port.bridgePort)});
                }
            }

            return pairs;
        }

        public PortPair CreatePair(int comA, int comB)
        {
            try
            {
                IBridgePortDevice portA = GetBridgePort(comA) ?? _serialPortLibrary.createBridgePort(comA);
                IBridgePortDevice portB = GetBridgePort(comB) ?? _serialPortLibrary.createBridgePort(comB);
                portA.bridgePort = portB.port;
                portB.bridgePort = portA.port;
                return new PortPair { PortA = portA, PortB = portB };
            }
            catch (Exception e)
            {
                throw new Exception($@"Cannot Create Port Pair. {e.Message}", e);
            }
        }

        public PortPair CreatePair(string pair)
        {
            var (comA, comB) = ParsePair(pair);
            return CreatePair(comA, comB);
        }

        public void DeletePair(PortPair pair)
        {
            //Not working due to interop issues between 0x86/0x64 code. Error: 0xE0000235. Vendor issue?
            try
            {
                pair.PortA.deleteDevice();
                pair.PortB.deleteDevice();
            }
            catch (Exception e)
            {
                throw new Exception($@"Cannot Delete Port Pair. {e.Message}", e);
            }
        }

        public void DeletePair(string name)
        {
            PortPair pair = GetPairByName(name);
            if(pair == null)
                throw new ArgumentException($"Virtual Com Pair {name.ToUpper()} not found.");

            DeletePair(pair);
        }

        public (int comA, int comB) ParsePair(string pair)
        {
            Match match = s_PairRegex.Match(pair);
            if (!match.Success)
                throw new ArgumentException(
                    $"Incorrect Virtual Com Pair name {pair.ToUpper()}. Correct example COM5:COM6");

            var comA = int.Parse(match.Groups[1].Value);
            var comB = int.Parse(match.Groups[2].Value);
            return (comA, comB);
        }

        public PortPair GetPairByName(string pair)
        {
            var (comA, comB) = ParsePair(pair);

            IBridgePortDevice portA = GetBridgePort(comA);
            IBridgePortDevice portB = GetBridgePort(comB);

            if (portA == null || portB == null)
            {
                return null;
            }

            return new PortPair {PortA = portA, PortB = portB};
        }

        private void InstallLicense()
        {
            var assembly = typeof(VirtualComManager).GetTypeInfo().Assembly;
            Stream resource = assembly.GetManifestResourceStream(LicenseResourceName);
            MemoryStream memoryStream = new MemoryStream();
            resource.CopyTo(memoryStream);
            var bytes = memoryStream.ToArray();
            if (bytes.Length > 0)
            {
                _serialPortLibrary.installLicenseInMemory(bytes);
            }
        }
    }
}
