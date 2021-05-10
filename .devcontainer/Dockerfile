FROM debian:10.4-slim AS downloader
RUN apt-get update \
    && apt-get -y install --no-install-recommends apt-utils \
    && apt-get install -y \
    curl \
    bzip2 \
    unzip

ARG GCC_URI=https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/10-2020q4/gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2
RUN mkdir -p /tmp/dc-downloads /tmp/dc-extracted/gcc /tmp/dc-extracted/cmake \
    && curl -o /tmp/dc-downloads/gcc-arm.tar.bz2  $GCC_URI \
    && bunzip2 -d /tmp/dc-downloads/gcc-arm.tar.bz2 \
    && tar -xvf /tmp/dc-downloads/gcc-arm.tar -C /tmp/dc-extracted/gcc --strip-components 1

ARG CMAKE_SCRIPT=https://cmake.org/files/v3.19/cmake-3.19.0-Linux-x86_64.sh
RUN curl -o /tmp/dc-downloads/cmake.sh $CMAKE_SCRIPT \
    && chmod +x /tmp/dc-downloads/cmake.sh \
    && bash /tmp/dc-downloads/cmake.sh --skip-license --prefix=/tmp/dc-extracted/cmake

ARG IDF_URI=https://dl.espressif.com/dl/esp-idf/releases/esp-idf-v3.3.1.zip
RUN curl -o /tmp/dc-downloads/esp-idf.zip $IDF_URI \
    && unzip -d /tmp/dc-extracted/esp-idf /tmp/dc-downloads/esp-idf.zip

ARG IDF_LIBS_URI=https://dl.cloudsmith.io/public/net-nanoframework/internal-build-tools/raw/names/IDF_libs/versions/v3.3.1/IDF_libs-v3.3.1.zip
RUN curl -o /tmp/dc-downloads/esp-idf-libs.zip $IDF_LIBS_URI -L \
    && unzip -d /tmp/dc-extracted/esp-idf-libs /tmp/dc-downloads/esp-idf-libs.zip

ARG XTENSA_URI=https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
RUN  mkdir -p /tmp/dc-extracted/xtensa \
    && curl -o /tmp/dc-downloads/xtensa.tar.gz $XTENSA_URI \
    && tar -xzf /tmp/dc-downloads/xtensa.tar.gz -C /tmp/dc-extracted/xtensa --strip-components 1

ARG TI_TOOL_URL=http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/rtsc/3_61_00_16/exports/xdccore/xdctools_3_61_00_16_core_linux.zip
RUN  mkdir -p /tmp/dc-extracted/titools \
    && curl -o /tmp/dc-downloads/titools.zip $TI_TOOL_URL -L \
    && unzip -d /tmp/dc-extracted/titools /tmp/dc-downloads/titools.zip

FROM debian:10.4-slim AS devcontainer

# Avoid warnings by switching to noninteractive
ENV DEBIAN_FRONTEND=noninteractive

# You can set up non-root user
# ARG USERNAME=vscode
# ARG USER_UID=1000
# ARG USER_GID=$USER_UID

# Configure apt and install packages
RUN apt-get update \
    && apt-get -y install --no-install-recommends apt-utils dialog icu-devtools 2>&1 \
    && apt-get install -y \
    git \
    curl \
    ninja-build \
    srecord \
    python3 \
    python3-pip \
    nodejs

# Create needed directories
RUN mkdir -p /usr/local/bin/gcc \
    && mkdir -p /usr/local/bin/xtensa \
    && mkdir -p /usr/local/bin/titools

# Clone repos for STM32 including AzureRTOS
RUN git clone --branch nf-build https://github.com/nanoframework/STM32CubeL4.git ./sources/STM32CubeL4 \
    && git clone --branch nf-build https://github.com/nanoframework/STM32CubeF7.git ./sources/STM32CubeF7 \
    && git clone --branch nf-build https://github.com/nanoframework/STM32CubeF4.git ./sources/STM32CubeF4 \
    && git clone --branch nf-build https://github.com/nanoframework/STM32CubeH7.git ./sources/STM32CubeH7 \
    && git clone --recursive https://github.com/azure-rtos/threadx.git ./sources/AzureRTOS \
    && git clone --branch stable_20.3.x https://github.com/nanoframework/chibios.git ./sources/ChibiOs \
    && git clone --branch nanoframework https://github.com/nanoframework/ChibiOS-Contrib.git ./sources/ChibiOs-Contrib
# Clone mbedtls and fatfs
RUN git clone --branch mbedtls-2.26.0 https://github.com/ARMmbed/mbedtls.git ./sources/mbedtls \
    && git clone --branch R0.14 https://github.com/abbrev/fatfs.git ./sources/fatfs
# Clone FreeRTOS and what is needed for ESP32
RUN git clone --branch V10.4.1-kernel-only https://github.com/FreeRTOS/FreeRTOS-Kernel.git ./sources/FreeRTOS \
    && git clone --branch 5.5.1 https://github.com/ARM-software/CMSIS_5.git ./sources/CMSIS_5 \
    && git clone --branch STABLE-2_0_3_RELEASE https://git.savannah.nongnu.org/git/lwip.git ./sources/lwip \
    && git clone --branch nf-build https://github.com/nanoframework/spiffs.git ./sources/spiffs
# Clone what is needed for TI
RUN git clone --branch 4.10.00.07 https://github.com/nanoframework/SimpleLink_CC32xx_SDK.git ./sources/SimpleLinkCC32 \
    # && git clone --branch 3.61.00.16 https://github.com/nanoframework/TI_XDCTools.git ./sources/TI_XDCTools \
    && git clone --branch 4.20.01.04 https://github.com/nanoframework/SimpleLink_CC13x2_26x2_SDK.git ./sources/SimpleLinkCC13 \
    && git clone --branch 1.5.0 https://github.com/nanoframework/TI_SysConfig.git ./sources/TI_SysConfig \
    && chmod +x ./sources/TI_SysConfig/sysconfig_cli.sh

# Copy from our other container
COPY --from=downloader /tmp/dc-extracted/gcc /usr/local/bin/gcc 
COPY --from=downloader /tmp/dc-extracted/cmake /usr
COPY --from=downloader /tmp/dc-extracted/esp-idf /sources/esp-idf
COPY --from=downloader /tmp/dc-extracted/esp-idf-libs /sources/esp-idf-libs
COPY --from=downloader /tmp/dc-extracted/xtensa /usr/local/bin/xtensa
COPY --from=downloader /tmp/dc-extracted/titools/xdctools_3_61_00_16_core /usr/local/bin/titools
COPY ./scripts/git-pull-repos.sh /usr/local/git-pull-repos.sh

# Putting hex2dfu in the container
ARG HEX2DFU=https://github.com/nanoframework/hex2dfu/releases/download/v2.0.9/hex2dfu
RUN mkdir -p /usr/local/bin/hex2dfu \
    && curl -o /usr/local/bin/hex2dfu/hex2dfu $HEX2DFU -L \
    && chmod +x /usr/local/bin/hex2dfu/hex2dfu

# Creating static link python for pyhton3
RUN ln -fs /usr/bin/python3 /usr/bin/python \
    && pip3 install pyserial

# Clean up downloaded files
RUN apt-get autoremove -y \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

# Switch back to dialog for any ad-hoc use of apt-get
ENV DEBIAN_FRONTEND=dialog
