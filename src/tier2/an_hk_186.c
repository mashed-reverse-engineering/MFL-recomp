#include "mfl_tier2.h"
/* FUN_004ad9e0 */

#define SQRT_INPUT_MAX (0x40000000)   /* 2^30, largest value accepted (inferred) */
#define SQRT_RESULT_MAX (0x8000)      /* sqrt(SQRT_INPUT_MAX), returned for oversized input (inferred) */

/*
 * Integer square root of value (inferred), using Newton iteration x = (x*x + value) / (2*x).
 *   value: the number to take the root of, 0 .. SQRT_INPUT_MAX.
 *   returns: the root, 0 for value 0, SQRT_RESULT_MAX for anything above SQRT_INPUT_MAX.
 * The starting guess is the smallest power of two whose square reaches value, then up to
 * three Newton steps refine it.
 */
int __stdcall FUN_004ad9e0(int value)

{
  int root;

  root = 1;
  /* out of range: clamp to the largest result */
  if (SQRT_INPUT_MAX < value) {
    return SQRT_RESULT_MAX;
  }
  /* find the starting guess: double root until root*root >= value */
  if (1 < value) {
    do {
      root = root * 2;
    } while (root * root < value);
  }
  /* sqrt(0) is 0; without this, the refinement below divides by root * 2 == 0 */
  if (value == 0) {
    return 0;
  }
  /* Newton refinement: first step always runs */
  root = (root * root + value) / (root * 2);
  /* second and third steps only run while root stays non-negative (overflow guard, inferred) */
  if ((-1 < root) && (root = (root * root + value) / (root * 2), -1 < root)) {
    root = (root * root + value) / (root * 2);
  }
  return root;
}


