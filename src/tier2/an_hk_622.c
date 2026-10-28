#include "mfl_tier2.h"
extern char LAB_00491280[];
/*00490f70*/ extern int rwp_world_create();
/*005543d0*/ extern int rw_first_field(int * p0);
/*005543e0*/ extern int rw_second_field(int * p0);
/*0056c090*/ extern void FUN_0056c090(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8, undefined4 p9, void *p10, undefined4 p11, undefined4 p12);
/*0056ca60*/ extern void FUN_0056ca60();
/*0056ca70*/ extern void rwp_shape_set_property_c(undefined4 p0, undefined4 p1);
/*0056cac0*/ extern void rwp_shape_set_property_b(undefined4 p0, undefined4 p1);
/*0056e3b0*/ extern void FUN_0056e3b0();
/*00571810*/ extern void FUN_00571810(undefined4 p0, void *p1);
/*00571830*/ extern void FUN_00571830(undefined4 p0, undefined4 p1);
/*00571840*/ extern void FUN_00571840(undefined4 p0, undefined4 p1);
/*00573ff0*/ extern int FUN_00573ff0();
/*006fab78*/ extern undefined4 g_rw_6fab78[0x1000];
/*00701c9c*/ extern undefined4 g_rw_701c9c;
/*00888620*/ extern undefined4 g_car_888620;

#define SLOT_CLEAR_COUNT (200)
#define GRAVITY_Y_BITS (0xc11ccccd)

/* rwp_world_create_once */

/* "clears the 200 slot table and creates the physics world with gravity (0,-9.8,0)"
   Only does work the first time: if g_rw_701c9c is still 0, clears the slot table,
   creates the world, sets up its gravity and font spacing, and runs the setup calls on a handle.
   default_value: used in place of value when value is 0 (inferred).
   value: passed to FUN_00573ff0 to get that handle (inferred).
   Returns the world. If the world already exists, returns g_rw_701c9c unchanged. */

int rwp_world_create_once(int default_value,int value)

{
  undefined4 field_value;
  undefined4 handle;
  int world;
  undefined4 *cursor;
  undefined4 one_f;
  undefined4 point_one_f;
  int world_copy;
  undefined4 handle_copy_a;
  undefined4 zero_1;
  undefined4 handle_copy_b;
  undefined1 *label;
  undefined4 scratch;
  undefined4 zero_2;
  struct {
    undefined4 local_c;
    undefined4 local_8;
    undefined4 local_4;
  } gravity;

  gravity.local_c = 0;
  gravity.local_8 = GRAVITY_Y_BITS;
  gravity.local_4 = 0;
  world = g_rw_701c9c;
  if (g_rw_701c9c == 0) {
    /* clear the 200 slot table; world is reused as the loop counter here */
    cursor = g_rw_6fab78;
    for (world = SLOT_CLEAR_COUNT; world != 0; world = world + -1) {
      *cursor = 0;
      cursor = cursor + 1;
    }
    /* create the world; gravity is {0, -9.8, 0}: local_c = x, local_8 = y, local_4 = z */
    world = rwp_world_create(0x3f800000);
    cursor = &gravity.local_c;
    g_rw_701c9c = world;
    field_value = rw_second_field(world);
    FUN_00571810(field_value,cursor);
    /* set the font intergap spacing to 1.0 (inferred) */
    scratch = FLOAT_ONE_BITS;
    field_value = rw_first_field(world);
    Rt2dFontSetIntergapSpacing(field_value,scratch);
    scratch = 0x19;
    field_value = rw_second_field(world);
    FUN_00571830(field_value,scratch);
    field_value = rw_second_field(world);
    FUN_00571840(field_value, 0x19);
    /* no explicit value given: fall back to default_value */
    if (value == 0) {
      value = default_value;
    }
    zero_2 = 0;
    /* get the handle for value, then run the setup calls on it (inferred) */
    handle = FUN_00573ff0(value, 0x0);
    label = &LAB_00491280;
    handle_copy_b = handle;
    FUN_0056ca60(handle, 0x491280);
    zero_1 = 0;
    world_copy = world;
    handle_copy_a = handle;
    FUN_0056e3b0(world, handle, 0x0);
    point_one_f = 0x3dcccccd;
    scratch = handle;
    rwp_shape_set_property_b(handle, 0x3dcccccd);
    one_f = 0x3f800000;
    field_value = handle;
    rwp_shape_set_property_c(handle, 0x3f800000);
    /* final setup call on the handle; most arguments are copies of handle (inferred) */
    FUN_0056c090(handle,1,field_value,one_f,scratch,point_one_f,world_copy,handle_copy_a,zero_1,handle_copy_b,label,value,zero_2);
    g_car_888620 = 0x7b;
  }
  return world;
}



