#include "mfl_tier2.h"
/*004a5df0*/ extern int object_atomic_create();
/*005543d0*/ extern int rw_first_field(int * p0);
/*0056a210*/ extern void FUN_0056a210(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*0056bf10*/ extern float FUN_0056bf10(undefined4 p0, undefined4 p1, undefined4 p2);
/*0056ca70*/ extern void FUN_0056ca70(undefined4 p0, undefined4 p1);
/*0056cac0*/ extern void FUN_0056cac0(undefined4 p0, undefined4 p1);
/*005749c0*/ extern void FUN_005749c0(undefined4 p0, undefined4 p1, undefined4 p2);
/*0058c8b0*/ extern void FUN_0058c8b0(undefined4 p0, undefined4 p1);
/*0058c990*/ extern int FUN_0058c990(undefined4 p0);
/*0058cb90*/ extern void * FUN_0058cb90(void);
/*0058cbe0*/ extern void FUN_0058cbe0(undefined4 p0);
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
#define GEOM_FLAGS_KEEP_MASK (0xfffffff3) /* clears bits 2-3 of the flags word at geom + 8 (inferred) */
/* rwp_objects_create_capsule_group */

/* "creates n bodies with the capsule shape of g_rwp_objects[type]"
 * Sets up a shared body from the object's centre of mass, capsule size and
 * properties, then creates `count` objects that each get a label, a geom and a
 * shatter velocity squared. Params 2, 4 and 6 are unused.
 * count: number of objects to create
 * rw_list: passed to rw_first_field (inferred)
 * object_type: index into g_rwp_objects
 */

void __cdecl rwp_objects_create_capsule_group(int count,char *param_2,undefined4 rw_list,char *param_4,int object_type,
          char *param_6)

{
  float capsule_radius_scaled;
  float capsule_length_scaled;
  int slot;
  undefined4 *body;
  int geom;
  undefined4 handle;
  int i;
  
  body = (undefined4 *)FUN_0058cb90();
  handle = *body;
  *(float *)&body[1] = g_rwp_objects[object_type].centre_of_mass_offset[0];
  *(float *)&body[2] = g_rwp_objects[object_type].centre_of_mass_offset[1];
  *(float *)&body[3] = g_rwp_objects[object_type].centre_of_mass_offset[2];
  FUN_005749c0(handle,(capsule_radius_scaled = g_rwp_objects[object_type].capsule[0] * k_f_0_1, *(int *)&capsule_radius_scaled),
               (capsule_length_scaled = g_rwp_objects[object_type].capsule[1] * k_f_0_1, *(int *)&capsule_length_scaled));
  FUN_0056cac0(handle,*(int *)&g_rwp_objects[object_type].property_b);
  FUN_0056ca70(handle,*(int *)&g_rwp_objects[object_type].property_c);
  FUN_0056bf10(body,*(int *)&g_rwp_objects[object_type].property_d,0);
  i = 0;
  if (0 < count) {
    /* one iteration per object: create it, attach it to the body and register it */
    do {
      geom = FUN_0058c990(body);
      slot = g_rw_6fa7d0;
      (&g_rw_6fa4b0)[g_rw_6fa7d0] = g_rwp_objects[object_type].base_label + i;
      (&g_rw_6fd0f8)[slot] = object_type;
      (&g_rw_6fa858)[slot] = 1;
      handle = object_atomic_create((&g_rw_6fa7d8)[g_rw_701c94],i);
      (&g_rw_6fab78)[g_rw_6fa7d0] = handle;
      FUN_0058c8b0(handle,geom);
      *(uint *)(geom + 8) = *(uint *)(geom + 8) & GEOM_FLAGS_KEEP_MASK;
      handle = rw_first_field(rw_list);
      FUN_0056a210(handle, geom, 0xffffffff, 2);
      g_rw_6fa7d0 = g_rw_6fa7d0 + 1;
      *(float *)((char *)&g_rwp_world_6fa18c + g_rw_6fa7d0 * 4) =
           g_rwp_objects[object_type].shatter_velocity * g_rwp_objects[object_type].shatter_velocity;
      i = i + 1;
    } while (i < count);
  }
  FUN_0058cbe0(body);
  return;
}


