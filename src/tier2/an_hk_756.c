#include "mfl_tier2.h"
/*0040d130*/ extern int __cdecl find_texture_a(RwChar *p0);
/*0069cbd4*/ extern undefined4 g_event_type;
/*0069d688*/ extern undefined4 g_career_row;
#define EVENT_TURBO_TOURNAMENT (0xe)
#define EVENT_ICE_TOURNAMENT (0xf)
#define EVENT_NIGHT_TOURNAMENT (0x10)
#define EVENT_TRAINING (0x11)
#define CUP_INDEX_OFFSET (1000)
/* medal_asset_names */

/*
 * Returns the texture (find_texture_a) for the medal or cup icon of the current event.
 * g_event_type picks the event family. Cup events (3-5) take medal_index 0/1/2 =
 * bronze/silver/gold, plus CUP_INDEX_OFFSET for the cup variant (inferred).
 * Returns 0 if no texture applies.
 * (was: "TurboT NightT TimeTrial BronzeMedal")
 */

undefined4 medal_asset_names(int medal_index)

{
  undefined4 texture;

  switch(g_event_type) {
  case EVENT_TURBO_TOURNAMENT:
    texture = find_texture_a("TurboT");
    return texture;
  case EVENT_ICE_TOURNAMENT:
    texture = find_texture_a("IceT");
    return texture;
  case EVENT_NIGHT_TOURNAMENT:
    texture = find_texture_a("NightT");
    return texture;
  /* time trial events: one fixed icon */
  case 0:
  case 1:
  case 2:
    texture = find_texture_a("TimeTrial");
    return texture;
  /* cup events */
  case 3:
  case 4:
  case 5:
    /* medal icons once g_career_row is past 11 (inferred: medal tiers unlocked) */
    if (0xb < (int)g_career_row) {
      if (medal_index == 0) {
        texture = find_texture_a("BronzeMedal");
        return texture;
      }
      if (medal_index == 1) {
        texture = find_texture_a("SilverMedal");
        return texture;
      }
      if (medal_index == 2) {
        texture = find_texture_a("GoldMedal");
        return texture;
      }
    }
    /* index >= 1000 is the cup variant: strip the offset, then pick the cup icon */
    if (medal_index >= 1000) {
      medal_index = medal_index + -CUP_INDEX_OFFSET;
    }
    if (medal_index == 0) {
      texture = find_texture_a("BronzeCup");
      return texture;
    }
    if (medal_index == 1) {
      texture = find_texture_a("SilverCup");
      return texture;
    }
    if (medal_index == 2) {
      texture = find_texture_a("GoldCup");
      return texture;
    }
    break;
  /* multiplayer events: one fixed icon */
  case 6:
  case 7:
  case 8:
    texture = find_texture_a("MultiPlayer");
    return texture;
  /* quick race events: one fixed icon */
  case 0xb:
  case 0xc:
  case 0xd:
    texture = find_texture_a("QuickRace");
    return texture;
  case EVENT_TRAINING:
    texture = find_texture_a("Training");
    return texture;
  }
  return 0;
}


