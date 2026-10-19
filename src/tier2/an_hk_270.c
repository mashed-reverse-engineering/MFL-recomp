#include "mfl_tier2.h"
/*00481760*/ extern undefined __cdecl atomic_set_render_callback(undefined4 p0, undefined4 p1);
/*0089de0c*/ extern undefined4 g_gfx_89de0c;
#define FLOAT_MINUS_ONE_BITS (0xbf800000)
#define FLOAT_10000_BITS (0x461c4000)
/* atomic_plugin_construct */

/* "plugin constructor: bbox min (-1, 0, 10000)" */

/*
 * Constructor for an atomic's plugin data block.
 * atomic_offset: offset of the atomic's plugin block from g_gfx_89de0c (inferred).
 * Writes 9 words into the block: word 0 = -1.0f, word 1 = 0, words 2-7 = 10000.0f,
 * word 8 = 0. The bbox min is (-1, 0, 10000) per the string above; the meaning of
 * words 3-8 is inferred. Then registers the render callback with a NULL argument.
 * Returns atomic_offset unchanged.
 */
int atomic_plugin_construct(int atomic_offset)

{
  undefined4 *plugin_data;

  /* fill the plugin block */
  plugin_data = (undefined4 *)(g_gfx_89de0c + atomic_offset);
  *plugin_data = FLOAT_MINUS_ONE_BITS;
  plugin_data[1] = 0;
  plugin_data[2] = FLOAT_10000_BITS;
  plugin_data[3] = FLOAT_10000_BITS;
  plugin_data[4] = FLOAT_10000_BITS;
  plugin_data[5] = FLOAT_10000_BITS;
  plugin_data[6] = FLOAT_10000_BITS;
  plugin_data[7] = FLOAT_10000_BITS;
  plugin_data[8] = 0;
  atomic_set_render_callback(atomic_offset,0);
  return atomic_offset;
}


