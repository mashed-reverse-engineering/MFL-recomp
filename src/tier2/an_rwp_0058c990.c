/* rwp_object_create (0x0058c990): create the Rwp object handle (0x34 bytes) of a shared collision body */
#include "mfl_tier2.h"

#define RWP_MEM_HINT 0x30900                /* allocation hint used by all Rwp objects */
#define RWP_OBJECT_KIND_BODY 0xb
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*0063a5d4*/ extern char g_data_63a5d4[];   /* class record of this object kind */
/*0056e490*/ extern void rwp_obj_0056e490(void *obj, int a, int b);
/*0058cbe0*/ extern int rwp_body_destroy(void *body);

typedef struct MflRwpBody {
  unsigned char pad00[0x4c];
  void *shape;   /* 0x4c */
  int refs;   /* 0x50 */
} MflRwpBody;

typedef struct MflRwpState {
  void *shape;   /* 0x0 */
} MflRwpState;

typedef struct MflRwpObject {
  void *cls;   /* 0x0 */
  MflRwpBody *body;   /* 0x4 */
  int kind;   /* 0x8 */
  short active;   /* 0xc */
  short pad0e;
  MflRwpState *state;   /* 0x10 */
  unsigned char pad14[0x1c];
  void *owner;   /* 0x30 */
} MflRwpObject;

/* Allocates the handle, initialises it (rwp_obj_0056e490), takes a reference on body (body must not be 0: its shape is read) and makes the
   body's shape the shape of the handle's state.  Returns the handle. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
MflRwpObject *rwp_object_create(MflRwpBody *body)
{
  MflRwpObject *o = (MflRwpObject *)RwMalloc(sizeof(MflRwpObject),RWP_MEM_HINT);

  o->cls = g_data_63a5d4;
  rwp_obj_0056e490(o, 1, 0);
  if (o->body) rwp_body_destroy(o->body);
  o->body = body;
  if (body) body->refs++;
  o->active = 1;
  o->kind = RWP_OBJECT_KIND_BODY;
  o->state->shape = body->shape;
  o->owner = 0;
  return o;
}
