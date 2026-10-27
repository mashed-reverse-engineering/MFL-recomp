// cluster-entries: course_nt1_helper_455770
/* Installed cluster course_nt1_helper_455770 (tools/cluster_install.py from output/cluster_pilot_kb/course_nt1_helper_455770.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0042a430*/ extern int course_get_ambient_light();
/*0042a440*/ extern int course_get_directional_light();
/*004a5d50*/ extern int course_helper_4a5d50();
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*006a1da8*/ extern undefined4 g_course_nt1_6a1da8[0x30];
/*006a1e68*/ extern undefined4 g_data_6a1e68[0x30]; /* the first four words hold 3.0, 4.0, 2.0, 3.5 after the reset */
/*006a1f28*/ extern undefined4 g_course_6a1f28;
/* course_nt1_helper_455770 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* callers course_nt1_cb0,course_nt1_s_cb0 */

void course_nt1_helper_455770(int course)

{
  undefined4 atomic;
  undefined4 light;
  int i;
  
  atomic = clump_first_atomic(*(undefined4 *)(course + 0xc110));
  light = course_get_directional_light();
  light = course_get_ambient_light(light);
  g_course_6a1f28 = course_helper_4a5d50(atomic,*(undefined4 *)(course + 0xc000),course,light);
  for (i = 0; i < 0x30; i++) {
    g_data_6a1e68[i] = 0;
  }
  for (i = 0; i < 0x30; i++) {
    g_course_nt1_6a1da8[i] = 0;
  }
  g_data_6a1e68[0] = 0x40400000;
  g_data_6a1e68[1] = 0x40800000;
  g_data_6a1e68[2] = 0x40000000;
  g_data_6a1e68[3] = 0x40600000;
  return;
}

