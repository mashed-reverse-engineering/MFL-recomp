/* rwp_dynworld_build (0x00569ba0): build a dyn world inside a work arena (tables, space, bit sets, node pools) */
#include "mfl_tier2.h"
#include "rwp_common.h"

extern int rwp_capsule_005757e0(MflWorkArea *ar, int nbits, int n);
extern void *rwp_farfield_00575fa0(MflWorkArea *ar, void *graph, int nbits, int a2, float size);
extern int rwp_capsule_00575af0(void *p, int v, int n);

/* What a world is sized from (the same record rwp_dynworld_memory_size measures). */
typedef struct MflDynCfg {
  unsigned char pad00[8];
  int nbits;   /* 0x8: ids (bits of every set) */
  int n_links;   /* 0xc */
  int awake_max;   /* 0x10 */
  int n_nodes_a;   /* 0x14: 44 byte nodes */
  int n_nodes_b;   /* 0x18: 156 byte nodes */
  int graph_n;   /* 0x1c */
} MflDynCfg;

/* zero n bytes at the arena address a */
#define ZERO(a, n) rwp_capsule_00575af0((void *)(a), 0, (n))
#define ALIGN16(x) (((x) + 15) & ~15)
/* One row-table pair: a table of n * esz bytes carved from the arena, followed by the shared cursor cell at t + 0x58. */
#define PAIR(off, esz) \
  ar->cur = ALIGN16(ar->cur); \
  t[(off) / 4] = ar->cur; \
  ar->cur = ALIGN16(ar->cur + n * (esz)); \
  t[(off) / 4 + 1] = (int)t + 0x58

/* Builds a dyn world inside the work arena `ar` (16 byte aligned allocations, zeroed where needed): the world record, its row tables, the space (575fa0),
   the bit sets, the node pools with their free lists, the link chains and the id arrays.  Returns the world. */
/* TIER2: unvalidated 2026-11-10 authority=static */
MflDynWorld *rwp_dynworld_build(MflWorkArea *ar, MflDynCfg *cfg, int a2, float a3, int a4)
{
  MflDynWorld *w;
  int *t;
  unsigned int n, i, wb;
  int *cn, *nd;
  unsigned int off;

  w = (MflDynWorld *)ALIGN16(ar->cur);
  ar->cur = ALIGN16((unsigned int)w + 0x84);
  w->arg_c = *(int *)&a3;
  w->flags = a4;
  w->words = (cfg->nbits + 31) >> 5;
  t = (int *)ALIGN16(ar->cur);
  ar->cur = ALIGN16((unsigned int)t + 0x64);
  n = cfg->n_nodes_a;
  t[14] = n;
  t[15] = n;
  PAIR(0x00, 64);
  PAIR(0x08, 32);
  PAIR(0x10, 16);
  PAIR(0x40, 32);
  PAIR(0x50, 16);
  PAIR(0x30, 32);
  PAIR(0x48, 32);
  PAIR(0x20, 48);
  PAIR(0x28, 48);
  t[6] = ar->cur;
  ar->cur = ar->cur + n * 4;
  t[7] = (int)t + 0x58;
  t[22] = ar->cur;
  ar->cur = ar->cur + n * 4;
  t[23] = 0;
  t[24] = n;
  w->tables = t;
  w->graph = (void *)rwp_capsule_005757e0(ar, cfg->nbits, cfg->graph_n);
  w->space = rwp_farfield_00575fa0(ar, w->graph, cfg->nbits, a2, a3 * 0.01f);
  w->bits_used = (unsigned int *)ar->cur;
  ZERO(ar->cur, w->words * 4);
  ar->cur = ar->cur + w->words * 4;
  w->bits_a = (unsigned int *)ar->cur;
  ZERO(ar->cur, w->words * 4);
  ar->cur = ar->cur + w->words * 4;
  w->bits_c = (unsigned int *)ar->cur;
  ZERO(ar->cur, w->words * 4);
  ar->cur = ar->cur + w->words * 4;
  w->bits_awake = (unsigned int *)ar->cur;
  ZERO(ar->cur, w->words * 4);
  ar->cur = ar->cur + w->words * 4;
  w->bits_space = (unsigned int *)ar->cur;
  ZERO(ar->cur, w->words * 4);
  ar->cur = ar->cur + w->words * 4;
  for (i = 0; i < w->words; i++) {
    w->bits_used[i] = 0;
    w->bits_a[i] = 0;
    w->bits_c[i] = 0;
    w->bits_awake[i] = 0;
    w->bits_space[i] = 0;
  }
  w->seen = (unsigned int *)ar->cur;
  ar->cur = ar->cur + w->words * 4;
  w->seen2 = (unsigned int *)ar->cur;
  ar->cur = ar->cur + w->words * 4;
  w->added = (unsigned int *)ar->cur;
  ar->cur = ar->cur + w->words * 4;
  w->nodes_a = (char *)ar->cur;
  ZERO(ar->cur, cfg->n_nodes_a * 44);
  ar->cur = ar->cur + cfg->n_nodes_a * 44;
  w->free_a = (char **)ar->cur;
  ar->cur = ar->cur + cfg->n_nodes_a * 4;
  for (i = 0; i < (unsigned int)cfg->n_nodes_a; i++)
    w->free_a[i] = w->nodes_a + (cfg->n_nodes_a - i - 1) * 44;
  w->nfree_a = cfg->n_nodes_a;
  w->ncap_a = cfg->n_nodes_a;
  for (i = 0, off = 0; i < (unsigned int)cfg->n_nodes_a; i++, off += 0x2c) {
    nd = (int *)(w->nodes_a + off);
    nd[1] = i;
  }
  if (cfg->n_nodes_b != 0) {
    w->nodes_b = (char *)ar->cur;
    ZERO(ar->cur, cfg->n_nodes_b * 156);
    ar->cur = ar->cur + cfg->n_nodes_b * 156;
    w->free_b = (char **)ar->cur;
    ar->cur = ar->cur + cfg->n_nodes_b * 4;
    for (i = 0; i < (unsigned int)cfg->n_nodes_b; i++)
      w->free_b[i] = w->nodes_b + (cfg->n_nodes_b - i - 1) * 156;
    w->nfree_b = cfg->n_nodes_b;
    w->ncap_b = cfg->n_nodes_b;
  } else {
    w->ncap_b = 0;
    w->nfree_b = 0;
    w->nodes_b = 0;
    w->free_b = 0;
  }
  w->nlinks = cfg->n_links;
  if (cfg->n_links != 0) {
    w->bits_link = (unsigned int *)ar->cur;
    wb = ((cfg->n_links + 31) >> 5) * 4;
    ZERO(ar->cur, wb);
    ar->cur = ar->cur + wb;
    w->chain = (char *)ar->cur;
    ar->cur = ar->cur + cfg->n_links * 16;
    for (i = 0, off = 0; i < (unsigned int)(cfg->n_links * 2 - 1); i++, off += 8) {
      cn = (int *)(w->chain + off);
      cn[1] = (int)(w->chain + off + 8);
    }
    cn = (int *)(w->chain + off);
    cn[1] = 0;
    w->chain_head = w->chain;
    w->links = (MflDynNode **)ar->cur;
    ZERO(ar->cur, cfg->nbits * 4);
    ar->cur = ar->cur + cfg->nbits * 4;
  } else {
    w->bits_link = 0;
    w->links = 0;
    w->chain = 0;
  }
  w->idcap = cfg->nbits;
  w->awake_max = cfg->awake_max;
  w->awake_count = 0;
  w->bodies = (MflDynBody **)ar->cur;
  ZERO(ar->cur, cfg->nbits * 4);
  ar->cur = ar->cur + cfg->nbits * 4;
  w->ids = (unsigned short *)ar->cur;
  ar->cur = ar->cur + cfg->nbits * 2;
  w->near_ids = (unsigned short *)ar->cur;
  ar->cur = ar->cur + cfg->nbits * 2;
  return w;
}
