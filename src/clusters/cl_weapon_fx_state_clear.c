// cluster-entries: weapon_fx_state_clear
/* Installed cluster weapon_fx_state_clear (tools/cluster_install.py from output/cluster_pilot11/weapon_fx_state_clear.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/* weapon_fx_state_clear */

/* "clears the weapon fx state record" */

void weapon_fx_state_clear_n(int *in_esi)

{
  int player;
  unsigned int flags_c;
  unsigned int flags_4c;
  
  player = in_esi[0xb0 / 4];
  mem_zero(in_esi,0xb4);
  flags_c = in_esi[0xc / 4];
  in_esi[0x10 / 4] = 0;
  in_esi[0x8 / 4] = 0;
  in_esi[0x4 / 4] = 0;
  in_esi[0x24 / 4] = 0;
  in_esi[0x20 / 4] = 0;
  in_esi[0x18 / 4] = 0;
  in_esi[0x38 / 4] = 0;
  in_esi[0x34 / 4] = 0;
  in_esi[0x30 / 4] = 0;
  in_esi[0x50 / 4] = 0;
  in_esi[0x48 / 4] = 0;
  in_esi[0x44 / 4] = 0;
  in_esi[0x64 / 4] = 0;
  in_esi[0x60 / 4] = 0;
  in_esi[0x58 / 4] = 0;
  in_esi[0x78 / 4] = 0;
  in_esi[0x74 / 4] = 0;
  in_esi[0x70 / 4] = 0;
  flags_4c = in_esi[0x4c / 4];
  in_esi[0xb0 / 4] = player;
  in_esi[0xc / 4] = flags_c | 0x20003;
  in_esi[0x28 / 4] = 0x3f800000;
  in_esi[0x14 / 4] = 0x3f800000;
  in_esi[0] = 0x3f800000;
  in_esi[0x68 / 4] = 0x3f800000;
  in_esi[0x54 / 4] = 0x3f800000;
  in_esi[0x40 / 4] = 0x3f800000;
  in_esi[0x4c / 4] = flags_4c | 0x20003;
  in_esi[0x8c / 4] = 0x3e000000;
  return;
}

/* entry thunk at the original address of weapon_fx_state_clear: register inputs (esi) -> natural C weapon_fx_state_clear_n; ret 0 */
__declspec(naked) void weapon_fx_state_clear(void)
{
    __asm {
        push esi
        call weapon_fx_state_clear_n
        add esp, 4
        ret
    }
}
