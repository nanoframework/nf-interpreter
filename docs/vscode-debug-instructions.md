# Instructions for debugging **nanoFramework** native code in VS Code

## Table of contents ##

- [Prerequisites](#prerequisites)
- [Preparation](#preparation)
- [Launch the debug session](#launch-the-debug-session)

**About this document**

This document describes how to debug **nanoFramework** native code using VS Code.

# Prerequisites

You'll need:
- [GNU ARM Embedded Toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads)
- [Visual Studio Code](http://code.visualstudio.com/)
- [C/C++ extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
- OpenOCD (any working distribution will work, follow some suggestions)
    - [Freddie Chopin OpenOCD (development)](http://www.freddiechopin.info/en/download/category/10-openocd-dev)
    - [OpenOCD – Open On-Chip Debugger](https://sourceforge.net/projects/openocd/)
    - [GNU ARM Eclipse OpenOCD](https://github.com/gnuarmeclipse/openocd)


# Preparation

You'll need a binary image with debug information to be loaded in the SoC or MCU.
Assuming you are using VS Code to launch your builds, you'll have this image ready to be loaded in the MCU.
(see [Build instructions documentation](build-instructions.md))

In order to launch the debug session you'll need to create and setup a *launch.json* file, located in the .vscode folder.
We recommend that you use the provided script [debug-setup.ps1](..\debug-setup.ps1) to help get you up and running quickly. 

A template file [launch.TEMPLATE.json](..\.vscode\launch.TEMPLATE.json) also exists for those that prefer. Just copy and rename it to *launch.json* and then adapt its content to your requirement (see below).

Here's what you need to change in order to adapt the template file to your setup and make it more suitable to your working style and preferences.
- name: here you can name each of the launch configurations to help choosing the appropriate one when launching the debug session. These could be for example: "nanoBooter for Discovery 4", "nanoCLR for Nucleo F091RC", "test featureXYZ for Discovery 4".
- miDebuggerPath: full path to the gdb executable (this one is inside the GCC tool-chain folder)
- program: full path to the .hex output file that results from a successful build
- setupCommands (third 'text' entry): full path to the final image (same as above)
- setupCommands (fourth 'text' entry): the same as the program entry above
- debugServerPath: full path to the OpenOCD executable
- debugServerArgs: full path to the scripts directory on the OpenOCD installation AND the appropriate .cfg files for the interface and the board.

_Note 1: The VS Code parser currently has trouble parsing and replacing the environment variable ${workspaceRoot} for certain OpenOCD commands. For this reason we currently use full (absolute) paths to keep OpenOCD happy._

_Note 2: Always use forward slashes in the paths above (backslashes are reserved for escaping characters), otherwise you'll receive strange and unclear errors from OpenOCD._


# Launch the debug session

Using VS Code menu View > Debug, clicking on the debug icon on the left hand toolbar or hitting the CTRL+SHIT+D shortcut you'll reach the debug view. There you'll find the launch configurations for debug that we've setup above (see the drop down at the top) and the familiar green play button (or F5 if you prefer).

When a debug session is active you will find a lot of familiar features:
- debug toolbar with the usual operations (pause, step over, into, out, restart and stop)
- variables list
- call stack that you can use to navigate up and down
- breakpoint list to manage those
- watch expressions
- support for 'mouse over' a variable which will display a context with the variable content
- ability to set/remove breakpoints by clicking near the line number
- other handy tools and options using the right click on the various objects
