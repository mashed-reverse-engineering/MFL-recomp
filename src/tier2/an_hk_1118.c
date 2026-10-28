#include "mfl_tier2.h"
/* gap_4c4cc0 */

/* Closest-hit recorder (inferred): best_ctx = {out_record, count_ptr, best_dist_ptr, word3}; copies tri data into out_record when cand_dist is a new minimum (or first hit), always bumps *count_ptr, returns tri. */

undefined4 *
gap_4c4cc0(char *unused_1,undefined4 arg_b,undefined4 *tri,float cand_dist,int *best_ctx)

{
  int *count_ptr;
  undefined4 *out_record;
  int ctx_word3;
  
  count_ptr = (int *)best_ctx[1];
  out_record = (undefined4 *)*best_ctx;
  ctx_word3 = best_ctx[3];
  if (*count_ptr == 0) {
    *(float *)best_ctx[2] = cand_dist;
  }
  else if (!(cand_dist < *(float *)best_ctx[2])) goto LAB_004c4d78;      /* original: fcomp, test ah,5, jp (NaN skips) */
  if (out_record != (undefined4 *)0x0) {
    *out_record = *tri;
    out_record[1] = tri[1];
    out_record[2] = tri[2];
    out_record[3] = tri[6];
    out_record[4] = *(undefined4 *)tri[7];
    out_record[5] = *(undefined4 *)(tri[7] + 4);
    out_record[6] = *(undefined4 *)(tri[7] + 8);
    out_record[7] = *(undefined4 *)tri[8];
    out_record[8] = *(undefined4 *)(tri[8] + 4);
    out_record[9] = *(undefined4 *)(tri[8] + 8);
    out_record[10] = *(undefined4 *)tri[9];
    out_record[0xb] = *(undefined4 *)(tri[9] + 4);
    out_record[0xc] = *(undefined4 *)(tri[9] + 8);
    out_record[0xd] = arg_b;
    *(float *)&out_record[0xe] = cand_dist;
    out_record[0xf] = ctx_word3;
    *count_ptr = *count_ptr + 1;
    return tri;
  }
LAB_004c4d78:
  *count_ptr = *count_ptr + 1;
  return tri;
}


