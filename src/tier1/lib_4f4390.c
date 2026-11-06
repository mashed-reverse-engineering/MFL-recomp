/* compile: /G5 */
#include "mfl_tier2.h"
/* Native read of a world sector: NATIVEDATA (0x510) stream read callback; returns the stream or NULL.
   L1 RT3 2026-11-06: no longer forwards to _rpWorldSectorNativeRead (0x00501d30, now an int3 trap row).
   It runs only for a 0x510 chunk and no shipped asset has one (output/rw_asset_formats_2026-11-06.md);
   it fails like a read error. Was byte-identical to 0x004f4390 (tier1 bytes-reloc pin), now tier1 by reading. */
void *lib_4f4390(void *a, void *b, void *c)
{
  return 0;
}
