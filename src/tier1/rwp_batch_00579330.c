/* compile: /G5 */
#include "mfl_tier2.h"
#define ALIGN16(x) (((x) + 0xf) & 0xfffffff0)
typedef struct RwpArena { char pad[8]; unsigned cursor; } RwpArena;
typedef struct RwpBatchLayout { unsigned a, b, c, d; } RwpBatchLayout;
/* Carves the four batch arrays out of the arena (16 byte aligned) and stores their starts in the layout record. (Rwp library) */
void rwp_batch_00579330(RwpArena *arena, RwpBatchLayout *out, unsigned n, unsigned m, unsigned k)
{
  arena->cursor = ALIGN16(arena->cursor);
  out->a = arena->cursor;
  arena->cursor = ALIGN16(arena->cursor + n * 76);
  out->b = arena->cursor;
  arena->cursor = ALIGN16(arena->cursor + n * 4);
  out->c = arena->cursor;
  arena->cursor = ALIGN16(arena->cursor + k * 16);
  out->d = arena->cursor;
  arena->cursor = ALIGN16(arena->cursor + m * 4);
}
