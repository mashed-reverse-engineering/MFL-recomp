/* rwp_object_enable (0x0058ca30), rwp_object_disable (0x0058ca80): add an Rwp object handle to its simulation space and take it out again */
#include "mfl_tier2.h"

/*007daa1c*/ extern int g_data_7daa1c;
/*0056b600*/ extern void *rwp_volume_0056b600(void *space, void *body, void *matrix);
/*0056b630*/ extern void rwp_volume_0056b630(void *sim);

typedef struct MflRwpClass {
  unsigned char pad00[8];
  unsigned int max;   /* 0x8 */
  unsigned int count;   /* 0xc */
} MflRwpClass;

typedef struct MflRwpState {
  void *shape;   /* 0x0 */
  void *sim;   /* 0x4: the body in the simulation space, 0 while disabled */
  void *matrix;   /* 0x8 */
} MflRwpState;

typedef struct MflRwpObject {
  MflRwpClass *cls;   /* 0x0 */
  void *body;   /* 0x4 */
  unsigned char pad08[8];
  MflRwpState *state;   /* 0x10 */
  unsigned char pad14[0x10];
  void *space;   /* 0x24 */
} MflRwpObject;

/* While the class has room for one more live object, the body is added to the space (rwp_volume_0056b600) and counted.  Returns o when the
   space gave a simulation body, else 0. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
MflRwpObject *rwp_object_enable(MflRwpObject *o)
{
  void *sim = 0;
  void *space = o->space;

  if (o->cls->count < o->cls->max) {
    sim = rwp_volume_0056b600(space, o->body, o->state->matrix);
    o->state->sim = sim;
    o->cls->count++;
    g_data_7daa1c = 0;
  }
  return sim ? o : 0;
}

/* Removes the object's body from the simulation space and uncounts it; returns o. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
MflRwpObject *rwp_object_disable(MflRwpObject *o)
{
  rwp_volume_0056b630(o->state->sim);
  o->state->sim = 0;
  o->cls->count--;
  return o;
}
