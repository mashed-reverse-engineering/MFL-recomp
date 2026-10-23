#include "mfl_tier2.h"
/*0056c950*/ extern int __cdecl FUN_0056c950(int a1, void *a2);
/*005f6720*/ extern undefined g_data_5f6720;
#define OBJ_OFFSET_40 (0x40)
#define OBJ_OFFSET_44 (0x44)
/*
 * Initialises two 32-bit slots of an object (inferred from the body).
 * Calls FUN_0056c950(obj, &g_data_5f6720) first, probably to attach the object to
 * the global g_data_5f6720 (inferred; the callee is not named yet), then stores
 * value_44 at offset 0x44 and value_40 at offset 0x40.
 * Returns obj unchanged, so calls can be chained (inferred).
 */
int FUN_0056fe40(int obj,undefined4 value_44,undefined4 value_40)

{
  FUN_0056c950(obj,&g_data_5f6720);
  *(undefined4 *)(obj + OBJ_OFFSET_44) = value_44;
  *(undefined4 *)(obj + OBJ_OFFSET_40) = value_40;
  return obj;
}


