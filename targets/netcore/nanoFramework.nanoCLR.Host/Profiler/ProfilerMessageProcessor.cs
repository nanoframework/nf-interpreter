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
        private readonly string _logFilePath;
        private bool _disposedValue;

        public ProfilerMessageProcessor()
        {
            // create output file for profiler messages
            string _profileLogFileName = $"{Guid.NewGuid()}_{DateTime.Now.ToString("s").Replace(":", "_")}.txt";

            _logFilePath = Path.Combine(Path.GetTempPath(), _profileLogFileName);

            // inform user about the log file
            Console.WriteLine($"{Environment.NewLine}Profiler output file: {_logFilePath}{Environment.NewLine}");

            try
            {
                _profileLogWriter = new StreamWriter(
                    _logFilePath,
                    new FileStreamOptions() { Mode = FileMode.Create, Access = FileAccess.ReadWrite, Options = FileOptions.WriteThrough, Share = FileShare.ReadWrite });
            }
            catch (IOException ex)
            {
                throw new IOException($"Failed to create profiler log file at {_logFilePath}", ex);
            }
        }
        internal void MessageHandler(string message)
        {
            _profileLogWriter.WriteLine(message);
            // Ensure message is written immediately
            _profileLogWriter.Flush();
        }

        #region Dispose implementation

        protected virtual void Dispose(bool disposing)
        {
            if (!_disposedValue)
            {
                if (disposing)
                {
                    // output the file path so user can find it (usefull in case of long running)
                    Console.WriteLine($"{Environment.NewLine}Profiler output file: {_logFilePath}{Environment.NewLine}");

                    _profileLogWriter.Dispose();
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
