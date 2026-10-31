#include "mfl_tier2.h"

/* The simplex of the distance search (see rwp_aggchildren_0058a8f0): the table of dot products gram and the determinants lam
   of every vertex subset (the weights of the subset's closest point), plus the mask / slot / bit state. */
typedef struct MflSimplex {
  float vert[4][6];
  float gram[4][4];   /* 0x60 */
  float lam[16][4];   /* 0xa0 */
  unsigned char pad1a0[4];
  int mask;   /* 0x1a4 */
  int slot;   /* 0x1a8 */
  int bit;    /* 0x1ac */
} MflSimplex;

/* Updates the subset determinants for the vertex just added (slot, bit): the Johnson / Cramer recursion of the GJK distance
   subalgorithm.  With all four vertices in use (mask | bit == 15) the four weights of the full subset are summed up at the end. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_aggchildren_0058a580(MflSimplex *sx)
{
  double acc;
  int slot = sx->slot, bit = sx->bit;
  int i, j, bi, bj, m1, m2, m3;

  sx->lam[bit][slot] = 1.0f;
  for (bi = 1, i = 0; bi <= sx->mask; bi += bi, i++) {
    if (!(bi & sx->mask)) continue;
    m1 = bi | bit;
    sx->lam[m1][slot] = sx->gram[i][i] - sx->gram[i][slot];
    sx->lam[m1][i] = sx->gram[slot][slot] - sx->gram[slot][i];
    for (bj = 1, j = 0; bj <= sx->mask; bj += bj, j++) {
      if (!(bj & sx->mask)) continue;
      if (bj >= bi) break;
      m2 = bj | bi;
      m3 = m2 | bit;
      acc = (sx->gram[j][j] - sx->gram[j][slot]) * sx->lam[m2][j];
      acc = acc + (sx->gram[i][j] - sx->gram[i][slot]) * sx->lam[m2][i];
      sx->lam[m3][slot] = acc;
      acc = (sx->gram[j][j] - sx->gram[j][i]) * sx->lam[bj | bit][j];
      acc = acc + (sx->gram[slot][j] - sx->gram[slot][i]) * sx->lam[bj | bit][slot];
      sx->lam[m3][i] = acc;
      acc = (sx->gram[slot][i] - sx->gram[slot][j]) * sx->lam[m1][slot];
      acc = acc + (sx->gram[i][i] - sx->gram[i][j]) * sx->lam[m1][i];
      sx->lam[m3][j] = acc;
    }
  }
  if ((sx->mask | bit) == 15) {
    acc = (sx->gram[1][1] - sx->gram[1][0]) * sx->lam[14][1];
    acc = acc + (sx->gram[3][1] - sx->gram[3][0]) * sx->lam[14][3];
    acc = acc + (sx->gram[2][1] - sx->gram[2][0]) * sx->lam[14][2];
    sx->lam[15][0] = acc;
    acc = (sx->gram[0][0] - sx->gram[0][1]) * sx->lam[13][0];
    acc = acc + (sx->gram[3][0] - sx->gram[3][1]) * sx->lam[13][3];
    acc = acc + (sx->gram[2][0] - sx->gram[2][1]) * sx->lam[13][2];
    sx->lam[15][1] = acc;
    acc = (sx->gram[1][0] - sx->gram[1][2]) * sx->lam[11][1];
    acc = acc + (sx->gram[3][0] - sx->gram[3][2]) * sx->lam[11][3];
    acc = acc + (sx->gram[0][0] - sx->gram[0][2]) * sx->lam[11][0];
    sx->lam[15][2] = acc;
    acc = (sx->gram[1][0] - sx->gram[1][3]) * sx->lam[7][1];
    acc = acc + (sx->gram[2][0] - sx->gram[2][3]) * sx->lam[7][2];
    acc = acc + (sx->gram[0][0] - sx->gram[0][3]) * sx->lam[7][0];
    sx->lam[15][3] = acc;
  }
}
