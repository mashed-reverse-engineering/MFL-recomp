/* compile: /G5 */
#include "mfl_tier2.h"
/* Native write of a geometry: NATIVEDATA (0x510) stream write callback.
   L1 RT3 2026-11-06: no longer forwards to _rpGeometryNativeWrite (0x00501380, now an int3 trap row). The only caller of a
   stream write callback is _rwPluginRegistryWriteDataChunks, itself an RT1 trap row, so this never runs; it fails like a
   write error (NULL stream). Was byte-identical to 0x004f4320 (tier1 bytes-reloc pin), now tier1 by reading. */
int lib_4f4320(void *a, void *b, void *c)
{
  return 0;
}
