#include "mfl_tier2.h"
/*00611418*/ extern undefined g_sound_frame_611418;
/*006124f0*/ extern undefined g_sound_frame_6124f0;
/*006135c8*/ extern undefined g_sound_frame_6135c8;
/*006146a0*/ extern undefined g_sound_frame_6146a0;
/*00615778*/ extern undefined g_sound_frame_615778;
/*00616850*/ extern undefined g_sound_frame_616850;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008be7c0*/ extern undefined4 g_sound_stream_slots;


/* voice_queue_assign_slot */

/* "(player, line): points g_sound_8be7c0[player] at the record of the line, table chosen by the
   player's team" */

void voice_queue_assign_slot(int player,int line)

{
  switch(g_player_slots[player].team) {
  case 0:
    (&g_sound_stream_slots)[player] = &g_sound_frame_611418 + line * 0x2c;
    return;
  case 1:
    (&g_sound_stream_slots)[player] = &g_sound_frame_6124f0 + line * 0x2c;
    return;
  case 2:
    (&g_sound_stream_slots)[player] = &g_sound_frame_6135c8 + line * 0x2c;
    return;
  case 3:
    (&g_sound_stream_slots)[player] = &g_sound_frame_6146a0 + line * 0x2c;
    return;
  case 4:
    (&g_sound_stream_slots)[player] = &g_sound_frame_615778 + line * 0x2c;
    return;
  case 5:
    (&g_sound_stream_slots)[player] = &g_sound_frame_616850 + line * 0x2c;
    return;
  default:
    (&g_sound_stream_slots)[player] = 0;
    return;
  }
}
