/* compile: /G5 */
#include "mfl_tier2.h"
#define ALIGN16(x) (((x) + 0xf) & 0xfffffff0)
typedef struct RwpBatchCfg { char pad[8]; unsigned a; unsigned c; unsigned pad2; unsigned b; } RwpBatchCfg;
/* Size in bytes of the scratch block needed for a batch configuration (four 16 byte aligned parts). (Rwp library) */
unsigned rwp_batch_005792f0(RwpBatchCfg *cfg)
{
  return ALIGN16(cfg->b * 16) + ALIGN16(cfg->c * 4) + ALIGN16(cfg->a * 76) + ALIGN16(cfg->a * 4);
}
