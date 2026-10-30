/* compile: /G5 */
#include "mfl_tier2.h"
#define ALIGN16(x) (((x) + 0xf) & 0xfffffff0)
typedef struct RwpBatchCfg { char pad[8]; unsigned count_b; char pad2[4]; unsigned count_a; } RwpBatchCfg;
/* Size of the batch scratch block for a configuration: aligned 40 byte records plus 16 byte entries plus the 0x30 header. (Rwp library) */
unsigned rwp_batch_00578e50(RwpBatchCfg *cfg)
{
  return ALIGN16(cfg->count_a * 40 - 40) + ALIGN16(cfg->count_b * 16) + 0x30;
}
