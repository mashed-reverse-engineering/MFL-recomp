/* compile: /G5 */
typedef struct RwpEngine { char pad[0x108]; void *(*alloc)(unsigned size, int flags); } RwpEngine;
extern RwpEngine *RwEngineInstance;
typedef struct { int a, b, c; } RwpCell;
typedef struct RwpPool {
  int pad0; int f4; int f8; unsigned short s0c; unsigned short cap;
  RwpCell *cells; unsigned short s14; unsigned short s16; unsigned *links; int f1c;
  unsigned short s20; unsigned short pad22; int f24; int f28; int f2c;
} RwpPool;
/* Initialises a pool: ncells zeroed 12-byte cells and an (uninitialised) array of nlinks 4-byte links, both allocated from the engine heap (flags 0x30900); the 16-bit loop index selects the 16-bit compare. */
RwpPool *rwp_obj_0056e490(RwpPool *p, unsigned short ncells, unsigned short nlinks)
{
  unsigned short i;
  p->s20 = 0xffff;
  p->f4 = 0;
  p->f24 = 0;
  p->f8 = 0;
  p->f2c = 0;
  p->cap = ncells;
  p->cells = RwEngineInstance->alloc(ncells * sizeof(RwpCell), 0x30900);
  p->s0c = 0;
  p->f1c = 0;
  for (i = 0; i < ncells; i++) {
    p->cells[i].b = 0;
    p->cells[i].c = 0;
    p->cells[i].a = 0;
  }
  p->s14 = 0;
  p->s16 = nlinks;
  p->f28 = 0;
  if (nlinks) p->links = RwEngineInstance->alloc(nlinks * 4, 0x30900);
  else p->links = 0;
  return p;
}
