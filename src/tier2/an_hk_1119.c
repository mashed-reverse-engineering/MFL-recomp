#include "mfl_tier2.h"
/* gap_4c4e70 */

/* no context */

undefined4 * gap_4c4e70(char *unused_1,undefined4 *hit,float distance,int *ctx)

{
  int *count_ptr;
  undefined4 *out_record;
  int hit_object;
  
  count_ptr = (int *)ctx[1];
  out_record = (undefined4 *)*ctx;
  hit_object = ctx[3];
  /* Keep-closest collector (inferred): ctx[0]=output record (may be null), ctx[1]=hit counter, ctx[2]=best distance, ctx[3]=hit object. The record is filled for the first hit and for any hit closer than the best so far. */
  if (*count_ptr == 0) {
    *(float *)ctx[2] = distance;
  }
  else if (!(distance < *(float *)ctx[2])) goto LAB_004c4f27;      /* original: fcomp, test ah,5, jp (NaN skips) */
  if (out_record != (undefined4 *)0x0) {
    *out_record = *hit;
    out_record[1] = hit[1];
    out_record[2] = hit[2];
    out_record[3] = hit[6];
    out_record[4] = *(undefined4 *)hit[7];
    out_record[5] = *(undefined4 *)(hit[7] + 4);
    out_record[6] = *(undefined4 *)(hit[7] + 8);
    out_record[7] = *(undefined4 *)hit[8];
    out_record[8] = *(undefined4 *)(hit[8] + 4);
    out_record[9] = *(undefined4 *)(hit[8] + 8);
    out_record[10] = *(undefined4 *)hit[9];
    out_record[0xb] = *(undefined4 *)(hit[9] + 4);
    out_record[0xc] = *(undefined4 *)(hit[9] + 8);
    out_record[0xd] = *(undefined4 *)(hit_object + 0x18);
    *(float *)&out_record[0xe] = distance;
    out_record[0xf] = hit_object;
    *count_ptr = *count_ptr + 1;
    return hit;
  }
LAB_004c4f27:
  *count_ptr = *count_ptr + 1;
  return hit;
}


