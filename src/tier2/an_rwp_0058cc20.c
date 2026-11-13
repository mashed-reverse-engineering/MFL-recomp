/* rwp_object_type_bounding_box (0x0058cc20), rwp_object_type_sync_matrix (0x0058cc30): the bounding-box and sync slots of the Rwp body object
   type record "rwpOBJTYPEBODY" (0x63a5d4: name, enable 0x58ca30, disable 0x58ca80, these two).  The original entries are 5 byte `jmp` stubs. */
#include "mfl_tier2.h"

#define rwp_object_shape_bounds FUN_0058cab0      /* tier 1 name of 0x58cab0: shape bounds at the object's matrix (0x56c350) */
/*0058cab0*/ extern int rwp_object_shape_bounds(void *o, void *box);
/*0058cad0*/ extern void *rwp_object_sync_matrix(void *o);

/* Bounding box of the object's shape at its matrix into box. */
/* TIER2: unvalidated 2026-11-13 authority=diffemu */
int rwp_object_type_bounding_box(void *o, void *box)
{
  return rwp_object_shape_bounds(o, box);
}

/* Copies the simulated matrix back to the object's matrix (rwp_object_sync_matrix); returns o. */
/* TIER2: unvalidated 2026-11-13 authority=diffemu */
void *rwp_object_type_sync_matrix(void *o)
{
  return rwp_object_sync_matrix(o);
}
