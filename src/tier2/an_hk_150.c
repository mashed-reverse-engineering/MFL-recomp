#include "mfl_tier2.h"

#define LIMB_MASK (0x7fffffff)
#define LIMB_BITS (0x1f)
#define HIGHER_BITS_MASK (0xfffffffe)

/* FUN_005bf4e0
 * Unsigned 32x32 -> 64-bit multiply (inferred), done by shift-and-add over the
 * bits of `multiplier`. The running sum is kept in 31-bit limbs: lo_sum holds
 * the low 31 bits, hi_sum the carries above them, and mcand_hi the bits of
 * `multiplicand` shifted past bit 31.
 *
 * multiplier, multiplicand: the two factors.
 * result_hi_out: receives bits 32..63 of the product.
 * result_lo_out: receives bits 0..31 of the product.
 */

void FUN_005bf4e0(uint multiplier,uint multiplicand,uint *result_hi_out,uint *result_lo_out)

{
  uint mcand_hi;
  uint lo_sum;
  uint hi_sum;

  lo_sum = 0;
  hi_sum = 0;
  mcand_hi = 0;
  /* Walk the multiplier bits from the bottom; stop before its last bit,
     which is handled after the loop. */
  for (; (multiplier & HIGHER_BITS_MASK) != 0; multiplier = multiplier >> 1) {
    mcand_hi = mcand_hi << 1;
    if ((int)multiplicand < 0) {
      mcand_hi = mcand_hi | 1;
      multiplicand = multiplicand & LIMB_MASK;
    }
    if ((multiplier & 1) != 0) {
      lo_sum = lo_sum + multiplicand;
      hi_sum = hi_sum + mcand_hi;
      if ((int)lo_sum < 0) {
        hi_sum = hi_sum + 1;
        lo_sum = lo_sum & LIMB_MASK;
      }
    }
    multiplicand = multiplicand << 1;
  }
  /* Last multiplier bit (same step as the loop body, without the shift). */
  mcand_hi = mcand_hi << 1;
  if ((int)multiplicand < 0) {
    mcand_hi = mcand_hi | 1;
    multiplicand = multiplicand & LIMB_MASK;
  }
  if ((multiplier & 1) != 0) {
    lo_sum = lo_sum + multiplicand;
    hi_sum = hi_sum + mcand_hi;
    if ((int)lo_sum < 0) {
      hi_sum = hi_sum + 1;
      lo_sum = lo_sum & LIMB_MASK;
    }
  }
  /* Product = hi_sum * 2^31 + lo_sum, split into 32-bit halves. */
  *result_hi_out = hi_sum >> 1;
  *result_lo_out = hi_sum << LIMB_BITS | lo_sum;
  return;
}


