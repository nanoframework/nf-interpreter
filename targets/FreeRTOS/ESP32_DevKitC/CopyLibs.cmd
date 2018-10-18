
echo "Copy IDF libraies to Esp32_tools\libs"

rmdir /S /Q c:\Esp32_tools\libs-v3.1
mkdir c:\Esp32_tools\libs-v3.1

copy /Y build\bootloader\bootloader.bin c:\Esp32_tools\libs-v3.1

copy /Y build\bootloader\bootloader_support\libbootloader_support.a c:\Esp32_tools\libs-v3.1
copy /Y build\bootloader\log\liblog.a c:\Esp32_tools\libs-v3.1
copy /Y build\bootloader\spi_flash\libspi_flash.a c:\Esp32_tools\libs-v3.1
copy /Y build\bootloader\micro-ecc\libmicro-ecc.a c:\Esp32_tools\libs-v3.1
copy /Y build\bootloader\soc\libsoc.a c:\Esp32_tools\libs-v3.1
copy /Y build\bootloader\main\libmain.a c:\Esp32_tools\libs-v3.1
copy /Y build\app_trace\libapp_trace.a c:\Esp32_tools\libs-v3.1
copy /Y build\app_update\libapp_update.a c:\Esp32_tools\libs-v3.1
copy /Y build\aws_iot\libaws_iot.a c:\Esp32_tools\libs-v3.1
copy /Y build\bootloader_support\libbootloader_support.a c:\Esp32_tools\libs-v3.1
copy /Y build\bt\libbt.a c:\Esp32_tools\libs-v3.1
copy /Y build\coap\libcoap.a c:\Esp32_tools\libs-v3.1
copy /Y build\console\libconsole.a c:\Esp32_tools\libs-v3.1
copy /Y build\cxx\libcxx.a c:\Esp32_tools\libs-v3.1
copy /Y build\driver\libdriver.a c:\Esp32_tools\libs-v3.1
copy /Y build\esp32\libesp32.a c:\Esp32_tools\libs-v3.1
copy /Y build\esp_adc_cal\libesp_adc_cal.a c:\Esp32_tools\libs-v3.1
copy /Y build\ethernet\libethernet.a c:\Esp32_tools\libs-v3.1
copy /Y build\expat\libexpat.a c:\Esp32_tools\libs-v3.1
copy /Y build\fatfs\libfatfs.a c:\Esp32_tools\libs-v3.1
copy /Y build\freertos\libfreertos.a c:\Esp32_tools\libs-v3.1
copy /Y build\heap\libheap.a c:\Esp32_tools\libs-v3.1
copy /Y build\idf_test\libidf_test.a c:\Esp32_tools\libs-v3.1
copy /Y build\jsmn\libjsmn.a c:\Esp32_tools\libs-v3.1
copy /Y build\json\libjson.a c:\Esp32_tools\libs-v3.1
copy /Y build\libsodium\liblibsodium.a c:\Esp32_tools\libs-v3.1
copy /Y build\log\liblog.a c:\Esp32_tools\libs-v3.1
copy /Y build\lwip\liblwip.a c:\Esp32_tools\libs-v3.1
copy /Y build\main\libmain.a c:\Esp32_tools\libs-v3.1
copy /Y build\mbedtls\libmbedtls.a c:\Esp32_tools\libs-v3.1
copy /Y build\mdns\libmdns.a c:\Esp32_tools\libs-v3.1
copy /Y build\micro-ecc\libmicro-ecc.a c:\Esp32_tools\libs-v3.1
copy /Y build\newlib\libnewlib.a c:\Esp32_tools\libs-v3.1
copy /Y build\nghttp\libnghttp.a c:\Esp32_tools\libs-v3.1
copy /Y build\nvs_flash\libnvs_flash.a c:\Esp32_tools\libs-v3.1
copy /Y build\openssl\libopenssl.a c:\Esp32_tools\libs-v3.1
copy /Y build\pthread\libpthread.a c:\Esp32_tools\libs-v3.1
copy /Y build\sdmmc\libsdmmc.a c:\Esp32_tools\libs-v3.1
copy /Y build\soc\libsoc.a c:\Esp32_tools\libs-v3.1
copy /Y build\spi_flash\libspi_flash.a c:\Esp32_tools\libs-v3.1
copy /Y build\spiffs\libspiffs.a c:\Esp32_tools\libs-v3.1
copy /Y build\tcpip_adapter\libtcpip_adapter.a c:\Esp32_tools\libs-v3.1
copy /Y build\ulp\libulp.a c:\Esp32_tools\libs-v3.1
copy /Y build\vfs\libvfs.a c:\Esp32_tools\libs-v3.1
copy /Y build\wear_levelling\libwear_levelling.a c:\Esp32_tools\libs-v3.1
copy /Y build\wpa_supplicant\libwpa_supplicant.a c:\Esp32_tools\libs-v3.1
copy /Y build\xtensa-debug-module\libxtensa-debug-module.a c:\Esp32_tools\libs-v3.1
copy /Y build\smartconfig_ack\libsmartconfig_ack.a c:\Esp32_tools\libs-v3.1


