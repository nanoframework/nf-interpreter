## .NET nanoFramework nanoCLR WIN32 version

This is the nanoCLR in WIN32 version.

It's a VC++ Solution, meant to be used for high level debugging, feature testing and Unit Testing for other projects.

### Usage

1. Build the Solution.
1. Make sure `nanoCLR` is the start project.
1. Use either directly from the command line or starting a debug session.
1. To load assemblies use the following syntax:
    `-load <path-to-the-PE-file-1> -load <path-to-the-PE-file-2> ...`
