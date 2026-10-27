#include "mfl_tier2.h"
/*004a5df0*/ extern int object_atomic_create();
/*005543d0*/ extern int rw_first_field(int * p0);
/*0056a210*/ extern void FUN_0056a210(undefined4 p0, void *p1, undefined4 p2, undefined4 p3);
/*0056bf10*/ extern float FUN_0056bf10(void *p0, undefined4 p1, undefined4 p2);
/*0056c080*/ extern void FUN_0056c080(undefined4 p0, undefined4 p1);
/*0056ca70*/ extern void FUN_0056ca70(undefined4 p0, undefined4 p1);
/*0056cac0*/ extern void FUN_0056cac0(undefined4 p0, undefined4 p1);
/*0056cdf0*/ extern void FUN_0056cdf0(undefined4 p0, void *p1);
/*0058c8b0*/ extern void FUN_0058c8b0(undefined4 p0, void *p1);
/*0058c990*/ extern char *FUN_0058c990(void *p0);
/*0058cb90*/ extern void * FUN_0058cb90(void);
/*0058cbe0*/ extern void FUN_0058cbe0(void *p0);
/*005dd378*/ extern float k_f_0_1;
/*006fa18c*/ extern undefined g_rwp_world_6fa18c;
/*006fa4b0*/ extern undefined4 g_rw_6fa4b0;
/*006fa7d0*/ extern undefined4 g_rw_6fa7d0;
/*006fa7d8*/ extern undefined4 g_rw_6fa7d8;
/*006fa858*/ extern undefined4 g_rw_6fa858;
/*006fab78*/ extern undefined4 g_rw_6fab78;
/*006fd0f8*/ extern undefined4 g_rw_6fd0f8;
/*00701c94*/ extern int g_rw_701c94;
/*00888640*/ extern struct RwpObjectDesc g_rwp_objects[32];
/* rwp_objects_create_box_group */

/* "creates n bodies with the box shape of g_rwp_objects[type]" */
/*
 * Creates `count` bodies for the box-shaped RWP object `object_type` (index into g_rwp_objects).
 * Each body takes the next free entry at g_rw_6fa7d0 in the g_rw_6fa4b0 / g_rw_6fd0f8 / g_rw_6fa858 /
 * g_rw_6fab78 tables, and g_rwp_world_6fa18c[entry] gets the squared shatter velocity.
 * `rw_owner` is passed to rw_first_field() (inferred). Parameters 2, 4 and 6..16 are unused here.
 */

void __cdecl rwp_objects_create_box_group(int count,char *unused_2,undefined4 rw_owner,char *unused_4,int object_type,
          char *unused_6,char *unused_7,char *unused_8,char *unused_9,
          char *unused_10,char *unused_11,char *unused_12,char *unused_13,
          char *unused_14,char *unused_15,char *unused_16)

{
  float scaled_property_a;
  int slot;
  int atomic_slot;
  undefined4 *scratch;
  char *obj;
  undefined4 handle;
  int i;
  struct {
    float x;
    float y;
    float z;
  } extents;

  /* scratch record: handle at [0], centre of mass offset at [1..3] (inferred) */
  scratch = (undefined4 *)FUN_0058cb90();
  handle = *scratch;
  *(float *)&scratch[1] = g_rwp_objects[object_type].centre_of_mass_offset[0];
  *(float *)&scratch[2] = g_rwp_objects[object_type].centre_of_mass_offset[1];
  *(float *)&scratch[3] = g_rwp_objects[object_type].centre_of_mass_offset[2];
  /* half extents of the box, scaled by k_f_0_1 (inferred: half of the box dimensions) */
  extents.x = g_rwp_objects[object_type].box_dimensions[0] * k_f_0_1;
  extents.y = g_rwp_objects[object_type].box_dimensions[1] * k_f_0_1;
  extents.z = g_rwp_objects[object_type].box_dimensions[2] * k_f_0_1;
  FUN_0056cdf0(handle,&extents.x);
  FUN_0056cac0(handle,*(int *)&g_rwp_objects[object_type].property_b);
  FUN_0056ca70(handle,*(int *)&g_rwp_objects[object_type].property_c);
  FUN_0056c080(handle,(scaled_property_a = g_rwp_objects[object_type].property_a * k_f_0_1, *(int *)&scaled_property_a));
  FUN_0056bf10(scratch,*(int *)&g_rwp_objects[object_type].property_d,0);
  i = 0;
  /* one body per iteration */
  if (0 < count) {
    do {
      obj = FUN_0058c990(scratch);
      slot = g_rw_6fa7d0;
      (&g_rw_6fa4b0)[g_rw_6fa7d0] = g_rwp_objects[object_type].base_label + i;
      atomic_slot = g_rw_701c94;
      (&g_rw_6fd0f8)[slot] = object_type;
      (&g_rw_6fa858)[slot] = 1;
      handle = object_atomic_create((&g_rw_6fa7d8)[atomic_slot],i);
      (&g_rw_6fab78)[g_rw_6fa7d0] = handle;
      FUN_0058c8b0(handle,obj);
      /* clear bits 0x0c of the flags word at +8 (inferred) */
      *(uint *)(obj + 8) = *(uint *)(obj + 8) & 0xfffffff3;
      handle = rw_first_field(rw_owner);
      FUN_0056a210(handle, obj, 0xffffffff, 2);
      g_rw_6fa7d0 = g_rw_6fa7d0 + 1;
      /* shatter velocity squared, stored at the entry just claimed */
      ((float *)&g_rwp_world_6fa18c)[g_rw_6fa7d0] =
           g_rwp_objects[object_type].shatter_velocity * g_rwp_objects[object_type].shatter_velocity;
      i = i + 1;
    } while (i < count);
  }
  FUN_0058cbe0(scratch);
  return;
}


