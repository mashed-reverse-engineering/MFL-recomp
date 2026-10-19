#include "mfl_tier2.h"
/* FUN_0056f450 */
/* Combines two 8-float boxes component by component into result (inferred: box bounds).
 * Components 0-2 take the larger of box_a and box_b, components 4-6 take the smaller
 * (inferred: 0-2 = max corner, 4-6 = min corner). Components 3 and 7 are not written.
 * result, box_a, box_b: float[8] arrays. Returns nothing. */
void __cdecl FUN_0056f450(float *result,float *box_a,float *box_b)

{
  float value;

  /* component 4: smaller of the two (the NaN-aware compare picks box_b when unordered) */
  if (box_a[4] < box_b[4] == (box_a[4] == box_b[4])) {
    value = box_b[4];
  }
  else {
    value = box_a[4];
  }
  result[4] = value;
  /* components 5 and 6: smaller of the two */
  if (box_b[5] < box_a[5]) {
    value = box_b[5];
  }
  else {
    value = box_a[5];
  }
  result[5] = value;
  if (box_b[6] < box_a[6]) {
    value = box_b[6];
  }
  else {
    value = box_a[6];
  }
  result[6] = value;
  /* component 0: larger of the two */
  if (*box_a < *box_b) {
    value = *box_b;
  }
  else {
    value = *box_a;
  }
  *result = value;
  /* component 1: larger of the two */
  if (box_b[1] < box_a[1] == (box_b[1] == box_a[1])) {
    value = box_b[1];
  }
  else {
    value = box_a[1];
  }
  result[1] = value;
  /* component 2: larger of the two; returns early when box_a[2] is the pick */
  if (box_b[2] < box_a[2] != (box_b[2] == box_a[2])) {
    result[2] = box_a[2];
    return;
  }
  result[2] = box_b[2];
  return;
}


