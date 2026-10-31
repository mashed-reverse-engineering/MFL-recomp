#include "mfl_tier2.h"

#define RWP_MEM_HINT 0x30900                /* allocation hint used by all Rwp objects */
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

/* A convex hull block allocated in one piece: header, then the vertex (12 bytes each), vertex-plane (2), face (12 + 2) and edge tables, all
   16 bit offsets from the block start. */
typedef struct MflHullBlock {
  unsigned int size;   /* 0x0 */
  char *inline_part;   /* 0x4: points at the fixed part at 0x8c */
  char *verts;   /* 0x8 */
  char *vert_planes;   /* 0xc */
  char *faces;   /* 0x10 */
  char *face_planes;   /* 0x14 */
  char *edges;   /* 0x18 */
  short vert_count;   /* 0x1c */
  short face_count;   /* 0x1e */
  short edge_count;   /* 0x20 */
  unsigned char pad22[0x62];
  int ready;   /* 0x84 */
  int pad88;   /* 0x88 */
} MflHullBlock;

/* Allocates a hull block for nverts vertices, nedges edges and nfaces faces and fills in its header (the table offsets are
   rounded up to 16, 2 and 4 byte boundaries), ready = 1.  Returns the block. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
MflHullBlock *rwp_capsule_00575230(int nverts, int nedges, int nfaces)
{
  int o1, o2, o3, o4;
  volatile int o5, total;
  MflHullBlock *b;

  o1 = (nverts * 12 + 0x9b) & 0xfff0;
  o2 = (o1 + nverts * 2 + 0x11) & 0xfff0;
  o3 = (o2 + nfaces * 12 + 1) & 0xfffe;
  o4 = (o3 + nfaces * 2 + 5) & 0xfffc;
  o5 = (o4 + nedges * 12 + 1) & 0xfffe;
  total = (o5 + nedges * 2) & 0xffff;
  b = (MflHullBlock *)RwMalloc(total, RWP_MEM_HINT);
  b->inline_part = (char *)b + 0x8c;
  b->size = total;
  b->vert_planes = (char *)b + (o2 & 0xffff);
  b->verts = (char *)b + (o1 & 0xffff);
  b->edges = (char *)b + (o5 & 0xffff);
  b->vert_count = nverts;
  b->face_planes = (char *)b + (o4 & 0xffff);
  b->faces = (char *)b + (o3 & 0xffff);
  b->edge_count = nedges;
  b->face_count = nfaces;
  b->ready = 1;
  b->pad88 = 0;
  return b;
}
