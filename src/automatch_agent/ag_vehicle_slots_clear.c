#include "mfl_tier2.h"
/*008c88a0*/ extern float g_game_8c88a0;
/*008c88a4*/ extern undefined4 g_game_8c88a4;
/*008c88f0*/ extern undefined4 g_game_8c88f0;
/*008c88f4*/ extern undefined4 g_game_8c88f4;
/*008c8940*/ extern undefined4 g_vehicle_slots_8c8940;
/*008c8944*/ extern undefined4 g_game_8c8944;
/*008c8990*/ extern undefined4 g_vehicle_slots_8c8990;
/*008c8994*/ extern undefined4 g_game_8c8994;
/*008c89e0*/ extern undefined4 g_vehicle_slots_8c89e0;
/*008c89e4*/ extern undefined4 g_game_8c89e4;
/*008c8a30*/ extern undefined4 g_vehicle_slots_8c8a30;
/*008c8a34*/ extern undefined4 g_game_8c8a34;
/*008c8a80*/ extern undefined4 g_vehicle_slots_8c8a80;
/*008c8a84*/ extern undefined4 g_game_8c8a84;
/*008c8ad0*/ extern undefined4 g_vehicle_slots_8c8ad0;
/*008c8ad4*/ extern undefined4 g_game_8c8ad4;


/* vehicle_slots_clear */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "clears 8 records of 0x50 bytes at 0x8C88A0" */

void vehicle_slots_clear(void)

{
  g_game_8c88a0 = 0.0f;
  g_game_8c88a4 = 0;
  g_game_8c88f0 = 0;
  g_game_8c88f4 = 0;
  g_vehicle_slots_8c8940 = 0;
  g_game_8c8944 = 0;
  g_vehicle_slots_8c8990 = 0;
  g_game_8c8994 = 0;
  g_vehicle_slots_8c89e0 = 0;
  g_game_8c89e4 = 0;
  g_vehicle_slots_8c8a30 = 0;
  g_game_8c8a34 = 0;
  g_vehicle_slots_8c8a80 = 0;
  g_game_8c8a84 = 0;
  g_vehicle_slots_8c8ad0 = 0;
  g_game_8c8ad4 = 0;
  return;
}
