// cluster-entries: rwp_objects_cull_and_register_triggers_cb_408ad0
/* Installed cluster rwp_objects_cull_and_register_triggers_cb_408ad0 (tools/cluster_install.py from output/cluster_pilot_kb/rwp_objects_cull_and_register_triggers_cb_408ad0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00406d10*/ extern int fx_object_timer_subtract(int event_a, float event_b);
/*00492ca0*/ extern int rwp_object_activate(int object, float *velocity, int hit_position);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005dd364*/ extern float k_f_n1;
/*005ddb58*/ extern float k_f_1000;
/*005ddb5c*/ extern float k_f_n4;
/*005ddb60*/ extern float k_f_n8;
/*005ddc28*/ extern float k_f_0;
/*00651250*/ extern undefined g_scene_651250;
/*00651aa8*/ extern undefined4 g_fx_651aa8;
/* rwp_objects_cull_and_register_triggers_cb_408ad0 */

/* function pointer pushed/stored by rwp_objects_cull_and_register_triggers (MOV dword ptr [ESP +
   0x64],0x408ad0) */

static undefined * fx_find_object_by_owner_r(int p0);
extern void fx_find_object_by_owner(void);
/* (inferred names) collision callback: when event_a is a hit on the current fx object (type 0x1c) and event_b a hit of type 0x19 on another record, the fx object velocity (+0xdc..) is flipped
   and scaled, the direction to the other record added, and the rwp body is activated with a 1000x impulse (y made positive); the fx object timer is then reduced by 10. */
void rwp_objects_cull_and_register_triggers_cb_408ad0(char *event_a, char *event_b)

{
  char *fx_pos_base;
  int other_record;
  float vel_x;
  float vel_y;
  float vel_z;
  FxSceneObject *fx_object;
  undefined4 rwp_handle;
  float *impulse_ptr;
  int other_pos_ptr;
  RwV3d dir_to_other;
  struct {
    float impulse_x;
    float impulse_y;
    float impulse_z;
  } frame;
  
  if (((*(int *)(event_a + 0x10) == 0x1c) &&
      (fx_object = (FxSceneObject *)fx_find_object_by_owner_r(*(undefined4 *)(event_a + 0x18)),
      fx_object != (FxSceneObject *)0x0)) && ((undefined *)fx_object == &g_scene_651250 + g_fx_651aa8 * 0xec)) {
    fx_pos_base = *(char **)(fx_object->frame_a + 4);
    if ((*(int *)(event_b + 0x10) == 0x19) &&
       (other_record = *(int *)(event_b + 0x14), *(int *)(other_record + 0x20) != 0x3f99999a)) {
      dir_to_other.x = *(float *)(other_record + 0x14) - *(float *)(fx_pos_base + 0x40);
      other_pos_ptr = other_record + 0x14;
      dir_to_other.y = *(float *)(other_record + 0x18) - *(float *)(fx_pos_base + 0x44);
      dir_to_other.z = *(float *)(other_record + 0x1c) - *(float *)(fx_pos_base + 0x48);
      RwV3dNormalize(&dir_to_other,&dir_to_other);
      vel_x = fx_object->vel.x * k_f_n8;
      fx_object->vel.x = vel_x;
      vel_y = fx_object->vel.y * k_f_n4;
      fx_object->vel.y = vel_y;
      vel_z = fx_object->vel.z * k_f_n8;
      fx_object->vel.z = vel_z;
      fx_object->vel.x = vel_x + dir_to_other.x;
      fx_object->vel.y = vel_y + dir_to_other.y;
      fx_object->vel.z = vel_z + dir_to_other.z;
      frame.impulse_x = fx_object->vel.x * k_f_1000;
      frame.impulse_y = fx_object->vel.y * k_f_1000;
      frame.impulse_z = fx_object->vel.z * k_f_1000;
      if (frame.impulse_y < k_f_0) {
        frame.impulse_y = frame.impulse_y * k_f_n1;
      }
      rwp_handle = fx_object->rwp_object;
      impulse_ptr = &frame.impulse_x;
      rwp_object_activate(rwp_handle,impulse_ptr,other_pos_ptr);
      fx_object_timer_subtract((int)fx_object,10.0f);
    }
  }
  return;
}

/* fx_find_object_by_owner_r: C -> original fx_find_object_by_owner (registers esi) */
__declspec(naked) undefined * fx_find_object_by_owner_r(int p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call fx_find_object_by_owner
        pop esi
        ret
    }
}
