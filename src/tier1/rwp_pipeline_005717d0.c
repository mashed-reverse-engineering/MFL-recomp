/* compile: /G5 */
#include "mfl_tier2.h"
extern char *RwEngineInstance;
typedef struct RwpPipeline { char pad[0x40c]; void *buf_a; char pad2[0x8]; void *buf_b; } RwpPipeline;
extern void rwp_capsule_00575b80(void *p);
/* Frees the two buffers of a pipeline and the pipeline; returns 1. (Rwp library) */
int rwp_pipeline_005717d0(RwpPipeline *p)
{
  rwp_capsule_00575b80(p->buf_a);
  rwp_capsule_00575b80(p->buf_b);
  RwFree(p);
  return 1;
}
