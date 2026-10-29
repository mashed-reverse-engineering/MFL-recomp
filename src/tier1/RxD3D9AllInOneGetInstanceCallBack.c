/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Instance callback of the all-in-one pipeline data block. */
void *RxD3D9AllInOneGetInstanceCallBack(void *p)
{
  return FLD(p, void **, 0x14)[0];
}
