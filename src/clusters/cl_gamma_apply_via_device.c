// cluster-entries: gamma_apply_via_device
/* Installed cluster gamma_apply_via_device (tools/cluster_install.py from output/cluster_pilot_hb1/gamma_apply_via_device.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* gamma_apply_via_device */
/* compile: /GS */

typedef int (__stdcall *ComMethod2)(int *self, void *caps);
typedef int (__stdcall *ComMethod3)(int *self, int a, void *ramp);
typedef int (__stdcall *ComMethod4)(int *self, int a, int b, void *ramp);

/* "calls the device gamma interface when supported and builds the ramp" (p1, p2: the two gamma floats the caller pushes) */

static int gamma_table_build_r(unsigned short * p0, float p1, float p2);
extern void gamma_table_build(void);
undefined4 gamma_apply_via_device_n(int *in_esi, float p1, float p2)

{
  int iVar1;
  struct {
    int reserved[3];
    unsigned int flags;
    char rest[0x120];
  } caps;
  unsigned short ramp[0x300];
  
  iVar1 = (*(ComMethod2 *)(*in_esi + 0x1c))(in_esi,&caps);
  if ((-1 < iVar1) && ((caps.flags & 0x20000) != 0)) {
    (*(ComMethod3 *)(*in_esi + 0x58))(in_esi,0,ramp);
    gamma_table_build_r(ramp,p1,p2);
    (*(ComMethod4 *)(*in_esi + 0x54))(in_esi,0,0,ramp);
    return 1;
  }
  return 0;
}

/* gamma_table_build_r: C -> original gamma_table_build (registers eax,s0,s1) */
__declspec(naked) int gamma_table_build_r(unsigned short * p0, float p1, float p2)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        mov eax, dword ptr [esp + 12]
        call gamma_table_build
        add esp, 8
        ret
    }
}

/* entry thunk at the original address of gamma_apply_via_device: register inputs (esi,s0,s1) -> natural C gamma_apply_via_device_n; ret 0 */
__declspec(naked) void gamma_apply_via_device(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push esi
        call gamma_apply_via_device_n
        add esp, 12
        ret
    }
}
