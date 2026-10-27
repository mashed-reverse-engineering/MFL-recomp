#include "mfl_tier2.h"
/*00496330*/ extern int trigger_grid_build_visible_cells();
/*00496620*/ extern int __cdecl trigger_sphere_register_in_grid(void *p0);
/*007106e0*/ extern struct Trigger g_triggers[300];
/*0071aaf0*/ extern undefined4 g_trigger_71aaf0;
/*0071aaf4*/ extern undefined4 g_trigger_count;
/*0071aafc*/ extern undefined4 g_particles_71aafc;
/* trigger_sphere_register */

/* "registers a collision trigger {x,y,z,radius,type,owner,0,callback} into the pool of 300 Trigger
   structs (g_triggers, 0x8C bytes each) and into the 11 x 11 broad phase grid; returns 0 when full"
    */

undefined4 trigger_sphere_register(float *desc)

{
  struct Trigger *trigger_b;
  Trigger *trigger_a;
  uint *puVar2;
  int iVar3;
  uint trigger_index;
  undefined4 *puVar5;
  
  if ((g_particles_71aafc == 0) && (g_trigger_71aaf0 != 0)) {
    trigger_grid_build_visible_cells();
  }
  trigger_index = g_trigger_count;
  if (299 < g_trigger_count) {      /* unsigned compare (jb) */
    return 0;
  }
  trigger_a = g_triggers + g_trigger_count;
  trigger_a->position_radius[0] = *desc;
  trigger_b = &g_triggers[trigger_index];
  trigger_b->position_radius[1] = desc[1];
  trigger_b->position_radius[2] = desc[2];
  trigger_b->position_radius[3] = desc[3];
  trigger_b->type = *(int *)&desc[4];
  trigger_b->owner = *(int *)&desc[5];
  trigger_b->flags = *(int *)&desc[6];
  trigger_b->callback = *(int *)&desc[7];
  puVar2 = &trigger_b->cell_count;
  trigger_b->index = (ushort)trigger_index;
  *puVar2 = 0;
  iVar3 = 0x10;
  do {
    puVar2 = puVar2 + -1;
    iVar3 = iVar3 + -1;
    *puVar2 = 0;
  } while (iVar3 != 0);
  puVar5 = (undefined4 *)&trigger_b->field_0x68;
  for (iVar3 = 9; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar5 = 0;
    puVar5 = puVar5 + 1;
  }
  if ((g_trigger_71aaf0 != 0) &&
     (iVar3 = trigger_sphere_register_in_grid(trigger_a), trigger_index = g_trigger_count, iVar3 == 0)) {
    return 0;
  }
  g_trigger_count = trigger_index + 1;
  return 1;
}


