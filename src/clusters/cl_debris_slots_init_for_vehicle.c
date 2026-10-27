// cluster-entries: debris_slots_init_for_vehicle
/* Installed cluster debris_slots_init_for_vehicle (tools/cluster_install.py from output/cluster_pilot11/debris_slots_init_for_vehicle.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/* debris_slots_init_for_vehicle */

/* "clears the record, stores the vehicle and builds the 2 half debris slot sets" */

static int debris_slots_init_from_atomics_r(undefined4 p0, int p1, undefined4 p2, int p3);
extern void debris_slots_init_from_atomics(void);
void debris_slots_init_for_vehicle_n(int in_edi, undefined4 in_eax, undefined4 param_3)

{
  int reg_ebx;
  int reg_eax_00;
  undefined1 local_50 [80];
  
  mem_zero(in_edi,0x208);
  mem_zero(local_50,0x50);
  *(undefined4 *)(in_edi + 0x204) = param_3;
  *(undefined4 *)(in_edi + 0x200) = in_eax;
  clump_get_atomics(in_eax,local_50);
  reg_eax_00 = 0;
  reg_ebx = in_edi;
  do {
    debris_slots_init_from_atomics_r(reg_eax_00,reg_ebx,in_edi,(int)local_50);
    reg_eax_00 = reg_eax_00 + 1;
    reg_ebx = reg_ebx + 0x100;
  } while (reg_eax_00 < 2);
  return;
}

/* debris_slots_init_from_atomics_r: C -> original debris_slots_init_from_atomics (registers eax,ebx,ecx,s0) */
__declspec(naked) int debris_slots_init_from_atomics_r(undefined4 p0, int p1, undefined4 p2, int p3)
{
    __asm {
        push ebx
        push dword ptr [esp + 20]
        mov eax, dword ptr [esp + 12]
        mov ebx, dword ptr [esp + 16]
        mov ecx, dword ptr [esp + 20]
        call debris_slots_init_from_atomics
        add esp, 4
        pop ebx
        ret
    }
}

/* entry thunk at the original address of debris_slots_init_for_vehicle: register inputs (edi,eax,s0) -> natural C debris_slots_init_for_vehicle_n; ret 0 */
__declspec(naked) void debris_slots_init_for_vehicle(void)
{
    __asm {
        push dword ptr [esp + 4]
        push eax
        push edi
        call debris_slots_init_for_vehicle_n
        add esp, 12
        ret
    }
}
