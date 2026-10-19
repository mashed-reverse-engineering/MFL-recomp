// cluster-entries: weapon_fx_explosion_pool_clear
/* Installed cluster weapon_fx_explosion_pool_clear (tools/cluster_install.py from output/cluster_pilot10/weapon_fx_explosion_pool_clear.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f4920*/ extern RpWorld * clump_get_world(RpClump * p0);
/* weapon_fx_explosion_pool_clear */

/* WARNING: Unknown calling convention */
/* "clears both slots of a pool and its counters" */

static int weapon_fx_explosion_slot_clear_r(undefined4 * p0);
extern void weapon_fx_explosion_slot_clear(void);
void weapon_fx_explosion_pool_clear_n(int in_edi)

{
  int reg_esi;
  RpWorld *world;
  int iVar1;
  
  iVar1 = 0;
  do {
    reg_esi = *(int *)(in_edi + iVar1 * 4);
    if (reg_esi != 0) {
      *(undefined4 *)(in_edi + iVar1 * 4) = 0;
      weapon_fx_explosion_slot_clear_r(reg_esi);
    }
    iVar1 = iVar1 + 1;
  } while (iVar1 < 2);
  *(undefined4 *)(in_edi + 8) = 0;
  *(undefined4 *)(in_edi + 0xc) = 0;
  *(undefined4 *)(in_edi + 0x10) = 0;
  mem_zero(in_edi,8);
  *(undefined4 *)(in_edi + 0x24) = 0;
  *(undefined4 *)(in_edi + 0x28) = 0;
  world = (RpWorld *)clump_get_world(*(undefined4 *)(in_edi + 0x20));
  if (world != (RpWorld *)0x0) {
    RpWorldRemoveClump(world,*(RpClump **)(in_edi + 0x20));
  }
  return;
}

/* weapon_fx_explosion_slot_clear_r: C -> original weapon_fx_explosion_slot_clear (registers esi) */
__declspec(naked) int weapon_fx_explosion_slot_clear_r(undefined4 * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_fx_explosion_slot_clear
        pop esi
        ret
    }
}

/* entry thunk at the original address of weapon_fx_explosion_pool_clear: register inputs (edi) -> natural C weapon_fx_explosion_pool_clear_n; ret 0 */
__declspec(naked) void weapon_fx_explosion_pool_clear(void)
{
    __asm {
        push edi
        call weapon_fx_explosion_pool_clear_n
        add esp, 4
        ret
    }
}
