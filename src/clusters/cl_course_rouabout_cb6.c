// cluster-entries: course_rouabout_cb6
/* Installed cluster course_rouabout_cb6 (tools/cluster_install.py from output/cluster_pilot_kb/course_rouabout_cb6.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00457520*/ extern void LAB_00457520(void); /* shared tail: resets the three spline records (reached by a tail jump) */
/*0048e650*/ extern void __cdecl rwp_object_set_in_world(int object, int in_world);
/*0048e720*/ extern void __cdecl rwp_object_set_matrix(int object, RwMatrix *matrix);
/*00492d10*/ extern void __cdecl rwp_object_set_active(int object, int active);
/*006a2164*/ extern undefined4 g_course_rouabout_6a2164;
/*006a21c8*/ extern int g_course_rouabout_6a21c8[0x1000];
/*006a23c8*/ extern undefined4 g_course_rouabout_6a23c8;
/*006a2634*/ extern undefined4 g_course_6a2634;
/* course_rouabout_cb6 */

/* Rouabout course callback: when the object table is loaded, walks its 8 objects from the end
   (clears the flag word, sets in-world / active, applies the slot matrix) and then runs the shared spline reset. */
void course_rouabout_cb6(void)

{
  int *object;
  RwMatrix *matrix;

  if (g_course_6a2634 != 0) {
    object = g_course_rouabout_6a21c8;
    matrix = (RwMatrix *)&g_course_rouabout_6a23c8;
    do {
      object--;
      g_course_rouabout_6a2164 = 0;
      object[-0x10] = 0;
      rwp_object_set_in_world(*object, 1);
      rwp_object_set_active(*object, 1);
      matrix--;
      rwp_object_set_matrix(*object, matrix);
    } while (matrix != (RwMatrix *)g_course_rouabout_6a21c8);
    g_course_rouabout_6a23c8 = 0;
  }
  LAB_00457520();
}

