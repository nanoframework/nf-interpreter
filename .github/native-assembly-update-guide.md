# Native Assembly Declaration Update Guide for nf-interpreter

This file documents the exact steps to follow when updating a native assembly declaration in nf-interpreter in response to an issue titled **"Update {LibraryName} declaration"**.

This guide is intended to be referenced from (or merged into) the Copilot instructions in the [nanoframework/Home](https://github.com/nanoframework/Home/blob/main/.github/copilot-instructions.md) repository for the cloud agent.

---

## 1. Read the issue body carefully

Extract all of the following before touching any file:

- **Library name** — from the issue title (`Update {LibraryName} declaration`)
- **Old checksum** — from the table (`Previously published` column)
- **New checksum** — from the table (`New` column)
- **Old native version** — from the table (`Previously published` column)
- **New native version** — from the table (`New` column)
- **Stubs artifact link** — direct zip URL or Azure DevOps build link in the issue body
- **Originating PR** — the PR in the class library repo that triggered this issue (e.g. `nanoframework/System.Device.Spi#171`)
- **This issue's number** — from the URL of the nf-interpreter issue (e.g. `#NNNN`)

---

## 2. Download the stub files

The stubs artifact ZIP contains the authoritative `.cpp`, `.h`, and `.cmake` files. **Do NOT generate or infer stub content.**

```bash
# Download and extract
curl -L -o /tmp/stubs.zip "<direct zip URL from issue body>"
unzip /tmp/stubs.zip -d /tmp/stubs
```

If the direct URL fails, use the Azure DevOps REST API:

```bash
# List artifacts
curl -s "https://dev.azure.com/nanoframework/{project}/_apis/build/builds/{buildId}/artifacts?api-version=7.1" | python3 -m json.tool

# Use the downloadUrl from the response
curl -L -o /tmp/stubs.zip "<downloadUrl from response>"
unzip /tmp/stubs.zip -d /tmp/stubs
```

After extracting, inspect the files. The artifact contains:
- **`{module}_native.cpp`**: method_lookup array + CLR_RT_NativeAssemblyData struct
- **`{module}_native.h`**: enum definitions + struct/field/method declarations
- **`Find{LibraryName}.cmake`**: CMake module — **ignore this file**, no changes needed

---

## 3. Identify the files to update in nf-interpreter

Search for the files by old checksum:

```bash
grep -r "0x{oldChecksum}" --include="*.cpp" --include="*.h" .
```

Typically one `.cpp` and one `.h` file under `src/` or `targets/`.

---

## 4. Update the `.cpp` file — three changes required

### 4a. Update `method_lookup[]` array (CRITICAL — most commonly missed)

The `method_lookup[]` array encodes the exact position of every managed method. A single wrong entry (including a misplaced `nullptr`) will cause runtime dispatch failures.

**Diff the entire array** between the artifact and nf-interpreter:

```bash
diff <artifact>/sys_dev_spi_native.cpp <nf-interpreter>/src/.../sys_dev_spi_native.cpp
```

Replace the nf-interpreter `method_lookup[]` body **exactly** with the artifact's array body. Check for:
- `nullptr` entries added or removed (these shift all subsequent indices)
- Named function entries added, removed, or reordered
- Total entry count (must match the artifact exactly)

For each **new** named function entry (appears in artifact but not in nf-interpreter):
- Add the function body from the artifact `.cpp`
- Add `NANOCLR_NATIVE_DECLARE(...)` in the `.h` file

For each **removed** named function entry (in nf-interpreter but not in artifact):
- Remove the function body **only if it is a stub** (returns `S_OK` / `NANOCLR_SET_AND_LEAVE_HR_VOID(S_OK)` / calls `stack.NotImplementedStub()`)
- If the function contains a real implementation, **do not delete it** — note the discrepancy in the PR description

### 4b. Update the checksum

```cpp
// Before:
0xOLDCHECKSUM,
// After:
0xNEWCHECKSUM,
```

### 4c. Update the native version tuple

```cpp
// Before:
{ 100, 2, 0, 0 }
// After:
{ 100, 2, 0, 1 }
```

---

## 5. Update the `.h` file — apply only additive changes

**CRITICAL RULES — read before touching the header:**

### What TO update:
- Add any new `FIELD___*` constants present in the artifact but missing in nf-interpreter
- Add any new `NANOCLR_NATIVE_DECLARE(...)` entries present in the artifact but missing in nf-interpreter
- Add any new struct blocks, enums, or typedefs that exist in the artifact but not in nf-interpreter

### What NOT to change:
- **Do NOT uncomment code** that is intentionally commented out. Comments such as `// moved to src\PAL\...` indicate deliberate decisions — if code is commented out in nf-interpreter but appears active in the artifact, preserve the commented-out form in nf-interpreter.
- **Do NOT remove helper method declarations** (e.g. `static HRESULT NativeTransfer(CLR_RT_StackFrame &stack, bool bufferIs16bits);`) that exist in nf-interpreter but not in the artifact. These declarations support real implementations in the `.cpp` file.
- **Do NOT remove** `#include` directives, `extern` references, or any other local additions that are absent from the artifact. They exist to support platform-specific implementations.
- **Do NOT remove** commented-out blocks or documentation comments.

> **Rule of thumb**: When the artifact `.h` is a *subset* of the nf-interpreter `.h` (artifact has less content), the correct action is to leave the extra content in place and only add what is genuinely new. When the artifact `.h` has *more* content (new entries), add only those new entries.

---

## 6. Pre-commit checklist

Before committing, verify **all** of the following:

- [ ] `method_lookup[]` entry count matches the artifact exactly
- [ ] `method_lookup[]` entry order matches the artifact exactly (every `nullptr` and named entry at the correct index)
- [ ] Checksum hex value updated to match the new value from the issue
- [ ] Version tuple updated to match the new value from the issue
- [ ] No existing real C++ function bodies were deleted
- [ ] No intentionally commented-out code was uncommented
- [ ] No local helper declarations were removed from `.h`
- [ ] All new named function entries have a corresponding function body in `.cpp`
- [ ] All new named function entries have a corresponding `NANOCLR_NATIVE_DECLARE(...)` in `.h`

---

## 7. PR title and description

**Title:**
```
Update {LibraryName} native declaration to v{newNativeVersion}
```

**Body — use the PR template exactly:**
```markdown
## Description
- Updated `{LibraryName}` native assembly declaration.
- Checksum: `{oldChecksum}` → `{newChecksum}`.
- Native version: `{oldNativeVersion}` → `{newNativeVersion}`.
- `method_lookup[]`: updated to match artifact (entry count changed).

## Motivation and Context
Resolves nanoFramework/nf-interpreter#{issueNumber}
Triggered by nanoframework/{ClassLibraryRepo}#{PR-number}

## How Has This Been Tested?
Stub files were taken directly from the CI pipeline artifact without modification.

## Screenshots
_N/A_

## Types of changes
- [ ] Improvement (non-breaking change that improves a feature, code or algorithm)
- [ ] Bug fix (non-breaking change which fixes an issue with code or algorithm)
- [ ] New feature (non-breaking change which adds functionality to code)
- [ ] Breaking change (fix or feature that would cause existing functionality to change)
- [ ] Config and build (change in the configuration and build system, has no impact on code or features)
- [ ] Dev Containers (changes related with Dev Containers, has no impact on code or features)
- [x] Dependencies/declarations (update dependencies or assembly declarations and changes associated, has no impact on code or features)
- [ ] Documentation (changes or updates in the documentation, has no impact on code or features)

## Checklist
- [ ] My code follows the code style of this project (only if there are changes in source code).
- [ ] My changes require an update to the documentation (there are changes that require the docs website to be updated).
- [ ] I have updated the documentation accordingly (the changes require an update on the docs in this repo).
- [x] I have read the [CONTRIBUTING](https://github.com/nanoframework/.github/blob/main/CONTRIBUTING.md) document.
- [ ] I have tested everything locally and all new and existing tests passed (only if there are changes in source code).
```

---

## Common mistakes to avoid

| Mistake | Correct behaviour |
|---|---|
| Uncommenting code that is intentionally commented out in nf-interpreter | Preserve the commented-out form; only add genuinely new content |
| Removing `static HRESULT Helper(...)` declarations from `.h` | Keep them — they support real implementations in `.cpp` |
| Updating only the checksum/version and skipping `method_lookup[]` | Always diff and update the complete `method_lookup[]` array |
| Copying the artifact `.h` verbatim over the nf-interpreter `.h` | Merge: add new entries, preserve all existing nf-interpreter content |
| Removing function body implementations from `.cpp` | Only remove stubs; never delete real implementations |
| Writing a custom PR description instead of following the template | Always use the template from this guide verbatim |
