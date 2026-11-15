/* rwp_joint_matrix_columns_build (0x0057cc30): build the joint matrix products per list entry */
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
void rwp_joint_matrix_columns_build(float *dst, int a2, float *rows, int a4, int a5, float *src, RwpIndexList *list, float *mats, int a8, int a9)
{
  int i, b, k;
  float *m, *row, *d, *sr;
  float p[22], r[8];
  double w_sq, t2, t4, t6, DB, t9, EB, t12, FB, t15, GC, t18, HC, t21, IC, t24, DA, t27, ED, t30, EE, EE_DB, FE, FE_DC, GF, t37, t39, t41, IF, t44, t46, t48, HD, HD_GA, HE, t53, t55, IG, IG_HD_GA, IH, t60, II, t63, t65, t67, t69, t71, IG_HD_GA_saz, t73, t74, t75, t77, t78, t79, t80, t81, t83, t84, t85, t86, t87, t89, t91, t93, t95, t96, t97, raz_IG_HD_GA, t99, t101, t102, t103, t104, t105, t107, t108, t109, t110, t111;

  if (list->count == 0) return;
  i = 0;
  m = mats;
  row = rows;
  do {
    b = list->items[i];
    d = dst + 8 * b;
    sr = src + 8 * b;
    for (k = 0; k < 8; k++) r[k] = row[k];
    w_sq = m[3] * m[3];
    t2 = m[0] * m[0];
    p[1] = t2;
    t4 = m[0] * m[1];
    p[2] = t4;
    t6 = m[2] * m[0];
    p[3] = t6;
    DB = m[4] * m[1];
    t9 = p[1] + DB;
    p[4] = t9;
    EB = m[5] * m[1];
    t12 = EB + p[2];
    p[5] = t12;
    FB = m[6] * m[1];
    t15 = FB + p[3];
    p[6] = t15;
    GC = m[8] * m[2];
    t18 = p[4] + GC;
    p[7] = t18;
    HC = m[9] * m[2];
    t21 = HC + p[5];
    p[8] = t21;
    IC = m[10] * m[2];
    t24 = IC + p[6];
    p[9] = t24;
    DA = m[4] * m[0];
    t27 = m[4] * m[2];
    p[10] = t27;
    ED = m[5] * m[4];
    t30 = ED + DA;
    p[11] = t30;
    EE = m[5] * m[5];
    EE_DB = EE + DB;
    FE = m[6] * m[5];
    FE_DC = FE + p[10];
    GF = m[8] * m[6];
    t37 = GF + p[11];
    p[12] = t37;
    t39 = m[9] * m[6];
    p[13] = t39;
    t41 = t39 + EE_DB;
    p[14] = t41;
    IF = m[10] * m[6];
    t44 = IF + FE_DC;
    p[15] = t44;
    t46 = m[8] * m[0];
    p[16] = t46;
    t48 = m[8] * m[1];
    p[17] = t48;
    HD = m[9] * m[4];
    HD_GA = HD + p[16];
    HE = m[9] * m[5];
    t53 = HE + p[17];
    p[18] = t53;
    t55 = p[13] + GC;
    p[19] = t55;
    IG = m[10] * m[8];
    IG_HD_GA = IG + HD_GA;
    IH = m[10] * m[9];
    t60 = IH + p[18];
    p[20] = t60;
    II = m[10] * m[10];
    t63 = II + p[19];
    p[21] = t63;
    t65 = w_sq * sr[0];
    d[0] = t65;
    t67 = w_sq * sr[1];
    d[1] = t67;
    t69 = w_sq * sr[2];
    d[2] = t69;
    t71 = p[12] * sr[5];
    IG_HD_GA_saz = IG_HD_GA * sr[6];
    t73 = t71 + IG_HD_GA_saz;
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
    t89 = r[0] * w_sq;
    row[0] = t89;  /* [ebx-0x8] */
    t91 = r[1] * w_sq;
    row[1] = t91;  /* [ebx-0x24] */
    t93 = r[2] * w_sq;
    row[2] = t93;  /* [ebx-0x20] */
    t95 = r[5] * p[12];
    t96 = r[4] * p[7];
    t97 = t95 + t96;
    raz_IG_HD_GA = r[6] * IG_HD_GA;
    t99 = t97 + raz_IG_HD_GA;
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
