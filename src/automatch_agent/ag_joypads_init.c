#include "mfl_tier2.h"
/*004a5190*/ extern int joypads_search();
/*0076ed18*/ extern undefined4 g_app_76ed18;
/*00770f60*/ extern float g_app_770f60;
/*00770f68*/ extern undefined4 g_app_770f68;
undefined4 joypads_init(undefined4 param_1)

{
  int iVar1;

  app_get_dinput_76ed10();
  g_app_770f68 = param_1;
  iVar1 = joypads_search();
  if (iVar1 == 0) {
    return 0;
  }
  g_app_76ed18 = 0x10;
  g_app_770f60 = 0.35f;
  return 1;
}
