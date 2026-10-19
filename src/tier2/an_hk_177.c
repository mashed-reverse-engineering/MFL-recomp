#include "mfl_tier2.h"
/* FUN_005cc950 */

#define S16_MAX (0x7fff)
#define S16_MIN_SYM (-0x7fff)
#define S16_OVERFLOW (0x8000)
#define UNROLL_MASK (0xfffffffc)

/* Adds two buffers of 16-bit samples into dst: dst[i] = saturate(src_a[i] + src_b[i]).
   Sums are clamped to the symmetric range [-0x7fff, 0x7fff] (inferred: -0x8000 never produced).
   dst, src_a, src_b: sample buffers (addresses; inferred to be non-overlapping).
   num_samples: byte length on entry (inferred), halved to a 16-bit sample count.
   Returns nothing. */
void FUN_005cc950(char *dst,char *src_a,char *src_b,uint num_samples)

{
  int scratch;      /* reused: a sum in the unrolled loop, then the remaining count in the tail loop */
  short *b_ptr;
  int sum1;
  uint next_idx;
  int sum3;
  int sum2;
  short *a_ptr;
  uint idx;

  num_samples = num_samples >> 1;
  next_idx = 0;
  idx = 0;
  /* Unrolled loop: 4 samples per pass, only while at least one full group of 4 remains. */
  if ((num_samples & UNROLL_MASK) != 0) {
    b_ptr = (short *)(src_b + 2);
    a_ptr = (short *)(src_a + 6);
    do {
      scratch = (int)b_ptr[-1] + (int)a_ptr[-3];
      sum1 = (int)*(short *)((src_a - src_b) + (LONG_PTR)b_ptr) + (int)*b_ptr;
      sum2 = (int)b_ptr[1] + (int)a_ptr[-1];
      sum3 = (int)b_ptr[2] + (int)*a_ptr;
      /* Saturate each of the four sums to the symmetric 16-bit range. */
      if (scratch < S16_OVERFLOW) {
        if (scratch < S16_MIN_SYM) {
          scratch = S16_MIN_SYM;
        }
      }
      else {
        scratch = S16_MAX;
      }
      *(short *)(dst + idx * 2) = (short)scratch;
      if (sum1 < S16_OVERFLOW) {
        if (sum1 < S16_MIN_SYM) {
          sum1 = S16_MIN_SYM;
        }
      }
      else {
        sum1 = S16_MAX;
      }
      *(short *)((dst - src_b) + (LONG_PTR)b_ptr) = (short)sum1;
      if (sum2 < S16_OVERFLOW) {
        if (sum2 < S16_MIN_SYM) {
          sum2 = S16_MIN_SYM;
        }
      }
      else {
        sum2 = S16_MAX;
      }
      *(short *)(dst + 4 + idx * 2) = (short)sum2;
      if (sum3 < S16_OVERFLOW) {
        if (sum3 < S16_MIN_SYM) {
          sum3 = S16_MIN_SYM;
        }
      }
      else {
        sum3 = S16_MAX;
      }
      next_idx = idx + 4;
      b_ptr = b_ptr + 4;
      *(short *)((dst - src_a) + (LONG_PTR)a_ptr) = (short)sum3;
      a_ptr = a_ptr + 4;
      idx = next_idx;
    } while (next_idx < (num_samples & UNROLL_MASK));
  }
  /* Tail: the 0-3 samples left over after the unrolled loop, one at a time. */
  if (next_idx < num_samples) {
    b_ptr = (short *)(src_b + next_idx * 2);
    scratch = num_samples - next_idx;
    do {
      sum3 = (int)*(short *)((LONG_PTR)b_ptr + (src_a - src_b)) + (int)*b_ptr;
      if (sum3 < S16_OVERFLOW) {
        if (sum3 < S16_MIN_SYM) {
          sum3 = S16_MIN_SYM;
        }
      }
      else {
        sum3 = S16_MAX;
      }
      *(short *)((LONG_PTR)b_ptr + (dst - src_b)) = (short)sum3;
      b_ptr = b_ptr + 1;
      scratch = scratch + -1;
    } while (scratch != 0);
  }
  return;
}


