/* compile: /G5 */
#include "mfl_tier2.h"
/*007da6d4*/ extern int g_data_7da6d4;
/*0063a220*/ extern float g_data_63a220;
/*004dbca0*/ extern void *__cdecl FUN_004dbca0(void);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Module open: caches the device capability values (float limit, flag) of the D3D9 device caps; returns its argument. */
void *PTankOpen(void *p)
{
  char *x = FUN_004dbca0();
  g_data_63a220 = FLD(x, float, 0xb0);
  g_data_7da6d4 = (FLD(x, unsigned, 0x8c) >> 0x14) & 1;
  return p;
}
