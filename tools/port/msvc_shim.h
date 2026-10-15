/* msvc_shim.h: lets a non-MSVC compiler (gcc/clang, LP64 or AArch64) parse the VC7-style sources for tools/portcheck.py (syntax + pointer/int cast diagnostics only; never linked).
   Mirrors recomp/auto/prelude.h and ghidra_types.h (types come from the file's own includes). */
#define __cdecl
#define __stdcall
#define __fastcall
#define __thiscall
#define __declspec(x)
#define __forceinline inline
#define __int64 long long
#define __int32 int
#define __int16 short
#define __int8 char
#define __ptr32
#define __unaligned
