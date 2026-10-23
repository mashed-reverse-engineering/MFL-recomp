#include "mfl_tier2.h"
/*0040af20*/ extern int autosave_request();
/*00413880*/ extern int current_course_id_with_mirror();
/*00435fb0*/ extern int ui_get_menu_69d2d0();
/*004957f0*/ extern int replay_record_save_frame();
/*004a8420*/ extern int __cdecl debug_printf(const char *fmt, ...);
typedef int (__cdecl *rwSprintfFn)(RwChar *, const RwChar *, ...);
#define rwsprintf RW_GLOBAL(0xc4, rwSprintfFn) /* stringFuncs.vecSprintf */
/*006037c8*/ extern undefined4 g_replay_6037c8;
/*00652f88*/ extern undefined4 g_replay_652f88;
/*007d2128*/ extern char *RwEngineInstance;
/*008d8b08*/ extern undefined4 g_replay_buffer;
#define REPLAY_ALL_FILE_ID (0x10b) /* "%sAll%d.rep" is written with this fixed number instead of a course id */
/* compile: /GS */

/*
 * replay_save (body at 00413d10, the entry at 0040f800 is a jmp thunk to it): once per race, writes the replay
 * recording as "<toast dir>Replay<course>.rep" and the all-courses replay file, then asks for an autosave.
 * Does nothing when the recording does not exist or the menu is active; either way it only runs once.
 */
/* TIER2: unvalidated 2026-10-23 authority=diffemu */
void replay_save(void)
{
  char replay_name [64];

  if (g_replay_6037c8 == 0) {
    if (g_replay_652f88 != 0) {
      if (ui_get_menu_69d2d0() == 0) {
        rwsprintf(replay_name, "%sReplay%d.rep", "U:\\toast\\", current_course_id_with_mirror());
        debug_printf("saving replay %s\n", replay_name);
        replay_record_save_frame(g_replay_652f88, &g_replay_buffer);
        rwsprintf(replay_name, "%sAll%d.rep", "U:\\toast\\", REPLAY_ALL_FILE_ID);
        autosave_request();
      }
    }
    g_replay_6037c8 = 1;
  }
}
