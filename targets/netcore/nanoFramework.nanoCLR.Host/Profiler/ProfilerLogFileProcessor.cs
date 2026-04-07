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
        private readonly string _logFilePath;
        private bool _disposedValue;

        public ProfilerLogFileProcessor()
        {
            // create output file for profiler binary dump
            string _profileBinaryFileName = $"{Guid.NewGuid()}_{DateTime.Now.ToString("s").Replace(":", "_")}.log";

            _logFilePath = Path.Combine(Path.GetTempPath(), _profileBinaryFileName);

            // inform user about the binary file
            Console.WriteLine($"{Environment.NewLine}Profiler log file: {_logFilePath}{Environment.NewLine}");

            try
            {
                _profilerDumpWriter = new StreamWriter(
                    _logFilePath,
                    new FileStreamOptions() { Mode = FileMode.Create, Access = FileAccess.ReadWrite, Options = FileOptions.WriteThrough, Share = FileShare.ReadWrite });
            }
            catch (IOException ex)
            {
                throw new IOException($"Failed to create profiler message log file at {_logFilePath}", ex);
            }
        }

        internal void MessageHandler(byte[] data, int length)
        {
            BitStream stream = new BitStream(data, 0, length);

            Tools.NanoProfiler.Packets.ProfilerPacket pp = ProfilerPacketFactory.Decode(stream, null);

        }

        #region Dispose implementation

        protected virtual void Dispose(bool disposing)
        {
            if (!_disposedValue)
            {
                if (disposing)
                {
                    // output the file path so user can find it (usefull in case of long running)
                    Console.WriteLine($"{Environment.NewLine}Profiler log file: {_logFilePath}{Environment.NewLine}");

                    _profilerDumpWriter.Dispose();
                }

                _disposedValue = true;
            }
        }

        public void Dispose()
        {
            Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }

        #endregion
    }
}
