#include "mfl_tier2.h"
/* bytes per physics entry, and fixed bytes before the entries (inferred) */
#define PHYS_BUFFER_ENTRY_SIZE (0x24)
#define PHYS_BUFFER_BASE_SIZE (0x19c)
/* physics_buffer_size */
/* "(float count) * 0x24 + 0x19C" */
/* Returns the byte size of a physics buffer holding entries for a float count.
 * count_bits: the float count, passed as its raw IEEE bit pattern in an int.
 * divisor: integer divisor applied to count * 60.0f (inferred: 60 units per second).
 * Returns entry_count * PHYS_BUFFER_ENTRY_SIZE + PHYS_BUFFER_BASE_SIZE. */
int physics_buffer_size(int count_bits,int divisor)
{
  int entry_count;

  /* scale the float count by 60.0f, divide, truncate to an int entry count */
  entry_count = ((int)((*(float *)&count_bits * 60.0f) / divisor));
  return entry_count * PHYS_BUFFER_ENTRY_SIZE + PHYS_BUFFER_BASE_SIZE;
}
