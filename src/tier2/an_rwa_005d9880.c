/* rwa_soft_dsp_get_params_005d9880 (0x005d9880): software dsp node: default creation parameters */
#include "mfl_tier2.h"

/*005c1d50*/ extern void _RwaStreamNodeGetParams(void *def, void *params);
/*0064a80c*/ extern undefined PTR_rwa_unit246d50_005c1c10_0064a80c;      /* the stream node's default allocator */

/* the stream node's defaults, then: 1 input, 2 blocks, no format / callbacks.  Returns def. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_soft_dsp_get_params_005d9880(void *def, char *params)
{
  _RwaStreamNodeGetParams(def, params);
  *(int *)(params + 0x1c) = 0;
  *(int *)(params + 0x24) = 0;
  *(int *)(params + 0x28) = 0;
  *(int *)(params + 0x04) = 0;
  *(int *)(params + 0x2c) = 0;
  *(void **)(params + 0x20) = &PTR_rwa_unit246d50_005c1c10_0064a80c;
  *(int *)(params + 0x00) = 1;
  *(int *)(params + 0x0c) = 2;
  return def;
}
