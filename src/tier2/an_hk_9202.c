#include "mfl_tier2.h"
/*005e1e54*/ extern undefined g_dinput_keyboard_5e1e54;
/*005e1ecc*/ extern undefined g_dinput_keyboard_5e1ecc;
/*00770f6c*/ extern undefined4 *g_dinput_keyboard_770f6c;
/*00770f70*/ extern undefined4 *g_app_770f70;
/* dinput_keyboard_create */

/* "IDirectInput::CreateDevice for the keyboard and sets its data format" */

/* COM methods are __stdcall (the callee pops the arguments); the cdecl cast of the decompiler made the caller pop them a second time */
int __cdecl dinput_keyboard_create(undefined4 param_1)

{
  int iVar1;

  iVar1 = (*(int (__stdcall **)(undefined4 *, undefined *, undefined4 **, int))(*g_dinput_keyboard_770f6c + 0xc))
                    (g_dinput_keyboard_770f6c,&g_dinput_keyboard_5e1ecc,&g_app_770f70,0);
  if (iVar1 < 0) {
    return 0;
  }
  else {
    iVar1 = (*(int (__stdcall **)(undefined4 *, undefined *))(*g_app_770f70 + 0x2c))(g_app_770f70,&g_dinput_keyboard_5e1e54);
    if (iVar1 < 0) {
      return 0;
    }
    iVar1 = (*(int (__stdcall **)(undefined4 *, undefined4, int))(*g_app_770f70 + 0x34))(g_app_770f70,param_1,6);
    return iVar1 >= 0;
  }
}
