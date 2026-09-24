//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Update session registry: the single gate through which an upgrade image is staged into a
// secondary slot at nanoCLR run time.
//
// Every writer (managed nanoFramework.Runtime.InFieldUpdate API, Wire Protocol Monitor_ImageWrite,
// future native in-field update agents) opens a session for an image, receives an opaque token and
// must present it on every chunk. One session per image, so two writers can never interleave
// chunks. Chunks are strictly sequential: the registry owns the write position, callers just append.
//
// Resume after a reboot needs no persistent record - the secondary slot itself is the state:
// Ifu_SessionResume() re-reads the MCUboot header already stored, finds the high-water mark (last
// byte that is not the erased value), rewinds to the start of the erase block holding it and
// re-erases that block, then reopens the session at that position. See MCUboot/docs/session-update.md.
//
// Status and owner codes are NOT declared here: they are the managed UpdateSessionResult and
// UpdateSessionOwner enums, which the interop generator emits into nf_runtime_ifu.h from the C#
// declarations. That assembly header is the single source of truth for both sides.

#ifndef MCUBOOT_UPDATE_SESSION_H
#define MCUBOOT_UPDATE_SESSION_H

#include <stdint.h>
#include <stdbool.h>
// the assembly header comes first: it pulls in nanoHAL, whose ASSERT macro clashes with MCUboot's
// when bootutil is seen first (same order as src/CLR/Debugger/Debugger.cpp)
#include <nanoFramework.Runtime.InFieldUpdate/nf_runtime_ifu.h>
#include <bootutil/image.h>

#ifdef __cplusplus
extern "C"
{
#endif

// State of one image's staging slot. Callers keep Token and hand it to every later call.
struct Ifu_Session
{
    bool Open;
    // set while a flash operation for this session is executing outside the registry lock
    bool WriteInFlight;
    uint8_t Image;
    UpdateSessionOwner Owner;
    uint32_t Token;
    // offset of the image inside the secondary flash area (Ifu_ImageOffset)
    uint32_t ImageOffset;
    // largest image the slot accepts (Ifu_UsableSize)
    uint32_t UsableSize;
    // total image length declared by the caller (header + payload + TLVs)
    uint32_t TotalLength;
    // image-relative position of the next chunk; == TotalLength when everything is stored
    uint32_t NextOffset;
    // true once the MCUboot header has been seen (after the first chunk, or on resume)
    bool HeaderKnown;
    struct image_header Header;
};

// Open a fresh session: erases the whole secondary slot and positions at 0.
// seed is mixed into the token (any changing value, e.g. the system tick counter).
UpdateSessionResult Ifu_SessionStart(
    uint8_t image,
    UpdateSessionOwner owner,
    uint32_t totalLength,
    uint32_t seed,
    Ifu_Session *out);

// Reopen a session on an image already partially stored in the secondary slot.
// expectedHeader may be NULL; when given, min(expectedHeaderLen, sizeof(struct image_header)) bytes
// are compared with the stored header. On success out->NextOffset is where the caller must
// continue from (the block containing the high-water mark has been re-erased).
UpdateSessionResult Ifu_SessionResume(
    uint8_t image,
    UpdateSessionOwner owner,
    uint32_t totalLength,
    const uint8_t *expectedHeader,
    uint32_t expectedHeaderLen,
    uint32_t seed,
    Ifu_Session *out);

// Append len bytes at the session's NextOffset. The first chunk must carry a full image header.
// On return *nextOffset (optional) holds the new position.
UpdateSessionResult Ifu_SessionWrite(
    uint8_t image,
    uint32_t token,
    const uint8_t *data,
    uint32_t len,
    uint32_t *nextOffset);

// Verify the staged image (header, TLV structure, SHA-256) and, on success, mark it pending for a
// test swap on the next reboot. Closes the session on every outcome except
// UpdateSessionResult_Incomplete. outSnapshot (optional) receives the final session state.
UpdateSessionResult Ifu_SessionComplete(uint8_t image, uint32_t token, Ifu_Session *outSnapshot);

// Close the session. With eraseSlot the secondary slot is erased; without it the partial image
// stays in place so Ifu_SessionResume() can pick it up later ("pause").
UpdateSessionResult Ifu_SessionAbort(uint8_t image, uint32_t token, bool eraseSlot);

// Erase the secondary slot of an image. Refused with UpdateSessionResult_Busy while any session is
// open on it.
UpdateSessionResult Ifu_EraseSecondary(uint8_t image);

// Owner of the session currently open on an image, UpdateSessionOwner_None when closed.
UpdateSessionOwner Ifu_SessionOwner(uint8_t image);

// Token of the session open on an image when it belongs to `owner`, 0 otherwise. Lets a writer
// recover its own token instead of caching it in state of its own that would need resetting.
uint32_t Ifu_SessionToken(uint8_t image, UpdateSessionOwner owner);

// Outcome of the most recent registry call, for UpdateManager.GetLastSessionError(). Shared by
// every writer, so it is a diagnostic aid and not a reliable per-caller error channel.
UpdateSessionResult Ifu_SessionLastStatus(void);

// Drop every session and clear the last status (RAM state only; flash is untouched, so a paused
// download stays resumable). Called from nanoHAL_Initialize()/nanoHAL_Uninitialize() so the
// registry always starts and ends a CLR lifetime clean, including on a CLR-only reboot where
// static state would otherwise survive.
void Ifu_SessionResetAll(void);

// Registry lock hooks. Weak no-ops in MCUboot_UpdateSession.cpp; the RTOS target overrides them
// because Wire Protocol handlers run on the receiver thread, concurrently with the CLR.
void Ifu_SessionLock(void);
void Ifu_SessionUnlock(void);

#ifdef __cplusplus
}
#endif

#endif // MCUBOOT_UPDATE_SESSION_H
