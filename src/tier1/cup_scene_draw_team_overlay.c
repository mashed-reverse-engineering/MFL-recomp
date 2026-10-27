#include "mfl_tier2.h"
/*0040fe50*/ extern int team_color_rgba();
/*004220f0*/ extern int vehicle_get_position_158();
/*00434bb0*/ extern int get_event_type();
/*0047ffe0*/ extern int hud_label_add_from_bytes();
/*005dd344*/ extern undefined g_cup_scene_5dd344;
/*007ef7b8*/ extern undefined4 game_mode;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
#define NO_SLOT (-1)                /* slot_index value for "no player in this slot" (inferred) */
#define COLOUR_CHANNEL_MAX (0xff)   /* full intensity of one RGBA byte */
#define LABEL_POS_X (0x43a00000)    /* float 320.0 as bits: screen_pos[0] (inferred: screen centre at 640x480) */
#define LABEL_POS_Y (0x43700000)    /* float 240.0 as bits: screen_pos[1] */
#define LABEL_FLOAT_3_0 (0x40400000) /* float 3.0 as bits: last-but-one argument of hud_label_add_from_bytes */
#define LABEL_FLOAT_2_0 (0x40000000) /* float 2.0 as bits: last argument of hud_label_add_from_bytes */
/* cup_scene_draw_team_overlay */
/* "inferred: colours per event type, team overlay of the cup screen" */
/*
 * Draws the team-coloured label over the player in slot_index on the cup screen (inferred).
 * Parameters: unused_arg is never read; slot_index indexes g_player_slots, NO_SLOT = nothing to draw.
 * Returns nothing. Draws only when the event type is not 0, 1 or 2 (inferred) and game_mode is 0.
 */
void __cdecl cup_scene_draw_team_overlay(undefined4 unused_arg,int slot_index)
{
  int event_type;
  undefined1 team_rgba [4];      /* RGBA colour, RGBA colour, 2 floats: each is read through a pointer as a whole */
  undefined1 white_rgba [4];
  undefined4 screen_pos [2];
  undefined1 pos_radius [16];

  /* defaults: white label colour, opaque red team colour (replaced by team_color_rgba), label position */
  white_rgba[0] = COLOUR_CHANNEL_MAX;
  white_rgba[1] = COLOUR_CHANNEL_MAX;
  white_rgba[2] = COLOUR_CHANNEL_MAX;
  white_rgba[3] = COLOUR_CHANNEL_MAX;
  team_rgba[0] = COLOUR_CHANNEL_MAX;
  team_rgba[1] = 0;
  team_rgba[2] = 0;
  team_rgba[3] = COLOUR_CHANNEL_MAX;
  screen_pos[0] = LABEL_POS_X;
  screen_pos[1] = LABEL_POS_Y;
  /* nested checks: slot must be real; event type must be none of 0, 1, 2 (meaning unknown); game_mode must be 0 */
  if (slot_index != NO_SLOT) {
    event_type = get_event_type();
    if (event_type != EVENT_TIME_TRIAL_0) {
      event_type = get_event_type();
      if (event_type != EVENT_TIME_TRIAL_1) {
        event_type = get_event_type();
        if ((event_type != EVENT_TIME_TRIAL_2) && (game_mode == GAME_MODE_STANDARD)) {
          /* fetch the slot's position and radius, then its team colour */
          vehicle_get_position_158(slot_index,pos_radius);
          team_color_rgba(g_player_slots[slot_index].team,team_rgba);
          /* the label is drawn three times with identical arguments (kept as in the original) */
          hud_label_add_from_bytes
                    (&g_cup_scene_5dd344,screen_pos,white_rgba,team_rgba,LABEL_FLOAT_3_0,LABEL_FLOAT_2_0);
          hud_label_add_from_bytes
                    (&g_cup_scene_5dd344,screen_pos,white_rgba,team_rgba,LABEL_FLOAT_3_0,LABEL_FLOAT_2_0);
          hud_label_add_from_bytes
                    (&g_cup_scene_5dd344,screen_pos,white_rgba,team_rgba,LABEL_FLOAT_3_0,LABEL_FLOAT_2_0);
        }
      }
    }
  }
  return;
}
