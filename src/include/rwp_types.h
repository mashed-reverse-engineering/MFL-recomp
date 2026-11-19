/* Shared record layouts of the Rwp collision block (reconstructed from the code; unnamed bytes are padding). */
#ifndef RWP_TYPES_H
#define RWP_TYPES_H

/* Contact plane written by the shape query (rwp_aggregate_00586f20): unit normal and its distance. */
typedef struct MflPlane {
  float n[3];   /* 0x0 */
  float d;      /* 0xc */
} MflPlane;

/* One of the two shapes of a contact pair (0x80 bytes, only the fields used so far). */
typedef struct MflShape {
  MflPlane plane;   /* 0x0: filled by the shape query */
  unsigned char pad10[0x40];
  float *points;   /* 0x50: vertices, 16 bytes each (xyz first) */
  unsigned short count;   /* 0x54: 1 = point, 2 = edge, more = polygon */
  unsigned short mask;   /* 0x56 */
  int v58;   /* 0x58 */
  int v5c;   /* 0x5c */
  float normal[3];   /* 0x60 */
  int v6c;   /* 0x6c */
  float lo0;   /* 0x70 */
  float lo1;   /* 0x74 */
  float radius;   /* 0x78 */
  unsigned char flags;   /* 0x7c: bit 5 = no plane output, bit 6 = oriented by the filter word */
  unsigned char pad7d;
  unsigned short filter;   /* 0x7e */
} MflShape;

typedef struct MflShapePair {
  MflShape a;
  MflShape b;   /* 0x80 */
  float pos[3];   /* 0x100: query direction (unit) */
  int v10c;   /* 0x10c */
  float margin;   /* 0x110 */
} MflShapePair;

#endif
