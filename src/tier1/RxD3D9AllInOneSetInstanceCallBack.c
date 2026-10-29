/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets the instance callback in the pipeline's all-in-one data block. */
void RxD3D9AllInOneSetInstanceCallBack(void *p, void *cb)
{
  FLD(p, void **, 0x14)[0] = cb;
}
