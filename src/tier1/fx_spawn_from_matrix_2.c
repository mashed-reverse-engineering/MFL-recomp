#include "mfl_tier2.h"
/*00498fe0*/ extern int fx_decal_stamp_matrix(RwReal *matrix, undefined4 stamp_arg, int param_3);
/* fx_spawn_from_matrix_2 */
/* "particles_fn_498fe0(matrix, ?, 2)" */
void fx_spawn_from_matrix_2(RwReal *matrix,undefined4 stamp_arg)
{
  fx_decal_stamp_matrix(matrix,stamp_arg,2);
}
