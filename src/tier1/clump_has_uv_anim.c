#include "mfl_tier2.h"
/*00484830*/ extern undefined __cdecl material_uv_anim_check(struct RpMaterial *p0, undefined4 *p1);
/*004848b0*/ extern undefined __cdecl clump_for_all_materials(struct RpClump *p0, void *p1);
/* clump_has_uv_anim */
/* Returns 1 when any material of `clump` has a UV animation, 0 otherwise (inferred: material_uv_anim_check
   sets the flag through flag_ptr). Does not change the clump. */
undefined4 clump_has_uv_anim(RpClump *clump)
{
  /* the original builds one 12 byte frame object {flag, callback, flag pointer} and passes the address of
     its 2nd member; the callee reads the member after it, so the three must be adjacent (locals are not) */
  struct { undefined4 flag; code *callback; undefined4 *flag_ptr; } frame;
  
  frame.flag = 0;
  frame.callback = material_uv_anim_check;
  frame.flag_ptr = &frame.flag;
  /* call material_uv_anim_check on every material of the clump; it sets frame.flag when one is animated */
  clump_for_all_materials(clump,&frame.callback);
  return frame.flag;
}
