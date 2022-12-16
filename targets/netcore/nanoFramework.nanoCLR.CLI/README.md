# .NET nanoFramework nanoCLR

This [.NET Core Tool](https://docs.microsoft.com/en-us/dotnet/core/tools/global-tools) allows running nanoCLR in Windows machines. This can be useful to have virtual devices for development purposes, performing automated tests on pipelines or other environments and other uses that benefit from having a virtual device opposed to real  physical hardware. The virtual device can be accessed just like a real one through a virtual serial port, a named pipe or a TCP/IP port.

Is part of .NET **nanoFramework** toolbox, along with other various tools that are required in .NET **nanoFramework** development, usage or repository management.

## Install .NET **nanoFramework** nanoCLR

Perform a one-time install of the .NET **nanoFramework** nanoCLR tool using the following .NET Core CLI command:

```console
dotnet tool install -g nanoclr
```

After a successful installation a message is displayed showing the command that's to be used to call the tool along with the version installed. Similar to the following example:

```console
You can invoke the tool using the following command: nanoff
Tool 'nanoclr' (version '9.9.9') was successfully installed.
```

## Update .NET **nanoFramework** nanoCLR

To update .NET **nanoFramework** nanoCLR tool use the following .NET Core CLI command:

```console
dotnet tool update -g nanoclr
```

## Usage

Once the tool is installed, you can call it by using its command `nanoclr`, which is a short version of the name to ease typing.

```console
nanoclr [command] [args]
```

The tool includes help for all available commands. You can see a list of all available ones by entering:

```console
nanoclr --help
```

## Virtual Serial Ports

This verb allows managing the virtual serial port exposing the virtual device.
The virtual serial port it's only available in Windows machines. It's made available with the sponsorship of [HHD Software](https://www.hhdsoftware.com/) which has kindly sponsored a free license for running their tool.

### List Virtual Serial ports

This command lists the installed virtual serial port bridges in the system.

```console
nanoclr virtualserial --list
```

### Create Virtual Serial port

This command creates a virtual serial port that will be used to expose the nanoCLR instance. A random COM port will be created.
A COM port can be optionally specified as an option. In this case, if that COM port it's not being used, it will be created.

```console
nanoclr virtualserial --create [COM99]
```

### Remove Virtual Serial port

This command removes an existing virtual serial port.

```console
nanoclr virtualserial --remove [COM99]
```

### Install Virtual Serial Port tools

This command installs [HHD Software](https://www.hhdsoftware.com/) Virtual Serial Port Tools software. The installer is downloaded from their website and executed. Executing the installer requires "Install Driver" privilege and must be elevated. If the process running it doesn't have the required permissions an UAC prompt will be show.

```console
nanoclr virtualserial --install
```

## Running the virtual nanoCLR

These options run the nanoCRL instance loading the specified assemblies and exposing the device on a specified interface. Please note that for successfully running a nanoCLR instance it's require to load a set of assemblies _and_ specify an interface to expose it, otherwise it has no use.

### Load a set of assemblies

This loads a set of .NET nanoFramework assemblies and runs the nanoCLR. These have to be in .NET nanoFramework PE (Portable Executable) format.
They are found in the output folder of a Visual Studio project (typically `bin\Debug` or `bin\Release`). Full path of the PE files is required.

```console
nanoclr run --assemblies "C:\nano\my_nice_project\bin\Debug\mscorlib.pe" "C:\nano\my_nice_project\bin\Debug\my_nice_project.pe"
```

### Specify Serial Port

This option specifies the COM port that will be used to expose the virtual nanoCLR that will be running. It requires a previous setup of a Virtual Serial Port (see [Create Virtual Serial port](#create-virtual-serial-port)).

```console
nanoclr run --serialport COM99  (--assemblies ...)
```

### Specify Named Pipe

This option specifies a [Named Pipe](https://learn.microsoft.com/en-us/windows/win32/ipc/named-pipes) that will be used to expose the virtual nanoCLR that will be running.

```console
nanoclr run --namedpipe MyNanoDevice  (--assemblies ...)
```

### Resolve references

This option tries to resolve cross-assembly references between the loaded assemblies.

```console
nanoclr run --resolve (--assemblies ...)
```

### Options to control debugger connection

There are two options that control how the nanoCLR execution interacts with a debugger. If it's intended that after a program terminates and exits the execution a debugger will be connecting, the option `--loopafterexit` should be included.
In case it's expected that imediatly after nanoCLR is started,a debugger is to connect to it, the option `--waitfordebugger` should be included.

## Maintenance operations with the nanoCLR

The nanoCLR it's, in fact, a wrapper to the nanoCLR instance that is distributed as DLL so it can be easily updated.
The following operations are available to manage this.

### Version of nanoCLR

Gets the version of the current nanoCLR instance.

```console
nanoclr instance --getversion
```

### Update nanoCLR instance

Checks for stable versions of nanoCLR and updates it, if there is one. A version can be specified. To check preview versions add the `--preview` option.

```console
nanoclr instance --update [--preview] [--clrversion 1.22.333.4444]
```

## Uninstall .NET **nanoFramework** nanoCLR

To uninstall the .NET **nanoFramework** nanoCLR tool use the following .NET Core CLI command:

```console
dotnet tool uninstall -g nanoclr
```
