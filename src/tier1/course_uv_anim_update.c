#include "mfl_tier2.h"
/*00485030*/ extern int frame_follow_path_step(int a, int b, void *params, float t, int d);
/*005dda1c*/ extern float k_f_3_3333333en4;
/*005dda20*/ extern float k_f_4_2949673e9;
/* course_uv_anim_update */
/* "scales the time by 1/3000 and updates the uv animations" */
/* world: course world (texture tables at +0xc110 and +0xc9b0); set: animation set with the
   entry count at +0xd0 and 12 byte entries from +0x70 (two table indices, 4 parameter bytes, one extra dword) */
void course_uv_anim_update(char *world, char *set, unsigned int time)
{
  float t;
  int i;
  char *p;

  t = (float)time * k_f_3_3333333en4;
  i = 0;
  if (0 < *(int *)(set + 0xd0)) {
    p = set + 0x74;
    do {
      frame_follow_path_step(((int *)(world + 0xc110))[p[-4]], ((int *)(world + 0xc9b0))[p[-3]], p, t, *(int *)(p + 4));
      i++;
      p += 0xc;
    } while (i < *(int *)(set + 0xd0));
  }
}
