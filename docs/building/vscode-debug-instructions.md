# Instructions for debuging **nano Framework** native code in VS Code

## Table of contents ##

- [Prerequisites](#Prerequisites)
- [Preparation](#Preparation)
- [Launch the debug session](#LaunchTheDebugSession)

**About this document**

This document describes how to debug **nano Framework** native code using VS Code.

<a name="Prerequisites"></a>
# Prerequisites

You'll need:
- [GNU ARM Embedded Toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads)
- [Visual Studio Code](http://code.visualstudio.com/)
- [C/C++ extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
- OpenOCD (any working distribution will work, follow some suggestions)
    - [Freddie Chopin OpenOCD (development)](http://www.freddiechopin.info/en/download/category/10-openocd-dev)
    - [OpenOCD – Open On-Chip Debugger](https://sourceforge.net/projects/openocd/)
    - [GNU ARM Eclipse OpenOCD](https://github.com/gnuarmeclipse/openocds)


<a name="Preparation"></a>
# Preparation

You'll need a binary image with debug information to be loaded in the SoC or MCU.
Assuming you are using VS Code to launch your builds, you'll have this image ready to be loaded in the MCU.
(see [Build instructions documentation](build-instructions.md))

In order to launch the debug session you'll need to tweak the *launch.json* file, located in the .vscode folder.
Here's what you probably need to change in order to adapt this file to your setup.
- miDebuggerPath: full path to the gdb executable (this one is inside the GCC toolchain folder)
- program: full path to the .hex or .bin output file that results from a successful build
- setupCommands (third ‘text’ entry): full path to the final image (same as above)
- setupCommands (fourth ‘text’ entry): the same as the program entry above
- debugServerPath: full path to the OpenOCD executable
- debugServerArgs: full path to the scripts directory on the OpenOCD installation AND the appropriate .cfg files for the interface and the board.


<a name="LaunchTheDebugSession"></a>
# Launch the debug session

Using VS Code menu View > Debug, clicking on the debug icon on the left hand toolbar or hitting the CTRL+SHIT+D shortcut you’ll reach the debug view. There you’ll find the launch configurations for debug that we’ve setup above (see the drop down at the top) and the familiar green play button (or F5 if you prefer).

When a debug session is active you can find a lot of familiar stuff:
- debug toolbar with the usual operations (pause, step over, into, out, restart and stop)
- variables list
- call stack that you can use to navigate up and down
- breakpoint list to manage those
- watch expressions
- support for ‘mouse over’ a variable which will display a context with the variable content
- ability to set/remove breakpoints by clicking near the line number
- other handy tools and options using the right click on the various objects
