#include "mfl_tier2.h"
/*004c52b0*/ extern int FUN_004c52b0(undefined4 p0, undefined4 p1, undefined4 p2);
/* load_shadow_models_cb_456540 */

/* function pointer pushed/stored by load_shadow_models (PUSH 0x456540) */

/* Enumeration callback: stores model_obj into model_slots[kind-1] (kinds 1..4 from FUN_004c52b0) and increments the visit counter at model_slots[4]; returns model_obj (inferred). */
undefined4 load_shadow_models_cb_456540(undefined4 model_obj,undefined4 *model_slots)

{
  undefined4 model_kind;
  
  model_kind = FUN_004c52b0(model_obj,0,0);
  switch(model_kind) {
  case 1:
    *model_slots = model_obj;
    model_slots[4] = model_slots[4] + 1;
    return model_obj;
  case 2:
    model_slots[1] = model_obj;
    model_slots[4] = model_slots[4] + 1;
    return model_obj;
  case 3:
    model_slots[2] = model_obj;
    model_slots[4] = model_slots[4] + 1;
    return model_obj;
  case 4:
    model_slots[3] = model_obj;
  }
  model_slots[4] = model_slots[4] + 1;
  return model_obj;
}


