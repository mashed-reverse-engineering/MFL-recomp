// cluster-entries: rwp_fn_45d5b0
/* Installed cluster rwp_fn_45d5b0 (tools/cluster_install.py from output/cluster_pilot_kb/rwp_fn_45d5b0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern char UNK_005df670[];
/*0045cc10*/ extern int object_debug_print();
/*0048e720*/ extern undefined __cdecl rwp_object_set_matrix(int p0, RwMatrix *p1);
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*0060d9c8*/ extern undefined4 g_course_60d9c8;
/*0060d9cc*/ extern undefined4 g_course_60d9cc;
/*006a27a0*/ extern char g_course_warzone_6a27a0[];
/*006a2950*/ extern char g_course_get_6a2950[];
/*006a27f0*/ extern char g_warzone_matrix_b[];
/*006a2848*/ extern char g_warzone_matrix_a[];
/* rwp_fn_45d5b0 */

/* callee category rwp */

undefined4 rwp_fn_45d5b0(undefined4 pass_through)

{
  int pontoon_a_body;
  int words_left;
  undefined4 *src_cursor;
  undefined4 *dst_cursor;
  
  pontoon_a_body = g_course_60d9c8;
  src_cursor = (undefined4 *)((int)&g_course_get_6a2950 + 0x8);
  dst_cursor = (undefined4 *)&g_warzone_matrix_a;
  for (words_left = 0x10; words_left != 0; words_left = words_left + -1) {
    *dst_cursor = *src_cursor;
    src_cursor = src_cursor + 1;
    dst_cursor = dst_cursor + 1;
  }
  src_cursor = (undefined4 *)((int)&g_course_warzone_6a27a0 + 0x8);
  dst_cursor = (undefined4 *)&g_warzone_matrix_b;
  for (words_left = 0x10; words_left != 0; words_left = words_left + -1) {
    *dst_cursor = *src_cursor;
    src_cursor = src_cursor + 1;
    dst_cursor = dst_cursor + 1;
  }
  rwp_object_set_matrix(pontoon_a_body,(RwMatrix *)((int)&g_course_get_6a2950 + 0x8));
  FID_conflict__wprintf("we have reset the pontoon\n");
  object_debug_print(&UNK_005df670,((int)&g_course_get_6a2950 + 0x8));
  rwp_object_set_matrix(g_course_60d9cc,(RwMatrix *)((int)&g_course_warzone_6a27a0 + 0x8));
  return pass_through;
}

