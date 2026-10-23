#include "mfl_tier2.h"
/*004a5df0*/ extern int object_atomic_create();
/*005543d0*/ extern int rw_first_field(int * p0);
/*0056a210*/ extern void FUN_0056a210(undefined4 p0, void *p1, undefined4 p2, undefined4 p3);
/*0056bf10*/ extern float FUN_0056bf10(void *p0, undefined4 p1, undefined4 p2);
/*0056ca70*/ extern void FUN_0056ca70(undefined4 p0, undefined4 p1);
/*0056cac0*/ extern void FUN_0056cac0(undefined4 p0, undefined4 p1);
/*0056cb10*/ extern void FUN_0056cb10(undefined4 p0, undefined4 p1);
/*0058c8b0*/ extern void FUN_0058c8b0(undefined4 p0, void *p1);
/*0058c990*/ extern char *FUN_0058c990(void *p0);
/*0058cb90*/ extern void * FUN_0058cb90(void);
/*0058cbe0*/ extern void FUN_0058cbe0(void *p0);
/*005dd378*/ extern float k_f_0_1;
/*006fa18c*/ extern char *g_rwp_world_6fa18c;
/*006fa4b0*/ extern undefined4 g_rw_6fa4b0;
/*006fa7d0*/ extern undefined4 g_rw_6fa7d0;
/*006fa7d8*/ extern undefined4 g_rw_6fa7d8;
/*006fa858*/ extern undefined4 g_rw_6fa858;
/*006fab78*/ extern undefined4 g_rw_6fab78;
/*006fd0f8*/ extern undefined4 g_rw_6fd0f8;
/*00701c94*/ extern int g_rw_701c94;
/*00888640*/ extern struct RwpObjectDesc g_rwp_objects[32];

#define BODY_FLAGS_KEEP_MASK (0xfffffff3)

/* rwp_objects_create_sphere_group */

/* "creates n bodies with the sphere shape of g_rwp_objects[type]"
 *
 * Creates `count` sphere bodies from the g_rwp_objects[object_index] description
 * (centre of mass offset, radius, properties, base label, shatter velocity).
 * Each body is registered in the g_rw_* tables at slot g_rw_6fa7d0, which is
 * incremented once per body.
 *
 * count: number of bodies to create
 * param_2, param_4: unused (inferred)
 * rw_list: passed to rw_first_field (inferred: a list of RW fields)
 * object_index: index into g_rwp_objects (object description)
 */

/* TIER2: unvalidated 2026-10-22 authority=diffemu */
void rwp_objects_create_sphere_group(int count,char *param_2,undefined4 rw_list,char *param_4,int object_index)
{
  float scaled_radius;
  int slot;
  undefined4 *group;
  char *body;
  undefined4 handle;
  int body_index;

  group = (undefined4 *)FUN_0058cb90();
  handle = *group;
  *(float *)&group[1] = g_rwp_objects[object_index].centre_of_mass_offset[0];
  *(float *)&group[2] = g_rwp_objects[object_index].centre_of_mass_offset[1];
  *(float *)&group[3] = g_rwp_objects[object_index].centre_of_mass_offset[2];
  FUN_0056cb10(handle,(scaled_radius = g_rwp_objects[object_index].sphere_radius * k_f_0_1, *(int *)&scaled_radius));
  FUN_0056cac0(handle,*(int *)&g_rwp_objects[object_index].property_b);
  FUN_0056ca70(handle,*(int *)&g_rwp_objects[object_index].property_c);
  FUN_0056bf10(group,*(int *)&g_rwp_objects[object_index].property_d,0);
  body_index = 0;
  /* one iteration per body: create it, register it, clear its flags and
     store its shatter velocity squared */
  if (0 < count) {
    do {
      body = FUN_0058c990(group);
      slot = g_rw_6fa7d0;
      (&g_rw_6fa4b0)[g_rw_6fa7d0] = g_rwp_objects[object_index].base_label + body_index;
      (&g_rw_6fd0f8)[slot] = object_index;
      (&g_rw_6fa858)[slot] = 1;
      handle = object_atomic_create((&g_rw_6fa7d8)[g_rw_701c94],body_index);
      (&g_rw_6fab78)[g_rw_6fa7d0] = handle;
      FUN_0058c8b0(handle,body);
      /* clear the flag bits outside BODY_FLAGS_KEEP_MASK (inferred) */
      *(uint *)(body + 8) = *(uint *)(body + 8) & BODY_FLAGS_KEEP_MASK;
      handle = rw_first_field(rw_list);
      FUN_0056a210(handle, body, 0xffffffff, 2);
      g_rw_6fa7d0 = g_rw_6fa7d0 + 1;
      ((float *)&g_rwp_world_6fa18c)[g_rw_6fa7d0] =
           g_rwp_objects[object_index].shatter_velocity * g_rwp_objects[object_index].shatter_velocity;
      body_index = body_index + 1;
    } while (body_index < count);
  }
  FUN_0058cbe0(group);
  return;
}


