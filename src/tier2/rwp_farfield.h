/* Farfield spatial index (Rwp broad phase): one fixed block of 0xc020 bytes.
     +0x0000  object boxes, 32 bytes each (max[3], pad, min[3], pad; the maximum corner comes first), 0x3ff entries; each box is the object's
              bounds grown by the margin at +0xc014.
     +0x7fe0  the world box (entry 0x3ff), the root cell's box.
     +0x8020  u16 tag per object: (cell << 6) | slot, slot 0x3f = the cell's own list, 0..7 = the octant list.
     +0x881e  list records, 4 bytes: owner (bits 0-9 object id, bit 15 = flag "straddles the octant centre"), next (0x3ff ends a list).
     +0x981a  free record head; +0xc00e free cell head; +0xc010 edge pool; +0xc018 pair filter.
     +0x981c  cells, 20 bytes: head dword (own list | parent cell << 10 | parent octant << 20), then 8 u16 octant slots
              (flag 0x8000 = leaf list | count << 10 (saturates at 0x1f) | list head; without the flag the low bits are a child cell). */
typedef struct MflFfBox {
  float max[3];
  float pad0;
  float min[3];
  float pad1;
} MflFfBox;
#define FF_E(f, i) (((MflFfBox *)(f)) + (i))
#define FF_MARGIN(f) (*(float *)((f) + 0xc014))
#define FF_TAG(f, i) (*(unsigned short *)((f) + 0x8020 + (i) * 2))
#define FF_OWNER(f, r) (*(unsigned short *)((f) + 0x881e + (r) * 4))
#define FF_NEXT(f, r) (*(unsigned short *)((f) + 0x8820 + (r) * 4))
#define FF_FREEREC(f) (*(unsigned short *)((f) + 0x981a))
#define FF_FREECELL(f) (*(unsigned short *)((f) + 0xc00e))
#define FF_HEAD(f, c) (*(unsigned int *)((f) + 0x981c + (c) * 20))
#define FF_SLOT(f, c, k) (*(unsigned short *)((f) + 0x9820 + ((c) * 10 + (k)) * 2))

extern int rwp_farfield_00575d00(MflFfBox *out, MflFfBox *b, MflFfBox *a);
extern int rwp_farfield_00575ba0(MflFfBox *a1, MflFfBox *a2);
extern void rwp_farfield_00576b10(unsigned char *f, unsigned short cell, unsigned short slot, int bits);
extern void rwp_farfield_00576b50(unsigned char *f, unsigned short cell, unsigned short slot);
extern void rwp_farfield_00576ba0(unsigned char *f, unsigned short cell, unsigned short slot);
extern void rwp_farfield_report_overlaps(unsigned char *f, MflFfBox *b, unsigned int id);
