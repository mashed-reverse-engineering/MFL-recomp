#include "mfl_tier2.h"
/*0042f780*/ extern int __cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*004c52f0*/ extern void FUN_004c52f0(undefined4 p0, void *p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*004c5410*/ extern void FUN_004c5410(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*00659a7c*/ extern undefined4 g_load_659a7c;
/*00659a9c*/ extern undefined4 g_load_659a9c;
/*00659aa0*/ extern undefined4 g_hudpanel_659aa0;
/*00659a94*/ extern char g_load_start_659a94[];
/* size in bytes of the zeroed block at g_load_659a7c (inferred) */
#define LOAD_LIGHTS_BLOCK_SIZE (0x30)
/* load_start_lights */
/* startlights.dff */
/*
 * Loads the race start-light model "startlights.dff" (inferred), stores it and
 * its HUD panel pointer (clump + 4, inferred) in globals, zeroes the light state
 * block and initialises each light slot. Takes no parameters, returns nothing.
 */
void load_start_lights(void)
{
  int startlights_clump;
  undefined4 *light_slot;
  char *startlights_name;
  undefined4 clump_arg_b;
  undefined4 clump_arg_c;

  clump_arg_c = 0;
  clump_arg_b = 0;
  startlights_name = "startlights.dff";
  startlights_clump = clump_load_by_name("startlights.dff",clump_arg_b,clump_arg_c);
  mem_zero(&g_load_659a7c,LOAD_LIGHTS_BLOCK_SIZE);
  g_load_659a9c = startlights_clump;
  g_hudpanel_659aa0 = *(undefined4 *)(startlights_clump + 4);
  FUN_004c52f0(startlights_clump,&g_load_659a7c,8,0,0);
  light_slot = &g_load_659a7c;
  /* walk the light slots up to g_load_start_659a94, initialising each (inferred) */
  do {
    FUN_004c5410(*light_slot,0x40,1);
    light_slot = light_slot + 1;
  } while ((LONG_PTR)light_slot < ((LONG_PTR)&g_load_start_659a94));
  return;
}
