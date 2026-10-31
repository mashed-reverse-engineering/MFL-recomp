#include "mfl_tier2.h"

typedef struct RwpIndexList {
  int *items;
  int count;
} RwpIndexList;

/* Per list entry i (body b = list->items[i]): m = the 3x4 matrix of entry i of mats (rows A B C w / D E F . / G H I .), w = m[3].
   Products of the 3x3 block with its own transpose-like mix are accumulated into the six "columns" p[] (the original spills every partial sum to the frame as a
   float, hence the float array, one slot per stored value) and into x87 temporaries (double t..).  dst row b (8 floats): [0..2] = w^2 * src row, [4..6] = the
   transform of src[4..6]; rows[i] (8 floats) is scaled by w^2 and its [4..6] transformed the same way, in place.  r[] is the rep movs copy of rows[i]. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_batch_0057cc30(float *dst, int a2, float *rows, int a4, int a5, float *src, RwpIndexList *list, float *mats)
{
  int i, b, k;
  float *m, *row, *d, *sr;
  float p[22], r[8];
  double t1, t2, t4, t6, t8, t9, t11, t12, t14, t15, t17, t18, t20, t21, t23, t24, t26, t27, t29, t30, t32, t33, t34, t35, t36, t37, t39, t41, t43, t44, t46, t48, t50, t51, t52, t53, t55, t57, t58, t59, t60, t62, t63, t65, t67, t69, t71, t72, t73, t74, t75, t77, t78, t79, t80, t81, t83, t84, t85, t86, t87, t89, t91, t93, t95, t96, t97, t98, t99, t101, t102, t103, t104, t105, t107, t108, t109, t110, t111;

  if (list->count == 0) return;
  i = 0;
  m = mats;
  row = rows;
  do {
    b = list->items[i];
    d = dst + 8 * b;
    sr = src + 8 * b;
    for (k = 0; k < 8; k++) r[k] = row[k];
    t1 = m[3] * m[3];
    t2 = m[0] * m[0];
    p[1] = t2;
    t4 = m[0] * m[1];
    p[2] = t4;
    t6 = m[2] * m[0];
    p[3] = t6;
    t8 = m[4] * m[1];
    t9 = p[1] + t8;
    p[4] = t9;
    t11 = m[5] * m[1];
    t12 = t11 + p[2];
    p[5] = t12;
    t14 = m[6] * m[1];
    t15 = t14 + p[3];
    p[6] = t15;
    t17 = m[8] * m[2];
    t18 = p[4] + t17;
    p[7] = t18;
    t20 = m[9] * m[2];
    t21 = t20 + p[5];
    p[8] = t21;
    t23 = m[10] * m[2];
    t24 = t23 + p[6];
    p[9] = t24;
    t26 = m[4] * m[0];
    t27 = m[4] * m[2];
    p[10] = t27;
    t29 = m[5] * m[4];
    t30 = t29 + t26;
    p[11] = t30;
    t32 = m[5] * m[5];
    t33 = t32 + t8;
    t34 = m[6] * m[5];
    t35 = t34 + p[10];
    t36 = m[8] * m[6];
    t37 = t36 + p[11];
    p[12] = t37;
    t39 = m[9] * m[6];
    p[13] = t39;
    t41 = t39 + t33;
    p[14] = t41;
    t43 = m[10] * m[6];
    t44 = t43 + t35;
    p[15] = t44;
    t46 = m[8] * m[0];
    p[16] = t46;
    t48 = m[8] * m[1];
    p[17] = t48;
    t50 = m[9] * m[4];
    t51 = t50 + p[16];
    t52 = m[9] * m[5];
    t53 = t52 + p[17];
    p[18] = t53;
    t55 = p[13] + t17;
    p[19] = t55;
    t57 = m[10] * m[8];
    t58 = t57 + t51;
    t59 = m[10] * m[9];
    t60 = t59 + p[18];
    p[20] = t60;
    t62 = m[10] * m[10];
    t63 = t62 + p[19];
    p[21] = t63;
    t65 = t1 * sr[0];
    d[0] = t65;
    t67 = t1 * sr[1];
    d[1] = t67;
    t69 = t1 * sr[2];
    d[2] = t69;
    t71 = p[12] * sr[5];
    t72 = t58 * sr[6];
    t73 = t71 + t72;
    t74 = p[7] * sr[4];
    t75 = t73 + t74;
    d[4] = t75;
    t77 = p[14] * sr[5];
    t78 = p[20] * sr[6];
    t79 = t77 + t78;
    t80 = p[8] * sr[4];
    t81 = t79 + t80;
    d[5] = t81;
    t83 = p[15] * sr[5];
    t84 = p[21] * sr[6];
    t85 = t83 + t84;
    t86 = p[9] * sr[4];
    t87 = t85 + t86;
    d[6] = t87;
    t89 = r[0] * t1;
    row[0] = t89;  /* [ebx-0x8] */
    t91 = r[1] * t1;
    row[1] = t91;  /* [ebx-0x24] */
    t93 = r[2] * t1;
    row[2] = t93;  /* [ebx-0x20] */
    t95 = r[5] * p[12];
    t96 = r[4] * p[7];
    t97 = t95 + t96;
    t98 = r[6] * t58;
    t99 = t97 + t98;
    row[4] = t99;  /* [ebx-0x18] */
    t101 = r[5] * p[14];
    t102 = r[4] * p[8];
    t103 = t101 + t102;
    t104 = r[6] * p[20];
    t105 = t103 + t104;
    row[5] = t105;  /* [ebx-0x14] */
    t107 = r[5] * p[15];
    t108 = r[4] * p[9];
    t109 = t107 + t108;
    t110 = r[6] * p[21];
    t111 = t109 + t110;
    row[6] = t111;  /* [ebx-0x10] */
    i++;
    m += 12;
    row += 8;
  } while (i != list->count);
}
