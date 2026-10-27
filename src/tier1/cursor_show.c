#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall ShowCursor(int);
/*004a4710*/ extern int dinput_release_object_004a4710();
/*004a5160*/ extern int dinput_release_all();
/*004a5520*/ extern int dinput_release_keyboard_004a5520();
/*004a87d0*/ extern int video_dialog_free();
/*004a8860*/ extern int app_get_app_7712f4();
/*0050b5e0*/ extern int nop_stub();


/* cursor_show */

/* "ShowCursor(1) when windowed" */

undefined4 cursor_show(void)
{
  int cursor_enabled;
  
  cursor_enabled = app_get_app_7712f4();
  if (cursor_enabled != 0) {
    ShowCursor(1);
  }
  video_dialog_free();
  nop_stub();
  dinput_release_keyboard_004a5520();
  dinput_release_all();
  dinput_release_object_004a4710();
  return 0;
}
