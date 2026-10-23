// cluster-entries: career_event_won
/* Installed cluster career_event_won (tools/cluster_install.py from output/cluster_pilot_uc3/career_event_won.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0040f7e0*/ extern int intro_banner_get_value(void);
/*0040af20*/ extern void autosave_request(void);
/*0042fb20*/ extern void career_event_won_set_course_state(int state);
extern int g_event_type;
extern int g_career_row;
extern struct GameState g_game_state;

/* Counts the cells equal to 2 (done) in `cells` rows of the progress table (18 words per row) around p: p[-18], p[0], p[18] ...; the block repeats every
   `stride` words until the address reaches `end` (a byte offset from the table start; the last block may run past the 15 rows into the data behind). */
static int count_done(uint *p, int cells, int stride, int end)
{
  int n = 0;
  int j;

  do {
    for (j = -1; j < cells - 1; j++) {
      if (p[j * 0x12] == 2) {
        n++;
      }
    }
    p = p + stride;
  } while ((char *)p < (char *)g_game_state.progress + end);
  return n;
}

#define PROGRESS_AT(bytes) (*(uint *)((char *)g_game_state.progress + (bytes)))

/* A career event was won (the intro banner reports the result): marks its cell of the current career row done (2) and opens the next cells (1).
   Events 0xe-0x10 (finals): the cell below opens, row 4 sets the course state 0x33e-0x340.  Events 3-5 (the three cup columns): the row's cell, its
   neighbours and the follow up cells open, the course state text depends on how many rows have the column done, and after 12 done cells the bonus cells
   (progress 0xed / 0xf0 ...) open.  Finally an autosave is requested. */
void career_event_won(void)
{
  int row_bytes;
  int n;
  int state;
  uint *p;
  int all_done;
  int event;

  if (g_event_type == EVENT_TURBO_TOURNAMENT || g_event_type == EVENT_ICE_TOURNAMENT || g_event_type == EVENT_NIGHT_TOURNAMENT) {
    if (intro_banner_get_value() != 0) {
      event = g_event_type;
      n = g_event_type + g_career_row * 0x12;
      all_done = g_career_row == 4;
      g_game_state.progress[n] = 2;
      g_game_state.progress[n + 0x12] = 1;
      if (all_done) {
        if (event == 0xe) {
          career_event_won_set_course_state(0x33e);
          autosave_request();
          return;
        }
        if (event == 0xf) {
          career_event_won_set_course_state(0x33f);
          autosave_request();
          return;
        }
        if (event == 0x10) {
          career_event_won_set_course_state(0x340);
        }
      }
      autosave_request();
      return;
    }
  } else if (g_event_type == EVENT_CUP_3 || g_event_type == EVENT_CUP_4 || g_event_type == EVENT_CUP_5) {
    if (intro_banner_get_value() != 0) {
      if (g_event_type == EVENT_CUP_3) {
        row_bytes = g_career_row * 0x48;
        if (g_game_state.progress[g_career_row * 0x12 + 3] != 2) {
          state = 0;
          n = count_done(g_game_state.progress + 0x15, 7, 0x7e, 0x444);
          if (n == 0) {
            state = 0x269;
          } else if (n == 1) {
            state = 0x26a;
          } else if (n == 2) {
            state = 0x26c;
          } else if (n == 3) {
            state = 0x26b;
          } else if (n == 0xb) {
            if (g_game_state.progress[0xe] != 2) {
              g_game_state.progress[0xe] = 1;
            }
            state = 0x26d;
          }
          career_event_won_set_course_state(state);
        }
        PROGRESS_AT(row_bytes + 0xc) = 2;
        if (PROGRESS_AT(row_bytes + 0x10) != 2) {
          PROGRESS_AT(row_bytes + 0x10) = 1;
        }
        PROGRESS_AT(row_bytes) = 1;
        PROGRESS_AT(row_bytes + 4) = 1;
        PROGRESS_AT(row_bytes + 0x48) = 1;
        PROGRESS_AT(row_bytes + 0x2c) = 1;
        PROGRESS_AT(row_bytes + 0x30) = 1;
        PROGRESS_AT(row_bytes + 0x74) = 1;
        n = count_done(g_game_state.progress + 0x15, 7, 0x7e, 0x444);
        if (n != 0) {
          p = g_game_state.progress + 0x15;
          do {
            if ((char *)p < (char *)g_game_state.progress + 0x3b4 && *p != 2) {
              *p = 1;
              p[3] = 2;
            }
            p = p + 0x12;
            n = n - 1;
          } while (n != 0);
          autosave_request();
          return;
        }
      } else if (g_event_type == EVENT_CUP_4) {
        row_bytes = g_career_row * 0x48;
        if (g_game_state.progress[g_career_row * 0x12 + 4] != 2) {
          state = 0;
          n = count_done(g_game_state.progress + 0x16, 7, 0x7e, 0x448);
          if (n == 0) {
            state = 0x26e;
          } else if (n == 1) {
            state = 0x270;
          } else if (n == 2) {
            state = 0x26f;
          }
          career_event_won_set_course_state(state);
        }
        PROGRESS_AT(row_bytes + 0x10) = 2;
        if (PROGRESS_AT(row_bytes + 0x14) != 2) {
          PROGRESS_AT(row_bytes + 0x14) = 1;
        }
        PROGRESS_AT(row_bytes + 0x24) = 2;
        PROGRESS_AT(row_bytes + 8) = 1;
        PROGRESS_AT(row_bytes + 0x34) = 1;
        PROGRESS_AT(row_bytes + 0x1c) = 1;
        n = count_done(g_game_state.progress + 0x16, 6, 0x6c, 0x3b8);
        if (n == 0xc && g_game_state.progress[0xed] != 2) {
          g_game_state.progress[0xed] = 1;
          g_game_state.progress[0xf0] = 2;
          if (g_game_state.progress[0xf] != 2) {
            g_game_state.progress[0xf] = 1;
          }
          career_event_won_set_course_state(0x271);
          autosave_request();
          return;
        }
      } else if (g_event_type == EVENT_CUP_5) {
        row_bytes = g_career_row * 0x48;
        if (g_game_state.progress[g_career_row * 0x12 + 5] != 2) {
          state = 0;
          n = count_done(g_game_state.progress + 0x17, 7, 0x7e, 0x44c);
          if (n == 0) {
            state = 0x272;
          } else if (n == 1) {
            state = 0x273;
          }
          g_game_state.progress[g_career_row * 0x12 + 5] = 2;
          all_done = 1;
          p = g_game_state.progress + 4;
          do {
            if (p[-1] != 2) all_done = 0;
            if (p[0] != 2) all_done = 0;
            if (p[1] != 2) all_done = 0;
            if (p[0x11] != 2) all_done = 0;
            if (p[0x12] != 2) all_done = 0;
            if (p[0x13] != 2) all_done = 0;
            if (p[0x23] != 2) all_done = 0;
            if (p[0x24] != 2) all_done = 0;
            if (p[0x25] != 2) all_done = 0;
            if (p[0x35] != 2) all_done = 0;
            if (p[0x36] != 2) all_done = 0;
            if (p[0x37] != 2) all_done = 0;
            if (p[0x47] != 2) all_done = 0;
            if (p[0x48] != 2) all_done = 0;
            if (p[0x49] != 2) all_done = 0;
            if (p[0x59] != 2) all_done = 0;
            if (p[0x5a] != 2) all_done = 0;
            if (p[0x5b] != 2) all_done = 0;
            if (p[0x6b] != 2) all_done = 0;
            if (p[0x6c] != 2) all_done = 0;
            if (p[0x6d] != 2) all_done = 0;
            p = p + 0x7e;
          } while ((char *)p < (char *)g_game_state.progress + 0x400);
          if (all_done) {
            state = 0x275;
            g_game_state.option_flags[0] = 1;
            career_event_won_set_course_state(0x275);
          }
          career_event_won_set_course_state(state);
        }
        PROGRESS_AT(row_bytes + 0x14) = 2;
        PROGRESS_AT(row_bytes + 0x28) = 2;
        PROGRESS_AT(row_bytes + 0x20) = 1;
        n = count_done(g_game_state.progress + 0x17, 6, 0x6c, 0x3bc);
        if (n == 0xc && g_game_state.progress[0xff] != 2) {
          g_game_state.progress[0xff] = 1;
          g_game_state.progress[0x102] = 2;
          if (g_game_state.progress[0x10] != 2) {
            g_game_state.progress[0x10] = 1;
          }
          career_event_won_set_course_state(0x274);
        }
      }
      autosave_request();
      return;
    }
  }
}

