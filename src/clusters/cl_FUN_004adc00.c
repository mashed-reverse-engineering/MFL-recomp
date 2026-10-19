// cluster-entries: FUN_004adc00
/* Installed cluster FUN_004adc00 (tools/cluster_install.py from output/cluster_pilot_zy/FUN_004adc00.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004aee70*/ extern int __stdcall FUN_004aee70(void *object, int flag, int arg);
/*005e203c*/ extern char g_data_5e203c[];

static void FUN_004aef40_r(void * p0, int p1, int p2, int p3);
extern void FUN_004aef40(void);
/* Releases the cached COM interface of the object (QueryInterface with g_data_5e203c, hand it to FUN_004aef40 with code 0x15, Release) when `flag` is 0 and the object has one (+0x34), then forwards to FUN_004aee70. */
int FUN_004adc00_n(int *object, int flag, int arg)
{
  void *interface_ptr;

  if (flag == 0 && object[0xd] != 0) {
    ((int (__stdcall *)(void *, void *, void **))(*(int **)object)[0])(object, g_data_5e203c, &interface_ptr);
    FUN_004aef40_r((char *)object - 0xc, 0x15, (int)interface_ptr, 0);
    ((int (__stdcall *)(void *))(*(int **)interface_ptr)[2])(interface_ptr);
  }
  return FUN_004aee70(object, flag, arg);
}

/* FUN_004aef40_r: C -> original FUN_004aef40 (registers ecx,s0,s1,s2) */
__declspec(naked) void FUN_004aef40_r(void * p0, int p1, int p2, int p3)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        mov ecx, dword ptr [esp + 16]
        call FUN_004aef40
        ret
    }
}

/* entry thunk at the original address of FUN_004adc00: register inputs (s0,s1,s2) -> natural C FUN_004adc00_n; ret 12 */
__declspec(naked) void FUN_004adc00(void)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        call FUN_004adc00_n
        add esp, 12
        ret 12
    }
}
