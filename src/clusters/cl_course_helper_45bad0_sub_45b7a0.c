// cluster-entries: course_helper_45bad0_sub_45b7a0
/* Installed cluster course_helper_45bad0_sub_45b7a0 (tools/cluster_install.py from output/cluster_pilot_kb/course_helper_45bad0_sub_45b7a0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*005dd33c*/ extern float k_f_4;
/*005dd340*/ extern float k_f_0_5;
/*005dd634*/ extern float k_f_2;
/*005ddacc*/ extern float k_f_360;
/*005ddc28*/ extern float k_f_0;
/*005de1f8*/ extern float k_f_3_1415927;
/*007ef7f4*/ extern float g_tick_seconds;
extern double sin(double);
/* course_helper_45bad0_sub_45b7a0 */

/* single caller course_helper_45bad0 */

/* Animates three child frames (anim+0xc[]): translate by a position row blended with the timer at anim+4 plus a sine bob, then rotate about a (0.5,0.5,0.5) axis; timer counts down by g_tick_seconds (inferred). */
void course_helper_45bad0_sub_45b7a0(int anim)

{
  RwFrame *frame;
  float inv_progress;
  int *frame_slot;
  float *pos_row;
  double half_timer;
  double sine_x87;
  double sin_twice;
  float half;
  int index;
  RwV3d pos;
  RwV3d axis;
  
  if (k_f_0 < *(float *)(anim + 4)) {
    half_timer = *(float *)(anim + 4) * k_f_0_5;      /* x87: stays extended for the sine, a rounded float copy goes to the rotation */
    half = (float)half_timer;
    index = 0;
    axis.x = 0.5;
    sine_x87 = sin(half_timer * k_f_3_1415927);
    sin_twice = sine_x87 + sine_x87; /* spilled as a double */
    axis.y = 0.5;
    axis.z = 0.5;
    pos_row = (float *)(anim + 0x18);
    frame_slot = (int *)(anim + 0xc);
    do {
      inv_progress = k_f_2 - *(float *)(anim + 4);
      frame = *(RwFrame **)(*frame_slot + 4);
      pos.x = inv_progress * *pos_row + pos_row[9];
      pos.z = pos_row[2] * inv_progress + pos_row[0xb];
      pos.y = pos_row[10] + sin_twice;
      RwFrameTranslate(frame,&pos,rwCOMBINEREPLACE);
      RwFrameRotate(frame,&axis,((float)index + k_f_4) * half * k_f_360,
                    rwCOMBINEPRECONCAT);
      index = index + 1;
      frame_slot = frame_slot + 1;
      pos_row = pos_row + 3;
    } while (index < 3);
    *(float *)(anim + 4) = *(float *)(anim + 4) - g_tick_seconds;
  }
  return;
}

