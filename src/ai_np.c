/* MFL.exe ai_np: functions moved verbatim from game_globals.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#include "mfl_types.h"
#include "mfl_externs.h"

// FUNCTION: MFL 0x0041a020
unsigned char ai_grid_cell_lookup(int x, int y)
{
    int ix = x + 0x200;
    int iy;
    short entry;
    if (ix >= 0 && ix <= 0x400) {
        iy = y + 0x200;
        if (iy >= 0 && iy <= 0x400) {
            entry = g_ai_grid[(ix >> 3) * 0x80 + (iy >> 3)];
            if (entry > 0 && entry < 0x200)
                return g_ai_cells[(x & 7) + ((y & 7) + entry * 8) * 8];
        }
    }
    return 0xff;
}
