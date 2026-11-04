#include "mfl_tier2.h"
#include "rwp_farfield.h"

/* Inserts object id with bounds `box` into the farfield index.  The object's box grown by the margin is stored, the pairs it forms with the
   objects already in the index are reported (5760b0), then the cell tree is descended from the root: while the box lies in one octant of the cell
   (575d00) the walk follows the octant's child cell; a leaf octant takes the record on its list and, once its count exceeds 3, is split into a
   new child cell (the flagged records are redistributed into the child's octants, the others go on the child's own list).  An object that
   straddles the centre of a cell is kept on that cell's own list.  Returns the index block. */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
unsigned char *rwp_farfield_00576670(unsigned char *f, unsigned int id, MflFfBox *box)
{
  MflFfBox *e = FF_E(f, id & 0xffff);
  MflFfBox cell, sub;
  unsigned int ci = 0, sel = 0xffff, r, nc, cur, nxt, o, sel2, hd, L;
  unsigned short w, *p;
  int k;

  e->min[0] = box->min[0] - FF_MARGIN(f);
  e->min[1] = box->min[1] - FF_MARGIN(f);
  e->min[2] = box->min[2] - FF_MARGIN(f);
  e->max[0] = FF_MARGIN(f) + box->max[0];
  e->max[1] = box->max[1] + FF_MARGIN(f);
  e->max[2] = box->max[2] + FF_MARGIN(f);
  rwp_farfield_005760b0(f, e, id);
  cell = *FF_E(f, 0x3ff);
  if (!(e->min[0] >= cell.min[0]) || !(e->max[0] <= cell.max[0]) || !(e->min[1] >= cell.min[1]) || !(e->max[1] <= cell.max[1])
      || !(e->min[2] >= cell.min[2]) || !(e->max[2] <= cell.max[2])) goto own_list;      /* an object outside the world stays on the root's list */
  sel = rwp_farfield_00575d00(&sub, e, &cell);
  if ((unsigned short)sel == 0xffff) goto own_list;
  for (;;) {
    cell = sub;
    k = (ci & 0xffff) * 10 + (sel & 0xffff);
    w = *(unsigned short *)(f + 0x9820 + k * 2);      /* FF_SLOT(f, ci, sel) */
    if (w & 0x8000) {
      if ((unsigned short)sel != 0xffff) goto leaf;
      break;
    }
    /* The original keeps cell numbers in the low half of a 32-bit register (`mov ax, word ptr [...]`); the high half left over from the slot
       index (k) travels with it and reaches the callees' unsigned short argument slots below (they mask it).  Kept so the call arguments are
       bit-identical. */
    ci = (k & 0xffff0000) | w;
    sel = rwp_farfield_00575d00(&sub, e, &cell);
    if ((unsigned short)sel == 0xffff) break;
  }
own_list:
  /* the record goes to the head of the cell's own list */
  r = FF_FREEREC(f);
  FF_FREEREC(f) = FF_NEXT(f, r);
  FF_OWNER(f, r) = (unsigned short)((FF_OWNER(f, r) & 0xfc00) | id);
  FF_NEXT(f, r) = FF_HEAD(f, ci & 0xffff) & 0x3ff;
  FF_HEAD(f, ci & 0xffff) = (FF_HEAD(f, ci & 0xffff) & 0xfffffc00) | r;
  FF_TAG(f, id & 0xffff) = (sel & 0x3f) | (ci << 6);
  return f;

leaf:
  r = FF_FREEREC(f);
  FF_FREEREC(f) = FF_NEXT(f, r);
  FF_OWNER(f, r) = (unsigned short)id;
  p = &FF_SLOT(f, ci & 0xffff, sel & 0xffff);
  FF_NEXT(f, r) = *p & 0x3ff;
  *p = (*p & 0xfc00) | r;
  FF_TAG(f, id & 0xffff) = (sel & 0x3f) | (ci << 6);
  if (!rwp_farfield_00575ba0(e, &cell)) return f;
  FF_OWNER(f, r) |= 0x8000;
  w = *p;
  if ((w & 0x7c00) != 0x7c00) w += 0x400;
  *p = w;
  w = *p;
  if ((w & 0x7c00) <= 0xc00) return f;
  nc = ((ci << 6) & 0xffff0000) | FF_FREECELL(f);      /* `mov bx, word ptr` into the register that held ci << 6 for the tag: its high half stays (see above) */
  if ((unsigned short)nc == 0x3ff) return f;
  L = w & 0x3ff;
  hd = FF_HEAD(f, nc & 0xffff);
  FF_FREECELL(f) = (unsigned short)hd;
  *p = (unsigned short)nc;
  FF_HEAD(f, nc & 0xffff) = ((((sel & 0xffff) << 10) | (ci & 0xffff)) << 10) | 0x3ff;
  for (k = 0; k < 8; k++) FF_SLOT(f, nc & 0xffff, k) = 0x83ff;
  if ((unsigned short)L == 0x3ff) return f;
  cur = L;
  do {
    nxt = FF_NEXT(f, cur);
    w = FF_OWNER(f, cur);
    o = w & 0x3ff;
    if (w & 0x8000) {
      sel2 = (unsigned short)rwp_farfield_00575d00(&sub, FF_E(f, o), &cell);
      FF_NEXT(f, cur) = FF_SLOT(f, nc & 0xffff, sel2) & 0x3ff;
      rwp_farfield_00576b10(f, nc, sel2, cur);
      FF_TAG(f, o) = (sel2 & 0x3f) | (nc << 6);
      if (rwp_farfield_00575ba0(FF_E(f, o), &sub)) rwp_farfield_00576ba0(f, nc, sel2);
      else FF_OWNER(f, cur) &= 0x7fff;
    } else {
      rwp_farfield_00576b50(f, nc, cur);
      FF_TAG(f, o) = (nc << 6) | 0x3f;
    }
    cur = nxt;
  } while ((unsigned short)nxt != 0x3ff);
  return f;
}
