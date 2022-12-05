//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.Runtime.Serialization;

namespace nanoFramework.nanoCLR.CLI
{
    /// <summary>
    /// Generic exception from CLI execution.
    /// </summary>
    [Serializable]
    public class CLIException : Exception
    {
        /// <summary>
        /// Exit code for the exception.
        /// </summary>
        public ExitCode ExecutionError { get; private set; }

        /// <summary>
        /// Error message from esptool.
        /// </summary>
        public string ExecutionErrorMessage { get; private set; }

        public CLIException(ExitCode error)
        {
            ExecutionError = error;
        }

        public CLIException(ExitCode error, string message) 
        {
            ExecutionError = error;
            ExecutionErrorMessage = message;
        }

        public CLIException(ExitCode error, string message, Exception innerException) : base(message, innerException)
        {
        }

        protected CLIException(SerializationInfo info, StreamingContext context) : base(info, context)
        {
        }
    }
}
