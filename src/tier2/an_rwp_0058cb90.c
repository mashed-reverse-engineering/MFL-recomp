/* rwp_body_create (0x0058cb90), rwp_body_destroy (0x0058cbe0): the shared, reference counted collision body (0x54 bytes) */
#include "mfl_tier2.h"

#define RWP_MEM_HINT 0x30900                /* allocation hint used by all Rwp objects */
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*00575b20*/ extern void *rwp_capsule_00575b20(int size, int align, unsigned int hint);
/*0056be30*/ extern void rwp_sphere_0056be30(void *body, void *shape);
/*0058c840*/ extern void rwp_null_0058c840(void *shape);
/*0056bf00*/ extern void rwp_sphere_0056bf00(void *body);
/*00575b80*/ extern void rwp_capsule_00575b80(void *shape);

typedef struct MflRwpBody {
  unsigned char pad00[0x4c];
  void *shape;   /* 0x4c */
  int refs;   /* 0x50 */
} MflRwpBody;

/* Allocates a body with one reference and an empty (null) shape of 0x60 bytes; returns it (0 when out of memory). */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
MflRwpBody *rwp_body_create(void)
{
  MflRwpBody *b = (MflRwpBody *)RwMalloc(sizeof(MflRwpBody), RWP_MEM_HINT);

  if (b) {
    b->refs = 1;
    b->shape = rwp_capsule_00575b20(0x60, 0x10, RWP_MEM_HINT);
    rwp_sphere_0056be30(b, b->shape);
    rwp_null_0058c840(b->shape);
  }
  return b;
}

/* Drops one reference; the last one releases the body and its shape and frees the body.  Returns TRUE. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwp_body_destroy(MflRwpBody *b)
{
  if (--b->refs == 0) {
    rwp_sphere_0056bf00(b);
    rwp_capsule_00575b80(b->shape);
  }
  if (b->refs == 0) RwFree(b);
  return 1;
}
