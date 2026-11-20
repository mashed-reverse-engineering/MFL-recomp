# tools/video: FFmpeg movie backend (mfl_video_ffmpeg.dll)

Optional separate DLL that plays the game's MPEG-1 movies (intro.mpg, renderware.mpg, ...) behind the plain-C interface of
`recomp/mfl_video.h`. Built LGPL-only from the unmodified FFmpeg 7.1 release; see `SOURCES` for version, URL, sha256, configure flags and
LGPL notes. No binary and no upstream source lives in the repo; everything builds into `tools/video/work/` (untracked).

## Files
| file | what |
|---|---|
| `SOURCES` | upstream version/URL/sha256, configure flags, LGPL notes |
| `build_ffmpeg.sh win32\|native` | fetch + sha256-verify the tarball, configure/build/install a minimal FFmpeg: win32 SHARED (the shipped DLLs), native static (needs `i686-w64-mingw32-gcc` for win32, plain gcc for native, curl, make) |
| `build_dll.sh` | `work/out/`: `mfl_video_ffmpeg.dll` (i686, libgcc static, imports the FFmpeg DLLs + KERNEL32/msvcrt) and the FFmpeg DLLs `avformat-61 avcodec-61 avutil-59 swscale-8.dll` (imports KERNEL32/msvcrt/bcrypt), `harness.exe`, native harness (when `work/prefix/native` exists) |
| `../ci/build_video_dll.sh <out>` | CI (build.yml job `video`): FFmpeg (skipped on a cache hit) + miniaudio.h + the DLL set into `<out>`; `pe_check.py --video` checks each DLL |

## Shipping layout (W16 VIDCI)
The release zip ships the five DLLs beside `MFLng.exe`, plus `LICENSES/ffmpeg.txt` (third_party/LICENSES: version, URL, sha256, configure line,
LGPL 2.1 text). FFmpeg stays in its own unmodified shared DLLs that `mfl_video_ffmpeg.dll` links dynamically, so the user can replace them
(LGPL 2.1 section 6(b)); no GPL part is enabled. Without `mfl_video_ffmpeg.dll` the exe runs with the blank backend: no intro movies,
no menu backdrop movie, no menu music (the music is the sound track of `frontend.mpg`). The launcher's updater installs these names too.
| `src/mfl_video_ffmpeg.c` | the backend: exports `mfl_video_open start update acquire_frame finished get_size set_volume close` (cdecl) + `mfl_video_ffmpeg_next` (clock-free step for the harness) |
| `src/harness.c` | test harness: native (links the backend source) or `harness.exe` (LoadLibrary of the DLL, like the exe) |
| `test/run_test.sh [movie dir]`, `test/pixdiff.py` | offline acceptance (needs `work/out/harness_native`, system ffmpeg/ffprobe) |

Exe side: `recomp/video/mfl_video_null.c` already has the loader glue in the repo. At the first `mfl_video_open` with no backend registered it `LoadLibraryA("mfl_video_ffmpeg.dll")` and fills a .bss backend table from the
8 exports; no DLL or a missing export keeps the blank backend (unchanged behaviour). No string literals / initialised writable statics (T-P15 rule).
It was only syntax-checked with i686 mingw, not compiled with cl 13: after applying, check the standalone link map `.data` length is still 00052000H.

## Build
    sh tools/video/build_ffmpeg.sh win32      # ~2 min
    sh tools/video/build_ffmpeg.sh native
    sh tools/video/build_dll.sh               # -> tools/video/work/out/
    sh tools/video/test/run_test.sh           # optional, offline
`WORK=<dir>` moves the build tree. Put every `.dll` of `work/out/` next to the exe only where movies should play; gate dirs without the DLL are unchanged.

## FFmpeg configuration and behaviour
No asm (C paths: same pixels on every target), no threads (call only from the game thread), file protocol, mpegps/mpegvideo demuxers, mpeg1video + mp2
decoders (every movie of the game is MPEG-1 video + MP2 audio in an MPEG program stream, renderware.mpg a raw MPEG-1 video stream; movie
sound since W16 mva1: own miniaudio device, `set_volume` = the original dB curve). `open` decodes the first frame (a file without video frame fails open);
`update` shows every frame whose pts is due and returns 1 if the shown frame changed; `finished` = stream ended and the last frame was on screen for one
frame duration; `acquire_frame` = swscale to BGRA, pitch w*4, valid until the next update/close; `loop` ignored (all callers pass 0). Frame time comes
from the sequence header (29.97 fps), not the pack header (25 fps).

## Acceptance state (offline, 2026-11-03)
renderware.mpg 241 frames and intro.mpg 1909 frames, 640x480: two runs give identical per-frame FNV-64 hashes, frame counts equal ffprobe, the last
renderware frame is byte-identical to system ffmpeg `-pix_fmt bgra`. Real-time path: 241 frames shown, finished after 8083 ms.
NOT run: `harness.exe` + the DLL under Wine (expected output identical to the native harness).

## Open
* Exe side frame transfer is not written (`movie_poll_and_set_surface` TODO: acquire + copy into the RwRaster). With the DLL present the movies run for
  their real length (intro 63.7 s, renderware 8 s) with nothing drawn from them, instead of being skipped at once; scenarios that wait through the intros take longer.
* Movie audio silent until the audio backend exists; `loop=1` not implemented (no caller).
* In-game check (intro reaches its end event) and Wine run of `harness.exe` belong to the Wine phase.
