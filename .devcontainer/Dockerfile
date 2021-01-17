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

ARG IDF_LIBS_URI='https://bintray.com/nfbot/internal-build-tools/download_file?file_path=IDF_libs-v3.3.1.zip'
RUN curl -o /tmp/dc-downloads/esp-idf-libs.zip $IDF_LIBS_URI \
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
    && git clone --recursive https://github.com/azure-rtos/threadx.git ./sources/AzureRTOS

# Copy from our other container
COPY --from=downloader /tmp/dc-extracted/gcc /usr/local/bin/gcc 
COPY --from=downloader /tmp/dc-extracted/cmake/bin /usr/local/bin/cmake
COPY --from=downloader /tmp/dc-extracted/esp-idf /sources/esp-idf
COPY --from=downloader /tmp/dc-extracted/esp-idf /sources/esp-idf_libs

# Clean up downloaded files
RUN apt-get autoremove -y \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

# Switch back to dialog for any ad-hoc use of apt-get
ENV DEBIAN_FRONTEND=dialog