#include "mfl_tier2.h"
/*0040ff20*/ extern int race_setup_get_slot_type();
/*0040ff40*/ extern int race_setup_set_slot_type();
/*0041a490*/ extern int ai_update_speed_scaling();
/*0041b4e0*/ extern undefined __cdecl ai_bot_update(undefined4 p0, undefined4 p1);
/*00434bb0*/ extern int get_event_type();
/*00437f30*/ extern int ui_get_car_69d2d8();
/*0044c540*/ extern int camera_get_car_8c747c();
/*0047a830*/ extern int car_is_active();
/*007ef7b8*/ extern undefined4 game_mode;
/*007f0284*/ extern struct AiData g_ai_data;

#define SLOT_TYPE_AI (2) /* slot type given to a car by race_setup_set_slot_type (inferred) */
#define AI_SLOT_COUNT (4) /* 4 bots / slots, see AiData.bot_state (inferred) */
/* race_mark_ai_slots */

/*
 * Per-frame AI slot pass (inferred).
 * "sets slot type 2 for the active cars 1..3 in the race / battle event types (inferred)"
 * Does nothing unless the AI route table is ready (the count at routes + 512 is > 3).
 * Takes no parameters and returns nothing. `scratch` is reused as a temporary for event
 * types and car flags before the slot loop, then as the slot index inside it.
 * `flag` holds the results of the camera and car checks inside the slot loop.
 */

void race_mark_ai_slots(void)

{
  int scratch;
  int slot_type;
  int flag;

  /* gate: AI route table ready, and not event 0x11 (unless ui_get_car_69d2d8 returns 0) (inferred) */
  if (((int)(*(uint *)((char *)&g_ai_data.routes + 512)) >= 4) &&
     ((scratch = get_event_type(), scratch != 0x11 || (scratch = ui_get_car_69d2d8(), scratch == 0)))) {
    /* race / battle event types, or game modes 4, 8, 9, 10: give the active cars 1..3 the AI slot type (inferred) */
    scratch = get_event_type();
    if ((scratch == 3) ||
       (((((scratch = get_event_type(), scratch == 4 || (scratch = get_event_type(), scratch == 5)) ||
          (scratch = get_event_type(), scratch == 0xb)) ||
         (((scratch = get_event_type(), scratch == 0xc || (scratch = get_event_type(), scratch == 0xd)) ||
          ((game_mode == 4 || ((game_mode == 9 || (game_mode == 8)))))))) || (game_mode == 10)))) {
      scratch = car_is_active(1);
      if (scratch == 1) {
        race_setup_set_slot_type(1,SLOT_TYPE_AI);
      }
      scratch = car_is_active(2);
      if (scratch == 1) {
        race_setup_set_slot_type(2,SLOT_TYPE_AI);
      }
      scratch = car_is_active(3);
      if (scratch == 1) {
        race_setup_set_slot_type(3,SLOT_TYPE_AI);
      }
    }
    ai_update_speed_scaling();
    /* loop over all AI slots: update the bot in each slot that is not type 0 or 1, or whenever the camera car check passes (inferred) */
    scratch = 0;
    do {
      slot_type = race_setup_get_slot_type(scratch);
      if (((slot_type != 0) && (slot_type != 1)) || (flag = camera_get_car_8c747c(), flag == 1)) {
        /* bot update for this slot, if its car is active */
        flag = car_is_active(scratch);
        if (flag == 1) {
          ai_bot_update(scratch,slot_type);
        }
        /* extra update for slot 0 in game mode 7, in event types 3..5 (inferred) */
        flag = get_event_type();
        if ((((flag == 3) || (flag = get_event_type(), flag == 4)) ||
            (flag = get_event_type(), flag == 5)) && (game_mode == 7)) {
          ai_bot_update(0,slot_type);
        }
      }
      scratch = scratch + 1;
    } while (scratch < AI_SLOT_COUNT);
  }
  return;
}


