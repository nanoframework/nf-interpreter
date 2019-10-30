echo off

SET idfVer=3.3
SET targetDir=c:\Esp32_tools\libs-v%idfVer%

echo Copy IDF libraries to %targetDir%

rmdir /S /Q %targetDir%
mkdir %targetDir%


copy /Y build\bootloader\bootloader.bin %targetDir%

copy /Y build\app_trace\libapp_trace.a %targetDir%
copy /Y build\app_update\libapp_update.a %targetDir%
copy /Y build\asio\libasio.a %targetDir%
copy /Y build\aws_iot\libaws_iot.a %targetDir%
copy /Y build\bootloader\bootloader_support\libbootloader_support.a %targetDir%

copy /Y build\bt\libbt.a %targetDir%

copy /Y build\coap\libcoap.a %targetDir%
copy /Y build\console\libconsole.a %targetDir%

copy /Y build\cxx\libcxx.a %targetDir%
copy /Y build\driver\libdriver.a %targetDir%
copy /Y build\efuse\libefuse.a %targetDir%
copy /Y build\esp-tls\libesp-tls.a %targetDir%
copy /Y build\esp32\libesp32.a %targetDir%

copy /Y build\esp_adc_cal\libesp_adc_cal.a %targetDir%
copy /Y build\esp_event\libesp_event.a %targetDir%

copy /Y build\esp_http_client\libesp_http_client.a %targetDir%
copy /Y build\esp_http_server\libesp_http_server.a %targetDir%

copy /Y build\esp_https_ota\libesp_https_ota.a %targetDir%
copy /Y build\esp_https_server\libesp_https_server.a %targetDir%
copy /Y build\esp_ringbuf\libesp_ringbuf.a %targetDir%
copy /Y build\espcoredump\libespcoredump.a %targetDir%
copy /Y build\ethernet\libethernet.a %targetDir%
copy /Y build\expat\libexpat.a %targetDir%
copy /Y build\fatfs\libfatfs.a %targetDir%
copy /Y build\freemodbus\libfreemodbus.a %targetDir%
copy /Y build\freertos\libfreertos.a %targetDir%
copy /Y build\heap\libheap.a %targetDir%
copy /Y build\idf_test\libidf_test.a %targetDir%
copy /Y build\jsmn\libjsmn.a %targetDir%
copy /Y build\json\libjson.a %targetDir%
copy /Y build\libsodium\liblibsodium.a %targetDir%
copy /Y build\log\liblog.a %targetDir%

copy /Y build\main\libmain.a %targetDir%
copy /Y build\lwip\liblwip.a %targetDir%
copy /Y build\mbedtls\libmbedtls.a %targetDir%
copy /Y build\mdns\libmdns.a %targetDir%
copy /Y build\micro-ecc\libmicro-ecc.a %targetDir%
copy /Y build\mqtt\libmqtt.a %targetDir%

copy /Y build\newlib\libnewlib.a %targetDir%

copy /Y build\nghttp\libnghttp.a %targetDir%
copy /Y build\nvs_flash\libnvs_flash.a %targetDir%
copy /Y build\openssl\libopenssl.a %targetDir%

copy /Y build\protobuf-c\libprotobuf-c.a %targetDir%
copy /Y build\protocomm\libprotocomm.a %targetDir%
copy /Y build\pthread\libpthread.a %targetDir%
copy /Y build\sdmmc\libsdmmc.a %targetDir%
copy /Y build\smartconfig_ack\libsmartconfig_ack.a %targetDir%
copy /Y build\soc\libsoc.a %targetDir%
copy /Y build\spi_flash\libspi_flash.a %targetDir%
copy /Y build\spiffs\libspiffs.a %targetDir%

copy /Y build\tcp_transport\libtcp_transport.a %targetDir%
copy /Y build\tcpip_adapter\libtcpip_adapter.a %targetDir%
copy /Y build\ulp\libulp.a %targetDir%
copy /Y build\unity\libunity.a %targetDir%
copy /Y build\vfs\libvfs.a %targetDir%
copy /Y build\wear_levelling\libwear_levelling.a %targetDir%
copy /Y build\wifi_provisioning\libwifi_provisioning.a %targetDir%
copy /Y build\wpa_supplicant\libwpa_supplicant.a %targetDir%
copy /Y build\xtensa-debug-module\libxtensa-debug-module.a %targetDir%






