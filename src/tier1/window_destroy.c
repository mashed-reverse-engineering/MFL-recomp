#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall DestroyWindow(int);
/*007e7704*/ extern undefined4 g_app_7e7704;
/*007e7728*/ extern undefined4 g_window_destroy_7e7728;
/*007e7704*/ extern undefined4 g_app_7e7704;
/*007e7728*/ extern undefined4 g_window_destroy_7e7728;
/* window_destroy */

/* "DestroyWindow(g_app_7e7704), returns g_app_7e7728" */

undefined4 window_destroy(void)
{
  DestroyWindow(g_app_7e7704);
  return g_window_destroy_7e7728;
}
