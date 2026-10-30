/* compile: /G5 */
#include "mfl_tier2.h"
extern char *RwEngineInstance;
extern void *rwp_pipeline_00571200(void *pipeline, int a, int b, int c);
/* Allocates a pipeline block of 0x424 bytes and builds the pipeline in it (rwp_pipeline_00571200). (Rwp library) */
void *rwp_pipeline_00570e50(int a, int b, int c)
{
  void *pipeline = RwMalloc(0x424, 0x30900);
  return rwp_pipeline_00571200(pipeline, a, b, c);
}
