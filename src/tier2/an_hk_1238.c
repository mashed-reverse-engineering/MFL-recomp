#include "mfl_tier2.h"
/*005bf2b0*/ extern int __cdecl FUN_005bf2b0(int a1);

/* sound object: flag word at +0x28, converted value at +0xcc */
typedef struct SoundObject {
  char unknown_00[0x28];
  uint flags;
  char unknown_2c[0xa0];
  undefined4 converted_value;
} SoundObject;

/* gap_5caa20 */

/* no context */

SoundObject *gap_5caa20(SoundObject *param_1,char *unused_2,undefined4 *param_3)

{
  undefined4 uVar1;

  uVar1 = FUN_005bf2b0(*param_3);
  param_1->converted_value = uVar1;
  param_1->flags = param_1->flags | 4;
  return param_1;
}
