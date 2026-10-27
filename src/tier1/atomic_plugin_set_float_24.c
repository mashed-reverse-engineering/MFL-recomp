#include "mfl_tier2.h"
/*0089de0c*/ extern undefined4 g_gfx_89de0c;


#define ATOMIC_PLUGIN_FLOAT_OFFSET_24 0x24

/* Sets a float value at offset 0x24 within the atomic plugin graphics data buffer. */
double atomic_plugin_set_float_24(char *offset, float value)
{
  *(float *)(g_gfx_89de0c + ATOMIC_PLUGIN_FLOAT_OFFSET_24 + offset) = value;
  return (double)value;
}
