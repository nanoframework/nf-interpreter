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

        /// <summary>
        /// Failed to fidn a Virtual Bridge containing the requested port.
        /// </summary>
        [Display(Name = "Failed to find a Virtual Bridge containing the requested port")]
        E1003 = 1003,

        ////////////////////////////////
        // Application general Errors //
        ////////////////////////////////

        /// <summary>
        /// Error parsing arguments.
        /// </summary>
        [Display(Name = "Invalid or missing arguments.")]
        E9000 = 9000,

        /// <summary>
        /// Invalid COM port.
        /// </summary>
        [Display(Name = "Invalid COM port. Check for valid port name. Valid COM ports: COM1, COM20.")]
        E9001 = 9001,

        /// <summary>
        /// Error calling elevated execution of process.
        /// </summary>
        [Display(Name = "Error calling elevated execution of process. Check for UAC prompt or other security issue blocking it's exection.")]
        E9002 = 9002,

        /// <summary>
        /// Error calling elevated execution of process.
        /// </summary>
        [Display(Name = "Timeout calling elevated execution of process. Check for UAC prompt or other security issue blocking it's exection.")]
        E9003 = 9003,

        /// <summary>
        /// COM port already in use.
        /// </summary>
        [Display(Name = "The specified COM port is already in use.")]
        E9004 = 9004,

        /// <summary>
        /// Error updating nanoCLR library.
        /// </summary>
        [Display(Name = "Error updating nanoCLR library.")]
        E9005 = 9005,

        /// <summary>
        /// Error updating nanoCLR library.
        /// </summary>
        [Display(Name = "Error installing Virtual Serial Port Tools.")]
        E9006 = 9006,

        /// <summary>
        /// Can't start a new instance.
        /// </summary>
        [Display(Name = "Can't start a new virtual device with these settings. Most likely there is already another instance running.")]
        E9007 = 9007,

        /// <summary>
        /// Unknown error when starting the virtual device instance.
        /// </summary>
        [Display(Name = "Unknown error when starting the virtual device instance.")]
        E9008 = 9008,

        /// <summary>
        /// Specified nanoCLR DLL file does not exist.
        /// </summary>
        [Display(Name = " Specified nanoCLR DLL file does not exist.")]
        E9009 = 9009,

        /// <summary>
        /// Specified nanoCLR DLL file does not exist.
        /// </summary>
        [Display(Name = "Cannot update the CLR instance when requesting to load a specific version of it.")]
        E9010 = 9010,

        /// <summary>
        /// Cannot retrieve list of native assemblies.
        /// </summary>
        [Display(Name = "Cannot retrieve list of native assemblies.")]
        E9011 = 9011,
    }
}
