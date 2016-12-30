## nanoBooter mbed-os Example

This is the nanoBooter example implementation for mbed-os.

### Prerequisites

1. ARM mbed Command Line Interface [mbed-cli](https://github.com/ARMmbed/mbed-cli)
    * Requires Python 2.7.x and git
2. Command-line compiler or IDE toolchain
    * GCC ARM, IAR, Keil uVision

### Building nanoBooter

1. Switch to `nanoBooter` directory and open command line window,
2. Make sure Python scripts and git executable are in the PATH,
   otherwise execute
    ```
    PATH=C:\Apps\Python27\Scripts;C:\Program Files\Git\bin;%PATH%
    ```
3. Create a new mbed-os progam in the current directory
    ```
    mbed new .
    ```
    It will take a moment for mbed-cli to download mbed-os git repository.

4. Configure toolchain path
    ```
    mbed config --global GCC_ARM_PATH C:\Apps\GCC\5.4-2016q3\bin
    ```

5. Optional: Configure default toolchain and target
    ```
    mbed toolchain GCC_ARM
    mbed target NUCLEO_F072RB
    ```
    To list supported targets launch
    ```
    mbed target -S
    ```

6. Compile nanoBooter
    ```
    mbed compile
    ```
    If toolchain and target were not set in step 5, use command line parameters
    ```
    mbed compile -t <toolchain> -m <target>
    ```

7. Deploy binary image `BUILD\<target>\<toolchain>\nanoBooter.bin`

### Important notes and limitations

* nanoBooter recognizes and responds only to Ping request,
* The reply is not correctly received in MFDeploy due to invalid CRC,
* WireProtocol implementation is duplicated at the moment (the source code will be added as proper mbed-os library),
* Used serial (USART) transport,
* nanoBooter code is in the main.cpp (will be moved to completely separated library/component).