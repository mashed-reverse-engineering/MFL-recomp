#include "mfl_tier2.h"
/*0048e410*/ extern int rwp_object_find_index_by_id();
/*0048e6d0*/ extern int rwp_object_get_handle();
/*006a1878*/ extern undefined4 g_data_6a1878;
/*006a187c*/ extern undefined4 g_data_6a187c;
/*006a1884*/ extern undefined4 g_course_physics_6a1884;
/*006a1888*/ extern undefined4 g_course_physics_6a1888;
/*006a188c*/ extern undefined4 g_course_6a188c;
/*006a18a0*/ extern undefined4 g_data_6a18a0;
/*006a18a4*/ extern undefined4 g_data_6a18a4;
/*006a18ac*/ extern undefined4 g_data_6a18ac;
/*006a18b0*/ extern undefined4 g_course_physics_6a18b0;
/*006a18b4*/ extern undefined4 g_data_6a18b4;
/*006a1da0*/ extern undefined4 g_course_neustein_6a1da0;
/*006a18dc*/ extern char g_course_physics_6a18dc[];
/*006a1890*/ extern char g_course_physics_6a1890[];
/* course_neustein_cb0_sub_4546e0 */

/* single caller course_neustein_cb0 */

void course_neustein_cb0_sub_4546e0(char *course)

{
  undefined4 *src_last;
  undefined4 *src_b;
  undefined4 *dst;
  undefined4 *dst_next;
  undefined4 *src_a;
  
  if (g_course_neustein_6a1da0 == 0) {
    /* one-time init: clears the object slots, looks up the world objects 4000/5000/0x1389/0xfa1 and caches their handles, then copies a 0x18-dword record per entry (vector at +0x3030 plus the matrix rows at +0x30b0, stepping back 0x300 dwords per iteration) from the course data into the object table */
    g_course_neustein_6a1da0 = 1;
    src_b = (undefined4 *)((char *)&g_course_physics_6a1890 + 0x4);
    do {
      src_b[-1] = 0;
      *src_b = 0;
      src_b = src_b + 10;
    } while ((char *)src_b < (char *)&g_data_6a18b4 + 0x30);
    g_course_6a188c = rwp_object_find_index_by_id(4000);
    g_course_physics_6a1888 = rwp_object_find_index_by_id(0x1389);
    g_course_physics_6a18b0 = rwp_object_find_index_by_id(5000);
    g_data_6a18b4 = rwp_object_find_index_by_id(0xfa1);
    g_course_physics_6a1884 = *(undefined4 *)(course + 0xc9b4);
    g_data_6a18ac = *(undefined4 *)(course + 0xc9b0);
    g_data_6a1878 = rwp_object_get_handle(g_course_physics_6a1888);
    g_data_6a187c = rwp_object_get_handle(g_course_6a188c);
    g_data_6a18a0 = rwp_object_get_handle(g_course_physics_6a18b0);
    g_data_6a18a4 = rwp_object_get_handle(g_data_6a18b4);
    course_neustein_cb0_sub_454650(&g_data_6a1878);
    course_neustein_cb0_sub_454650(&g_data_6a18a0);
    src_b = (undefined4 *)(course + 0x30b0);
    src_a = (undefined4 *)(course + 0x3030);
    dst = (undefined4 *)((char *)&g_course_physics_6a18dc + 0x3f8);
    do {
      dst[-3] = *src_a;
      dst[-2] = src_a[1];
      dst[-1] = src_a[2];
      *dst = src_b[-0x10];
      dst[1] = src_b[-0xf];
      dst[2] = src_b[-0xe];
      dst[3] = *src_b;
      dst[4] = src_b[1];
      dst[5] = src_b[2];
      dst[6] = src_b[0x10];
      dst[7] = src_b[0x11];
      dst[8] = src_b[0x12];
      dst[9] = src_b[0x20];
      dst[10] = src_b[0x21];
      dst[0xb] = src_b[0x22];
      dst[0xc] = src_b[0x30];
      dst[0xd] = src_b[0x31];
      dst[0xe] = src_b[0x32];
      dst[0xf] = src_b[0x40];
      dst[0x10] = src_b[0x41];
      dst[0x11] = src_b[0x42];
      dst[0x12] = src_b[0x50];
      dst[0x13] = src_b[0x51];
      src_last = src_b + 0x52;
      dst_next = dst + 0x18;
      src_a = src_a + -0x300;
      src_b = src_b + -0x300;
      dst[0x14] = *src_last;
      dst = dst_next;
    } while ((char *)dst_next < (char *)&g_data_6a18b4 + 0x4e0);
  }
  return;
}


