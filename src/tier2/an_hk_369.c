#include "mfl_tier2.h"
/*00407590*/ extern int fx_render_all();
/*004b4ba7*/ extern int entry();
/*004c5b10*/ extern void FUN_004c5b10(undefined4 p0, undefined4 p1, undefined4 p2);
/*006512cc*/ extern undefined g_race_render_6512cc;
/*00651aa0*/ extern undefined4 g_fx_object_count;
/*00651aa8*/ extern undefined4 g_fx_651aa8;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

/* Size in bytes of one fx entry in the g_race_render_6512cc array (inferred). */
#define FX_ENTRY_SIZE (0xec)
/* Colour used for the highlight quad (ARGB, inferred). */
#define HIGHLIGHT_COLOR (0xff0408c2)
/* Render state values for the additive blend (RW blend function enum). */
#define RW_BLEND_SRCALPHA (5)
#define RW_BLEND_ONE (2)
#define RW_BLEND_INVSRCALPHA (6)

/* fx_render_all_and_highlight */

/* "fx_render_all then draws additive highlight quads for the selected entry" */

void fx_render_all_and_highlight(void)

{
  int fx_index;
  undefined *fx_entry;
  int slot_index;
  int *slot_ptr;
  undefined4 highlight_color;

  fx_index = 0;
  fx_render_all();
  /* Loop over every fx entry. */
  if (0 < (int)g_fx_object_count) {
    fx_entry = &g_race_render_6512cc;
    do {
      highlight_color = HIGHLIGHT_COLOR;
      /* Only the selected entry (g_fx_651aa8) gets the highlight. */
      if (fx_index == g_fx_651aa8) {
        slot_index = 0;
        /* Additive blend (src alpha, one) for the highlight quad. */
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER,(void *)0x0);
        RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)RW_BLEND_SRCALPHA);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)RW_BLEND_ONE);
        FUN_004c5b10(fx_entry,fx_entry + 0xc,&highlight_color);
        /* Back to src alpha / inverse src alpha for the slot callbacks. */
        RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)RW_BLEND_SRCALPHA);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)RW_BLEND_INVSRCALPHA);
        slot_ptr = (int *)(fx_entry + -0x60);
        /* Walk the 10 slot pointers; slots 1 and 2 (inferred) call the vtable function at +0x48. */
        do {
          if ((0 < slot_index) && (slot_index < 3)) {
            (**(code **)(*slot_ptr + 0x48))(*slot_ptr);
          }
          slot_index = slot_index + 1;
          slot_ptr = slot_ptr + 1;
        } while (slot_index < 10);
      }
      fx_index = fx_index + 1;
      fx_entry = fx_entry + FX_ENTRY_SIZE;
    } while (fx_index < (int)g_fx_object_count);
  }
  return;
}


