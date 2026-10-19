#include "mfl_tier2.h"
/*006c29f8*/ extern undefined g_app_state_6c29f8[0x1000];
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
static int hud_label_draw_text_r(void *reg_edi, undefined4 param_2, undefined4 param_3, void *param_4, void *param_5, void *param_6, void *param_7, float param_8, int param_9);
extern void hud_label_draw_text(void);
#define HUD_LABEL_COUNT (8)
#define HUD_LABEL_STRIDE (0xb0)
__declspec(naked) int hud_label_draw_text_r(void *reg_edi, undefined4 param_2, undefined4 param_3, void *param_4, void *param_5, void *param_6, void *param_7, float param_8, int param_9)
{
    __asm {
        push edi
        push dword ptr [esp + 40]
        push dword ptr [esp + 40]
        push dword ptr [esp + 40]
        push dword ptr [esp + 40]
        push dword ptr [esp + 40]
        push dword ptr [esp + 40]
        push dword ptr [esp + 40]
        push dword ptr [esp + 40]
        mov edi, dword ptr [esp + 40]
        call hud_label_draw_text
        add esp, 32
        pop edi
        ret
    }
}

/* hud_labels_render_all */

/* "blending setup then renders each 0xB0 byte label (8)" */
/* Draws the HUD labels. Takes no parameters and returns nothing. Sets z test and z write off
   and blends with SRC = SRCALPHA, DEST = ONE, then draws every label slot whose active flag is
   nonzero. Restores SRC = SRCALPHA, DEST = INVSRCALPHA on exit; z test and z write stay off.
   Slot layout is inferred: slots are HUD_LABEL_STRIDE bytes apart, and label_cursor points 0xa0
   bytes into the slot (the first cursor is g_app_state_6c29f8 itself). */

void hud_labels_render_all(void)

{
  int labels_left;
  undefined *label_cursor;

  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDONE);
  label_cursor = g_app_state_6c29f8;
  labels_left = HUD_LABEL_COUNT;
  /* walk the HUD_LABEL_COUNT slots; the active flag is 4 bytes before label_cursor */
  do {
    if (*(int *)(label_cursor + -4) != 0) {
      hud_label_draw_text_r((void *)(label_cursor + -0xa0),*(undefined4 *)(label_cursor + -0x14),
                          *(undefined4 *)(label_cursor + -0x10),(void *)(label_cursor + 4),(void *)(label_cursor + 4),(void *)label_cursor,(void *)label_cursor,
                          *(float *)(label_cursor + 8),1);
    }
    label_cursor = label_cursor + HUD_LABEL_STRIDE;
    labels_left = labels_left + -1;
  } while (labels_left != 0);
  RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDINVSRCALPHA);
  return;
}


