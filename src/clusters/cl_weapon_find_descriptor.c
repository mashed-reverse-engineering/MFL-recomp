// cluster-entries: weapon_find_descriptor
/* Installed cluster weapon_find_descriptor (tools/cluster_install.py from output/cluster_pilot10/weapon_find_descriptor.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0060dc20*/ extern undefined4 weapon_descriptors;
/*0060de60*/ extern undefined4 weapon_descriptor_count;
/* weapon_find_descriptor */

/* WARNING: Unknown calling convention */
/* esi = weapon type id; linear search of weapon_descriptors (0x40 stride; count at
   weapon_descriptor_count); returns descriptor or 0 */

int * weapon_find_descriptor_n(int in_esi)

{
  int *piVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < (int)weapon_descriptor_count) {
    piVar1 = &weapon_descriptors;
    do {
      if (in_esi == *piVar1) {
        return piVar1;
      }
      iVar2 = iVar2 + 1;
      piVar1 = piVar1 + 0x10;
    } while (iVar2 < (int)weapon_descriptor_count);
  }
  return (int *)0x0;
}

/* entry thunk at the original address of weapon_find_descriptor: register inputs (esi) -> natural C weapon_find_descriptor_n; ret 0 */
__declspec(naked) void weapon_find_descriptor(void)
{
    __asm {
        push esi
        call weapon_find_descriptor_n
        add esp, 4
        ret
    }
}
