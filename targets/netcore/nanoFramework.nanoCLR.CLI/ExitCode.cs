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
        [Display(Name = "Virtual Serial Port Tools software not installed. Make sure it's connected and has booted in DFU mode")]
        E1000 = 1000,

    }
}
