#include "mfl_tier2.h"
/*0076e8dc*/ extern undefined4 g_app_76e8dc;
/*0076e8e0*/ extern undefined4 g_app_76e8e0;
extern undefined4 __cdecl app_fn_4a34f0(void);
void video_get_default_mode(void)

{
  g_app_76e8dc = app_fn_4a34f0();
  g_app_76e8e0 = g_app_76e8dc;
  return;
}
