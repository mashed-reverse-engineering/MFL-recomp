/* rwp_farfield_00575fa0 (0x00575fa0): create the farfield spatial index of a dyn world (called once from the world builder 0x569ba0) */
#include "mfl_tier2.h"

/* Farfield block (0xc01c bytes, layout as in an_rwp_00576c50.c): world box at +0x7fe0 (first corner) / +0x7ff0 (second corner), object count
   at +0x801c, 4 byte list records at +0x881e (u16 next at +2), 20 byte cells at +0x981c (dword 0 = list head / free link, 8 u16 slot lists at +4),
   free record head +0x981a, free cell head +0xc00e, edge pool +0xc010, margin +0xc014, +0xc018 cleared, per object u16 at +0xb810. */
typedef struct MflFfArena {
  unsigned char pad00[8];
  unsigned char *next;   /* 0x8: bump pointer, no alignment for this block */
} MflFfArena;

#define FF_U32(f, off) (*(unsigned int *)((f) + (off)))
#define FF_U16(f, off) (*(unsigned short *)((f) + (off)))
#define FF_CELLS 0x198      /* cells 1..0x197 on the free list, 0x198 ends it */
#define FF_RECS 0x3ff       /* records 0..0x3fe on the free list */
#define FF_NONE 0x3ff

/* Takes the block from the arena, stores the world box (copied as raw words), the edge pool and the object count, threads the free cell list
   (1 -> 2 -> ... -> 0x198, end 0x3ff) and the free record list (0 -> 1 -> ... -> 0x3fe, end 0x3ff), makes cell 0 the empty root (no list,
   8 empty flagged slots 0x83ff), clears the per object words and stores the margin.  Returns the block. */
/* TIER2: unvalidated 2026-11-13 authority=diffemu */
unsigned char *rwp_farfield_00575fa0(MflFfArena *ar, void *graph, unsigned int nobj, const unsigned int *box, unsigned int margin_bits)
{
  unsigned char *f = ar->next;
  unsigned int i;

  ar->next = f + 0xc01c;
  FF_U32(f, 0x7ff0) = box[4];
  FF_U32(f, 0x7ff4) = box[5];
  FF_U32(f, 0x7ff8) = box[6];
  FF_U32(f, 0x7fe0) = box[0];
  FF_U32(f, 0x7fe4) = box[1];
  FF_U32(f, 0x7fe8) = box[2];
  *(void **)(f + 0xc010) = graph;
  FF_U32(f, 0x801c) = nobj;
  for (i = 1; i < FF_CELLS; i++) FF_U32(f, 0x981c + 0x14 * i) = i + 1;
  FF_U32(f, 0x981c + 0x14 * FF_CELLS) = FF_NONE;
  FF_U16(f, 0xc00e) = 1;
  for (i = 0; i < FF_RECS - 1; i++) FF_U16(f, 0x8820 + 4 * i) = (unsigned short)(i + 1);
  FF_U16(f, 0x8820 + 4 * i) = FF_NONE;
  FF_U16(f, 0x981a) = 0;
  FF_U32(f, 0x981c) = FF_NONE;
  for (i = 0; i < 8; i++) FF_U16(f, 0x9820 + 2 * i) = 0x83ff;
  for (i = 0; i < nobj; i++) FF_U16(f, 0xb810 + 2 * i) = 0;
  FF_U32(f, 0xc018) = 0;
  FF_U32(f, 0xc014) = margin_bits;      /* the float margin, as raw bits */
  return f;
}
