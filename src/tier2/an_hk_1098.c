#include "mfl_tier2.h"
/*0049a960*/ extern int RpPTankAtomicLock_00545d10();
/*0049a970*/ extern int RpPTankAtomicUnlock_00545f20();
/*005dd330*/ extern float float_1_0;
/*005ddb00*/ extern float k_f_0_025;
/*005e1a5c*/ extern float k_f_0_0028;
/*00771ab0*/ extern undefined4 g_data_771ab0;
/*007da6bc*/ extern char *g_ptank_plugin_offset;
/*007e06e0*/ extern undefined4 g_data_7e06e0;
/*007dc68c*/ extern char g_data_7dc68c[];
/* course_storm_cb1_sub_4aaf40 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* single caller course_storm_cb1 */

void course_storm_cb1_sub_4aaf40(void)

{
  uint *ptank_flags;
  float x_or_scale;
  float *particle;
  int alpha;
  int remaining;
  struct {
    undefined1 *color_cursor;
    int color_stride;
    float *pos_cursor;
    char *pos_stride;
    float *size_cursor;
    char *size_stride;
  } frame;
  
  RpPTankAtomicLock_00545d10(g_data_771ab0,&frame.pos_cursor,1,0x40000000);
  RpPTankAtomicLock_00545d10(g_data_771ab0,&frame.color_cursor,2,0x40000000);
  RpPTankAtomicLock_00545d10(g_data_771ab0,&frame.size_cursor,4,0x40000000);
  g_data_7e06e0 = 0;
  if (((frame.pos_cursor != (float *)0x0) && (frame.color_cursor != (undefined1 *)0x0)) &&
     (frame.size_cursor != (float *)0x0)) {
    particle = (float *)((char *)&g_data_7dc68c + 0x58);
    remaining = 0x200;
    do {
      if (*(short *)(particle + -1) != 0) {
        *(short *)((char *)particle + -2) = *(short *)((char *)particle + -2) + 1;
        x_or_scale = *particle;
        *particle = particle[3] + x_or_scale;
        particle[1] = particle[1] + particle[4];
        particle[2] = particle[5] + particle[2];
        particle[4] = particle[4] - k_f_0_0028;
        if (*(short *)((char *)particle + -2) < 0x40) {
          *frame.pos_cursor = particle[3] + x_or_scale;
          frame.pos_cursor[1] = particle[1];
          frame.pos_cursor[2] = particle[2];
          frame.pos_cursor = (float *)((int)frame.pos_cursor + frame.pos_stride);
          *frame.color_cursor = 0xff;
          frame.color_cursor[1] = 0xff;
          frame.color_cursor[2] = 0xff;
          alpha = (0x40 - *(short *)((char *)particle + -2)) * 4;
          if (alpha < 0) {
            alpha = 0;
          }
          else if (0xff < alpha) {
            alpha = 0xff;
          }
          frame.color_cursor[3] = (char)alpha;
          frame.color_cursor = frame.color_cursor + frame.color_stride;
          x_or_scale = (float)(int)*(short *)((char *)particle + -2) * k_f_0_025 + float_1_0;
          *frame.size_cursor = x_or_scale;
          frame.size_cursor[1] = x_or_scale;
          frame.size_cursor = (float *)((int)frame.size_cursor + frame.size_stride);
          g_data_7e06e0 = g_data_7e06e0 + 1;
        }
        else {
          *(undefined2 *)((char *)particle + -2) = 0;
          *(undefined2 *)(particle + -1) = 0;
        }
      }
      particle = particle + 8;
      remaining = remaining + -1;
    } while (remaining != 0);
    RpPTankAtomicUnlock_00545f20(g_data_771ab0);
  }
  ptank_flags = (uint *)(*(char **)(g_ptank_plugin_offset + g_data_771ab0) + 0x40);
  *ptank_flags = *ptank_flags | 0x800000;
  *(int *)(*(char **)(g_ptank_plugin_offset + g_data_771ab0) + 4) = g_data_7e06e0;
  return;
}


