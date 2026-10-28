#include "mfl_tier2.h"
/*004a5df0*/ extern int object_atomic_create();
/*005543d0*/ extern int rw_first_field(int * p0);
/*0056a210*/ extern void rwp_world_activate_object(undefined4 p0, void *p1, undefined4 p2, undefined4 p3);
/*0056bf10*/ extern float FUN_0056bf10(void *p0, undefined4 p1, undefined4 p2);
/*0056ca70*/ extern void rwp_shape_set_property_c(undefined4 p0, undefined4 p1);
/*0056cac0*/ extern void rwp_shape_set_property_b(undefined4 p0, undefined4 p1);
/*0056fe40*/ extern void FUN_0056fe40(undefined4 p0, undefined4 p1, undefined4 p2);
/*0058c8b0*/ extern void FUN_0058c8b0(undefined4 p0, void *p1);
/*0058c990*/ extern char *FUN_0058c990(void *p0);
/*0058cb90*/ extern void * rwp_body_create(void);
/*0058cbe0*/ extern void rwp_body_destroy(void *p0);
/*005dd378*/ extern float k_f_0_1;
/*006fa18c*/ extern char *g_rwp_world_6fa18c;
/*006fa4b0*/ extern undefined4 g_rw_6fa4b0;
/*006fa7d0*/ extern undefined4 g_rw_6fa7d0;
/*006fa7d8*/ extern undefined4 g_rw_6fa7d8;
/*006fab78*/ extern undefined4 g_rw_6fab78;
/*006fd0f8*/ extern undefined4 g_rw_6fd0f8;
/*00701c94*/ extern int g_rw_701c94;
/*00888640*/ extern struct RwpObjectDesc g_rwp_objects[32];
/* rwp_objects_create_cylinder_group */

/* "creates n bodies with the cylinder shape of g_rwp_objects[type]" */
/*
 * Creates `count` bodies of the cylinder shape described by g_rwp_objects[type].
 * Sets up the body's centre of mass, cylinder radius/height and properties, then
 * creates each object and records it in the g_rw_* object tables.
 * type: index into g_rwp_objects (RwpObjectDesc).
 * count: number of objects to create.
 * object_list: passed to rw_first_field (inferred).
 */

void __cdecl rwp_objects_create_cylinder_group(int count,char *unused_2,undefined4 object_list,char *unused_4,int type,
          char *unused_6,char *unused_7,char *unused_8,char *unused_9,
          char *unused_10,char *unused_11,char *unused_12,char *unused_13,
          char *unused_14,char *unused_15,char *unused_16)

{
  float radius_bits;
  float height_bits;
  int slot;
  undefined4 *body;
  char *entry;
  undefined4 handle;
  int group_index;

  /* body: centre of mass, cylinder radius/height (inferred), property values */
  body = (undefined4 *)rwp_body_create();
  handle = *body;
  *(float *)&body[1] = g_rwp_objects[type].centre_of_mass_offset[0];
  *(float *)&body[2] = g_rwp_objects[type].centre_of_mass_offset[1];
  *(float *)&body[3] = g_rwp_objects[type].centre_of_mass_offset[2];
  FUN_0056fe40(handle,(radius_bits = g_rwp_objects[type].cylinder[0] * k_f_0_1, *(int *)&radius_bits),
               (height_bits = g_rwp_objects[type].cylinder[1] * k_f_0_1, *(int *)&height_bits));
  rwp_shape_set_property_b(handle,*(int *)&g_rwp_objects[type].property_b);
  rwp_shape_set_property_c(handle,*(int *)&g_rwp_objects[type].property_c);
  FUN_0056bf10(body,*(int *)&g_rwp_objects[type].property_d,0);
  group_index = 0;
  if (0 < count) {
    /* one iteration per object: create it, register it in the g_rw_* tables,
       clear flag bits 2-3 of its entry (inferred), then store its shatter velocity squared */
    do {
      entry = FUN_0058c990(body);
      handle = object_atomic_create((&g_rw_6fa7d8)[g_rw_701c94],group_index);
      slot = g_rw_6fa7d0;
      (&g_rw_6fa4b0)[g_rw_6fa7d0] = g_rwp_objects[type].base_label + group_index;
      (&g_rw_6fab78)[slot] = handle;
      (&g_rw_6fd0f8)[slot] = type;
      FUN_0058c8b0(handle,entry);
      *(uint *)(entry + 8) = *(uint *)(entry + 8) & 0xfffffff3;
      handle = rw_first_field(object_list);
      rwp_world_activate_object(handle, entry, 0xffffffff, 2);
      g_rw_6fa7d0 = g_rw_6fa7d0 + 1;
      ((float *)&g_rwp_world_6fa18c)[g_rw_6fa7d0] =
           g_rwp_objects[type].shatter_velocity * g_rwp_objects[type].shatter_velocity;
      group_index = group_index + 1;
    } while (group_index < count);
  }
  rwp_body_destroy(body);
  return;
}


