/* compile: /G5 */
#include "mfl_tier2.h"
/* Native write of a world sector: NATIVEDATA (0x510) stream write callback.
   L1 RT3 2026-11-06: no longer forwards to _rpWorldSectorNativeWrite (0x00501aa0, now an int3 trap row). Only
   _rwPluginRegistryWriteDataChunks (an RT1 trap row) calls stream write callbacks, so this never runs; it fails like a
   write error (NULL stream). Was byte-identical to 0x004f4370 (tier1 bytes-reloc pin), now tier1 by reading. */
int lib_4f4370(void *a, void *b, void *c)
{
  return 0;
}
