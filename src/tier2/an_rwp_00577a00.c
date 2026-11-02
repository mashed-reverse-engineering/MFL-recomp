/* rwp_solver_work_carve_step1 (0x00577a00): carve the work arrays of the first solver step out of the work area */
#include "mfl_tier2.h"
#include "rwp_common.h"

/* The work area cursor of the solver (the next free byte is at 0x8) and the pointer to the block carved for one step. */
/* Arrays of the first solver table set: pointers into the block (0 when it did not fit) and the element strides (bytes). */
typedef struct MflStepTables {
  char *p00;   /* 0x00 */
  char *p04;   /* 0x04 */
  unsigned char pad08[4];
  unsigned int s0c;   /* 0x0c */
  char *p10;   /* 0x10 */
  unsigned char pad14[4];
  unsigned int s18;   /* 0x18 */
  char *p1c;   /* 0x1c */
  unsigned char pad20[4];
  unsigned int s24;   /* 0x24 */
  char *p28;   /* 0x28 */
  unsigned char pad2c[4];
  unsigned int s30;   /* 0x30 */
  char *p34;   /* 0x34 */
  unsigned char pad38[4];
  unsigned int s3c;   /* 0x3c */
  char *p40;   /* 0x40 */
  unsigned char pad44[4];
  unsigned int s48;   /* 0x48 */
  char *p4c;   /* 0x4c */
  unsigned char pad50[4];
  unsigned int s54;   /* 0x54 */
  unsigned char pad58[0xc];
  char *p64;   /* 0x64 */
  unsigned char pad68[4];
  unsigned int s6c;   /* 0x6c */
  char *p70;   /* 0x70 */
  unsigned char pad74[4];
  unsigned int s78;   /* 0x78 */
  unsigned char pad7c[0xc];
  char *p88;   /* 0x88 */
  unsigned int s8c;   /* 0x8c */
  unsigned int s90;   /* 0x90 */
  char *p94;   /* 0x94 */
  unsigned char pad98[4];
  unsigned int s9c;   /* 0x9c */
} MflStepTables;
/* The second set (two arrays). */
typedef struct MflStepTables2 {
  char *p00;   /* 0x00 */
  unsigned char pad04[4];
  unsigned int s08;   /* 0x08 */
  char *p0c;   /* 0x0c */
  unsigned char pad10[4];
  unsigned int s14;   /* 0x14 */
} MflStepTables2;

#define align16(x) (((x) + 15) & ~15)

/* Carves the work arrays of one solver step out of the work area: n5 rows, n6 / n7 element counts.  Every array takes
   its size rounded up to 16 bytes in turn; an array that would pass the total reserved size is left 0.  The strides of the
   arrays are stored next to the pointers. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_solver_work_carve_step1(MflWorkArea *area, MflStepTables2 *t2, MflStepTables *t, MflWorkBase *wb, int n5, int n6, int n7)
{
  int q = n5 / 4;
  int a = align16(q * 8);
  int e1 = align16(q * 16);
  int c = align16(n6 * 4);
  int d = align16(n7 * 4);
  int f = align16((n5 / 3) * 64);
  int g = align16(q * 192);
  int total = e1 + f * 2 + e1 * 4 + a * 2 + d + c * 4 + a * 2 + g;
  int off;
  unsigned int cur;
  char *p;

  cur = align16(area->cur);
  area->cur = cur;
  wb->base = (char *)cur;
  area->cur = align16(area->cur + total);
  off = 0;
  p = 0;
  if (g <= total) { p = wb->base; off = g; }
  t->p00 = p;
  p = 0;
  if (off + a <= total) { p = wb->base + off; off += a; }
  t->p1c = p;
  p = 0;
  if (off + a <= total) { p = wb->base + off; off += a; }
  t->p28 = p;
  p = 0;
  if (off + c <= total) { p = wb->base + off; off += c; }
  t->p34 = p;
  p = 0;
  if (off + c <= total) { p = wb->base + off; off += c; }
  t->p10 = p;
  p = 0;
  if (off + c <= total) { p = wb->base + off; off += c; }
  t->p40 = p;
  p = 0;
  if (off + c <= total) { p = wb->base + off; off += c; }
  t->p04 = p;
  p = 0;
  if (off + d <= total) { p = wb->base + off; off += d; }
  t->p4c = p;
  p = 0;
  if (off + a <= total) { p = wb->base + off; off += a; }
  t->p70 = p;
  p = 0;
  if (off + a <= total) { p = wb->base + off; off += a; }
  t->p64 = p;
  p = 0;
  if (off + e1 <= total) { p = wb->base + off; off += e1; }
  t->p88 = p;
  p = 0;
  if (off + e1 <= total) { p = wb->base + off; off += e1; }
  t->p94 = p;
  p = 0;
  if (off + e1 <= total) { p = wb->base + off; off += e1; }
  t2->p00 = p;
  p = 0;
  if (off + e1 <= total) { p = wb->base + off; off += e1; }
  t2->p0c = p;
  t->s24 = t->s30 = align16(q * 2);
  t->s3c = t->s18 = t->s48 = t->s0c = align16(n6);
  t->s54 = align16(n7);
  t->s78 = t->s6c = align16(q);
  t->s90 = t->s9c = t2->s08 = t2->s14 = align16(n5);
}
