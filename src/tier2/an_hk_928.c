#include "mfl_tier2.h"
/*00498fe0*/ extern int fx_decal_stamp_matrix(RwReal *param_1, undefined4 param_2, int param_3);
/* fx_spawn_from_matrix_2 */

/* "particles_fn_498fe0(matrix, ?, 2)" */
/* TIER2: unvalidated 2026-10-19 authority=diffemu */
void fx_spawn_from_matrix_2(RwReal *param_1,undefined4 param_2)
{
  fx_decal_stamp_matrix(param_1,param_2,2);
}
