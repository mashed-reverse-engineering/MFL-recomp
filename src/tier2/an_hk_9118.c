#include "mfl_tier2.h"
/*00496330*/ extern int trigger_grid_build_visible_cells();
/*00496620*/ extern int __cdecl trigger_sphere_register_in_grid(undefined4 p0);
/*007106e0*/ extern struct Trigger g_triggers[300];
/*0071aaf0*/ extern undefined4 g_trigger_71aaf0;
/*0071aaf4*/ extern undefined4 g_trigger_count;
/*0071aafc*/ extern undefined4 g_particles_71aafc;
/* trigger_sphere_register */

/* "registers a collision trigger {x,y,z,radius,type,owner,0,callback} into the pool of 300 Trigger
   structs (g_triggers, 0x8C bytes each) and into the 11 x 11 broad phase grid; returns 0 when full"
    */

undefined4 trigger_sphere_register(float *param_1)

{
  struct Trigger *p_uVar4;
  Trigger *pTVar1;
  uint *puVar2;
  int iVar3;
  uint uVar4;
  undefined4 *puVar5;
  
  if ((g_particles_71aafc == 0) && (g_trigger_71aaf0 != 0)) {
    trigger_grid_build_visible_cells();
  }
  uVar4 = g_trigger_count;
  if (299 < g_trigger_count) {      /* unsigned compare (jb) */
    return 0;
  }
  pTVar1 = g_triggers + g_trigger_count;
  pTVar1->position_radius[0] = *param_1;
  p_uVar4 = &g_triggers[uVar4];
  p_uVar4->position_radius[1] = param_1[1];
  p_uVar4->position_radius[2] = param_1[2];
  p_uVar4->position_radius[3] = param_1[3];
  p_uVar4->type = *(int *)&param_1[4];
  p_uVar4->owner = *(int *)&param_1[5];
  p_uVar4->flags = *(int *)&param_1[6];
  p_uVar4->callback = *(int *)&param_1[7];
  puVar2 = &p_uVar4->cell_count;
  p_uVar4->index = (ushort)uVar4;
  *puVar2 = 0;
  iVar3 = 0x10;
  do {
    puVar2 = puVar2 + -1;
    iVar3 = iVar3 + -1;
    *puVar2 = 0;
  } while (iVar3 != 0);
  puVar5 = (undefined4 *)&p_uVar4->field_0x68;
  for (iVar3 = 9; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar5 = 0;
    puVar5 = puVar5 + 1;
  }
  if ((g_trigger_71aaf0 != 0) &&
     (iVar3 = trigger_sphere_register_in_grid((undefined4)pTVar1), uVar4 = g_trigger_count, iVar3 == 0)) {
    return 0;
  }
  g_trigger_count = uVar4 + 1;
  return 1;
}


