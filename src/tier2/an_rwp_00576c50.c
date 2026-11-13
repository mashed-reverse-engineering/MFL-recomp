/* rwp_farfield_00576c50 (0x00576c50): remove an object from the farfield spatial index.  T-P3b C (2026-11-02), deleted in 12fd789e when stock
   diffemu went INCONCLUSIVE; restored in W7 with the HS spec tools/diffemu_audit/tweak/specs/s_b3b.py (consistent random index). */
#include "mfl_tier2.h"

/* Farfield spatial index (a fixed pool of 0xc020 bytes): per object a tag at +0x8020 (cell << 6 | slot, slot 0x3f = the cell's own list); 4 byte list
   records at +0x881e (object id, flag, next); 20 byte cells at +0x981c (head list, parent cell and slot, 8 slot lists with a flag count); the free
   record head is at +0x981a, the free cell head at +0xc00e and the edge pool at +0xc010. */
typedef struct MflFfRec {
  unsigned short owner : 10;
  unsigned short pad : 5;
  unsigned short flag : 1;
  unsigned short next;
} MflFfRec;
typedef struct MflFfSlot {
  unsigned short idx : 10;
  unsigned short count : 5;
  unsigned short flag : 1;
} MflFfSlot;
typedef struct MflFfCell {
  unsigned int idx : 10;
  unsigned int parent : 10;
  unsigned int slot : 3;
  unsigned int pad : 9;
  MflFfSlot slots[8];
} MflFfCell;
typedef struct MflFfTag {
  unsigned short kind : 6;
  unsigned short cell : 10;
} MflFfTag;
#define FF_REC(f, i) (((MflFfRec *)((f) + 0x881e))[i])
#define FF_CELL(f, c) (((MflFfCell *)((f) + 0x981c))[c])

extern void rwp_capsule_00575950(void *pool, unsigned int v);

/* Removes object v: its edges are unlinked (575950), its record is taken out of the list of its cell (or of its slot list, adjusting the flag count)
   and pushed on the free record list; a cell whose head list is empty and whose 8 slots are all empty is released to the free cell list and
   cleared in its parent's slot. */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
void rwp_farfield_00576c50(unsigned char *f, unsigned int v)
{
  MflFfTag tag;
  unsigned short kind, cell, cur, local, *pp;
  MflFfCell *c;
  MflFfSlot *p;
  int i, n;

  rwp_capsule_00575950(*(void **)(f + 0xc010), v & 0xffff);
  tag = *(MflFfTag *)(f + 0x8020 + (v & 0xffff) * 2);
  kind = tag.kind;
  cell = tag.cell;
  if (kind == 0x3f || kind == 0xffff) {
    c = &FF_CELL(f, cell);
    local = c->idx;
    cur = local;
    pp = &local;
    while (FF_REC(f, cur).owner != (unsigned short)v && cur != 0x3ff) {
      pp = &FF_REC(f, cur).next;
      cur = *pp;
    }
    cur = *pp;
    if (pp == &local) c->idx = FF_REC(f, cur).next;
    else *pp = FF_REC(f, cur).next;
  } else {
    p = &FF_CELL(f, cell).slots[kind];
    local = p->idx;
    cur = local;
    pp = &local;
    while (FF_REC(f, cur).owner != (unsigned short)v && cur != 0x3ff) {
      pp = &FF_REC(f, cur).next;
      cur = *pp;
    }
    cur = *pp;
    if (pp == &local) p->idx = FF_REC(f, cur).next;
    else *pp = FF_REC(f, cur).next;
    if (FF_REC(f, cur).flag) {
      if (p->count != 0x1f) p->count--;
      else {
        n = 0;
        i = p->idx;
        while (i != 0x3ff && n < 0x1f) if (FF_REC(f, i).flag) n++;      /* as the original: the index is not advanced */
        p->count = n;
      }
    }
  }
  FF_REC(f, cur).next = *(unsigned short *)(f + 0x981a);
  *(unsigned short *)(f + 0x981a) = cur;
  if (cell != 0) {
    c = &FF_CELL(f, cell);
    if (c->idx == 0x3ff) {
      for (i = 0; i < 8; i++) {
        if (!c->slots[i].flag) return;
        if (c->slots[i].idx != 0x3ff) return;
      }
      FF_CELL(f, c->parent).slots[c->slot].idx = 0x3ff;
      FF_CELL(f, c->parent).slots[c->slot].count = 0;
      FF_CELL(f, c->parent).slots[c->slot].flag = 1;
      *(unsigned int *)c = *(unsigned short *)(f + 0xc00e);
      *(unsigned short *)(f + 0xc00e) = cell;
    }
  }
}
