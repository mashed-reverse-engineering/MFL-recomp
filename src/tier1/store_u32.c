#include "mfl_tier2.h"


/* store_u32 */

/* Stores a 32-bit unsigned integer at the destination address. */
void store_u32(unsigned int *dst, unsigned int value)
{
  *dst = value;
  return;
}
