//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System.ComponentModel.DataAnnotations;

namespace nanoFramework.nanoCLR.CLI
{
    public enum ExitCode
    {
        /// <summary>
        /// Execution terminated without any error
        /// </summary>
        [Display(Name = "")]
        OK = 0,

        //////////////////////////////////////
        // Virtual Serial Port Tools errors //
        //////////////////////////////////////

        /// <summary>
        /// Virtual Serial Port Tools software not installed.
        /// </summary>
        [Display(Name = "Virtual Serial Port Tools software not installed.")]
        E1000 = 1000,

        /// <summary>
        /// Failed to create Virtual Bridge.
        /// </summary>
        [Display(Name = "Failed to create Virtual Bridge")]
        E1001 = 1001,

        /// <summary>
        /// Failed to delete Virtual Bridge.
        /// </summary>
        [Display(Name = "Failed to delete Virtual Bridge")]
        E1002 = 1002,

        ////////////////////////////////
        // Application general Errors //
        ////////////////////////////////

        /// <summary>
        /// Error parsing arguments.
        /// </summary>
        [Display(Name = "Invalid or missing arguments.")]
        E9000 = 9000,
    }
}
