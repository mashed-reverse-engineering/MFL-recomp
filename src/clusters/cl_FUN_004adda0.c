// cluster-entries: FUN_004adda0
/* Installed cluster FUN_004adda0 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004adda0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) void __stdcall DeleteCriticalSection(void *);
extern char PTR_FUN_005e22a0[];
extern char PTR_FUN_005e2264[];
extern char PTR_LAB_005e238c[];
typedef int (__fastcall *ThisMethod1)(int *self, int unused_edx, int arg);
/* FUN_004adda0 */

static int FUN_004acb00_r(int * p0);
extern void FUN_004acb00(void);
static int FUN_004ac8c0_r(int * p0);
extern void FUN_004ac8c0(void);
static void FUN_004b1080_r(int * p0);
extern void FUN_004b1080(void);
static void FUN_004b00b0_r(int * p0);
extern void FUN_004b00b0(void);
void FUN_004adda0_n(int *in_ecx)

{
  *in_ecx = (int)PTR_FUN_005e22a0;
  in_ecx[3] = (int)PTR_FUN_005e2264;
  in_ecx[4] = (int)PTR_LAB_005e238c;
  FUN_004acb00_r(in_ecx);
  FUN_004ac8c0_r(in_ecx);
  if (in_ecx[0x14] != 0) {
    (*(ThisMethod1 *)(*(int *)(in_ecx[0x14] + 8) + 0xc))((int *)(in_ecx[0x14] + 8),0,1);
    in_ecx[0x14] = 0;
  }
  if (in_ecx[0x1e] != 0) {
    (*(ThisMethod1 *)(*(int *)in_ecx[0x1e] + 0xc))((int *)in_ecx[0x1e],0,1);
    in_ecx[0x1e] = 0;
  }
  DeleteCriticalSection(in_ecx + 0x31);
  DeleteCriticalSection(in_ecx + 0x25);
  DeleteCriticalSection(in_ecx + 0x1f);
  FUN_004b1080_r(in_ecx + 0x17);
  FUN_004b1080_r(in_ecx + 0x16);
  FUN_004b1080_r(in_ecx + 0x15);
  FUN_004b00b0_r(in_ecx);
  return;
}

/* FUN_004acb00_r: C -> original FUN_004acb00 (registers ecx) */
__declspec(naked) int FUN_004acb00_r(int * p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call FUN_004acb00
        ret
    }
}

/* FUN_004ac8c0_r: C -> original FUN_004ac8c0 (registers ecx) */
__declspec(naked) int FUN_004ac8c0_r(int * p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call FUN_004ac8c0
        ret
    }
}

/* FUN_004b1080_r: C -> original FUN_004b1080 (registers ecx) */
__declspec(naked) void FUN_004b1080_r(int * p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call FUN_004b1080
        ret
    }
}

/* FUN_004b00b0_r: C -> original FUN_004b00b0 (registers ecx) */
__declspec(naked) void FUN_004b00b0_r(int * p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call FUN_004b00b0
        ret
    }
}

/* entry thunk at the original address of FUN_004adda0: register inputs (ecx) -> natural C FUN_004adda0_n; ret 0 */
__declspec(naked) void FUN_004adda0(void)
{
    __asm {
        push ecx
        call FUN_004adda0_n
        add esp, 4
        ret
    }
}
