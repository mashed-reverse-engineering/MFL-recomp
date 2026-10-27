#include "mfl_tier2.h"
/*00406270*/ extern int scene_object_select_clear();
/*0042b030*/ extern undefined __cdecl course_unload(undefined4 p0);
/*0042b110*/ extern int current_course_id();
/*00467d90*/ extern int weapons_unequip_all_a();
/*00467df0*/ extern int weapons_unequip_all_b();
/*00480af0*/ extern int physics_scenarios_reset();
/*00481f70*/ extern int car_wheel_contacts_clear_all();
/*004967f0*/ extern int triggers_assign_ids();
/*0049fee0*/ extern int materials_clear_textures_88602c();
/*00801b14*/ extern undefined4 g_rw_801b14;


/* course_unload_current */

/* "unequips all weapons, resets and unloads the current course" */

void course_unload_current(void)
{
  undefined4 course_id;
  
  materials_clear_textures_88602c();
  weapons_unequip_all_a();
  weapons_unequip_all_b();
  course_id = current_course_id();
  car_wheel_contacts_clear_all();
  course_unload(course_id);
  scene_object_select_clear();
  g_rw_801b14 = 0;
  triggers_assign_ids();
  physics_scenarios_reset();
  return;
}
