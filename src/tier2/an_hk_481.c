#include "mfl_tier2.h"
extern struct FxSlot g_fx_slots[10];
/*00490cc0*/ extern undefined __cdecl rwp_object_destroy(undefined4 p0);
/*0049c120*/ extern int fx_buffers_free();
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*00651260*/ extern undefined4 g_fx_651260;
/*00651948*/ extern undefined4 g_fx_651948;
/*00651aa0*/ extern undefined4 g_fx_object_count;
/*00651aa4*/ extern undefined4 g_explosion_651aa4;
/*00651aa8*/ extern undefined4 g_fx_651aa8;
/*00651ab0*/ extern char g_fx_651ab0[];

/* Effect object record: size in uint words, index of the rwp body handle (inferred) */
#define FX_OBJECT_STRIDE (0x3b)
#define FX_OBJECT_BODY_INDEX (0xd)
/* body handle value meaning "no rwp body" (inferred from the -1 < check) */
#define FX_NO_BODY (0xffffffff)
/* FxSlot size in uint words (8 * 4 = 32 bytes) */
#define FX_SLOT_STRIDE (8)
/* fx_objects_destroy */

/* "RpClumpDestroy for each effect object and its rwp body" */

/*
 * Frees all effects: destroys the clump and rwp body of each effect object in
 * g_fx_651260, resets the object table, then clears every g_fx_slots entry
 * (its clump and fx_system buffer). No parameters, no return value.
 */

void fx_objects_destroy(void)

{
  uint fx_system;
  int object_count;
  undefined4 *obj;
  uint *slot_fx_system;
  int obj_index;

  obj_index = 0;
  /* Pass 1: walk the effect object table, destroying each clump and rwp body */
  if (0 < (int)g_fx_object_count) {
    obj = &g_fx_651260;
    do {
      RpClumpDestroy((RpClump *)*obj);
      if (-1 < (int)obj[FX_OBJECT_BODY_INDEX]) {
        rwp_object_destroy(obj[FX_OBJECT_BODY_INDEX]);
      }
      object_count = g_fx_object_count;
      (&g_fx_651948)[obj_index] = 0;
      *obj = 0;
      obj[1] = 0;
      obj[0x2f] = 0;
      obj[FX_OBJECT_BODY_INDEX] = FX_NO_BODY;
      obj_index = obj_index + 1;
      obj = obj + FX_OBJECT_STRIDE;
    } while (obj_index < object_count);
  }
  g_fx_object_count = 0;
  g_explosion_651aa4 = 0;
  g_fx_651aa8 = 0;
  /* Pass 2: clear each FxSlot's clump and free its fx_system buffer (inferred) */
  slot_fx_system =&g_fx_slots[0].fx_system;
  do {
    if ((RpClump *)((FxSlot *)(slot_fx_system + -4))->clump != (RpClump *)0x0) {
      RpClumpDestroy((RpClump *)((FxSlot *)(slot_fx_system + -4))->clump);
    }
    fx_system = *slot_fx_system;
    ((FxSlot *)(slot_fx_system + -4))->clump = 0;
    if (fx_system != 0) {
      fx_buffers_free(fx_system);
    }
    *slot_fx_system = 0;
    slot_fx_system[-1] = 0;
    slot_fx_system = slot_fx_system + FX_SLOT_STRIDE;
  } while ((LONG_PTR)slot_fx_system < ((LONG_PTR)&g_fx_651ab0));
  return;
}


