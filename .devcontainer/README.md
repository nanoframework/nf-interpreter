# This folder contains all the elements used for .NET nanoFramework dev containers

You'll find Docker files for .NET nanoFramework. They are used to facilitate building images.

The available pre build images are:

* ghcr.io/nanoframework/dev-container-all: contains all elements to build a firmware image for any of the targets. **Important**: the size of this container is very large, if you are interested only in building targets on a single platform, you're better using one of the following images specific to that platform.
* ghcr.io/nanoframework/dev-container-chibios: contains all elements to build a firmware image for any of the ChibiOS targets
* ghcr.io/nanoframework/dev-container-ti: contains all elements to build a firmware image for any of the TI SimpleLink targets
* ghcr.io/nanoframework/dev-container-esp32: contains all elements to build a firmware image for any of the ESP32 targets
* ghcr.io/nanoframework/dev-container-azure-rtos: contains all elements to build a firmware image for any of the Eclipse ThreadX (a.k.a. Azure RTOS) targets
* ghcr.io/nanoframework/dev-container-freertos-nxp: contains all elements to build a firmware image for any of the NXP targets

You can choose the dev container needed when opening a remote container in VSCode. The options are:

* `nanoFramework-All` to use the pre build container with all the elements to build a firmware image for any of the targets
* `nanoFramework-AzureRTOS` to use the pre build container with all the elements to build Eclipse ThreadX (a.k.a. Azure RTOS) targets
* `nanoFramework-ChibiOS` to use the pre build container with all the elements to build ChibiOS targets
* `nanoFramework-ESP32` to use the pre build container with all the elements to build ESP32 targets
* `nanoFramework-TI` to use the pre build container with all the elements to build TI SimpleLink targets
* `nanoFramework-FreeRTOS-NXP` to use the pre build container with all the elements to build TI SimpleLink targets

To use the source dockerfile for the respective platform adjust its `devcontainer.json` file and change the `"dockerFile": "Dockerfile.<platform>"` element for the image you would like to use:

* `Dockerfile.All.SRC` to build the container image from the source with all the elements to build all the images
* `Dockerfile.AzureRTOS.SRC` to build the container image from the source with all the elements to build Eclipse ThreadX (a.k.a. Azure RTOS) based devices
* `Dockerfile.ChibiOS.SRC` to build the container image from the source with all the elements to build ChibiOS based devices
* `Dockerfile.ESP32.SRC` to build the container image from the source with all the elements to build ESP32 based devices
* `Dockerfile.TI.SRC` to build the container image from the source with all the elements to build TI SimpleLink based devices
* `Dockerfile.FreeRTOS.NXP.SRC` to build the container image from the source with all the elements to build NXP based devices


## Building and releasing Docker images in a fork

Add a "repository variable" called `PUBLISH_DOCKER_IMAGE` with the value `true` in your forked repository
See: https://docs.github.com/en/actions/learn-github-actions/variables#creating-configuration-variables-for-a-repository for further help.

**Note:** by default, the build and publish of the devcontainer docker images will still only happen when the docker source files change and are "pushed" to the `main` branch.
