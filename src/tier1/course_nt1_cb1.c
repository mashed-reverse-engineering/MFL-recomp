#include "mfl_tier2.h"
/*008c5720*/ extern undefined4 g_course_nt1_8c5720;


/* course_nt1_cb1 */

/* Course NT1 callback: looks up entry 2*index of g_course_nt1_8c5720 and applies it to the clump at course+0xc118. */
void course_nt1_cb1(char *course,int sound_index)
{
  undefined4 sound_handle;
  
  sound_handle = func_0x00484b20(g_course_nt1_8c5720,sound_index * 2);
  func_0x00484960(*(undefined4 *)(course + 0xc118),sound_handle);
  func_0x004557f0(course);
  return;
}
