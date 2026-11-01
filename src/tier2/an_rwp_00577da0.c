#include "mfl_tier2.h"
#include "rwp_common.h"

/* Work area cursor (next free byte at 0x8) and the base of the block carved for the step (see 577a00). */
/* Nine array descriptors of 12 bytes: pointer, a zero word and the element stride. */
typedef struct MflArraySlot {
  char *ptr;
  int zero;   /* 0x4 */
  int stride;   /* 0x8 */
} MflArraySlot;
typedef struct MflStepArrays2 {
  MflArraySlot s[9];
} MflStepArrays2;
/* The second table set: pointers into the block (0 when they did not fit) with the element counts / strides behind them. */
typedef struct MflStepArrays3 {
  char *p000;
  unsigned char pad004[8];
  int n00c;
  char *p010;
  unsigned char pad014[4];
  int n018;
  char *p01c;
  unsigned char pad020[4];
  int n024;
  char *p028;
  unsigned char pad02c[4];
  int n030;
  char *p034;
  unsigned char pad038[4];
  int n03c;
  char *p040;
  unsigned char pad044[4];
  int n048;
  char *p04c;
  unsigned char pad050[4];
  int n054;
  char *p058;
  unsigned char pad05c[4];
  int n060;
  char *p064;
  unsigned char pad068[4];
  int n06c;
  char *p070;
  unsigned char pad074[4];
  int n078;
  char *p07c;
  unsigned char pad080[4];
  int n084;
  char *p088;
  unsigned char pad08c[4];
  int n090;
  char *p094;
  unsigned char pad098[4];
  int n09c;
  char *p0a0;
  unsigned char pad0a4[4];
  int n0a8;
  char *p0ac;
  unsigned char pad0b0[4];
  int n0b4;
  char *p0b8;
  unsigned char pad0bc[4];
  int n0c0;
  char *p0c4;
  unsigned char pad0c8[4];
  int n0cc;
  char *p0d0;
  unsigned char pad0d4[4];
  int n0d8;
  char *p0dc;
  unsigned char pad0e0[4];
  int n0e4;
  char *p0e8;
  unsigned char pad0ec[4];
  int n0f0;
  unsigned char pad0f4[24];
  char *p10c;
  unsigned char pad110[4];
  int n114;
  char *p118;
  unsigned char pad11c[4];
  int n120;
} MflStepArrays3;

#define al16(x) (((x) + 15) & ~15)
/* Takes the next `size` bytes of the block for dst: the pointer when it still fits into the reserved total, else 0. */
#define CARVE(dst, size) p = 0; if (off + (size) <= total) { p = wb->base + off; off += (size); } dst = p;

/* Carves the work arrays of the second solver step out of the work area: a5 rows, a6 / a7 / a8 element counts.  Like 577a00, every array takes its size
   rounded up to 16 bytes in turn, an array that would pass the total reserved size is left 0, and the strides / counts are stored next to the pointers. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_batch_00577da0(MflWorkArea *area, MflStepArrays2 *t2, MflStepArrays3 *t3, MflWorkBase *wb, int a5, int a6, int a7, int a8)
{
  int q = a5 / 4;
  int A = al16(a5 * 4);
  int B = al16(a5 * 64);
  int C = al16(a6 * 32);
  int D = al16(a6 * 48);
  int E = al16(a7 * 4);
  int G = al16(a7 * 8);
  int H = al16(a7 * 64);
  int I = al16(a8 * 4);
  int J = al16(q * 16);
  int K = al16((a5 / 3) * 4);
  int total = 14 * A + B + 2 * C + 2 * D + 2 * E + G + H + 6 * I + 2 * J + K;
  int off;
  unsigned int cur;
  char *p;

  cur = al16(area->cur);
  area->cur = cur;
  wb->base = (char *)cur;
  area->cur = al16(area->cur + total);
  off = 0;

  CARVE(t2->s[2].ptr, A)
  CARVE(t2->s[3].ptr, A)
  CARVE(t2->s[4].ptr, C)
  CARVE(t2->s[6].ptr, H)
  CARVE(t2->s[5].ptr, E)
  CARVE(t2->s[1].ptr, J)
  CARVE(t2->s[0].ptr, J)
  CARVE(t2->s[7].ptr, I)
  CARVE(t2->s[8].ptr, I)
  t2->s[4].stride = a6;
  t2->s[6].stride = a7;
  t2->s[3].stride = al16(a5);
  t2->s[2].stride = al16(a5);
  t2->s[1].stride = al16(a5);
  t2->s[0].stride = al16(a5);
  t2->s[5].stride = a7;
  t2->s[7].stride = a8;
  t2->s[8].stride = a8;
  t2->s[3].zero = 0;
  t2->s[2].zero = 0;
  t2->s[4].zero = 0;
  t2->s[6].zero = 0;
  t2->s[5].zero = 0;
  t2->s[1].zero = 0;
  t2->s[0].zero = 0;
  t2->s[7].zero = 0;
  t2->s[8].zero = 0;
  CARVE(t3->p118, D)
  CARVE(t3->p10c, C)
  CARVE(t3->p000, B)
  CARVE(t3->p010, A)
  CARVE(t3->p01c, A)
  CARVE(t3->p028, A)
  CARVE(t3->p034, A)
  CARVE(t3->p040, A)
  CARVE(t3->p04c, A)
  CARVE(t3->p058, A)
  CARVE(t3->p088, A)
  CARVE(t3->p064, A)
  CARVE(t3->p070, A)
  CARVE(t3->p07c, A)
  CARVE(t3->p094, A)
  CARVE(t3->p0a0, K)
  CARVE(t3->p0ac, G)
  CARVE(t3->p0b8, E)
  CARVE(t3->p0c4, I)
  CARVE(t3->p0d0, I)
  CARVE(t3->p0dc, I)
  t3->n120 = a6;
  t3->n114 = a6;
  CARVE(t3->p0e8, I)
  t3->n00c = a5;
  t3->n018 = a5;
  t3->n024 = a5;
  t3->n030 = a5;
  t3->n03c = a5;
  t3->n048 = a5;
  t3->n054 = a5;
  t3->n060 = a5;
  t3->n090 = a5;
  t3->n06c = a5;
  t3->n078 = a5;
  t3->n084 = a5;
  t3->n09c = a5;
  t3->n0b4 = a7;
  t3->n0c0 = a7;
  t3->n0a8 = (a5 / 3);
  t3->n0cc = a8;
  t3->n0d8 = a8;
  t3->n0e4 = a8;
  t3->n0f0 = a8;
}
