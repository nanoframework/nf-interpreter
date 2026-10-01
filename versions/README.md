# Firmware versioning

> [!IMPORTANT]
> **Checklist for reviewers (human and bot).** A PR needs attention here when it:
>
> 1. **Changes `versions/pathfilters.json`** → the generated `versions/**/version.json` files must be updated in the same PR. Run `pwsh versions/Update-PathFilters.ps1 -Check`; it must report *up to date*.
> 2. **Adds, renames or removes** a target board, a platform/vendor folder under `targets/`, or a top-level folder that feeds the build → regenerate with `pwsh versions/Update-PathFilters.ps1` and commit the changed `version.json` files. Renamed or removed paths make the script fail until the spec is fixed.
> 3. **Changes how sources are assembled** (`CMake/**`, a target `CMakeLists.txt`, `FindXXX.cmake` modules, nanoBooter source lists) or **edits the spec** → run `pwsh versions/Test-BuildCoverage.ps1` on a build of an affected target (see [Coverage check](#coverage-check)) and mention the result in the PR.
> 4. **Edits a `version.json` by hand** → don't. Only the `version` field of the parent files (`versions/<image>/version.json`) is edited by hand; leaf files are generated.
>
> A missing path filter is a silent failure: changes to that file won't bump the version, so different binaries ship with the same version number.

## How it works

Each firmware image has its own version, per platform, computed by [Nerdbank.GitVersioning](https://github.com/dotnet/Nerdbank.GitVersioning) (nbgv):

| Version | What it versions | Where |
|---|---|---|
| `firmware/<platform>` | the firmware package (all the images together), used for the package name and version published to Cloudsmith | `versions/firmware/<platform>/version.json` |
| `nanoCLR/<platform>` | the nanoCLR image, reported as the CLR version | `versions/nanoCLR/<platform>/version.json` |
| `nanoBooter/<platform>` | the nanoBooter image, reported as the booter version (platforms with a nanoBooter only) | `versions/nanoBooter/<platform>/version.json` |

Platforms are the RTOS folders, split by vendor where the tree already is: `ChibiOS`, `ESP32`, `FreeRTOS-NXP`, `TI_SimpleLink`, `ThreadX-<vendor>`.

Versions have the format `major.minor.patch.height`:

- `major.minor.patch` is set by hand in the parent file, e.g. `versions/nanoCLR/version.json`, and applies to all the platforms of that image. It always has 3 numbers: that's what makes nbgv put the height in the 4th field.
- `height` is the number of commits since `major.minor.patch` was last changed that touched a path matched by the leaf's `pathFilters`. A commit that touches only ESP32 code doesn't change any ChibiOS version; a booter-only change doesn't change the nanoCLR version.
- Firmware packages keep the formats used before per-image versioning: `major.minor.patch.height` for stable and `major.minor.patch-preview.height` for preview.
- The versions started at `2.0.1`, so they sort above every `2.0.0.<build counter>` published before.

The leaves inherit the nbgv settings from the root `version.json`, which is still used for the WIN32, POSIX and netcore builds and for the release process.

> [!NOTE]
> The `version` in the root `version.json` does **not** set the firmware image versions: the leaves get their `major.minor.patch` from the parent files in `versions/`. The root `version` only applies to:
>
> - The builds that don't use `versionLeaf`: WIN32 and POSIX nanoCLR, and the nf-Community-Targets pipeline, as `major.minor.patch.<counter>`.
> - `nbgv prepare-release`, which names the release branch after it.
>
> Keep the root `version` equal to the one in `versions/firmware/version.json`, so the release branch name matches the firmware being released.
>
> Removing the root `version.json` requires moving those builds and the release process to leaves first. It also holds the nbgv settings the leaves inherit.

### Releases

The release process (`nbgv prepare-release`, run by the `StartReleaseCandidate` step in `azure-pipelines.yml`) only updates the root `version.json`. It doesn't touch the files in `versions/`:

- The heights keep increasing on their own, so every release gets new image versions whenever their sources changed.
- To start a new version for an image (e.g. nanoCLR `2.0.1` → `2.1.0`), edit the `version` in the parent file (`versions/firmware/version.json`, `versions/nanoCLR/version.json` or `versions/nanoBooter/version.json`) by hand, in its own commit. The height of that image restarts from 0 on all platforms.
- The firmware package version is the one nanoff and Cloudsmith see. When bumping an image version, consider bumping `versions/firmware/version.json` too, so the package version reflects the change. It isn't required: the firmware height keeps increasing anyway.
- When bumping `versions/firmware/version.json`, bump the root `version.json` to the same version in the same commit.
- Never lower a version: nanoff would see the new firmware as older than what's on the devices.

Versions are reproducible locally:

```powershell
nbgv get-version -p versions/nanoBooter/ChibiOS -v SimpleVersion
```

### In the build

- CMake receives `NANOCLR_VERSION` and `NANOBOOTER_VERSION`, both defaulting to `BUILD_VERSION` (so local builds and presets work unchanged).
- The generated `target_os.h` defines `NANOCLR_VERSION_*` and `NANOBOOTER_VERSION_*`, and maps `VERSION_*` to the image being built (`I_AM_NANOBOOTER`).
- In Azure Pipelines, `azure-pipelines-templates/nb-gitversioning.yml` computes the three versions when the job passes `versionLeaf`. Jobs without it (community targets, WIN32, POSIX) use the legacy root version plus build counter.

## Path filters

nbgv stable releases only support plain path prefixes in `pathFilters`. To keep them maintainable, the filters are generated:

- `versions/pathfilters.json` is the **source of truth**. Paths are repository-root relative, can use `*`, `?` and `**` wildcards, and `@name` references a named set. An exclude identical to an include is dropped, so a leaf can include its own platform files while excluding a set with all the platforms.
- `versions/Update-PathFilters.ps1` expands the spec against the files tracked by git and writes each leaf `version.json`.

Guidelines when editing the spec:

- **Over-including is safe**: it only causes extra version bumps.
- **Under-including is not**: a change wouldn't bump the version. When in doubt, include.
- Keep the booter's `src` list (`src-booter`) and the `clr-only` exclusions in sync with what the booter actually compiles; the coverage check tells you.
- The booter only tracks the Kconfig files listed in `kconfig-booter`. `Kconfig.apis`, `Kconfig.graphics` and `Kconfig.networking` are left out because no booter source uses their symbols. When adding a `Kconfig.*` file, or when booter code starts using a symbol from one of the excluded files, update `kconfig-booter`. The coverage check can't detect this, because Kconfig files aren't compiler inputs.

> [!NOTE]
> TODO: when nbgv 3.11 (wildcard support in `pathFilters`) is released as stable, consider emitting the wildcards directly instead of expanding them, so new boards don't require regenerating the files.

## Scripts

### Update the path filters

Run after changing `versions/pathfilters.json` or adding/renaming/removing boards or folders:

```powershell
pwsh versions/Update-PathFilters.ps1
```

Commit the spec and the regenerated `version.json` files together.

To only verify (no changes written, exits with 1 if anything is out of date), e.g. before opening a PR:

```powershell
pwsh versions/Update-PathFilters.ps1 -Check
```

### Coverage check

Verifies that every repository file compiled into an image is matched by that image's path filters. It reads the inputs of `<image>.elf` from the ninja deps log, so it needs a **complete ninja build**:

```powershell
cmake --preset ORGPAL_PALTHREE
cmake --build build
pwsh versions/Test-BuildCoverage.ps1 -BuildDir build -Component nanoBooter -Platform ChibiOS
pwsh versions/Test-BuildCoverage.ps1 -BuildDir build -Component nanoCLR -Platform ChibiOS
```

- Exit code 1 and a list of uncovered files means the spec must be fixed: add the missing paths (or narrow an exclusion), regenerate, and re-run.
- Uncovered *CMake configure inputs* are reported as warnings. Those excluded on purpose (e.g. other platforms' CMake files) aren't reported.
- `-ShowUnused` lists include filters that don't match anything in that build: candidates for tightening, as long as no other target of the platform uses them.
- It only checks the target that was built. Use a reference target per platform that enables most features.

When to run it:

- After changing the spec, especially the booter lists or any exclusion.
- After changing how sources are assembled for a platform (CMake modules, target `CMakeLists.txt`).
- When adding a new platform or vendor.

## In Azure Pipelines

Both scripts run in CI and report issues as annotations in the build summary and the PR checks.

| Check | Where | PR builds | Branch builds (develop, main, release, nightly) |
|---|---|---|---|
| `Update-PathFilters.ps1 -Check` | `Check_Code_Style` job | **error**: blocks all the firmware builds | not run |
| `Test-BuildCoverage.ps1` | after the build step of every firmware job (`azure-pipelines-templates/check-version-coverage.yml`), for each image built | **error**: the job fails before packaging and publishing | **warning**: the job is marked as succeeded with issues, publishing isn't blocked |

- The coverage check only covers the targets built in that run. A PR changing files under `versions/` (other than docs) triggers the core build, so the check runs.
- Jobs that don't pass `versionLeaf` to `nb-gitversioning.yml` (community targets, WIN32, POSIX) aren't checked.
- Locally, `Test-BuildCoverage.ps1 -WarnOnly` reproduces the branch build behavior.
