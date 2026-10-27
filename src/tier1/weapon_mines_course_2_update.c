#include "mfl_tier2.h"
/*00458170*/ extern int __cdecl course_point_in_water_box_test(struct RwV3d *p0);
/*006a7d98*/ extern undefined4 g_weapon_mines_6a7d98;
/*006a8b18*/ extern char g_weapon_mines_6a8b18[];


/* weapon_mines_course_2_update */

/* Per-frame mine update: a mine in state 2 or 3 whose position is inside a water box goes to state 4. Other states: unknown. */

void weapon_mines_course_2_update(void)
{
  int in_water;
  int *mine;     /* stride 0x1b ints (108 bytes) per mine; state word at mine[0], position at mine - 8 ints */

  mine = &g_weapon_mines_6a7d98;
  do {
    if ((*mine == 2) || (*mine == 3)) {
      in_water = course_point_in_water_box_test((RwV3d *)(mine + -8));
      if (in_water != 0) {
        *mine = 4;
      }
    }
    mine = mine + 0x1b;
  } while ((int)mine < ((int)&g_weapon_mines_6a8b18));   /* end of the mine table */
  return;
}
