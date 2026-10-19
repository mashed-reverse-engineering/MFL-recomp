#include "mfl_tier2.h"
/*007eebc0*/ extern struct GameState g_game_state;
/*007ef4e4*/ extern char g_best_fraction[];

#define ENTRIES_PER_GROUP (3)
#define BEST_TABLE_ENTRIES (45)
#define DEFAULT_BEST_MINUTES (9)
#define DEFAULT_BEST_SECONDS (0x3b)
#define DEFAULT_BEST_FRACTION_BITS (0x3f7d70a4) /* 0.99f */
/* best_times_init_defaults */

/* Resets the best-time tables (best_minutes, best_seconds, best_fraction; BEST_TABLE_ENTRIES entries each)
   to their defaults. Takes no parameters and returns nothing.
   Every entry is first set to 9:59.99, then the entries with known times are overwritten below. */

void best_times_init_defaults(void)

{
  uint *entry_ptr;
  int entries_left;

  /* default fill: walks best_seconds one group of ENTRIES_PER_GROUP at a time until it reaches best_fraction;
     the minutes and fraction entries are reached with the table-length offsets */
  entry_ptr = g_game_state.best_seconds;
  do {
    entries_left = ENTRIES_PER_GROUP;
    do {
      entry_ptr[-BEST_TABLE_ENTRIES] = DEFAULT_BEST_MINUTES;
      *entry_ptr = DEFAULT_BEST_SECONDS;
      entry_ptr[BEST_TABLE_ENTRIES] = DEFAULT_BEST_FRACTION_BITS;
      entry_ptr = entry_ptr + 1;
      entries_left = entries_left + -1;
    } while (entries_left != 0);
  } while ((LONG_PTR)entry_ptr < ((LONG_PTR)&g_best_fraction));

  /* known best times (inferred: one entry per course/mode, index = course * 3 + mode) */
  g_game_state.best_seconds[1] = 0x11;
  g_game_state.best_seconds[2] = 0x11;
  g_game_state.best_seconds[7] = 0x28;
  g_game_state.best_seconds[8] = 0x28;
  g_game_state.best_seconds[0x15] = 0x28;
  g_game_state.best_minutes[0] = 0;
  g_game_state.best_seconds[0] = 0x12;
  g_game_state.best_fraction[0] = 0.26;
  g_game_state.best_minutes[1] = 0;
  g_game_state.best_fraction[1] = 0.64;
  g_game_state.best_minutes[2] = 0;
  g_game_state.best_fraction[2] = 0.1;
  g_game_state.best_minutes[3] = 0;
  g_game_state.best_seconds[3] = 0x1a;
  g_game_state.best_fraction[3] = 0.58;
  g_game_state.best_minutes[4] = 0;
  g_game_state.best_seconds[4] = 0x1a;
  g_game_state.best_fraction[4] = 0.46;
  g_game_state.best_minutes[5] = 0;
  g_game_state.best_seconds[5] = 0x18;
  g_game_state.best_fraction[5] = 0.03;
  g_game_state.best_minutes[6] = 0;
  g_game_state.best_seconds[6] = 0x2a;
  g_game_state.best_fraction[6] = 0.18;
  g_game_state.best_minutes[7] = 0;
  g_game_state.best_fraction[7] = 0.98;
  g_game_state.best_minutes[8] = 0;
  g_game_state.best_fraction[8] = 0.51;
  g_game_state.best_minutes[9] = 0;
  g_game_state.best_seconds[9] = 0x1b;
  g_game_state.best_fraction[9] = 0.79;
  g_game_state.best_minutes[10] = 0;
  g_game_state.best_seconds[10] = 0x19;
  g_game_state.best_fraction[10] = 0.86;
  g_game_state.best_minutes[0xb] = 0;
  g_game_state.best_seconds[0xb] = 0x19;
  g_game_state.best_fraction[0xb] = 0.18;
  g_game_state.best_minutes[0xc] = 0;
  g_game_state.best_seconds[0xc] = 0x38;
  g_game_state.best_fraction[0xc] = 0.59;
  g_game_state.best_minutes[0xd] = 0;
  g_game_state.best_seconds[0xd] = 0x34;
  g_game_state.best_fraction[0xd] = 0.51;
  g_game_state.best_minutes[0xe] = 0;
  g_game_state.best_seconds[0xe] = 0x33;
  g_game_state.best_fraction[0xe] = 0.56;
  g_game_state.best_minutes[0xf] = 0;
  g_game_state.best_seconds[0xf] = 0x1a;
  g_game_state.best_fraction[0xf] = 0.58;
  g_game_state.best_minutes[0x10] = 0;
  g_game_state.best_seconds[0x10] = 0x19;
  g_game_state.best_fraction[0x10] = 0.11;
  g_game_state.best_minutes[0x11] = 0;
  g_game_state.best_seconds[0x11] = 0x19;
  g_game_state.best_fraction[0x11] = 0.6;
  g_game_state.best_minutes[0x12] = 0;
  g_game_state.best_seconds[0x12] = 0x2f;
  g_game_state.best_fraction[0x12] = 0.58;
  g_game_state.best_minutes[0x13] = 0;
  g_game_state.best_seconds[0x13] = 0x2d;
  g_game_state.best_fraction[0x13] = 0.21;
  g_game_state.best_minutes[0x14] = 0;
  g_game_state.best_seconds[0x14] = 0x2b;
  g_game_state.best_fraction[0x14] = 0.04;
  g_game_state.best_minutes[0x15] = 0;
  g_game_state.best_fraction[0x15] = 0.14;
  g_game_state.best_minutes[0x16] = 0;
  g_game_state.best_seconds[0x16] = 0x26;
  g_game_state.best_fraction[0x16] = 0.96;
  g_game_state.best_minutes[0x17] = 0;
  g_game_state.best_seconds[0x17] = 0x25;
  g_game_state.best_fraction[0x17] = 0.03;
  g_game_state.best_minutes[0x18] = 0;
  g_game_state.best_seconds[0x18] = 0x22;
  g_game_state.best_fraction[0x18] = 0.51;
  g_game_state.best_seconds[0x23] = 0x3b;
  g_game_state.best_seconds[0x26] = 0x3b;
  g_game_state.best_seconds[0x28] = 0x3b;
  g_game_state.best_seconds[0x29] = 0x3b;
  g_game_state.best_seconds[0x2c] = 0x3b;
  g_game_state.best_seconds[0x1a] = 0x1a;
  g_game_state.best_seconds[0x1d] = 0x20;
  g_game_state.best_seconds[0x20] = 0x20;
  g_game_state.best_seconds[0x21] = 0x20;
  g_game_state.best_seconds[0x1f] = 0x25;
  g_game_state.best_minutes[0x19] = 0;
  g_game_state.best_seconds[0x19] = 0x21;
  g_game_state.best_fraction[0x19] = 0.32;
  g_game_state.best_minutes[0x1a] = 0;
  g_game_state.best_fraction[0x1a] = 0.58;
  g_game_state.best_minutes[0x1b] = 0;
  g_game_state.best_seconds[0x1b] = 0x1d;
  g_game_state.best_fraction[0x1b] = 0.25;
  g_game_state.best_minutes[0x1c] = 0;
  g_game_state.best_seconds[0x1c] = 0x1b;
  g_game_state.best_fraction[0x1c] = 0.75;
  g_game_state.best_minutes[0x1d] = 0;
  g_game_state.best_fraction[0x1d] = 0.86;
  g_game_state.best_minutes[0x1e] = 0;
  g_game_state.best_seconds[0x1e] = 0x2a;
  g_game_state.best_fraction[0x1e] = 0.46;
  g_game_state.best_minutes[0x1f] = 0;
  g_game_state.best_fraction[0x1f] = 0.86;
  g_game_state.best_minutes[0x20] = 0;
  g_game_state.best_fraction[0x20] = 0.89;
  g_game_state.best_minutes[0x21] = 0;
  g_game_state.best_fraction[0x21] = 0.3;
  g_game_state.best_minutes[0x22] = 0;
  g_game_state.best_seconds[0x22] = 0x1c;
  g_game_state.best_fraction[0x22] = 0.96;
  g_game_state.best_minutes[0x23] = 9;
  g_game_state.best_fraction[0x23] = 0.99;
  g_game_state.best_minutes[0x24] = 0;
  g_game_state.best_seconds[0x24] = 0x34;
  g_game_state.best_fraction[0x24] = 0.28;
  g_game_state.best_minutes[0x25] = 0;
  g_game_state.best_seconds[0x25] = 0x32;
  g_game_state.best_fraction[0x25] = 0.06;
  g_game_state.best_minutes[0x26] = 9;
  g_game_state.best_fraction[0x26] = 0.99;
  g_game_state.best_minutes[0x27] = 1;
  g_game_state.best_seconds[0x27] = 9;
  g_game_state.best_fraction[0x27] = 0.94;
  g_game_state.best_minutes[0x28] = 0;
  g_game_state.best_fraction[0x28] = 0.36;
  g_game_state.best_minutes[0x29] = 9;
  g_game_state.best_fraction[0x29] = 0.99;
  g_game_state.best_minutes[0x2a] = 0;
  g_game_state.best_seconds[0x2a] = 0x29;
  g_game_state.best_fraction[0x2a] = 0.53;
  g_game_state.best_minutes[0x2b] = 0;
  g_game_state.best_seconds[0x2b] = 0x29;
  g_game_state.best_fraction[0x2b] = 0.45;
  g_game_state.best_minutes[0x2c] = 9;
  g_game_state.best_fraction[0x2c] = 0.99;
  return;
}


