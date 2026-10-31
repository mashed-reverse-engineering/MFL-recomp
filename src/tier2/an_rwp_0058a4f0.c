#include "mfl_tier2.h"

/* The simplex of the distance search (see rwp_aggchildren_0058a8f0): vertices of 6 floats (difference first), the table of
   their dot products and the mask / slot / bit state. */
typedef struct MflSimplex {
  float vert[4][6];
  float gram[4][4];   /* 0x60 */
  unsigned char pada0[0x104];
  int mask;   /* 0x1a4 */
  int slot;   /* 0x1a8 */
  int bit;    /* 0x1ac */
} MflSimplex;

/* Adds the dot products of the newly added vertex (sx->slot) with every vertex of the subset to the symmetric table gram. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_aggchildren_0058a4f0(MflSimplex *sx)
{
  double acc;
  int b, i = 0;
  float *v = sx->vert[0];

  for (b = 1; b <= (sx->mask | sx->bit); b += b) {
    if (b & (sx->mask | sx->bit)) {
      acc = v[1] * sx->vert[sx->slot][1];
      acc = acc + sx->vert[sx->slot][0] * v[0];
      acc = acc + sx->vert[sx->slot][2] * v[2];
      sx->gram[i][sx->slot] = acc;
      *(int *)&sx->gram[sx->slot][i] = *(int *)&sx->gram[i][sx->slot];
    }
    i++;
    v += 6;
  }
}
