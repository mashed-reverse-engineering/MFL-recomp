#include "mfl_tier2.h"
extern char LAB_00494320[];
extern char LAB_00494360[];
/*00494300*/ extern int rwp_world_callback_a();
/*005543d0*/ extern int rw_first_field(int * p0);
/*0056b550*/ extern void FUN_0056b550();
/*0056b590*/ extern void FUN_0056b590();
/*0056e400*/ extern void FUN_0056e400(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8, undefined4 p9);

#define CALLBACK_A_ADDR (0x494300) /* == rwp_world_callback_a */
#define CALLBACK_B_ADDR (0x494320) /* == LAB_00494320 */
/* rwp_world_register_callbacks */

/* "registers the three physics callbacks" */

/*
 * Registers the physics callbacks for `world` (inferred).
 * world: the world object; its first field (rw_first_field) is the target of the registrations (inferred).
 * unused_arg_2..5: never read.
 * Returns nothing.
 */
void __cdecl rwp_world_register_callbacks(undefined4 world,char *unused_arg_2,char *unused_arg_3,char *unused_arg_4,
          char *unused_arg_5)

{
  undefined4 first_field;
  undefined1 *callback_b_data;
  undefined4 first_field_alias_2;
  undefined4 first_field_alias_3;
  code *callback_a;
  undefined4 first_field_alias_4;
  undefined4 world_alias;

  world_alias = world;
  first_field = rw_first_field(world);
  callback_a = rwp_world_callback_a;
  first_field_alias_3 = first_field;
  first_field_alias_4 = first_field;
  /* register callback A (rwp_world_callback_a) on the first field */
  FUN_0056b590(first_field, CALLBACK_A_ADDR, first_field);
  callback_b_data = &LAB_00494320;
  first_field_alias_2 = first_field;
  /* register callback B (LAB_00494320) on the first field */
  FUN_0056b550(first_field, CALLBACK_B_ADDR, first_field);
  /* hand world, the third callback data (LAB_00494360) and the callbacks to FUN_0056e400 (inferred) */
  FUN_0056e400(world,&LAB_00494360,world,first_field,callback_b_data,first_field_alias_2,first_field_alias_3,callback_a,first_field_alias_4,world_alias);
  return;
}



