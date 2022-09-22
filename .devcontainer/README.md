# This folder contains all the elements used for .NET nanoFramework dev container

You'll find Docker files for .NET nanoFramework. They are used to facilitate building images.

The available pre build images are:

* ghcr.io/nanoframework/dev-container-all: contains all elements to build a firmware image for any of the targets. **Important**: the size of this container is very large, if you are interested only in building targets on a single platform, you're better using one of the following images specific to that platform.
* ghcr.io/nanoframework/dev-container-chibios: contains all elements to build a firmware image for any of the ChibiOS targets
* ghcr.io/nanoframework/dev-container-ti: contains all elements to build a firmware image for any of the TI SimpleLink targets
* ghcr.io/nanoframework/dev-container-esp32: contains all elements to build a firmware image for any of the ESP32 targets
* ghcr.io/nanoframework/dev-container-azure-rtos: contains all elements to build a firmware image for any of the Azure RTOS targets

To choose the dev container you want to use, adjust `devcontainer.json` and change the `"dockerFile": "Dockerfile"` elements for the image you'd liked to use:

* `Dockerfile.All` to use the pre build container with all the elements to build a firmware image for any of the targets
* `Dockerfile.AzureRTOS` to use the pre build container with all the elements to build Azure RTOS targets
* `Dockerfile.ChibiOS` to use the pre build container with all the elements to build ChibiOS targets
* `Dockerfile.ESP32` to use the pre build container with all the elements to build ESP32 targets
* `Dockerfile.TI` to use the pre build container with all the elements to build TI SimpleLink targets
* `./sources/Dockerfile.All` to build the container image from the source with all the elements to build all the images
* `./sources/Dockerfile.AzureRTOS` to build the container image from the source with all the elements to build Azure RTOS based devices
* `./sources/Dockerfile.ChibiOS` to build the container image from the source with all the elements to build ChibiOS based devices
* `./sources/Dockerfile.ESP32` to build the container image from the source with all the elements to build ESP32 based devices
* `./sources/Dockerfile.TI` to build the container image from the source with all the elements to build TI SimpleLink based devices
