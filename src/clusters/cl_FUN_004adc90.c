// cluster-entries: FUN_004adc90
/* Installed cluster FUN_004adc90 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004adc90.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) void __stdcall InitializeCriticalSection(void *);
extern __declspec(dllimport) int __stdcall SetEvent(int);
extern char PTR_FUN_005e22a0[];
extern char PTR_FUN_005e2264[];
extern char PTR_LAB_005e238c[];
/* FUN_004adc90 */

static void FUN_004b0df0_r(int * p0, int p1, int p2, void * p3, int p4);
extern void FUN_004b0df0(void);
static void FUN_004b1060_r(int * p0, int p1);
extern void FUN_004b1060(void);
int *FUN_004adc90_n(int *in_ecx, int param_1, int param_2, int param_3, int param_4)

{
  FUN_004b0df0_r(in_ecx,param_2,param_3,in_ecx + 0x1f,param_1);
  *in_ecx = (int)PTR_FUN_005e22a0;
  in_ecx[3] = (int)PTR_FUN_005e2264;
  in_ecx[4] = (int)PTR_LAB_005e238c;
  in_ecx[0x14] = 0;
  FUN_004b1060_r(in_ecx + 0x15,0);
  FUN_004b1060_r(in_ecx + 0x16,1);
  FUN_004b1060_r(in_ecx + 0x17,1);
  in_ecx[0x18] = 0;
  in_ecx[0x19] = 0;
  in_ecx[0x1a] = 0;
  in_ecx[0x1b] = 0;
  in_ecx[0x1c] = 0;
  in_ecx[0x1d] = 0;
  in_ecx[0x1e] = 0;
  InitializeCriticalSection(in_ecx + 0x1f);
  InitializeCriticalSection(in_ecx + 0x25);
  in_ecx[0x2b] = 0;
  in_ecx[0x2c] = 1;
  in_ecx[0x2d] = 0;
  in_ecx[0x2e] = 0;
  in_ecx[0x2f] = 0;
  in_ecx[0x30] = 0;
  InitializeCriticalSection(in_ecx + 0x31);
  SetEvent(in_ecx[0x17]);
  return in_ecx;
}

/* FUN_004b0df0_r: C -> original FUN_004b0df0 (registers ecx,s0,s1,s2,s3) */
__declspec(naked) void FUN_004b0df0_r(int * p0, int p1, int p2, void * p3, int p4)
{
    __asm {
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        mov ecx, dword ptr [esp + 20]
        call FUN_004b0df0
        ret
    }
}

/* FUN_004b1060_r: C -> original FUN_004b1060 (registers ecx,s0) */
__declspec(naked) void FUN_004b1060_r(int * p0, int p1)
{
    __asm {
        push dword ptr [esp + 8]
        mov ecx, dword ptr [esp + 8]
        call FUN_004b1060
        ret
    }
}

/* entry thunk at the original address of FUN_004adc90: register inputs (ecx,s0,s1,s2,s3) -> natural C FUN_004adc90_n; ret 16 */
__declspec(naked) void FUN_004adc90(void)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push ecx
        call FUN_004adc90_n
        add esp, 20
        ret 16
    }
}
