#include "mfl_tier2.h"
/*004757a0*/ extern int get_viewport_camera_matrix();
/*008c5720*/ extern undefined4 g_course_nt1_8c5720;
extern float func_0x004757e0();
extern void __cdecl func_0x00449c40(int m, float f);

/* offset of the per-view slot inside the race object (inferred: passed with the camera matrix to func_0x00484960) */
#define WORLD_RECORD_C118_OFFSET (0xc118) /* inferred: unknown pointer, see an_hk_245/562 */

/*
 * course_nt1_s_cb1 (NT1 special course): per-view camera setup. Returns nothing.
 * race_obj: race/game object pointer (inferred from the field offsets used).
 * view_index: view or viewport index; doubled to form the lookup key (inferred).
 */
void course_nt1_s_cb1(char *race_obj,int view_index)

{
  undefined4 camera_matrix;
  float10 viewport_scale;

  /* per-frame update of the race object (inferred from the callee name) */
  func_0x004557f0(race_obj);
  /* look up the view entry for this index and store it in the race object's view slot */
  camera_matrix = func_0x00484b20(g_course_nt1_8c5720,view_index * 2);
  func_0x00484960(*(undefined4 *)(race_obj + WORLD_RECORD_C118_OFFSET),camera_matrix);
  /* fetch a float scale and pass its raw bits as an integer argument to get_viewport_camera_matrix */
  viewport_scale = (float10)func_0x004757e0(0);
  func_0x00449c40(get_viewport_camera_matrix(0),(float)viewport_scale);
  return;
}


