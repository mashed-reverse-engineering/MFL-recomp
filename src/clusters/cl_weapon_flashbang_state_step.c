// cluster-entries: weapon_flashbang_state_step
/* Installed cluster weapon_flashbang_state_step (tools/cluster_install.py from output/cluster_pilot11/weapon_flashbang_state_step.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/* weapon_flashbang_state_step */

/* "transform frame by state 1..3 then advances state 2->3->4->5" */

static int weapon_fx_clump_remove_a_r(int p0);
extern void weapon_fx_clump_remove_a(void);
void weapon_flashbang_state_step_n(int in_eax)

{
  int iVar1;
  
  if ((0 < *(int *)(in_eax + 0xc)) && (*(int *)(in_eax + 0xc) < 4)) {
    RwFrameTransform(*(RwFrame **)(*(char **)(in_eax + 0x10) + 4),*(RwMatrix **)(in_eax + 8),
                     rwCOMBINEREPLACE);
  }
  iVar1 = *(int *)(in_eax + 0xc);
  if ((iVar1 != 0) && (iVar1 != 1)) {
    if (iVar1 == 2) {
      *(undefined4 *)(in_eax + 0xc) = 3;
      return;
    }
    if (iVar1 == 3) {
      *(undefined4 *)(in_eax + 0xc) = 4;
      return;
    }
    if (iVar1 == 4) {
      *(undefined4 *)(in_eax + 0xc) = 5;
      return;
    }
    if (iVar1 == 5) {
      *(undefined4 *)(in_eax + 0xc) = 0;
      weapon_fx_clump_remove_a_r(in_eax);
    }
  }
  return;
}

/* weapon_fx_clump_remove_a_r: C -> original weapon_fx_clump_remove_a (registers esi) */
__declspec(naked) int weapon_fx_clump_remove_a_r(int p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_fx_clump_remove_a
        pop esi
        ret
    }
}

/* entry thunk at the original address of weapon_flashbang_state_step: register inputs (eax) -> natural C weapon_flashbang_state_step_n; ret 0 */
__declspec(naked) void weapon_flashbang_state_step(void)
{
    __asm {
        push eax
        call weapon_flashbang_state_step_n
        add esp, 4
        ret
    }
}
