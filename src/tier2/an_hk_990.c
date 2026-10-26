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

/* Debug pass: sorts the draw list g_gfx_6c3da0 (g_gfx_6c3eac entries) ascending by the float at +0x24 of each entry's record (base g_gfx_89de0c), a selection sort
   unrolled by 4 in the inner loop, then calls g_gfx_89de08 on every entry with culling set to mode 2, and empties the list. */
void gfx_render_state_probe(void)

{
  int item_or_count;
  int *slot;
  int j_next4;
  int j;
  int base_or_i;
  union {
    char gs_buf[16];
    struct {
      int remaining;
      int i;
      int outer_left;
      void *old_cull_mode;
    } m;
  } frame;
  
  RwRenderStateGet(rwRENDERSTATECULLMODE,&frame.m.old_cull_mode);
  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)0x2);
  base_or_i = g_gfx_89de0c;
  item_or_count = g_gfx_6c3eac;
  if (0 < (int)g_gfx_6c3eac) {
    frame.m.i = 1;
    slot = g_gfx_6c3da0;
    frame.m.outer_left = g_gfx_6c3eac;
    frame.m.remaining = g_gfx_6c3eac;
    do {
      frame.m.remaining = frame.m.remaining + -1;
      j = frame.m.i;
      if (3 < frame.m.remaining) {
        j_next4 = frame.m.i + 3;
        do {
          item_or_count = *slot;
          if (*(float *)(item_or_count + 0x24 + base_or_i) < *(float *)((g_gfx_6c3da0)[j] + 0x24 + base_or_i))
          {
            *slot = (g_gfx_6c3da0)[j];
            (g_gfx_6c3da0)[j] = item_or_count;
          }
          item_or_count = *slot;
          if (*(float *)(item_or_count + 0x24 + base_or_i) <
              *(float *)((&g_gfx_render_6c3da4)[j] + 0x24 + base_or_i)) {
            *slot = (&g_gfx_render_6c3da4)[j];
            (&g_gfx_render_6c3da4)[j] = item_or_count;
          }
          item_or_count = *slot;
          if (*(float *)(item_or_count + 0x24 + base_or_i) <
              *(float *)((&g_gfx_render_6c3da8)[j] + 0x24 + base_or_i)) {
            *slot = (&g_gfx_render_6c3da8)[j];
            (&g_gfx_render_6c3da8)[j] = item_or_count;
          }
          item_or_count = *slot;
          if (*(float *)(item_or_count + 0x24 + base_or_i) <
              *(float *)((&g_gfx_render_6c3dac)[j] + 0x24 + base_or_i)) {
            *slot = (&g_gfx_render_6c3dac)[j];
            (&g_gfx_render_6c3dac)[j] = item_or_count;
          }
          j_next4 = j_next4 + 4;
          j = j + 4;
          item_or_count = g_gfx_6c3eac;
        } while (j_next4 < (int)g_gfx_6c3eac);
      }
      if (j < item_or_count) {
        do {
          item_or_count = *slot;
          if (*(float *)(item_or_count + 0x24 + base_or_i) < *(float *)((g_gfx_6c3da0)[j] + 0x24 + base_or_i))
          {
            *slot = (g_gfx_6c3da0)[j];
            (g_gfx_6c3da0)[j] = item_or_count;
          }
          j = j + 1;
          item_or_count = g_gfx_6c3eac;
        } while (j < (int)g_gfx_6c3eac);
      }
      frame.m.i = frame.m.i + 1;
      slot = slot + 1;
      frame.m.outer_left = frame.m.outer_left + -1;
    } while (frame.m.outer_left != 0);
  }
  base_or_i = 0;
  if (0 < item_or_count) {
    do {
      (*g_gfx_89de08)((g_gfx_6c3da0)[base_or_i]);
      g_gfx_6c3ea0 = g_gfx_6c3ea0 + 1;
      base_or_i = base_or_i + 1;
    } while (base_or_i < (int)g_gfx_6c3eac);
  }
  g_gfx_6c3eac = 0;
  RwRenderStateSet(rwRENDERSTATECULLMODE,frame.m.old_cull_mode);
  return;
}


