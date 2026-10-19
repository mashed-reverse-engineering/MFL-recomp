#include "mfl_tier2.h"
/* FUN_00571810
 * Copies three 32-bit words from src (a 3-component vector, inferred) into obj at
 * byte offsets 0x4c, 0x50 and 0x54 (inferred to be one vector field of obj).
 * obj: destination object, byte-addressed. src: source words, read as undefined4 *.
 * Returns nothing.
 */

void __cdecl FUN_00571810(char *obj,undefined4 *src)

{
  *(undefined4 *)(obj + 0x4c) = *src;
  *(undefined4 *)(obj + 0x50) = src[1];
  *(undefined4 *)(obj + 0x54) = src[2];
  return;
}


