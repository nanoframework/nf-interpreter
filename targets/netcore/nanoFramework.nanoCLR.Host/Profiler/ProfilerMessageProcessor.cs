//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.IO;

namespace nanoFramework.nanoCLR.Host.Profiler
{
    /// <summary>
    /// Class to process profiler messages.
    /// </summary>
    internal class ProfilerMessageProcessor : IDisposable
    {
        private readonly StreamWriter _profileLogWriter;

        public ProfilerMessageProcessor()
        {
            // create output file for profiler messages
            string _profileLogFileName = $"{Guid.NewGuid()}_{DateTime.Now.ToString("s").Replace(":", "_")}.log";

            string logFilePath = Path.Combine(Path.GetTempPath(), _profileLogFileName);

            // inform user about the log file
            Console.WriteLine($"Profiler output file: {logFilePath}");

            try
            {
                _profileLogWriter = new StreamWriter(
                    logFilePath,
                    new FileStreamOptions() { Mode = FileMode.Create, Access = FileAccess.ReadWrite, Options = FileOptions.WriteThrough, Share = FileShare.ReadWrite });
            }
            catch (IOException ex)
            {
                throw new IOException($"Failed to create profiler log file at {logFilePath}", ex);
            }
        }
        internal void MessageHandler(string message)
        {
            _profileLogWriter.WriteLine(message);
        }

        public void Dispose() => ((IDisposable)_profileLogWriter).Dispose();
    }
}
