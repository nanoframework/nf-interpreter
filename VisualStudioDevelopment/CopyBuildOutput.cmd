cd Build\STM769IDiscovery
copy nanobooter.elf ..\nanobooter.elf 
"C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/Linux/gcc_arm/bin/arm-none-eabi-objcopy.exe"  -Oihex nanobooter.elf ..\nanobooter.hex 
"C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/Linux/gcc_arm/bin/arm-none-eabi-objcopy.exe"  -Osrec nanobooter.elf ..\nanobooter.s19
"C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/Linux/gcc_arm/bin/arm-none-eabi-objcopy.exe"  -Obinary nanobooter.elf ..\nanobooter.bin
"C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/Linux/gcc_arm/bin/arm-none-eabi-objdump.exe"  -d -EL -S nanobooter.elf > ..\nanobooter.lst
"C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/Linux/gcc_arm/bin/arm-none-eabi-size.exe"  nanobooter.elf > ..\nanobooter.size.txt

copy nanoCLR.elf ..\nanoCLR.elf 
"C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/Linux/gcc_arm/bin/arm-none-eabi-objcopy.exe"  -Oihex nanoCLR.elf ..\nanoCLR.hex 
"C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/Linux/gcc_arm/bin/arm-none-eabi-objcopy.exe"  -Osrec nanoCLR.elf ..\nanoCLR.s19
"C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/Linux/gcc_arm/bin/arm-none-eabi-objcopy.exe"  -Obinary nanoCLR.elf ..\nanoCLR.bin
"C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/Linux/gcc_arm/bin/arm-none-eabi-objdump.exe"  -d -EL -S nanoCLR.elf > ..\nanoCLR.lst
"C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/Linux/gcc_arm/bin/arm-none-eabi-size.exe"  nanoCLR.elf > ..\nanoCLR.size.txt

