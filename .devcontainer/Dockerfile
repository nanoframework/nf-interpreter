FROM debian:10.4-slim AS downloader
RUN apt-get update \
    && apt-get -y install --no-install-recommends apt-utils \
    && apt-get install -y \
    curl \
    bzip2 \
    unzip \
    srecord

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

ARG IDF_LIBS_URI=https://dl.bintray.com/nfbot/internal-build-tools/IDF_libs-v3.3.1.zip
RUN curl -o /tmp/dc-downloads/esp-idf-libs.zip $IDF_LIBS_URI -L \
    && unzip -d /tmp/dc-extracted/esp-idf-libs /tmp/dc-downloads/esp-idf-libs.zip

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
    ninja-build

RUN mkdir -p /usr/local/bin/gcc \
    && git clone https://github.com/STMicroelectronics/STM32CubeL4.git ./sources/STM32CubeL4 \
    && git clone --recursive https://github.com/azure-rtos/threadx.git ./sources/AzureRTOS \
    && git clone --branch stable_20.3.x https://github.com/nanoframework/chibios.git ./sources/ChibiOs \
    && git clone --branch nanoframework https://github.com/nanoframework/ChibiOS-Contrib.git ./sources/ChibiOs-Contrib \
    && git clone --branch mbedtls-2.23.0 https://github.com/nanoframework/mbedtls.git ./sources/mbedtls \
    && git clone --branch R0.14 https://github.com/abbrev/fatfs.git ./sources/fatfs \
    && git clone --branch V10.4.1-kernel-only https://github.com/FreeRTOS/FreeRTOS-Kernel.git ./sources/FreeRTOS \
    && git clone --branch 5.5.1 https://github.com/ARM-software/CMSIS_5.git ./sources/CMSIS_5 \
    && git clone --branch STABLE-2_0_3_RELEASE https://git.savannah.nongnu.org/git/lwip.git ./sources/lwip

# Copy from our other container
COPY --from=downloader /tmp/dc-extracted/gcc /usr/local/bin/gcc 
COPY --from=downloader /tmp/dc-extracted/cmake /usr
COPY --from=downloader /tmp/dc-extracted/esp-idf /sources/esp-idf
COPY --from=downloader /tmp/dc-extracted/esp-idf /sources/esp-idf_libs

# Putting hex2dfu in the container even if it will not be used, prep will fail otherwise
ARG HEX2DFU=https://github.com/nanoframework/hex2dfu/releases/latest/download/hex2dfu.exe
RUN mkdir -p /usr/local/bin/hex2dfu \
    && curl -o /usr/local/bin/hex2dfu/hex2dfu.exe $HEX2DFU

# Clean up downloaded files
RUN apt-get autoremove -y \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

# Switch back to dialog for any ad-hoc use of apt-get
ENV DEBIAN_FRONTEND=dialog