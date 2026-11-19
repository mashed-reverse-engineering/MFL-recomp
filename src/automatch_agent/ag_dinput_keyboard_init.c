#include "mfl_tier2.h"
/*004a5470*/ extern int __cdecl dinput_keyboard_create(undefined4 p0);
/*004a4670*/ extern int app_get_dinput_76ed10();
/*00770f6c*/ extern undefined4 g_dinput_keyboard_770f6c;


/* dinput_keyboard_init */

/* "stores the DirectInput interface and creates the keyboard" */

bool dinput_keyboard_init(undefined4 dinput_arg)

{
  int created;
  
  g_dinput_keyboard_770f6c = app_get_dinput_76ed10();
  created = dinput_keyboard_create(dinput_arg);
  return created != 0;
}
