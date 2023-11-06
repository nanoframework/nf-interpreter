# This folder contains all the elements used for .NET nanoFramework dev container

You'll find Docker files for .NET nanoFramework. They are used to facilitate building images.

The available pre build images are:

* ghcr.io/nanoframework/dev-container-all: contains all elements to build a firmware image for any of the targets. **Important**: the size of this container is very large, if you are interested only in building targets on a single platform, you're better using one of the following images specific to that platform.
* ghcr.io/nanoframework/dev-container-chibios: contains all elements to build a firmware image for any of the ChibiOS targets
* ghcr.io/nanoframework/dev-container-ti: contains all elements to build a firmware image for any of the TI SimpleLink targets
* ghcr.io/nanoframework/dev-container-esp32: contains all elements to build a firmware image for any of the ESP32 targets
* ghcr.io/nanoframework/dev-container-azure-rtos: contains all elements to build a firmware image for any of the Azure RTOS targets
* ghcr.io/nanoframework/dev-container-freertos-nxp: contains all elements to build a firmware image for any of the FreeRTOS NXP targets

To choose the dev container you want to use, select it when opening the remote container.


## Building and releasing Docker images in a fork

Add a "repository variable" called `PUBLISH_DOCKER_IMAGE` with the value `true` in your forked repository
See: https://docs.github.com/en/actions/learn-github-actions/variables#creating-configuration-variables-for-a-repository for further help.

**Note:** by default, the build and publish of the devcontainer docker images will still only happen when the docker source files change and are "pushed" to the `main` branch.
