// cluster-entries: course_object_atomic_register_cb_473d50
/* Installed cluster course_object_atomic_register_cb_473d50 (tools/cluster_install.py from output/cluster_pilot_kb/course_object_atomic_register_cb_473d50.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0045f5a0*/ extern int explosion_queue();
/*004726a0*/ extern int sfx_play_at();
/*00473ac0*/ extern int __cdecl course_bbox_transform_and_test(struct RwBBox *p0, RwMatrix *p1, RwReal *p2, RwMatrix *p3, struct RwV3d *p4);
/*0047e640*/ extern int __cdecl car_collision_impulse(int p0, struct RwBBox *p1, RwMatrix *p2, float p3, float *p4);
/*004817b0*/ extern int atomic_plugin_get_bbox();
/*004866d0*/ extern int fx_emitter_spawn_b_default();
/*0048e6d0*/ extern int rwp_object_get_handle();
/*0049cb60*/ extern int sphere_vs_box_test(float *collider, float *param_3);
/*004c45e0*/ extern float rp_random_float_range(undefined4 p0, undefined4 p1);
/*005dd32c*/ extern float k_f_3;
/*005ddc28*/ extern float k_f_0;
/*005df528*/ extern float k_f_1_8;
/*0062ac74*/ extern float g_car_62ac74;
/*007ef7f4*/ extern float g_tick_seconds;
#define COLLIDER_TYPE_CAR_FIRST (0xff00) /* collider types 0xff00..0xff07 are cars; type - 0xff00 = car index (inferred from car_collision_impulse arg) */
#define COURSE_OBJECT_TRIGGER_TYPE (29) /* trigger record kind of the course object (see comment in the function) */
/* course_object_atomic_register_cb_473d50 */

/* function pointer pushed/stored by course_object_atomic_register (MOV dword ptr [EAX +
   0x6af674],0x473d50) */

/* Trigger callback of a course object (e.g. a breakable): on a valid hit (bbox/sphere test or car impact) spawns an effect and sound once per cooldown, throws the object's fields +0xdc..0xe8 and queues an explosion (inferred). */
void course_object_atomic_register_cb_473d50(float *hit_pos, char *collider)

{
  float fbits1;
  int obj; /* record the trigger belongs to (pointer stored in a float slot of the trigger) */
  RwReal *collider_bounds;
  RwBBox *obj_bounds;
  int result;
  float10 rand_val;
  RwV3d impulse_out;
  struct {
    float explosion_x;
    float explosion_y;
    float explosion_z;
    float explosion_w;
  } frame;
  
  /* Collision callback for trigger records: hit_pos[4]==29 marks the record kind, hit_pos[5] holds the pointer to the record (obj). Collider types 7/0x12/0x1c are ignored, 10 = bbox test vs another object, 0x15 = sphere-vs-box test (inferred). */
  if (*(int *)&hit_pos[4] != COURSE_OBJECT_TRIGGER_TYPE) {
    return;
  }
  obj = *(int *)&hit_pos[5];
  switch(*(undefined4 *)(collider + 0x10)) {
  case 7:
  case 0x12:
  case 0x1c:
    goto switchD_00473d7e_caseD_7;
  default:
    goto switchD_00473d7e_caseD_8;
  case 10:
    result = rwp_object_get_handle(*(undefined4 *)(collider + 0x18));
    obj_bounds = (RwBBox *)atomic_plugin_get_bbox(*(undefined4 *)(obj + 0x8c));
    collider_bounds = (RwReal *)atomic_plugin_get_bbox(result);
    result = course_bbox_transform_and_test
                      (obj_bounds,(RwMatrix *)(*(int *)(*(char **)(obj + 0x8c) + 4) + 0x10),collider_bounds,
                       (RwMatrix *)(*(char **)(result + 4) + 0x10),&impulse_out);
    break;
  case 0x15:
    result = *(int *)(collider + 0x14) + 0x68;
    result = sphere_vs_box_test((float *)result,hit_pos);
  }
  if (result != 0) {
switchD_00473d7e_caseD_8:
    result = *(int *)(collider + 0x10);
    if ((0xfeff < result) && (result < COLLIDER_TYPE_CAR_FIRST + 8)) {
      obj_bounds = (RwBBox *)atomic_plugin_get_bbox(*(undefined4 *)(obj + 0x8c));
      impulse_out.x = 0.0;
      impulse_out.y = 0.0;
      impulse_out.z = 0.0;
      result = car_collision_impulse
                        (result - COLLIDER_TYPE_CAR_FIRST,obj_bounds,
                         (RwMatrix *)(*(int *)(*(char **)(obj + 0x8c) + 4) + 0x10),1000.0,
                         &impulse_out.x);
      if (result == 0) {
        return;
      }
    }
    rand_val = (float10)rp_random_float_range(0x3f4ccccd,0x3f99999a);
    /* on a hit: spawn the effect at hit_pos, then (once per cooldown, obj+0x94 == 0) play sfx 0x1e, stamp obj+0x94 with g_tick_seconds and randomise velocity-like fields obj+0xdc..0xe8 (inferred), then queue an explosion (variant depends on obj+0xec == 1) */
    fx_emitter_spawn_b_default(hit_pos,&g_car_62ac74,0x41f00000,(fbits1 = (float)rand_val, *(int *)&fbits1));
    if (*(float *)(obj + 0x94) == k_f_0) {
      frame.explosion_x = *hit_pos;
      frame.explosion_y = hit_pos[1];
      frame.explosion_z = hit_pos[2];
      frame.explosion_w = hit_pos[3];
      sfx_play_at(0x1e,hit_pos);
      *(undefined4 *)(obj + 0x94) = *(int *)&g_tick_seconds;
      if (*(int *)(obj + 0xec) == 1) {
        rand_val = (float10)rp_random_float_range(0xc0000000,0x40000000);
        *(float *)(obj + 0xdc) = (float)rand_val;
        rand_val = (float10)rp_random_float_range(0x3fc00000,0x40600000);
        *(float *)(obj + 0xe0) = (float)rand_val;
        rand_val = (float10)rp_random_float_range(0xc0000000,0x40000000);
        *(float *)(obj + 0xe4) = (float)rand_val;
        rand_val = (float10)rp_random_float_range(0x40000000,0x40800000);
        *(float *)(obj + 0xe8) = (float)rand_val;
        frame.explosion_w = frame.explosion_w * k_f_1_8;
        explosion_queue(&frame.explosion_x,0x3f800000,0x42c80000,0x42c80000,2);
        return;
      }
      rand_val = (float10)rp_random_float_range(0xc0400000,0x40400000);
      *(float *)(obj + 0xdc) = (float)rand_val;
      rand_val = (float10)rp_random_float_range(0x40a00000,0x41000000);
      *(float *)(obj + 0xe0) = (float)rand_val;
      rand_val = (float10)rp_random_float_range(0xc0400000,0x40400000);
      *(float *)(obj + 0xe4) = (float)rand_val;
      rand_val = (float10)rp_random_float_range(0x40a00000,0x40e00000);
      *(float *)(obj + 0xe8) = (float)rand_val;
      frame.explosion_w = frame.explosion_w * k_f_3;
      explosion_queue(&frame.explosion_x,0x3f800000,0x44160000,0x43c80000,2);
    }
  }
switchD_00473d7e_caseD_7:
  return;
}

