#include "mfl_tier2.h"
/*006c3da0*/ extern undefined4 g_gfx_6c3da0[0x1000];
/*006c3da4*/ extern undefined4 g_gfx_render_6c3da4;
/*006c3da8*/ extern undefined4 g_gfx_render_6c3da8;
/*006c3dac*/ extern undefined4 g_gfx_render_6c3dac;
/*006c3ea0*/ extern undefined4 g_gfx_6c3ea0;
/*006c3eac*/ extern undefined4 g_gfx_6c3eac;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*0089de08*/ extern int (__cdecl *g_gfx_89de08)();
/*0089de0c*/ extern undefined4 g_gfx_89de0c;
/* gfx_render_state_probe */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "gets and sets the cull mode while drawing the debug pass" */

void gfx_render_state_probe(void)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  union {
    char gs_buf[16];
    struct {
      int iStack_14;
      int iStack_10;
      int iStack_c;
      void *local_8;
    } m;
  } frame;
  
  RwRenderStateGet(rwRENDERSTATECULLMODE,&frame.m.local_8);
  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)0x2);
  iVar5 = g_gfx_89de0c;
  iVar1 = g_gfx_6c3eac;
  if (0 < (int)g_gfx_6c3eac) {
    frame.m.iStack_10 = 1;
    piVar2 = g_gfx_6c3da0;
    frame.m.iStack_c = g_gfx_6c3eac;
    frame.m.iStack_14 = g_gfx_6c3eac;
    do {
      frame.m.iStack_14 = frame.m.iStack_14 + -1;
      iVar4 = frame.m.iStack_10;
      if (3 < frame.m.iStack_14) {
        iVar3 = frame.m.iStack_10 + 3;
        do {
          iVar1 = *piVar2;
          if (*(float *)(iVar1 + 0x24 + iVar5) < *(float *)((g_gfx_6c3da0)[iVar4] + 0x24 + iVar5))
          {
            *piVar2 = (g_gfx_6c3da0)[iVar4];
            (g_gfx_6c3da0)[iVar4] = iVar1;
          }
          iVar1 = *piVar2;
          if (*(float *)(iVar1 + 0x24 + iVar5) <
              *(float *)((&g_gfx_render_6c3da4)[iVar4] + 0x24 + iVar5)) {
            *piVar2 = (&g_gfx_render_6c3da4)[iVar4];
            (&g_gfx_render_6c3da4)[iVar4] = iVar1;
          }
          iVar1 = *piVar2;
          if (*(float *)(iVar1 + 0x24 + iVar5) <
              *(float *)((&g_gfx_render_6c3da8)[iVar4] + 0x24 + iVar5)) {
            *piVar2 = (&g_gfx_render_6c3da8)[iVar4];
            (&g_gfx_render_6c3da8)[iVar4] = iVar1;
          }
          iVar1 = *piVar2;
          if (*(float *)(iVar1 + 0x24 + iVar5) <
              *(float *)((&g_gfx_render_6c3dac)[iVar4] + 0x24 + iVar5)) {
            *piVar2 = (&g_gfx_render_6c3dac)[iVar4];
            (&g_gfx_render_6c3dac)[iVar4] = iVar1;
          }
          iVar3 = iVar3 + 4;
          iVar4 = iVar4 + 4;
          iVar1 = g_gfx_6c3eac;
        } while (iVar3 < (int)g_gfx_6c3eac);
      }
      if (iVar4 < iVar1) {
        do {
          iVar1 = *piVar2;
          if (*(float *)(iVar1 + 0x24 + iVar5) < *(float *)((g_gfx_6c3da0)[iVar4] + 0x24 + iVar5))
          {
            *piVar2 = (g_gfx_6c3da0)[iVar4];
            (g_gfx_6c3da0)[iVar4] = iVar1;
          }
          iVar4 = iVar4 + 1;
          iVar1 = g_gfx_6c3eac;
        } while (iVar4 < (int)g_gfx_6c3eac);
      }
      frame.m.iStack_10 = frame.m.iStack_10 + 1;
      piVar2 = piVar2 + 1;
      frame.m.iStack_c = frame.m.iStack_c + -1;
    } while (frame.m.iStack_c != 0);
  }
  iVar5 = 0;
  if (0 < iVar1) {
    do {
      (*g_gfx_89de08)((g_gfx_6c3da0)[iVar5]);
      g_gfx_6c3ea0 = g_gfx_6c3ea0 + 1;
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)g_gfx_6c3eac);
  }
  g_gfx_6c3eac = 0;
  RwRenderStateSet(rwRENDERSTATECULLMODE,frame.m.local_8);
  return;
}


