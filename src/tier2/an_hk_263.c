#include "mfl_tier2.h"
/*0046ef30*/ extern int sound_stub_return_0();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*005b8600*/ extern int FUN_005b8600(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*005b8b30*/ extern void FUN_005b8b30(undefined4 p0, undefined4 p1);
/*00617958*/ extern undefined4 g_soundengine_617958;
/*006ad8b8*/ extern undefined4 g_soundengine_create_6ad8b8;
/*006ad8bc*/ extern undefined4 g_soundengine_create_6ad8bc;
/*006ad8c0*/ extern undefined4 g_soundengine_create_6ad8c0;
/*006ad8c4*/ extern undefined4 g_soundengine_create_6ad8c4;
/*006ad8c8*/ extern undefined4 g_soundengine_create_6ad8c8;
/*006ad8cc*/ extern undefined4 g_soundengine_create_6ad8cc;
/*006ad8d0*/ extern undefined4 g_soundengine_create_6ad8d0;
/*006ad8d4*/ extern undefined4 g_soundengine_create_6ad8d4;
/*006ad8d8*/ extern undefined4 g_soundengine_create_6ad8d8;
/*006add78*/ extern undefined1 g_sound_6add78;
/*006ae07c*/ extern undefined4 g_sound_stream_handles;
/*006ae0bc*/ extern undefined4 g_sound_6ae0bc;
/*006ae110*/ extern undefined4 race_audio_active;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*007f0244*/ extern char g_ai_editor_view_origin_z[];

#define SOUND_STREAM_COUNT (4)
#define TEAM_SOUND_STRIDE (0x80) /* bytes per team sound block in g_sound_6add78 (inferred) */

/* soundengine_create_race_streams */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* sets race_audio_active=1 (trainer uses it as 'game active') */

/*
 * Sets up race audio once per race: creates SOUND_STREAM_COUNT sound streams
 * (handles go into g_sound_stream_handles), then binds each handle to the
 * sound block of a player slot's team (inferred).
 * Does nothing if race_audio_active == 1 or sound_stub_return_0() != 0.
 * Takes no parameters, returns nothing.
 */
void soundengine_create_race_streams(void)

{
  int stream_index;
  undefined4 stream_handle;
  int *slot_team_ptr;
  undefined4 *stream_handle_ptr;

  if (race_audio_active != 1) {
    /* sound_stub_return_0() == 0 means the sound system is ready (inferred).
       stream_index is reused as the loop index below. */
    stream_index = sound_stub_return_0();
    if (stream_index == 0) {
      FID_conflict__wprintf
                (
                "\\mashed_fully_loaded\\toast_us\\TOAST\\CODE\\SRC\\APPCODE\\AUDIO\\soundengine.c: CreateStream\n"
                );
      /* create one stream per handle slot and store each handle */
      stream_index = 0;
      do {
        stream_handle = FUN_005b8600(g_sound_6ae0bc,0,0,0,0);
        (&g_sound_stream_handles)[stream_index] = stream_handle;
        FID_conflict__wprintf("CreateStream %d Done\n",stream_index);
        stream_index = stream_index + 1;
      } while (stream_index < SOUND_STREAM_COUNT);
      FID_conflict__wprintf
                (
                "\\mashed_fully_loaded\\toast_us\\TOAST\\CODE\\SRC\\APPCODE\\AUDIO\\soundengine.c: CreateStream Done\n"
                );
      /* reset the race audio state globals (meanings not known) */
      g_soundengine_create_6ad8bc = 1;
      g_soundengine_create_6ad8c4 = 0;
      g_soundengine_create_6ad8c8 = 0;
      g_soundengine_create_6ad8c0 = 0;
      g_soundengine_create_6ad8cc = 0;
      g_soundengine_create_6ad8d0 = 4;
      g_soundengine_create_6ad8d4 = 2;
      g_soundengine_create_6ad8d8 = 1;
      /* bind each handle to a team sound block, walking g_player_slots
         (one PlayerSlot = 4 ints) until its end */
      stream_handle_ptr = &g_sound_stream_handles;
      slot_team_ptr = &g_player_slots[0].team;
      do {
        g_soundengine_create_6ad8b8 = &g_sound_6add78 + *slot_team_ptr * TEAM_SOUND_STRIDE;
        FUN_005b8b30(*stream_handle_ptr,&g_soundengine_create_6ad8b8);
        slot_team_ptr = slot_team_ptr + 4;
        stream_handle_ptr = stream_handle_ptr + 1;
      } while ((LONG_PTR)slot_team_ptr < ((LONG_PTR)&g_ai_editor_view_origin_z));
      g_soundengine_617958 = 0;
      /* mark race audio as active so this function is not run again */
      race_audio_active = 1;
    }
  }
  return;
}


