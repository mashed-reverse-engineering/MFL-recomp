#include "mfl_tier2.h"
/*005b7330*/ extern int __cdecl FUN_005b7330(int a1, int a2, int a3, int a4);
/*005dda20*/ extern float k_f_4_2949673e9;
/* FUN_005b6a80 */
/*
 * Switches the owner object to a new target (inferred).
 * owner:      object whose target pointer is at +0x50 (char * for layout).
 * new_target: object to switch to, or NULL to just clear the current target.
 * Clears owner->target (+0x50). If new_target is set and owner+0x84 is 0,
 * copies new_target's unsigned value at +0x2c into owner+0x3c as a float.
 * If owner+0x30 is set, notifies that handle through FUN_005b7330.
 */

void FUN_005b6a80(char *owner,char *new_target)

{
  float value_f;

  /* drop the current target */
  if (*(int *)(owner + 0x50) != 0) {
    *(undefined4 *)(owner + 0x50) = 0;
  }
  /* adopt the new target and cache its value (unsigned wrap to float) */
  if ((new_target != 0) && (*(int *)(owner + 0x50) = new_target, *(int *)(owner + 0x84) == 0)) {
    value_f = (float)*(int *)(new_target + 0x2c);
    if (*(int *)(new_target + 0x2c) < 0) {
      value_f = value_f + k_f_4_2949673e9;
    }
    *(float *)(owner + 0x3c) = value_f;
  }
  /* notify the attached handle (inferred) */
  if (*(int *)(owner + 0x30) != 0) {
    FUN_005b7330(*(int *)(owner + 0x30),9,1,*(undefined4 *)(owner + 0x50));
  }
  return;
}


