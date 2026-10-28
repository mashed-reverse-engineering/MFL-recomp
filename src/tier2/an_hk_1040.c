#include "mfl_tier2.h"
/*006a1e68*/ extern undefined4 g_data_6a1e68;
/*006a1e6c*/ extern undefined4 g_data_6a1e6c;
/*006a1e70*/ extern undefined4 g_data_6a1e70;
/*006a1e74*/ extern undefined4 g_data_6a1e74;
/*006a1f28*/ extern undefined4 g_course_6a1f28;
/*006a1da4*/ extern char g_data_6a1da4[];
/* course_nt1_helper_4557f0 */

/* callers course_nt1_cb1,course_nt1_s_cb1 */

void course_nt1_helper_4557f0(void)

{
  int byte_offset;
  int next_offset;
  struct {
    undefined4 x;
    undefined4 y;
    undefined4 z;
  } unit_z_vec;
  
  /* Accumulate 6 rows of 8 floats (stride 0x20 bytes): table[+4..] += g_data_6a1e68[..] (inferred). */
  unit_z_vec.x = 0;
  unit_z_vec.y = 0;
  unit_z_vec.z = 0x3f800000;
  byte_offset = 0;
  do {
    next_offset = byte_offset + 0x20;
    *(float *)(byte_offset + ((char *)&g_data_6a1da4 + 0x4)) = *(float *)((char *)((undefined *)&g_data_6a1e68) + byte_offset) + *(float *)(byte_offset + ((char *)&g_data_6a1da4 + 0x4))
    ;
    *(float *)(byte_offset + ((char *)&g_data_6a1da4 + 0x8)) = *(float *)((char *)((undefined *)&g_data_6a1e6c) + byte_offset) + *(float *)(byte_offset + ((char *)&g_data_6a1da4 + 0x8))
    ;
    *(float *)(byte_offset + ((char *)&g_data_6a1da4 + 0xc)) = *(float *)((char *)((undefined *)&g_data_6a1e70) + byte_offset) + *(float *)(byte_offset + ((char *)&g_data_6a1da4 + 0xc))
    ;
    *(float *)(byte_offset + ((char *)&g_data_6a1da4 + 0x10)) = *(float *)((char *)((undefined *)&g_data_6a1e74) + byte_offset) + *(float *)(byte_offset + ((char *)&g_data_6a1da4 + 0x10))
    ;
    *(float *)(byte_offset + ((char *)&g_data_6a1da4 + 0x14)) = *(float *)(byte_offset + ((char *)&g_data_6a1e74 + 0x4)) + *(float *)(byte_offset + ((char *)&g_data_6a1da4 + 0x14));
    *(float *)(byte_offset + ((char *)&g_data_6a1da4 + 0x18)) = *(float *)(byte_offset + ((char *)&g_data_6a1e74 + 0x8)) + *(float *)(byte_offset + ((char *)&g_data_6a1da4 + 0x18));
    *(float *)(byte_offset + ((char *)&g_data_6a1da4 + 0x1c)) = *(float *)(byte_offset + ((char *)&g_data_6a1e74 + 0xc)) + *(float *)(byte_offset + ((char *)&g_data_6a1da4 + 0x1c));
    *(float *)(byte_offset + ((char *)&g_data_6a1da4 + 0x20)) = *(float *)(byte_offset + ((char *)&g_data_6a1e74 + 0x10)) + *(float *)(byte_offset + ((char *)&g_data_6a1da4 + 0x20));
    byte_offset = next_offset;
  } while (next_offset < 0xc0);
  /* Passes the +Z unit vector (0,0,1) and the table at g_data_6a1da4+4 (inferred). */
  course_nt1_helper_4557f0_sub_4a5ba0(g_course_6a1f28,&unit_z_vec.x,((char *)&g_data_6a1da4 + 0x4));
  return;
}


