// cluster-entries: course_warzone_cb1_sub_45d8c0
/* Installed cluster course_warzone_cb1_sub_45d8c0 (tools/cluster_install.py from output/cluster_pilot_kb/course_warzone_cb1_sub_45d8c0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00475100*/ extern int course_height_grid_sample_with_normal();
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*005dd338*/ extern float k_f_0_01;
/*005dd620*/ extern float float_10_0;
/*005df6a0*/ extern float k_f_0_0098;
/* course_warzone_cb1_sub_45d8c0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* single caller course_warzone_cb1 */

void course_warzone_cb1_sub_45d8c0(int *list)

{
  RwV3d *v;
  RwFrame *frame;
  int iVar1;
  int i;
  float *item_cursor;
  RwV3d unused_vec;
  struct {
    RwV3d position; /* sampled point (x, z in; y out: the terrain height) */
    undefined4 uStack_4;
  } frame_obj;
  
  i = 0;
  item_cursor = (float *)list[1];
  frame_obj.uStack_4 = 0;
  if (0 < *list) {
    do {
      if (item_cursor[9] != 0.0) {
        unused_vec.x = item_cursor[0xb];
        unused_vec.y = item_cursor[0xb];
        unused_vec.z = item_cursor[0xb];
        frame = *(RwFrame **)(list[2] + i * 4);
        RwFrameScale(frame,&unused_vec,rwCOMBINEREPLACE);
        RwFrameRotate(*(RwFrame **)(list[2] + i * 4),(RwV3d *)(item_cursor + 3),item_cursor[10],
                      rwCOMBINEPOSTCONCAT);
        v = (RwV3d *)(item_cursor + 6);
        RwFrameTranslate(*(RwFrame **)(list[2] + i * 4),v,rwCOMBINEPOSTCONCAT);
        v->x = v->x + *item_cursor;
        item_cursor[7] = item_cursor[7] + item_cursor[1];
        item_cursor[8] = item_cursor[2] + item_cursor[8];
        frame_obj.position.x = RwFrame_modelling(frame).pos.x;
        frame_obj.position.y = 0.0;
        frame_obj.position.z = RwFrame_modelling(frame).pos.z;
        item_cursor[10] = item_cursor[10] + float_10_0;
        item_cursor[0xb] = item_cursor[0xb] - k_f_0_01;
        iVar1 = course_height_grid_sample_with_normal(&frame_obj.position);
        item_cursor[1] = item_cursor[1] - k_f_0_0098;
        if ((iVar1 == 2) && (RwFrame_modelling(frame).pos.y < frame_obj.position.y)) {
          RwFrame_modelling(frame).pos.y = frame_obj.position.y;
          item_cursor[1] = -item_cursor[1];
        }
        *(int *)&item_cursor[9] = *(int *)&item_cursor[9] + -1;
      }
      item_cursor = item_cursor + 0xc;
      i = i + 1;
    } while (i < *list);
  }
  return;
}

