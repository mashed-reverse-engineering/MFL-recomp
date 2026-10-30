/* compile: /G5 */
#include "mfl_tier2.h"
#define ALIGN16(x) (((x) + 0xf) & 0xfffffff0)
typedef struct RwpPipelineCfg { unsigned flags; char pad04[0x4]; unsigned n8; char pad0c[0x14]; unsigned n20; char pad24[0x18]; unsigned n3c; char pad40[0x4]; unsigned s44; unsigned s48; char pad4c[0x4]; unsigned s50; unsigned s54; unsigned s58; unsigned s5c; unsigned s60; unsigned s64; } RwpPipelineCfg;
extern unsigned rwp_batch_005792f0(RwpPipelineCfg *cfg);
extern unsigned rwp_batch_00578e80(RwpPipelineCfg *cfg);
extern unsigned rwp_batch_00578e50(RwpPipelineCfg *cfg);
extern unsigned rwp_grid_00577ce0(RwpPipelineCfg *cfg);
extern unsigned rwp_grid_00577970(RwpPipelineCfg *cfg);
/* Computes the scratch size of a pipeline configuration (batch, grid and constraint parts, depending on the enabled stages) and stores each part size; returns the largest block needed. (Rwp library) */
unsigned rwp_pipeline_00570f20(RwpPipelineCfg *cfg)
{
  unsigned max = 0;
  unsigned a, n;
  if (cfg->flags & 1) {
    cfg->s60 = rwp_batch_005792f0(cfg);
    if (cfg->s60 > 0) max = cfg->s60;
  }
  cfg->s50 = rwp_batch_00578e80(cfg);
  cfg->s64 = rwp_batch_00578e50(cfg);
  n = cfg->n3c + 1;
  cfg->s5c = ALIGN16(cfg->n8 * 2) + ALIGN16(n * 4) * 2 + ALIGN16(n * 32) + ALIGN16(n * 16) + ALIGN16(n * 12);
  if (cfg->flags & 8) {
    a = cfg->s50;
    if (cfg->flags & 2) a += cfg->s64;
    if (cfg->flags & 4) a += cfg->s5c;
    if (a > max) max = a;
  } else if (cfg->flags & 2) {
    a = cfg->s64;
    if (cfg->flags & 4) a += cfg->s5c;
    if (a > max) max = a;
  } else if (cfg->flags & 4) {
    if (cfg->s5c > max) max = cfg->s5c;
  }
  cfg->s44 = rwp_grid_00577ce0(cfg);
  cfg->s48 = rwp_grid_00577970(cfg);
  cfg->s54 = ALIGN16(cfg->n20 * 16) + ALIGN16(cfg->n20 * 4);
  cfg->s58 = ALIGN16(cfg->n20 * 64);
  if (cfg->flags & 0x10) {
    a = cfg->s44 + cfg->s48;
    if (cfg->flags & 0x20) a += cfg->s54;
    if (cfg->flags & 0x40) a += cfg->s58;
    if (a > max) max = a;
  } else if (cfg->flags & 0x20) {
    a = cfg->s54;
    if (cfg->flags & 0x40) a += cfg->s58;
    if (a > max) max = a;
  } else if (cfg->flags & 0x40) {
    if (cfg->s58 > max) max = cfg->s58;
  }
  return max;
}
