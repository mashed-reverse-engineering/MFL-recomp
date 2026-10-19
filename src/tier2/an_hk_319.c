#include "mfl_tier2.h"
/*0048e720*/ extern undefined __cdecl rwp_object_set_matrix(int p0, RwMatrix *p1);
/*004c5360*/ extern void atomic_set_render_flag(RpAtomic * p0, int p1);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*006fab78*/ extern undefined4 g_rw_6fab78;
#define RWP_OBJECT_MAX (200)
#define PARKED_Y (2500.0)
/* rwp_object_set_active */

/* "(index, active): RwpObject enable; an inactive object is parked at y = 2500" */

void rwp_object_set_active(int index,int active)

{
  RwV3d parked_pos;
  RwMatrix matrix;

  /* valid slot index that holds a live atomic: set its render flag */
  if (((index >= 0) && (index < RWP_OBJECT_MAX)) && ((&g_rw_6fab78)[index] != 0)) {
    parked_pos.x = 0.0;
    parked_pos.y = PARKED_Y;
    parked_pos.z = 0.0;
    atomic_set_render_flag((&g_rw_6fab78)[index],active);
    /* inactive: build an identity matrix and move it to the parked position,
       then hand it to the object (0x20003 flag bits: inferred, matrix type/dirty) */
    if (active == 0) {
      matrix.flags = matrix.flags | 0x20003;
      matrix.at.z = 1.0;
      matrix.up.y = 1.0;
      matrix.right.x = 1.0;
      matrix.up.x = 0.0;
      matrix.right.z = 0.0;
      matrix.right.y = 0.0;
      matrix.at.y = 0.0;
      matrix.at.x = 0.0;
      matrix.up.z = 0.0;
      matrix.pos.z = 0.0;
      matrix.pos.y = 0.0;
      matrix.pos.x = 0.0;
      RwMatrixTranslate(&matrix,&parked_pos,rwCOMBINEREPLACE);
      rwp_object_set_matrix(index,&matrix);
    }
  }
  return;
}


