//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.IO;
using nanoFramework.Tools.NanoProfiler;
using nanoFramework.Tools.NanoProfiler.Packets;

namespace nanoFramework.nanoCLR.Host.Profiler
{
    internal class ProfilerLogFileProcessor : IDisposable
    {
        private readonly StreamWriter _profilerDumpWriter;

        public ProfilerLogFileProcessor()
        {
            // create output file for profiler binary dump
            string _profileBinaryFileName = $"{Guid.NewGuid()}_{DateTime.Now.ToString("s").Replace(":", "_")}.log";

            string logFilePath = Path.Combine(Path.GetTempPath(), _profileBinaryFileName);

            // inform user about the binary file
            Console.WriteLine($"Profiler log file: {logFilePath}");

            try
            {
                _profilerDumpWriter = new StreamWriter(
                    logFilePath,
                    new FileStreamOptions() { Mode = FileMode.Create, Access = FileAccess.ReadWrite, Options = FileOptions.WriteThrough, Share = FileShare.ReadWrite });
            }
            catch (IOException ex)
            {
                throw new IOException($"Failed to create profiler message log file at {logFilePath}", ex);
            }
        }

        internal void MessageHandler(byte[] data, int length)
        {
            BitStream stream = new BitStream(data, 0, length);

            Tools.NanoProfiler.Packets.ProfilerPacket pp = ProfilerPacketFactory.Decode(stream, null);

        }

        public void Dispose() => ((IDisposable)_profilerDumpWriter).Dispose();
    }
}
