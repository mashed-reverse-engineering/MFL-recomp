/* compile: /G5 */
#include "mfl_tier2.h"
#define ALIGN16(x) (((x) + 0xf) & 0xfffffff0)
typedef struct RwpArena { char pad[8]; unsigned cursor; } RwpArena;
typedef struct RwpBatchLayout { unsigned f00; char pad04[0x4]; unsigned f08; unsigned f0c; char pad10[0x4]; unsigned f14; unsigned f18; char pad1c[0x4]; unsigned f20; unsigned f24; unsigned f28; unsigned f2c; char pad30[0x4]; unsigned f34; unsigned f38; char pad3c[0x4]; unsigned f40; unsigned f44; char pad48[0x4]; unsigned f4c; char pad50[0x8]; unsigned f58; char pad5c[0x4]; unsigned f60; char pad64[0x18]; unsigned f7c; } RwpBatchLayout;
typedef struct RwpBatchCfg { char pad00[0x30]; unsigned a; unsigned b; unsigned c; } RwpBatchCfg;
/* Carves the batch arrays (nodes, constraints, pairs, bodies, contacts) out of the arena for a configuration and records their sizes in the layout record. (Rwp library) */
void rwp_batch_00578f10(RwpArena *arena, RwpBatchLayout *l, int unused, RwpBatchCfg *cfg)
{
  l->f28 = l->f14 = cfg->a + 1;
  l->f08 = cfg->a * 2;
  l->f20 = cfg->a * 2;
  arena->cursor = ALIGN16(arena->cursor);
  l->f0c = arena->cursor;
  arena->cursor = ALIGN16(arena->cursor + l->f14 * 20);
  l->f24 = arena->cursor;
  arena->cursor = ALIGN16(arena->cursor + l->f28 * 32);
  l->f00 = arena->cursor;
  arena->cursor = ALIGN16(arena->cursor + l->f08 * 8);
  l->f18 = arena->cursor;
  arena->cursor = ALIGN16(arena->cursor + l->f20 * 64);
  l->f34 = cfg->b;
  l->f40 = cfg->b * 2 + 16;
  arena->cursor = ALIGN16(arena->cursor);
  l->f2c = arena->cursor;
  arena->cursor = ALIGN16(arena->cursor + l->f34 * 288);
  l->f38 = arena->cursor;
  arena->cursor = ALIGN16(arena->cursor + l->f40 * 16);
  l->f4c = cfg->c;
  l->f60 = cfg->c;
  arena->cursor = ALIGN16(arena->cursor);
  l->f44 = arena->cursor;
  arena->cursor = ALIGN16(arena->cursor + l->f4c * 76);
  l->f58 = arena->cursor;
  arena->cursor = arena->cursor + l->f60 * 96;
  l->f7c = 1;
}
