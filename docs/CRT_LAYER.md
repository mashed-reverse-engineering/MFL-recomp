# CRT layer (swappable C runtime)

Status: design notes from static analysis (2026-10-29). Decision (user): the CRT becomes a swappable library layer,
open-source libc per platform. Nothing here changes the build.

## Open: which libc
Two analyses disagree and the choice is open.
* This document (2026-10-29) recommends musl for string/stdio/malloc/libm.
* The earlier CRT analysis (ROADMAP, 2026-10-19, libcmt delink split) rejected Wine, ReactOS and musl as sources for the game's CRT and favoured: (A) rebuilding from the VC7 CRT source (private oracle, not publishable), (G) importing the system msvcrt plus a small shim (not bit-exact), and mingw-w64 pieces (public domain) for x87 math and startup.
Both are kept here until a gate run (determinism sweeps, libm x87) decides. Whatever is chosen must pass the heap and libm gates listed under Plan.

## Code without any source
About 23.6 KB of the CRT region is the floating-point package (`_ftol`/`_CI*` helpers, FP exception/status handling) and the C++/SEH exception-handling support. No source for it exists anywhere (not in musl, mingw-w64, Wine or the VC7 CRT source we have). It needs a hand lift from the original bytes (decompiler output cleaned to C, x87 operation order kept), with diffemu checks; estimate 15-30 agent hours for the whole CRT layer.

## What the game links
VC7 (VS .NET 2002) static multithreaded CRT `libcmt.lib`: 513 functions matched by bytes (`tools/libmatch_lib.py`,
`output/crt_libmatch.csv`, `output/crt_libsyms.csv` 871 symbols, `names/MFL_crt_names.csv`), 242 members are overlaid
by `tools/rw_overlay_prep.py`. Everything is ORIGINAL BYTES today: no CRT function has a C replacement in `recomp/`
(the few "C-def" hits like `exit`/`floor`/`entry` are call wrappers or game functions that happen to share a name; `entry` 004b4ba7 is the
CRT startup in tier1 C). The CRT region is 0x4b0b70..0x4d0000 plus 0x5d3000..0x5d5500 (libm/FPU tail). Not decompilation work.

## Functions the game code calls (counts = call sites in recomp C sources, tier1/automatch/clusters; rough)
| Function (addr) | Sites | State | Replacement | Caveat |
|---|---|---|---|---|
| `_ftol` 004b2b48 / `__ftol` 005d317c | 100+ (compiler emitted for float->int) | original | NOT libc: compiler helper; provide `mfl_ftol` with the same semantics | truncation toward zero via x87 control word switch; must keep exact behaviour (rounding mode), also for out of range values (0x80000000) |
| `sprintf` 004b2a60, `vsprintf` 004b42b5, `_vsnprintf` 004b3b74 | 66 | original | musl `vsnprintf` family | `%f` of floats prints (checked via text / replays), locale; game formats log/UI strings; musl is correct, VC7 rounds differently on exotic values; no `%I64` issues seen |
| `memset` 004bbe30, `memcpy` 004bbe90, `memmove` 004bdba0 | 20+ | original | libc | none; `rep movs` bytes only; overlapping `memcpy` in original is memmove-safe by accident, keep an eye |
| `strcat`, `strcpy`, `_strncpy`, `_strchr`, `_strrchr`, `_strstr`, `strtok`, `_strlen`, `__stricmp`, `toupper`/`_tolower` | 30+ | original | libc (`strcasecmp` for `_stricmp`) | `__stricmp` has a MSVC specific return value (lower-case difference) only matters if sign-compared; locale = "C" |
| `_malloc` 004b45eb, `_free_004b45fd` (free), `operator_new` 004b3c0c, `_msize`/heap | 10+ | original | libc malloc/free | HEAP LAYOUT AFFECTS DETERMINISM: replays/oracle runs depend on heap pointer values and fill (see ROADMAP "heap fill pattern"); the CRT small block heap (`__sbh`) pads/zeroes differently; do the swap with the determinism gates on |
| `_fopen`, `_fclose` 004b4358, `_fread` 004b49bf, `_fwrite` 004b4b12, `_fseek` 005d3133, `_fputs` 004b4244, `_fgets` | 20+ | original | libc stdio | text vs binary mode (`"rb"` vs CRLF translation), `FILE` layout is cast in a few places (`struct _iobuf`): wrap, do not expose; 64 bit offsets: `_fseek` here is long |
| `_ctime64` 004b43a9, `_ftime64` 004b43c2, `time` | 8 | original | libc `time`, `ftime`/`clock_gettime` | only logging/timestamps (ROADMAP: replay/rand not driven by CRT `rand`/`time`) |
| `exit` 004b322b, `_exit`, `entry` 004b4ba7 | 25 | original / C | libc `exit`; startup replaced by `main` shim | `entry` is CRT startup + `WinMain` call; must keep `_cinit` order (FP init `_fpinit`, `_RTC`, static ctors) |
| `_hypot` 005d3087, `atan` 004b35d0, `atan2` (game `atan2_degrees` wraps it), `floor` 004b2f90, `ceil` 005d3460, `fabs`, `fmod` 005d3580, `finite` 004b38f7 | 40+ | original | musl libm (`hypot`, `atan`, `atan2`, `floor`, `ceil`, `fmod`, `isfinite`) | see x87 below |
| `_CIacos` 004b3270, `_CIasin` 004b39b0, `_CIsin` 004b36f0, `_CIcos` 004b37a0, `_CIsqrt` 005d4250, `_CIpow`/`_pow` 004b2c30, `_CIpow_pentium4` 004b60f0, `_CIexp*` | used implicitly by `acos`/`sin`/`cos`/`pow`/`sqrt`/`exp` (compiler `_CI` forms: argument in st0, result in st0) | original | musl libm / platform libm via a `_CI*` adapter in assembly or C | **x87 precision**, see below |
| `_rand` 005d366b, `_atof` 005d3bb6, `_sscanf` 005d368d, `qsort` 005d38b0 | few | original | libc | `qsort` is not stable in either; sort-compare ties (cars/ranking) could reorder; `rand` unused by gameplay (Lua `math_random`) |
| `wcslen`, `wcscpy`, `_wcsncpy` | 3 | original | libc | wchar_t is 16 bit on Windows, 32 bit on Linux: only the video path (`movie_renderer_create`) and text use it; give the game its own `wchar16` type |
| `__chkstk` 004b3430, `__alldiv`, `__allmul`, `_RTC_*`, `_except_handler3`, `_CxxThrowException`, SEH/EH frames | few | original | compiler support (libgcc/compiler-rt on other targets, or keep) | x86-only; not a libc function |
| Lua 4.0 `lua_*` 4c7xxx, zlib, qhull | n/a | not CRT | own library sources | separate library tracks |

Not CRT but sits in the same address range and is NOT replaceable by libc: RenderWare file/stream shims (0x4c3xxx..0x4c6xxx, `piz_*`),
`stream_read_file` etc., `RwFrame*` (0x4d0xxx). They are game/engine code.

## x87 precision (the real constraint)
- The game is built with /G5-era VC7 x87 code: floats are carried in 80 bit registers and spilled as 32 bit when stored. VC7 libm functions
  (`_CIacos`, `_CIpow`, `_CIsin`...) compute in x87 (`fsin`, `fpatan`, `fyl2x`+`f2xm1`). musl on x86 uses its own algorithms (correct to < 1 ulp
  double); results differ in the last bits of the double and therefore after rounding to float in some cases. x87 `fsin/fcos` also differ from
  musl above |x| > ~2^63 and lose accuracy near multiples of pi.
- Precision control (verified from the original bytes): the CRT start-up (`_fpmath` 004b2b2f -> `_setdefaultprecision` 004b5dd3 -> `_controlfp(_PC_53, _MCW_PC)`) leaves the x87 at 53-bit precision. Once RenderWare's D3D9 layer starts the device system it calls a helper that clears the precision bits and masks all exceptions (control word 0x7f, 24-bit mantissa, round to nearest; the device is also created without `D3DCREATE_FPU_PRESERVE`), saves the old word and restores it when the device system stops; the video-mode path sets it again. So menus and races run at 24-bit precision control: every x87 add/multiply rounds to float, which is why SSE `float` code is close to the original on ARM and why "extended precision" 1-ulp differences do not exist in play. The exponent range stays extended (a float store still overflows to infinity). diffemu models it with `DIFFEMU_FPUCW=0x7f` (VERIFICATION.md).
- Consequence: physics/AI paths that use `acos`, `pow`, `atan2` (e.g. `ai_*`, `car_*`, camera) can diverge from the original replays in single bits and
  amplify (chaotic courses 2/5/6 are already unstable, see ROADMAP). Therefore:
  1. Keep VC7 original bytes for the oracle/gate builds (the current hybrid).
  2. For new platforms use an x87-faithful implementation only where the sweeps show a difference: candidates in order: musl (`acos`, `pow`, `atan2` are accurate and
     correctly rounded in most cases) -> glibc (CORE-MATH style, < 1 ulp) -> a port of the VC7 routine (the bytes are known, Ghidra C exists for `_CIacos`).
  3. Wrap every libm call behind `mfl_m_*` names so that one switch selects "faithful" vs "platform".
  4. `_ftol` must truncate (x87 `fistp` with RC=chop); on SSE targets use `cvttss2si`/`cvttsd2si` — identical results for in-range values, out-of-range is 0x80000000 on both.
- Float formatting: `sprintf("%f")` of values already rounded to float is identical; do not feed doubles that depend on x87 extended residue.

## Plan
1. Introduce a header `mfl_crt.h` with `mfl_memcpy`, `mfl_sprintf`, `mfl_m_acos`... (macro names to the existing CRT symbols on Windows hybrid, to libc on others). No code yet.
2. Order: string/memory (trivial) -> stdio (file mode) -> malloc (determinism gate) -> libm (x87 gate) -> startup (`entry`).
3. Gate each step with the determinism sweeps (heap and libm are the risky ones); keep the VC7 CRT bytes as the reference build.

## Other library layers (inventory, 2026-10-30)
Besides the CRT and RenderWare layers, these pieces of the exe are not application code and still need a layer decision:
* **D3DX** (statically linked): the shader assembler/tokenizer (about 9 KB), D3DX math folded into RenderWare objects, and the texture-create helper. Inventory is still to be done.
* **RtPNG / libpng / zlib**: the RenderWare texture-loading path (about 100 KB); zlib is not wired into the source-library build yet.
* **Platform layers**: Direct3D 9 (device vtable calls by offset in at least 21 files), DirectSound, DirectInput (data formats), and Win32 (window, dialog procedures for the controller and video dialogs). Fine on Windows x86 as is; a platform interface (like `VIDEO_INTERFACE.md`) is needed for any other target. No inventory document exists yet.
