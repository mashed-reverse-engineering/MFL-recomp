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
  int iVar1;
  int iVar2;
  struct {
    undefined4 uStack_c;
    undefined4 uStack_8;
    undefined4 uStack_4;
  } frame;
  
  frame.uStack_c = 0;
  frame.uStack_8 = 0;
  frame.uStack_4 = 0x3f800000;
  iVar1 = 0;
  do {
    iVar2 = iVar1 + 0x20;
    *(float *)(iVar1 + ((char *)&g_data_6a1da4 + 0x4)) = *(float *)((char *)((undefined *)&g_data_6a1e68) + iVar1) + *(float *)(iVar1 + ((char *)&g_data_6a1da4 + 0x4))
    ;
    *(float *)(iVar1 + ((char *)&g_data_6a1da4 + 0x8)) = *(float *)((char *)((undefined *)&g_data_6a1e6c) + iVar1) + *(float *)(iVar1 + ((char *)&g_data_6a1da4 + 0x8))
    ;
    *(float *)(iVar1 + ((char *)&g_data_6a1da4 + 0xc)) = *(float *)((char *)((undefined *)&g_data_6a1e70) + iVar1) + *(float *)(iVar1 + ((char *)&g_data_6a1da4 + 0xc))
    ;
    *(float *)(iVar1 + ((char *)&g_data_6a1da4 + 0x10)) = *(float *)((char *)((undefined *)&g_data_6a1e74) + iVar1) + *(float *)(iVar1 + ((char *)&g_data_6a1da4 + 0x10))
    ;
    *(float *)(iVar1 + ((char *)&g_data_6a1da4 + 0x14)) = *(float *)(iVar1 + ((char *)&g_data_6a1e74 + 0x4)) + *(float *)(iVar1 + ((char *)&g_data_6a1da4 + 0x14));
    *(float *)(iVar1 + ((char *)&g_data_6a1da4 + 0x18)) = *(float *)(iVar1 + ((char *)&g_data_6a1e74 + 0x8)) + *(float *)(iVar1 + ((char *)&g_data_6a1da4 + 0x18));
    *(float *)(iVar1 + ((char *)&g_data_6a1da4 + 0x1c)) = *(float *)(iVar1 + ((char *)&g_data_6a1e74 + 0xc)) + *(float *)(iVar1 + ((char *)&g_data_6a1da4 + 0x1c));
    *(float *)(iVar1 + ((char *)&g_data_6a1da4 + 0x20)) = *(float *)(iVar1 + ((char *)&g_data_6a1e74 + 0x10)) + *(float *)(iVar1 + ((char *)&g_data_6a1da4 + 0x20));
    iVar1 = iVar2;
  } while (iVar2 < 0xc0);
  course_nt1_helper_4557f0_sub_4a5ba0(g_course_6a1f28,&frame.uStack_c,((char *)&g_data_6a1da4 + 0x4));
  return;
}


