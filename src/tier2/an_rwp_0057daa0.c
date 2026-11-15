#include "mfl_tier2.h"
#include "rwp_common.h"
#include <math.h>

extern void rwp_batch_0057da00(char **table, float *out, char *m, int a, int b, int c);

/* rwp_batch_0057daa0: normalises one row of 4 constraint vertices per index.  Row j of the cell table (8 cells of 32 bytes: 4 vertices x 2 bodies)
   gets the squared lengths g[b] of the vertex b summed over both bodies (57da00 builds the world space cell of a body record, 0x30 bytes at
   bodies + 0x30 * index, index in the pad word of the cell, -1 = none) plus w times the 4th column of q16.  With r = 1 / sqrt(g) and
   s = g * r = |g|^0.5: the cells are scaled by r, the sentinel padded limit arrays lim_a / lim_b / lim_c / lim_d (-FLT_MAX / FLT_MAX = unlimited) by s and
   q16 by r^2, q19 / q22 by r. */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
void rwp_batch_0057daa0(char *cells, int a1, int a2, int a3, float *lim_a, int a5, int a6, float *lim_b, int a8, int a9, float *lim_c, int a11, int a12,
                        float *lim_d, int a14, int a15, float *q16, int a17, int a18, float *q19, int a20, int a21, float *q22, int count, int a24,
                        char *bodies, int a26, int a27, float w)
{
  int j, k, b, idx;
  char *row;
  float *c;
  float g[4], s[4], out[8];
  volatile float g_orig;
  double acc, g_w, r_w;

  if (count == 0) return;
  row = cells;
  for (j = 0; j != count; j++) {
    g[0] = 0.0f;
    g[1] = 0.0f;
    g[2] = 0.0f;
    g[3] = 0.0f;
    s[0] = 0.0f;
    s[1] = 0.0f;
    s[2] = 0.0f;
    s[3] = 0.0f;
    idx = *(int *)(row + 0xc);
    if (idx != -1) {
      for (b = 0; b < 4; b++) {
        rwp_batch_0057da00(&cells, out, bodies + idx * 0x30, j, b, 0);
        acc = out[6] * out[6];
        acc = acc + out[5] * out[5];
        acc = acc + out[4] * out[4];
        acc = acc + out[2] * out[2];
        acc = acc + out[1] * out[1];
        acc = acc + out[0] * out[0];
        g[b] = acc;
      }
    }
    idx = *(int *)(row + 0x2c);
    if (idx != -1) {
      for (b = 0; b < 4; b++) {
        rwp_batch_0057da00(&cells, out, bodies + idx * 0x30, j, b, 1);
        acc = out[6] * out[6];
        acc = acc + out[5] * out[5];
        acc = acc + out[4] * out[4];
        acc = acc + out[2] * out[2];
        acc = acc + out[1] * out[1];
        acc = acc + out[0] * out[0];
        g[b] = acc + g[b];
      }
    }
    g[0] = w * q16[j * 4] + g[0];
    g[1] = w * q16[j * 4 + 1] + g[1];
    g[2] = w * q16[j * 4 + 2] + g[2];
    g_w = w * q16[j * 4 + 3] + g[3];
    for (b = 0; b < 3; b++) {
      if (g[b] != 0.0f) {
        g_orig = g[b];
        g[b] = 1.0f / sqrt(g[b]);
        s[b] = g_orig * g[b];
      }
    }
    r_w = g_w;
    if (g_w != 0.0f) {
      g_orig = g_w;
      r_w = 1.0f / sqrt(g_w);
      s[3] = g_orig * r_w;
    }
    for (k = 0; k < 2; k++) {
      for (b = 0; b < 3; b++) {
        c = (float *)(row + 0x40 * b + 0x20 * k);
        c[0] = g[b] * c[0];
        c[1] = g[b] * c[1];
        c[2] = g[b] * c[2];
        c[4] = g[b] * c[4];
        c[5] = g[b] * c[5];
        c[6] = g[b] * c[6];
      }
      c = (float *)(row + 0xc0 + 0x20 * k);
      c[0] = r_w * c[0];
      c[1] = r_w * c[1];
      c[2] = r_w * c[2];
      c[4] = r_w * c[4];
      c[5] = r_w * c[5];
      c[6] = r_w * c[6];
    }
    for (b = 0; b < 4; b++)
      if (((unsigned *)lim_a)[j * 4 + b] != 0xff7fffff) lim_a[j * 4 + b] = s[b] * lim_a[j * 4 + b];
    for (b = 0; b < 4; b++)
      if (((unsigned *)lim_b)[j * 4 + b] != 0x7f7fffff) lim_b[j * 4 + b] = s[b] * lim_b[j * 4 + b];
    for (b = 0; b < 4; b++)
      if (((unsigned *)lim_c)[j * 4 + b] != 0xff7fffff) lim_c[j * 4 + b] = s[b] * lim_c[j * 4 + b];
    for (b = 0; b < 4; b++)
      if (((unsigned *)lim_d)[j * 4 + b] != 0x7f7fffff) lim_d[j * 4 + b] = s[b] * lim_d[j * 4 + b];
    q16[j * 4] = (g[0] * q16[j * 4]) * g[0];
    q16[j * 4 + 1] = (g[1] * g[1]) * q16[j * 4 + 1];
    q16[j * 4 + 2] = (g[2] * q16[j * 4 + 2]) * g[2];
    q16[j * 4 + 3] = (r_w * q16[j * 4 + 3]) * r_w;
    q19[j * 4] = g[0] * q19[j * 4];
    q19[j * 4 + 1] = g[1] * q19[j * 4 + 1];
    q19[j * 4 + 2] = g[2] * q19[j * 4 + 2];
    q19[j * 4 + 3] = r_w * q19[j * 4 + 3];
    q22[j * 4] = g[0] * q22[j * 4];
    q22[j * 4 + 1] = g[1] * q22[j * 4 + 1];
    q22[j * 4 + 2] = g[2] * q22[j * 4 + 2];
    q22[j * 4 + 3] = r_w * q22[j * 4 + 3];
    row += 0x100;
  }
}
