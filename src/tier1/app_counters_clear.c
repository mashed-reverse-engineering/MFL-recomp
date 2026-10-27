#include "mfl_tier2.h"
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
/* window_destroy */
/* "DestroyWindow(g_app_7e7704), returns g_app_7e7728" */
/* IEEE 754 bit pattern of float 1.0f */
/* app_counters_clear */
/* Zeroes the 12 app counter globals 0x771A4C..0x771A80 (except 0x771A5C and 0x771A70),
   and sets 0x771A48, 0x771A5C, 0x771A70 and 0x771A84 to float 1.0 (inferred: their initial values).
   Returns 1. */
undefined4 app_counters_clear(void)
{
  *(float *)&g_app_counters_771a80 = 0.0f;
  *(float *)&g_app_counters_771a7c = 0.0f;
  *(float *)&g_app_counters_771a78 = 0.0f;
  *(float *)&g_app_counters_771a74 = 0.0f;
  *(float *)&g_app_counters_771a6c = 0.0f;
  *(float *)&g_app_counters_771a68 = 0.0f;
  *(float *)&g_app_counters_771a64 = 0.0f;
  *(float *)&g_app_counters_771a60 = 0.0f;
  *(float *)&g_app_counters_771a58 = 0.0f;
  *(float *)&g_app_counters_771a54 = 0.0f;
  *(float *)&g_app_counters_771a50 = 0.0f;
  *(float *)&g_app_counters_771a4c = 0.0f;
  *(float *)&g_app_counters_771a84 = 1.0f;
  *(float *)&g_app_counters_771a70 = 1.0f;
  *(float *)&g_app_counters_771a5c = 1.0f;
  *(float *)&g_app_771a48 = 1.0f;
  return 1;
}
