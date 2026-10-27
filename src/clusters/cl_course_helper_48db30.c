// cluster-entries: course_helper_48db30
/* Installed cluster course_helper_48db30 (tools/cluster_install.py from output/cluster_pilot_k/course_helper_48db30.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0048db10*/ extern int course_helper_48db30_sub_48db10();
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*006f6930*/ extern undefined4 g_rw_6f6930;
/*006f6938*/ extern undefined4 g_data_6f6938;
/*006f6948*/ extern undefined4 g_data_6f6948;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*0088a844*/ extern undefined4 g_rw_88a844;
/* course_helper_48db30 */

/* callers course_neustein_cb0,course_warzone_cb0 */

/* rounding point: the original spills this product to a float before the add */
static float float_barrier(float value)
{
  return value;
}

/* (inferred names) builds `count` 0x18 byte entries (index in the byte at +0x14) from the 0x40 byte records: centre = origin + scale * record vector (record +0x30), two radii = scale * length of the record vectors at +0x10 and +0x00 (entry +0xc / +0x10); registers the table in the g_rw_88a844 list. Always returns 1. */
undefined4 course_helper_48db30(float *origin, int count, int records, float scale)

{
  undefined4 *entries;
  int zero_left;
  int i;
  float *record_cursor;
  undefined4 *zero_cursor;
  RwReal length;
  struct {
    float centre_x;
    float centre_y;
    float centre_z;
  } frame;
  
  entries = RwMalloc(count * 0x18,0);
  i = 0;
  g_data_6f6938 = count;
  g_data_6f6948 = entries;
  if (0 < count) {
    record_cursor = (float *)(records + 0x34);
    do {
      zero_cursor = entries;
      for (zero_left = 6; zero_left != 0; zero_left = zero_left + -1) {
        *zero_cursor = 0;
        zero_cursor = zero_cursor + 1;
      }
      *(char *)(entries + 5) = (char)i;
      frame.centre_x = record_cursor[-1] * scale + *origin;
      frame.centre_y = scale * *record_cursor + origin[1];
      frame.centre_z = float_barrier(record_cursor[1] * scale) + origin[2];
      length = RwV3dLength((RwV3d *)(record_cursor + -9));
      *(float *)&entries[3] = length * scale;
      length = RwV3dLength((RwV3d *)(record_cursor + -0xd));
      *(float *)&entries[4] = length * scale;
      course_helper_48db30_sub_48db10(entries,&frame.centre_x,0);
      entries = entries + 6;
      i = i + 1;
      record_cursor = record_cursor + 0x10;
    } while (i < count);
  }
  *(undefined4 **)(g_rw_88a844 + g_rw_6f6930 * 8) = g_data_6f6948;
  *(undefined1 *)(g_rw_88a844 + 4 + g_rw_6f6930 * 8) = g_data_6f6938;
  g_rw_6f6930 = g_rw_6f6930 + 1;
  return 1;
}

