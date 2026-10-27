#include "mfl_tier2.h"
extern char LAB_00484990[];
/*00484890*/ extern undefined __cdecl geometry_for_all_materials_cb(undefined4 p0, void *p1);
/* atomic_set_material_alpha_value */
/* "material alpha callback over all materials of the atomic" */
/* Runs the material alpha callback (LAB_00484990) over every material of `atomic` via
   geometry_for_all_materials_cb. `alpha` is passed by address, so the callback reads the
   value through the context pointer (inferred). Returns `atomic`. */
undefined4 __cdecl atomic_set_material_alpha_value(undefined4 atomic,undefined4 alpha)
{
  /* context handed to the callback: [0] = callback code address, [1] = pointer to `alpha` (inferred) */
  struct {
    undefined1 *local_8;
    undefined4 *local_4;
  } material_cb_ctx;

  material_cb_ctx.local_8 = &LAB_00484990;
  material_cb_ctx.local_4 = &alpha;
  geometry_for_all_materials_cb(atomic,&material_cb_ctx.local_8);
  return atomic;
}
