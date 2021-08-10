//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using hhdvspkit;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text.RegularExpressions;

namespace nanoFramework.nanoCLR.VirtualBridge
{
    public class VirtualBridgeManager
    {
        private const string LicenseResourceName = "nanoFramework.nanoCLR.CLI.License.vspt.vsptlic";
        private static Regex s_PairRegex = new(@"COM(\d+):COM(\d+)", RegexOptions.IgnoreCase);

        internal const string VirtualBridgeVerb = "virtualbridge";

        /// <summary>
        /// If Virtual Serial Port Tools are installed this property is <see langword="true"/>, otherwise <see langword="false"/>.
        /// </summary>
        public bool IsInstalled { get; private set; }
        
        private SerialPortLibrary _serialPortLibrary = null;

        public VirtualBridgeManager()
        {
        }

        public bool Initialize()
        {
            try
            {
                _serialPortLibrary = new SerialPortLibrary();
                InstallLicense();
                IsInstalled = true;
            }
            catch (Exception e)
            {
                IsInstalled = false;
                return false;
            }

            return true;
        }

        private IBridgePortDevice[] GetVirtualBridges() =>
            _serialPortLibrary.getPorts(SerialPortType.Bridge).Cast<IBridgePortDevice>().ToArray();

        private IBridgePortDevice GetBridgeContainingPort(int port) => GetVirtualBridges().FirstOrDefault(p => p.port == port);

        public IEnumerable<VirtualBridge> GetAvailableVirtualBridges()
        {
            IBridgePortDevice[] bridgePorts = GetVirtualBridges();

            var pairs = new List<VirtualBridge>();

            foreach (var port in bridgePorts)
            {
                if (port.bridgePort > 0 && pairs.All(b => b.PortA.port != port.bridgePort))
                {
                    pairs.Add(new VirtualBridge {PortA = port, PortB = bridgePorts.First(p => p.port == port.bridgePort)});
                }
            }

            return pairs;
        }

        public VirtualBridge CreateVirtualBridge(int comA, int comB)
        {
            try
            {
                IBridgePortDevice portA = GetBridgeContainingPort(comA) ?? _serialPortLibrary.createBridgePort(comA);
                IBridgePortDevice portB = GetBridgeContainingPort(comB) ?? _serialPortLibrary.createBridgePort(comB);

                portA.bridgePort = portB.port;
                portB.bridgePort = portA.port;

                return new VirtualBridge { PortA = portA, PortB = portB };
            }
            catch (Exception e)
            {
                throw new Exception($@"Cannot create Virtual Bridge. {e.Message}", e);
            }
        }

        public VirtualBridge CreateVirtualBridge(string bridgeName)
        {
            var (comA, comB) = ParseVirtualBridge(bridgeName);

            Console.WriteLine($"Creating Virtual Bridge: {comA}<->{comB} ...");

            return CreateVirtualBridge(comA, comB);
        }

        public void DeleteVirtualBridge(VirtualBridge bridge)
        {
            //Not working due to interop issues between 0x86/0x64 code. Error: 0xE0000235. Vendor issue?
            try
            {
                bridge.PortA.deleteDevice();
                bridge.PortB.deleteDevice();
            }
            catch (Exception e)
            {
                throw new Exception($@"Cannot Delete Port Pair. {e.Message}", e);
            }
        }

        public void DeleteVirtualBridge(string bridgeName)
        {
            VirtualBridge pair = GetVirtualBridgeByName(bridgeName);

            if (pair == null)
            {
                throw new ArgumentException($"Virtual COM Pair {bridgeName.ToUpper()} not found.");
            }

            DeleteVirtualBridge(pair);
        }

        public (int comA, int comB) ParseVirtualBridge(string bridgeName)
        {
            Match match = s_PairRegex.Match(bridgeName);

            if (!match.Success)
            {
                throw new ArgumentException(
                    $"Incorrect Virtual COM Pair name {bridgeName.ToUpper()}. Correct example COM5:COM6");
            }

            var comA = int.Parse(match.Groups[1].Value);
            var comB = int.Parse(match.Groups[2].Value);

            return (comA, comB);
        }

        public VirtualBridge GetVirtualBridgeByName(string bridgeName)
        {
            var (comA, comB) = ParseVirtualBridge(bridgeName);

            IBridgePortDevice portA = GetBridgeContainingPort(comA);
            IBridgePortDevice portB = GetBridgeContainingPort(comB);

            if (portA == null || portB == null)
            {
                return null;
            }

            return new VirtualBridge {PortA = portA, PortB = portB};
        }

        private void InstallLicense()
        {
            var assembly = typeof(VirtualBridgeManager).GetTypeInfo().Assembly;

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
