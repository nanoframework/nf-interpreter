# Notes about CMake usage

## Reusing downloaded build dependencies

Dependencies fetched with CMake `FetchContent` are placed in `build/_deps`. A plain reconfigure
reuses them without downloading again. CMake keeps the records of which repositories were already
cloned under `build/CMakeFiles`, though. Running `cmake --fresh`, "Delete Cache and Reconfigure" in
VS Code, or cleaning the build directory while keeping only `_deps` removes those records, so the
next configure clones the dependencies again.

To avoid these downloads in local builds, use one of the following:

- **`FETCHCONTENT_FULLY_DISCONNECTED=ON`**: skips all download and update steps and uses the
  sources already in `build/_deps`. Enable it only *after* a successful configure of the target
  (for example with `-DFETCHCONTENT_FULLY_DISCONNECTED=ON` or in the `user-prefs` preset).
  CMake stops with an error if a required dependency hasn't been downloaded yet, so turn it off
  when switching to a target that needs other dependencies or when a dependency version changes.
  It has no effect on ThreadX, USBX, FileX, NetX Duo and ESP-IDF, which are fetched with
  `FetchContent_Populate`.
- **Local clones**: point the options in `config/user-tools-repos.json` (for example
  `CHIBIOS_SOURCE_FOLDER`, `MBEDTLS_SOURCE`, `LWIP_SOURCE`, `THREADX_SOURCE_FOLDER` or
  `ESP32_IDF_PATH`; see `config/user-tools-repos.TEMPLATE.json`) to your own clones. The build
  uses them as they are, so make sure they are checked out at the required version.
- **`FETCHCONTENT_SOURCE_DIR_<NAME>`**: CMake's generic override for any dependency declared with
  `FetchContent_Declare`, where `<NAME>` is the upper-case dependency name (for example
  `FETCHCONTENT_SOURCE_DIR_MBEDTLS`). Useful for dependencies without a dedicated option above.

Dependency versions remain controlled by their declared Git revisions; `UPDATE_DISCONNECTED ON`
does not automatically pull upstream branch changes. Messages saying a dependency is "from GitHub
repo" identify its source, not necessarily a new download.
