#include "mfl_tier2.h"
/* FUN_0056f3a0 */

/* Merges two axis-aligned boxes into their union (inferred).
   box_a and box_b each hold a min corner in floats [0..2] and a max corner in floats [4..6];
   float [3] and [7] are not read or written. merged_box receives the smallest min corner and
   the largest max corner of the two boxes. Only merged_box is written. */
void FUN_0056f3a0(float *merged_box,float *box_a,float *box_b)

{
  float chosen;

  /* max corner, x (inferred) */
  if (!(box_a[4] >= box_b[4])) {      /* original: fcomp, and eax,0x100 (C0 = less or unordered) */
    chosen = box_b[4];
  }
  else {
    chosen = box_a[4];
  }
  merged_box[4] = chosen;
  /* max corner, y (inferred). Picks box_b only when box_b > box_a. Ties keep box_a; NaN picks box_b */
  if (!(box_b[5] <= box_a[5])) {
    chosen = box_b[5];
  }
  else {
    chosen = box_a[5];
  }
  merged_box[5] = chosen;
  /* max corner, z (inferred), same selection rule as y */
  if (!(box_b[6] <= box_a[6])) {
    chosen = box_b[6];
  }
  else {
    chosen = box_a[6];
  }
  merged_box[6] = chosen;
  /* min corner, x (inferred). Picks box_b only when box_a > box_b */
  if (!(*box_a <= *box_b)) {
    chosen = *box_b;
  }
  else {
    chosen = *box_a;
  }
  *merged_box = chosen;
  /* min corner, y (inferred) */
  if (!(box_b[1] >= box_a[1])) {
    chosen = box_b[1];
  }
  else {
    chosen = box_a[1];
  }
  merged_box[1] = chosen;
  /* min corner, z (inferred) */
  if (box_a[2] <= box_b[2]) {
    merged_box[2] = box_a[2];
    return;
  }
  merged_box[2] = box_b[2];
  return;
}

