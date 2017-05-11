# Class Libraries 

**About this document**

This document describes the design and organization of **nanoFramework** Class Libraries, offers some explanation on the choices that were made and how to add a new Class Library. The examples bellow are related with ChibiOS (which is the currently reference implementation for **nanoFramework**).


## Libraries

- Base Class Library (also know as mscorlib).
- Windows.Device.Gpio


## Distribution strategy

To ease the burden of distributing and updating the class libraries we've choose to use Nuget to handle all this. It has the added benefit of dealing with the dependency management, version and such.

So, for each class library, there is a Nuget package that includes the assembly and documentation files. The Nuget package takes care of making sure that the required dependency(ies) and correct version(s) are added to a managed (C#) project, making a developer's life much easier.


## How to add a new class library

Follows the procedure to add a new class library to a **nanoFramework** target image. 

The example is for adding Windows.Devices.Gpio library.


1. In VS2017 start a new project for a **nanoFramework** C# Class library. Source code [here](https://github.com/nanoframework/nf-class-libraries/tree/master/Windows.Devices.Gpio)

2. Implement all the required methods, enums, properties in that project. It's recommended that you add XML comments there (and enable the automated documentation generation in the project properties).

3. Add the Nuget packaging project to distribute the managed assembly and documentation. We have a second Nuget package that includes all the build artifacts, generated stubs, dump files and such. This is to be used in automated testing and distribution of followup projects or build steps. 

4. Upon a successfully build of the managed project the skeleton with the stubs should be available in the respective folder. Because **nanoFramework** aims to be target independent the native implementation of a class library will live 'inside' each target RTOS folder.
The next step would be to copy the stubs to a folder with the assembly name inside the nanoCLR folder. For our example: [Windows.Devices.Gpio](../../targets/CMSIS-OS/ChibiOS/nanoCLR/Windows.Devices.Gpio).

5. Add the CMake as a module to the modules folder [here](../../CMake/Modules). The name of the module should follow the assembly name (Find**Windows.Devices.Gpio**.cmake). Mind the CMake rules for the naming: start with _Find_ followed by the module name and _cmake_ extension. The CMake for the Windows.Devices.Gpio module is [here](../../CMake/Modules/FindWindows.Devices.Gpio.cmake).

6. In the CMake [NF_API_Namespaces.cmake](../../CMake/Modules/NF_API_Namespaces.cmake) add an option for the API. The option name must follow the pattern API_**namespace**. The option for Windows.Devices.Gpio is API_Windows.Devices.Gpio.

7. In the CMake [NF_API_Namespaces.cmake](../../CMake/Modules/NF_API_Namespaces.cmake) find the macro `ParseApiOptions` and add a block for the API. Just copy/paste an existing one and replace the namespace with the one that you are adding.

8. Update the template file for the CMake variants [here](../../cmake-variants.TEMPLATE.json) to include the respective option. For the Windows.Devices.Gpio example you would add to the _OPTION1..._ and _OPTION2..._ (under _linkage_) the following line: "API_Windows.Devices.Gpio" : "OFF"


## How to include a class library in the build

To include a class library in the build for a target image you have to add to the CMake an option for the API. For the Windows.Devices.Gpio example the option would be `-DAPI_Windows.Devices.Gpio=ON`.
You can also add this to your own cmake-variants.json file.
To exclude a class library just set the option to OFF or simply don't include it in the command.
