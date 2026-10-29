/* compile: /G5 */
#include "mfl_tier2.h"
extern void (__cdecl *g_rt_7da8a8)(int);
/* x */
void png_save_uint_32(unsigned char *buf, unsigned i)
{
  buf[0] = i >> 24;
  buf[1] = i >> 16;
  buf[2] = i >> 8;
  buf[3] = i;
}
