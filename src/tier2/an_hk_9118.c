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

/* Register a sphere trigger from desc (8 floats/dwords: x,y,z,radius,type,owner,flags,callback) into the g_triggers pool; zero its cell table, add it to the broad phase grid if enabled; returns 1 on success, 0 if full or grid insert fails. */
undefined4 trigger_sphere_register(float *desc)

{
  struct Trigger *trigger_slot;
  Trigger *trigger_new;
  uint *clear_ptr;
  int clear_count;
  uint trigger_index;
  undefined4 *pad_ptr;
  
  if ((g_particles_71aafc == 0) && (g_trigger_71aaf0 != 0)) {
    trigger_grid_build_visible_cells();
  }
  trigger_index = g_trigger_count;
  if (299 < g_trigger_count) {      /* unsigned compare (jb) */
    return 0;
  }
  trigger_new = g_triggers + g_trigger_count;
  trigger_new->position_radius[0] = *desc;
  trigger_slot = &g_triggers[trigger_index];
  trigger_slot->position_radius[1] = desc[1];
  trigger_slot->position_radius[2] = desc[2];
  trigger_slot->position_radius[3] = desc[3];
  trigger_slot->type = *(int *)&desc[4];
  trigger_slot->owner = *(int *)&desc[5];
  trigger_slot->flags = *(int *)&desc[6];
  trigger_slot->callback = *(int *)&desc[7];
  clear_ptr = &trigger_slot->cell_count;
  trigger_slot->index = (ushort)trigger_index;
  *clear_ptr = 0;
  clear_count = 0x10;
  do {
    clear_ptr = clear_ptr + -1;
    clear_count = clear_count + -1;
    *clear_ptr = 0;
  } while (clear_count != 0);
  pad_ptr = (undefined4 *)&trigger_slot->field_0x68;
  for (clear_count = 9; clear_count != 0; clear_count = clear_count + -1) {
    *pad_ptr = 0;
    pad_ptr = pad_ptr + 1;
  }
  if ((g_trigger_71aaf0 != 0) &&
     (clear_count = trigger_sphere_register_in_grid(trigger_new), trigger_index = g_trigger_count, clear_count == 0)) {
    return 0;
  }
  g_trigger_count = trigger_index + 1;
  return 1;
}


