#include "mfl_tier2.h"
/*0045d590*/ extern int course_zone_contains_matrix_in_zone_6a2888();
/*006a5e2c*/ extern undefined4 g_weapon_6a5e2c;
/*006a6048*/ extern undefined g_weapon_rocket_6a6048;
#define ROCKET_RECORD_DWORDS (0x1b)     /* rocket record stride: 0x6C bytes (inferred from the loop step) */
/* weapon_rocket_course_33_update */

/* Refreshes the stored position of every rocket record (no parameters, returns nothing).
 * A record's object pointer at +0x0 leads to an RwFrame (its parent at +0x4). The frame's
 * modelling matrix (+0x10) is kept inside the course zone by the callee (inferred from its
 * name; the matrix is passed as both input and output), then the matrix position
 * (+0x40..+0x48) is copied into the record's three stored position dwords (inferred).
 * Only records whose in-use flag is set are updated (inferred). */
/* TIER2: validated 2026-10-18 authority=diffemu */
typedef struct { int a, b, c; } V3y;
void weapon_rocket_course_33_update(void)
{
  char *frame;
  int *p;
  int active;

  p = (int *)&g_weapon_rocket_6a6048;
  do {
    active = p[-0x1e];
    p = p + -ROCKET_RECORD_DWORDS;
    if (active != 0) {
      frame = *(char **)(*p + 4) + 0x10;
      course_zone_contains_matrix_in_zone_6a2888(frame,frame);
      *(V3y *)(p - 0x14) = *(V3y *)(frame + 0x30);
    }
  } while (p != &g_weapon_6a5e2c);
  return;
}
