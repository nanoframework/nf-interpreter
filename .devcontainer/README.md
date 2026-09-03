# This folder contains all the elements used for .NET nanoFramework dev containers

You'll find Docker files for .NET nanoFramework. They are used to facilitate building images.

The available pre build images are:

* ghcr.io/nanoframework/dev-container-all: contains all elements to build a firmware image for any of the targets. **Important**: the size of this container is very large, if you are interested only in building targets on a single platform, you're better using one of the following images specific to that platform.
* ghcr.io/nanoframework/dev-container-chibios: contains all elements to build a firmware image for any of the ChibiOS targets
* ghcr.io/nanoframework/dev-container-ti: contains all elements to build a firmware image for any of the TI SimpleLink targets
* ghcr.io/nanoframework/dev-container-esp32: contains all elements to build a firmware image for any of the ESP32 targets
* ghcr.io/nanoframework/dev-container-azure-rtos: contains all elements to build a firmware image for any of the Eclipse ThreadX (used to be Azure RTOS) targets
* ghcr.io/nanoframework/dev-container-freertos-nxp: contains all elements to build a firmware image for any of the NXP targets
* ChibiOS-RP: contains all elements to build a firmware image for Raspberry Pi Pico (RP2040/RP2350) ChibiOS targets. Uses ChibiOS master branch which includes RP support.

You can choose the dev container needed when opening a remote container in VSCode. The options are:

* `nanoFramework-All` to use the pre build container with all the elements to build a firmware image for any of the targets
* `nanoFramework-ThreadX` to use the pre build container with all the elements to build Eclipse ThreadX (used to be Azure RTOS) targets
* `nanoFramework-ChibiOS` to use the pre build container with all the elements to build ChibiOS targets
* `nanoFramework-ESP32` to use the pre build container with all the elements to build ESP32 targets
* `nanoFramework-TI` to use the pre build container with all the elements to build TI SimpleLink targets
* `nanoFramework-FreeRTOS-NXP` to use the pre build container with all the elements to build TI SimpleLink targets
* `nanoFramework-ChibiOS-RP` to build from source a container with all the elements to build Raspberry Pi Pico RP2040/RP2350 ChibiOS targets

To use the source dockerfile for the respective platform adjust its `devcontainer.json` file and change the `"dockerFile": "Dockerfile.<platform>"` element for the image you would like to use:

* `Dockerfile.All.SRC` to build the container image from the source with all the elements to build all the images
* `Dockerfile.ThreadX.SRC` to build the container image from the source with all the elements to build Eclipse ThreadX (used to be Azure RTOS) based devices
* `Dockerfile.ChibiOS.SRC` to build the container image from the source with all the elements to build ChibiOS based devices
* `Dockerfile.ESP32.SRC` to build the container image from the source with all the elements to build ESP32 based devices
* `Dockerfile.TI.SRC` to build the container image from the source with all the elements to build TI SimpleLink based devices
* `Dockerfile.FreeRTOS.NXP.SRC` to build the container image from the source with all the elements to build NXP based devices
* `Dockerfile.ChibiOS-RP.SRC` to build the container image from the source with all the elements to build Raspberry Pi Pico (RP2040/RP2350) ChibiOS targets

## Non-root `vscode` user

All dev containers are meant to run as a non-root `vscode` user (`"remoteUser": "vscode"` with
`"updateRemoteUserUID": true` in `devcontainer.json`). This keeps files created in the bind-mounted
workspace owned by your host user instead of `root`.

This fix is implemented in the `*.SRC` Dockerfiles. The **prebuilt images published to ghcr.io**
(the ones used by the plain, non-`.SRC` `Dockerfile.<platform>` files) will only pick it up once
they are rebuilt and republished — which happens when the version tag in the corresponding
`Dockerfile.<platform>` is bumped and pushed to `main`. Until then, containers started from a
prebuilt image tag that predates the fix do **not** have a `vscode` user.

### Symptoms of an outdated/stale container

* VS Code fails to start the container with an error such as:
  `Error response from daemon: unable to find user vscode: no matching entries in passwd file`
* You are dropped into the container as `root` instead of `vscode`.
* Files created in the workspace from inside the container are owned by `root` on the host.

### How to fix it

1. **Try rebuilding first.** From the Command Palette in VS Code, run
   `Dev Containers: Rebuild Container` (or `Dev Containers: Rebuild Without Cache` if the first
   attempt doesn't pick up changes). If the prebuilt image on ghcr.io has already been
   republished with the fix, this is all you need.
2. **If rebuilding still fails**, the prebuilt image itself doesn't have the `vscode` user yet.
   Remove the stale container and any locally cached image built from it, then either wait for
   an updated prebuilt image or build from source locally as a workaround:

   ```bash
   # Find containers for this workspace/devcontainer config
   docker ps -a --filter label=devcontainer.local_folder=<path-to-your-workspace>

   # Remove the stale container(s)
   docker rm -f <container-id>

   # Remove the locally built image derived from the devcontainer (tagged vsc-<repo>-<hash>)
   docker images --filter reference='vsc-nf-interpreter-*'
   docker rmi <image-id>
   ```

   Then reopen the folder in a container (or run `Dev Containers: Rebuild Container` again).
3. **Workaround to unblock immediately**: edit the platform's `devcontainer.json` and change
   `"dockerFile": "Dockerfile.<platform>"` to `"dockerFile": "Dockerfile.<platform>.SRC"`. This
   builds the image from source locally (slower, since it downloads toolchains, but it always
   includes the `vscode` user fix) instead of pulling the prebuilt ghcr.io image. Revert this
   change once the prebuilt image has been republished.
4. **Clean up root-owned files left over from before the fix.** If you built or ran the
   container as `root` before this change, files/directories such as `build/` or `sdkconfig`
   in the bind-mounted workspace may still be owned by `root` on the host. Fix them once with:

   ```bash
   sudo chown -R "$USER":"$USER" .
   ```
   
   or simply delete the affected build directories and let the build regenerate them.

## Building and releasing Docker images in a fork

Add a "repository variable" called `PUBLISH_DOCKER_IMAGE` with the value `true` in your forked repository
See: https://docs.github.com/en/actions/learn-github-actions/variables#creating-configuration-variables-for-a-repository for further help.

**Note:** by default, the build and publish of the devcontainer docker images will still only happen when the docker source files change and are "pushed" to the `main` branch.
