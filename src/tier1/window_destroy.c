#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall DestroyWindow(int);
/*007e7704*/ extern undefined4 g_app_7e7704;
/*007e7728*/ extern undefined4 g_window_destroy_7e7728;
/*00771a48*/ extern undefined4 g_app_771a48;
/*00771a4c*/ extern undefined4 g_app_counters_771a4c;
/*00771a50*/ extern undefined4 g_app_counters_771a50;
/*00771a54*/ extern undefined4 g_app_counters_771a54;
/*00771a58*/ extern undefined4 g_app_counters_771a58;
/*00771a5c*/ extern undefined4 g_app_counters_771a5c;
/*00771a60*/ extern undefined4 g_app_counters_771a60;
/*00771a64*/ extern undefined4 g_app_counters_771a64;
/*00771a68*/ extern undefined4 g_app_counters_771a68;
/*00771a6c*/ extern undefined4 g_app_counters_771a6c;
/*00771a70*/ extern undefined4 g_app_counters_771a70;
/*00771a74*/ extern undefined4 g_app_counters_771a74;
/*00771a78*/ extern undefined4 g_app_counters_771a78;
/*00771a7c*/ extern undefined4 g_app_counters_771a7c;
/*00771a80*/ extern undefined4 g_app_counters_771a80;
/*00771a84*/ extern undefined4 g_app_counters_771a84;
/*007e7704*/ extern undefined4 g_app_7e7704;
/*007e7728*/ extern undefined4 g_window_destroy_7e7728;
/* window_destroy */

/* "DestroyWindow(g_app_7e7704), returns g_app_7e7728" */

undefined4 window_destroy(void)
{
  DestroyWindow(g_app_7e7704);
  return g_window_destroy_7e7728;
}
