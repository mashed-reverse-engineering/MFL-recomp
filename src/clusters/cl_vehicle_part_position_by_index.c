// cluster-entries: vehicle_part_position_by_index
/* Installed cluster vehicle_part_position_by_index (tools/cluster_install.py from output/cluster_pilot_hb1/vehicle_part_position_by_index.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004222a0*/ extern void vehicle_part_matrix(void *vehicle, int part, RwMatrix *out);
/* vehicle_part_position_by_index */

/* "positions of vehicle part slots 0..6 (parts 0x21,0x22,0x24,0x23,0x10,0x35,0x36) into the output vec3" */

int vehicle_part_position_by_index_n(void *in_ecx, int in_eax, RwV3d *in_esi)

{
  int part;
  RwMatrix matrix;
  
  switch (in_eax) {
  case 0:
    part = 0x21;
    break;
  case 1:
    part = 0x22;
    break;
  case 2:
    part = 0x24;
    break;
  case 3:
    part = 0x23;
    break;
  case 4:
    part = 0x10;
    break;
  case 5:
    part = 0x35;
    break;
  case 6:
    part = 0x36;
    break;
  default:
    return 0;
  }
  vehicle_part_matrix(in_ecx,part,&matrix);
  *in_esi = matrix.pos;
  return in_eax;
}

/* entry thunk at the original address of vehicle_part_position_by_index: register inputs (ecx,eax,esi) -> natural C vehicle_part_position_by_index_n; ret 0 */
__declspec(naked) void vehicle_part_position_by_index(void)
{
    __asm {
        push esi
        push eax
        push ecx
        call vehicle_part_position_by_index_n
        add esp, 12
        ret
    }
}
