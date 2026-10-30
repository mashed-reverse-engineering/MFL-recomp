/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct { float x, y, z; } RwpV3;
typedef struct RwpPipeline { char pad[0x4c]; RwpV3 gravity; } RwpPipeline;
/* Sets the 12-byte vector at +0x4c of the pipeline; returns the pipeline. */
RwpPipeline *FUN_00571810(RwpPipeline *p, RwpV3 *v)
{
  p->gravity = *v;
  return p;
}
