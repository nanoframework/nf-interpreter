echo "Pulling all the repos"
cd /sources/stm32f0xx_hal_driver
git fetch --depth=1 origin tag v1.7.8
git checkout tags/v1.7.8
cd /sources/stm32f4xx_hal_driver
git fetch --depth=1 origin tag v1.8.2
git checkout tags/v1.8.2
cd /sources/stm32f7xx_hal_driver
git fetch --depth=1 origin tag v1.3.1
git checkout tags/v1.3.1
cd /sources/stm32h7xx_hal_driver
git fetch --depth=1 origin tag v1.11.3
git checkout tags/v1.11.3
cd /sources/stm32l4xx_hal_driver
git fetch --depth=1 origin tag v1.13.4
git checkout tags/v1.13.4
cd /sources/cmsis_device_f0
git fetch --depth=1 origin tag v2.3.7
git checkout tags/v2.3.7
cd /sources/cmsis_device_f4
git fetch --depth=1 origin tag v2.6.9
git checkout tags/v2.6.9
cd /sources/cmsis_device_f7
git fetch --depth=1 origin tag v1.2.9
git checkout tags/v1.2.9
cd /sources/cmsis_device_h7
git fetch --depth=1 origin tag v1.10.4
git checkout tags/v1.10.4
cd /sources/cmsis_device_l4
git fetch --depth=1 origin tag v1.7.3
git checkout tags/v1.7.3
cd /sources/cmsis_core
git fetch --depth=1 origin tag v5.6.0
git checkout tags/v5.6.0
cd /sources/ThreadX
git pull
cd /
rm -rf /sources/ChibiOs
git svn clone http://svn.code.sf.net/p/chibios/code/branches/stable_21.11.x -rHEAD /sources/ChibiOs
cd /sources/ChibiOs-Contrib
git pull origin chibios-21.11.x
cd /sources/mbedtls
git fetch --depth=1 origin tag mbedtls-3.6.5
git checkout tags/mbedtls-3.6.5
git submodule update --init
cd /sources/fatfs
git fetch --depth=1 origin tag R0.16
git checkout tags/R0.16
cd /sources/FreeRTOS
git pull origin V10.4.1-kernel-only
cd /sources/CMSIS_5
git pull origin 5.5.1
cd /sources/lwip
git pull origin STABLE-2_1_3_RELEASE
cd /sources/littlefs
git pull origin v2.11.2
cd /sources/SimpleLinkCC32
git pull origin 4.10.00.07
cd /sources/SimpleLinkCC13
git pull origin 4.20.01.04
cd /sources/TI_SysConfig
git pull origin 1.5.0
echo "All repos pulled and up to date"
