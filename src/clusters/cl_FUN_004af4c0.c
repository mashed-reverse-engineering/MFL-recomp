// cluster-entries: FUN_004af4c0
/* Installed cluster FUN_004af4c0 (tools/cluster_install.py from output/cluster_pilot_uc3/FUN_004af4c0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"

/* Binds `source` (COM interface) with the optional `filter`: a filter that FUN_004b2840 rejects is passed straight to FUN_004af330, otherwise the enumerators of the two sides (source itself when its index equals the byte at +0x26, else the sub object at +0xc; vtable +0x30)
   are searched with FUN_004af3e0.  Returns 0 on success, else the last unexpected error (default 0x80040207). */
static int FUN_004af330_r(void * p0, int p1, int p2);
extern void FUN_004af330(void);
static int FUN_004af3e0_r(void * p0, int p1, int p2, int * p3);
extern void FUN_004af3e0(void);
static int FUN_004b2840_r(int p0);
extern void FUN_004b2840(void);
int FUN_004af4c0_n(char *this_, int *source, int filter)
{
  int *enumerator;
  int error;
  int result;
  int side;
  int *iface;

  enumerator = 0;
  if (filter != 0 && FUN_004b2840_r(filter) == 0) {
    return FUN_004af330_r(this_, (int)source, filter);
  }
  error = 0x80040207;
  for (side = 0; side < 2; side++) {
    iface = side == *(unsigned char *)(this_ + 0x26) ? source : (int *)(this_ + 0xc);
    result = ((int (__stdcall *)(void *, int **))(*(int **)iface)[12])(iface, &enumerator);
    if (result >= 0) {
      result = FUN_004af3e0_r(this_, (int)source, filter, enumerator);
      ((int (__stdcall *)(void *))(*(int **)enumerator)[2])(enumerator);
      if (result >= 0) {
        return 0;
      }
      if (result != 0x80004005 && result != 0x80070057 && result != 0x8004022a) {
        error = result;
      }
    }
  }
  return error;
}

/* FUN_004af330_r: C -> original FUN_004af330 (registers ecx,s0,s1) */
__declspec(naked) int FUN_004af330_r(void * p0, int p1, int p2)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        mov ecx, dword ptr [esp + 12]
        call FUN_004af330
        ret
    }
}

/* FUN_004af3e0_r: C -> original FUN_004af3e0 (registers ecx,s0,s1,s2) */
__declspec(naked) int FUN_004af3e0_r(void * p0, int p1, int p2, int * p3)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        mov ecx, dword ptr [esp + 16]
        call FUN_004af3e0
        ret
    }
}

/* FUN_004b2840_r: C -> original FUN_004b2840 (registers ecx) */
__declspec(naked) int FUN_004b2840_r(int p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call FUN_004b2840
        ret
    }
}

/* entry thunk at the original address of FUN_004af4c0: register inputs (ecx,s0,s1) -> natural C FUN_004af4c0_n; ret 8 */
__declspec(naked) void FUN_004af4c0(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push ecx
        call FUN_004af4c0_n
        add esp, 12
        ret 8
    }
}
