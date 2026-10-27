#include "mfl_tier2.h"
typedef int CourseId;
/*00419fa0*/ extern double ai_get_float_8c9980();
/*0042b110*/ extern int current_course_id();
/*0042b200*/ extern char *course_get_68b8b4_ptr();
/*00464aa0*/ extern int powerups_clear();
/*005dd328*/ extern float k_f_0_6;
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd378*/ extern float k_f_0_1;
/*005dd628*/ extern float k_f_0_25;
/*005dd804*/ extern float k_f_5_75;
/*005dd808*/ extern float k_f_8_5;
/*005dd80c*/ extern float k_f_4_95;
/*005dd810*/ extern float k_f_7_7;
/*005dd814*/ extern float k_f_6;
/*005dd818*/ extern float k_f_5_25;
/*005dd81c*/ extern float k_f_4_9;
/*005dd820*/ extern float k_f_2_6;
/*005fb0a8*/ extern float g_bomb_5fb0a8;
/*005fb0ac*/ extern float g_course_5fb0ac;
/*005fb0b0*/ extern undefined4 g_bomb_5fb0b0;
/*005fb0b4*/ extern undefined4 g_course_powerups_5fb0b4;
/*005fb0b8*/ extern undefined4 g_course_5fb0b8;
/*0064d8d8*/ extern float g_course_64d8d8;
/*0064d8dc*/ extern undefined4 g_bomb_64d8dc;
/*0064d8e0*/ extern undefined4 g_bomb_64d8e0;
/*0064d8e4*/ extern undefined4 g_course_64d8e4;
/*0064d8e8*/ extern undefined4 g_viewport_64d8e8;
/*007ef7c8*/ extern undefined4 g_bomb_7ef7c8;
/*007ef7cc*/ extern float g_course_7ef7cc;
/* course_powerups_setup */
#define COURSE_SANDSNOW (13)
#define COURSE_NT1 (17)
#define COURSE_CITY (26)
#define COURSE_NEUSTEIN (39)
/* elements per slot in the g_*_64d8* table (stride, inferred) */
#define SLOT_STRIDE (5)
/* highest slot index the setup loop fills (inferred) */
#define LAST_SLOT (5)

/* "per course id (switch) powerups_clear and the course specific powerup layout" */

/* Sets up the powerup state for the race about to start (inferred).
 * Clears the powerup state with powerups_clear(), then computes the per-course
 * values from the course id and the AI skill factor (ai_get_float_8c9980()):
 * g_course_7ef7cc is the course value, slot_value is the value written to every
 * active slot. Courses not in the switch reset g_bomb_7ef7c8, set the course value
 * to 30.0 and return without building the slot table. (inferred)
 * Slot 0 is always an active entry; slots 1..LAST_SLOT are active only when
 * course_table[slot - 1] (the value read by course_get_68b8b4_ptr()) is > 0.
 * Parameters: none. Returns nothing; results are written to the globals. */

void course_powerups_setup(void)

{
  float powerup_scale;
  int slot_count;
  float slot_value;
  char *course_table;
  CourseId course_id;
  int slot;
  double ai_skill;

  slot = 0;
  course_table = course_get_68b8b4_ptr();
  course_id = current_course_id();
  ai_skill = (double)ai_get_float_8c9980();
  g_course_powerups_5fb0b4 = 0;
  g_course_5fb0b8 = -1;
  powerup_scale = (float)((double)float_1_0 - ai_skill * (double)k_f_0_1);
  powerups_clear();
  /* per-course layout: sets the course value and slot_value (inferred) */
  switch(course_id) {
  case COURSE_SANDSNOW:
    g_course_7ef7cc = powerup_scale + k_f_8_5;
    slot_value = powerup_scale * k_f_0_5 + k_f_5_75;
    break;
  default:
    g_bomb_7ef7c8 = 0;
    g_course_7ef7cc = 30.0;
    return;
  case COURSE_NT1:
    slot_value = powerup_scale * k_f_0_25 + k_f_2_6;
    g_course_7ef7cc = powerup_scale * k_f_0_6 + k_f_4_9;
    break;
  case COURSE_CITY:
    g_course_7ef7cc = powerup_scale * k_f_3 + k_f_7_7;
    slot_value = powerup_scale * k_f_0_5 + k_f_4_95;
    break;
  case COURSE_NEUSTEIN:
    slot_value = powerup_scale * k_f_0_5 + k_f_5_25;
    g_course_7ef7cc = powerup_scale * k_f_3 + k_f_6;
  }
  g_bomb_7ef7c8 = 0;
  /* loop over slots 0..LAST_SLOT: clear each row, then fill it (inferred) */
  do {
    (&g_bomb_64d8dc)[slot * SLOT_STRIDE] = 0xffffffff;
    (&g_bomb_64d8e0)[slot * SLOT_STRIDE] = 0;
    (&g_course_64d8d8)[slot * SLOT_STRIDE] = 0.0;
    (&g_course_64d8e4)[slot * SLOT_STRIDE] = 0;
    (&g_viewport_64d8e8)[slot * SLOT_STRIDE] = 0;
    /* slot 0: always active, fixed values (inferred) */
    if (slot == 0) {
      g_bomb_64d8dc = 0;
      g_bomb_64d8e0 = 0;
      g_course_64d8e4 = 1;
      g_course_64d8d8 = slot_value;
LAB_00405192:
      g_course_powerups_5fb0b4 = g_course_powerups_5fb0b4 + 1;
    }
    else {
      /* slots 1..: active only when the table entry is positive; the entry
         also raises g_course_5fb0b8 (max count) (inferred) */
      slot_count = *(int *)(course_table + -4 + slot * 4);
      if (0 < slot_count) {
        (&g_course_64d8d8)[slot * SLOT_STRIDE] = slot_value;
        (&g_bomb_64d8dc)[slot * SLOT_STRIDE] = slot_count;
        (&g_bomb_64d8e0)[slot * SLOT_STRIDE] = 0;
        (&g_course_64d8e4)[slot * SLOT_STRIDE] = 1;
        if ((int)g_course_5fb0b8 < slot_count) {
          g_course_5fb0b8 = slot_count;
        }
        goto LAB_00405192;
      }
    }
    slot = slot + 1;
    /* past the last slot: copy the course value to g_course_5fb0ac, reset g_bomb_5fb0b0, return (inferred) */
    if (LAST_SLOT < slot) {
      g_bomb_5fb0a8 = 1.0f;
      g_course_5fb0ac = g_course_7ef7cc;
      g_bomb_5fb0b0 = 0xffffffff;
      return;
    }
  } while( true );
}



