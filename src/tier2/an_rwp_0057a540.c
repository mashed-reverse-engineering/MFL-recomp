/* rwp_solver_apply_multiplier_delta (0x0057a540): apply one multiplier change to the body accumulators of a constraint */
#include "mfl_tier2.h"

typedef struct MflRowPair {
  int a;
  int b;
} MflRowPair;
/* The solver tables the row functions take by value (34 dwords): 96 byte rows (six float4), the per-body ranges of rows
   (first row, count) and the row -> accumulator slot map. */
typedef struct MflRowSetByVal34 {
  float *rows;   /* 0x0: 96 byte records */
  int pad04[3];
  int *first;   /* 0x10: first row of body i */
  int pad14[2];
  int *slot;   /* 0x1c: accumulator (float4) index of row j */
  int pad20[8];
  int *count;   /* 0x40: number of rows of body i */
  int pad44[8];
  MflRowPair *bodies;   /* 0x64: (body a, body b) for jacobian group w = idx / 4 */
  int pad68[2];
  MflRowPair *sel;   /* 0x70: (row a, row b) for group w */
  int pad74[5];
} MflRowSetByVal34;

/* Applies one multiplier change `delta` of the constraint idx: the rows of the two bodies of the constraint (lane idx % 4 of each row) scaled by
   delta are accumulated into the float4 of every row of both bodies:  acc[slot[j]] += sum over the six float4 of row j (v[g] * row[g]),
   with v[g] = delta * selrow[g][lane].  The scalar version of 57ae50. */
/* TIER2: unvalidated 2026-10-31 authority=static */
void rwp_solver_apply_multiplier_delta(float *acc, int a2, int a3, MflRowSetByVal34 s, float delta, int idx)
{
  int w = idx / 4;
  int lane = idx % 4;
  int ba = s.bodies[w].a;
  int bb = s.bodies[w].b;
  float va[8], vb[8];
  float *row, *dst, *sr;
  double lane_x, lane_y, lane_z;
  float lane_w;
  int k;

  if (ba != -1) {
    sr = s.rows + s.sel[w].a * 24 + lane;
    va[0] = delta * sr[0];
    va[1] = delta * sr[4];
    va[2] = delta * sr[8];
    va[4] = delta * sr[12];
    va[5] = delta * sr[16];
    va[6] = delta * sr[20];
  }
  if (bb != -1) {
    sr = s.rows + s.sel[w].b * 24 + lane;
    vb[0] = delta * sr[0];
    vb[1] = delta * sr[4];
    vb[2] = delta * sr[8];
    vb[4] = delta * sr[12];
    vb[5] = delta * sr[16];
    vb[6] = delta * sr[20];
  }
  if (ba != -1 && s.count[ba] != 0) {
    for (k = 0; k != s.count[ba]; k++) {
      row = s.rows + (s.first[ba] + k) * 24;
      dst = acc + s.slot[s.first[ba] + k] * 4;
      lane_y = va[0] * row[1] + dst[1];
      lane_y = lane_y + va[1] * row[5];
      lane_y = lane_y + va[2] * row[9];
      lane_y = lane_y + va[4] * row[13];
      lane_y = lane_y + va[5] * row[17];
      lane_y = lane_y + va[6] * row[21];
      lane_z = va[0] * row[2] + dst[2];
      lane_z = lane_z + va[1] * row[6];
      lane_z = lane_z + va[2] * row[10];
      lane_z = lane_z + va[4] * row[14];
      lane_z = lane_z + va[5] * row[18];
      lane_z = lane_z + va[6] * row[22];
      lane_w = va[0] * row[3] + dst[3];
      lane_w = lane_w + va[1] * row[7];
      lane_w = lane_w + va[2] * row[11];
      lane_w = lane_w + va[4] * row[15];
      lane_w = lane_w + va[5] * row[19];
      lane_w = lane_w + va[6] * row[23];
      lane_x = va[0] * row[0] + dst[0];
      lane_x = lane_x + va[1] * row[4];
      lane_x = lane_x + va[2] * row[8];
      lane_x = lane_x + va[4] * row[12];
      lane_x = lane_x + va[5] * row[16];
      lane_x = lane_x + va[6] * row[20];
      dst[3] = lane_w;
      dst[0] = lane_x;
      dst[1] = lane_y;
      dst[2] = lane_z;
    }
  }
  if (bb != -1 && s.count[bb] != 0) {
    for (k = 0; k != s.count[bb]; k++) {
      row = s.rows + (s.first[bb] + k) * 24;
      dst = acc + s.slot[s.first[bb] + k] * 4;
      lane_y = vb[0] * row[1] + dst[1];
      lane_y = lane_y + vb[1] * row[5];
      lane_y = lane_y + vb[2] * row[9];
      lane_y = lane_y + vb[4] * row[13];
      lane_y = lane_y + vb[5] * row[17];
      lane_y = lane_y + vb[6] * row[21];
      lane_z = vb[0] * row[2] + dst[2];
      lane_z = lane_z + vb[1] * row[6];
      lane_z = lane_z + vb[2] * row[10];
      lane_z = lane_z + vb[4] * row[14];
      lane_z = lane_z + vb[5] * row[18];
      lane_z = lane_z + vb[6] * row[22];
      lane_w = vb[0] * row[3] + dst[3];
      lane_w = lane_w + vb[1] * row[7];
      lane_w = lane_w + vb[2] * row[11];
      lane_w = lane_w + vb[4] * row[15];
      lane_w = lane_w + vb[5] * row[19];
      lane_w = lane_w + vb[6] * row[23];
      lane_x = vb[0] * row[0] + dst[0];
      lane_x = lane_x + vb[1] * row[4];
      lane_x = lane_x + vb[2] * row[8];
      lane_x = lane_x + vb[4] * row[12];
      lane_x = lane_x + vb[5] * row[16];
      lane_x = lane_x + vb[6] * row[20];
      dst[3] = lane_w;
      dst[0] = lane_x;
      dst[1] = lane_y;
      dst[2] = lane_z;
    }
  }
}
