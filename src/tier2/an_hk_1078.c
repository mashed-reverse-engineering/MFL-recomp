#include "mfl_tier2.h"
/*0056b220*/ extern void rwp_world_set_bit_60_64(void *p0, void *p1, int p2);
/*0058c8a0*/ extern char *rwp_owner_object_get(int p0);
/*006fab78*/ extern undefined4 g_rw_6fab78;
/* course_neustein_cb1_sub_48e5a0 */

/* single caller course_neustein_cb1_sub_454970 */

void course_neustein_cb1_sub_48e5a0(int param_1,int param_2)

{
  char *iVar1;
  char *iVar2;
  
  if ((((-1 < param_1) && (param_1 < 200)) && ((&g_rw_6fab78)[param_1] != 0)) &&
     (iVar2 = rwp_owner_object_get((&g_rw_6fab78)[param_1]), iVar2 != 0)) {
    iVar1 = *(char **)(iVar2 + 0x24);
    if (iVar1 != 0) {
      if (((*(uint *)(*(char **)(iVar1 + 100) + (uint)(*(ushort *)(iVar2 + 0x20) >> 5) * 4) &
           1 << ((byte)*(ushort *)(iVar2 + 0x20) & 0x1f)) != 0) && (param_2 == 0)) {
        rwp_world_set_bit_60_64(iVar1,iVar2,0);
        return;
      }
      if ((iVar1 != 0) &&
         ((*(uint *)(*(char **)(iVar1 + 100) + (uint)(*(ushort *)(iVar2 + 0x20) >> 5) * 4) &
          1 << ((byte)*(ushort *)(iVar2 + 0x20) & 0x1f)) != 0)) {
        return;
      }
    }
    if (param_2 != 0) {
      rwp_world_set_bit_60_64(iVar1,iVar2,1);
    }
  }
  return;
}


