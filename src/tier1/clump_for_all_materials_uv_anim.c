#include "mfl_tier2.h"
extern char LAB_00484850[];
/*004848b0*/ extern undefined __cdecl clump_for_all_materials(struct RpClump *p0, void *p1);
/* clump_for_all_materials_uv_anim */
/* "clump_for_all_materials with the callback at 0x484850" */
/* Runs clump_for_all_materials on clump, passing it a two-field frame: the callback
   LAB_00484850 and a pointer to user_data (inferred: the frame's second field is
   read back by the callback). Returns clump. */
RpClump * __cdecl clump_for_all_materials_uv_anim(RpClump *clump,undefined4 user_data)
{
  struct {
    undefined1 *callback;
    undefined4 *user_data_ptr;
  } frame;
  
  frame.callback = &LAB_00484850;
  frame.user_data_ptr = &user_data;
  clump_for_all_materials(clump,&frame.callback);
  return clump;
}
