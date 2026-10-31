#include "mfl_tier2.h"

/* The constraint stream: constraint rows in a structure-of-arrays layout (rows of 16 words, one word per constraint in each of the
   scalar arrays), the per-block pair table, the word counts and the write cursor. */
typedef struct MflConstraintState {
  int (*rows)[16];   /* 0x0 */
  unsigned char pad04[0xc];
  int *a10;   /* 0x10 */
  unsigned char pad14[0x8];
  int *a1c;   /* 0x1c */
  unsigned char pad20[0x8];
  int *a28;   /* 0x28 */
  unsigned char pad2c[0x8];
  int *a34;   /* 0x34 */
  unsigned char pad38[0x8];
  int *a40;   /* 0x40 */
  unsigned char pad44[0x8];
  int *a4c;   /* 0x4c */
  unsigned char pad50[0x8];
  int *a58;   /* 0x58 */
  unsigned char pad5c[0x8];
  int *a64;   /* 0x64 */
  unsigned char pad68[0x20];
  int *a88;   /* 0x88 */
  unsigned char pad8c[0x8];
  int *a94;   /* 0x94 */
  unsigned char pad98[0x14];
  int (*pairs)[2];   /* 0xac */
  unsigned char padb0[0x8];
  int *words;   /* 0xb8 */
  unsigned char padbc[0x3c];
  int cursor;   /* 0xf8 */
  unsigned char padfc[0xc];
  int base;   /* 0x108 */
} MflConstraintState;

/* Stores one constraint (27 words) into the slot (base + words[cursor]) of the stream's arrays and counts it in words[cursor]. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_constraints_0057f8a0(MflConstraintState *s, int *c)
{
  int k, idx;
  int *row;
  int pair_second;
  int pair_first;
  int cur = s->cursor;

  k = s->base + s->words[cur];
  pair_first = s->pairs[cur][0];
  pair_second = s->pairs[cur][1];
  idx = (k & 3) + (k >> 2) * 4;
  s->a28[idx] = c[16];
  s->a1c[idx] = c[17];
  s->a64[idx] = c[18];
  s->a10[idx] = c[19];
  s->a34[idx] = c[20];
  s->a40[idx] = c[21];
  s->a4c[idx] = c[22];
  s->a58[idx] = c[23];
  s->a88[idx] = c[24];
  row = s->rows[idx];
  row[0] = c[0];
  row[1] = c[1];
  row[2] = c[2];
  row = s->rows[idx];
  row[4] = c[4];
  row[5] = c[5];
  row[6] = c[6];
  s->rows[idx][3] = pair_first;
  s->rows[idx][7] = s->cursor;
  row = s->rows[idx];
  row[8] = c[8];
  row[9] = c[9];
  row[10] = c[10];
  row = s->rows[idx];
  row[12] = c[12];
  row[13] = c[13];
  row[14] = c[14];
  s->rows[idx][11] = pair_second;
  s->rows[idx][15] = s->cursor;
  s->a94[idx] = c[26];
  s->words[s->cursor] = s->words[s->cursor] + 1;
}
