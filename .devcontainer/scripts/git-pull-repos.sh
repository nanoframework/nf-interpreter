echo "Pulling all the repos"
cd /sources/STM32CubeL4
git pull origin nf-build
cd /sources/STM32CubeF7
git pull origin nf-build
cd /sources/STM32CubeF4
git pull origin nf-build
cd /sources/STM32CubeH7
git pull origin nf-build
cd /sources/AzureRTOS
git pull
cd /
rm -rf /sources/ChibiOs
git svn clone https://svn.osdn.net/svnroot/chibios/branches/stable_21.11.x -rHEAD /sources/ChibiOs
cd /sources/ChibiOs-Contrib
git pull origin nanoframework
cd /sources/mbedtls
git pull origin mbedtls-2.28.0
cd /sources/fatfs
git pull origin R0.14
cd /sources/FreeRTOS
git pull origin V10.4.1-kernel-only
cd /sources/CMSIS_5
git pull origin 5.5.1
cd /sources/lwip
git pull origin STABLE-2_0_3_RELEASE
cd /sources/spiffs
git pull origin nf-build
cd /sources/SimpleLinkCC32
git pull origin 4.10.00.07
cd /sources/SimpleLinkCC13
git pull origin 4.20.01.04
cd /sources/TI_SysConfig
git pull origin 1.5.0
echo "All repos pulled and up to date"