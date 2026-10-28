#include "mfl_tier2.h"
/*0056b220*/ extern void rwp_world_set_bit_60_64(void *p0, void *p1, int p2);
/*0058c8a0*/ extern char *rwp_owner_object_get(int p0);
/*006fab78*/ extern undefined4 g_rw_6fab78;
/* course_neustein_cb1_sub_48e5a0 */

/* single caller course_neustein_cb1_sub_454970 */

/* Set/clear object (looked up by slot_index in g_rw_6fab78, max 200) membership in its world bitmask via rwp_world_set_bit_60_64 depending on enable (inferred). */
void course_neustein_cb1_sub_48e5a0(int slot_index,int enable)

{
  char *world;
  char *object;
  
  if ((((-1 < slot_index) && (slot_index < 200)) && ((&g_rw_6fab78)[slot_index] != 0)) &&
     (object = rwp_owner_object_get((&g_rw_6fab78)[slot_index]), object != 0)) {
    world = *(char **)(object + 0x24);
    if (world != 0) {
      if (((*(uint *)(*(char **)(world + 100) + (uint)(*(ushort *)(object + 0x20) >> 5) * 4) &
           1 << ((byte)*(ushort *)(object + 0x20) & 0x1f)) != 0) && (enable == 0)) {
        rwp_world_set_bit_60_64(world,object,0);
        return;
      }
      if ((world != 0) &&
         ((*(uint *)(*(char **)(world + 100) + (uint)(*(ushort *)(object + 0x20) >> 5) * 4) &
          1 << ((byte)*(ushort *)(object + 0x20) & 0x1f)) != 0)) {
        return;
      }
    }
    if (enable != 0) {
      rwp_world_set_bit_60_64(world,object,1);
    }
  }
  return;
}


